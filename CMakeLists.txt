# CMake 3.9 was released in 2017/07
#  As of 2023, many versions of Linux, NetBSD and FreeBSD provide,
#   and many OpenWRT packages require, much newer CMake packages.
#  We're stopping before 3.10 because that version starts requiring
#   c++11, which isn't available on e.g HPUX.
cmake_minimum_required(VERSION 3.9...3.12)

# JSON-C library is C only project.
# PROJECT_VERSION{,_MAJOR,_MINOR,_PATCH} set by project():
project(json-c LANGUAGES C VERSION 0.18.99)

# set default build type if not specified by user
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE debug)
endif()

set(CMAKE_C_FLAGS_RELEASE   "${CMAKE_C_FLAGS_RELEASE} -O2")

include(CTest)

# Set some packaging variables.
set(CPACK_PACKAGE_NAME              "${PROJECT_NAME}")
set(CPACK_PACKAGE_VERSION_MAJOR     "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR     "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH     "${PROJECT_VERSION_PATCH}")
set(JSON_C_BUGREPORT                "json-c@googlegroups.com")
set(CPACK_SOURCE_IGNORE_FILES
        ${PROJECT_SOURCE_DIR}/build
        ${PROJECT_SOURCE_DIR}/cmake-build-debug
        ${PROJECT_SOURCE_DIR}/pack
        ${PROJECT_SOURCE_DIR}/.idea
        ${PROJECT_SOURCE_DIR}/.DS_Store
        ${PROJECT_SOURCE_DIR}/.git
        ${PROJECT_SOURCE_DIR}/.vscode)

include(CheckSymbolExists)
include(CheckIncludeFile)
include(CheckIncludeFiles)
include(CheckCSourceCompiles)
include(CheckTypeSize)
include(CPack)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

option(BUILD_SHARED_LIBS  "Default to building shared libraries" ON)
option(BUILD_STATIC_LIBS  "Default to building static libraries" ON)

# Generate a release merge and test it to verify the correctness of republishing the package.
ADD_CUSTOM_TARGET(distcheck
COMMAND make package_source
    COMMAND tar -xvf "${PROJECT_NAME}-${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}-Source.tar.gz"
    COMMAND mkdir "${PROJECT_NAME}-${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}-Source/build"
    COMMAND cmake "${PROJECT_NAME}-${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}-Source/" -B"./${PROJECT_NAME}-${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}-Source/build/"
    COMMAND make -C "${PROJECT_NAME}-${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}-Source/build"
    COMMAND make test -C "${PROJECT_NAME}-${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}-Source/build"
    COMMAND rm -rf "${PROJECT_NAME}-${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}-Source"
)

# Enable or disable features. By default, all features are turned off.
option(DISABLE_BSYMBOLIC              "Avoid linking with -Bsymbolic-function."               OFF)
option(DISABLE_THREAD_LOCAL_STORAGE   "Disable using Thread-Local Storage (HAVE___THREAD)."   OFF)
option(DISABLE_WERROR                 "Avoid treating compiler warnings as fatal errors."     OFF)
option(ENABLE_RDRAND                  "Enable RDRAND Hardware RNG Hash Seed."                 OFF)
option(ENABLE_THREADING               "Enable partial threading support."                     OFF)
option(OVERRIDE_GET_RANDOM_SEED       "Override json_c_get_random_seed() with custom code."   OFF)
option(DISABLE_EXTRA_LIBS             "Avoid linking against extra libraries, such as libbsd." OFF)
option(DISABLE_JSON_POINTER           "Disable JSON pointer (RFC6901) and JSON patch support." OFF)
option(DISABLE_JSON_PATCH             "Disable JSON patch (RFC6902) support."                 OFF)
option(NEWLOCALE_NEEDS_FREELOCALE     "Work around newlocale bugs in old FreeBSD by calling freelocale"  OFF)
option(BUILD_APPS                     "Default to building apps" ON)

