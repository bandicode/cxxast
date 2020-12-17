
enable_testing()

if(NOT DEFINED CACHE{BUILD_CXXAST_TESTS})
  set(BUILD_CXXAST_TESTS ON CACHE BOOL "whether to build cxxast test")
endif()

if(BUILD_CXXAST_TESTS)

  if(NOT DEFINED CATCH2_SINGLE_HEADER_FILE OR NOT CATCH2_SINGLE_HEADER_FILE)

    message("Downloading Catch2")
    file(DOWNLOAD https://github.com/catchorg/Catch2/releases/download/v2.11.3/catch.hpp "${CMAKE_CURRENT_LIST_DIR}/catch.hpp" SHOW_PROGRESS)
  
    set(CATCH2_SINGLE_HEADER_FILE "${CMAKE_CURRENT_LIST_DIR}/catch.hpp" CACHE STRING "Path of catch.hpp")

  endif()

  add_executable(TEST_cxxast "main.cpp" "tests-api.cpp" "tests-cxx-restricted-parser.cpp" "tests-cxx-libclang-parser.cpp" ${CATCH2_SINGLE_HEADER_FILE})
  add_dependencies(TEST_cxxast cxxast)
  target_include_directories(TEST_cxxast PUBLIC "../include")
  target_link_libraries(TEST_cxxast cxxast)

  set_target_properties(TEST_cxxast PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
  set_target_properties(TEST_cxxast PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

  add_test(NAME TEST_cxxast COMMAND TEST_cxxast)

endif()
