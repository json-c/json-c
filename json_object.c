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

#include "strerror_override.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <math.h>

#include "debug.h"
#include "printbuf.h"
#include "linkhash.h"
#include "arraylist.h"
#include "json_inttypes.h"
#include "json_object.h"
#include "json_object_private.h"
#include "json_util.h"
#include "math_compat.h"
#include "strdup_compat.h"
#include "snprintf_compat.h"

// Don't define this.  It's not thread-safe.
/* #define REFCOUNT_DEBUG 1 */

const char *json_number_chars = "0123456789.+-eE";
const char *json_hex_chars = "0123456789abcdefABCDEF";

static void json_object_generic_delete(struct json_object* jso);
static struct json_object* json_object_new(enum json_type o_type);

static json_object_to_json_string_fn json_object_object_to_json_string;
static json_object_to_json_string_fn json_object_boolean_to_json_string;
static json_object_to_json_string_fn json_object_double_to_json_string_default;
static json_object_to_json_string_fn json_object_int_to_json_string;
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
				struct json_object* obj =
				  (struct json_object*) lh_entry_v(ent);
				MC_DEBUG("\t%s:%p\n",
					 json_type_to_name(obj->o_type), obj);
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
get_string_component(const struct json_object *jso)
{
	return (jso->o.c_string.len < LEN_DIRECT_STRING_DATA) ?
		   jso->o.c_string.str.data : jso->o.c_string.str.ptr;
}

/* string escaping */

