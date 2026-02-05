# ~~~
# Copyright (c) 2025-2026 The Khronos Group Inc.
# Copyright (c) 2025-2026 RasterGrid Kft.
#
# SPDX-License-Identifier: Apache-2.0
# ~~~

function(TARGET_EMBED_VKSC_ENVIRONMENT target)
    # NOTE: Due to CMP0165, enable_language() can only be reliably called after the project() declaration
    if(NOT DEFINED PROJECT_NAME)
        message(FATAL_ERROR "Please include VulkanSCPccUtilities after your CMake project() declaration")
    endif()
    enable_language(CXX)

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
        if(CMAKE_GENERATOR MATCHES "Visual Studio")
            target_link_options(${target} ${link_type}
                    $<$<AND:$<OR:$<AND:$<LINK_LANGUAGE:C>,$<OR:$<C_COMPILER_ID:MSVC>,$<C_COMPILER_FRONTEND_VARIANT:MSVC>>>,$<AND:$<LINK_LANGUAGE:CXX>,$<OR:$<CXX_COMPILER_ID:MSVC>,$<CXX_COMPILER_FRONTEND_VARIANT:MSVC>>>>,$<STREQUAL:$<GENEX_EVAL:$<TARGET_PROPERTY:MSVC_RUNTIME_LIBRARY>>,MultiThreaded$<GENEX_EVAL:$<$<CONFIG:Debug>:Debug>>>>:/NODEFAULTLIB:MSVCRT>
                    $<$<AND:$<OR:$<AND:$<LINK_LANGUAGE:C>,$<OR:$<C_COMPILER_ID:MSVC>,$<C_COMPILER_FRONTEND_VARIANT:MSVC>>>,$<AND:$<LINK_LANGUAGE:CXX>,$<OR:$<CXX_COMPILER_ID:MSVC>,$<CXX_COMPILER_FRONTEND_VARIANT:MSVC>>>>,$<OR:$<STREQUAL:$<GENEX_EVAL:$<TARGET_PROPERTY:MSVC_RUNTIME_LIBRARY>>,MultiThreaded$<GENEX_EVAL:$<$<CONFIG:Debug>:Debug>>DLL>,$<NOT:$<BOOL:$<GENEX_EVAL:$<TARGET_PROPERTY:MSVC_RUNTIME_LIBRARY>>>>>>:/NODEFAULTLIB:LIBCMT>
    )
        endif()
    endif()
endfunction()

