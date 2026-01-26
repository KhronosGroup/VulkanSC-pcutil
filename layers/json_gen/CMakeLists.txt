# ~~~
# Copyright (c) 2025 The Khronos Group Inc.
# Copyright (c) 2025 RasterGrid Kft.
#
# SPDX-License-Identifier: Apache-2.0
# ~~~

# This variable enables downstream users to customize the CMake targets
# based on the target API variant (e.g. Vulkan SC)
set(LAYER_NAME "VkLayer_khronos_json_gen")

add_library(VulkanJSONGenLayer MODULE)
add_library(Vulkan::JSONGenLayer ALIAS VulkanJSONGenLayer)

target_sources(VulkanJSONGenLayer PRIVATE
    vk_json_layer.hpp
    vk_json_layer.cpp
    vk_json_layer_platform.cpp
    ${LAYER_NAME}.def
    allocator.cpp
)

target_include_directories(VulkanJSONGenLayer
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/generated"
        "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(VulkanJSONGenLayer
    PRIVATE
        Vulkan::Headers
        Vulkan::UtilityHeaders
        VulkanSC::PCJson
        Vulkan::SafeStruct
)

set_target_properties(VulkanJSONGenLayer PROPERTIES OUTPUT_NAME ${LAYER_NAME})

# There are 2 primary deliverables for the JSON gen layer.
# - The actual library VkLayer_khronos_json_gen.(dll|so)
# - The respective json file, VkLayer_khronos_json_gen.json
# This code generates the appropriate json for both local testing and the installation.
# NOTE: For WIN32 the JSON and dll MUST be placed in the same location, due to Win32 using a relative path for installation.
set(INPUT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/${LAYER_NAME}.json.in")
set(INTERMEDIATE_FILE "${CMAKE_CURRENT_BINARY_DIR}/json/${LAYER_NAME}.json")
set(OUTPUT_FILE_FINAL_NAME "${LAYER_NAME}.json")
set(JSON_LAYER_NAME "VK_LAYER_KHRONOS_json_gen")

if(WIN32)
    set(JSON_LIBRARY_PATH ".\\\\${CMAKE_SHARED_LIBRARY_PREFIX}${LAYER_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}")
else()
    set(JSON_LIBRARY_PATH "./${CMAKE_SHARED_LIBRARY_PREFIX}${LAYER_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}")
endif()

configure_file("${INPUT_FILE}" "${INTERMEDIATE_FILE}" @ONLY)

# To support both multi/single configuration generators just copy the json to the correct directory
add_custom_command(TARGET VulkanJSONGenLayer POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${INTERMEDIATE_FILE}" $<TARGET_FILE_DIR:VulkanJSONGenLayer>/${OUTPUT_FILE_FINAL_NAME}
)

# For UNIX-based systems, `library_path` should not contain a relative path (indicated by "./") before installing to system directories
# This json isn't used for regular local development, it's used for installation
if(PCU_ENABLE_INSTALL)
    include(GNUInstallDirs)

    set(LAYER_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR})
    if (WIN32)
        set(LAYER_INSTALL_DIR ${CMAKE_INSTALL_BINDIR}) # WIN32/MINGW expect the dll in the `bin` dir, this matches our WIN32 SDK process
    endif()

    if (UNIX)
        set(UNIX_INTERMEDIATE_FILE "${CMAKE_CURRENT_BINARY_DIR}/json/unix_install_validation.json")
        set(JSON_LIBRARY_PATH "${CMAKE_SHARED_LIBRARY_PREFIX}${LAYER_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}")

        configure_file(${INPUT_FILE} ${UNIX_INTERMEDIATE_FILE} @ONLY)

        install(FILES ${UNIX_INTERMEDIATE_FILE} DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/vulkan/explicit_layer.d RENAME ${OUTPUT_FILE_FINAL_NAME})
    endif()

    if (WIN32)
        install(FILES ${INTERMEDIATE_FILE} DESTINATION ${LAYER_INSTALL_DIR} RENAME ${OUTPUT_FILE_FINAL_NAME})
    endif()
    if (MSVC)
        install(FILES $<TARGET_PDB_FILE:VulkanJSONGenLayer> DESTINATION ${LAYER_INSTALL_DIR} OPTIONAL)
    endif()

    install(TARGETS VulkanJSONGenLayer EXPORT VulkanJSONGenLayerConfig DESTINATION ${LAYER_INSTALL_DIR})
endif()
