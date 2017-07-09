#Licensed under the MIT license. See LICENSE file in the project root for full license information.

cmake_minimum_required(VERSION 2.8.7)
project(json-c)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4100 /wd4996 /wd4244 /wd4706 /wd4702 /wd4127 /wd4701")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4100 /wd4996 /wd4244 /wd4706 /wd4702 /wd4127 /wd4701")
  file(COPY ./config.h.win32 DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/include/)
  file(RENAME ${CMAKE_CURRENT_BINARY_DIR}/include/config.h.win32 ${CMAKE_CURRENT_BINARY_DIR}/include/config.h)
  file(COPY ./json_config.h.win32 DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/include/)
  file(RENAME ${CMAKE_CURRENT_BINARY_DIR}/include/json_config.h.win32 ${CMAKE_CURRENT_BINARY_DIR}/include/json_config.h)
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
    file(COPY ./config.h.win32 DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/include/)
    file(RENAME ${CMAKE_CURRENT_BINARY_DIR}/include/config.h.win32 ${CMAKE_CURRENT_BINARY_DIR}/include/config.h)
    file(COPY ./json_config.h.win32 DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/include/)
    file(RENAME ${CMAKE_CURRENT_BINARY_DIR}/include/json_config.h.win32 ${CMAKE_CURRENT_BINARY_DIR}/include/json_config.h)
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

include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)

set(JSON_C_HEADERS
    ./json.h
    ${CMAKE_CURRENT_BINARY_DIR}/include/config.h
    ./json_config.h
    ./arraylist.h
    ./debug.h
    ./json_inttypes.h
    ./json_object.h
    ./json_object_private.h
    ./json_pointer.h
    ./json_tokener.h
    ./json_util.h
    ./linkhash.h
    ./math_compat.h
    ./strdup_compat.h
    ./vasprintf_compat.h
    ./printbuf.h
    ./random_seed.h
)

set(JSON_C_SOURCES
    ./arraylist.c
    ./debug.c
    ./json_object.c
    ./json_pointer.c
    ./json_tokener.c
    ./json_util.c
    ./linkhash.c
    ./printbuf.c
    ./random_seed.c
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

install(TARGETS json-c json-c-static
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY . DESTINATION ${CMAKE_INSTALL_PREFIX}/include/json-c FILES_MATCHING PATTERN "*.h")
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include/json-c FILES_MATCHING PATTERN "*.h")
