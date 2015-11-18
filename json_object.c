/*
 * $Id: json_object.c,v 1.17 2006/07/25 03:24:50 mclark Exp $
 *
 * Copyright (c) 2004, 2005 Metaparadigm Pte. Ltd.
 * Michael Clark <michael@metaparadigm.com>
 * Copyright (c) 2009 Hewlett-Packard Development Company, L.P.
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See COPYING for details.
 *
 */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <math.h>
#include <errno.h>

#include "debug.h"
#include "printbuf.h"
#include "linkhash.h"
#include "arraylist.h"
#include "json_inttypes.h"
#include "json_object.h"
#include "json_object_private.h"
#include "json_util.h"
#include "math_compat.h"

#if !defined(HAVE_STRDUP) && defined(_MSC_VER)
  /* MSC has the version as _strdup */
# define strdup _strdup
#elif !defined(HAVE_STRDUP)
# error You do not have strdup on your system.
#endif /* HAVE_STRDUP */

#if !defined(HAVE_SNPRINTF) && defined(_MSC_VER)
  /* MSC has the version as _snprintf */
# define snprintf _snprintf
#elif !defined(HAVE_SNPRINTF)
# error You do not have snprintf on your system.
#endif /* HAVE_SNPRINTF */

// Don't define this.  It's not thread-safe.
/* #define REFCOUNT_DEBUG 1 */

const char *json_number_chars = "0123456789.+-eE";
const char *json_hex_chars = "0123456789abcdefABCDEF";

static void json_object_generic_delete(struct json_object* jso);
static struct json_object* json_object_new(enum json_type o_type);

static json_object_to_json_string_fn json_object_object_to_json_string;
static json_object_to_json_string_fn json_object_boolean_to_json_string;
static json_object_to_json_string_fn json_object_int_to_json_string;
static json_object_to_json_string_fn json_object_double_to_json_string;
static json_object_to_json_string_fn json_object_string_to_json_string;
static json_object_to_json_string_fn json_object_array_to_json_string;


/* ref count debugging */

#ifdef REFCOUNT_DEBUG

static struct lh_table *json_object_table;

static void json_object_init(void) __attribute__ ((constructor));
static void json_object_init(void) {
	MC_DEBUG("json_object_init: creating object table\n");
	json_object_table = lh_kptr_table_new(128, NULL);
}

static void json_object_fini(void) __attribute__ ((destructor));
static void json_object_fini(void)
{
	struct lh_entry *ent;
	if (MC_GET_DEBUG())
	{
		if (json_object_table->count)
		{
			MC_DEBUG("json_object_fini: %d referenced objects at exit\n",
			   json_object_table->count);
			lh_foreach(json_object_table, ent)
			{
				struct json_object* obj = (struct json_object*)ent->v;
				MC_DEBUG("\t%s:%p\n", json_type_to_name(obj->o_type), obj);
			}
		}
	}
	MC_DEBUG("json_object_fini: freeing object table\n");
	lh_table_free(json_object_table);
}
#endif /* REFCOUNT_DEBUG */


/* helper for accessing the optimized string data component in json_object
 */
static const char *
get_string_component(struct json_object *jso)
{
	return (jso->o.c_string.len < LEN_DIRECT_STRING_DATA) ?
		   jso->o.c_string.str.data : jso->o.c_string.str.ptr;
}

/* string escaping
 *
 * String escaping is a surprisingly performance intense operation.
 * I spent many hours in the profiler, and the root problem seems
 * to be that there is no easy way to detect the character classes
 * that need to be escaped, where the root cause is that these
 * characters are spread all over the ascii table. I tried
 * several approaches, including call tables, re-structuring
 * the case condition, different types of if conditions and
 * reordering the if conditions. What worked out best is this:
 * The regular case is that a character must not be escaped. So
 * we want to process that as fast as possible. In order to
 * detect this as quickly as possible, we have a lookup table
 * that tells us if a char needs escaping ("needsEscape", below).
 * This table has a spot for each ascii code. Note that it uses
 * chars, because anything larger causes worse cache operation
 * and anything smaller requires bit indexing and masking
 * operations, which are also comparatively costly. So plain
 * chars work best. What we then do is a single lookup into the
 * table to detect if we need to escape a character. If we need,
 * we go into the depth of actual escape detection. But if we
 * do NOT need to escape, we just quickly advance the index
 * and are done with that char. Note that it may look like the
 * extra table lookup costs performance, but right the contrary
 * is the case. We get amore than 30% performance increase due
 * to it (compared to the latest version of the code that did not
 * do the lookups.
 * rgerhards@adiscon.com, 2015-11-18
 */

