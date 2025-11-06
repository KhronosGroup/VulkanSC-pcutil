# ~~~
# Copyright (c) 2025 The Khronos Group Inc.
# Copyright (c) 2025 RasterGrid Kft.
#
# SPDX-License-Identifier: Apache-2.0
# ~~~

add_library(VulkanSCPCUtil INTERFACE)
add_library(VulkanSC::PCUtil ALIAS VulkanSCPCUtil)

target_include_directories(VulkanSCPCUtil INTERFACE "${PROJECT_SOURCE_DIR}/include/vulkan/pcutil")

target_link_libraries(VulkanSCPCUtil INTERFACE Vulkan::Headers)
