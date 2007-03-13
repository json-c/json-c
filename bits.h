/*
 * $Id: bits.h,v 1.9 2006/01/26 02:16:28 mclark Exp $
 *
 * Copyright (c) 2004, 2005 Metaparadigm Pte. Ltd.
 * Michael Clark <michael@metaparadigm.com>
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See COPYING for details.
 *
 */

#ifndef _bits_h_
#define _bits_h_

#include "config.h"

#if STDC_HEADERS
# include <stddef.h>
#endif /* STDC_HEADERS */

/* CAW: wrapped in ifndef's to make win32 compliant
**      this fails to take over GCC specifics, but this
**      seems to be unimportant.
 */

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif

#define hexdigit(x) (((x) <= '9') ? (x) - '0' : ((x) & 7) + 9)
#define error_ptr(error) ((void*)error)
#define is_error(ptr) ((unsigned long)ptr > (unsigned long)-4000L)

#endif