static char needsEscape[256] = {
	1, 1, 1, 1, 1, 1, 1, 1, /* ascii codes 0 .. 7 */
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
	0, 0, 1, 0, 0, 0, 0, 0, /* ascii codes 32 .. 39 */
	0, 0, 0, 0, 0, 0, 0, 1,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 1, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0
};

static void json_escape_str(struct printbuf *pb, const char *str, int len)
{
	int pos = 0, start_offset = 0;
	unsigned char c;
	while (pos < len)
	{
		c = str[pos];
		if(needsEscape[c]) {
			if(pos - start_offset > 0)
				printbuf_memappend_no_nul(pb, str + start_offset, pos - start_offset);
			switch(c)
			{
			case '\b': printbuf_memappend_no_nul(pb, "\\b", 2);
				break;
			case '\n': printbuf_memappend_no_nul(pb, "\\n", 2);
				break;
			case '\r': printbuf_memappend_no_nul(pb, "\\r", 2);
				break;
			case '\t': printbuf_memappend_no_nul(pb, "\\t", 2);
				break;
			case '\f': printbuf_memappend_no_nul(pb, "\\f", 2);
				break;
			case '"': printbuf_memappend_no_nul(pb, "\\\"", 2);
				break;
			case '\\': printbuf_memappend_no_nul(pb, "\\\\", 2);
				break;
			case '/': printbuf_memappend_no_nul(pb, "\\/", 2);
				break;
			default: sprintbuf(pb, "\\u00%c%c",
				json_hex_chars[c >> 4],
				json_hex_chars[c & 0xf]);
				break;
			}
			start_offset = ++pos;
		} else
			pos++;
	}
	if (pos - start_offset > 0)
		printbuf_memappend_no_nul(pb, str + start_offset, pos - start_offset);
}


/* reference counting */

extern struct json_object* json_object_get(struct json_object *jso)
{
	if (jso)
		jso->_ref_count++;
	return jso;
}

int json_object_put(struct json_object *jso)
{
	if(jso)
	{
		jso->_ref_count--;
		if(!jso->_ref_count)
		{
			if (jso->_user_delete)
				jso->_user_delete(jso, jso->_userdata);
			jso->_delete(jso);
			return 1;
		}
	}
	return 0;
}


/* generic object construction and destruction parts */

static void json_object_generic_delete(struct json_object* jso)
{
#ifdef REFCOUNT_DEBUG
	MC_DEBUG("json_object_delete_%s: %p\n",
	   json_type_to_name(jso->o_type), jso);
	lh_table_delete(json_object_table, jso);
#endif /* REFCOUNT_DEBUG */
	printbuf_free(jso->_pb);
	free(jso);
}

static struct json_object* json_object_new(enum json_type o_type)
{
	struct json_object *jso;

	jso = (struct json_object*)calloc(sizeof(struct json_object), 1);
	if (!jso)
		return NULL;
	jso->o_type = o_type;
	jso->_ref_count = 1;
	jso->_delete = &json_object_generic_delete;
#ifdef REFCOUNT_DEBUG
	lh_table_insert(json_object_table, jso, jso);
	MC_DEBUG("json_object_new_%s: %p\n", json_type_to_name(jso->o_type), jso);
#endif /* REFCOUNT_DEBUG */
	return jso;
}


/* type checking functions */

int json_object_is_type(struct json_object *jso, enum json_type type)
{
	if (!jso)
		return (type == json_type_null);
	return (jso->o_type == type);
}

enum json_type json_object_get_type(struct json_object *jso)
{
	if (!jso)
		return json_type_null;
	return jso->o_type;
}

/* set a custom conversion to string */

