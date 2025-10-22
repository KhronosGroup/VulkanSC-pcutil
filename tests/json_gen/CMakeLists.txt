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

if (WIN32)
    set(PATH_VALUE
        "${VULKAN_TOOLS_INSTALL_DIR}/bin"
        "${VULKAN_LOADER_INSTALL_DIR}/bin"
        "$ENV{PATH}"
    )
    set(DRIVER_FILES_VALUE "${VULKAN_TOOLS_INSTALL_DIR}/bin/VkICD_mock_icd.json")
    set(LAYER_PATH_VALUE "$<TARGET_FILE_DIR:VulkanJSONGenLayer>")

    cmake_path(CONVERT "${PATH_VALUE}" TO_NATIVE_PATH_LIST PATH_VALUE)
    cmake_path(CONVERT "${DRIVER_FILES_VALUE}" TO_NATIVE_PATH_LIST DRIVER_FILES_VALUE)

    set(COMMON_TEST_ENVIRONMENT
        "VK_LAYER_PATH=${LAYER_PATH_VALUE}"
        "PATH=${PATH_VALUE}"
        "VK_DRIVER_FILES=${DRIVER_FILES_VALUE}"
        "VK_LOADER_LAYERS_DISABLE=~implicit~"
        "VK_INSTANCE_LAYERS=VK_LAYER_KHRONOS_json_gen"
        "VK_JSON_FILE_PATH=${CMAKE_CURRENT_BINARY_DIR}"
    )
else()
    set(LAYER_PATH_VALUE "$<TARGET_FILE_DIR:VulkanJSONGenLayer>")
    set(DRIVER_FILES_VALUE "${VULKAN_TOOLS_INSTALL_DIR}/share/vulkan/icd.d/VkICD_mock_icd.json")
    set(LD_LIBRARY_PATH_VALUE
        "${VULKAN_TOOLS_INSTALL_DIR}/lib/"
        "${VULKAN_LOADER_INSTALL_DIR}/lib/"
        "${VULKAN_VALIDATION_LAYERS_INSTALL_DIR}/lib/")

    cmake_path(CONVERT "${LD_LIBRARY_PATH_VALUE}" TO_NATIVE_PATH_LIST LD_LIBRARY_PATH_VALUE)

    set(COMMON_TEST_ENVIRONMENT
        "VK_LAYER_PATH=${LAYER_PATH_VALUE}"
        "LD_LIBRARY_PATH=${LD_LIBRARY_PATH_VALUE}"
        "VK_DRIVER_FILES=${DRIVER_FILES_VALUE}"
        "VK_LOADER_LAYERS_DISABLE=~implicit~"
        "VK_INSTANCE_LAYERS=VK_LAYER_KHRONOS_json_gen"
        "VK_JSON_FILE_PATH=${CMAKE_CURRENT_BINARY_DIR}"
    )
endif()
list(JOIN COMMON_TEST_ENVIRONMENT "$<SEMICOLON>" COMMON_TEST_ENVIRONMENT)

macro(pcu_add_test)
    cmake_parse_arguments(""
        ""
        "TEST_NAME;TEST_PREFIX"
        "LABELS"
        ${ARGN}
    )
    add_executable(${_TEST_NAME} ${_TEST_NAME}.cpp)

    target_link_libraries(${_TEST_NAME} PRIVATE
        Vulkan::PCJson
        Vulkan::Headers
        Vulkan::UtilityHeaders
        Vk::Loader
        GTest::gtest
        GTest::gtest_main
        jsoncpp_static
        json_validator
    )

    gtest_add_tests(
        TARGET ${_TEST_NAME}
        TEST_PREFIX "${_TEST_PREFIX}"
        TEST_LIST ${_TEST_NAME}_LIST
    )
    foreach(TEST_I IN LISTS ${_TEST_NAME}_LIST)
        set_tests_properties("${TEST_I}" PROPERTIES
            LABELS "${_LABELS}"
            ENVIRONMENT "${COMMON_TEST_ENVIRONMENT}"
        )
    endforeach()

    if(WIN32)
        if(NOT TARGET Vulkan-Loader-copy)
            get_property(GENERATOR_IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
            if(GENERATOR_IS_MULTI_CONFIG)
                add_custom_target(Vulkan-Loader-copy ALL
                    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/vulkan-1.dll")
                add_custom_command(
                    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/vulkan-1.dll"
                    DEPENDS "$<TARGET_PROPERTY:Vk::Loader,IMPORTED_LOCATION>"
                    COMMAND "${CMAKE_COMMAND}" -E copy "$<TARGET_PROPERTY:Vk::Loader,IMPORTED_LOCATION>" "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/vulkan-1.dll"
                    COMMENT "Copying Vulkan-Loader to ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>")
            else()
                add_custom_target(Vulkan-Loader-copy ALL
                    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/vulkan-1.dll")
                add_custom_command(
                    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/vulkan-1.dll"
                    DEPENDS "$<TARGET_PROPERTY:Vk::Loader,IMPORTED_LOCATION>"
                    COMMAND "${CMAKE_COMMAND}" -E copy "$<TARGET_PROPERTY:Vk::Loader,IMPORTED_LOCATION>" "${CMAKE_CURRENT_BINARY_DIR}/vulkan-1.dll"
                    COMMENT "Copying Vulkan-Loader to ${CMAKE_CURRENT_BINARY_DIR}")
            endif()
        endif()
        add_dependencies(${_TEST_NAME} Vulkan-Loader-copy)
    endif()
endmacro()

pcu_add_test(
    TEST_NAME gltest_uuid
    TEST_PREFIX "Layer."
    LABLELS Layer UUID
)

pcu_add_test(
    TEST_NAME gltest_objres
    TEST_PREFIX "Layer."
    LABLELS Layer ObjectReservation
)

pcu_add_test(
    TEST_NAME gltest_json
    TEST_PREFIX "Layer."
    LABLELS Layer JSONGen
)
