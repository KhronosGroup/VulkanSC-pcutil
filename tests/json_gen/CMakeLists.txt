# ~~~
# Copyright (c) 2025 The Khronos Group Inc.
# Copyright (c) 2025 RasterGrid Kft.
#
# SPDX-License-Identifier: Apache-2.0
# ~~~

enable_testing()

find_package(GTest REQUIRED CONFIG)

add_library(test_applications OBJECT saxpy.cpp cube.cpp)

target_link_libraries(test_applications PUBLIC
    Vulkan::Headers
    GTest::gtest
)

function(set_gen_layer_test_environment TEST_NAME)
    # The correct way of handling environment variables for tests
#[[
    if (WIN32)
        set(PATH "PATH=path_list_prepend:${VULKAN_TOOLS_INSTALL_DIR}/bin/"
            "PATH=path_list_prepend:${VULKAN_LOADER_INSTALL_DIR}/bin/")
        set(DRIVER_PATH "${VULKAN_TOOLS_INSTALL_DIR}/bin/VkICD_mock_icd.json")
        set(LAYER_PATH "$<TARGET_FILE_DIR:VulkanJSONGenLayer>")

        cmake_path(CONVERT "${PATH}" TO_NATIVE_PATH_LIST PATH)
    else()
        set(LAYER_PATH "$<TARGET_FILE_DIR:VulkanJSONGenLayer>")
        set(DRIVER_PATH "${VULKAN_TOOLS_INSTALL_DIR}/share/vulkan/icd.d/VkICD_mock_icd.json")
        set(LD_LIBRARY_PATH "LD_LIBRARY_PATH=path_list_prepend:${VULKAN_TOOLS_INSTALL_DIR}/lib/"
            "LD_LIBRARY_PATH=path_list_prepend:${VULKAN_LOADER_INSTALL_DIR}/lib/"
            "LD_LIBRARY_PATH=path_list_prepend:${VULKAN_VALIDATION_LAYERS_INSTALL_DIR}/lib/")
    
        cmake_path(CONVERT "${LD_LIBRARY_PATH}" TO_NATIVE_PATH_LIST LD_LIBRARY_PATH)
    endif()

    cmake_path(CONVERT "${DRIVER_PATH}" TO_NATIVE_PATH_LIST DRIVER_PATH)
    cmake_path(CONVERT "${LAYER_PATH}" TO_NATIVE_PATH_LIST LAYER_PATH)
    
    set_property(TEST genlayer.uuid PROPERTY ENVIRONMENT_MODIFICATION
        ${PATH}
        ${LD_LIBRARY_PATH}
        "VK_DRIVER_FILES=set:${DRIVER_PATH}"
        "VK_LOADER_LAYERS_DISABLE=set:~implicit~"
        "VK_INSTANCE_LAYERS=set:VK_LAYER_KHRONOS_json_gen"
        "VK_JSON_FILE_PATH=${CMAKE_CURRENT_BINARY_DIR}"
        "VK_LAYER_PATH=set:${LAYER_PATH}"
        "VK_LOADER_DEBUG=set:all")
]]

    # Workaround for tooling
    if (WIN32)
        set(PATH_VALUE "PATH=${VULKAN_TOOLS_INSTALL_DIR}/bin/$<SEMICOLON>${VULKAN_LOADER_INSTALL_DIR}/bin$<SEMICOLON>$ENV{PATH}")
        set(DRIVER_PATH_VALUE "${VULKAN_TOOLS_INSTALL_DIR}/bin/VkICD_mock_icd.json")
        set(LAYER_PATH "$<TARGET_FILE_DIR:VulkanJSONGenLayer>")

        cmake_path(CONVERT "${PATH}" TO_NATIVE_PATH_LIST PATH)
    else()
        set(LAYER_PATH_VALUE "$<TARGET_FILE_DIR:VulkanJSONGenLayer>")
        set(DRIVER_PATH_VALUE "${VULKAN_TOOLS_INSTALL_DIR}/share/vulkan/icd.d/VkICD_mock_icd.json")
        set(LD_LIBRARY_PATH_VALUE "${VULKAN_TOOLS_INSTALL_DIR}/lib/"
            "${VULKAN_LOADER_INSTALL_DIR}/lib/"
            "${VULKAN_VALIDATION_LAYERS_INSTALL_DIR}/lib/"
            "/home/mate/Develop/Vulkan/.install/lib")

        cmake_path(CONVERT "${LD_LIBRARY_PATH_VALUE}" TO_NATIVE_PATH_LIST LD_LIBRARY_PATH_VALUE)
        cmake_path(CONVERT "${DRIVER_PATH_VALUE}" TO_NATIVE_PATH_LIST DRIVER_PATH_VALUE)

        set_property(TEST ${TEST_NAME} PROPERTY ENVIRONMENT
            "VK_LAYER_PATH=${LAYER_PATH_VALUE}"
            "LD_LIBRARY_PATH=${LD_LIBRARY_PATH_VALUE}"
            "VK_DRIVER_FILES=${DRIVER_PATH_VALUE}"
            "VK_LOADER_LAYERS_DISABLE=~implicit~"
            "VK_INSTANCE_LAYERS=VK_LAYER_KHRONOS_json_gen"
            "VK_JSON_FILE_PATH=${CMAKE_CURRENT_BINARY_DIR}")
    endif()
