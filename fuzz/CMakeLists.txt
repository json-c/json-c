# https://cmake.org/cmake/help/v3.0/command/add_test.html
# https://pabloariasal.github.io/2018/02/19/its-time-to-do-cmake-right/
enable_language(CXX)
set (CMAKE_CXX_STANDARD 11)

include_directories(PUBLIC ${CMAKE_SOURCE_DIR})
include_directories(PUBLIC ${PROJECT_BINARY_DIR})

# This sets up either the standalone runner or the user supplied libfuzzing
# engine. We're using the standalone runner for unit testing alone at the
# moment to make the sure the fuzzer builds and runs appropriately.
if(DEFINED ENV{LIB_FUZZING_ENGINE})
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} $ENV{LIB_FUZZING_ENGINE}")
        set(FUZZING_ENGINE "")
else()
        add_library(fuzzing_engine standalone_runner.cc)
        set(FUZZING_ENGINE fuzzing_engine)
endif()

foreach(FUZZERNAME
	tokener_parse_ex_fuzzer)

	add_executable(${FUZZERNAME} ${FUZZERNAME}.cc)
	target_link_libraries(${FUZZERNAME}
		json-c
		${FUZZING_ENGINE})

	file(GLOB TESTFILES "${CMAKE_SOURCE_DIR}/fuzz/tests/*.json")
	foreach(TESTFILE ${TESTFILES})
		get_filename_component(FILENAME ${TESTFILE} NAME_WE)
		add_test(NAME test_${FUZZERNAME}_${FILENAME}
			COMMAND ${CMAKE_BINARY_DIR}/fuzz/${FUZZERNAME}
			${TESTFILE})

	endforeach(TESTFILE)
endforeach(FUZZERNAME)
