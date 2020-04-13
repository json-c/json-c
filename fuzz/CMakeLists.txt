# https://cmake.org/cmake/help/v3.0/command/add_test.html
# https://pabloariasal.github.io/2018/02/19/its-time-to-do-cmake-right/
enable_language(CXX)

include_directories(PUBLIC ${CMAKE_SOURCE_DIR})
include_directories(PUBLIC ${PROJECT_BINARY_DIR})

if(DEFINED ENV{LIB_FUZZING_ENGINE})
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} $ENV{LIB_FUZZING_ENGINE}")
        set(fuzzing_engine "")
else()
        add_library(fuzzing_engine standalone_runner.cc)
        set(fuzzing_engine fuzzing_engine)
endif()

foreach(FUZZERNAME
	tokener_parse_ex_fuzzer)

	add_executable(${FUZZERNAME} ${FUZZERNAME}.cc)
	target_link_libraries(${FUZZERNAME}
		json-c
		${fuzzing_engine})

endforeach(FUZZERNAME)
