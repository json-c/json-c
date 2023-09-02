#ifndef __math_compat_h
#define __math_compat_h

/**
 * @file
 * @brief Do not use, json-c internal, may be changed or removed at any time.
 */

/* Define isnan, isinf, infinity and nan on Windows/MSVC */

#ifndef HAVE_DECL_ISNAN
#ifdef HAVE_DECL__ISNAN
#include <float.h>
#define isnan(x) _isnan(x)
#else
#ifndef __wasi__
/* On platforms like AIX and "IBM i" we need to provide our own isnan */
#define isnan(x) ((x) != (x))
#endif
#endif
#endif

#ifndef HAVE_DECL_ISINF
#ifdef HAVE_DECL__FINITE
#include <float.h>
#define isinf(x) (!_finite(x))
#else
#ifndef __wasi__
#include <float.h>
/* On platforms like AIX and "IBM i" we need to provide our own isinf */
#define isinf(x) ((x) < -DBL_MAX || (x) > DBL_MAX)
#endif
#endif
#endif

#ifndef HAVE_DECL_INFINITY
#ifndef __wasi__
#include <float.h>
#define INFINITY (DBL_MAX + DBL_MAX)
#define HAVE_DECL_INFINITY
#endif
#endif

#if !defined(HAVE_DECL_NAN) && !defined(__wasi__)
#define NAN (INFINITY - INFINITY)
#define HAVE_DECL_NAN
#endif

#endif
