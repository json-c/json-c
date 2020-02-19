# Many projects still are stuck using CMake 2.8 is several places so it's good to provide backward support too. This is
# specially true in old embedded systems (OpenWRT and friends) where CMake isn't necessarily upgraded.
cmake_minimum_required(VERSION 2.8)

if(POLICY CMP0048)
	cmake_policy(SET CMP0048 NEW)
endif()

# JSON-C library is C only project.
project(json-c LANGUAGES C VERSION 0.13.99)

# If we've got 3.0 then it's good, let's provide support. Otherwise, leave it be.
if(POLICY CMP0038)
  # Policy CMP0038 introduced was in CMake 3.0
  cmake_policy(SET CMP0038 NEW)
endif()

if(POLICY CMP0054)
    cmake_policy(SET CMP0054 NEW)
endif()

# Include file check macros honor CMAKE_REQUIRED_LIBRARIES
# i.e. the check_include_file() calls will include -lm when checking.
if(POLICY CMP0075)
	cmake_policy(SET CMP0075 NEW)
endif()

include(CTest)

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING AND
   (NOT MSVC OR NOT (MSVC_VERSION LESS 1800)) # Tests need at least VS2013
   )
add_subdirectory(tests)
endif()

# Set some packaging variables.
set(CPACK_PACKAGE_NAME              "${PROJECT_NAME}")
set(CPACK_PACKAGE_VERSION_MAJOR     "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR     "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH     "${PROJECT_VERSION_PATCH}")
set(JSON_C_BUGREPORT                "json-c@googlegroups.com")

include(CheckSymbolExists)
include(CheckIncludeFile)
include(CheckIncludeFiles)
include(CheckCSourceCompiles)
include(CheckTypeSize)
include(CPack)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

option(BUILD_SHARED_LIBS  "Default to building shared libraries" ON)

# Enable or disable features. By default, all features are turned off.
option(ENABLE_RDRAND                "Enable RDRAND Hardware RNG Hash Seed"          OFF)
option(ENABLE_THREADING             "Enable partial threading support."             OFF)
option(DISABLE_WERROR               "Avoid treating compiler warnings as fatal errors"    OFF)
option(DISABLE_BSYMBOLIC            "Avoid linking with -Bsymbolic-function"   OFF)

if (UNIX OR MINGW OR CYGWIN)
    list(APPEND CMAKE_REQUIRED_DEFINITIONS -D_GNU_SOURCE)
endif()

if (UNIX)
    list(APPEND CMAKE_REQUIRED_LIBRARIES   m)
endif()

if (MSVC)
    list(APPEND CMAKE_REQUIRED_DEFINITIONS /D_CRT_SECURE_NO_DEPRECATE)
    list(APPEND CMAKE_REQUIRED_FLAGS /wd4996)
endif()

check_include_file("fcntl.h"        HAVE_FCNTL_H)
check_include_file("inttypes.h"     HAVE_INTTYPES_H)
check_include_file(stdarg.h         HAVE_STDARG_H)
check_include_file(strings.h        HAVE_STRINGS_H)
check_include_file(string.h         HAVE_STRING_H)
check_include_file(syslog.h         HAVE_SYSLOG_H)


check_include_files("stdlib.h;stdarg.h;string.h;float.h" STDC_HEADERS)

check_include_file(unistd.h         HAVE_UNISTD_H)
check_include_file(sys/types.h      HAVE_SYS_TYPES_H)

check_include_file("dlfcn.h"        HAVE_DLFCN_H)
check_include_file("endian.h"       HAVE_ENDIAN_H)
check_include_file("limits.h"       HAVE_LIMITS_H)
check_include_file("locale.h"       HAVE_LOCALE_H)
check_include_file("memory.h"       HAVE_MEMORY_H)

check_include_file(stdint.h         HAVE_STDINT_H)
check_include_file(stdlib.h         HAVE_STDLIB_H)
check_include_file(sys/cdefs.h      HAVE_SYS_CDEFS_H)
check_include_file(sys/param.h      HAVE_SYS_PARAM_H)
check_include_file(sys/stat.h       HAVE_SYS_STAT_H)
check_include_file(xlocale.h        HAVE_XLOCALE_H)

if (HAVE_INTTYPES_H AND NOT MSVC)
    set(JSON_C_HAVE_INTTYPES_H 1)
endif()

check_symbol_exists(_isnan          "float.h" HAVE_DECL__ISNAN)
check_symbol_exists(_finite         "float.h" HAVE_DECL__FINITE)

if ((MSVC AND NOT (MSVC_VERSION LESS 1800)) OR MINGW OR CYGWIN OR UNIX)
    check_symbol_exists(INFINITY    "math.h" HAVE_DECL_INFINITY)
    check_symbol_exists(isinf       "math.h" HAVE_DECL_ISINF)
    check_symbol_exists(isnan       "math.h" HAVE_DECL_ISNAN)
    check_symbol_exists(nan         "math.h" HAVE_DECL_NAN)
