/*
 * $Id: debug.h,v 1.3 2005/06/14 22:41:51 mclark Exp $
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

#ifndef _DEBUG_H_
#define _DEBUG_H_

#include "config.h"

#if HAVE_STRERROR
#define errstr strerror(errno)
#else /* !HAVE_STRERROR */
#define errstr
#endif /* HAVE_STRERROR */

extern void mc_set_debug(int debug);
extern int mc_get_debug();

extern void mc_set_syslog(int syslog);
extern void mc_abort(const char *msg, ...);
extern void mc_debug(const char *msg, ...);
extern void mc_error(const char *msg, ...);
extern void mc_info(const char *msg, ...);

#endif