void json_object_set_serializer(json_object *jso,
	json_object_to_json_string_fn to_string_func,
	void *userdata,
	json_object_delete_fn *user_delete)
{
	// First, clean up any previously existing user info
	if (jso->_user_delete)
	{
		jso->_user_delete(jso, jso->_userdata);
	}
	jso->_userdata = NULL;
	jso->_user_delete = NULL;

	if (to_string_func == NULL)
	{
		// Reset to the standard serialization function
		switch(jso->o_type)
		{
		case json_type_null:
			jso->_to_json_string = NULL;
			break;
		case json_type_boolean:
			jso->_to_json_string = &json_object_boolean_to_json_string;
			break;
		case json_type_double:
			jso->_to_json_string = &json_object_double_to_json_string;
			break;
		case json_type_int:
			jso->_to_json_string = &json_object_int_to_json_string;
			break;
		case json_type_object:
			jso->_to_json_string = &json_object_object_to_json_string;
			break;
		case json_type_array:
			jso->_to_json_string = &json_object_array_to_json_string;
			break;
		case json_type_string:
			jso->_to_json_string = &json_object_string_to_json_string;
			break;
		}
		return;
	}

	jso->_to_json_string = to_string_func;
	jso->_userdata = userdata;
	jso->_user_delete = user_delete;
}


/* extended conversion to string */

const char* json_object_to_json_string_ext(struct json_object *jso, int flags)
{
	if (!jso)
		return "null";

	if ((!jso->_pb) && !(jso->_pb = printbuf_new()))
		return NULL;

	printbuf_reset(jso->_pb);

	jso->_to_json_string(jso, jso->_pb, 0, flags);

	printbuf_terminate_string(jso->_pb);
	return jso->_pb->buf;
}

/* backwards-compatible conversion to string */

const char* json_object_to_json_string(struct json_object *jso)
{
	return json_object_to_json_string_ext(jso, JSON_C_TO_STRING_SPACED);
}

static void indent(struct printbuf *pb, int level, int flags)
{
	if (flags & JSON_C_TO_STRING_PRETTY)
	{
		if (flags & JSON_C_TO_STRING_PRETTY_TAB)
		{
			printbuf_memset(pb, -1, '\t', level);
		}
		else
		{
			printbuf_memset(pb, -1, ' ', level * 2);
		}
	}
}

/* json_object_object */

static int json_object_object_to_json_string(struct json_object* jso,
					     struct printbuf *pb,
					     int level,
						 int flags)
{
	int had_children = 0;
	struct json_object_iter iter;

	printbuf_memappend_no_nul(pb, "{" /*}*/, 1);
	if (flags & JSON_C_TO_STRING_PRETTY)
		printbuf_memappend_no_nul(pb, "\n", 1);
	json_object_object_foreachC(jso, iter)
	{
		if (had_children)
		{
			printbuf_memappend_no_nul(pb, ",", 1);
			if (flags & JSON_C_TO_STRING_PRETTY)
				printbuf_memappend_no_nul(pb, "\n", 1);
		}
		had_children = 1;
		if (flags & JSON_C_TO_STRING_SPACED)
			printbuf_memappend_no_nul(pb, " ", 1);
		indent(pb, level+1, flags);
		printbuf_memappend_no_nul(pb, "\"", 1);
		json_escape_str(pb, iter.key, strlen(iter.key));
		if (flags & JSON_C_TO_STRING_SPACED)
			printbuf_memappend_no_nul(pb, "\": ", 3);
		else
			printbuf_memappend_no_nul(pb, "\":", 2);
		if(iter.val == NULL)
			printbuf_memappend_no_nul(pb, "null", 4);
		else
			iter.val->_to_json_string(iter.val, pb, level+1,flags);
	}
	if (flags & JSON_C_TO_STRING_PRETTY)
	{
		if (had_children)
			printbuf_memappend_no_nul(pb, "\n",1);
		indent(pb,level,flags);
	}
	if (flags & JSON_C_TO_STRING_SPACED)
		printbuf_memappend_no_nul(pb, /*{*/ " }", 2);
	else
		printbuf_memappend_no_nul(pb, /*{*/ "}", 1);
	return 0; /* we need to keep compatible with the API */
}


static void json_object_lh_entry_free(struct lh_entry *ent)
{
	if (!ent->k_is_constant)
		free(ent->k);
	json_object_put((struct json_object*)ent->v);
}

static void json_object_object_delete(struct json_object* jso)
{
	lh_table_free(jso->o.c_object);
	json_object_generic_delete(jso);
}

