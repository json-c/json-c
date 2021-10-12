/*
 * $Id: json_object_private.h,v 1.4 2006/01/26 02:16:28 mclark Exp $
 *
 * Copyright (c) 2004, 2005 Metaparadigm Pte. Ltd.
 * Michael Clark <michael@metaparadigm.com>
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See COPYING for details.
 *
 */

/**
 * @file
 * @brief Do not use, json-c internal, may be changed or removed at any time.
 */
#ifndef _json_object_private_h_
#define _json_object_private_h_

#ifdef __cplusplus
extern "C" {
#endif

struct json_object;
#include "json_inttypes.h"
#include "json_types.h"

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */

#ifdef _MSC_VER
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif

/* json object int type, support extension*/
typedef enum json_object_int_type
{
	json_object_int_type_int64,
	json_object_int_type_uint64
} json_object_int_type;

struct json_object
{
	enum json_type o_type;
	uint32_t _ref_count;
	json_object_to_json_string_fn *_to_json_string;
	struct printbuf *_pb;
	json_object_delete_fn *_user_delete;
	void *_userdata;
	// Actually longer, always malloc'd as some more-specific type.
	// The rest of a struct json_object_${o_type} follows
};

struct json_object_object
{
	struct json_object base;
	struct lh_table *c_object;
};
struct json_object_array
{
	struct json_object base;
	struct array_list *c_array;
};

struct json_object_boolean
{
	struct json_object base;
	json_bool c_boolean;
};
struct json_object_double
{
	struct json_object base;
	double c_double;
};
struct json_object_int
{
	struct json_object base;
	enum json_object_int_type cint_type;
	union
	{
		int64_t c_int64;
		uint64_t c_uint64;
	} cint;
};
struct json_object_string
{
	struct json_object base;
	ssize_t len; // Signed b/c negative lengths indicate data is a pointer
	// Consider adding an "alloc" field here, if json_object_set_string calls
	// to expand the length of a string are common operations to perform.
	union
	{
		char idata[1]; // Immediate data.  Actually longer
		char *pdata;   // Only when len < 0
	} c_string;
};

void _json_c_set_last_err(const char *err_fmt, ...);

/**
 * The characters that can make up hexadecimal numbers
 */
extern const char *json_hex_chars;

/**
 * @brief A buffer of characters that may contain null charaters in the middle
 *
 * A buffer of data that can hold a normal null-terminated string
 * (in which case `length` should just be equal to `strlen`)
 * or a string with embedded null characters (in which case `length` reflects
 * all the characters that make up the "string").
 * Either way, this struct can be treated as if it contains null characters,
 * since the `length` member should always be equal to the proper size of the
 * buffer and the terminating null character wouldn't be included
 * (it wouldn't be counted by strlen).
 */
struct json_key
{
	/**
	 * @brief Stores the length of the buffer
	 *
	 * If the length is positive, then `pdata` should be used.
	 * Otherwise, idata should be used.
	 */
	ssize_t length;
	union
	{
		/**
		 * @brief A pointer to data that is stored elsewhere
		 *
		 * If the data stored there will not change for the lifetime of
		 * the key, use `pdata` rather than `idata`.
		 */
		const char *pdata;
		/**
		 * @brief Data stored inline
		 *
		 * If the data stored may be overwritten, such as if it is
		 * copied from the stack, this struct should be allocated with
		 * enough space to store the whole string (of length `len`)
		 * and one additional null character.
		 */
		char idata[0];
	} str;
};

#ifdef __cplusplus
}
#endif

#endif