if (AMIGA)
    set(DISABLE_THREAD_LOCAL_STORAGE ON)
    set(ENABLE_THREADING OFF)
    set(BUILD_SHARED_LIBS OFF)
    set(BUILD_APPS OFF)
    set(DISABLE_STATIC_FPIC ON)
    if ($ENV{CROSS_PFX} STREQUAL "m68k-amigaos")
        set(AMIGA_M68K ON)
        set(BUILD_TESTING OFF)
        if (${M68K_CRT} STREQUAL "newlib")
            set(NEWLIB ON)
        elseif(${M68K_CRT} STREQUAL "clib2")
            set(CLIB2 ON)
        elseif(${M68K_CRT} STREQUAL "ixemul")
            set(IXEMUL ON)
        elseif(${M68K_CRT} STREQUAL "nix20")
            set(NIX20 ON)
        elseif(${M68K_CRT} STREQUAL "nix13")
            set(NIX13 ON)
        else()
            set(NEWLIB ON)
        endif()
        message(STATUS "Building for Motorola 68k AmigaOS using CRT: ${M68K_CRT}")
    elseif ($ENV{CROSS_PFX} STREQUAL "ppc-amigaos")
        set(AMIGA_PPC ON)
        message(STATUS "Building for PowerPC AmigaOS")
    elseif($ENV{CROSS_PFX} STREQUAL "ppc-morphos")
        set(MORPHOS ON)
        if (NOIXEMUL)
            message(STATUS "Building for PowerPC MorphOS without ixemul")
            set(DISABLE_WERROR ON)
            set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} -noixemul")
        else()
            message(STATUS "Building for PowerPC MorphOS with ixemul")
        endif()
    else()
        message(FATAL_ERROR "Unsupported AmigaOS target")
    endif()
else()
    message(STATUS "Building for ${CMAKE_SYSTEM_NAME}")
endif()

if (UNIX OR MINGW OR CYGWIN OR AMIGA)
    list(APPEND CMAKE_REQUIRED_DEFINITIONS -D_GNU_SOURCE)
endif()

if (BUILD_SHARED_LIBS)
    add_definitions(-D JSON_C_DLL)
endif()

if (UNIX OR AMIGA)
    list(APPEND CMAKE_REQUIRED_LIBRARIES   m)
endif()

if (MSVC)
    list(APPEND CMAKE_REQUIRED_DEFINITIONS /D_CRT_SECURE_NO_DEPRECATE)
    list(APPEND CMAKE_REQUIRED_FLAGS /wd4996)
endif()

if (NOT DISABLE_STATIC_FPIC)
    # Use '-fPIC'/'-fPIE' option.
    # This will allow other libraries to statically link in libjson-c.a
    # which in turn prevents crashes in downstream apps that may use
    # a different JSON library with identical symbol names.
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
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
check_include_file(sys/resource.h   HAVE_SYS_RESOURCE_H) # for getrusage

check_include_file("dlfcn.h"        HAVE_DLFCN_H)
check_include_file("endian.h"       HAVE_ENDIAN_H)
check_include_file("limits.h"       HAVE_LIMITS_H)
check_include_file("locale.h"       HAVE_LOCALE_H)
check_include_file("memory.h"       HAVE_MEMORY_H)

check_include_file(stdint.h         HAVE_STDINT_H)
check_include_file(stdlib.h         HAVE_STDLIB_H)
check_include_file(sys/cdefs.h      HAVE_SYS_CDEFS_H)
check_include_file(sys/param.h      HAVE_SYS_PARAM_H)
check_include_file(sys/random.h     HAVE_SYS_RANDOM_H)
check_include_file(sys/stat.h       HAVE_SYS_STAT_H)
check_include_file(xlocale.h        HAVE_XLOCALE_H)

# Set json-c specific vars to stamp into json_config.h
# in a way that hopefully won't conflict with other
# projects that use json-c.
if (HAVE_INTTYPES_H)
	set(JSON_C_HAVE_INTTYPES_H 1)
endif()
if (HAVE_STDINT_H)
	set(JSON_C_HAVE_STDINT_H 1)
endif()

check_symbol_exists(_isnan          "float.h" HAVE_DECL__ISNAN)
check_symbol_exists(_finite         "float.h" HAVE_DECL__FINITE)

if ((MSVC AND NOT (MSVC_VERSION LESS 1800)) OR MINGW OR CYGWIN OR UNIX OR AMIGA)
    check_symbol_exists(INFINITY    "math.h" HAVE_DECL_INFINITY)
    check_symbol_exists(isinf       "math.h" HAVE_DECL_ISINF)
    check_symbol_exists(isnan       "math.h" HAVE_DECL_ISNAN)
    check_symbol_exists(nan         "math.h" HAVE_DECL_NAN)
endif()

check_symbol_exists(_doprnt         "stdio.h" HAVE_DOPRNT)
if (UNIX OR MINGW OR CYGWIN OR AMIGA)
    check_symbol_exists(snprintf    "stdio.h" HAVE_SNPRINTF)
