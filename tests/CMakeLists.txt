# ~~~
# Copyright (c) 2025 The Khronos Group Inc.
# Copyright (c) 2025 RasterGrid Kft.
#
# SPDX-License-Identifier: Apache-2.0
# ~~~

find_package(valijson REQUIRED CONFIG)
find_package(jsoncpp REQUIRED CONFIG)

set(SCHEMA_PATH "${CMAKE_CURRENT_LIST_DIR}/../json/")

add_library(json_validator
    json_validator.h
    json_validator.cpp)

target_include_directories(json_validator PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(json_validator PUBLIC jsoncpp_static valijson)
target_compile_definitions(json_validator INTERFACE SCHEMA_PATH="${SCHEMA_PATH}")

add_subdirectory(json)
add_subdirectory(json_gen)
add_subdirectory(pcjson)
add_subdirectory(pcutil)
add_subdirectory(vkscpcinfo)
