# ~~~
# Copyright (c) 2025 The Khronos Group Inc.
# Copyright (c) 2025 RasterGrid Kft.
# Copyright (c) 2025 Valve Corporation
# Copyright (c) 2025 LunarG, Inc.
#
# SPDX-License-Identifier: Apache-2.0
# ~~~

find_package(valijson REQUIRED CONFIG)
find_package(jsoncpp REQUIRED CONFIG)

set(SCHEMA_PATH "${CMAKE_CURRENT_LIST_DIR}/../json/")

add_library(json_validator
    json_validator.h
    json_validator.cpp)

target_include_directories(json_validator PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(json_validator PUBLIC jsoncpp_static valijson)
target_compile_definitions(json_validator PRIVATE SCHEMA_PATH="${SCHEMA_PATH}")

add_subdirectory(json)
add_subdirectory(json_gen)
add_subdirectory(pcjson)
add_subdirectory(pcutil)
add_subdirectory(vkscpcinfo)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(BIT_WIDTH 64)
else()
    set(BIT_WIDTH 32)
endif()
if(CMAKE_C_FLAGS MATCHES "-m(32|64)")
    set(CMAKE_C_FLAGS_ARG -D "CMAKE_C_FLAGS=-m${CMAKE_MATCH_1}")
endif()
if(CMAKE_CXX_FLAGS MATCHES "-m(32|64)")
    set(CMAKE_CXX_FLAGS_ARG -D "CMAKE_CXX_FLAGS=-m${CMAKE_MATCH_1}")
endif()
if(CMAKE_GENERATOR_PLATFORM)
    set(CMAKE_GENERATOR_PLATFORM_ARG --build-generator-platform "${CMAKE_GENERATOR_PLATFORM}")
endif()
if(CMAKE_GENERATOR_TOOLSET)
    set(CMAKE_GENERATOR_TOOLSET_ARG --build-generator-toolset "${CMAKE_GENERATOR_TOOLSET}")
endif()

# Test add_subdirectory suppport
add_test(NAME Integration.add_subdirectory
    COMMAND "${CMAKE_CTEST_COMMAND}"
        --build-and-test "${CMAKE_CURRENT_LIST_DIR}/integration"
                         "${CMAKE_CURRENT_BINARY_DIR}/add_subdirectory"
        --build-generator ${CMAKE_GENERATOR}
        ${CMAKE_GENERATOR_PLATFORM_ARG}
        ${CMAKE_GENERATOR_TOOLSET_ARG}
        --build-options -C "${VulkanSC-Pipeline-Cache-Utils_SOURCE_DIR}/external/$<CONFIG>/${BIT_WIDTH}/helper.cmake"
                        -D FIND_PACKAGE_TESTING=OFF
                        -D TEST_JSON_GEN_LAYER=${BUILD_JSON_GEN_LAYER}
                        -D TEST_ADDRESS_SANITIZER=${ENABLE_ADDRESS_SANITIZER}
                        -D TEST_THREAD_SANITIZER=${ENABLE_THREAD_SANITIZER}
                        ${CMAKE_C_FLAGS_ARG}
                        ${CMAKE_CXX_FLAGS_ARG}
)

set(test_install_dir "${CMAKE_CURRENT_BINARY_DIR}/install")
add_test(NAME Integration.install
    COMMAND "${CMAKE_COMMAND}"
        --install "${VulkanSC-Pipeline-Cache-Utils_BINARY_DIR}"
        --prefix "${test_install_dir}"
        --config $<CONFIG>
)
set_tests_properties(Integration.install PROPERTIES FIXTURES_SETUP Integration.find_package.setup)

# Test find_package suppport
add_test(NAME Integration.find_package
    COMMAND "${CMAKE_CTEST_COMMAND}"
        --build-and-test "${CMAKE_CURRENT_LIST_DIR}/integration"
                         "${CMAKE_CURRENT_BINARY_DIR}/find_package"
        --build-generator ${CMAKE_GENERATOR}
        ${CMAKE_GENERATOR_PLATFORM_ARG}
        ${CMAKE_GENERATOR_TOOLSET_ARG}
        --build-options  -D "CMAKE_PREFIX_PATH=${VULKAN_HEADERS_INSTALL_DIR};${JSONCPP_INSTALL_DIR};${HASHLIB_INSTALL_DIR}"
                         -D "VulkanSCPCUtil_ROOT=${test_install_dir}"
                         -D "VulkanJSONGenLayer_ROOT=${test_install_dir}"
                         -D FIND_PACKAGE_TESTING=ON
                         -D TEST_JSON_GEN_LAYER=${BUILD_JSON_GEN_LAYER}
                         -D TEST_ADDRESS_SANITIZER=${ENABLE_ADDRESS_SANITIZER}
                         -D TEST_THREAD_SANITIZER=${ENABLE_THREAD_SANITIZER}
                         ${CMAKE_C_FLAGS_ARG}
                         ${CMAKE_CXX_FLAGS_ARG}
)

# Installing comes before testing
set_tests_properties(Integration.find_package PROPERTIES FIXTURES_REQUIRED Integration.find_package.setup)