endif()
check_symbol_exists(vasprintf       "stdio.h" HAVE_VASPRINTF)
check_symbol_exists(vsnprintf       "stdio.h" HAVE_VSNPRINTF)
check_symbol_exists(vprintf         "stdio.h" HAVE_VPRINTF)
if (NOT AMIGA_M68K)
    check_symbol_exists(arc4random      "stdlib.h" HAVE_ARC4RANDOM)
endif()
if (NOT HAVE_ARC4RANDOM AND DISABLE_EXTRA_LIBS STREQUAL "OFF")
    check_include_file(bsd/stdlib.h HAVE_BSD_STDLIB_H)
    if (HAVE_BSD_STDLIB_H)
        list(APPEND CMAKE_REQUIRED_LIBRARIES "bsd")
        unset(HAVE_ARC4RANDOM CACHE)
        check_symbol_exists(arc4random   "bsd/stdlib.h" HAVE_ARC4RANDOM)
        if (NOT HAVE_ARC4RANDOM)
            list(REMOVE_ITEM CMAKE_REQUIRED_LIBRARIES "bsd")
        endif()
    endif()
endif()

if (HAVE_FCNTL_H)
    check_symbol_exists(open        "fcntl.h" HAVE_OPEN)
endif()
if (HAVE_STDLIB_H)
    check_symbol_exists(realloc     "stdlib.h" HAVE_REALLOC)
endif()
if (HAVE_LOCALE_H)
    check_symbol_exists(setlocale   "locale.h" HAVE_SETLOCALE)
    check_symbol_exists(uselocale   "locale.h" HAVE_USELOCALE)
    if (NOT NIX20 AND NOT NIX13)
        # libnix does not fully support this yet
        check_symbol_exists(duplocale   "locale.h" HAVE_DUPLOCALE)
    endif()
endif()

# uClibc *intentionally* crashes in duplocale(), at least as of:
# https://github.com/ffainelli/uClibc/blob/266bdc1/libc/misc/locale/locale.c#L1322
# So, if it looks like we're compiling for a system like that just disable
# locale handling entirely.
execute_process (COMMAND ${CMAKE_C_COMPILER} -dumpmachine ERROR_QUIET OUTPUT_VARIABLE CMAKE_GNU_C_MACHINE)

if (CMAKE_GNU_C_MACHINE MATCHES "uclibc")
	message(STATUS "Detected uClibc compiler, disabling locale handling")
	set(HAVE_SETLOCALE 0)
	set(HAVE_USELOCALE 0)
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
if (HAVE_SYS_RANDOM_H)
    check_symbol_exists(getrandom   "sys/random.h" HAVE_GETRANDOM)
endif()
if (HAVE_SYS_RESOURCE_H)
    check_symbol_exists(getrusage   "sys/resource.h" HAVE_GETRUSAGE)
endif()

check_symbol_exists(strtoll     "stdlib.h" HAVE_STRTOLL)
check_symbol_exists(strtoull    "stdlib.h" HAVE_STRTOULL)

set(json_c_strtoll "strtoll")
if (NOT HAVE_STRTOLL)
# Use _strtoi64 if strtoll is not available.
check_symbol_exists(_strtoi64 "stdlib.h" __have_strtoi64)
if (__have_strtoi64)
    #set(HAVE_STRTOLL 1)
    set(json_c_strtoll "_strtoi64")
endif()
endif()

set(json_c_strtoull "strtoull")
if (NOT HAVE_STRTOULL)
# Use _strtoui64 if strtoull is not available.
check_symbol_exists(_strtoui64 "stdlib.h" __have_strtoui64)
if (__have_strtoui64)
    #set(HAVE_STRTOULL 1)
    set(json_c_strtoull "_strtoui64")
endif()
endif()


check_type_size(int                 SIZEOF_INT)
check_type_size(int64_t             SIZEOF_INT64_T)
check_type_size(long                SIZEOF_LONG)
check_type_size("long long"         SIZEOF_LONG_LONG)
check_type_size("size_t"            SIZEOF_SIZE_T)
if (MSVC)
list(APPEND CMAKE_EXTRA_INCLUDE_FILES BaseTsd.h)
check_type_size("SSIZE_T"           SIZEOF_SSIZE_T)
else()
check_type_size("ssize_t"           SIZEOF_SSIZE_T)
endif()

