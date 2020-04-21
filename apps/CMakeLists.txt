
# Note: it is intentional that there are no install instructions here yet.
# When/if the interface of the app(s) listed here settles down enough to
# publish as part of a regular build that will be added.

add_executable(json_parse json_parse.c)
#target_compile_definitions(json_parse PRIVATE TEST_FORMATTED=1)
target_link_libraries(json_parse PRIVATE ${PROJECT_NAME})

include_directories(PUBLIC ${CMAKE_SOURCE_DIR})

