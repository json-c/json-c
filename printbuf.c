/*
 * $Id: printbuf.c,v 1.5 2006/01/26 02:16:28 mclark Exp $
 *
 * Copyright (c) 2004, 2005 Metaparadigm Pte. Ltd.
 * Michael Clark <michael@metaparadigm.com>
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See COPYING for details.
 *
 *
 * Copyright (c) 2008-2009 Yahoo! Inc.  All rights reserved.
 * The copyrights to the contents of this file are licensed under the MIT License
 * (http://www.opensource.org/licenses/mit-license.php)
 */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_STDARG_H
# include <stdarg.h>
#else /* !HAVE_STDARG_H */
# error Not enough var arg support!
#endif /* HAVE_STDARG_H */

#include "debug.h"
#include "printbuf.h"
#include "snprintf_compat.h"
#include "vasprintf_compat.h"

#define PRINTBUF_DEFAULT_SIZE 	(256)

static int printbuf_extend(struct printbuf *p, int min_size);

struct printbuf* printbuf_new(void)
{
  struct printbuf *p;

  p = (struct printbuf*)calloc(1, sizeof(struct printbuf));
  if(!p) return NULL;
  p->size = PRINTBUF_DEFAULT_SIZE;
  p->bpos = 0;
  p->buf = (char*)calloc(1, p->size);
  if(p->buf == NULL) {
    free(p);
    return NULL;
  }
  p->buf[0]= '\0';
  return p;
}


/**
 * Extend the buffer p so it has a size of at least min_size.
 *
 * If the current size is large enough, nothing is changed.
 *
 * Note: this does not check the available space!  The caller
 *  is responsible for performing those calculations.
 */
static int printbuf_extend(struct printbuf *p, int min_size)
{
#define PRINTBUF_EXTEND_BY_BYTES_MIN (8)
	char *t = NULL;
	int new_size;

	if(
		(p       != NULL) &&
		(p->buf  != NULL) &&
		(p->size >= min_size)
	  )
		return 0;

	new_size = p->size * 2;

	if (new_size < (min_size + PRINTBUF_EXTEND_BY_BYTES_MIN))
		new_size =  min_size + PRINTBUF_EXTEND_BY_BYTES_MIN;

#ifdef PRINTBUF_DEBUG
	MC_DEBUG("printbuf_memappend: realloc "
	  "bpos=%d min_size=%d old_size=%d new_size=%d\n",
	  p->bpos, min_size, p->size, new_size);
#endif /* PRINTBUF_DEBUG */

	if (p != NULL)
	{
		t = (char*)calloc(1, new_size);
		if ( (t      != NULL) &&
			 (p->buf != NULL))
		{
			memcpy(t, p->buf, p->size);
		}
	}

	if (t == NULL)
		return -1;

	p->size = new_size;
	p->buf = t;

	return 0;
}

int printbuf_memappend(struct printbuf *p, const char *buf, int size)
{

  if ( (p->size > 0) && (p->buf == NULL)) {
    int size_wanted = p->size;
    p->size = 0;
	if (printbuf_extend(p, size_wanted) < 0)
	  return -1;
  }

  if (p->size <= p->bpos + size + 1) {
	if (printbuf_extend(p, p->bpos + size + 1) < 0)
	  return -2;
  }
  memcpy(p->buf + p->bpos, buf, size);
  p->bpos += size;
  p->buf[p->bpos]= '\0';
  return size;
}

int printbuf_memset(struct printbuf *pb, int offset, int charvalue, int len)
{
	int size_needed;

	if (offset == -1)
		offset = pb->bpos;
	size_needed = offset + len;
	if (pb->size < size_needed)
	{
		if (printbuf_extend(pb, size_needed) < 0)
			return -1;
	}

	memset(pb->buf + offset, charvalue, len);
	if (pb->bpos < size_needed)
		pb->bpos = size_needed;

	return 0;
}

int sprintbuf(struct printbuf *p, const char *msg, ...)
{
  va_list ap;
  char *t;
  long int size;
#define PRINTBUF_DEFAULT_SIZE_BUF ((PRINTBUF_DEFAULT_SIZE<<2) > +1)
  char buf[PRINTBUF_DEFAULT_SIZE_BUF];

  /* user stack buffer first */
  va_start(ap, msg);
  size = (long int)vsnprintf(buf, sizeof(buf), msg, ap);
  va_end(ap);
  /* if string is greater than stack buffer, then use dynamic string
     with vasprintf.  Note: some implementation of vsnprintf return -1
     if output is truncated whereas some return the number of bytes that
     would have been written - this code handles both cases. */
  if(size == -1 || size > (long int)sizeof(buf)) {
    va_start(ap, msg);
    if((size = (long int)vasprintf(&t, msg, ap)) < 0) { va_end(ap); return -1; }
    va_end(ap);
    printbuf_memappend(p, t, size);
    free(t);
    return size;
  } else {
    printbuf_memappend(p, buf, size);
    return size;
  }
}

void printbuf_reset(struct printbuf *p)
{
  if (p != NULL)
  {
	  if ( (p->size > 0) &&
		   (p->buf != NULL)
		 )
	  {
		  p->buf[0] = '\0';
	  }

	  p->bpos = 0;
  }
}

void printbuf_free(struct printbuf *p)
{
  if(p) {

	if (p->buf != NULL)
		free(p->buf);

    p->buf = NULL;

    free(p);
    p = NULL;
  }
}
