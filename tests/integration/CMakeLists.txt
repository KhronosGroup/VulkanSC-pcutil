# ~~~
# Copyright (c) 2025 The Khronos Group Inc.
# Copyright (c) 2025 RasterGrid Kft.
# Copyright (c) 2025 Valve Corporation
# Copyright (c) 2025 LunarG, Inc.
#
# SPDX-License-Identifier: Apache-2.0
# ~~~

cmake_minimum_required(VERSION 3.22.1)

if(CMAKE_SCRIPT_MODE_FILE)
    if(NOT EXISTS JSON_GEN_LAYER_LOCATION)
        message(FATAL_ERROR "JSON gen layer artifact not found: JSON_GEN_LAYER_LOCATION = ${JSON_GEN_LAYER_LOCATION}")
    endif()
    return()
endif()

project(VulkanSC-Pipeline-Cache-Utils-Integration-Tests)

set(EXPECTED_TARGETS VulkanSC::PCUtil)
if(FIND_PACKAGE_TESTING)
    set(EXPECTED_VERSIONS VulkanSCPCUtil_VERSION)
    find_package(VulkanSCPCUtil REQUIRED CONFIG)
    if(TEST_JSON_GEN_LAYER)
        list(APPEND EXPECTED_VERSIONS VulkanJSONGenLayer_VERSION)
        find_package(VulkanJSONGenLayer REQUIRED CONFIG)
        list(APPEND EXPECTED_TARGETS Vulkan::JSONGenLayer)
        set(JSON_GEN_LAYER_LOCATION Vulkan::JSONGenLayer IMPORTED_LOCATION)
    endif()
    # NOTE: VulkanSC-pcutil has no chance at testing with the same CMake target names as a "real"
    #       VulkanSC application, because dependencies only get exported under VulkanSC:: namespace
    #       by the SDK. Consuming SC header under Vulkan::Headers is SC ecosystem-internal only.
    find_package(VulkanHeaders REQUIRED CONFIG)

    foreach(VER IN LISTS EXPECTED_VERSIONS)
        if (NOT DEFINED ${VER})
            message(FATAL_ERROR "${VER} not defined!")
        endif()
        message(STATUS "${VER} = ${${VER}}")
    endforeach()
else()
    option(BUILD_TESTS "Build pipeline cache utils unit tests of built" OFF)
    option(BUILD_JSON_GEN_LAYER "Build JSON gen layer" ${TEST_JSON_GEN_LAYER})
    option(ENABLE_ADDRESS_SANITIZER "Use address sanitization" ${TEST_ADDRESS_SANITIZER})
    option(ENABLE_THREAD_SANITIZER "Use thread sanitization" ${TEST_THREAD_SANITIZER})
    add_subdirectory(../../ "${CMAKE_CURRENT_BINARY_DIR}/pcutils" EXCLUDE_FROM_ALL)
    if(TEST_JSON_GEN_LAYER)
        get_target_property(JSON_GEN_LAYER_LOCATION VulkanJSONGenLayer $<TARGET_FILE>)
    endif()

    # Consuming pcutil via add_subdirectory should NOT add installation code to the parent CMake project.
    if (DEFINED CMAKE_INSTALL_INCLUDEDIR)
        message(FATAL_ERROR "CMAKE_INSTALL_INCLUDEDIR was defined!")
    endif()
endif()

if (TEST_ADDRESS_SANITIZER)
    add_compile_options(-fsanitize=address)
    if (NOT MSVC)
        add_link_options(-fsanitize=address)
    endif()
endif()
if (TEST_THREAD_SANITIZER)
    add_compile_options(-fsanitize=thread)
    if (NOT MSVC)
        add_link_options(-fsanitize=thread)
    endif()
endif()

# NOTE: static lib transitive dependencies leak. See: https://gitlab.kitware.com/cmake/cmake/-/issues/19224
#       Resolution would need manual archiving rules: https://stackoverflow.com/questions/2157629/linking-static-libraries-to-other-static-libraries
find_package(jsoncpp REQUIRED CONFIG QUIET)

add_executable(pcutil_test ../pcutil/pcutil_test.cpp)
target_link_libraries(pcutil_test PRIVATE VulkanSC::PCUtil)
add_test(NAME pcutil COMMAND pcutil_test)

if(TEST_JSON_GEN_LAYER)
    add_test(NAME json_gen_layer COMMAND "${CMAKE_COMMAND}" -D JSON_GEN_LAYER_LOCATION="${JSON_GEN_LAYER_LOCATION}" -P "${CMAKE_CURRENT_LIST_FILE}")
endif()
