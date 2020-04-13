# https://cmake.org/cmake/help/v3.0/command/add_test.html
# https://pabloariasal.github.io/2018/02/19/its-time-to-do-cmake-right/
enable_language(CXX)

include_directories(PUBLIC ${CMAKE_SOURCE_DIR})

foreach(FUZZERNAME
	tokener_parse_ex_fuzzer)

add_executable(${FUZZERNAME} ${FUZZERNAME}.cc standalone_runner.cc)

target_include_directories(${FUZZERNAME} PUBLIC ${PROJECT_BINARY_DIR})

target_link_libraries(${FUZZERNAME} PRIVATE json-c)

endforeach(FUZZERNAME)
