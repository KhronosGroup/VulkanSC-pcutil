# ~~~
# Copyright (c) 2025 The Khronos Group Inc.
# Copyright (c) 2025 RasterGrid Kft.
#
# SPDX-License-Identifier: Apache-2.0
# ~~~
cmake_minimum_required(VERSION 3.22.1)

project(VulkanSC-Pipeline-Cache-Utils)

option(BUILD_TESTS "Build pipeline cache utils unit tests of built" ${PROJECT_IS_TOP_LEVEL})
option(PCU_BUILD_PCINFO "Build pcinfo" ON)
option(PCU_BUILD_JSON_GEN_LAYER "Build JSON gen layer" ON)
option(PCU_WERROR "Treat pipeline cache utils compiler warnings as errors" ${PROJECT_IS_TOP_LEVEL})
option(ENABLE_ADDRESS_SANITIZER "Use address sanitization")

set(API_TYPE "vulkansc")
add_definitions(-DVULKANSC)

add_subdirectory(scripts)

set(CMAKE_CXX_VISIBILITY_PRESET "hidden")
set(CMAKE_C_VISIBILITY_PRESET "hidden")
set(CMAKE_VISIBILITY_INLINES_HIDDEN "YES")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if (ENABLE_ADDRESS_SANITIZER)
    add_compile_options(-fsanitize=address)
    if (NOT MSVC)
        add_link_options(-fsanitize=address)
    endif()
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

find_package(VulkanHeaders REQUIRED CONFIG QUIET)
find_package(VulkanUtilityLibraries REQUIRED CONFIG QUIET)
find_package(jsoncpp REQUIRED CONFIG QUIET)

include(GNUInstallDirs)

if (PCU_WERROR)
    if (${CMAKE_CXX_COMPILER_ID} MATCHES "(GNU|Clang)" AND NOT CMAKE_CXX_COMPILER_FRONTEND_VARIANT MATCHES "MSVC")
        add_compile_options(-Werror)
    elseif(MSVC OR CMAKE_CXX_COMPILER_FRONTEND_VARIANT MATCHES "MSVC")
        add_compile_options(/WX)
    endif()
endif()

if (${CMAKE_CXX_COMPILER_ID} MATCHES "(GNU|Clang)" AND NOT CMAKE_CXX_COMPILER_FRONTEND_VARIANT MATCHES "MSVC")
    add_compile_options(
        -Wall
        -Wextra
        -Wno-unused-parameter
        -Wno-missing-field-initializers
        -Wno-format-security
    )

    if (${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
        add_compile_options(
            -Wno-stringop-truncation
            -Wl,--no-undefined
            -Wno-address         # TODO: JOSN lib generator generates non-null address checks for stack arrays
        )
    endif()

    if (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
        add_compile_options(
            -Wno-sign-conversion
            -Wno-shorten-64-to-32
            -Wno-string-conversion
            -Wno-implicit-int-conversion
            -Wno-enum-enum-conversion
        )
    endif()
elseif(MSVC OR CMAKE_CXX_COMPILER_FRONTEND_VARIANT MATCHES "MSVC")
    add_compile_options("/W3")
    # Warn about nested declarations
    add_compile_options("/w34456")
    # Warn about potentially uninitialized variables
    add_compile_options("/w34701")
    add_compile_options("/w34703")
    # Warn about different indirection types
    add_compile_options("/w34057")
    # Warn about signed/unsigned mismatch
    add_compile_options("/w34245")
    # Warn about unused variables
    add_compile_options("/w34189")
    # Do not warn about switch statements with no case labels
    add_compile_options("/wd4065")
    # Stricter ISO conformance
    add_compile_options("/permissive-")

    # Allow usage of unsafe CRT functions and minimize what Windows.h leaks
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS NOMINMAX WIN32_LEAN_AND_MEAN)
endif()

if (PCU_CODEGEN)
    find_package(Python3 REQUIRED QUIET)
    get_target_property(VULKAN_HEADERS_INTERFACE_INCLUDE_DIRECTORIES Vulkan::Headers INTERFACE_INCLUDE_DIRECTORIES)
    get_filename_component(VULKAN_HEADERS_PACKAGE_PREFIX_DIR
        "${VULKAN_HEADERS_INTERFACE_INCLUDE_DIRECTORIES}/.."
        ABSOLUTE
    )
    find_file(VK_XML vk.xml
        HINTS
            "${VULKAN_HEADERS_PACKAGE_PREFIX_DIR}"
        PATH_SUFFIXES
            registry              # <BUILD_INTERFACE>
            share/vulkan/registry # <INSTALL_INTERFACE>
        REQUIRED
    )
    get_filename_component(VK_XML_DIR
        "${VK_XML}"
        DIRECTORY
    )
    add_custom_target(pcu_codegen
        COMMAND Python3::Interpreter "${PROJECT_SOURCE_DIR}/scripts/generate_source.py"
            --api ${API_TYPE}
            "${VK_XML_DIR}"
    )
endif()

add_subdirectory(library/pcutil)
add_subdirectory(library/pcjson)

if(PCU_BUILD_JSON_GEN_LAYER)
    add_subdirectory(layers/json_gen)
endif()

if(PCU_BUILD_PCINFO)
    add_subdirectory(vkscpcinfo)
endif()

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