endif()

check_symbol_exists(_doprnt         "stdio.h" HAVE_DOPRNT)
if (UNIX OR MINGW OR CYGWIN)
    check_symbol_exists(snprintf    "stdio.h" HAVE_SNPRINTF)
endif()
check_symbol_exists(vasprintf       "stdio.h" HAVE_VASPRINTF)
check_symbol_exists(vsnprintf       "stdio.h" HAVE_VSNPRINTF)
check_symbol_exists(vprintf         "stdio.h" HAVE_VPRINTF)

if (HAVE_FCNTL_H)
    check_symbol_exists(open        "fcntl.h" HAVE_OPEN)
endif()
if (HAVE_STDLIB_H)
    check_symbol_exists(realloc     "stdlib.h" HAVE_REALLOC)
endif()
if (HAVE_LOCALE_H)
    check_symbol_exists(setlocale   "locale.h" HAVE_SETLOCALE)
    check_symbol_exists(uselocale   "locale.h" HAVE_USELOCALE)
endif()
if (HAVE_STRINGS_H)
    check_symbol_exists(strcasecmp  "strings.h" HAVE_STRCASECMP)
    check_symbol_exists(strncasecmp "strings.h" HAVE_STRNCASECMP)
endif()
if (HAVE_STRING_H)
    check_symbol_exists(strdup      "string.h" HAVE_STRDUP)
    check_symbol_exists(strerror    "string.h" HAVE_STRERROR)
endif()
if (HAVE_SYSLOG_H)
    check_symbol_exists(vsyslog     "syslog.h" HAVE_VSYSLOG)
endif()

if (MSVC)
    check_symbol_exists(strtoll     "stdlib.h" HAVE_STRTOLL)

    set(json_c_strtoll "strtoll")
    if (NOT HAVE_STRTOLL)
        # Use _strtoi64 if strtoll is not available.
        check_symbol_exists(_strtoi64 "stdlib.h" __have_strtoi64)
        if (__have_strtoi64)
            set(HAVE_STRTOLL 1)
            set(json_c_strtoll "_strtoi64")
            # could do the same for strtoull, if needed
        endif()
    endif()
endif()


check_type_size(int                 SIZEOF_INT)
check_type_size(int64_t             SIZEOF_INT64_T)
check_type_size(long                SIZEOF_LONG)
check_type_size("long long"         SIZEOF_LONG_LONG)
check_type_size("size_t"            SIZEOF_SIZE_T)

check_c_source_compiles(
[=[
extern void json_object_get();
__asm__(".section .gnu.json_object_get\\n\\t.ascii \\"Please link against libjson-c instead of libjson\\"\\n\\t.text");
int main(int c, char *v) { return 0;}
]=]
HAS_GNU_WARNING_LONG)

check_c_source_compiles(
  "int main() { int i, x = 0; i = __sync_add_and_fetch(&x,1); return x; }"
  HAVE_ATOMIC_BUILTINS)

check_c_source_compiles(
  "__thread int x = 0; int main() { return 0; }"
  HAVE___THREAD)

if (HAVE___THREAD)
    set(SPEC___THREAD __thread)
elseif (MSVC)
    set(SPEC___THREAD __declspec(thread))
endif()

# Hardware random number is not available on Windows? Says, config.h.win32. Best to preserve compatibility.
if (WIN32)
    set(ENABLE_RDRAND 0)
endif()

# Once we've done basic symbol/header searches let's add them in.
configure_file(${PROJECT_SOURCE_DIR}/cmake/config.h.in        ${PROJECT_BINARY_DIR}/config.h)
message(STATUS "Written ${PROJECT_BINARY_DIR}/config.h")
configure_file(${PROJECT_SOURCE_DIR}/cmake/json_config.h.in   ${PROJECT_BINARY_DIR}/json_config.h)
message(STATUS "Written ${PROJECT_BINARY_DIR}/json_config.h")

if ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} -ffunction-sections -fdata-sections")
	if ("${DISABLE_WERROR}" STREQUAL "OFF")
	    set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} -Werror")
	endif()
    set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} -Wall")
    set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} -Wcast-qual")
    set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} -Wno-error=deprecated-declarations")
    set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} -Wextra")
    set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} -Wwrite-strings")
    set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} -Wno-unused-parameter")

    add_definitions(-D_GNU_SOURCE)
elseif ("${CMAKE_C_COMPILER_ID}" STREQUAL "MSVC")
    set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} /DEBUG")
    set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} /wd4100")
    set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} /wd4996")
    set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} /wd4244")
    set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} /wd4706")
    set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} /wd4702")
    set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} /wd4127")
    set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} /wd4701")
endif()

