cmake_minimum_required(VERSION 3.10)
set(PROJECT_NAME "disk_space_2")
project(${PROJECT_NAME} LANGUAGES CXX)

set(PLUGIN_NAME "${PROJECT_NAME}_plugin")
list(APPEND PLUGIN_SOURCES
  "disk_space_2_plugin.cpp"
  "disk_space_2_plugin.h"
)

add_library(${PLUGIN_NAME} SHARED
  "include/disk_space_2/disk_space_2_plugin.h"
  "disk_space_2_plugin_c_api.cpp"
  ${PLUGIN_SOURCES}
)
apply_standard_settings(${PLUGIN_NAME})
set_target_properties(${PLUGIN_NAME} PROPERTIES
  CXX_VISIBILITY_PRESET hidden)
target_compile_definitions(${PLUGIN_NAME} PRIVATE FLUTTER_PLUGIN_IMPL)
target_compile_definitions(${PLUGIN_NAME} PRIVATE _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)

target_include_directories(${PLUGIN_NAME} INTERFACE
  "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(${PLUGIN_NAME} PRIVATE flutter flutter_wrapper_plugin)

# List of absolute paths to libraries that should be bundled with the plugin.
set(disk_space_2_bundled_libraries
  ""
  PARENT_SCOPE
)
