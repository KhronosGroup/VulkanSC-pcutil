# ~~~
# Copyright (c) 2025-2026 The Khronos Group Inc.
# Copyright (c) 2025-2026 RasterGrid Kft.
#
# SPDX-License-Identifier: Apache-2.0
# ~~~

if(CMAKE_SCRIPT_MODE_FILE)
    load_cache("${CACHE_DIR}" READ_WITH_PREFIX _ VulkanSC_PCC-STRINGS)
    message("List of available Vulkan SC PCC target architectures/devices (possible values of VulkanSC_PCC):")
    foreach(PCC IN LISTS _VulkanSC_PCC-STRINGS)
        message("\t* ${PCC}")
    endforeach()
    return()
endif()

find_program(VulkanSC_PCCTOOL_EXECUTABLE
    REQUIRED
    NAMES vkscpcctool
    HINTS
        "$ENV{VULKANSC_SDK}/bin"
)

function(resolve_vksc_pcc_executable PCC_EXECUTABLE PCC_JSON_PATH OUT_VAR)
    message(DEBUG "PCC entry: ${PCC_EXECUTABLE}")
    if(WIN32)
        set(NATIVE_PATH_SEPARATOR_REGEX [[\\]])
    else()
        set(NATIVE_PATH_SEPARATOR_REGEX [[/]])
    endif()
    if(NOT PCC_EXECUTABLE MATCHES ${NATIVE_PATH_SEPARATOR_REGEX})
        separate_arguments(PCC_EXECUTABLE NATIVE_COMMAND PROGRAM "${PCC_EXECUTABLE}")
        list(POP_BACK PCC_EXECUTABLE) # pop the empty element due to separate_arguments not having <args>
    else()
        cmake_path(IS_RELATIVE PCC_EXECUTABLE PCC_EXECUTABLE_IS_RELATIVE)
        if(PCC_EXECUTABLE_IS_RELATIVE)
            cmake_path(GET PCC_JSON_PATH PARENT_PATH PCC_JSON_PARENT_PATH)
            cmake_path(ABSOLUTE_PATH PCC_EXECUTABLE BASE_DIRECTORY "${PCC_JSON_PARENT_PATH}" NORMALIZE)
        else()
            cmake_path(NORMAL_PATH PCC_EXECUTABLE)
        endif()
    endif()
    set(${OUT_VAR} "${PCC_EXECUTABLE}" PARENT_SCOPE)
    message(DEBUG "PCC resolved: ${PCC_EXECUTABLE}")
endfunction()

