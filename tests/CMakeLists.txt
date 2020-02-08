
enable_testing()

add_executable(tests "main.cpp")
add_dependencies(tests cxxast)
target_include_directories(tests PUBLIC "../include")
target_link_libraries(tests cxxast)

add_test(NAME basic_test COMMAND tests)