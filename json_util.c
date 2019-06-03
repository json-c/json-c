/*
 * $Id: json_util.c,v 1.4 2006/01/30 23:07:57 mclark Exp $
 *
 * Copyright (c) 2004, 2005 Metaparadigm Pte. Ltd.
 * Michael Clark <michael@metaparadigm.com>
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See COPYING for details.
 *
 */

#include "config.h"
#undef realloc

#include "strerror_override.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <limits.h>
#include <string.h>
#include <ctype.h>

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif /* HAVE_SYS_TYPES_H */

#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif /* HAVE_SYS_STAT_H */

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif /* HAVE_FCNTL_H */

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif /* HAVE_UNISTD_H */

#ifdef WIN32
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
# include <io.h>
#endif /* defined(WIN32) */

#if !defined(HAVE_OPEN) && defined(WIN32)
# define open _open
#endif

#include "snprintf_compat.h"

#include "debug.h"
#include "printbuf.h"
#include "json_inttypes.h"
#include "json_object.h"
#include "json_tokener.h"
#include "json_util.h"

static int _json_object_to_fd(int fd, struct json_object *obj, int flags, const char *filename);

static char _last_err[256] = "";

const char *json_util_get_last_err()
{
	if (_last_err[0] == '\0')
		return NULL;
	return _last_err;
}

void _json_c_set_last_err(const char *err_fmt, ...)
{
	va_list ap;
	va_start(ap, err_fmt);
	// Ignore (attempted) overruns from snprintf
	(void)vsnprintf(_last_err, sizeof(_last_err), err_fmt, ap);
	va_end(ap);
}

struct json_object* json_object_from_fd(int fd)
{
  struct printbuf *pb;
  struct json_object *obj;
  char buf[JSON_FILE_BUF_SIZE];
  int ret;

  if(!(pb = printbuf_new())) {
    _json_c_set_last_err("json_object_from_file: printbuf_new failed\n");
    return NULL;
  }
  while((ret = read(fd, buf, JSON_FILE_BUF_SIZE)) > 0) {
    printbuf_memappend(pb, buf, ret);
  }
  if(ret < 0) {
    _json_c_set_last_err("json_object_from_fd: error reading fd %d: %s\n", fd, strerror(errno));
    printbuf_free(pb);
    return NULL;
  }
  obj = json_tokener_parse(pb->buf);
  printbuf_free(pb);
  return obj;
}

struct json_object* json_object_from_file(const char *filename)
{
  struct json_object *obj;
  int fd;

  if((fd = open(filename, O_RDONLY)) < 0) {
    _json_c_set_last_err("json_object_from_file: error opening file %s: %s\n",
	     filename, strerror(errno));
    return NULL;
  }
  obj = json_object_from_fd(fd);
  close(fd);
  return obj;
}

/* extended "format and write to file" function */

int json_object_to_file_ext(const char *filename, struct json_object *obj, int flags)
{
	int fd, ret;
	int saved_errno;

	if (!obj) {
		_json_c_set_last_err("json_object_to_file: object is null\n");
		return -1;
	}

	if ((fd = open(filename, O_WRONLY | O_TRUNC | O_CREAT, 0644)) < 0) {
		_json_c_set_last_err("json_object_to_file: error opening file %s: %s\n",
		              filename, strerror(errno));
		return -1;
	}
	ret = _json_object_to_fd(fd, obj, flags, filename);
	saved_errno = errno;
	close(fd);
	errno = saved_errno;
	return ret;
}

int json_object_to_fd(int fd, struct json_object *obj, int flags)
{
	if (!obj) {
		_json_c_set_last_err("json_object_to_fd: object is null\n");
		return -1;
	}

	return _json_object_to_fd(fd, obj, flags, NULL);
}
static int _json_object_to_fd(int fd, struct json_object *obj, int flags, const char *filename)
{
	int ret;
	const char *json_str;
	unsigned int wpos, wsize;

	filename = filename ? filename : "(fd)";

	if (!(json_str = json_object_to_json_string_ext(obj,flags))) {
		return -1;
	}

	wsize = (unsigned int)(strlen(json_str) & UINT_MAX); /* CAW: probably unnecessary, but the most 64bit safe */
	wpos = 0;
	while(wpos < wsize) {
		if((ret = write(fd, json_str + wpos, wsize-wpos)) < 0) {
		  _json_c_set_last_err("json_object_to_file: error writing file %s: %s\n",
			 filename, strerror(errno));
		  return -1;
		}

		/* because of the above check for ret < 0, we can safely cast and add */
		wpos += (unsigned int)ret;
	}

	return 0;
}

// backwards compatible "format and write to file" function

int json_object_to_file(const char *filename, struct json_object *obj)
{
  return json_object_to_file_ext(filename, obj, JSON_C_TO_STRING_PLAIN);
}

int json_parse_double(const char *buf, double *retval)
{
  char *end;
  *retval = strtod(buf, &end);
  return end == buf ? 1 : 0;
}

// The input buffer 'buf' must contain only digits (0 to 9), except
// for the first character, which may be a negative sign '-'.
int json_parse_sanitized_int64(const char *buf, size_t len, int64_t *retval)
{
	uint64_t uval = 0;
	int is_negative = (*buf == '-');
	size_t ii = is_negative ? 1 : 0;

	if (ii == len || buf[ii] == '\0')
		return 1;

	while (ii < len)
	{
		uint64_t tmp = (uval * 10) + buf[ii++] - '0';
		// Check for overflow.
		if ((int64_t) uval > (int64_t) tmp)
		{
			*retval = is_negative ? INT64_MIN : INT64_MAX;
			return 0;
		}
		uval = tmp;
	}

	*retval = is_negative ? -uval : uval;

	return 0;
}

int json_parse_int64(const char *buf, int64_t *retval)
{
	size_t len = 0;
	// Skip leading white spaces.
	while (isspace(*buf))
		buf++;
	// Calculate length of valid input.
	if (buf[len] == '-')
		len++;
	while (buf[len] >= '0' && buf[len] <= '9')
		len++;
	if (len == 0)
		return 1;
	return json_parse_sanitized_int64(buf, len, retval);
}

#ifndef HAVE_REALLOC
void* rpl_realloc(void* p, size_t n)
{
	if (n == 0)
		n = 1;
	if (p == 0)
		return malloc(n);
	return realloc(p, n);
}
#endif

#define NELEM(a)        (sizeof(a) / sizeof(a[0]))
static const char* json_type_name[] = {
  /* If you change this, be sure to update the enum json_type definition too */
  "null",
  "boolean",
  "double",
  "int",
  "object",
  "array",
  "string",
};

const char *json_type_to_name(enum json_type o_type)
{
	int o_type_int = (int)o_type;
	if (o_type_int < 0 || o_type_int >= (int)NELEM(json_type_name))
	{
		_json_c_set_last_err("json_type_to_name: type %d is out of range [0,%d]\n", o_type, NELEM(json_type_name));
		return NULL;
	}
	return json_type_name[o_type];
}