check_c_source_compiles(
"
extern void json_object_get();
__asm__(\".section .gnu.json_object_get\\n\\t.ascii \\\"Please link against libjson-c instead of libjson\\\"\\n\\t.text\");
int main(int c, char *v) { return 0;}
"
HAS_GNU_WARNING_LONG)

check_c_source_compiles(
  "int main() { int i, x = 0; i = __sync_add_and_fetch(&x,1); return x; }"
  HAVE_ATOMIC_BUILTINS)

if (NOT DISABLE_THREAD_LOCAL_STORAGE)
  check_c_source_compiles(
    "__thread int x = 0; int main() { return 0; }"
    HAVE___THREAD)

  if (HAVE___THREAD)
      set(SPEC___THREAD __thread)
  elseif (MSVC)
      set(SPEC___THREAD __declspec(thread))
  endif()
endif()

# Hardware random number is not available on Windows? Says, config.h.win32. Best to preserve compatibility.
if (WIN32)
    set(ENABLE_RDRAND 0)
endif()

# Once we've done basic symbol/header searches let's add them in.
configure_file(${PROJECT_SOURCE_DIR}/cmake/config.h.in        ${PROJECT_BINARY_DIR}/config.h)
message(STATUS "Wrote ${PROJECT_BINARY_DIR}/config.h")
configure_file(${PROJECT_SOURCE_DIR}/cmake/json_config.h.in   ${PROJECT_BINARY_DIR}/json_config.h)
message(STATUS "Wrote ${PROJECT_BINARY_DIR}/json_config.h")

if (NOT DEFINED CMAKE_C_COMPILER_FRONTEND_VARIANT OR "${CMAKE_C_COMPILER_FRONTEND_VARIANT}" STREQUAL "")  # only available in cmake 3.14+
	if ("${CMAKE_C_COMPILER_ID}" STREQUAL "MSVC")
		set(CMAKE_C_COMPILER_FRONTEND_VARIANT "MSVC")
	elseif ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
		set(CMAKE_C_COMPILER_FRONTEND_VARIANT "GNU")
	elseif ("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
		# Detect clang-cl.exe, it is Clang but with MSVC compatible command line arguments
		execute_process (COMMAND ${CMAKE_C_COMPILER} -? ERROR_QUIET OUTPUT_QUIET RESULT_VARIABLE _clang_result)
		if (_clang_result EQUAL 0)
			set(CMAKE_C_COMPILER_FRONTEND_VARIANT "MSVC")
		else()
			set(CMAKE_C_COMPILER_FRONTEND_VARIANT "GNU")
			message(STATUS, "clang result is NOT 0")
		endif()
	endif()
endif()

if ("${CMAKE_C_COMPILER_FRONTEND_VARIANT}" STREQUAL "GNU" OR "${CMAKE_C_COMPILER_FRONTEND_VARIANT}" STREQUAL "Clang")
    set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} -ffunction-sections -fdata-sections")
	if ("${DISABLE_WERROR}" STREQUAL "OFF")
	    set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} -Werror")
	endif()
    if (AMIGA_M68K)
        set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} -fbaserel")
    endif()
    set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} -Wall")
    set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} -Wcast-qual")
    set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} -Wno-error=deprecated-declarations")
    set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} -Wextra")
    set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} -Wwrite-strings")
    set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} -Wno-unused-parameter")
    if (NOT WIN32)
        set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} -Wstrict-prototypes")
    endif()

    add_definitions(-D_GNU_SOURCE)

    if ("${CMAKE_C_COMPILER_FRONTEND_VARIANT}" STREQUAL "Clang")
        # Remove this for 1.0 when we can bump the ABI and actually fix these warnings.
        set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} -Wno-shorten-64-to-32")
    endif()
elseif ("${CMAKE_C_COMPILER_FRONTEND_VARIANT}" STREQUAL "MSVC")
    set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} /DEBUG")
    set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} /wd4100")
    set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} /wd4996")
    set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} /wd4244")
    set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} /wd4706")
    set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} /wd4702")
    set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} /wd4127")
    set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} /wd4701")
endif()

