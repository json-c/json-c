/*
 * $Id: json_util.h,v 1.4 2006/01/30 23:07:57 mclark Exp $
 *
 * Copyright (c) 2004, 2005 Metaparadigm Pte. Ltd.
 * Michael Clark <michael@metaparadigm.com>
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See COPYING for details.
 *
 */

#ifndef _json_util_h_
#define _json_util_h_

#include "json_object.h"

#ifndef json_min
#define json_min(a,b) ((a) < (b) ? (a) : (b))
#endif

#ifndef json_max
#define json_max(a,b) ((a) > (b) ? (a) : (b))
#endif


#ifdef __cplusplus
extern "C" {
#endif

#define JSON_FILE_BUF_SIZE 4096

/* utility functions */
/**
 * Read the full contents of the given file, then convert it to a
 * json_object using json_tokener_parse().
 *
 * Returns -1 if something fails.  See json_util_get_last_err() for details.
 */
extern struct json_object* json_object_from_file(const char *filename);

/**
 * Create a JSON object from already opened file descriptor.
 *
 * This function can be helpful, when you opened the file already,
 * e.g. when you have a temp file.
 * Note, that the fd must be readable at the actual position, i.e.
 * use lseek(fd, 0, SEEK_SET) before.
 *
 * Returns -1 if something fails.  See json_util_get_last_err() for details.
 */
extern struct json_object* json_object_from_fd(int fd);

/**
 * Equivalent to:
 *   json_object_to_file_ext(filename, obj, JSON_C_TO_STRING_PLAIN);
 *
 * Returns -1 if something fails.  See json_util_get_last_err() for details.
 */
extern int json_object_to_file(const char *filename, struct json_object *obj);

/**
 * Open and truncate the given file, creating it if necessary, then
 * convert the json_object to a string and write it to the file.
 *
 * Returns -1 if something fails.  See json_util_get_last_err() for details.
 */
extern int json_object_to_file_ext(const char *filename, struct json_object *obj, int flags);

/**
 * Return the last error from json_object_to_file{,_ext} or
 * json_object_from_{file,fd}, or NULL if there is none.
 */
const char *json_util_get_last_err(void);


extern int json_parse_int64(const char *buf, int64_t *retval);
extern int json_parse_double(const char *buf, double *retval);

/**
 * Return a string describing the type of the object.
 * e.g. "int", or "object", etc...
 */
extern const char *json_type_to_name(enum json_type o_type);

/** duplilcate a json_object.
* Not thread safe, because of obj need to convert to string first
* @param obj the json_object instance
* @returns a new json object, remember when you newjson object should release by yourself
*/
extern struct json_object* json_object_duplicate(const struct json_object* obj);

#ifdef __cplusplus
}
#endif

#endif
