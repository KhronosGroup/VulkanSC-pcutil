# ~~~
# Copyright (c) 2025 The Khronos Group Inc.
# Copyright (c) 2025 RasterGrid Kft.
#
# SPDX-License-Identifier: Apache-2.0
# ~~~

find_package(valijson REQUIRED CONFIG)
find_package(jsoncpp REQUIRED CONFIG)
find_package(GTest REQUIRED CONFIG)

enable_testing()

add_executable(json_test json_test.cpp json_validator.cpp)
target_link_libraries(json_test PRIVATE GTest::gtest GTest::gtest_main jsoncpp_static valijson)
add_test(NAME json_test COMMAND json_test)