if (NOT ("${CMAKE_C_COMPILER_FRONTEND_VARIANT}" STREQUAL "MSVC"))
	check_c_source_compiles(
	"
	/* uClibc toolchains without threading barf when _REENTRANT is defined */
	#define _REENTRANT 1
	#include <sys/types.h>
	int main (void)
	{
	  return 0;
	}
	"
	REENTRANT_WORKS
	)
	if (REENTRANT_WORKS)
		add_compile_options("-D_REENTRANT")
	endif()

	# OSX Mach-O doesn't support linking with '-Bsymbolic-functions'.
	# Others may not support it, too.
	list(APPEND CMAKE_REQUIRED_LIBRARIES "-Wl,-Bsymbolic-functions")
	check_c_source_compiles(
	"
	int main (void)
	{
	  return 0;
	}
	"
	BSYMBOLIC_WORKS
	)
	list(REMOVE_ITEM CMAKE_REQUIRED_LIBRARIES "-Wl,-Bsymbolic-functions")
	if (DISABLE_BSYMBOLIC STREQUAL "OFF" AND BSYMBOLIC_WORKS)
		set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-Bsymbolic-functions")
		# XXX need cmake>=3.13 for this:
		#add_link_options("-Wl,-Bsymbolic-functions")
	endif()

	file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/check-version-script.sym" "TEST { global: *; };")
	list(APPEND CMAKE_REQUIRED_LIBRARIES "-Wl,--version-script,${CMAKE_CURRENT_BINARY_DIR}/check-version-script.sym")
	check_c_source_compiles(
	"
	int main (void)
	{
	  return 0;
	}
	"
	VERSION_SCRIPT_WORKS
	)
	list(REMOVE_ITEM CMAKE_REQUIRED_LIBRARIES "-Wl,--version-script,${CMAKE_CURRENT_BINARY_DIR}/check-version-script.sym")
	if (VERSION_SCRIPT_WORKS)
		set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--version-script,${CMAKE_CURRENT_SOURCE_DIR}/json-c.sym")
	endif()
endif()

if ($ENV{VALGRIND})
	# Build so that valgrind doesn't complain about linkhash.c
    add_definitions(-DVALGRIND=1)
endif()

set(JSON_C_PUBLIC_HEADERS
    # Note: config.h is _not_ included here
    ${PROJECT_BINARY_DIR}/json_config.h

    ${PROJECT_BINARY_DIR}/json.h
    ${PROJECT_SOURCE_DIR}/arraylist.h
    ${PROJECT_SOURCE_DIR}/debug.h
    ${PROJECT_SOURCE_DIR}/json_c_version.h
    ${PROJECT_SOURCE_DIR}/json_inttypes.h
    ${PROJECT_SOURCE_DIR}/json_object.h
    ${PROJECT_SOURCE_DIR}/json_object_iterator.h
    ${PROJECT_SOURCE_DIR}/json_tokener.h
    ${PROJECT_SOURCE_DIR}/json_types.h
    ${PROJECT_SOURCE_DIR}/json_util.h
    ${PROJECT_SOURCE_DIR}/json_visit.h
    ${PROJECT_SOURCE_DIR}/linkhash.h
    ${PROJECT_SOURCE_DIR}/printbuf.h
)

set(JSON_C_HEADERS
    ${JSON_C_PUBLIC_HEADERS}
    ${PROJECT_SOURCE_DIR}/json_object_private.h
    ${PROJECT_SOURCE_DIR}/json_pointer_private.h
    ${PROJECT_SOURCE_DIR}/random_seed.h
    ${PROJECT_SOURCE_DIR}/strerror_override.h
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
    ${PROJECT_SOURCE_DIR}/json_tokener.c
    ${PROJECT_SOURCE_DIR}/json_util.c
    ${PROJECT_SOURCE_DIR}/json_visit.c
    ${PROJECT_SOURCE_DIR}/linkhash.c
    ${PROJECT_SOURCE_DIR}/printbuf.c
    ${PROJECT_SOURCE_DIR}/random_seed.c
    ${PROJECT_SOURCE_DIR}/strerror_override.c
)

if (NOT DISABLE_JSON_POINTER)
    set(JSON_C_PUBLIC_HEADERS   ${JSON_C_PUBLIC_HEADERS}  ${PROJECT_SOURCE_DIR}/json_pointer.h)
    set(JSON_C_SOURCES          ${JSON_C_SOURCES}         ${PROJECT_SOURCE_DIR}/json_pointer.c)
    set(JSON_H_JSON_POINTER "#include \"json_pointer.h\"")

    if (NOT DISABLE_JSON_PATCH)
        set(JSON_C_PUBLIC_HEADERS   ${JSON_C_PUBLIC_HEADERS}  ${PROJECT_SOURCE_DIR}/json_patch.h)
        set(JSON_C_SOURCES          ${JSON_C_SOURCES}         ${PROJECT_SOURCE_DIR}/json_patch.c)
	set(JSON_H_JSON_PATCH "#include \"json_patch.h\"")
    endif()
else()
    set(JSON_H_JSON_POINTER "")
    set(JSON_H_JSON_PATCH "")
endif()

