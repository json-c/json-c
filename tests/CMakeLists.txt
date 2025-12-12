cmake_minimum_required(VERSION 3.9)
add_executable(test1Formatted test1.c parse_flags.c parse_flags.h)
target_compile_definitions(test1Formatted PRIVATE TEST_FORMATTED=1)
target_link_libraries(test1Formatted PRIVATE ${PROJECT_NAME})

add_executable(test2Formatted test2.c parse_flags.c parse_flags.h)
target_compile_definitions(test2Formatted PRIVATE TEST_FORMATTED=1)
target_link_libraries(test2Formatted PRIVATE ${PROJECT_NAME})

# https://cmake.org/cmake/help/v3.0/command/add_test.html
# https://pabloariasal.github.io/2018/02/19/its-time-to-do-cmake-right/

include_directories(PUBLIC ${CMAKE_SOURCE_DIR})

set(ALL_TEST_NAMES
    test1
    test2
    test4
    testReplaceExisting
    test_cast
    test_charcase
    test_compare
    test_deep_copy
    test_double_serializer
    test_float
    test_int_add
    test_int_get
    test_locale
    test_null
    test_parse
    test_parse_int64
    test_printbuf
    test_set_serializer
    test_set_value
    test_strerror
    test_util_file
    test_visit
    test_object_iterator)

if (NOT DISABLE_JSON_POINTER)
    set(ALL_TEST_NAMES ${ALL_TEST_NAMES} test_json_pointer)
    if (NOT DISABLE_JSON_PATCH)
        set(ALL_TEST_NAMES ${ALL_TEST_NAMES} test_json_patch)
    endif()
endif()

foreach(TESTNAME ${ALL_TEST_NAMES})

add_executable(${TESTNAME} ${TESTNAME}.c)
add_test(NAME ${TESTNAME} COMMAND ${PROJECT_SOURCE_DIR}/tests/${TESTNAME}.test)

# XXX using the non-target_ versions of these doesn't work :(
target_include_directories(
  ${TESTNAME}
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
  )
target_link_libraries(
  ${TESTNAME}
  PRIVATE
    ${PROJECT_NAME}
  )

endforeach(TESTNAME)
