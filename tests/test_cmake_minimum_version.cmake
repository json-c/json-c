# CMake 4.1 deprecates projects that require CMake versions below 3.10.
# Keep every top-level CMake entry point above that boundary while preserving
# the existing 3.12 policy ceiling.
set(cmake_files
    "${CMAKE_CURRENT_LIST_DIR}/../CMakeLists.txt"
    "${CMAKE_CURRENT_LIST_DIR}/../apps/CMakeLists.txt"
    "${CMAKE_CURRENT_LIST_DIR}/../tests/CMakeLists.txt")

foreach(cmake_file IN LISTS cmake_files)
    file(RELATIVE_PATH relative_file "${CMAKE_CURRENT_LIST_DIR}/.." "${cmake_file}")
    file(READ "${cmake_file}" contents)
    string(REGEX MATCH
        "cmake_minimum_required\\(VERSION[ \t]+([0-9]+\\.[0-9]+)\\.\\.\\.([0-9]+\\.[0-9]+)\\)"
        declaration "${contents}")
    if(NOT declaration)
        message(FATAL_ERROR "${relative_file}: missing a ranged cmake_minimum_required declaration")
    endif()
    if(CMAKE_MATCH_1 VERSION_LESS "3.10")
        message(FATAL_ERROR
            "${relative_file}: minimum CMake version ${CMAKE_MATCH_1} must be at least 3.10")
    endif()
    if(CMAKE_MATCH_2 VERSION_LESS "3.12")
        message(FATAL_ERROR
            "${relative_file}: policy ceiling ${CMAKE_MATCH_2} must preserve 3.12")
    endif()
    message(STATUS "${relative_file}: ${CMAKE_MATCH_1}...${CMAKE_MATCH_2}")
endforeach()