struct json_object* json_object_new_object(void)
{
	struct json_object *jso = json_object_new(json_type_object);
	if (!jso)
		return NULL;
	jso->_delete = &json_object_object_delete;
	jso->_to_json_string = &json_object_object_to_json_string;
	jso->o.c_object = lh_kchar_table_new(JSON_OBJECT_DEF_HASH_ENTRIES,
					&json_object_lh_entry_free);
	if (!jso->o.c_object)
	{
		json_object_generic_delete(jso);
		errno = ENOMEM;
		return NULL;
	}
	return jso;
}

struct lh_table* json_object_get_object(struct json_object *jso)
{
	if (!jso)
		return NULL;
	switch(jso->o_type)
	{
	case json_type_object:
		return jso->o.c_object;
	default:
		return NULL;
	}
}

void json_object_object_add_ex(struct json_object* jso,
	const char *const key,
	struct json_object *const val,
	const unsigned opts)
{
	// We lookup the entry and replace the value, rather than just deleting
	// and re-adding it, so the existing key remains valid.
	json_object *existing_value = NULL;
	struct lh_entry *existing_entry;
	const unsigned long hash = lh_get_hash(jso->o.c_object, (void*)key);
	existing_entry = (opts & JSON_C_OBJECT_ADD_KEY_IS_NEW) ? NULL : 
			      lh_table_lookup_entry_w_hash(jso->o.c_object, (void*)key, hash);
	if (!existing_entry)
	{
		void *const k = (opts & JSON_C_OBJECT_KEY_IS_CONSTANT) ?
					(void*)key : strdup(key);
		lh_table_insert_w_hash(jso->o.c_object, k, val, hash, opts);
		return;
	}
	existing_value = (json_object *)existing_entry->v;
	if (existing_value)
		json_object_put(existing_value);
	existing_entry->v = val;
}

void json_object_object_add(struct json_object* jso, const char *key,
			    struct json_object *val)
{
	// We lookup the entry and replace the value, rather than just deleting
	// and re-adding it, so the existing key remains valid.
	json_object *existing_value = NULL;
	struct lh_entry *existing_entry;
	const unsigned long hash = lh_get_hash(jso->o.c_object, (void*)key);
	existing_entry = lh_table_lookup_entry_w_hash(jso->o.c_object, (void*)key, hash);
	if (!existing_entry)
	{
		lh_table_insert_w_hash(jso->o.c_object, strdup(key), val, hash, 0);
		return;
	}
	existing_value = (json_object  *)existing_entry->v;
	if (existing_value)
		json_object_put(existing_value);
	existing_entry->v = val;
}


int json_object_object_length(struct json_object *jso)
{
	return lh_table_length(jso->o.c_object);
}

struct json_object* json_object_object_get(struct json_object* jso, const char *key)
{
	struct json_object *result = NULL;
	json_object_object_get_ex(jso, key, &result);
	return result;
}

json_bool json_object_object_get_ex(struct json_object* jso, const char *key, struct json_object **value)
{
	if (value != NULL)
		*value = NULL;

	if (NULL == jso)
		return FALSE;

	switch(jso->o_type)
	{
	case json_type_object:
		return lh_table_lookup_ex(jso->o.c_object, (void*)key, (void**)value);
	default:
		if (value != NULL)
			*value = NULL;
		return FALSE;
	}
}

void json_object_object_del(struct json_object* jso, const char *key)
{
	lh_table_delete(jso->o.c_object, key);
}


/* json_object_boolean */

static int json_object_boolean_to_json_string(struct json_object* jso,
					      struct printbuf *pb,
					      int level,
						  int flags)
{
	if (jso->o.c_boolean)
		printbuf_memappend_no_nul(pb, "true", 4);
	else
		printbuf_memappend_no_nul(pb, "false", 5);
	return 0; /* we need to keep compatible with the API */
}

struct json_object* json_object_new_boolean(json_bool b)
{
	struct json_object *jso = json_object_new(json_type_boolean);
	if (!jso)
		return NULL;
	jso->_to_json_string = &json_object_boolean_to_json_string;
	jso->o.c_boolean = b;
	return jso;
}

