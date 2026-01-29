# ~~~
# Copyright (c) 2025-2026 The Khronos Group Inc.
# Copyright (c) 2025-2026 RasterGrid Kft.
#
# SPDX-License-Identifier: Apache-2.0
# ~~~

function(TARGET_EMBED_VKSC_ENVIRONMENT target)
    set(EmbeddedEnviromentFileName "${CMAKE_CURRENT_BINARY_DIR}/DeviceFilterStub.cpp")
    set(EmbeddedEnvHelperClassName "SetEnvHelper_${target}")

    set(EmbeddedHelperSource "#include <stdlib.h>\n\
    \n\
    class ${EmbeddedEnvHelperClassName}\n\
    {\n\
    public:\n\
        ${EmbeddedEnvHelperClassName}()\n\
        {\n")

        foreach(env ${ARGN})
            string(REPLACE "=" ";" env_separated ${env})
            list(GET env_separated 0 env_name)
            list(GET env_separated 1 env_value)

            if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
                string(APPEND EmbeddedHelperSource "        _putenv_s(\"${env_name}\", \"${env_value}\")\;\n")
            else()
                string(APPEND EmbeddedHelperSource "        setenv(\"${env_name}\", \"${env_value}\", 1)\;\n")
            endif()
        endforeach()

        string(APPEND EmbeddedHelperSource "    }\n\
    }\;\n\
    ${EmbeddedEnvHelperClassName} embedEnvHelper_${target}\;\n")

    write_file(${EmbeddedEnviromentFileName} ${EmbeddedHelperSource})

    add_library(DeviceFilterStub STATIC ${EmbeddedEnviromentFileName})
    get_target_property(is_imported ${target} IMPORTED)
    if (${is_imported})
        set(link_type "INTERFACE")
    else()
        set(link_type "PRIVATE")
    endif()

    if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
        target_link_libraries(${target} ${link_type} $<LINK_LIBRARY:WHOLE_ARCHIVE,DeviceFilterStub>)
    else()
        if(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
            target_link_options(${target} ${link_type} "-force_load $<TARGET_FILE:DeviceFilterStub>")
        elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "GNU")
            target_link_options(${target} ${link_type} "-Wl,--push-state,--whole-archive;$<TARGET_FILE:DeviceFilterStub>;-Wl,--pop-state")
        elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" OR CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
            target_link_options(${target} ${link_type} "/WHOLEARCHIVE:$<TARGET_FILE:DeviceFilterStub>")
        else()
            message(FATAL_ERROR "Unsupported compiler for whole archive inclusion of static libraries")
        endif()
    endif()

    if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        set_target_properties(DeviceFilterStub PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
        target_link_options(${target} ${link_type} $<$<CXX_COMPILER_ID:MSVC>:/NODEFAULTLIB:LIBCMT$<$<CONFIG:Debug>:D>>)
    endif()
endfunction()

function(ADD_VKSC_PIPELINE_CACHE TARGET_NAME)
    cmake_parse_arguments(ARG
        "DEBUG"      # options
        "PATH;FLAGS" # single-value args
        ""           # multi-value args
        ${ARGN}
    )
    cmake_path(IS_RELATIVE ARG_PATH ARG_PATH_IS_RELATIVE)
    if(ARG_PATH_IS_RELATIVE)
        cmake_path(ABSOLUTE_PATH ARG_PATH BASE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
    endif()
    get_property(GENERATOR_IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
    if(GENERATOR_IS_MULTI_CONFIG)
        set(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/${TARGET_NAME}.bin")
        set(DEPFILE "${TARGET_NAME}.$<CONFIG>.dyndep")
    else()
        set(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.bin")
        set(DEPFILE "${TARGET_NAME}.dyndep")
    endif()

    get_property(LANGUAGES GLOBAL PROPERTY ENABLED_LANGUAGES)
    if(NOT CXX IN_LIST LANGUAGES)
        message(FATAL_ERROR
            "VulkanSC SDK CMake integration requires CXX language support be enabled. "
            "The VulkanSC package cannot reliably enable_language(CXX). For details, see CMP0165."
        )
    endif()

    set(VulkanSC_PCC_DYNDEP_SCANNER "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/VulkanSCPcJsonDyndepScanner.cmake")

    set(PCC_COMMAND_STRING "${VulkanSC_PCC_EXECUTABLE} --path ${ARG_PATH} --out ${OUTPUT} ${ARG_FLAGS}")
    if(ARG_DEBUG AND VulkanSC_DEBUG_FLAG)
        string(APPEND PCC_COMMAND_STRING " ${VulkanSC_DEBUG_FLAG}")
    endif()
    if(VulkanSC_FLAGS)
        string(APPEND PCC_COMMAND_STRING " ${VulkanSC_FLAGS}")
    endif()
    if(VulkanSC_DEVICE_FLAGS)
        string(APPEND PCC_COMMAND_STRING " ${VulkanSC_DEVICE_FLAGS}")
    endif()
    separate_arguments(PCC_COMMAND NATIVE_COMMAND PROGRAM SEPARATE_ARGS "${PCC_COMMAND_STRING}")
    add_custom_command(
        COMMAND ${PCC_COMMAND}
        COMMAND_EXPAND_LISTS
        WORKING_DIRECTORY "${ARG_PATH}"
        OUTPUT
            "${OUTPUT}"
        DEPENDS
            "${VulkanSC_PCC_EXECUTABLE}"
        DEPFILE
            "${DEPFILE}"
        COMMENT
            "Building PCJSON ${OUTPUT}"
    )
    add_custom_target(${TARGET_NAME}_vksc_pcjson_scan_ ALL
        COMMAND "${CMAKE_COMMAND}"
            -D ARG_PATH:PATH="${ARG_PATH}"
            -D ARG_OUTPUT:FILEPATH="${OUTPUT}"
            -D ARG_DEPFILE:FILEPATH="${CMAKE_CURRENT_BINARY_DIR}/${DEPFILE}"
            -P "${VulkanSC_PCC_DYNDEP_SCANNER}"
        WORKING_DIRECTORY "${ARG_PATH}"
        BYPRODUCTS
            "${DEPFILE}"
        COMMENT
            "Scanning PCJSON dynamic dependencies for ${DEPFILE}"
    )
    add_custom_target(${TARGET_NAME} ALL
        DEPENDS
            "${OUTPUT}"
    )
    add_dependencies(${TARGET_NAME} ${TARGET_NAME}_vksc_pcjson_scan_)
endfunction()

# Only include environment stub if not cross-compiling
if(NOT CMAKE_CROSSCOMPILING)
    if(VulkanSC_VENDOR_ID_FILTER)
        list(APPEND EMBEDDED_ENVIRONMENT "VK_LOADER_VENDOR_ID_FILTER=${VulkanSC_VENDOR_ID_FILTER}")
    endif()
    if(VulkanSC_DEVICE_ID_FILTER)
        list(APPEND EMBEDDED_ENVIRONMENT "VK_LOADER_DEVICE_ID_FILTER=${VulkanSC_DEVICE_ID_FILTER}")
    endif()
    if(VulkanSC_DRIVER_ID_FILTER)
        list(APPEND EMBEDDED_ENVIRONMENT "VK_LOADER_DRIVER_ID_FILTER=${VulkanSC_DRIVER_ID_FILTER}")
    endif()
    # Only include environment stub if there is any environment needed to be configured
    if(DEFINED EMBEDDED_ENVIRONMENT)
        target_embed_vksc_environment(VulkanSCVulkanSC "${EMBEDDED_ENVIRONMENT}")
    endif()
endif()
