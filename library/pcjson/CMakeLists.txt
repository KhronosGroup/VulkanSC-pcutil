# ~~~
# Copyright (c) 2025 The Khronos Group Inc.
# Copyright (c) 2025 RasterGrid Kft.
#
# SPDX-License-Identifier: Apache-2.0
# ~~~
add_library(VulkanSCPCJson)
add_library(VulkanSC::PCJson ALIAS VulkanSCPCJson)

target_sources(VulkanSCPCJson PRIVATE
    generated/vksc_pipeline_json_gen.hpp
    generated/vksc_pipeline_json_parse.hpp
    vksc_pipeline_json_base.hpp
    vksc_pipeline_json_gen.cpp
    vksc_pipeline_json_parse.cpp)

target_include_directories(VulkanSCPCJson
    PRIVATE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/vulkan/pcjson>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/library/pcjson>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/library/pcjson/generated>
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

find_package(jsoncpp REQUIRED CONFIG QUIET)
find_package(hashlib REQUIRED CONFIG QUIET)
target_link_libraries(VulkanSCPCJson PRIVATE jsoncpp_static hashlib::hashlib Vulkan::Headers)
set_target_properties(VulkanSCPCJson PROPERTIES OUTPUT_NAME vkscpcjson)

install(TARGETS VulkanSCPCJson)
install(
    DIRECTORY ${PROJECT_SOURCE_DIR}/include/vulkan/pcjson
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/vulkan"
)
