# ~~~
# Copyright (c) 2025 The Khronos Group Inc.
# Copyright (c) 2025 RasterGrid Kft.
#
# SPDX-License-Identifier: Apache-2.0
# ~~~

find_package(GTest REQUIRED CONFIG)

enable_testing()

add_executable(json_test generated/json_test.cpp)
target_link_libraries(json_test PRIVATE GTest::gtest GTest::gtest_main jsoncpp_static valijson json_validator)
target_include_directories(json_test PRIVATE "${CMAKE_CURRENT_LIST_DIR}")

set(JSON_DATA_PATH "${CMAKE_CURRENT_LIST_DIR}/data/")

target_compile_definitions(json_test PRIVATE JSON_DATA_PATH="${JSON_DATA_PATH}")

add_test(NAME json_test COMMAND json_test)
