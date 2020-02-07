#Licensed under the MIT license. See LICENSE file in the project root for full license information.

cmake_minimum_required(VERSION 2.8.7)
cmake_policy(SET CMP0048 NEW)
project(json-c VERSION 0.13.1)

if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  conan_basic_setup()
endif()

include(CheckSymbolExists)

check_symbol_exists(strtoll "stdlib.h" HAVE_STRTOLL)

set(cmake_strtoll "strtoll")
if (NOT HAVE_STRTOLL)
  # Use _strtoi64 if strtoll is not available.
  check_symbol_exists(_strtoi64 stdlib.h have_strtoi64)
  if (have_strtoi64)
    set(HAVE_STRTOLL 1)
    set(cmake_strtoll "_strtoi64")
    # could do the same for strtoull, if needed
  endif ()
endif ()



if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4100 /wd4996 /wd4244 /wd4706 /wd4702 /wd4127 /wd4701")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4100 /wd4996 /wd4244 /wd4706 /wd4702 /wd4127 /wd4701")
  set(cmake_create_config 1)
elseif(MINGW)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -D_GNU_SOURCE=1")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -D_GNU_SOURCE=1")
  if (MSYS OR CMAKE_GENERATOR STREQUAL "Unix Makefiles")
    execute_process(COMMAND echo ${CMAKE_CURRENT_SOURCE_DIR} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    execute_process(COMMAND sh autogen.sh WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    execute_process(COMMAND sh ./configure WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    file(COPY ./config.h DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/include/)
    file(COPY ./json_config.h DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/include/)
  else()
    set(cmake_create_config 1)
  endif()
elseif(UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -D_GNU_SOURCE")
  execute_process(COMMAND echo ${CMAKE_CURRENT_SOURCE_DIR} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  execute_process(COMMAND sh autogen.sh WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  execute_process(COMMAND ./configure WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  file(COPY ./config.h DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/include/)
  file(COPY ./json_config.h DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/include/)
endif()

if (cmake_create_config)
  file(REMOVE ./config.h)  # make sure any stale one is gone
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.win32 ${CMAKE_CURRENT_BINARY_DIR}/include/config.h)
  file(COPY ./json_config.h.win32 DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/include/)
  file(RENAME ${CMAKE_CURRENT_BINARY_DIR}/include/json_config.h.win32 ${CMAKE_CURRENT_BINARY_DIR}/include/json_config.h)
endif ()

include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)

set(JSON_C_PUBLIC_HEADERS
    ./json.h
    ${CMAKE_CURRENT_BINARY_DIR}/include/config.h
    ${CMAKE_CURRENT_BINARY_DIR}/include/json_config.h
    ./arraylist.h
    ./debug.h
    ./json_c_version.h
    ./json_inttypes.h
    ./json_object.h
    ./json_object_iterator.h
    ./json_pointer.h
    ./json_tokener.h
    ./json_util.h
    ./json_visit.h
    ./linkhash.h
    ./printbuf.h
)
set(JSON_C_HEADERS
    ${JSON_C_PUBLIC_HEADERS}
    ./json_object_private.h
    ./random_seed.h
    ./strerror_override.h
    ./strerror_override_private.h
    ./math_compat.h
    ./snprintf_compat.h
    ./strdup_compat.h
    ./vasprintf_compat.h
)

set(JSON_C_SOURCES
    ./arraylist.c
    ./debug.c
    ./json_c_version.c
    ./json_object.c
    ./json_object_iterator.c
    ./json_pointer.c
    ./json_tokener.c
    ./json_util.c
    ./json_visit.c
    ./linkhash.c
    ./printbuf.c
    ./random_seed.c
    ./strerror_override.c
)

add_library(json-c
    SHARED
    ${JSON_C_SOURCES}
    ${JSON_C_HEADERS}
)

add_library(json-c-static
    STATIC
    ${JSON_C_SOURCES}
    ${JSON_C_HEADERS}
)

set_property(TARGET json-c PROPERTY C_STANDARD 99)
set_property(TARGET json-c-static PROPERTY C_STANDARD 99)
set_target_properties(json-c-static PROPERTIES OUTPUT_NAME json-c)

# -fPIC is implicit in SHARED libraries, see POSITION_INDEPENDANT_CODE property in cmake documentation
set_target_properties(json-c-static PROPERTIES COMPILE_FLAGS "-fPIC")

install(TARGETS json-c json-c-static
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(FILES ${JSON_C_PUBLIC_HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/json-c )

if (UNIX)
	set(prefix ${CMAKE_INSTALL_PREFIX})
	set(exec_prefix ${CMAKE_INSTALL_PREFIX}/bin)
	set(libdir ${CMAKE_INSTALL_PREFIX}/lib)
	set(includedir ${CMAKE_INSTALL_PREFIX}/include)
	set(VERSION ${PROJECT_VERSION})
	configure_file(json-c.pc.in json-c.pc @ONLY)
	set(INSTALL_PKGCONFIG_DIR "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" CACHE PATH "Installation directory for pkgconfig (.pc) files")
	install(FILES ${CMAKE_BINARY_DIR}/json-c.pc DESTINATION "${INSTALL_PKGCONFIG_DIR}")
endif ()

# ------------------------- Begin Generic CMake Variable Logging ------------------

# if you are building in-source, this is the same as CMAKE_SOURCE_DIR, otherwise
# this is the top level directory of your build tree
MESSAGE( STATUS "CMAKE_BINARY_DIR:         " ${CMAKE_BINARY_DIR} )

# if you are building in-source, this is the same as CMAKE_CURRENT_SOURCE_DIR, otherwise this
# is the directory where the compiled or generated files from the current CMakeLists.txt will go to
MESSAGE( STATUS "CMAKE_CURRENT_BINARY_DIR: " ${CMAKE_CURRENT_BINARY_DIR} )

# this is the directory, from which cmake was started, i.e. the top level source directory
MESSAGE( STATUS "CMAKE_SOURCE_DIR:         " ${CMAKE_SOURCE_DIR} )

# this is the directory where the currently processed CMakeLists.txt is located in
MESSAGE( STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR} )

# contains the full path to the top level directory of your build tree
MESSAGE( STATUS "PROJECT_BINARY_DIR: " ${PROJECT_BINARY_DIR} )

# contains the full path to the root of your project source directory,
# i.e. to the nearest directory where CMakeLists.txt contains the PROJECT() command
MESSAGE( STATUS "PROJECT_SOURCE_DIR: " ${PROJECT_SOURCE_DIR} )

# set this variable to specify a common place where CMake should put all executable files
# (instead of CMAKE_CURRENT_BINARY_DIR)
MESSAGE( STATUS "EXECUTABLE_OUTPUT_PATH: " ${EXECUTABLE_OUTPUT_PATH} )

# set this variable to specify a common place where CMake should put all libraries
# (instead of CMAKE_CURRENT_BINARY_DIR)
MESSAGE( STATUS "LIBRARY_OUTPUT_PATH:     " ${LIBRARY_OUTPUT_PATH} )

# tell CMake to search first in directories listed in CMAKE_MODULE_PATH
# when you use FIND_PACKAGE() or INCLUDE()
MESSAGE( STATUS "CMAKE_MODULE_PATH: " ${CMAKE_MODULE_PATH} )

# this is the complete path of the cmake which runs currently (e.g. /usr/local/bin/cmake)
MESSAGE( STATUS "CMAKE_COMMAND: " ${CMAKE_COMMAND} )

# this is the CMake installation directory
MESSAGE( STATUS "CMAKE_ROOT: " ${CMAKE_ROOT} )

# this is the filename including the complete path of the file where this variable is used.
MESSAGE( STATUS "CMAKE_CURRENT_LIST_FILE: " ${CMAKE_CURRENT_LIST_FILE} )

# this is linenumber where the variable is used
MESSAGE( STATUS "CMAKE_CURRENT_LIST_LINE: " ${CMAKE_CURRENT_LIST_LINE} )

# this is used when searching for include files e.g. using the FIND_PATH() command.
MESSAGE( STATUS "CMAKE_INCLUDE_PATH: " ${CMAKE_INCLUDE_PATH} )

# this is used when searching for libraries e.g. using the FIND_LIBRARY() command.
MESSAGE( STATUS "CMAKE_LIBRARY_PATH: " ${CMAKE_LIBRARY_PATH} )

# the complete system name, e.g. "Linux-2.4.22", "FreeBSD-5.4-RELEASE" or "Windows 5.1"
MESSAGE( STATUS "CMAKE_SYSTEM: " ${CMAKE_SYSTEM} )

# the short system name, e.g. "Linux", "FreeBSD" or "Windows"
MESSAGE( STATUS "CMAKE_SYSTEM_NAME: " ${CMAKE_SYSTEM_NAME} )

# only the version part of CMAKE_SYSTEM
MESSAGE( STATUS "CMAKE_SYSTEM_VERSION: " ${CMAKE_SYSTEM_VERSION} )

# the processor name (e.g. "Intel(R) Pentium(R) M processor 2.00GHz")
MESSAGE( STATUS "CMAKE_SYSTEM_PROCESSOR: " ${CMAKE_SYSTEM_PROCESSOR} )

# is TRUE on all UNIX-like OS's, including Apple OS X and CygWin
MESSAGE( STATUS "UNIX: " ${UNIX} )

# is TRUE on Windows, including CygWin
MESSAGE( STATUS "WIN32: " ${WIN32} )

# is TRUE on Apple OS X
MESSAGE( STATUS "APPLE: " ${APPLE} )

# is TRUE when using the MinGW compiler in Windows
MESSAGE( STATUS "MINGW: " ${MINGW} )

# is TRUE on Windows when using the CygWin version of cmake
MESSAGE( STATUS "CYGWIN: " ${CYGWIN} )

# is TRUE on Windows when using a Borland compiler
MESSAGE( STATUS "BORLAND: " ${BORLAND} )

# Microsoft compiler
MESSAGE( STATUS "MSVC: " ${MSVC} )
MESSAGE( STATUS "MSVC_IDE: " ${MSVC_IDE} )
MESSAGE( STATUS "MSVC60: " ${MSVC60} )
MESSAGE( STATUS "MSVC70: " ${MSVC70} )
MESSAGE( STATUS "MSVC71: " ${MSVC71} )
MESSAGE( STATUS "MSVC80: " ${MSVC80} )
MESSAGE( STATUS "CMAKE_COMPILER_2005: " ${CMAKE_COMPILER_2005} )


# set this to true if you don't want to rebuild the object files if the rules have changed,
# but not the actual source files or headers (e.g. if you changed the some compiler switches)
MESSAGE( STATUS "CMAKE_SKIP_RULE_DEPENDENCY: " ${CMAKE_SKIP_RULE_DEPENDENCY} )

# since CMake 2.1 the install rule depends on all, i.e. everything will be built before installing.
# If you don't like this, set this one to true.
MESSAGE( STATUS "CMAKE_SKIP_INSTALL_ALL_DEPENDENCY: " ${CMAKE_SKIP_INSTALL_ALL_DEPENDENCY} )

# If set, runtime paths are not added when using shared libraries. Default it is set to OFF
MESSAGE( STATUS "CMAKE_SKIP_RPATH: " ${CMAKE_SKIP_RPATH} )

# set this to true if you are using makefiles and want to see the full compile and link
# commands instead of only the shortened ones
MESSAGE( STATUS "CMAKE_VERBOSE_MAKEFILE: " ${CMAKE_VERBOSE_MAKEFILE} )

# this will cause CMake to not put in the rules that re-run CMake. This might be useful if
# you want to use the generated build files on another machine.
MESSAGE( STATUS "CMAKE_SUPPRESS_REGENERATION: " ${CMAKE_SUPPRESS_REGENERATION} )


# A simple way to get switches to the compiler is to use ADD_DEFINITIONS().
# But there are also two variables exactly for this purpose:

# the compiler flags for compiling C sources
MESSAGE( STATUS "CMAKE_C_FLAGS: " ${CMAKE_C_FLAGS} )

# the compiler flags for compiling C++ sources
MESSAGE( STATUS "CMAKE_CXX_FLAGS: " ${CMAKE_CXX_FLAGS} )


# Choose the type of build.  Example: SET(CMAKE_BUILD_TYPE Debug)
MESSAGE( STATUS "CMAKE_BUILD_TYPE: " ${CMAKE_BUILD_TYPE} )

# if this is set to ON, then all libraries are built as shared libraries by default.
MESSAGE( STATUS "BUILD_SHARED_LIBS: " ${BUILD_SHARED_LIBS} )

# the compiler used for C files
MESSAGE( STATUS "CMAKE_C_COMPILER: " ${CMAKE_C_COMPILER} )

# the compiler used for C++ files
MESSAGE( STATUS "CMAKE_CXX_COMPILER: " ${CMAKE_CXX_COMPILER} )

# if the compiler is a variant of gcc, this should be set to 1
MESSAGE( STATUS "CMAKE_COMPILER_IS_GNUCC: " ${CMAKE_COMPILER_IS_GNUCC} )

# if the compiler is a variant of g++, this should be set to 1
MESSAGE( STATUS "CMAKE_COMPILER_IS_GNUCXX : " ${CMAKE_COMPILER_IS_GNUCXX} )

# the tools for creating libraries
MESSAGE( STATUS "CMAKE_AR: " ${CMAKE_AR} )
MESSAGE( STATUS "CMAKE_RANLIB: " ${CMAKE_RANLIB} )

#
#MESSAGE( STATUS ": " ${} )

# ------------------------- End of Generic CMake Variable Logging ------------------