json_bool json_object_get_boolean(struct json_object *jso)
{
	if (!jso)
		return FALSE;
	switch(jso->o_type)
	{
	case json_type_boolean:
		return jso->o.c_boolean;
	case json_type_int:
		return (jso->o.c_int64 != 0);
	case json_type_double:
		return (jso->o.c_double != 0);
	case json_type_string:
		return (jso->o.c_string.len != 0);
	default:
		return FALSE;
	}
}


/* json_object_int */

static int json_object_int_to_json_string(struct json_object* jso,
					  struct printbuf *pb,
					  int level,
					  int flags)
{
	sprintbuf(pb, "%" PRId64, jso->o.c_int64);
	return 0; /* we need to keep compatible with the API */
}

struct json_object* json_object_new_int(int32_t i)
{
	struct json_object *jso = json_object_new(json_type_int);
	if (!jso)
		return NULL;
	jso->_to_json_string = &json_object_int_to_json_string;
	jso->o.c_int64 = i;
	return jso;
}

int32_t json_object_get_int(struct json_object *jso)
{
  int64_t cint64;
  enum json_type o_type;

  if(!jso) return 0;

  o_type = jso->o_type;
  cint64 = jso->o.c_int64;

  if (o_type == json_type_string)
  {
	/*
	 * Parse strings into 64-bit numbers, then use the
	 * 64-to-32-bit number handling below.
	 */
	if (json_parse_int64(get_string_component(jso), &cint64) != 0)
		return 0; /* whoops, it didn't work. */
	o_type = json_type_int;
  }

  switch(o_type) {
  case json_type_int:
	/* Make sure we return the correct values for out of range numbers. */
	if (cint64 <= INT32_MIN)
		return INT32_MIN;
	else if (cint64 >= INT32_MAX)
		return INT32_MAX;
	else
		return (int32_t)cint64;
  case json_type_double:
    return (int32_t)jso->o.c_double;
  case json_type_boolean:
    return jso->o.c_boolean;
  default:
    return 0;
  }
}

struct json_object* json_object_new_int64(int64_t i)
{
	struct json_object *jso = json_object_new(json_type_int);
	if (!jso)
		return NULL;
	jso->_to_json_string = &json_object_int_to_json_string;
	jso->o.c_int64 = i;
	return jso;
}

int64_t json_object_get_int64(struct json_object *jso)
{
	int64_t cint;

	if (!jso)
		return 0;
	switch(jso->o_type)
	{
	case json_type_int:
		return jso->o.c_int64;
	case json_type_double:
		return (int64_t)jso->o.c_double;
	case json_type_boolean:
		return jso->o.c_boolean;
	case json_type_string:
		if (json_parse_int64(get_string_component(jso), &cint) == 0)
			return cint;
	default:
		return 0;
	}
}


/* json_object_double */

static int json_object_double_to_json_string(struct json_object* jso,
					     struct printbuf *pb,
					     int level,
						 int flags)
{
  char buf[128], *p, *q;
  int size;
  /* Although JSON RFC does not support
     NaN or Infinity as numeric values
     ECMA 262 section 9.8.1 defines
     how to handle these cases as strings */
  if(isnan(jso->o.c_double))
    size = snprintf(buf, sizeof(buf), "NaN");
  else if(isinf(jso->o.c_double))
    if(jso->o.c_double > 0)
      size = snprintf(buf, sizeof(buf), "Infinity");
    else
      size = snprintf(buf, sizeof(buf), "-Infinity");
  else
    size = snprintf(buf, sizeof(buf), "%.17g", jso->o.c_double);

  p = strchr(buf, ',');
  if (p) {
    *p = '.';
  } else {
    p = strchr(buf, '.');
  }
  if (p && (flags & JSON_C_TO_STRING_NOZERO)) {
    /* last useful digit, always keep 1 zero */
    p++;
    for (q=p ; *q ; q++) {
      if (*q!='0') p=q;
    }
    /* drop trailing zeroes */
    *(++p) = 0;
    size = p-buf;
  }
  printbuf_memappend_no_nul(pb, buf, size);
  return 0; /* we need to keep compatible with the API */
}

struct json_object* json_object_new_double(double d)
{
	struct json_object *jso = json_object_new(json_type_double);
	if (!jso)
		return NULL;
	jso->_to_json_string = &json_object_double_to_json_string;
	jso->o.c_double = d;
	return jso;
}

