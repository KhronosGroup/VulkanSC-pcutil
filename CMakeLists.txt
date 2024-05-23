# Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
# Copyright (c) 2024 The Khronos Group Inc.
# Copyright (c) 2024 RasterGrid Kft.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# Author: Daniel Koch <dkoch@nvidia.com>

cmake_minimum_required(VERSION 3.10.2)

project(VULKANSC_PC_UTILS)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_C_COMPILER_ID MATCHES "Clang")
    set(COMMON_COMPILE_FLAGS "-Wall -Wextra -Werror -Wconversion")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_COMPILE_FLAGS}")
endif()

add_library(VulkanSC-pcutil INTERFACE)
add_library(VulkanSC::pcutil ALIAS VulkanSC-pcutil)

target_include_directories(VulkanSC-pcutil INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Create vulkansc-pcutil-targets.cmake
set_target_properties(VulkanSC-pcutil PROPERTIES EXPORT_NAME "pcutil")
install(
    TARGETS VulkanSC-pcutil
    EXPORT vulkansc-pcutil-targets
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(
    EXPORT vulkansc-pcutil-targets
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/VulkanSC-pcutil
    NAMESPACE VulkanSC::
)

# Create vulkansc-pcutil-config.cmake
set(PCUTIL_EXPORT_TARGETS ${CMAKE_INSTALL_LIBDIR}/cmake/VulkanSC-pcutil/vulkansc-pcutil-targets.cmake)
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/vulkansc-pcutil-config.cmake.in" [=[
    include(CMakeFindDependencyMacro)
    # NOTE: Because VulkanHeaders is a PUBLIC dependency it needs to be found prior to VulkanSC-pcutil
    find_dependency(VulkanHeaders REQUIRED)

    @PACKAGE_INIT@

    include(@PACKAGE_PCUTIL_EXPORT_TARGETS@)
]=])

configure_package_config_file(
    ${CMAKE_CURRENT_BINARY_DIR}/vulkansc-pcutil-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/vulkansc-pcutil-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/VulkanSC-pcutil
    PATH_VARS PCUTIL_EXPORT_TARGETS
    NO_SET_AND_CHECK_MACRO
    NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/vulkansc-pcutil-config.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/VulkanSC-pcutil
)

install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/pcreader.hpp ${CMAKE_CURRENT_SOURCE_DIR}/pcwriter.hpp
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

option(BUILD_TESTS "Build tests")
if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

add_executable(pcinfo pcinfo.cpp)

target_link_libraries(pcinfo PRIVATE VulkanSC-pcutil)

target_include_directories(pcinfo PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/fake/)

add_executable(pctest pipeline.cpp)

target_include_directories(pctest PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/fake/)

