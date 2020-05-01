
enable_testing()

if(NOT DEFINED CACHE{BUILD_CXXAST_TESTS})
  set(BUILD_CXXAST_TESTS ON CACHE BOOL "whether to build cxxast test")
endif()

if(BUILD_CXXAST_TESTS)

  add_executable(TEST_cxxast "main.cpp")
  add_dependencies(TEST_cxxast cxxast)
  target_include_directories(TEST_cxxast PUBLIC "../include")
  target_link_libraries(TEST_cxxast cxxast)

  set_target_properties(TEST_cxxast PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
  set_target_properties(TEST_cxxast PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

  add_test(NAME TEST_cxxast COMMAND TEST_cxxast)

endif()