struct json_object* json_object_new_double_s(double d, const char *ds)
{
	struct json_object *jso = json_object_new_double(d);
	if (!jso)
		return NULL;

	char *new_ds = strdup(ds);
	if (!new_ds)
	{
		json_object_generic_delete(jso);
		errno = ENOMEM;
		return NULL;
	}
	json_object_set_serializer(jso, json_object_userdata_to_json_string,
	    new_ds, json_object_free_userdata);
	return jso;
}

int json_object_userdata_to_json_string(struct json_object *jso,
	struct printbuf *pb, int level, int flags)
{
	int userdata_len = strlen((const char *)jso->_userdata);
	printbuf_memappend_no_nul(pb, (const char *)jso->_userdata, userdata_len);
	return 0; /* we need to keep compatible with the API */
}

void json_object_free_userdata(struct json_object *jso, void *userdata)
{
	free(userdata);
}

double json_object_get_double(struct json_object *jso)
{
  double cdouble;
  char *errPtr = NULL;

  if(!jso) return 0.0;
  switch(jso->o_type) {
  case json_type_double:
    return jso->o.c_double;
  case json_type_int:
    return jso->o.c_int64;
  case json_type_boolean:
    return jso->o.c_boolean;
  case json_type_string:
    errno = 0;
    cdouble = strtod(get_string_component(jso), &errPtr);

    /* if conversion stopped at the first character, return 0.0 */
    if (errPtr == get_string_component(jso))
        return 0.0;

    /*
     * Check that the conversion terminated on something sensible
     *
     * For example, { "pay" : 123AB } would parse as 123.
     */
    if (*errPtr != '\0')
        return 0.0;

    /*
     * If strtod encounters a string which would exceed the
     * capacity of a double, it returns +/- HUGE_VAL and sets
     * errno to ERANGE. But +/- HUGE_VAL is also a valid result
     * from a conversion, so we need to check errno.
     *
     * Underflow also sets errno to ERANGE, but it returns 0 in
     * that case, which is what we will return anyway.
     *
     * See CERT guideline ERR30-C
     */
    if ((HUGE_VAL == cdouble || -HUGE_VAL == cdouble) &&
        (ERANGE == errno))
            cdouble = 0.0;
    return cdouble;
  default:
    return 0.0;
  }
}


/* json_object_string */

static int json_object_string_to_json_string(struct json_object* jso,
					     struct printbuf *pb,
					     int level,
						 int flags)
{
	printbuf_memappend_no_nul(pb, "\"", 1);
	json_escape_str(pb, get_string_component(jso), jso->o.c_string.len);
	printbuf_memappend_no_nul(pb, "\"", 1);
	return 0; /* we need to keep compatible with the API */
}

static void json_object_string_delete(struct json_object* jso)
{
	if(jso->o.c_string.len >= LEN_DIRECT_STRING_DATA)
		free(jso->o.c_string.str.ptr);
	json_object_generic_delete(jso);
}

struct json_object* json_object_new_string(const char *s)
{
	struct json_object *jso = json_object_new(json_type_string);
	if (!jso)
		return NULL;
	jso->_delete = &json_object_string_delete;
	jso->_to_json_string = &json_object_string_to_json_string;
	jso->o.c_string.len = strlen(s);
	if(jso->o.c_string.len < LEN_DIRECT_STRING_DATA) {
		memcpy(jso->o.c_string.str.data, s, jso->o.c_string.len);
	} else {
		jso->o.c_string.str.ptr = strdup(s);
		if (!jso->o.c_string.str.ptr)
		{
			json_object_generic_delete(jso);
			errno = ENOMEM;
			return NULL;
		}
	}
	return jso;
}

struct json_object* json_object_new_string_len(const char *s, int len)
{
	char *dstbuf;
	struct json_object *jso = json_object_new(json_type_string);
	if (!jso)
		return NULL;
	jso->_delete = &json_object_string_delete;
	jso->_to_json_string = &json_object_string_to_json_string;
	if(len < LEN_DIRECT_STRING_DATA) {
		dstbuf = jso->o.c_string.str.data;
	} else {
		jso->o.c_string.str.ptr = (char*)malloc(len + 1);
		if (!jso->o.c_string.str.ptr)
		{
			json_object_generic_delete(jso);
			errno = ENOMEM;
			return NULL;
		}
		dstbuf = jso->o.c_string.str.ptr;
	}
	memcpy(dstbuf, (void *)s, len);
	dstbuf[len] = '\0';
	jso->o.c_string.len = len;
	return jso;
}

