#Licensed under the MIT license. See LICENSE file in the project root for full license information.

cmake_minimum_required(VERSION 2.8.11)
project(json-c)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4100 /wd4996 /wd4244 /wd4706 /wd4702 /wd4127 /wd4701")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4100 /wd4996 /wd4244 /wd4706 /wd4702 /wd4127 /wd4701")
elseif(LINUX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror")
endif()

include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)

file(COPY ./config.h.win32 DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/include/)
file(RENAME ${CMAKE_CURRENT_BINARY_DIR}/include/config.h.win32 ${CMAKE_CURRENT_BINARY_DIR}/include/config.h)

file(COPY ./json_config.h.win32 DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/include/)
file(RENAME ${CMAKE_CURRENT_BINARY_DIR}/include/json_config.h.win32 ${CMAKE_CURRENT_BINARY_DIR}/include/json_config.h)

set(JSON_C_HEADERS
    ./json.h
    ${CMAKE_CURRENT_BINARY_DIR}/include/config.h
    ./json_config.h
    ./arraylist.h
    ./debug.h
    ./json_inttypes.h
    ./json_object.h
    ./json_object_private.h
    ./json_tokener.h
    ./json_util.h
    ./linkhash.h
    ./math_compat.h
    ./printbuf.h
    ./random_seed.h
)

set(JSON_C_SOURCES
    ./arraylist.c
    ./debug.c
    ./json_object.c
    ./json_tokener.c
    ./json_util.c
    ./linkhash.c
    ./printbuf.c
    ./random_seed.c
)

add_library(json-c
    ${JSON_C_SOURCES}
    ${JSON_C_HEADERS}
    )

