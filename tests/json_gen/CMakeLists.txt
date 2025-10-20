# ~~~
# Copyright (c) 2025 The Khronos Group Inc.
# Copyright (c) 2025 RasterGrid Kft.
#
# SPDX-License-Identifier: Apache-2.0
# ~~~

enable_testing()

find_package(valijson REQUIRED CONFIG)
find_package(jsoncpp REQUIRED CONFIG)
find_package(GTest REQUIRED CONFIG)
include(GoogleTest)

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

    set(COMMON_TEST_ENVIRONMENT
        "VK_LAYER_PATH=${LAYER_PATH_VALUE}"
        "LD_LIBRARY_PATH=${LD_LIBRARY_PATH_VALUE}"
        "VK_DRIVER_FILES=${DRIVER_PATH_VALUE}"
        "VK_LOADER_LAYERS_DISABLE=~implicit~"
        "VK_INSTANCE_LAYERS=VK_LAYER_KHRONOS_json_gen"
        "VK_JSON_FILE_PATH=${CMAKE_CURRENT_BINARY_DIR}"
    )
    list(JOIN COMMON_TEST_ENVIRONMENT "$<SEMICOLON>" COMMON_TEST_ENVIRONMENT)
endif()

macro(pcu_add_test)
    cmake_parse_arguments(""
        ""
        "TEST_NAME;TEST_PREFIX;TEST_LIST"
        "LABELS"
        ${ARGN}
    )
    add_executable(${_TEST_NAME} ${_TEST_NAME}.cpp)

    target_link_libraries(${_TEST_NAME} PRIVATE
        Vulkan::PCJson
        Vulkan::Headers
        Vulkan::UtilityHeaders
        Vulkan::SafeStruct
        Vk::Loader
        GTest::gtest
        GTest::gtest_main
        jsoncpp_static
        json_validator
    )

    # gtest_discover_tests requires running the output binaries which will not
    # work with cross-compiling, thus we fall back to using gtest_add_tests
    if (CMAKE_CROSSCOMPILING)
        gtest_add_tests(
            TARGET ${_TEST_NAME}
            TEST_PREFIX "${_TEST_PREFIX}"
            TEST_LIST ${_TEST_LIST}
        )
        foreach(TEST_I IN LISTS ${_TEST_LIST})
            set_tests_properties("${TEST_I}" PROPERTIES
                LABELS "${_LABELS}"
                ENVIRONMENT "${COMMON_TEST_ENVIRONMENT}"
            )
        endforeach()
    else()
        gtest_discover_tests(${_TEST_NAME}
            TEST_PREFIX "${_TEST_PREFIX}"
            TEST_LIST ${_TEST_LIST}
            DISCOVERY_TIMEOUT 100
        )
    endif()
endmacro()

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

pcu_add_test(
    TEST_NAME gltest_uuid
    TEST_PREFIX "Layer."
    TEST_LIST UUID_TESTS
    LABLELS Layer UUID
)

# 4. Test object count tracking
# 4.0. SAXPY, Cube
# 4.1. Single-device, high watermark behavior (mix destroys and creates)
# 4.2. Single-device, max and limit tracking (query the tricky-to-track members of objres)
# 4.3. Multi-device, 2nd created after 1st destroyed
# 4.4. Multi-device, create concurrently

pcu_add_test(
    TEST_NAME gltest_objres
    TEST_PREFIX "Layer."
    TEST_LIST OBJRES_TESTS
    LABLELS Layer ObjectReservation
)

# 5. Test pipeline JSON
# 5.1. Simple compute
# 5.2. Simple compute

pcu_add_test(
    TEST_NAME gltest_json
    TEST_PREFIX "Layer."
    TEST_LIST JSONGEN_TESTS
    LABLELS Layer JSONGen
)

# 6. Workflow/integration tests
# 6.1. How to relay pipelinecache create infos in objres from VK to VKSC?

# Post process test properties, because gtest_discover_tests can't relay
# multi-value properties to tests properly. We have to resort to heavier
# hammers. https://gitlab.kitware.com/cmake/cmake/-/issues/27313

string(CONFIGURE [[
foreach(TEST_NAME IN LISTS UUID_TESTS OBJRES_TESTS JSONGEN_TESTS)
    set_tests_properties(${TEST_NAME} PROPERTIES ENVIRONMENT "@COMMON_TEST_ENVIRONMENT@")
endforeach()
set_tests_properties(${UUID_TESTS}    PROPERTIES LABELS "@UUID_LABELS@")
set_tests_properties(${OBJRES_TESTS}  PROPERTIES LABELS "@OBJRES_LABELS@")
set_tests_properties(${JSONGEN_TESTS} PROPERTIES LABELS "@JSONGEN_LABELS@")
]] ENVIRONMENT_CMAKE @ONLY)
get_property(GENERATOR_IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(GENERATOR_IS_MULTI_CONFIG)
    file(GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/environment.$<CONFIG>.cmake" CONTENT "${ENVIRONMENT_CMAKE}")
    set_property(DIRECTORY APPEND PROPERTY TEST_INCLUDE_FILES "${CMAKE_CURRENT_BINARY_DIR}/environment.$<CONFIG>.cmake")
else()
    file(GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/environment.cmake" CONTENT "${ENVIRONMENT_CMAKE}")
    set_property(DIRECTORY APPEND PROPERTY TEST_INCLUDE_FILES "${CMAKE_CURRENT_BINARY_DIR}/environment.cmake")
endif()