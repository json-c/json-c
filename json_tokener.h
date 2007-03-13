/*
 * $Id: json_tokener.h,v 1.8 2006/01/26 02:16:28 mclark Exp $
 *
 * Copyright (c) 2004, 2005 Metaparadigm Pte. Ltd.
 * Michael Clark <michael@metaparadigm.com>
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See COPYING for details.
 *
 */

#ifndef _json_tokener_h_
#define _json_tokener_h_

#include "config.h"
#include "json_object.h"

enum json_tokener_error {
  json_tokener_success,
  json_tokener_error_parse_unexpected,
  json_tokener_error_parse_null,
  json_tokener_error_parse_boolean,
  json_tokener_error_parse_number,
  json_tokener_error_parse_array,
  json_tokener_error_parse_object,
  json_tokener_error_parse_string,
  json_tokener_error_parse_comment,
  json_tokener_error_parse_eof
};

enum json_tokener_state {
  json_tokener_state_eatws,
  json_tokener_state_start,
  json_tokener_state_finish,
  json_tokener_state_null,
  json_tokener_state_comment_start,
  json_tokener_state_comment,
  json_tokener_state_comment_eol,
  json_tokener_state_comment_end,
  json_tokener_state_string,
  json_tokener_state_string_escape,
  json_tokener_state_escape_unicode,
  json_tokener_state_boolean,
  json_tokener_state_number,
  json_tokener_state_array,
  json_tokener_state_array_sep,
  json_tokener_state_object,
  json_tokener_state_object_field_start,
  json_tokener_state_object_field,
  json_tokener_state_object_field_end,
  json_tokener_state_object_value,
  json_tokener_state_object_sep
};

struct json_tokener
{
  char *source;
  int pos;
  struct printbuf *pb;
};

#if !HAVE_STRNCASECMP && defined(_MSC_VER)
  /* MSC has the version as _strnicmp */
# define strncasecmp _strnicmp
#elif !HAVE_STRNCASECMP
# error You do not have strncasecmp on your system.  
#endif /* HAVE_STRNCASECMP */

#if !HAVE_STRNDUP
  char* strndup(const char* str, size_t n);
#endif /* !HAVE_STRNDUP */

extern struct json_object* json_tokener_parse(char *s);

#endif
