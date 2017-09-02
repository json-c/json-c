#ifndef _json_strerror_override_h_
#define _json_strerror_override_h_

#include "config.h"
#include <errno.h>

#include "json_object.h" /* for JSON_EXPORT */

#ifdef __cplusplus
extern "C" {
#endif

JSON_EXPORT char *_json_c_strerror(int errno_in);

#ifndef STRERROR_OVERRIDE_IMPL
#define strerror	_json_c_strerror
#else
#include <string.h>
#endif

#ifdef __cplusplus
}
#endif

#endif /* _json_strerror_override_h_ */ 
