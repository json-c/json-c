
#ifndef _json_inttypes_h_
#define _json_inttypes_h_

#include "json_config.h"

#if defined(_MSC_VER) && _MSC_VER <= 1700

#if defined(_MSC_VER) && _MSC_VER >= 1600
#include <stdint.h>
#endif

/* Anything less than Visual Studio C++ 10 is missing stdint.h and inttypes.h */
typedef __int32 int32_t;
typedef unsigned __int32 uint32_t;
#if (!defined(__cplusplus))
#define INT32_MIN    ((int32_t)_I32_MIN)
#define INT32_MAX    ((int32_t)_I32_MAX)
#endif
typedef __int64 int64_t;
#if (!defined(__cplusplus))
#define INT64_MIN    ((int64_t)_I64_MIN)
#define INT64_MAX    ((int64_t)_I64_MAX)
#endif
#define PRId64 "I64d"
#define SCNd64 "I64d"

#else

#ifdef JSON_C_HAVE_INTTYPES_H
#include <inttypes.h>
#endif
/* inttypes.h includes stdint.h */

#endif

#endif
