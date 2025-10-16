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

add_executable(pjtest_parse pjtest_parse.cpp)

target_link_libraries(pjtest_parse PRIVATE
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
        TARGET pjtest_parse
        TEST_PREFIX "PCJSON.Parse."
        TEST_LIST PARSE_TESTS
    )
    foreach(PARSE_TEST IN LISTS PARSE_TESTS)
        set_tests_properties("${PARSE_TEST}" PROPERTIES LABELS "PCJSON;Parse")
    endforeach()
else()
    gtest_discover_tests(pjtest_parse
        TEST_PREFIX "PCJSON.Parse."
        PROPERTIES LABELS "PCJSON;Parse"
        DISCOVERY_TIMEOUT 100
    )
endif()

add_executable(pjtest_gen pjtest_gen.cpp)

target_link_libraries(pjtest_gen PRIVATE
    Vulkan::Headers
    Vulkan::PCJson
    GTest::gtest
    GTest::gtest_main
    jsoncpp_static
    json_validator
)

if (CMAKE_CROSSCOMPILING)
    gtest_add_tests(
        TARGET pjtest_gen
        TEST_PREFIX "PCJSON.Gen."
        TEST_LIST GEN_TESTS
    )
    foreach(GEN_TEST IN LISTS GEN_TESTS)
        set_tests_properties("${GEN_TEST}" PROPERTIES LABELS "PCJSON;Gen")
    endforeach()
else()
    gtest_discover_tests(pjtest_gen
        TEST_PREFIX "PCJSON.Gen."
        PROPERTIES LABELS "PCJSON;Gen"
        DISCOVERY_TIMEOUT 100
    )
endif()
