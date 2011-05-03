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

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#if HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif /* HAVE_SYS_TYPES_H */

#if HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif /* HAVE_SYS_STAT_H */

#if HAVE_FCNTL_H
#include <fcntl.h>
#endif /* HAVE_FCNTL_H */

#if HAVE_UNISTD_H
# include <unistd.h>
#endif /* HAVE_UNISTD_H */

#ifdef WIN32
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
# include <io.h>
#endif /* defined(WIN32) */

#if !HAVE_OPEN && defined(WIN32)
# define open _open
#endif


#include "bits.h"
#include "debug.h"
#include "printbuf.h"
#include "json_inttypes.h"
#include "json_object.h"
#include "json_tokener.h"
#include "json_util.h"

struct json_object* json_object_from_file(const char *filename)
{
  struct printbuf *pb;
  struct json_object *obj;
  char buf[JSON_FILE_BUF_SIZE];
  int fd, ret;

  if((fd = open(filename, O_RDONLY)) < 0) {
    MC_ERROR("json_object_from_file: error reading file %s: %s\n",
	     filename, strerror(errno));
    return (struct json_object*)error_ptr(-1);
  }
  if(!(pb = printbuf_new())) {
    close(fd);
    MC_ERROR("json_object_from_file: printbuf_new failed\n");
    return (struct json_object*)error_ptr(-1);
  }
  while((ret = read(fd, buf, JSON_FILE_BUF_SIZE)) > 0) {
    printbuf_memappend(pb, buf, ret);
  }
  close(fd);
  if(ret < 0) {
    MC_ABORT("json_object_from_file: error reading file %s: %s\n",
	     filename, strerror(errno));
    printbuf_free(pb);
    return (struct json_object*)error_ptr(-1);
  }
  obj = json_tokener_parse(pb->buf);
  printbuf_free(pb);
  return obj;
}

int json_object_to_file(char *filename, struct json_object *obj)
{
  const char *json_str;
  int fd, ret;
  unsigned int wpos, wsize;

  if(!obj) {
    MC_ERROR("json_object_to_file: object is null\n");
    return -1;
  }

  if((fd = open(filename, O_WRONLY | O_TRUNC | O_CREAT, 0644)) < 0) {
    MC_ERROR("json_object_to_file: error opening file %s: %s\n",
	     filename, strerror(errno));
    return -1;
  }

  if(!(json_str = json_object_to_json_string(obj))) {
    close(fd);
    return -1;
  }

  wsize = (unsigned int)(strlen(json_str) & UINT_MAX); /* CAW: probably unnecessary, but the most 64bit safe */
  wpos = 0;
  while(wpos < wsize) {
    if((ret = write(fd, json_str + wpos, wsize-wpos)) < 0) {
      close(fd);
      MC_ERROR("json_object_to_file: error writing file %s: %s\n",
	     filename, strerror(errno));
      return -1;
    }

	/* because of the above check for ret < 0, we can safely cast and add */
    wpos += (unsigned int)ret;
  }

  close(fd);
  return 0;
}

int json_parse_int64(const char *buf, int64_t *retval)
{
	int64_t num64;
	if (sscanf(buf, "%" SCNd64, &num64) != 1)
	{
		MC_DEBUG("Failed to parse, sscanf != 1\n");
		return 1;
	}
	const char *buf_skip_space = buf;
	int orig_has_neg = 0;
	// Skip leading spaces
	while (isspace((int)*buf_skip_space) && *buf_skip_space)
		buf_skip_space++;
	if (*buf_skip_space == '-')
	{
		buf_skip_space++;
		orig_has_neg = 1;
	}
	// Skip leading zeros, but keep at least one digit
	while (buf_skip_space[0] == '0' && buf_skip_space[1] != '\0')
		buf_skip_space++;
	if (buf_skip_space[0] == '0' && buf_skip_space[1] == '\0')
		orig_has_neg = 0; // "-0" is the same as just plain "0"
	
	if (errno != ERANGE)
	{
		char buf_cmp[100];
		char *buf_cmp_start = buf_cmp;
		int recheck_has_neg = 0;
		snprintf(buf_cmp_start, sizeof(buf_cmp), "%" PRId64, num64);
		if (*buf_cmp_start == '-')
		{
			recheck_has_neg = 1;
			buf_cmp_start++;
		}
		// No need to skip leading spaces or zeros here.

		int buf_cmp_len = strlen(buf_cmp_start);
		/**
		 * If the sign is different, or
		 * some of the digits are different, or
		 * there is another digit present in the original string
		 * then we NOT successfully parsed the value.
		 */
		if (orig_has_neg != recheck_has_neg ||
		    strncmp(buf_skip_space, buf_cmp_start, strlen(buf_cmp_start)) != 0 ||
			(strlen(buf_skip_space) != buf_cmp_len &&
			 isdigit((int)buf_skip_space[buf_cmp_len])
		    )
		   )
		{
			errno = ERANGE;
		}
	}
	if (errno == ERANGE)
	{
		if (orig_has_neg)
			num64 = INT64_MIN;
		else
			num64 = INT64_MAX;
	}
	*retval = num64;
	return 0;
}

#if HAVE_REALLOC == 0
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
	if (o_type < 0 || o_type >= NELEM(json_type_name))
	{
		MC_ERROR("json_type_to_name: type %d is out of range [0,%d]\n", o_type, NELEM(json_type_name));
		return NULL;
	}
	return json_type_name[o_type];
}

