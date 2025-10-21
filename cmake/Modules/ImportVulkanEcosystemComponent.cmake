# ~~~
# Copyright (c) 2025 The Khronos Group Inc.
# Copyright (c) 2025 RasterGrid Kft.
#
# SPDX-License-Identifier: Apache-2.0
# ~~~

# This module helps consume Vulkan ecosystem components in the Vulkan SC ecosystem.
# Historically, Vulkan SC components share the same imported name as their original
# counterparts, rendering them incompatible in the same build. This module imports
# Vulkan components not under their original Vulkan:: CMake namespace, but under Vk::
# The install location has to be excplicitly specified to avoid oridering issues
# with find_package() around variables like CMAKE_PREFIX_PATH. This module is intended
# to interface trivially with update_deps.py and is thus not multi-config friendly.
macro(find_vk_package PACKAGE_NAME INSTALL_LOCATION)
    if(${PACKAGE_NAME} STREQUAL VulkanLoader)
        add_library(VkLoader SHARED IMPORTED GLOBAL)
        add_library(Vk::Loader ALIAS VkLoader)
        if(WIN32)
            set_target_properties(VkLoader PROPERTIES
                IMPORTED_IMPLIB "${INSTALL_LOCATION}/lib/${CMAKE_IMPORT_LIBRARY_PREFIX}vulkan-1${CMAKE_IMPORT_LIBRARY_SUFFIX}"
                IMPORTED_LOCATION "${INSTALL_LOCATION}/bin/${CMAKE_SHARED_LIBRARY_PREFIX}vulkan-1${CMAKE_SHARED_LIBRARY_SUFFIX}"
                INTERFACE_LINK_LIBRARIES Vulkan::Headers)
        else()
            set_target_properties(VkLoader PROPERTIES
                IMPORTED_LOCATION "${INSTALL_LOCATION}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}vulkan${CMAKE_SHARED_LIBRARY_SUFFIX}"
                INTERFACE_LINK_LIBRARIES Vulkan::Headers)
        endif()
    else()
        message(FATAL_ERROR "Unknkown Vulkan ecosystem component.")
    endif()
endmacro()