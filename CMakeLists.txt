# ~~~
# Copyright (c) 2025 The Khronos Group Inc.
# Copyright (c) 2025 RasterGrid Kft.
#
# SPDX-License-Identifier: Apache-2.0
# ~~~
cmake_minimum_required(VERSION 3.22.1)

project(VulkanSC-Pipeline-Cache-Utils VERSION 1.0.19)

option(BUILD_TESTS "Build pipeline cache utils unit tests of built" ${PROJECT_IS_TOP_LEVEL})
option(BUILD_VKSCPCINFO "Build vkscpcinfo" ON)
option(BUILD_JSON_GEN_LAYER "Build JSON gen layer" ON)
option(BUILD_WERROR "Treat pipeline cache utils compiler warnings as errors" ${PROJECT_IS_TOP_LEVEL})
option(ENABLE_ADDRESS_SANITIZER "Use address sanitization")
option(ENABLE_THREAD_SANITIZER "Use thread sanitization")
option(PCU_ENABLE_INSTALL "Install VulkanSC-pcutil" ${PROJECT_IS_TOP_LEVEL})

set(API_TYPE "vulkansc")
add_definitions(-DVULKANSC)

add_subdirectory(scripts)
include(cmake/Modules/ImportVulkanEcosystemComponent.cmake)

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
if (ENABLE_THREAD_SANITIZER)
    add_compile_options(-fsanitize=thread)
    if (NOT MSVC)
        add_link_options(-fsanitize=thread)
    endif()
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

find_package(VulkanHeaders REQUIRED CONFIG QUIET)
find_package(VulkanUtilityLibraries REQUIRED CONFIG QUIET)
find_package(jsoncpp REQUIRED CONFIG QUIET)
find_package(cxxopts REQUIRED CONFIG QUIET)
if (BUILD_TESTS) # Layer testing needs the loader
    find_package(VulkanLoader REQUIRED CONFIG QUIET)
    find_vk_package(VulkanLoader ${VULKAN_LOADER_INSTALL_DIR})
endif()

if (BUILD_WERROR)
    add_compile_options("$<IF:$<CXX_COMPILER_ID:MSVC>,/WX,-Werror>")
endif()

if (${CMAKE_CXX_COMPILER_ID} MATCHES "(GNU|Clang)")
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
        if (ENABLE_ADDRESS_SANITIZER)
            add_compile_options(
                -Wno-maybe-uninitialized # GCC 13's libstdc++ regex header doesn't parse without tripping maybe-uninitialized
            )
        endif()
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
elseif(MSVC)
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

if(BUILD_JSON_GEN_LAYER)
    add_subdirectory(layers/json_gen)
endif()

if(BUILD_VKSCPCINFO)
    add_subdirectory(vkscpcinfo)
endif()

add_subdirectory(vkscpcctool)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(PCU_ENABLE_INSTALL)
    include(CMakePackageConfigHelpers)

    install(EXPORT VulkanSCPCUtilConfig NAMESPACE "VulkanSC::" DESTINATION "share/cmake/VulkanSCPCUtil")

    set(version_config "${CMAKE_CURRENT_BINARY_DIR}/generated/VulkanSCPCUtilConfigVersion.cmake")
    write_basic_package_version_file("${version_config}" COMPATIBILITY SameMajorVersion ARCH_INDEPENDENT)
    install(FILES "${version_config}" DESTINATION "share/cmake/VulkanSCPCUtil")

    install(TARGETS VulkanSCPCUtil EXPORT VulkanSCPCUtilConfig)
    install(
        DIRECTORY "${PROJECT_SOURCE_DIR}/include/vulkan/pcutil"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/vulkan"
    )

    install(TARGETS vkscpcctool)
    install(FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/VulkanSCPccDiscovery.cmake"
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/VulkanSCPccUtilities.cmake"
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/VulkanSCPcJsonDyndepScanner.cmake"
        DESTINATION "share/cmake/Modules")

    if(BUILD_JSON_GEN_LAYER)
        set_target_properties(VulkanJSONGenLayer PROPERTIES EXPORT_NAME "JSONGenLayer")

        install(EXPORT VulkanJSONGenLayerConfig DESTINATION share/cmake/VulkanJSONGenLayer NAMESPACE Vulkan::)

        set(version_config "${CMAKE_CURRENT_BINARY_DIR}/generated/VulkanJSONGenLayerConfigVersion.cmake")
        write_basic_package_version_file("${version_config}" COMPATIBILITY SameMajorVersion ARCH_INDEPENDENT)
        install(FILES "${version_config}" DESTINATION "share/cmake/VulkanJSONGenLayer")
    endif()
endif()
