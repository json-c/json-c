/*
 * $Id: json_object.h,v 1.12 2006/01/30 23:07:57 mclark Exp $
 *
 * Copyright (c) 2004, 2005 Metaparadigm Pte. Ltd.
 * Michael Clark <michael@metaparadigm.com>
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See COPYING for details.
 *
 */

#ifndef _json_object_h_
#define _json_object_h_

#ifdef __cplusplus
extern "C" {
#endif

#define JSON_OBJECT_DEF_HASH_ENTRIES 16

#undef FALSE
#define FALSE ((boolean)0)

#undef TRUE
#define TRUE ((boolean)1)

extern const char *json_number_chars;
extern const char *json_hex_chars;

/* CAW: added for ANSI C iteration correctness */
struct json_object_iter
{
	char *key;
	struct json_object *val;
	struct lh_entry *entry;
};

/* forward structure definitions */

typedef int boolean;
typedef struct printbuf printbuf;
typedef struct lh_table lh_table;
typedef struct array_list array_list;
typedef struct json_object json_object;
typedef struct json_object_iter json_object_iter;
typedef struct json_tokener json_tokener;

/* supported object types */

typedef enum json_type {
  /* If you change this, be sure to update json_type_to_name() too */
  json_type_null,
  json_type_boolean,
  json_type_double,
  json_type_int,
  json_type_object,
  json_type_array,
  json_type_string,
} json_type;

/* reference counting functions */

/**
 * Increment the reference count of json_object
 * @param obj the json_object instance
 */
extern struct json_object* json_object_get(struct json_object *obj);

/**
 * Decrement the reference count of json_object and free if it reaches zero
 * @param obj the json_object instance
 */
extern void json_object_put(struct json_object *obj);


/**
 * Check if the json_object is of a given type
 * @param obj the json_object instance
 * @param type one of:
     json_type_boolean,
     json_type_double,
     json_type_int,
     json_type_object,
     json_type_array,
     json_type_string,
 */
extern int json_object_is_type(struct json_object *obj, enum json_type type);

/**
 * Get the type of the json_object
 * @param obj the json_object instance
 * @returns type being one of:
     json_type_boolean,
     json_type_double,
     json_type_int,
     json_type_object,
     json_type_array,
     json_type_string,
 */
extern enum json_type json_object_get_type(struct json_object *obj);


/** Stringify object to json format
 * @param obj the json_object instance
 * @returns a string in JSON format
 */
extern const char* json_object_to_json_string(struct json_object *obj);


/* object type methods */

/** Create a new empty object
 * @returns a json_object of type json_type_object
 */
extern struct json_object* json_object_new_object(void);

/** Get the hashtable of a json_object of type json_type_object
 * @param obj the json_object instance
 * @returns a linkhash
 */
extern struct lh_table* json_object_get_object(struct json_object *obj);

/** Add an object field to a json_object of type json_type_object
 *
 * The reference count will *not* be incremented. This is to make adding
 * fields to objects in code more compact. If you want to retain a reference
 * to an added object you must wrap the passed object with json_object_get
 *
 * @param obj the json_object instance
 * @param key the object field name (a private copy will be duplicated)
 * @param val a json_object or NULL member to associate with the given field
 */
extern void json_object_object_add(struct json_object* obj, const char *key,
				   struct json_object *val);

/** Get the json_object associate with a given object field
 * @param obj the json_object instance
 * @param key the object field name
 * @returns the json_object associated with the given field name
 */
extern struct json_object* json_object_object_get(struct json_object* obj,
						  const char *key);

/** Delete the given json_object field
 *
 * The reference count will be decremented for the deleted object
 *
 * @param obj the json_object instance
 * @param key the object field name
 */
extern void json_object_object_del(struct json_object* obj, const char *key);

/** Iterate through all keys and values of an object
 * @param obj the json_object instance
 * @param key the local name for the char* key variable defined in the body
 * @param val the local name for the json_object* object variable defined in the body
 */
#if defined(__GNUC__) && !defined(__STRICT_ANSI__)

# define json_object_object_foreach(obj,key,val) \
 char *key; struct json_object *val; \
 for(struct lh_entry *entry = json_object_get_object(obj)->head; ({ if(entry) { key = (char*)entry->k; val = (struct json_object*)entry->v; } ; entry; }); entry = entry->next )

#else /* ANSI C or MSC */

# define json_object_object_foreach(obj,key,val) \
 char *key; struct json_object *val; struct lh_entry *entry; \
 for(entry = json_object_get_object(obj)->head; (entry ? (key = (char*)entry->k, val = (struct json_object*)entry->v, entry) : 0); entry = entry->next)

#endif /* defined(__GNUC__) && !defined(__STRICT_ANSI__) */

/** Iterate through all keys and values of an object (ANSI C Safe)
 * @param obj the json_object instance
 * @param iter the object iterator
 */
#define json_object_object_foreachC(obj,iter) \
 for(iter.entry = json_object_get_object(obj)->head; (iter.entry ? (iter.key = (char*)iter.entry->k, iter.val = (struct json_object*)iter.entry->v, iter.entry) : 0); iter.entry = iter.entry->next)

/* Array type methods */

/** Create a new empty json_object of type json_type_array
 * @returns a json_object of type json_type_array
 */
extern struct json_object* json_object_new_array(void);

/** Get the arraylist of a json_object of type json_type_array
 * @param obj the json_object instance
 * @returns an arraylist
 */
extern struct array_list* json_object_get_array(struct json_object *obj);

/** Get the length of a json_object of type json_type_array
 * @param obj the json_object instance
 * @returns an int
 */
extern int json_object_array_length(struct json_object *obj);

/** Sorts the elements of jso of type json_type_array
*
* Pointers to the json_object pointers will be passed as the two arguments
* to @sort_fn
*
* @param obj the json_object instance
* @param sort_fn a sorting function
*/
extern void json_object_array_sort(struct json_object *jso, int(*sort_fn)(const void *, const void *));

/** Add an element to the end of a json_object of type json_type_array
 *
 * The reference count will *not* be incremented. This is to make adding
 * fields to objects in code more compact. If you want to retain a reference
 * to an added object you must wrap the passed object with json_object_get
 *
 * @param obj the json_object instance
 * @param val the json_object to be added
 */
extern int json_object_array_add(struct json_object *obj,
				 struct json_object *val);

/** Insert or replace an element at a specified index in an array (a json_object of type json_type_array)
 *
 * The reference count will *not* be incremented. This is to make adding
 * fields to objects in code more compact. If you want to retain a reference
 * to an added object you must wrap the passed object with json_object_get
 *
 * The reference count of a replaced object will be decremented.
 *
 * The array size will be automatically be expanded to the size of the
 * index if the index is larger than the current size.
 *
 * @param obj the json_object instance
 * @param idx the index to insert the element at
 * @param val the json_object to be added
 */
extern int json_object_array_put_idx(struct json_object *obj, int idx,
				     struct json_object *val);

/** Get the element at specificed index of the array (a json_object of type json_type_array)
 * @param obj the json_object instance
 * @param idx the index to get the element at
 * @returns the json_object at the specified index (or NULL)
 */
extern struct json_object* json_object_array_get_idx(struct json_object *obj,
						     int idx);

/* boolean type methods */

/** Create a new empty json_object of type json_type_boolean
 * @param b a boolean TRUE or FALSE (0 or 1)
 * @returns a json_object of type json_type_boolean
 */
extern struct json_object* json_object_new_boolean(boolean b);

/** Get the boolean value of a json_object
 *
 * The type is coerced to a boolean if the passed object is not a boolean.
 * integer and double objects will return FALSE if there value is zero
 * or TRUE otherwise. If the passed object is a string it will return
 * TRUE if it has a non zero length. If any other object type is passed
 * TRUE will be returned if the object is not NULL.
 *
 * @param obj the json_object instance
 * @returns a boolean
 */
extern boolean json_object_get_boolean(struct json_object *obj);


/* int type methods */

/** Create a new empty json_object of type json_type_int
 * Note that values are stored as 64-bit values internally.
 * To ensure the full range is maintained, use json_object_new_int64 instead.
 * @param i the integer
 * @returns a json_object of type json_type_int
 */
extern struct json_object* json_object_new_int(int32_t i);


/** Create a new empty json_object of type json_type_int
 * @param i the integer
 * @returns a json_object of type json_type_int
 */
extern struct json_object* json_object_new_int64(int64_t i);


/** Get the int value of a json_object
 *
 * The type is coerced to a int if the passed object is not a int.
 * double objects will return their integer conversion. Strings will be
 * parsed as an integer. If no conversion exists then 0 is returned.
 *
 * Note that integers are stored internally as 64-bit values.
 * If the value of too big or too small to fit into 32-bit, INT32_MAX or
 * INT32_MIN are returned, respectively.
 *
 * @param obj the json_object instance
 * @returns an int
 */
extern int32_t json_object_get_int(struct json_object *obj);

/** Get the int value of a json_object
 *
 * The type is coerced to a int64 if the passed object is not a int64.
 * double objects will return their int64 conversion. Strings will be
 * parsed as an int64. If no conversion exists then 0 is returned.
 *
 * @param obj the json_object instance
 * @returns an int64
 */
extern int64_t json_object_get_int64(struct json_object *obj);


/* double type methods */

/** Create a new empty json_object of type json_type_double
 * @param d the double
 * @returns a json_object of type json_type_double
 */
extern struct json_object* json_object_new_double(double d);

/** Get the double value of a json_object
 *
 * The type is coerced to a double if the passed object is not a double.
 * integer objects will return their dboule conversion. Strings will be
 * parsed as a double. If no conversion exists then 0.0 is returned.
 *
 * @param obj the json_object instance
 * @returns an double
 */
extern double json_object_get_double(struct json_object *obj);


/* string type methods */

/** Create a new empty json_object of type json_type_string
 *
 * A copy of the string is made and the memory is managed by the json_object
 *
 * @param s the string
 * @returns a json_object of type json_type_string
 */
extern struct json_object* json_object_new_string(const char *s);

extern struct json_object* json_object_new_string_len(const char *s, int len);

/** Get the string value of a json_object
 *
 * If the passed object is not of type json_type_string then the JSON
 * representation of the object is returned.
 *
 * The returned string memory is managed by the json_object and will
 * be freed when the reference count of the json_object drops to zero.
 *
 * @param obj the json_object instance
 * @returns a string
 */
extern const char* json_object_get_string(struct json_object *obj);

/** Get the string length of a json_object
 *
 * If the passed object is not of type json_type_string then zero
 * will be returned.
 *
 * @param obj the json_object instance
 * @returns int
 */
extern int json_object_get_string_len(struct json_object *obj);

#ifdef __cplusplus
}
#endif

#endif
