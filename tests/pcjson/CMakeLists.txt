# ~~~
# Copyright (c) 2025 The Khronos Group Inc.
# Copyright (c) 2025 RasterGrid Kft.
#
# SPDX-License-Identifier: Apache-2.0
# ~~~

enable_testing()

find_package(GTest REQUIRED CONFIG)

add_executable(pjtest_parse pjtest_parse.cpp)

target_link_libraries(pjtest_parse PRIVATE
    Vulkan::Headers
    Vulkan::PCJson
    GTest::gtest
    GTest::gtest_main
)

add_test(NAME pjtest.parse COMMAND pjtest_parse)

add_executable(pjtest_gen pjtest_gen.cpp)

target_link_libraries(pjtest_gen PRIVATE
    Vulkan::Headers
    Vulkan::PCJson
    GTest::gtest
    GTest::gtest_main
    jsoncpp_static
)

add_test(NAME pjtest.gen COMMAND pjtest_gen)
