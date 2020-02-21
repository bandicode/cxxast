
enable_testing()

add_executable(tests "main.cpp")
add_dependencies(tests cxxast)
target_include_directories(tests PUBLIC "../include")
target_link_libraries(tests cxxast)

set_target_properties(tests PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set_target_properties(tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

add_test(NAME basic_test COMMAND tests)