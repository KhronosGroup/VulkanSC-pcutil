# ~~~
# Copyright (c) 2025 The Khronos Group Inc.
# Copyright (c) 2025 RasterGrid Kft.
#
# SPDX-License-Identifier: Apache-2.0
# ~~~

enable_testing()

add_executable(pcutil_test pcutil_test.cpp)

target_link_libraries(pcutil_test PRIVATE
    Vulkan::Headers
    VulkanSC::PCUtil
    VulkanSC::PCUtil
)

add_test(NAME Library.PCUtil.GoldenResult COMMAND pcutil_test)

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/golden_results.txt" GOLDEN_RESULTS)
set_directory_properties(PROPERTIES CMAKE_CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/golden_results.txt")
set_tests_properties(Library.PCUtil.GoldenResult PROPERTIES
    PASS_REGULAR_EXPRESSION "${GOLDEN_RESULTS}"
    LABELS "Library;PCUtil;PCReader;PCWriter;Legacy"
)

find_package(GTest REQUIRED CONFIG)
find_package(SPIRV-Headers REQUIRED CONFIG QUIET)
find_package(SPIRV-Tools REQUIRED CONFIG QUIET)

add_executable(pcutil_test_readerwriter pcutil_test_readerwriter.cpp pcutil_test_writer_helper.cpp)

target_link_libraries(pcutil_test_readerwriter PRIVATE
    Vulkan::Headers
    VulkanSC::PCUtil
    VulkanSC::PCUtil
    GTest::gtest
    GTest::gtest_main
)

gtest_add_tests(
    TARGET pcutil_test_readerwriter
    TEST_PREFIX "Library.PCUtil."
    TEST_LIST READWRITE_TESTS
)
foreach(TEST_I IN LISTS READWRITE_TESTS)
    set_tests_properties("${TEST_I}" PROPERTIES LABELS "Library;PCUtil;PCReader;PCWriter")
endforeach()