if (NOT ("${CMAKE_C_COMPILER_ID}" STREQUAL "MSVC"))
	check_c_source_compiles(
	[=[
	/* uClibc toolchains without threading barf when _REENTRANT is defined */
	#define _REENTRANT 1
	#include <sys/types.h>
	int main ()
	{
	  return 0;
	}
	]=]
	REENTRANT_WORKS
	)
	if (REENTRANT_WORKS)
		add_compile_options("-D_REENTRANT")
	endif()

	if (DISABLE_BSYMBOLIC STREQUAL "OFF")
		set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKED_FLAGS} -Wl,-Bsymbolic-functions")
		# XXX need cmake>=3.13 for this:
		#add_link_options("-Wl,-Bsymbolic-functions")
	endif()
endif()

if ($ENV{VALGRIND})
	# Build so that valgrind doesn't complain about linkhash.c
    add_definitions(-DVALGRIND=1)
endif()

set(JSON_C_PUBLIC_HEADERS
    ${PROJECT_BINARY_DIR}/config.h
    ${PROJECT_BINARY_DIR}/json_config.h

    ${PROJECT_SOURCE_DIR}/json.h
    ${PROJECT_SOURCE_DIR}/arraylist.h
    ${PROJECT_SOURCE_DIR}/debug.h
    ${PROJECT_SOURCE_DIR}/json_c_version.h
    ${PROJECT_SOURCE_DIR}/json_inttypes.h
    ${PROJECT_SOURCE_DIR}/json_object.h
    ${PROJECT_SOURCE_DIR}/json_object_iterator.h
    ${PROJECT_SOURCE_DIR}/json_pointer.h
    ${PROJECT_SOURCE_DIR}/json_tokener.h
    ${PROJECT_SOURCE_DIR}/json_util.h
    ${PROJECT_SOURCE_DIR}/json_visit.h
    ${PROJECT_SOURCE_DIR}/linkhash.h
    ${PROJECT_SOURCE_DIR}/printbuf.h
)

set(JSON_C_HEADERS
    ${JSON_C_PUBLIC_HEADERS}
    ${PROJECT_SOURCE_DIR}/json_object_private.h
    ${PROJECT_SOURCE_DIR}/random_seed.h
    ${PROJECT_SOURCE_DIR}/strerror_override.h
    ${PROJECT_SOURCE_DIR}/strerror_override_private.h
    ${PROJECT_SOURCE_DIR}/math_compat.h
    ${PROJECT_SOURCE_DIR}/snprintf_compat.h
    ${PROJECT_SOURCE_DIR}/strdup_compat.h
    ${PROJECT_SOURCE_DIR}/vasprintf_compat.h
)

set(JSON_C_SOURCES
    ${PROJECT_SOURCE_DIR}/arraylist.c
    ${PROJECT_SOURCE_DIR}/debug.c
    ${PROJECT_SOURCE_DIR}/json_c_version.c
    ${PROJECT_SOURCE_DIR}/json_object.c
    ${PROJECT_SOURCE_DIR}/json_object_iterator.c
    ${PROJECT_SOURCE_DIR}/json_pointer.c
    ${PROJECT_SOURCE_DIR}/json_tokener.c
    ${PROJECT_SOURCE_DIR}/json_util.c
    ${PROJECT_SOURCE_DIR}/json_visit.c
    ${PROJECT_SOURCE_DIR}/linkhash.c
    ${PROJECT_SOURCE_DIR}/printbuf.c
    ${PROJECT_SOURCE_DIR}/random_seed.c
    ${PROJECT_SOURCE_DIR}/strerror_override.c
)

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_BINARY_DIR})

# XXX for a normal full distribution we'll need to figure out
# XXX how to build both shared and static libraries.
# Probably leverage that to build a local VALGRIND=1 library for testing too.
add_library(${PROJECT_NAME}
    ${JSON_C_SOURCES}
    ${JSON_C_HEADERS}
)
set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION 4.0.0
    SOVERSION 4)

# If json-c is used as subroject it set to target correct interface -I flags and allow
# to build external target without extra include_directories(...)
target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>
)

install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-targets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT ${PROJECT_NAME}-targets
    FILE ${PROJECT_NAME}-targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

configure_package_config_file(
    "cmake/Config.cmake.in"
    ${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
)

install(
    FILES ${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

if (UNIX OR MINGW OR CYGWIN)
    SET(prefix ${CMAKE_INSTALL_PREFIX})
    # exec_prefix is prefix by default and CMake does not have the
    # concept.
    SET(exec_prefix ${CMAKE_INSTALL_PREFIX})
    SET(libdir ${CMAKE_INSTALL_FULL_LIBDIR})
    SET(includedir ${CMAKE_INSTALL_FULL_INCLUDEDIR})
    SET(VERSION ${PROJECT_VERSION})
    configure_file(json-c.pc.in json-c.pc @ONLY)
    set(INSTALL_PKGCONFIG_DIR "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" CACHE PATH "Installation directory for pkgconfig (.pc) files")
    install(FILES ${PROJECT_BINARY_DIR}/json-c.pc DESTINATION "${INSTALL_PKGCONFIG_DIR}")
endif ()

install(FILES ${JSON_C_PUBLIC_HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/json-c)
