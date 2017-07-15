#ifndef _json_strerror_override_h_
#define _json_strerror_override_h_

#include "config.h"
#include <errno.h>

char *_json_c_strerror(int errno_in);

#ifndef STRERROR_OVERRIDE_IMPL
#define strerror	_json_c_strerror
#endif

#endif /* _json_strerror_override_h_ */ 
