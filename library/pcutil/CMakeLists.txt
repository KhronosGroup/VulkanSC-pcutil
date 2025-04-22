# ~~~
# Copyright (c) 2025 The Khronos Group Inc.
# Copyright (c) 2025 RasterGrid Kft.
#
# SPDX-License-Identifier: Apache-2.0
# ~~~

foreach(Utility IN ITEMS Reader Writer)
    string(TOLOWER ${Utility} lowerutility)
    add_library(VulkanPipelineCache${Utility} INTERFACE)
    add_library(Vulkan::PipelineCache${Utility} ALIAS VulkanPipelineCache${Utility})

    target_include_directories(VulkanPipelineCache${Utility} INTERFACE "${PROJECT_SOURCE_DIR}/include/vulkan/pcutil")

    target_link_libraries(VulkanPipelineCache${Utility} INTERFACE Vulkan::Headers)
endforeach()
