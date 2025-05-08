cmake_minimum_required(VERSION 3.10)
set(PROJECT_NAME "disk_space_2")
project(${PROJECT_NAME} LANGUAGES CXX)

# Add compiler warnings and optimizations
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Werror=return-type
    -O2
  )
endif()

set(PLUGIN_NAME "${PROJECT_NAME}_plugin")

add_library(${PLUGIN_NAME} SHARED
  "${PLUGIN_NAME}.cc"
)
apply_standard_settings(${PLUGIN_NAME})
set_target_properties(${PLUGIN_NAME} PROPERTIES
  CXX_VISIBILITY_PRESET hidden
  VERSION 1.0.0
  SOVERSION 1
)
target_compile_definitions(${PLUGIN_NAME} PRIVATE FLUTTER_PLUGIN_IMPL)
target_include_directories(${PLUGIN_NAME} INTERFACE
  "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(${PLUGIN_NAME} PRIVATE flutter)
target_link_libraries(${PLUGIN_NAME} PRIVATE PkgConfig::GTK)

# Add documentation
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/README.md"
  DESTINATION "${CMAKE_INSTALL_DOCDIR}/${PROJECT_NAME}")

set(disk_space_2_bundled_libraries
  ""
  PARENT_SCOPE
)
