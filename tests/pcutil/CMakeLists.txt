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

add_test(NAME Library.PCUtil.GoldenResult COMMAND pctest)

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/golden_results.txt" GOLDEN_RESULTS)
set_directory_properties(PROPERTIES CMAKE_CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/golden_results.txt")
set_tests_properties(Library.PCUtil.GoldenResult PROPERTIES
    PASS_REGULAR_EXPRESSION "${GOLDEN_RESULTS}"
    LABELS "Library;PCUtil;PCReader;PCWriter;Legacy"
)

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

gtest_add_tests(
    TARGET pctest_readerwriter
    TEST_PREFIX "Library.PCUtil."
    TEST_LIST READWRITE_TESTS
)
foreach(TEST_I IN LISTS READWRITE_TESTS)
    set_tests_properties("${TEST_I}" PROPERTIES LABELS "Library;PCUtil;PCReader;PCWriter")
endforeach()


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

if (WIN32)
    set(PATH_VALUE
        "${VULKANSC_TOOLS_INSTALL_DIR}/bin"
        "${VULKANSC_LOADER_INSTALL_DIR}/bin"
    )
    set(DRIVER_FILES_VALUE "${VULKANSC_TOOLS_INSTALL_DIR}/bin/VkICD_mock_icd_vksc.json")
    set(LAYER_PATH_VALUE "${VULKANSC_VALIDATION_LAYERS_INSTALL_DIR}/bin")

    cmake_path(CONVERT "${PATH_VALUE}" TO_NATIVE_PATH_LIST PATH_VALUE)
    cmake_path(CONVERT "${DRIVER_FILES_VALUE}" TO_NATIVE_PATH_LIST DRIVER_FILES_VALUE)
    cmake_path(CONVERT "${LAYER_PATH}" TO_NATIVE_PATH_LIST LAYER_PATH)

    set(WRITERDEVICE_TEST_ENVIRONMENT
        "VK_LAYER_PATH=${LAYER_PATH_VALUE}"
        "VK_DRIVER_FILES=${DRIVER_FILES_VALUE}"
        "VK_LOADER_LAYERS_DISABLE=~implicit~")
    list(TRANSFORM PATH_VALUE PREPEND "PATH=path_list_prepend:" OUTPUT_VARIABLE WRITERDEVICE_TEST_ENVIRONMENT_MODIFICATION)
else()
    set(LAYER_PATH_VALUE "${VULKANSC_VALIDATION_LAYERS_INSTALL_DIR}/share/vulkansc/explicit_layer.d")
    set(DRIVER_FILES_VALUE "${VULKANSC_TOOLS_INSTALL_DIR}/share/vulkansc/icd.d/VkICD_mock_icd_vksc.json")
    set(LD_LIBRARY_PATH_VALUE
        "${VULKANSC_TOOLS_INSTALL_DIR}/lib"
        "${VULKANSC_LOADER_INSTALL_DIR}/lib"
        "${VULKANSC_VALIDATION_LAYERS_INSTALL_DIR}/lib")

    cmake_path(CONVERT "${LD_LIBRARY_PATH_VALUE}" TO_NATIVE_PATH_LIST LD_LIBRARY_PATH_VALUE)

    set(WRITERDEVICE_TEST_ENVIRONMENT
        "VK_LAYER_PATH=${LAYER_PATH_VALUE}"
        "LD_LIBRARY_PATH=${LD_LIBRARY_PATH_VALUE}"
        "VK_DRIVER_FILES=${DRIVER_FILES_VALUE}"
        "VK_LOADER_LAYERS_DISABLE=~implicit~")
endif()
list(JOIN WRITERDEVICE_TEST_ENVIRONMENT "$<SEMICOLON>" WRITERDEVICE_TEST_ENVIRONMENT)

gtest_add_tests(
    TARGET pctest_writerdevice
    TEST_PREFIX "Library.PCUtil."
    TEST_LIST WRITERDEVICE_TESTS
)
foreach(TEST_I IN LISTS WRITERDEVICE_TESTS)
    set_tests_properties("${TEST_I}" PROPERTIES
        LABELS "Library;PCUtil;PCReader;PCWriter"
        ENVIRONMENT "${WRITERDEVICE_TEST_ENVIRONMENT}"
    )
    if(WIN32)
        set_tests_properties("${TEST_I}" PROPERTIES ENVIRONMENT_MODIFICATION "${WRITERDEVICE_TEST_ENVIRONMENT_MODIFICATION}")
    endif()
endforeach()