static int json_escape_str(struct printbuf *pb, const char *str, int len, int flags)
{
	int pos = 0, start_offset = 0;
	unsigned char c;
	while (len--)
	{
		c = str[pos];
		switch(c)
		{
		case '\b':
		case '\n':
		case '\r':
		case '\t':
		case '\f':
		case '"':
		case '\\':
		case '/':
			if((flags & JSON_C_TO_STRING_NOSLASHESCAPE) && c == '/')
			{
				pos++;
				break;
			}

			if(pos - start_offset > 0)
				printbuf_memappend(pb, str + start_offset, pos - start_offset);

			if(c == '\b') printbuf_memappend(pb, "\\b", 2);
			else if(c == '\n') printbuf_memappend(pb, "\\n", 2);
			else if(c == '\r') printbuf_memappend(pb, "\\r", 2);
			else if(c == '\t') printbuf_memappend(pb, "\\t", 2);
			else if(c == '\f') printbuf_memappend(pb, "\\f", 2);
			else if(c == '"') printbuf_memappend(pb, "\\\"", 2);
			else if(c == '\\') printbuf_memappend(pb, "\\\\", 2);
			else if(c == '/') printbuf_memappend(pb, "\\/", 2);

			start_offset = ++pos;
			break;
		default:
			if(c < ' ')
			{
				char sbuf[7];
				if(pos - start_offset > 0)
					printbuf_memappend(pb,
							   str + start_offset,
							   pos - start_offset);
				snprintf(sbuf, sizeof(sbuf),
					 "\\u00%c%c",
					 json_hex_chars[c >> 4],
					 json_hex_chars[c & 0xf]);
				printbuf_memappend_fast(pb, sbuf, (int) sizeof(sbuf) - 1);
				start_offset = ++pos;
			} else
				pos++;
		}
	}
	if (pos - start_offset > 0)
		printbuf_memappend(pb, str + start_offset, pos - start_offset);
	return 0;
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

int json_object_is_type(const struct json_object *jso, enum json_type type)
{
	if (!jso)
		return (type == json_type_null);
	return (jso->o_type == type);
}

enum json_type json_object_get_type(const struct json_object *jso)
{
	if (!jso)
		return json_type_null;
	return jso->o_type;
}

void* json_object_get_userdata(json_object *jso) {
	return jso ? jso->_userdata : NULL;
}

void json_object_set_userdata(json_object *jso, void *userdata,
			      json_object_delete_fn *user_delete)
{
	// Can't return failure, so abort if we can't perform the operation.
	assert(jso != NULL);

	// First, clean up any previously existing user info
	if (jso->_user_delete)
		jso->_user_delete(jso, jso->_userdata);

	jso->_userdata = userdata;
	jso->_user_delete = user_delete;
}

/* set a custom conversion to string */

void json_object_set_serializer(json_object *jso,
	json_object_to_json_string_fn to_string_func,
	void *userdata,
	json_object_delete_fn *user_delete)
{
	json_object_set_userdata(jso, userdata, user_delete);

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
			jso->_to_json_string = &json_object_double_to_json_string_default;
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
}


/* extended conversion to string */

const char* json_object_to_json_string_length(struct json_object *jso, int flags, size_t *length)
{
	const char *r = NULL;
	size_t s = 0;

	if (!jso)
	{
		s = 4;
		r = "null";
	}
	else if ((jso->_pb) || (jso->_pb = printbuf_new()))
	{
		printbuf_reset(jso->_pb);

		if(jso->_to_json_string(jso, jso->_pb, 0, flags) >= 0)
		{
			s = (size_t)jso->_pb->bpos;
			r = jso->_pb->buf;
		}
	}

	if (length)
		*length = s;
	return r;
}

const char* json_object_to_json_string_ext(struct json_object *jso, int flags)
{
	return json_object_to_json_string_length(jso, flags, NULL);
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

	printbuf_strappend(pb, "{" /*}*/);
	if (flags & JSON_C_TO_STRING_PRETTY)
		printbuf_strappend(pb, "\n");
	json_object_object_foreachC(jso, iter)
	{
		if (had_children)
		{
			printbuf_strappend(pb, ",");
			if (flags & JSON_C_TO_STRING_PRETTY)
				printbuf_strappend(pb, "\n");
		}
		had_children = 1;
		if (flags & JSON_C_TO_STRING_SPACED)
			printbuf_strappend(pb, " ");
		indent(pb, level+1, flags);
		printbuf_strappend(pb, "\"");
		json_escape_str(pb, iter.key, strlen(iter.key), flags);
		if (flags & JSON_C_TO_STRING_SPACED)
			printbuf_strappend(pb, "\": ");
		else
			printbuf_strappend(pb, "\":");
		if(iter.val == NULL)
			printbuf_strappend(pb, "null");
		else
			if (iter.val->_to_json_string(iter.val, pb, level+1,flags) < 0)
				return -1;
	}
	if (flags & JSON_C_TO_STRING_PRETTY)
	{
		if (had_children)
			printbuf_strappend(pb, "\n");
		indent(pb,level,flags);
	}
	if (flags & JSON_C_TO_STRING_SPACED)
		return printbuf_strappend(pb, /*{*/ " }");
	else
		return printbuf_strappend(pb, /*{*/ "}");
}


static void json_object_lh_entry_free(struct lh_entry *ent)
{
	if (!ent->k_is_constant)
		free(lh_entry_k(ent));
	json_object_put((struct json_object*)lh_entry_v(ent));
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

struct lh_table* json_object_get_object(const struct json_object *jso)
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

int json_object_object_add_ex(struct json_object* jso,
	const char *const key,
	struct json_object *const val,
	const unsigned opts)
{
	assert(json_object_get_type(jso) == json_type_object);

	// We lookup the entry and replace the value, rather than just deleting
	// and re-adding it, so the existing key remains valid.
	json_object *existing_value = NULL;
	struct lh_entry *existing_entry;
	const unsigned long hash = lh_get_hash(jso->o.c_object, (const void *)key);
	existing_entry = (opts & JSON_C_OBJECT_ADD_KEY_IS_NEW) ? NULL : 
			      lh_table_lookup_entry_w_hash(jso->o.c_object,
							   (const void *)key, hash);

	// The caller must avoid creating loops in the object tree, but do a
	// quick check anyway to make sure we're not creating a trivial loop.
	if (jso == val)
		return -1;

	if (!existing_entry)
	{
		const void *const k = (opts & JSON_C_OBJECT_KEY_IS_CONSTANT) ?
					(const void *)key : strdup(key);
		if (k == NULL)
			return -1;
		return lh_table_insert_w_hash(jso->o.c_object, k, val, hash, opts);
	}
	existing_value = (json_object *) lh_entry_v(existing_entry);
	if (existing_value)
		json_object_put(existing_value);
	existing_entry->v = val;
	return 0;
}

int json_object_object_add(struct json_object* jso, const char *key,
                           struct json_object *val)
{
	return json_object_object_add_ex(jso, key, val, 0);
}


int json_object_object_length(const struct json_object *jso)
{
	assert(json_object_get_type(jso) == json_type_object);
	return lh_table_length(jso->o.c_object);
}

struct json_object* json_object_object_get(const struct json_object* jso,
					   const char *key)
{
	struct json_object *result = NULL;
	json_object_object_get_ex(jso, key, &result);
	return result;
}

json_bool json_object_object_get_ex(const struct json_object* jso, const char *key,
				    struct json_object **value)
{
	if (value != NULL)
		*value = NULL;

	if (NULL == jso)
		return FALSE;

	switch(jso->o_type)
	{
	case json_type_object:
		return lh_table_lookup_ex(jso->o.c_object, (const void *) key,
					  (void**) value);
	default:
		if (value != NULL)
			*value = NULL;
		return FALSE;
	}
}

void json_object_object_del(struct json_object* jso, const char *key)
{
	assert(json_object_get_type(jso) == json_type_object);
	lh_table_delete(jso->o.c_object, key);
}


/* json_object_boolean */

static int json_object_boolean_to_json_string(struct json_object* jso,
					      struct printbuf *pb,
					      int level,
					      int flags)
{
	if (jso->o.c_boolean)
		return printbuf_strappend(pb, "true");
	return printbuf_strappend(pb, "false");
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

json_bool json_object_get_boolean(const struct json_object *jso)
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

int json_object_set_boolean(struct json_object *jso,json_bool new_value){
	if (!jso || jso->o_type!=json_type_boolean)
		return 0;
	jso->o.c_boolean=new_value;
	return 1;
}


/* json_object_int */

static int json_object_int_to_json_string(struct json_object* jso,
					  struct printbuf *pb,
					  int level,
					  int flags)
{
	/* room for 19 digits, the sign char, and a null term */
	char sbuf[21];
	snprintf(sbuf, sizeof(sbuf), "%" PRId64, jso->o.c_int64);
	return printbuf_memappend (pb, sbuf, strlen(sbuf));
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

int32_t json_object_get_int(const struct json_object *jso)
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
	if (cint64 >= INT32_MAX)
		return INT32_MAX;
	return (int32_t) cint64;
  case json_type_double:
    if (jso->o.c_double <= INT32_MIN)
      return INT32_MIN;
    if (jso->o.c_double >= INT32_MAX)
      return INT32_MAX;
    return (int32_t)jso->o.c_double;
  case json_type_boolean:
    return jso->o.c_boolean;
  default:
    return 0;
  }
}

int json_object_set_int(struct json_object *jso,int new_value){
	if (!jso || jso->o_type!=json_type_int)
		return 0;
	jso->o.c_int64=new_value;
	return 1;
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

int64_t json_object_get_int64(const struct json_object *jso)
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
		/* FALLTHRU */
	default:
		return 0;
	}
}

int json_object_set_int64(struct json_object *jso,int64_t new_value){
	if (!jso || jso->o_type!=json_type_int)
		return 0;
	jso->o.c_int64=new_value;
	return 1;
}

/* json_object_double */

#ifdef HAVE___THREAD
// i.e. __thread or __declspec(thread)
static SPEC___THREAD char *tls_serialization_float_format = NULL;
#endif
static char *global_serialization_float_format = NULL;

int json_c_set_serialization_double_format(const char *double_format, int global_or_thread)
{
	if (global_or_thread == JSON_C_OPTION_GLOBAL)
	{
#ifdef HAVE___THREAD
		if (tls_serialization_float_format)
		{
			free(tls_serialization_float_format);
			tls_serialization_float_format = NULL;
		}
#endif
		if (global_serialization_float_format)
			free(global_serialization_float_format);
		global_serialization_float_format = double_format ? strdup(double_format) : NULL;
	}
	else if (global_or_thread == JSON_C_OPTION_THREAD)
	{
#ifdef HAVE___THREAD
		if (tls_serialization_float_format)
		{
			free(tls_serialization_float_format);
			tls_serialization_float_format = NULL;
		}
		tls_serialization_float_format = double_format ? strdup(double_format) : NULL;
#else
		_set_last_err("json_c_set_option: not compiled with __thread support\n");
		return -1;
#endif
	}
	else
	{
		_set_last_err("json_c_set_option: invalid global_or_thread value: %d\n", global_or_thread);
		return -1;
	}
	return 0;
}


static int json_object_double_to_json_string_format(struct json_object* jso,
						    struct printbuf *pb,
						    int level,
						    int flags,
						    const char *format)
{
	char buf[128], *p, *q;
	int size;
	double dummy; /* needed for modf() */
	/* Although JSON RFC does not support
	NaN or Infinity as numeric values
	ECMA 262 section 9.8.1 defines
	how to handle these cases as strings */
	if (isnan(jso->o.c_double))
	{
		size = snprintf(buf, sizeof(buf), "NaN");
	}
	else if (isinf(jso->o.c_double))
	{
		if(jso->o.c_double > 0)
			size = snprintf(buf, sizeof(buf), "Infinity");
		else
			size = snprintf(buf, sizeof(buf), "-Infinity");
	}
	else
	{
		const char *std_format = "%.17g";

#ifdef HAVE___THREAD
		if (tls_serialization_float_format)
			std_format = tls_serialization_float_format;
		else
#endif
		if (global_serialization_float_format)
			std_format = global_serialization_float_format;
		if (!format)
			format = std_format;
		size = snprintf(buf, sizeof(buf), format, jso->o.c_double);
		if (modf(jso->o.c_double, &dummy) == 0  && size >= 0 && size < (int)sizeof(buf) - 2)
		{
			// Ensure it looks like a float, even if snprintf didn't.
			strcat(buf, ".0");
			size += 2;
		}
	}
	// although unlikely, snprintf can fail
	if (size < 0)
		return -1;

	p = strchr(buf, ',');
	if (p)
		*p = '.';
	else
		p = strchr(buf, '.');
	if (p && (flags & JSON_C_TO_STRING_NOZERO))
	{
		/* last useful digit, always keep 1 zero */
		p++;
		for (q=p ; *q ; q++) {
			if (*q!='0') p=q;
		}
		/* drop trailing zeroes */
		*(++p) = 0;
		size = p-buf;
	}

	if (size >= (int)sizeof(buf))
		// The standard formats are guaranteed not to overrun the buffer,
		// but if a custom one happens to do so, just silently truncate.
		size = sizeof(buf) - 1;
	printbuf_memappend(pb, buf, size);
	return size;
}

static int json_object_double_to_json_string_default(struct json_object* jso,
						     struct printbuf *pb,
						     int level,
						     int flags)
{
	return json_object_double_to_json_string_format(jso, pb, level, flags,
							NULL);
}

int json_object_double_to_json_string(struct json_object* jso,
				      struct printbuf *pb,
				      int level,
				      int flags)
{
	return json_object_double_to_json_string_format(jso, pb, level, flags,
							(const char *)jso->_userdata);
}

struct json_object* json_object_new_double(double d)
{
	struct json_object *jso = json_object_new(json_type_double);
	if (!jso)
		return NULL;
	jso->_to_json_string = &json_object_double_to_json_string_default;
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
	printbuf_memappend(pb, (const char *)jso->_userdata, userdata_len);
	return userdata_len;
}

void json_object_free_userdata(struct json_object *jso, void *userdata)
{
	free(userdata);
}

double json_object_get_double(const struct json_object *jso)
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

int json_object_set_double(struct json_object *jso,double new_value){
	if (!jso || jso->o_type!=json_type_double)
		return 0;
	jso->o.c_double=new_value;
	return 1;
}

/* json_object_string */

static int json_object_string_to_json_string(struct json_object* jso,
					     struct printbuf *pb,
					     int level,
					     int flags)
{
	printbuf_strappend(pb, "\"");
	json_escape_str(pb, get_string_component(jso), jso->o.c_string.len, flags);
	printbuf_strappend(pb, "\"");
	return 0;
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
	memcpy(dstbuf, (const void *)s, len);
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

int json_object_get_string_len(const struct json_object *jso)
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

int json_object_set_string(json_object* jso, const char* s) {
	return json_object_set_string_len(jso, s, (int)(strlen(s)));
}

int json_object_set_string_len(json_object* jso, const char* s, int len){
	if (jso==NULL || jso->o_type!=json_type_string) return 0; 	
	char *dstbuf; 
	if (len<LEN_DIRECT_STRING_DATA) {
		dstbuf=jso->o.c_string.str.data;
		if (jso->o.c_string.len>=LEN_DIRECT_STRING_DATA) free(jso->o.c_string.str.ptr); 
	} else {
		dstbuf=(char *)malloc(len+1);
		if (dstbuf==NULL) return 0;
		if (jso->o.c_string.len>=LEN_DIRECT_STRING_DATA) free(jso->o.c_string.str.ptr);
		jso->o.c_string.str.ptr=dstbuf;
	}
	jso->o.c_string.len=len;
	memcpy(dstbuf, (const void *)s, len);
	dstbuf[len] = '\0';
	return 1; 
}

/* json_object_array */

static int json_object_array_to_json_string(struct json_object* jso,
                                            struct printbuf *pb,
                                            int level,
                                            int flags)
{
	int had_children = 0;
	size_t ii;

	printbuf_strappend(pb, "[");
	if (flags & JSON_C_TO_STRING_PRETTY)
		printbuf_strappend(pb, "\n");
	for(ii=0; ii < json_object_array_length(jso); ii++)
	{
		struct json_object *val;
		if (had_children)
		{
			printbuf_strappend(pb, ",");
			if (flags & JSON_C_TO_STRING_PRETTY)
				printbuf_strappend(pb, "\n");
		}
		had_children = 1;
		if (flags & JSON_C_TO_STRING_SPACED)
			printbuf_strappend(pb, " ");
		indent(pb, level + 1, flags);
		val = json_object_array_get_idx(jso, ii);
		if(val == NULL)
			printbuf_strappend(pb, "null");
		else
			if (val->_to_json_string(val, pb, level+1, flags) < 0)
				return -1;
	}
	if (flags & JSON_C_TO_STRING_PRETTY)
	{
		if (had_children)
			printbuf_strappend(pb, "\n");
		indent(pb,level,flags);
	}

	if (flags & JSON_C_TO_STRING_SPACED)
		return printbuf_strappend(pb, " ]");
	return printbuf_strappend(pb, "]");
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
        if(jso->o.c_array == NULL)
	{
	    free(jso);
	    return NULL;
	}
	return jso;
}

struct array_list* json_object_get_array(const struct json_object *jso)
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

void json_object_array_sort(struct json_object *jso,
			    int(*sort_fn)(const void *, const void *))
{
	assert(json_object_get_type(jso) == json_type_array);
	array_list_sort(jso->o.c_array, sort_fn);
}

struct json_object* json_object_array_bsearch(
		const struct json_object *key,
		const struct json_object *jso,
		int (*sort_fn)(const void *, const void *))
{
	struct json_object **result;

	assert(json_object_get_type(jso) == json_type_array);
	result = (struct json_object **)array_list_bsearch(
			(const void **)(void *)&key, jso->o.c_array, sort_fn);

	if (!result)
		return NULL;
	return *result;
}

size_t json_object_array_length(const struct json_object *jso)
{
	assert(json_object_get_type(jso) == json_type_array);
	return array_list_length(jso->o.c_array);
}

int json_object_array_add(struct json_object *jso,struct json_object *val)
{
	assert(json_object_get_type(jso) == json_type_array);
	return array_list_add(jso->o.c_array, val);
}

int json_object_array_put_idx(struct json_object *jso, size_t idx,
			      struct json_object *val)
{
	assert(json_object_get_type(jso) == json_type_array);
	return array_list_put_idx(jso->o.c_array, idx, val);
}

int json_object_array_del_idx(struct json_object *jso, size_t idx, size_t count)
{
	assert(json_object_get_type(jso) == json_type_array);
	return array_list_del_idx(jso->o.c_array, idx, count);
}

struct json_object* json_object_array_get_idx(const struct json_object *jso,
					      size_t idx)
{
	assert(json_object_get_type(jso) == json_type_array);
	return (struct json_object*)array_list_get_idx(jso->o.c_array, idx);
}

static int json_array_equal(struct json_object* jso1,
			    struct json_object* jso2)
{
	size_t len, i;

	len = json_object_array_length(jso1);
	if (len != json_object_array_length(jso2))
		return 0;

	for (i = 0; i < len; i++) {
		if (!json_object_equal(json_object_array_get_idx(jso1, i),
				       json_object_array_get_idx(jso2, i)))
			return 0;
	}
	return 1;
}

static int json_object_all_values_equal(struct json_object* jso1,
					struct json_object* jso2)
{
	struct json_object_iter iter;
	struct json_object *sub;

	assert(json_object_get_type(jso1) == json_type_object);
	assert(json_object_get_type(jso2) == json_type_object);
	/* Iterate over jso1 keys and see if they exist and are equal in jso2 */
        json_object_object_foreachC(jso1, iter) {
		if (!lh_table_lookup_ex(jso2->o.c_object, (void*)iter.key,
					(void**)(void *)&sub))
			return 0;
		if (!json_object_equal(iter.val, sub))
			return 0;
        }

	/* Iterate over jso2 keys to see if any exist that are not in jso1 */
        json_object_object_foreachC(jso2, iter) {
		if (!lh_table_lookup_ex(jso1->o.c_object, (void*)iter.key,
					(void**)(void *)&sub))
			return 0;
        }

	return 1;
}

int json_object_equal(struct json_object* jso1, struct json_object* jso2)
{
	if (jso1 == jso2)
		return 1;

	if (!jso1 || !jso2)
		return 0;

	if (jso1->o_type != jso2->o_type)
		return 0;

	switch(jso1->o_type) {
		case json_type_boolean:
			return (jso1->o.c_boolean == jso2->o.c_boolean);

		case json_type_double:
			return (jso1->o.c_double == jso2->o.c_double);

		case json_type_int:
			return (jso1->o.c_int64 == jso2->o.c_int64);

		case json_type_string:
			return (jso1->o.c_string.len == jso2->o.c_string.len &&
				memcmp(get_string_component(jso1),
				       get_string_component(jso2),
				       jso1->o.c_string.len) == 0);

		case json_type_object:
			return json_object_all_values_equal(jso1, jso2);

		case json_type_array:
			return json_array_equal(jso1, jso2);

		case json_type_null:
			return 1;
	};

	return 0;
}

