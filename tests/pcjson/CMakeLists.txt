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

macro(pcu_add_test)
    cmake_parse_arguments(""
        ""
        "TEST_NAME;TEST_PREFIX"
        "LABELS"
        ${ARGN}
    )
    add_executable(${_TEST_NAME} ${_TEST_NAME}.cpp)

    target_link_libraries(${_TEST_NAME} PRIVATE
        Vulkan::Headers
        Vulkan::UtilityHeaders
        VulkanSC::PCJson
        GTest::gtest
        GTest::gtest_main
        json_validator
    )

    gtest_add_tests(
        TARGET ${_TEST_NAME}
        TEST_PREFIX "${_TEST_PREFIX}"
        TEST_LIST ${_TEST_NAME}_LIST
    )
    foreach(TEST_I IN LISTS ${_TEST_LIST}_LIST)
        set_tests_properties("${TEST_I}" PROPERTIES
            LABELS "${_LABELS}"
        )
    endforeach()
endmacro()

pcu_add_test(
    TEST_NAME pcjson_test_parse
    TEST_PREFIX "Library."
    LABLELS Library Parse
)

pcu_add_test(
    TEST_NAME pcjson_test_gen
    TEST_PREFIX "Library."
    LABLELS Library Gen
)