configure_file(json.h.cmakein ${PROJECT_BINARY_DIR}/json.h @ONLY)

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_BINARY_DIR})

add_subdirectory(doc)

# "uninstall" custom target for make generators in unix like operating systems
# and if that target is not present
if (CMAKE_GENERATOR STREQUAL "Unix Makefiles")
    if(NOT TARGET uninstall)
        add_custom_target(uninstall
                COMMAND cat ${PROJECT_BINARY_DIR}/install_manifest.txt | xargs rm
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                )
    endif()
endif()

# XXX for a normal full distribution we'll need to figure out
# XXX how to build both shared and static libraries.
# Probably leverage that to build a local VALGRIND=1 library for testing too.
add_library(${PROJECT_NAME}
    ${JSON_C_SOURCES}
    ${JSON_C_HEADERS}
)
set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION 5.4.0
    SOVERSION 5)
list(APPEND CMAKE_TARGETS ${PROJECT_NAME})
# If json-c is used as subroject it set to target correct interface -I flags and allow
# to build external target without extra include_directories(...)
target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>
)

target_link_libraries(${PROJECT_NAME} PUBLIC ${CMAKE_REQUIRED_LIBRARIES})

# Allow to build static and shared libraries at the same time
if (BUILD_STATIC_LIBS AND BUILD_SHARED_LIBS)
    set(STATIC_LIB ${PROJECT_NAME}-static)
    add_library(${STATIC_LIB} STATIC
        ${JSON_C_SOURCES}
        ${JSON_C_HEADERS}
    )
    target_include_directories(${PROJECT_NAME}-static
        PUBLIC
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
            $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>
    )

	target_link_libraries(${PROJECT_NAME}-static PUBLIC ${CMAKE_REQUIRED_LIBRARIES})

    # rename the static library
    if (NOT MSVC)
    set_target_properties(${STATIC_LIB} PROPERTIES
        OUTPUT_NAME ${PROJECT_NAME}
    )
    endif()
    list(APPEND CMAKE_TARGETS ${STATIC_LIB})
endif ()

# Always create new install dirs with 0755 permissions, regardless of umask
set(CMAKE_INSTALL_DEFAULT_DIRECTORY_PERMISSIONS
	OWNER_READ
	OWNER_WRITE
	OWNER_EXECUTE
	GROUP_READ
	GROUP_EXECUTE
	WORLD_READ
	WORLD_EXECUTE
   )

install(TARGETS ${CMAKE_TARGETS}
    EXPORT ${PROJECT_NAME}-targets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} ${CMAKE_INSTALL_INCLUDEDIR}/json-c
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

    SET(prefix ${CMAKE_INSTALL_PREFIX})
    # exec_prefix is prefix by default and CMake does not have the
    # concept.
    SET(exec_prefix ${CMAKE_INSTALL_PREFIX})
    SET(libdir ${CMAKE_INSTALL_FULL_LIBDIR})
    SET(includedir ${CMAKE_INSTALL_FULL_INCLUDEDIR})
    SET(VERSION ${PROJECT_VERSION})

	# Linking against the static json-c requires
	# dependent packages to include additional libs:
	SET(LIBS_LIST ${CMAKE_REQUIRED_LIBRARIES})

	# Note: We would need cmake >= 3.12 in order to use list(TRANSFORM ...)
	function(list_transform_prepend var prefix)
		set(temp "")
		foreach(f ${${var}})
			list(APPEND temp "${prefix}${f}")
		endforeach()
		set(${var} "${temp}" PARENT_SCOPE)
	endfunction()
	list_transform_prepend(LIBS_LIST "-l")

	string(REPLACE ";" " " LIBS "${LIBS_LIST}")

    configure_file(json-c.pc.in json-c.pc @ONLY)
    set(INSTALL_PKGCONFIG_DIR "${CMAKE_INSTALL_LIBDIR}/pkgconfig" CACHE PATH "Installation directory for pkgconfig (.pc) files")
    install(FILES ${PROJECT_BINARY_DIR}/json-c.pc DESTINATION "${INSTALL_PKGCONFIG_DIR}")

install(FILES ${JSON_C_PUBLIC_HEADERS} DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/json-c)

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING AND
   (NOT MSVC OR NOT (MSVC_VERSION LESS 1800)) # Tests need at least VS2013
   )
add_subdirectory(tests)
endif()

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_APPS)
# skip apps when we're included in someone else's build
if (NOT MSVC)  # cmd line apps don't built on Windows currently.
add_subdirectory(apps)
endif()
endif()
