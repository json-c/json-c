
/* dummy source file for compatibility purposes */

#if defined(HAVE_CDEFS_H)
#include <sys/cdefs.h>
#endif

#ifndef __warn_references

#ifdef __GNUC__
#define __warn_references(sym,msg)                      \
    __asm(".pushsection .gnu.warning." #sym "\n"            \
      ".ascii \"" msg "\"\n"                    \
      ".popsection");

#else
#define __warn_references(sym,msg)    /* nothing */
#endif

#endif 

#include "json_object.h"

__warn_references(json_object_get, "Warning: please link against libjson-c instead of libjson");

/*        __asm__(".section .gnu.warning." __STRING(sym)  \
            " ; .ascii \"" msg "\" ; .text") */
