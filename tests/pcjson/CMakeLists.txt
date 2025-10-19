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
        Vulkan::PCJson
        GTest::gtest
        GTest::gtest_main
        json_validator
    )

    # gtest_discover_tests requires running the output binaries which will not
    # work with cross-compiling, thus we fall back to using gtest_add_tests
    if (CMAKE_CROSSCOMPILING)
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
    else()
        gtest_discover_tests(${_TEST_NAME}
            TEST_PREFIX "${_TEST_PREFIX}"
            DISCOVERY_TIMEOUT 100
        )
    endif()
endmacro()

pcu_add_test(
    TEST_NAME pjtest_parse
    TEST_PREFIX "Library."
    LABLELS Library Parse
)

pcu_add_test(
    TEST_NAME pjtest_gen
    TEST_PREFIX "Library."
    LABLELS Library Gen
)
