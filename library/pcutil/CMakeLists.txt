# ~~~
# Copyright (c) 2025 The Khronos Group Inc.
# Copyright (c) 2025 RasterGrid Kft.
#
# SPDX-License-Identifier: Apache-2.0
# ~~~

add_library(VulkanSCPCUtil INTERFACE)
add_library(VulkanSC::PCUtil ALIAS VulkanSCPCUtil)

target_include_directories(VulkanSCPCUtil INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(VulkanSCPCUtil INTERFACE Vulkan::Headers)
set_target_properties(VulkanSCPCUtil PROPERTIES
    EXPORT_NAME PCUtil
)

if(PCU_ENABLE_INSTALL)
    install(TARGETS VulkanSCPCUtil EXPORT VulkanSCPCUtilConfig)
    install(
        DIRECTORY "${PROJECT_SOURCE_DIR}/include/vulkan/pcutil"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/vulkan"
    )
endif()
