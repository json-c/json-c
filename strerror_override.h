#ifndef __STRERROR_OVERRIDE_H__
#define __STRERROR_OVERRIDE_H__

#include "config.h"
#include <errno.h>

#if ENABLE_STRERROR_OVERRIDE
char *_json_c_strerror(int errno_in);
#define strerror	_json_c_strerror
#endif

#endif /* __STRERROR_OVERRIDE_H__ */ 
