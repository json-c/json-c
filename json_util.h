/*
 * $Id: json_util.h,v 1.3 2006/01/26 02:16:28 mclark Exp $
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

#include "config.h"

#ifdef WIN32
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
# include <io.h>
#endif

#include "json_object.h"

#if !HAVE_OPEN && defined(WIN32)
# define open _open
#endif

#define JSON_FILE_BUF_SIZE 4096

/* utlitiy functions */
extern struct json_object* json_object_from_file(char *filename);
extern int json_object_to_file(char *filename, struct json_object *obj);

#endif
