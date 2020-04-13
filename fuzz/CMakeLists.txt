# https://cmake.org/cmake/help/v3.0/command/add_test.html
# https://pabloariasal.github.io/2018/02/19/its-time-to-do-cmake-right/
enable_language(CXX)

include_directories(PUBLIC ${CMAKE_SOURCE_DIR})

foreach(FUZZERNAME
	tokener_parse_ex_fuzzer)

	add_executable(${FUZZERNAME}
		${FUZZERNAME}.cc)

	target_include_directories(${FUZZERNAME}
		PUBLIC
		${PROJECT_BINARY_DIR})

	set_target_properties(${FUZZERNAME} PROPERTIES CXX_STANDARD 11)

	# define whether we want to use the standalone runner for our own testing or
	# link against libFuzzer populated from env variables
	if (NOT DEFINED $ENV{LIB_FUZZING_ENGINE})
		add_library(standalone_runner standalone_runner.cc)
		target_link_libraries(${FUZZERNAME}
			json-c
			standalone_runner)
	else()
		target_link_libraries(${FUZZERNAME}
			json-c
			${LIB_FUZZING_ENGINE})
	endif()
endforeach(FUZZERNAME)