endfunction()

# 1. ABI testing
# 1.1. Exported functions are really exported (ld/ojbdump.exe/dependencies.exe)
# 1.2. No unexpected external dependencies (CRT, hashlib, etc.)

# 2. Test layer activation logic
# 2.1. Activates when ICD doesn't support VK_EXT_pipeline_properties
# 2.2. Activates when force-enabled over ICDs that do/don't support VK_EXT_pipeline_properties
# 2.3. Doesn't activate when ICD supports VK_EXT_pipeline_properties

# 3. Test UUID query
# 3.1. UUIDs are non-zero
# 3.2. UUIDs are reproducible (two identical API streams produce the same UUID)

add_executable(gltest_uuid gltest_uuid.cpp)

target_link_libraries(gltest_uuid PRIVATE
    test_applications
    Vulkan::PCJson
    Vulkan::Headers
    Vk::Loader
    GTest::gtest
    GTest::gtest_main
)

add_test(NAME genlayer.uuid
    COMMAND gltest_uuid
)

set_gen_layer_test_environment(genlayer.uuid)

# 4. Test object count tracking
# 4.0. SAXPY, Cube
# 4.1. Single-device, high watermark behavior (mix destroys and creates)
# 4.2. Single-device, max and limit tracking (query the tricky-to-track members of objres)
# 4.3. Multi-device, 2nd created after 1st destroyed
# 4.4. Multi-device, create concurrently

add_executable(gltest_objres gltest_objres.cpp)

target_link_libraries(gltest_objres PRIVATE
    test_applications
    Vulkan::PCJson
    Vulkan::Headers
    Vk::Loader
    GTest::gtest
    GTest::gtest_main
)

add_test(NAME genlayer.objectreservation
    COMMAND gltest_objres
)

set_gen_layer_test_environment(genlayer.objectreservation)

# 5. Test pipeline JSON
# 5.1. Simple compute
# 5.2. Simple compute

add_executable(gltest_json gltest_json.cpp)

target_link_libraries(gltest_json PRIVATE
    test_applications
    Vulkan::PCJson
    Vulkan::Headers
    Vk::Loader
    GTest::gtest
    GTest::gtest_main
)

add_test(NAME genlayer.json
    COMMAND gltest_json
)

set_gen_layer_test_environment(genlayer.json)

# 6. Workflow/integration tests
# 6.1. How to relay pipelinecache create infos in objres from VK to VKSC?