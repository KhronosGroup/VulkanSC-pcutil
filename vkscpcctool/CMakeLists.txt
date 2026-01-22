# ~~~
# Copyright (c) 2025-2026 The Khronos Group Inc.
# Copyright (c) 2025-2026 RasterGrid Kft.
#
# SPDX-License-Identifier: Apache-2.0
# ~~~

add_executable(vkscpcctool vkscpcctool.cpp)

target_link_libraries(vkscpcctool PRIVATE
    jsoncpp_static
    cxxopts::cxxopts
    Vulkan::Headers
    Vulkan::UtilityHeaders)
