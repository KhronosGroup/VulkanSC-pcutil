# ~~~
# Copyright (c) 2025 The Khronos Group Inc.
# Copyright (c) 2025 RasterGrid Kft.
#
# SPDX-License-Identifier: Apache-2.0
# ~~~

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
