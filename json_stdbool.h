#ifndef _json_stdbool_h_
#define _json_stdbool_h_

#include "json_config.h"

#ifndef __cplusplus
#if defined(_MSC_VER)

/* The C language implementation does not provide the standard header
 * file "stdbool.h" as required by ISO/IEC 9899:1999.
 */
#if !defined(bool)
#define	bool	int
#endif

#if !defined(true)
#define true	1
#endif

#if !defined(false)
#define	false	0
#endif

#else /* ndef _MSC_VER */

#ifdef JSON_C_HAVE_STDBOOL_H
#include <stdbool.h>
#endif

#endif /* _MSC_VER */
#endif /* __cplusplus */

#endif /* _json_stdbool_h_ */
