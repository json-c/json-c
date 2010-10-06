
#ifndef _json_inttypes_h_
#define _json_inttypes_h_

#if defined(_MSC_VER) && _MSC_VER < 1600

/* Anything less than Visual Studio C++ 10 is missing stdint.h and inttypes.h */
typedef __int64 int64_t;
#define PRId64 "I64d"
#define SCNd64 "I64d"

#else

#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif
/* inttypes.h includes stdint.h */

#endif

#endif
