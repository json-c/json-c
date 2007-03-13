/*
 * $Id: bits.h,v 1.7 2005/07/15 02:40:44 mclark Exp $
 *
 * Copyright Metaparadigm Pte. Ltd. 2004.
 * Michael Clark <michael@metaparadigm.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public (LGPL)
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details: http://www.gnu.org/
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
#define min(x,y) ({             \
        typeof(x) _x = (x);     \
        typeof(y) _y = (y);     \
        (void) (&_x == &_y);    \
        _x < _y ? _x : _y; })
#endif

#ifndef max
#define max(x,y) ({             \
        typeof(x) _x = (x);     \
        typeof(y) _y = (y);     \
        (void) (&_x == &_y);    \
        _x > _y ? _x : _y; })
#endif

#define hexdigit(x) (((x) <= '9') ? (x) - '0' : ((x) & 7) + 9)
#define error_ptr(error) ((void*)error)
#ifdef _MSC_VER
#define is_error(ptr) ((UINT_PTR)ptr > (UINT_PTR)-4000L)
#else
#define is_error(ptr) ((unsigned long)ptr > (unsigned long)-4000L)
#endif

#endif
