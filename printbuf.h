/*
 * $Id: printbuf.h,v 1.4 2006/01/26 02:16:28 mclark Exp $
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

#ifndef _printbuf_h_
#define _printbuf_h_

#ifdef __cplusplus
extern "C" {
#endif

struct printbuf {
  char *buf;
  int bpos;
  int size;
};

extern struct printbuf*
printbuf_new(void);

/* As an optimization, printbuf_memappend_fast is defined as a macro
 * that handles copying data if the buffer is large enough; otherwise
 * it invokes printbuf_memappend_real() which performs the heavy
 * lifting of realloc()ing the buffer and copying data.
 * Your code should not use printbuf_memappend directly--use
 * printbuf_memappend_fast instead.
 */
extern int
printbuf_memappend(struct printbuf *p, const char *buf, int size);

#define printbuf_memappend_fast(p, bufptr, bufsize)          \
do {                                                         \
  if ((p->size - p->bpos) > bufsize) {                       \
    memcpy(p->buf + p->bpos, (bufptr), bufsize);             \
    p->bpos += bufsize;                                      \
    p->buf[p->bpos]= '\0';                                   \
  } else {  printbuf_memappend(p, (bufptr), bufsize); }      \
} while (0)

/* The following functions provide a printbuf interface where the
 * string terminator '\0' is not always written. This is faster, but
 * the string cannot be used with standard functions while being
 * constructed. To do so, printbuf_terminate_string() must be
 * called first.
 */
extern void
printbuf_memappend_no_nul(struct printbuf *p, const char *buf, int size);
extern void
printbuf_terminate_string(struct printbuf *const p);

#define printbuf_length(p) ((p)->bpos)

/**
 * Set len bytes of the buffer to charvalue, starting at offset offset.
 * Similar to calling memset(x, charvalue, len);
 *
 * The memory allocated for the buffer is extended as necessary.
 *
 * If offset is -1, this starts at the end of the current data in the buffer.
 */
extern int
printbuf_memset(struct printbuf *pb, int offset, int charvalue, int len);

extern int
sprintbuf(struct printbuf *p, const char *msg, ...);

extern void
printbuf_reset(struct printbuf *p);

extern void
printbuf_free(struct printbuf *p);

/**
 * Set initial size allocation for memory when creating strings,
 * as is done for example in json_object_to_json_string(). The
 * default size is 32, which is very conservative. If an app
 * knows it typically deals with larger strings, performance
 * can be improved by setting the initial size to a different
 * number, e.g. 1k. Note that this also means that memory
 * consumption can increase. How far entriely depens on the
 * application and its use of json-c.
 *
 * Note: each time this function is called, the initial size is
 * changed to the given value. Already existing elements are not
 * affected. This function is usually meant to be called just once
 * at start of an application, but there is no harm calling it more
 * than once. Note that the function is NOT thread-safe and must not
 * be called on different threads concurrently.
 *
 * @param size new initial size for printbuf (formatting buffer)
 */
extern void
json_global_set_printbuf_initial_size(int size);
#ifdef __cplusplus
}
#endif

#endif