const char* json_object_get_string(struct json_object *jso)
{
	if (!jso)
		return NULL;
	switch(jso->o_type)
	{
	case json_type_string:
		return get_string_component(jso);
	default:
		return json_object_to_json_string(jso);
	}
}

int json_object_get_string_len(struct json_object *jso)
{
	if (!jso)
		return 0;
	switch(jso->o_type)
	{
	case json_type_string:
		return jso->o.c_string.len;
	default:
		return 0;
	}
}


/* json_object_array */

static int json_object_array_to_json_string(struct json_object* jso,
                                            struct printbuf *pb,
                                            int level,
                                            int flags)
{
	int had_children = 0;
	int ii;
	printbuf_memappend_no_nul(pb, "[", 1);
	if (flags & JSON_C_TO_STRING_PRETTY)
		printbuf_memappend_no_nul(pb, "\n", 1);
	for(ii=0; ii < json_object_array_length(jso); ii++)
	{
		struct json_object *val;
		if (had_children)
		{
			printbuf_memappend_no_nul(pb, ",", 1);
			if (flags & JSON_C_TO_STRING_PRETTY)
				printbuf_memappend_no_nul(pb, "\n", 1);
		}
		had_children = 1;
		if (flags & JSON_C_TO_STRING_SPACED)
			printbuf_memappend_no_nul(pb, " ", 1);
		indent(pb, level + 1, flags);
		val = json_object_array_get_idx(jso, ii);
		if(val == NULL)
			printbuf_memappend_no_nul(pb, "null", 4);
		else
			val->_to_json_string(val, pb, level+1, flags);
	}
	if (flags & JSON_C_TO_STRING_PRETTY)
	{
		if (had_children)
			printbuf_memappend_no_nul(pb, "\n", 1);
		indent(pb,level,flags);
	}

	if (flags & JSON_C_TO_STRING_SPACED)
		printbuf_memappend_no_nul(pb, " ]", 2);
	else
		printbuf_memappend_no_nul(pb, "]", 1);
	return 0; /* we need to keep compatible with the API */
}

static void json_object_array_entry_free(void *data)
{
	json_object_put((struct json_object*)data);
}

static void json_object_array_delete(struct json_object* jso)
{
	array_list_free(jso->o.c_array);
	json_object_generic_delete(jso);
}

struct json_object* json_object_new_array(void)
{
	struct json_object *jso = json_object_new(json_type_array);
	if (!jso)
		return NULL;
	jso->_delete = &json_object_array_delete;
	jso->_to_json_string = &json_object_array_to_json_string;
	jso->o.c_array = array_list_new(&json_object_array_entry_free);
	return jso;
}

struct array_list* json_object_get_array(struct json_object *jso)
{
	if (!jso)
		return NULL;
	switch(jso->o_type)
	{
	case json_type_array:
		return jso->o.c_array;
	default:
		return NULL;
	}
}

void json_object_array_sort(struct json_object *jso, int(*sort_fn)(const void *, const void *))
{
	array_list_sort(jso->o.c_array, sort_fn);
}

struct json_object* json_object_array_bsearch(
		const struct json_object *key,
		const struct json_object *jso,
		int (*sort_fn)(const void *, const void *))
{
	struct json_object **result;

	result = (struct json_object **)array_list_bsearch(
			(const void **)&key, jso->o.c_array, sort_fn);

	if (!result)
		return NULL;
	return *result;
}

int json_object_array_length(struct json_object *jso)
{
	return array_list_length(jso->o.c_array);
}

int json_object_array_add(struct json_object *jso,struct json_object *val)
{
	return array_list_add(jso->o.c_array, val);
}

int json_object_array_put_idx(struct json_object *jso, int idx,
			      struct json_object *val)
{
	return array_list_put_idx(jso->o.c_array, idx, val);
}

struct json_object* json_object_array_get_idx(struct json_object *jso,
					      int idx)
{
	return (struct json_object*)array_list_get_idx(jso->o.c_array, idx);
}

