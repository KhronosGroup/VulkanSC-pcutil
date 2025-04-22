# ~~~
# Copyright (c) 2025 The Khronos Group Inc.
# Copyright (c) 2025 RasterGrid Kft.
#
# SPDX-License-Identifier: Apache-2.0
# ~~~
add_library(VulkanPipelineCacheJSON)
add_library(Vulkan::PipelineCacheJSON ALIAS VulkanPipelineCacheJSON)

target_sources(VulkanPipelineCacheJSON PRIVATE vulkan_json_gen.c)

target_include_directories(VulkanPipelineCacheJSON
    PRIVATE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/vulkan/pcjson>
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(VulkanPipelineCacheJSON PUBLIC Vulkan::Headers)

set_target_properties(VulkanPipelineCacheJSON PROPERTIES C_EXTENSIONS ON)
