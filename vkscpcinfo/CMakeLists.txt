# ~~~
# Copyright (c) 2025 The Khronos Group Inc.
# Copyright (c) 2025 RasterGrid Kft.
#
# SPDX-License-Identifier: Apache-2.0
# ~~~

add_executable(vkscpcinfo vkscpcinfo.cpp)

target_link_libraries(vkscpcinfo PRIVATE
    Vulkan::Headers
    VulkanSC::PCUtil
)
