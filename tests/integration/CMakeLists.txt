# ~~~
# Copyright (c) 2024 The Khronos Group Inc.
# Copyright (c) 2024 RasterGrid Kft.
#
# SPDX-License-Identifier: Apache-2.0
# ~~~

cmake_minimum_required(VERSION 3.10.2)

project(integration LANGUAGES CXX)

# We need the fake vulkan_sc_core.hpp on the include path, unfortunately
# without actually including the VulkanSC-Headers package
include_directories(${PROJECT_SOURCE_DIR}/../../fake)

if(FIND_PACKAGE_TESTING)
    find_package(VulkanSC-pcutil REQUIRED CONFIG)
else()
    add_subdirectory(${PROJECT_SOURCE_DIR}/../../ ${PROJECT_BINARY_DIR}/subdir)
endif()

if(NOT TARGET VulkanSC::pcutil)
    message(FATAL_ERROR "VulkanSC::pcutil target not defined!")
endif()

add_executable(test test.cpp)
target_link_libraries(test PRIVATE VulkanSC::pcutil)
