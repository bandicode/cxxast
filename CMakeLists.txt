cmake_minimum_required(VERSION 3.9)
project(cxxast)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  # disables warning about DLL export 
  # this is annoying when using C++ STL
  add_compile_options(/wd4251)  
endif()

##################################################################
###### dependencies
##################################################################

add_subdirectory(dependencies)

##################################################################
###### cxxast library
##################################################################

file(GLOB_RECURSE CXXAST_LIBRARY_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE CXXAST_LIBRARY_HDR_FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/cxx/*.h)

list(FILTER CXXAST_LIBRARY_SRC_FILES EXCLUDE REGEX "main.cpp")

add_library(cxxast SHARED ${CXXAST_LIBRARY_HDR_FILES} ${CXXAST_LIBRARY_SRC_FILES})
target_compile_definitions(cxxast PRIVATE -DCXXAST_BUILD_LIB)
target_include_directories(cxxast PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(cxxast PUBLIC "${DYNLIB_PROJECT_DIR}/include")
target_link_libraries(cxxast dynlib)

if(NOT WIN32)
  target_link_libraries(cxxast ${CMAKE_DL_LIBS})
endif()

foreach(_source IN ITEMS ${CXXAST_LIBRARY_HDR_FILES} ${CXXAST_LIBRARY_SRC_FILES})
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
    source_group("${_group_path}" FILES "${_source}")
endforeach()

set_target_properties(cxxast PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set_target_properties(cxxast PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

##################################################################
###### apps & tests
##################################################################

add_subdirectory(apps)
add_subdirectory(tests)
