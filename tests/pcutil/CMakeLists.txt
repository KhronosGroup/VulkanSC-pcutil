# ~~~
# Copyright (c) 2025 The Khronos Group Inc.
# Copyright (c) 2025 RasterGrid Kft.
#
# SPDX-License-Identifier: Apache-2.0
# ~~~

enable_testing()

add_executable(pctest pctest.cpp)

target_link_libraries(pctest PRIVATE
    Vulkan::Headers
    Vulkan::PipelineCacheReader
    Vulkan::PipelineCacheWriter
)

add_test(NAME pctest.legacy COMMAND pctest)

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/golden_results.txt" GOLDEN_RESULTS)
set_directory_properties(PROPERTIES CMAKE_CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/golden_results.txt")
set_tests_properties(pctest.legacy PROPERTIES PASS_REGULAR_EXPRESSION "${GOLDEN_RESULTS}")

find_package(GTest REQUIRED CONFIG)

add_executable(pctest_readerwriter pctest_readerwriter.cpp pctest_writer_helper.cpp)

target_link_libraries(pctest_readerwriter PRIVATE
    Vulkan::Headers
    Vulkan::PipelineCacheReader
    Vulkan::PipelineCacheWriter
    GTest::gtest
    GTest::gtest_main
)

add_test(NAME pctest.readerwriter COMMAND pctest_readerwriter)

