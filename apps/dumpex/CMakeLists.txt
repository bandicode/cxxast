
set(CXXAST_BUILD_ASTDUMPER FALSE CACHE BOOL "Check if you want to build the ast-dumper app")

if(CXXAST_BUILD_ASTDUMPER)

  add_executable(cxxast-dump-ex "main.cpp")

  target_link_libraries(cxxast-dump-ex cxxast)

  set_target_properties(cxxast-dump-ex PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
  set_target_properties(cxxast-dump-ex PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

endif()
