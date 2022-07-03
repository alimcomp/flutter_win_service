# The Flutter tooling requires that developers have CMake 3.10 or later
# installed. You should not increase this version, as doing so will cause
# the plugin to fail to compile for some customers of the plugin.
cmake_minimum_required(VERSION 3.10)

project(service_library VERSION 0.0.1 LANGUAGES C)

add_library(service SHARED
  "include/result_struct.h"
  "include/dart_api_dl.c"
  "include/win_service.h"
  "win_service.c"
  "service.c"
)

set_target_properties(service PROPERTIES
  PUBLIC_HEADER service.h
  OUTPUT_NAME "service"
)

target_compile_definitions(service PUBLIC DART_SHARED_LIB)