function(ADD_VKSC_PIPELINE_CACHE TARGET_NAME)
    cmake_parse_arguments(ARG
        "DEBUG"          # options
        "PATH;FLAGS;OUT" # single-value args
        "DEPENDS"        # multi-value args
        ${ARGN}
    )
    cmake_path(IS_RELATIVE ARG_PATH ARG_PATH_IS_RELATIVE)
    if(ARG_PATH_IS_RELATIVE)
        cmake_path(ABSOLUTE_PATH ARG_PATH BASE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
    endif()
    get_property(GENERATOR_IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
    if(GENERATOR_IS_MULTI_CONFIG)
        if(NOT DEFINED ARG_OUT)
            set(ARG_OUT "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/${TARGET_NAME}.bin")
        endif()
        set(DEPFILE "${TARGET_NAME}.$<CONFIG>.dyndep")
    else()
        if(NOT DEFINED ARG_OUT)
            set(ARG_OUT "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.bin")
        endif()
        set(DEPFILE "${TARGET_NAME}.dyndep")
    endif()
    cmake_path(IS_RELATIVE ARG_OUT ARG_OUT_IS_RELATIVE)
    if(ARG_OUT_IS_RELATIVE)
        cmake_path(ABSOLUTE_PATH ARG_OUT BASE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
    endif()

    set(VulkanSC_PCC_DYNDEP_SCANNER "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/VulkanSCPcJsonDyndepScanner.cmake")

    set(PCC_COMMAND_STRING "${VulkanSC_PCC_EXECUTABLE} --path ${ARG_PATH} --out ${ARG_OUT} ${ARG_FLAGS}")
    if(ARG_DEBUG AND VulkanSC_PCC_DEBUG_FLAG)
        string(APPEND PCC_COMMAND_STRING " ${VulkanSC_PCC_DEBUG_FLAG}")
    endif()
    if(VulkanSC_PCC_FLAGS)
        string(APPEND PCC_COMMAND_STRING " ${VulkanSC_PCC_FLAGS}")
    endif()
    if(VulkanSC_PCC_DEVICE_FLAGS)
        string(APPEND PCC_COMMAND_STRING " ${VulkanSC_PCC_DEVICE_FLAGS}")
    endif()
    separate_arguments(PCC_COMMAND NATIVE_COMMAND PROGRAM SEPARATE_ARGS "${PCC_COMMAND_STRING}")

    file(RELATIVE_PATH RELATIVE_ARG_OUT "${CMAKE_BINARY_DIR}" "${ARG_OUT}")
    file(TO_NATIVE_PATH "${RELATIVE_ARG_OUT}" RELATIVE_ARG_OUT)
    if(CMAKE_GENERATOR MATCHES "Visual Studio")
        set(COMMENT "Building pipeline cache binary ${RELATIVE_ARG_OUT}")
    else()
        set(COMMENT "Building PC binary ${RELATIVE_ARG_OUT}")
    endif()
    add_custom_command(
        COMMAND ${PCC_COMMAND}
        COMMAND_EXPAND_LISTS
        WORKING_DIRECTORY "${ARG_PATH}"
        OUTPUT
            "${ARG_OUT}"
        DEPENDS
            "${VulkanSC_PCC_EXECUTABLE}"
        DEPFILE "${DEPFILE}"
        COMMENT "${COMMENT}"
    )
    add_custom_target(${TARGET_NAME} ALL
        DEPENDS
            "${ARG_OUT}"
            ${ARG_DEPENDS}
    )

    if(CMAKE_GENERATOR MATCHES "Visual Studio")
        file(TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${DEPFILE}" NATIVE_DEPFILE)
        set(COMMENT "Scanning PCJSON dynamic dependencies -> ${NATIVE_DEPFILE}")
    else()
        file(RELATIVE_PATH RELATIVE_DEPFILE "${CMAKE_BINARY_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/${DEPFILE}")
        set(COMMENT "Scanning PCJSON dynamic dependencies ${RELATIVE_DEPFILE}")
    endif()
    add_custom_target(${TARGET_NAME}_scan ALL
        COMMAND "${CMAKE_COMMAND}"
            -D ARG_PATH:PATH="${ARG_PATH}"
            -D ARG_OUTPUT:FILEPATH="${ARG_OUT}"
            -D ARG_DEPFILE:FILEPATH="${CMAKE_CURRENT_BINARY_DIR}/${DEPFILE}"
            -P "${VulkanSC_PCC_DYNDEP_SCANNER}"
        WORKING_DIRECTORY "${ARG_PATH}"
        BYPRODUCTS
            "${DEPFILE}"
        COMMENT "${COMMENT}"
    )
    add_dependencies(${TARGET_NAME} ${TARGET_NAME}_scan)
endfunction()

# Only include environment stub if not cross-compiling
if(NOT CMAKE_CROSSCOMPILING AND NOT DEFINED VulkanSC_NO_EMBEDDED_ENVIRONMENT_STUB)
    if(VulkanSC_PCC_VENDOR_ID_FILTER)
        list(APPEND EMBEDDED_ENVIRONMENT "VK_LOADER_VENDOR_ID_FILTER=${VulkanSC_PCC_VENDOR_ID_FILTER}")
    endif()
    if(VulkanSC_PCC_DEVICE_ID_FILTER)
        list(APPEND EMBEDDED_ENVIRONMENT "VK_LOADER_DEVICE_ID_FILTER=${VulkanSC_PCC_DEVICE_ID_FILTER}")
    endif()
    if(VulkanSC_PCC_DRIVER_ID_FILTER)
        list(APPEND EMBEDDED_ENVIRONMENT "VK_LOADER_DRIVER_ID_FILTER=${VulkanSC_PCC_DRIVER_ID_FILTER}")
    endif()
    # Only include environment stub if there is any environment needed to be configured
    if(DEFINED EMBEDDED_ENVIRONMENT)
        target_embed_vksc_environment(VulkanSCVulkanSC "${EMBEDDED_ENVIRONMENT}")
    endif()
endif()
