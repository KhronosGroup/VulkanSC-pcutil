# ~~~
# Copyright (c) 2025 The Khronos Group Inc.
# Copyright (c) 2025 RasterGrid Kft.
#
# SPDX-License-Identifier: Apache-2.0
# ~~~

enable_testing()

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

find_package(GTest REQUIRED CONFIG)
find_package(SPIRV-Headers REQUIRED CONFIG QUIET)
find_package(SPIRV-Tools REQUIRED CONFIG QUIET)

add_executable(pctest_readerwriter pctest_readerwriter.cpp pctest_writer_helper.cpp)

target_link_libraries(pctest_readerwriter PRIVATE
    Vulkan::Headers
    Vulkan::PipelineCacheReader
    Vulkan::PipelineCacheWriter
    GTest::gtest
    GTest::gtest_main
)

add_test(NAME pctest.readerwriter COMMAND pctest_readerwriter)


add_executable(pctest_writerdevice pctest_writerdevice.cpp pctest_writer_helper.cpp)

if(WIN32)
    set(_VulkanSC_library_name vulkansc-1)
else()
    set(_VulkanSC_library_name vulkansc)
endif()

find_library(VulkanSC_LIBRARY
    NAMES ${_VulkanSC_library_name}
)

target_link_libraries(pctest_writerdevice PRIVATE
    Vulkan::Headers
    Vulkan::PipelineCacheReader
    Vulkan::PipelineCacheWriter
    GTest::gtest
    GTest::gtest_main
    "${VulkanSC_LIBRARY}"
)

if (TARGET SPIRV-Tools)
    target_link_libraries(pctest_writerdevice PRIVATE SPIRV-Tools)
elseif(TARGET SPIRV-Tools-static)
    target_link_libraries(pctest_writerdevice PRIVATE SPIRV-Tools-static)
elseif(TARGET SPIRV-Tools-shared)
    target_link_libraries(pctest_writerdevice PRIVATE SPIRV-Tools-shared)
else()
    message(FATAL_ERROR "Cannot determine SPIRV-Tools target name")
endif()

add_test(NAME pctest.writerdevice COMMAND pctest_writerdevice)

if (WIN32)
    set(PATH "PATH=path_list_prepend:${VULKANSC_TOOLS_INSTALL_DIR}/bin/"
        "PATH=path_list_prepend:${VULKANSC_LOADER_INSTALL_DIR}/bin/")
    set(DRIVER_PATH "${VULKANSC_TOOLS_INSTALL_DIR}/bin/VkICD_mock_icd_vksc.json")
    set(LAYER_PATH "${VULKANSC_VALIDATION_LAYERS_INSTALL_DIR}/bin/")

    cmake_path(CONVERT "${PATH}" TO_NATIVE_PATH_LIST PATH)
else()
    set(LAYER_PATH "${VULKANSC_VALIDATION_LAYERS_INSTALL_DIR}/share/vulkansc/explicit_layer.d/")
    set(DRIVER_PATH "${VULKANSC_TOOLS_INSTALL_DIR}/share/vulkansc/icd.d/VkICD_mock_icd_vksc.json")
    set(LD_LIBRARY_PATH "LD_LIBRARY_PATH=path_list_prepend:${VULKANSC_TOOLS_INSTALL_DIR}/lib/"
        "LD_LIBRARY_PATH=path_list_prepend:${VULKANSC_LOADER_INSTALL_DIR}/lib/"
        "LD_LIBRARY_PATH=path_list_prepend:${VULKANSC_VALIDATION_LAYERS_INSTALL_DIR}/lib/")

    cmake_path(CONVERT "${LD_LIBRARY_PATH}" TO_NATIVE_PATH_LIST LD_LIBRARY_PATH)
endif()

cmake_path(CONVERT "${DRIVER_PATH}" TO_NATIVE_PATH_LIST DRIVER_PATH)
cmake_path(CONVERT "${LAYER_PATH}" TO_NATIVE_PATH_LIST LAYER_PATH)

set_property(TEST pctest.writerdevice PROPERTY ENVIRONMENT_MODIFICATION
    ${PATH}
    ${LD_LIBRARY_PATH}
    ";VK_DRIVER_FILES=set:${DRIVER_PATH}"
    "VK_LOADER_LAYERS_DISABLE=set:~implicit~"
    "VK_LAYER_PATH=set:${LAYER_PATH}")

