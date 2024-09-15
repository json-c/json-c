# generate doxygen documentation for json-c API

find_package(Doxygen)

if (DOXYGEN_FOUND)

	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
	  ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
	message(STATUS "Wrote ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile")

	add_custom_target(doc
	COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

else (DOXYGEN_FOUND)
	message("Warning: doxygen not found, the 'doc' target will not be included")
endif(DOXYGEN_FOUND)
