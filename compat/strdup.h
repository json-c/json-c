#ifndef __STRDUP_H_COMPAT__
#define __STRDUP_H_COMPAT__

#include "../config.h"

#if !defined(HAVE_STRDUP) && defined(_MSC_VER)
  /* MSC has the version as _strdup */
# define strdup _strdup
#elif !defined(HAVE_STRDUP)
# error You do not have strdup on your system.
#endif /* HAVE_STRDUP */

#endif /* __STRDUP_H_COMPAT__ */