# If a PCC executable is set manually then do not perform the PCC discovery
if(NOT DEFINED VulkanSC_PCC_EXECUTABLE)
    # Find PCC JSON manifests in the SDK first
    file(GLOB PCC_GLOBBED_PATHS "$ENV{VULKANSC_SDK}/share/vulkansc/pcc.d/*.json")
    foreach(PCC_GLOBBED_PATH IN LISTS PCC_GLOBBED_PATHS)
        file(REAL_PATH "${PCC_GLOBBED_PATH}" PCC_GLOBBED_PATH)
        if(NOT PCC_GLOBBED_PATH IN_LIST PCC_JSON_PATHS)
            message(DEBUG "Discovered PCJSON: ${PCC_GLOBBED_PATH}")
            list(APPEND PCC_JSON_PATHS "${PCC_GLOBBED_PATH}")
        else()
            message(DEBUG "Skipped PCJSON: ${PCC_GLOBBED_PATH} (a file resolving to the same file already found)")
        endif()
    endforeach()

    if(WIN32)
        # Find PCC JSON manifests in the registry
        if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
            foreach(HIVE IN ITEMS HKLM HKCU)
                cmake_host_system_information(
                    RESULT PCC_PROP_NAMES
                    QUERY WINDOWS_REGISTRY "${HIVE}/SOFTWARE/Khronos/VulkanSC/PCC" VALUE_NAMES
                )
                foreach(PCC_PROP_NAME IN LISTS PCC_PROP_NAMES)
                    file(REAL_PATH "${PCC_PROP_NAME}" PCC_PROP_NAME)
                    if(NOT PCC_PROP_NAME IN_LIST PCC_JSON_PATHS)
                        message(DEBUG "Discovered PCJSON: ${PCC_PROP_NAME}")
                        list(APPEND PCC_JSON_PATHS "${PCC_PROP_NAME}")
                    else()
                        message(DEBUG "Skipped PCJSON: ${PCC_PROP_NAME} (a registry entry resolving to the same file already found)")
                    endif()
                endforeach()
            endforeach()
        else()
            message(FATAL_ERROR "PCC (pipeline cache compiler) discovery on Windows requires CMake 3.24 or newer")
        endif()
    else()
        # Find PCC JSON manifests in the supported FHS locations
        foreach(MANIFEST_DIR IN ITEMS
            $ENV{HOME}/.config/vulkansc/pcc.d
            /etc/xdg/vulkansc/pcc.d
            /usr/local/etc/vulkansc/pcc.d
            /etc/vulkansc/pcc.d
            $ENV{HOME}/.local/share/vulkansc/pcc.d
            /usr/local/share/vulkansc/pcc.d
            /usr/share/vulkansc/pcc.d
        )
            file(GLOB PCC_GLOBBED_PATHS "${MANIFEST_DIR}/*.json")
            foreach(PCC_GLOBBED_PATH IN LISTS PCC_GLOBBED_PATHS)
                file(REAL_PATH "${PCC_GLOBBED_PATH}" PCC_GLOBBED_PATH)
                if(NOT PCC_GLOBBED_PATH IN_LIST PCC_JSON_PATHS)
                    message(DEBUG "Discovered PCJSON: ${PCC_GLOBBED_PATH}")
                    list(APPEND PCC_JSON_PATHS "${PCC_GLOBBED_PATH}")
                else()
                    message(DEBUG "Skipped PCJSON: ${PCC_GLOBBED_PATH} (a file resolving to the same file already found)")
                endif()
            endforeach()
        endforeach()
    endif()
    foreach(PCC_JSON_PATH IN LISTS PCC_JSON_PATHS)
        message(DEBUG "Processing PCJSON: ${PCC_JSON_PATH}")
        file(READ "${PCC_JSON_PATH}" PCC_JSON)
        string(JSON PCC_NAME GET "${PCC_JSON}" "name")

        # Parse available architectures in the PCC manifest
        string(JSON ARCH_LENGTH ERROR_VARIABLE ARCH_LENGTH_ERROR LENGTH "${PCC_JSON}" "architectures")
        if(NOT ARCH_LENGTH_ERROR)
            math(EXPR ARCH_LENGTH_STOP "${ARCH_LENGTH} - 1")
            foreach(ARCH_INDEX RANGE ${ARCH_LENGTH_STOP})
                string(JSON ARCH_NAME MEMBER "${PCC_JSON}" "architectures" ${ARCH_INDEX})
                string(JSON PCC_EXECUTABLE GET "${PCC_JSON}" "architectures" ${ARCH_NAME} "pipeline_cache_compiler")
                resolve_vksc_pcc_executable("${PCC_EXECUTABLE}" "${PCC_JSON_PATH}" PCC_EXECUTABLE)
                string(JSON FLAGS GET "${PCC_JSON}" "architectures" ${ARCH_NAME} "pipeline_cache_flags")
                string(JSON DEBUG_FLAG GET "${PCC_JSON}" "architectures" ${ARCH_NAME} "pipeline_cache_debug_flag")
                string(JSON VENDOR_ID_FILTER ERROR_VARIABLE VENDOR_ID_FILTER_ERROR GET "${PCC_JSON}" "architectures" ${ARCH_NAME} "vendor_id_filter")
                string(JSON DEVICE_ID_FILTER ERROR_VARIABLE DEVICE_ID_FILTER_ERROR GET "${PCC_JSON}" "architectures" ${ARCH_NAME} "device_id_filter")
                string(JSON DRIVER_ID_FILTER ERROR_VARIABLE DRIVER_ID_FILTER_ERROR GET "${PCC_JSON}" "architectures" ${ARCH_NAME} "driver_id_filter")

                # Only add a configuration if one with the same name does not exist yet
                if(NOT DEFINED VulkanSC_PCC_EXECUTABLE_${PCC_NAME}_${ARCH_NAME})
                    message(DEBUG "Discovered PCC: ${PCC_NAME} - ${ARCH_NAME}")
                    set(VulkanSC_PCC_EXECUTABLE_${PCC_NAME}_${ARCH_NAME} "${PCC_EXECUTABLE}")
                    set(VulkanSC_PCC_FLAGS_${PCC_NAME}_${ARCH_NAME} "${FLAGS}")
                    set(VulkanSC_PCC_DEBUG_FLAG_${PCC_NAME}_${ARCH_NAME} "${DEBUG_FLAG}")
                    if(NOT VENDOR_ID_FILTER_ERROR)
                        set(VulkanSC_PCC_VENDOR_ID_FILTER_${PCC_NAME}_${ARCH_NAME} ${VENDOR_ID_FILTER})
                    endif()
                    if(NOT DEVICE_ID_FILTER_ERROR)
                        set(VulkanSC_PCC_DEVICE_ID_FILTER_${PCC_NAME}_${ARCH_NAME} ${DEVICE_ID_FILTER})
                    endif()
                    if(NOT DRIVER_ID_FILTER_ERROR)
                        set(VulkanSC_PCC_DRIVER_ID_FILTER_${PCC_NAME}_${ARCH_NAME} ${DRIVER_ID_FILTER})
                    endif()
                    list(APPEND VulkanSC_PCC_OPTIONS "${PCC_NAME} - ${ARCH_NAME}")
                else()
                    message(DEBUG "Target architecture skipped: ${PCC_NAME} - ${ARCH_NAME} (an entry with the same name is already registered)")
                endif()
            endforeach()
        endif()

        # Parse available devices in the PCC manifest
        string(JSON DEV_LENGTH ERROR_VARIABLE DEV_LENGTH_ERROR LENGTH "${PCC_JSON}" "devices")
        if(NOT DEV_LENGTH_ERROR)
            math(EXPR DEV_LENGTH_STOP "${DEV_LENGTH} - 1")
            foreach(DEV_INDEX RANGE ${DEV_LENGTH_STOP})
                string(JSON VENDOR_ID_FILTER ERROR_VARIABLE VENDOR_ID_FILTER_ERROR GET "${PCC_JSON}" "devices" ${DEV_INDEX} "vendor_id_filter")
                string(JSON DEVICE_ID_FILTER ERROR_VARIABLE DEVICE_ID_FILTER_ERROR GET "${PCC_JSON}" "devices" ${DEV_INDEX} "device_id_filter")
                string(JSON DRIVER_ID_FILTER ERROR_VARIABLE DRIVER_ID_FILTER_ERROR GET "${PCC_JSON}" "devices" ${DEV_INDEX} "driver_id_filter")
                if(NOT VENDOR_ID_FILTER_ERROR)
                    set(VENDOR_ARG "--vendor-id-filter ${VENDOR_ID_FILTER}")
                endif()
                if(NOT DEVICE_ID_FILTER_ERROR)
                    set(DEVICE_ARG "--device-id-filter ${DEVICE_ID_FILTER}")
                endif()
                if(NOT DRIVER_ID_FILTER_ERROR)
                    set(DRIVER_ARG "--driver-id-filter ${DRIVER_ID_FILTER}")
                endif()
                separate_arguments(DEVICEFILTER_COMMAND NATIVE_COMMAND PROGRAM SEPARATE_ARGS
                    "${VulkanSC_PCCTOOL_EXECUTABLE} --list-devices ${VENDOR_ARG} ${DEVICE_ARG} ${DRIVER_ARG}")
                cmake_path(GET VulkanSC_PCCTOOL_EXECUTABLE PARENT_PATH PCCTOOL_PARENT_PATH)
                execute_process(
                    COMMAND ${DEVICEFILTER_COMMAND}
                    WORKING_DIRECTORY "${PCCTOOL_PARENT_PATH}"
                    OUTPUT_VARIABLE DEVICEFILTER_OUTPUT
                    OUTPUT_STRIP_TRAILING_WHITESPACE
                    COMMAND_ERROR_IS_FATAL ANY
                )
                message(DEBUG "Device filter command: ${DEVICEFILTER_COMMAND}")
                message(DEBUG "Device filter output: ${DEVICEFILTER_OUTPUT}")
                set(DEVICEFILTER_REGEX "([0-9a-fA-Fx,-]*) ([^\n]*)")
                string(REGEX MATCHALL "${DEVICEFILTER_REGEX}" DEVICEFILTER_MATCHES "${DEVICEFILTER_OUTPUT}")
                foreach(DEVICEFILTER_MATCH IN LISTS DEVICEFILTER_MATCHES)
                    if(DEVICEFILTER_MATCH MATCHES "${DEVICEFILTER_REGEX}")
                        set(DEVICE_ID "${CMAKE_MATCH_1}")
                        set(DEVICE_NAME "${CMAKE_MATCH_2}")
                        string(JSON PCC_EXECUTABLE GET "${PCC_JSON}" "devices" ${DEV_INDEX} "pipeline_cache_compiler")
                        resolve_vksc_pcc_executable("${PCC_EXECUTABLE}" "${PCC_JSON_PATH}" PCC_EXECUTABLE)
                        string(JSON DEVICE_FLAG GET "${PCC_JSON}" "devices" ${DEV_INDEX} "pipeline_cache_device_flag")
                        string(JSON DEBUG_FLAG GET "${PCC_JSON}" "devices" ${DEV_INDEX} "pipeline_cache_debug_flag")

                        # Only add a configuration if one with the same name does not exist yet
                        if(NOT DEFINED VulkanSC_PCC_EXECUTABLE_${PCC_NAME}_${DEVICE_NAME})
                            message(DEBUG "Discovered PCC: ${PCC_NAME} - ${DEVICE_NAME}")
                            set(VulkanSC_PCC_EXECUTABLE_${PCC_NAME}_${DEVICE_NAME} "${PCC_EXECUTABLE}")
                            set(VulkanSC_PCC_DEVICE_FLAGS_${PCC_NAME}_${DEVICE_NAME} "${DEVICE_FLAG} ${DEVICE_ID}")
                            set(VulkanSC_PCC_DEBUG_FLAG_${PCC_NAME}_${DEVICE_NAME} "${DEBUG_FLAG}")
                            if(NOT VENDOR_ID_FILTER_ERROR)
                                set(VulkanSC_PCC_VENDOR_ID_FILTER_${PCC_NAME}_${DEVICE_NAME} ${VENDOR_ID_FILTER})
                            endif()
                            set(VulkanSC_PCC_DEVICE_ID_FILTER_${PCC_NAME}_${DEVICE_NAME} ${DEVICE_ID})
                            if(NOT DRIVER_ID_FILTER_ERROR)
                                set(VulkanSC_PCC_DRIVER_ID_FILTER_${PCC_NAME}_${DEVICE_NAME} ${DRIVER_ID_FILTER})
                            endif()
                            list(APPEND VulkanSC_PCC_OPTIONS "${PCC_NAME} - ${DEVICE_NAME}")
                        else()
                            message(DEBUG "Target device skipped: ${PCC_NAME} - ${DEVICE_NAME} (an entry with the same name is already registered)")
                        endif()
                    endif()
                endforeach()
                unset(VENDOR_ARG)
                unset(DEVICE_ARG)
                unset(DRIVER_ARG)
            endforeach()
        endif()
    endforeach()

    # Set Emulation PCC as default if available
    list(LENGTH VulkanSC_PCC_OPTIONS VulkanSC_PCC_OPTIONS_LENGTH)
    if(NOT VulkanSC_PCC_OPTIONS_LENGTH EQUAL 0)
        list(FIND VulkanSC_PCC_OPTIONS "Emulation - Portable" Emulation_Portable_INDEX)
        if(NOT Emulation_Portable_INDEX EQUAL -1)
            set(VulkanSC_PCC_DEFAULT "Emulation - Portable")
        else()
            list(GET VulkanSC_PCC_OPTIONS 0 VulkanSC_PCC_DEFAULT)
        endif()
        message(VERBOSE "List of available Vulkan SC PCC target architectures/devices (possible values of VulkanSC_PCC):")
        foreach(PCC IN LISTS VulkanSC_PCC_OPTIONS)
            message(VERBOSE "\t* ${PCC}")
        endforeach()
        message(STATUS "Selected PCC compiler: ${VulkanSC_PCC_DEFAULT}")
    else()
        message(FATAL_ERROR "No installed Vulkan SC pipeline cache compiler found.\n"
                            "Alternatively, custom Vulkan SC pipeline cache compiler can be configured with VulkanSC_PCC_EXECUTABLE.")
    endif()
    set(VulkanSC_PCC "${VulkanSC_PCC_DEFAULT}" CACHE STRING "The name of the pipeline cache compiler and its architecture to use")
    if(NOT VulkanSC_PCC_OPTIONS_LENGTH EQUAL 0)
        set_property(CACHE VulkanSC_PCC PROPERTY STRINGS "${VulkanSC_PCC_OPTIONS}")
        set(VulkanSC_PCC_DISCOVERY ON CACHE INTERNAL "Signals PCC discovery having occured")
    endif()

    if(VulkanSC_PCC)
        if(VulkanSC_PCC IN_LIST VulkanSC_PCC_OPTIONS)
            if(VulkanSC_PCC MATCHES "^([A-Za-z0-9]*) - ([^\n]*)$")
                set(VulkanSC_PCC_EXECUTABLE_DEFAULT ${VulkanSC_PCC_EXECUTABLE_${CMAKE_MATCH_1}_${CMAKE_MATCH_2}})
                set(VulkanSC_PCC_FLAGS_DEFAULT ${VulkanSC_PCC_FLAGS_${CMAKE_MATCH_1}_${CMAKE_MATCH_2}})
                set(VulkanSC_PCC_DEBUG_FLAG_DEFAULT ${VulkanSC_PCC_DEBUG_FLAG_${CMAKE_MATCH_1}_${CMAKE_MATCH_2}})
                set(VulkanSC_PCC_DEVICE_FLAGS_DEFAULT ${VulkanSC_PCC_DEVICE_FLAGS_${CMAKE_MATCH_1}_${CMAKE_MATCH_2}})
                set(VulkanSC_PCC_VENDOR_ID_FILTER_DEFAULT ${VulkanSC_PCC_VENDOR_ID_FILTER_${CMAKE_MATCH_1}_${CMAKE_MATCH_2}})
                set(VulkanSC_PCC_DEVICE_ID_FILTER_DEFAULT ${VulkanSC_PCC_DEVICE_ID_FILTER_${CMAKE_MATCH_1}_${CMAKE_MATCH_2}})
                set(VulkanSC_PCC_DRIVER_ID_FILTER_DEFAULT ${VulkanSC_PCC_DRIVER_ID_FILTER_${CMAKE_MATCH_1}_${CMAKE_MATCH_2}})
            else()
                message(FATAL_ERROR "Unexpected regex error while matching selected VulkanSC PCC pipeline cache compiler: ${VulkanSC_PCC}")
            endif()
        else()
            list(JOIN VulkanSC_PCC_OPTIONS "\n\t* " VulkanSC_PCC_OPTIONS_LIST)
            message(FATAL_ERROR "No pipeline cache compiler configuration found matching VulkanSC_PCC: ${VulkanSC_PCC}.\n"
                                "Available options:\n\t* ${VulkanSC_PCC_OPTIONS_LIST}\n")
        endif()
    endif()

    set(VulkanSC_PCC_EXECUTABLE "${VulkanSC_PCC_EXECUTABLE_DEFAULT}" CACHE FILEPATH "Path to the VulkanSC pipeline cache compiler")
    set(VulkanSC_PCC_FLAGS "${VulkanSC_PCC_FLAGS_DEFAULT}" CACHE STRING "Flags used by the pipeline cache compiler")
    set(VulkanSC_PCC_DEBUG_FLAG "${VulkanSC_PCC_DEBUG_FLAG_DEFAULT}" CACHE STRING "Flag used by the pipeline cache compiler to embed SPIR-V debug info")
    set(VulkanSC_PCC_DEVICE_FLAGS "${VulkanSC_PCC_DEVICE_FLAGS_DEFAULT}" CACHE STRING "Flag used by the pipeline cache compiler to select targeting a specific device")
    set(VulkanSC_PCC_VENDOR_ID_FILTER "${VulkanSC_PCC_VENDOR_ID_FILTER_DEFAULT}" CACHE STRING "The VK_LOADER_VENDOR_ID_FILTER given to the Vulkan SC loader")
    set(VulkanSC_PCC_DEVICE_ID_FILTER "${VulkanSC_PCC_DEVICE_ID_FILTER_DEFAULT}" CACHE STRING "The VK_LOADER_DEVICE_ID_FILTER given to the Vulkan SC loader")
    set(VulkanSC_PCC_DRIVER_ID_FILTER "${VulkanSC_PCC_DRIVER_ID_FILTER_DEFAULT}" CACHE STRING "The VK_LOADER_DRIVER_ID_FILTER given to the Vulkan SC loader")
endif()

if(DEFINED CACHE{VulkanSC_PCC_DISCOVERY})
    add_custom_target(VulkanSC_PCC_ListOptions COMMAND "${CMAKE_COMMAND}" -D CACHE_DIR="${CMAKE_BINARY_DIR}" -P "${CMAKE_CURRENT_LIST_FILE}")
endif()
