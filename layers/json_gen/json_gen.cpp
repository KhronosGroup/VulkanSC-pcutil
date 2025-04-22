
/*
** Copyright (c) 2025 The Khronos Group Inc.
** Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
**
** SPDX-License-Identifier: Apache-2.0
*/

/*
 * This file is generated from the Khronos Vulkan XML API Registry.
 */

#include "vulkan/vk_layer.h"
#include "vulkan/vulkan.h"
#include "vk_layer_table.h"
#include "vulkan_json_layer_data.hpp"
#include "vulkan_json_layer.hpp"

// TODO: vulkan_json_layer.hpp should not include windows.h, as it does not use
//       WinAPI types on its interface, only in its implementation. Some macros
//       need undefining, as they conflict with dispatch table members. It's
//       safe to undefine them here in a .cpp file where we know we won't rely
//       on them any longer.
#ifdef _WIN32
#undef CreateSemaphore
#undef CreateEvent
#endif

#include <algorithm>
#include <chrono>
#include <fstream>
#include <mutex>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string.h>
#include <string>
#include <type_traits>
#include <map>
#include <thread>
#include <unordered_map>
#include <vector>
#include <unordered_set>
#include <utility>

//============================= API EntryPoints =============================//

// Specifically implemented functions

EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCreateInstance(const VkInstanceCreateInfo* pCreateInfo,
                                                                const VkAllocationCallbacks* pAllocator, VkInstance* pInstance) {
    // Get the function pointer
    VkLayerInstanceCreateInfo* chain_info = get_chain_info(pCreateInfo, VK_LAYER_LINK_INFO);
    assert(chain_info->u.pLayerInfo != 0);
    PFN_vkGetInstanceProcAddr fpGetInstanceProcAddr = chain_info->u.pLayerInfo->pfnNextGetInstanceProcAddr;
    assert(fpGetInstanceProcAddr != 0);
    PFN_vkCreateInstance fpCreateInstance = (PFN_vkCreateInstance)fpGetInstanceProcAddr(NULL, "vkCreateInstance");
    if (fpCreateInstance == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    // Call the function and create the dispatch table
    chain_info->u.pLayerInfo = chain_info->u.pLayerInfo->pNext;
    VkResult result = fpCreateInstance(pCreateInfo, pAllocator, pInstance);
    if (result == VK_SUCCESS) {
        initInstanceTable(*pInstance, fpGetInstanceProcAddr);
    }
    if (pCreateInfo != nullptr && pCreateInfo->pApplicationInfo != nullptr) {
        uint32_t major = VK_API_VERSION_MAJOR(pCreateInfo->pApplicationInfo->apiVersion);
        uint32_t minor = VK_API_VERSION_MINOR(pCreateInfo->pApplicationInfo->apiVersion);
        // Need extension version for private data slot feature.
        if (major <= 1 && minor < 3) {
            vk_json::s_pipe.m_usePrivateDataExtension = true;
        }
    } else {
        // If pcreateInfo->pApplicationInfo is nullptr, then it is assumed to be VK1.0
        vk_json::s_pipe.m_usePrivateDataExtension = true;
    }

    return result;
}

EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkDestroyInstance(VkInstance instance, const VkAllocationCallbacks* pAllocator) {
    // Destroy the dispatch table
    dispatch_key key = get_dispatch_key(instance);
    instance_dispatch_table(instance)->DestroyInstance(instance, pAllocator);
    destroy_instance_dispatch_table(key);
}

EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCreateDevice(VkPhysicalDevice physicalDevice,
                                                              const VkDeviceCreateInfo* pCreateInfo,
                                                              const VkAllocationCallbacks* pAllocator, VkDevice* pDevice) {
    // Get the function pointer
    VkLayerDeviceCreateInfo* chain_info = get_chain_info(pCreateInfo, VK_LAYER_LINK_INFO);
    assert(chain_info->u.pLayerInfo != 0);
    PFN_vkGetInstanceProcAddr fpGetInstanceProcAddr = chain_info->u.pLayerInfo->pfnNextGetInstanceProcAddr;
    PFN_vkGetDeviceProcAddr fpGetDeviceProcAddr = chain_info->u.pLayerInfo->pfnNextGetDeviceProcAddr;
    PFN_vkCreateDevice fpCreateDevice = (PFN_vkCreateDevice)fpGetInstanceProcAddr(NULL, "vkCreateDevice");
    if (fpCreateDevice == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    // We have seen what the app has requested in vkCreateInstance, and we have assumed that privateData feature is supported.
    // Now, let us actually validate if that is supported.
    bool isPrivateDataExtSupported{false};
    if (vk_json::s_pipe.m_usePrivateDataExtension == true) {
        uint32_t count;
        instance_dispatch_table(physicalDevice)->EnumerateDeviceExtensionProperties(physicalDevice, nullptr, &count, nullptr);
        std::vector<VkExtensionProperties> extensions(count);
        instance_dispatch_table(physicalDevice)
            ->EnumerateDeviceExtensionProperties(physicalDevice, nullptr, &count, extensions.data());
        for (auto& extension : extensions) {
            if (strncmp(extension.extensionName, "VK_EXT_private_data", strlen("VK_EXT_private_data")) == 0) {
                isPrivateDataExtSupported = true;
                break;
            }
        }
    }
    // If neither VK 1.3 or VK_EXT_private_data are supported, then we cannot use this layer.
    if (vk_json::s_pipe.m_usePrivateDataExtension == true && isPrivateDataExtSupported == false) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    // Keep a copy of it since we modify this struct to enable some features if they aren't present.
    VkDeviceCreateInfo devCreateInfoCopy = *pCreateInfo;
    std::vector<const char*> extList;
    if (vk_json::s_pipe.m_usePrivateDataExtension) {
        bool isPrivDataExtSet{false};
        for (uint32_t i = 0; i < pCreateInfo->enabledExtensionCount; i++) {
            const char* ptr = const_cast<const char*>(*pCreateInfo->ppEnabledExtensionNames);
            extList.push_back(ptr);
            if (strncmp(ptr, "VK_EXT_private_data", strlen("VK_EXT_private_data")) == 0) {
                isPrivDataExtSet = true;
            }
            ptr++;
        }
        if (isPrivDataExtSet == false) {
            extList.push_back("VK_EXT_private_data");
        }
        devCreateInfoCopy.enabledExtensionCount = static_cast<uint32_t>(extList.size());
        devCreateInfoCopy.ppEnabledExtensionNames = extList.data();
    }
    bool isFeatureSet{false};
    // The feature struct needs to be set even when the extension is used.
    VkBaseOutStructure* pnext = (VkBaseOutStructure*)devCreateInfoCopy.pNext;
    while (pnext != nullptr) {
        if (pnext->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES) {
            VkPhysicalDeviceVulkan13Features* f = (VkPhysicalDeviceVulkan13Features*)pnext;
            f->privateData = VK_TRUE;
            isFeatureSet = true;
            break;
        }
        if (pnext->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRIVATE_DATA_FEATURES) {
            VkPhysicalDevicePrivateDataFeatures* f = (VkPhysicalDevicePrivateDataFeatures*)pnext;
            f->privateData = VK_TRUE;
            isFeatureSet = true;
            break;
        }
        pnext = pnext->pNext;
    }
    VkPhysicalDevicePrivateDataFeatures privDataFeats{};
    privDataFeats.privateData = VK_TRUE;
    privDataFeats.pNext = (void*)devCreateInfoCopy.pNext;
    if (isFeatureSet == false) {
        devCreateInfoCopy.pNext = &privDataFeats;
    }
    vk_json::s_pipe.setDevice(pCreateInfo);

    // Call the function and create the dispatch table
    chain_info->u.pLayerInfo = chain_info->u.pLayerInfo->pNext;
    VkResult result = fpCreateDevice(physicalDevice, &devCreateInfoCopy, pAllocator, pDevice);
    if (result == VK_SUCCESS) {
        initDeviceTable(*pDevice, fpGetDeviceProcAddr);
    }

    return result;
}

EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkDestroyDevice(VkDevice device, const VkAllocationCallbacks* pAllocator) {
    vk_json::s_pipe.destroy(device);
    // Destroy the dispatch table
    dispatch_key key = get_dispatch_key(device);
    device_dispatch_table(device)->DestroyDevice(device, pAllocator);
    destroy_device_dispatch_table(key);
}

EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceExtensionProperties(const char* pLayerName,
                                                                                      uint32_t* pPropertyCount,
                                                                                      VkExtensionProperties* pProperties) {
    return util_GetExtensionProperties(0, NULL, pPropertyCount, pProperties);
}

EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceLayerProperties(uint32_t* pPropertyCount,
                                                                                  VkLayerProperties* pProperties) {
    static const VkLayerProperties layerProperties[] = {{
        "VK_LAYER_KHRONOS_json_gen",
        VK_MAKE_VERSION(1, 2, VK_HEADER_VERSION),  // specVersion
        VK_MAKE_VERSION(0, 2, 0),                  // implementationVersion
        "layer: json_gen",
    }};

    return util_GetLayerProperties(ARRAY_SIZE(layerProperties), layerProperties, pPropertyCount, pProperties);
}

EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateDeviceLayerProperties(VkPhysicalDevice physicalDevice,
                                                                                uint32_t* pPropertyCount,
                                                                                VkLayerProperties* pProperties) {
    static const VkLayerProperties layerProperties[] = {{
        "VK_LAYER_KHRONOS_json_gen",
        VK_MAKE_VERSION(1, 2, VK_HEADER_VERSION),
        VK_MAKE_VERSION(0, 2, 0),
        "layer: json_gen",
    }};

    return util_GetLayerProperties(ARRAY_SIZE(layerProperties), layerProperties, pPropertyCount, pProperties);
}

EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkQueuePresentKHR(VkQueue queue, const VkPresentInfoKHR* pPresentInfo) {
    VkResult result = device_dispatch_table(queue)->QueuePresentKHR(queue, pPresentInfo);

    return result;
}

// Autogen instance functions

EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkReleaseDisplayEXT(VkPhysicalDevice physicalDevice, VkDisplayKHR display) {
    VkResult result = instance_dispatch_table(physicalDevice)->ReleaseDisplayEXT(physicalDevice, display);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceCooperativeMatrixPropertiesKHR(
    VkPhysicalDevice physicalDevice, uint32_t* pPropertyCount, VkCooperativeMatrixPropertiesKHR* pProperties) {
    VkResult result = instance_dispatch_table(physicalDevice)
                          ->GetPhysicalDeviceCooperativeMatrixPropertiesKHR(physicalDevice, pPropertyCount, pProperties);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceSurfaceCapabilities2EXT(
    VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkSurfaceCapabilities2EXT* pSurfaceCapabilities) {
    VkResult result = instance_dispatch_table(physicalDevice)
                          ->GetPhysicalDeviceSurfaceCapabilities2EXT(physicalDevice, surface, pSurfaceCapabilities);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkEnumeratePhysicalDevices(VkInstance instance, uint32_t* pPhysicalDeviceCount,
                                                                          VkPhysicalDevice* pPhysicalDevices) {
    VkResult result = instance_dispatch_table(instance)->EnumeratePhysicalDevices(instance, pPhysicalDeviceCount, pPhysicalDevices);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceCooperativeMatrixPropertiesNV(
    VkPhysicalDevice physicalDevice, uint32_t* pPropertyCount, VkCooperativeMatrixPropertiesNV* pProperties) {
    VkResult result = instance_dispatch_table(physicalDevice)
                          ->GetPhysicalDeviceCooperativeMatrixPropertiesNV(physicalDevice, pPropertyCount, pProperties);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceImageFormatProperties(
    VkPhysicalDevice physicalDevice, VkFormat format, VkImageType type, VkImageTiling tiling, VkImageUsageFlags usage,
    VkImageCreateFlags flags, VkImageFormatProperties* pImageFormatProperties) {
    VkResult result =
        instance_dispatch_table(physicalDevice)
            ->GetPhysicalDeviceImageFormatProperties(physicalDevice, format, type, tiling, usage, flags, pImageFormatProperties);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV(
    VkPhysicalDevice physicalDevice, uint32_t* pCombinationCount, VkFramebufferMixedSamplesCombinationNV* pCombinations) {
    VkResult result =
        instance_dispatch_table(physicalDevice)
            ->GetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV(physicalDevice, pCombinationCount, pCombinations);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceCooperativeVectorPropertiesNV(
    VkPhysicalDevice physicalDevice, uint32_t* pPropertyCount, VkCooperativeVectorPropertiesNV* pProperties) {
    VkResult result = instance_dispatch_table(physicalDevice)
                          ->GetPhysicalDeviceCooperativeVectorPropertiesNV(physicalDevice, pPropertyCount, pProperties);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceExternalImageFormatPropertiesNV(
    VkPhysicalDevice physicalDevice, VkFormat format, VkImageType type, VkImageTiling tiling, VkImageUsageFlags usage,
    VkImageCreateFlags flags, VkExternalMemoryHandleTypeFlagsNV externalHandleType,
    VkExternalImageFormatPropertiesNV* pExternalImageFormatProperties) {
    VkResult result = instance_dispatch_table(physicalDevice)
                          ->GetPhysicalDeviceExternalImageFormatPropertiesNV(physicalDevice, format, type, tiling, usage, flags,
                                                                             externalHandleType, pExternalImageFormatProperties);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCreateHeadlessSurfaceEXT(VkInstance instance,
                                                                          const VkHeadlessSurfaceCreateInfoEXT* pCreateInfo,
                                                                          const VkAllocationCallbacks* pAllocator,
                                                                          VkSurfaceKHR* pSurface) {
    VkResult result = instance_dispatch_table(instance)->CreateHeadlessSurfaceEXT(instance, pCreateInfo, pAllocator, pSurface);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR(
    VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, uint32_t* pCounterCount, VkPerformanceCounterKHR* pCounters,
    VkPerformanceCounterDescriptionKHR* pCounterDescriptions) {
    VkResult result = instance_dispatch_table(physicalDevice)
                          ->EnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR(
                              physicalDevice, queueFamilyIndex, pCounterCount, pCounters, pCounterDescriptions);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceImageFormatProperties2KHR(
    VkPhysicalDevice physicalDevice, const VkPhysicalDeviceImageFormatInfo2* pImageFormatInfo,
    VkImageFormatProperties2* pImageFormatProperties) {
    VkResult result = instance_dispatch_table(physicalDevice)
                          ->GetPhysicalDeviceImageFormatProperties2KHR(physicalDevice, pImageFormatInfo, pImageFormatProperties);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL
vkGetPhysicalDeviceSurfaceCapabilities2KHR(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceSurfaceInfo2KHR* pSurfaceInfo,
                                           VkSurfaceCapabilities2KHR* pSurfaceCapabilities) {
    VkResult result = instance_dispatch_table(physicalDevice)
                          ->GetPhysicalDeviceSurfaceCapabilities2KHR(physicalDevice, pSurfaceInfo, pSurfaceCapabilities);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL
vkGetPhysicalDeviceSurfaceFormats2KHR(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceSurfaceInfo2KHR* pSurfaceInfo,
                                      uint32_t* pSurfaceFormatCount, VkSurfaceFormat2KHR* pSurfaceFormats) {
    VkResult result = instance_dispatch_table(physicalDevice)
                          ->GetPhysicalDeviceSurfaceFormats2KHR(physicalDevice, pSurfaceInfo, pSurfaceFormatCount, pSurfaceFormats);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceDisplayProperties2KHR(VkPhysicalDevice physicalDevice,
                                                                                        uint32_t* pPropertyCount,
                                                                                        VkDisplayProperties2KHR* pProperties) {
    VkResult result = instance_dispatch_table(physicalDevice)
                          ->GetPhysicalDeviceDisplayProperties2KHR(physicalDevice, pPropertyCount, pProperties);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkEnumeratePhysicalDeviceGroupsKHR(
    VkInstance instance, uint32_t* pPhysicalDeviceGroupCount, VkPhysicalDeviceGroupProperties* pPhysicalDeviceGroupProperties) {
    VkResult result = instance_dispatch_table(instance)->EnumeratePhysicalDeviceGroupsKHR(instance, pPhysicalDeviceGroupCount,
                                                                                          pPhysicalDeviceGroupProperties);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceDisplayPlaneProperties2KHR(
    VkPhysicalDevice physicalDevice, uint32_t* pPropertyCount, VkDisplayPlaneProperties2KHR* pProperties) {
    VkResult result = instance_dispatch_table(physicalDevice)
                          ->GetPhysicalDeviceDisplayPlaneProperties2KHR(physicalDevice, pPropertyCount, pProperties);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetDisplayModeProperties2KHR(VkPhysicalDevice physicalDevice, VkDisplayKHR display,
                                                                              uint32_t* pPropertyCount,
                                                                              VkDisplayModeProperties2KHR* pProperties) {
    VkResult result =
        instance_dispatch_table(physicalDevice)->GetDisplayModeProperties2KHR(physicalDevice, display, pPropertyCount, pProperties);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetDisplayPlaneCapabilities2KHR(VkPhysicalDevice physicalDevice,
                                                                                 const VkDisplayPlaneInfo2KHR* pDisplayPlaneInfo,
                                                                                 VkDisplayPlaneCapabilities2KHR* pCapabilities) {
    VkResult result =
        instance_dispatch_table(physicalDevice)->GetDisplayPlaneCapabilities2KHR(physicalDevice, pDisplayPlaneInfo, pCapabilities);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceCooperativeMatrixFlexibleDimensionsPropertiesNV(
    VkPhysicalDevice physicalDevice, uint32_t* pPropertyCount, VkCooperativeMatrixFlexibleDimensionsPropertiesNV* pProperties) {
    VkResult result =
        instance_dispatch_table(physicalDevice)
            ->GetPhysicalDeviceCooperativeMatrixFlexibleDimensionsPropertiesNV(physicalDevice, pPropertyCount, pProperties);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugUtilsMessengerEXT(VkInstance instance,
                                                                              const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                                                              const VkAllocationCallbacks* pAllocator,
                                                                              VkDebugUtilsMessengerEXT* pMessenger) {
    VkResult result =
        instance_dispatch_table(instance)->CreateDebugUtilsMessengerEXT(instance, pCreateInfo, pAllocator, pMessenger);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR(
    VkPhysicalDevice physicalDevice, const VkPhysicalDeviceVideoEncodeQualityLevelInfoKHR* pQualityLevelInfo,
    VkVideoEncodeQualityLevelPropertiesKHR* pQualityLevelProperties) {
    VkResult result =
        instance_dispatch_table(physicalDevice)
            ->GetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR(physicalDevice, pQualityLevelInfo, pQualityLevelProperties);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceCalibrateableTimeDomainsKHR(VkPhysicalDevice physicalDevice,
                                                                                              uint32_t* pTimeDomainCount,
                                                                                              VkTimeDomainKHR* pTimeDomains) {
    VkResult result = instance_dispatch_table(physicalDevice)
                          ->GetPhysicalDeviceCalibrateableTimeDomainsKHR(physicalDevice, pTimeDomainCount, pTimeDomains);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceOpticalFlowImageFormatsNV(
    VkPhysicalDevice physicalDevice, const VkOpticalFlowImageFormatInfoNV* pOpticalFlowImageFormatInfo, uint32_t* pFormatCount,
    VkOpticalFlowImageFormatPropertiesNV* pImageFormatProperties) {
    VkResult result = instance_dispatch_table(physicalDevice)
                          ->GetPhysicalDeviceOpticalFlowImageFormatsNV(physicalDevice, pOpticalFlowImageFormatInfo, pFormatCount,
                                                                       pImageFormatProperties);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceCalibrateableTimeDomainsEXT(VkPhysicalDevice physicalDevice,
                                                                                              uint32_t* pTimeDomainCount,
                                                                                              VkTimeDomainKHR* pTimeDomains) {
    VkResult result = instance_dispatch_table(physicalDevice)
                          ->GetPhysicalDeviceCalibrateableTimeDomainsEXT(physicalDevice, pTimeDomainCount, pTimeDomains);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkEnumeratePhysicalDeviceGroups(
    VkInstance instance, uint32_t* pPhysicalDeviceGroupCount, VkPhysicalDeviceGroupProperties* pPhysicalDeviceGroupProperties) {
    VkResult result = instance_dispatch_table(instance)->EnumeratePhysicalDeviceGroups(instance, pPhysicalDeviceGroupCount,
                                                                                       pPhysicalDeviceGroupProperties);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDevicePresentRectanglesKHR(VkPhysicalDevice physicalDevice,
                                                                                       VkSurfaceKHR surface, uint32_t* pRectCount,
                                                                                       VkRect2D* pRects) {
    VkResult result =
        instance_dispatch_table(physicalDevice)->GetPhysicalDevicePresentRectanglesKHR(physicalDevice, surface, pRectCount, pRects);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkAcquireDrmDisplayEXT(VkPhysicalDevice physicalDevice, int32_t drmFd,
                                                                      VkDisplayKHR display) {
    VkResult result = instance_dispatch_table(physicalDevice)->AcquireDrmDisplayEXT(physicalDevice, drmFd, display);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetDrmDisplayEXT(VkPhysicalDevice physicalDevice, int32_t drmFd,
                                                                  uint32_t connectorId, VkDisplayKHR* display) {
    VkResult result = instance_dispatch_table(physicalDevice)->GetDrmDisplayEXT(physicalDevice, drmFd, connectorId, display);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceDisplayPropertiesKHR(VkPhysicalDevice physicalDevice,
                                                                                       uint32_t* pPropertyCount,
                                                                                       VkDisplayPropertiesKHR* pProperties) {
    VkResult result =
        instance_dispatch_table(physicalDevice)->GetPhysicalDeviceDisplayPropertiesKHR(physicalDevice, pPropertyCount, pProperties);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceDisplayPlanePropertiesKHR(
    VkPhysicalDevice physicalDevice, uint32_t* pPropertyCount, VkDisplayPlanePropertiesKHR* pProperties) {
    VkResult result = instance_dispatch_table(physicalDevice)
                          ->GetPhysicalDeviceDisplayPlanePropertiesKHR(physicalDevice, pPropertyCount, pProperties);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceToolProperties(VkPhysicalDevice physicalDevice,
                                                                                 uint32_t* pToolCount,
                                                                                 VkPhysicalDeviceToolProperties* pToolProperties) {
    VkResult result =
        instance_dispatch_table(physicalDevice)->GetPhysicalDeviceToolProperties(physicalDevice, pToolCount, pToolProperties);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetDisplayPlaneSupportedDisplaysKHR(VkPhysicalDevice physicalDevice,
                                                                                     uint32_t planeIndex, uint32_t* pDisplayCount,
                                                                                     VkDisplayKHR* pDisplays) {
    VkResult result = instance_dispatch_table(physicalDevice)
                          ->GetDisplayPlaneSupportedDisplaysKHR(physicalDevice, planeIndex, pDisplayCount, pDisplays);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetDisplayModePropertiesKHR(VkPhysicalDevice physicalDevice, VkDisplayKHR display,
                                                                             uint32_t* pPropertyCount,
                                                                             VkDisplayModePropertiesKHR* pProperties) {
    VkResult result =
        instance_dispatch_table(physicalDevice)->GetDisplayModePropertiesKHR(physicalDevice, display, pPropertyCount, pProperties);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetDisplayPlaneCapabilitiesKHR(VkPhysicalDevice physicalDevice,
                                                                                VkDisplayModeKHR mode, uint32_t planeIndex,
                                                                                VkDisplayPlaneCapabilitiesKHR* pCapabilities) {
    VkResult result =
        instance_dispatch_table(physicalDevice)->GetDisplayPlaneCapabilitiesKHR(physicalDevice, mode, planeIndex, pCapabilities);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCreateDisplayModeKHR(VkPhysicalDevice physicalDevice, VkDisplayKHR display,
                                                                      const VkDisplayModeCreateInfoKHR* pCreateInfo,
                                                                      const VkAllocationCallbacks* pAllocator,
                                                                      VkDisplayModeKHR* pMode) {
    VkResult result =
        instance_dispatch_table(physicalDevice)->CreateDisplayModeKHR(physicalDevice, display, pCreateInfo, pAllocator, pMode);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCreateDisplayPlaneSurfaceKHR(VkInstance instance,
                                                                              const VkDisplaySurfaceCreateInfoKHR* pCreateInfo,
                                                                              const VkAllocationCallbacks* pAllocator,
                                                                              VkSurfaceKHR* pSurface) {
    VkResult result = instance_dispatch_table(instance)->CreateDisplayPlaneSurfaceKHR(instance, pCreateInfo, pAllocator, pSurface);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL
vkGetPhysicalDeviceFragmentShadingRatesKHR(VkPhysicalDevice physicalDevice, uint32_t* pFragmentShadingRateCount,
                                           VkPhysicalDeviceFragmentShadingRateKHR* pFragmentShadingRates) {
    VkResult result =
        instance_dispatch_table(physicalDevice)
            ->GetPhysicalDeviceFragmentShadingRatesKHR(physicalDevice, pFragmentShadingRateCount, pFragmentShadingRates);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL
vkGetPhysicalDeviceImageFormatProperties2(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceImageFormatInfo2* pImageFormatInfo,
                                          VkImageFormatProperties2* pImageFormatProperties) {
    VkResult result = instance_dispatch_table(physicalDevice)
                          ->GetPhysicalDeviceImageFormatProperties2(physicalDevice, pImageFormatInfo, pImageFormatProperties);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceSurfaceSupportKHR(VkPhysicalDevice physicalDevice,
                                                                                    uint32_t queueFamilyIndex, VkSurfaceKHR surface,
                                                                                    VkBool32* pSupported) {
    VkResult result = instance_dispatch_table(physicalDevice)
                          ->GetPhysicalDeviceSurfaceSupportKHR(physicalDevice, queueFamilyIndex, surface, pSupported);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceSurfaceFormatsKHR(VkPhysicalDevice physicalDevice,
                                                                                    VkSurfaceKHR surface,
                                                                                    uint32_t* pSurfaceFormatCount,
                                                                                    VkSurfaceFormatKHR* pSurfaceFormats) {
    VkResult result = instance_dispatch_table(physicalDevice)
                          ->GetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, pSurfaceFormatCount, pSurfaceFormats);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
    VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkSurfaceCapabilitiesKHR* pSurfaceCapabilities) {
    VkResult result = instance_dispatch_table(physicalDevice)
                          ->GetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, pSurfaceCapabilities);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceVideoCapabilitiesKHR(VkPhysicalDevice physicalDevice,
                                                                                       const VkVideoProfileInfoKHR* pVideoProfile,
                                                                                       VkVideoCapabilitiesKHR* pCapabilities) {
    VkResult result = instance_dispatch_table(physicalDevice)
                          ->GetPhysicalDeviceVideoCapabilitiesKHR(physicalDevice, pVideoProfile, pCapabilities);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceSurfacePresentModesKHR(VkPhysicalDevice physicalDevice,
                                                                                         VkSurfaceKHR surface,
                                                                                         uint32_t* pPresentModeCount,
                                                                                         VkPresentModeKHR* pPresentModes) {
    VkResult result = instance_dispatch_table(physicalDevice)
                          ->GetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, pPresentModeCount, pPresentModes);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceVideoFormatPropertiesKHR(
    VkPhysicalDevice physicalDevice, const VkPhysicalDeviceVideoFormatInfoKHR* pVideoFormatInfo,
    uint32_t* pVideoFormatPropertyCount, VkVideoFormatPropertiesKHR* pVideoFormatProperties) {
    VkResult result = instance_dispatch_table(physicalDevice)
                          ->GetPhysicalDeviceVideoFormatPropertiesKHR(physicalDevice, pVideoFormatInfo, pVideoFormatPropertyCount,
                                                                      pVideoFormatProperties);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceToolPropertiesEXT(
    VkPhysicalDevice physicalDevice, uint32_t* pToolCount, VkPhysicalDeviceToolProperties* pToolProperties) {
    VkResult result =
        instance_dispatch_table(physicalDevice)->GetPhysicalDeviceToolPropertiesEXT(physicalDevice, pToolCount, pToolProperties);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugReportCallbackEXT(VkInstance instance,
                                                                              const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
                                                                              const VkAllocationCallbacks* pAllocator,
                                                                              VkDebugReportCallbackEXT* pCallback) {
    VkResult result = instance_dispatch_table(instance)->CreateDebugReportCallbackEXT(instance, pCreateInfo, pAllocator, pCallback);

    return result;
}

EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceExternalFencePropertiesKHR(
    VkPhysicalDevice physicalDevice, const VkPhysicalDeviceExternalFenceInfo* pExternalFenceInfo,
    VkExternalFenceProperties* pExternalFenceProperties) {
    instance_dispatch_table(physicalDevice)
        ->GetPhysicalDeviceExternalFencePropertiesKHR(physicalDevice, pExternalFenceInfo, pExternalFenceProperties);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceFeatures(VkPhysicalDevice physicalDevice,
                                                                       VkPhysicalDeviceFeatures* pFeatures) {
    instance_dispatch_table(physicalDevice)->GetPhysicalDeviceFeatures(physicalDevice, pFeatures);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceSparseImageFormatProperties(
    VkPhysicalDevice physicalDevice, VkFormat format, VkImageType type, VkSampleCountFlagBits samples, VkImageUsageFlags usage,
    VkImageTiling tiling, uint32_t* pPropertyCount, VkSparseImageFormatProperties* pProperties) {
    instance_dispatch_table(physicalDevice)
        ->GetPhysicalDeviceSparseImageFormatProperties(physicalDevice, format, type, samples, usage, tiling, pPropertyCount,
                                                       pProperties);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format,
                                                                               VkFormatProperties* pFormatProperties) {
    instance_dispatch_table(physicalDevice)->GetPhysicalDeviceFormatProperties(physicalDevice, format, pFormatProperties);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceProperties(VkPhysicalDevice physicalDevice,
                                                                         VkPhysicalDeviceProperties* pProperties) {
    instance_dispatch_table(physicalDevice)->GetPhysicalDeviceProperties(physicalDevice, pProperties);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceQueueFamilyProperties(
    VkPhysicalDevice physicalDevice, uint32_t* pQueueFamilyPropertyCount, VkQueueFamilyProperties* pQueueFamilyProperties) {
    instance_dispatch_table(physicalDevice)
        ->GetPhysicalDeviceQueueFamilyProperties(physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL
vkGetPhysicalDeviceMemoryProperties(VkPhysicalDevice physicalDevice, VkPhysicalDeviceMemoryProperties* pMemoryProperties) {
    instance_dispatch_table(physicalDevice)->GetPhysicalDeviceMemoryProperties(physicalDevice, pMemoryProperties);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceFeatures2KHR(VkPhysicalDevice physicalDevice,
                                                                           VkPhysicalDeviceFeatures2* pFeatures) {
    instance_dispatch_table(physicalDevice)->GetPhysicalDeviceFeatures2KHR(physicalDevice, pFeatures);
    set_layer_supported_features(pFeatures);
    vk_json::s_pipe.setPhysicalDeviceFeatures2(pFeatures);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR(
    VkPhysicalDevice physicalDevice, const VkQueryPoolPerformanceCreateInfoKHR* pPerformanceQueryCreateInfo, uint32_t* pNumPasses) {
    instance_dispatch_table(physicalDevice)
        ->GetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR(physicalDevice, pPerformanceQueryCreateInfo, pNumPasses);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceProperties2KHR(VkPhysicalDevice physicalDevice,
                                                                             VkPhysicalDeviceProperties2* pProperties) {
    instance_dispatch_table(physicalDevice)->GetPhysicalDeviceProperties2KHR(physicalDevice, pProperties);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceFormatProperties2KHR(VkPhysicalDevice physicalDevice, VkFormat format,
                                                                                   VkFormatProperties2* pFormatProperties) {
    instance_dispatch_table(physicalDevice)->GetPhysicalDeviceFormatProperties2KHR(physicalDevice, format, pFormatProperties);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceQueueFamilyProperties2KHR(
    VkPhysicalDevice physicalDevice, uint32_t* pQueueFamilyPropertyCount, VkQueueFamilyProperties2* pQueueFamilyProperties) {
    instance_dispatch_table(physicalDevice)
        ->GetPhysicalDeviceQueueFamilyProperties2KHR(physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL
vkGetPhysicalDeviceMemoryProperties2KHR(VkPhysicalDevice physicalDevice, VkPhysicalDeviceMemoryProperties2* pMemoryProperties) {
    instance_dispatch_table(physicalDevice)->GetPhysicalDeviceMemoryProperties2KHR(physicalDevice, pMemoryProperties);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceSparseImageFormatProperties2KHR(
    VkPhysicalDevice physicalDevice, const VkPhysicalDeviceSparseImageFormatInfo2* pFormatInfo, uint32_t* pPropertyCount,
    VkSparseImageFormatProperties2* pProperties) {
    instance_dispatch_table(physicalDevice)
        ->GetPhysicalDeviceSparseImageFormatProperties2KHR(physicalDevice, pFormatInfo, pPropertyCount, pProperties);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceExternalBufferPropertiesKHR(
    VkPhysicalDevice physicalDevice, const VkPhysicalDeviceExternalBufferInfo* pExternalBufferInfo,
    VkExternalBufferProperties* pExternalBufferProperties) {
    instance_dispatch_table(physicalDevice)
        ->GetPhysicalDeviceExternalBufferPropertiesKHR(physicalDevice, pExternalBufferInfo, pExternalBufferProperties);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceExternalSemaphorePropertiesKHR(
    VkPhysicalDevice physicalDevice, const VkPhysicalDeviceExternalSemaphoreInfo* pExternalSemaphoreInfo,
    VkExternalSemaphoreProperties* pExternalSemaphoreProperties) {
    instance_dispatch_table(physicalDevice)
        ->GetPhysicalDeviceExternalSemaphorePropertiesKHR(physicalDevice, pExternalSemaphoreInfo, pExternalSemaphoreProperties);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkDestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT messenger,
                                                                           const VkAllocationCallbacks* pAllocator) {
    instance_dispatch_table(instance)->DestroyDebugUtilsMessengerEXT(instance, messenger, pAllocator);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkSubmitDebugUtilsMessageEXT(VkInstance instance,
                                                                        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                                        VkDebugUtilsMessageTypeFlagsEXT messageTypes,
                                                                        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData) {
    instance_dispatch_table(instance)->SubmitDebugUtilsMessageEXT(instance, messageSeverity, messageTypes, pCallbackData);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceMultisamplePropertiesEXT(
    VkPhysicalDevice physicalDevice, VkSampleCountFlagBits samples, VkMultisamplePropertiesEXT* pMultisampleProperties) {
    instance_dispatch_table(physicalDevice)
        ->GetPhysicalDeviceMultisamplePropertiesEXT(physicalDevice, samples, pMultisampleProperties);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceExternalBufferProperties(
    VkPhysicalDevice physicalDevice, const VkPhysicalDeviceExternalBufferInfo* pExternalBufferInfo,
    VkExternalBufferProperties* pExternalBufferProperties) {
    instance_dispatch_table(physicalDevice)
        ->GetPhysicalDeviceExternalBufferProperties(physicalDevice, pExternalBufferInfo, pExternalBufferProperties);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceFeatures2(VkPhysicalDevice physicalDevice,
                                                                        VkPhysicalDeviceFeatures2* pFeatures) {
    instance_dispatch_table(physicalDevice)->GetPhysicalDeviceFeatures2(physicalDevice, pFeatures);
    set_layer_supported_features(pFeatures);
    vk_json::s_pipe.setPhysicalDeviceFeatures2(pFeatures);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceProperties2(VkPhysicalDevice physicalDevice,
                                                                          VkPhysicalDeviceProperties2* pProperties) {
    instance_dispatch_table(physicalDevice)->GetPhysicalDeviceProperties2(physicalDevice, pProperties);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceFormatProperties2(VkPhysicalDevice physicalDevice, VkFormat format,
                                                                                VkFormatProperties2* pFormatProperties) {
    instance_dispatch_table(physicalDevice)->GetPhysicalDeviceFormatProperties2(physicalDevice, format, pFormatProperties);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceQueueFamilyProperties2(
    VkPhysicalDevice physicalDevice, uint32_t* pQueueFamilyPropertyCount, VkQueueFamilyProperties2* pQueueFamilyProperties) {
    instance_dispatch_table(physicalDevice)
        ->GetPhysicalDeviceQueueFamilyProperties2(physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceSparseImageFormatProperties2(
    VkPhysicalDevice physicalDevice, const VkPhysicalDeviceSparseImageFormatInfo2* pFormatInfo, uint32_t* pPropertyCount,
    VkSparseImageFormatProperties2* pProperties) {
    instance_dispatch_table(physicalDevice)
        ->GetPhysicalDeviceSparseImageFormatProperties2(physicalDevice, pFormatInfo, pPropertyCount, pProperties);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL
vkGetPhysicalDeviceMemoryProperties2(VkPhysicalDevice physicalDevice, VkPhysicalDeviceMemoryProperties2* pMemoryProperties) {
    instance_dispatch_table(physicalDevice)->GetPhysicalDeviceMemoryProperties2(physicalDevice, pMemoryProperties);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceExternalFenceProperties(
    VkPhysicalDevice physicalDevice, const VkPhysicalDeviceExternalFenceInfo* pExternalFenceInfo,
    VkExternalFenceProperties* pExternalFenceProperties) {
    instance_dispatch_table(physicalDevice)
        ->GetPhysicalDeviceExternalFenceProperties(physicalDevice, pExternalFenceInfo, pExternalFenceProperties);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkDestroySurfaceKHR(VkInstance instance, VkSurfaceKHR surface,
                                                               const VkAllocationCallbacks* pAllocator) {
    instance_dispatch_table(instance)->DestroySurfaceKHR(instance, surface, pAllocator);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceExternalSemaphoreProperties(
    VkPhysicalDevice physicalDevice, const VkPhysicalDeviceExternalSemaphoreInfo* pExternalSemaphoreInfo,
    VkExternalSemaphoreProperties* pExternalSemaphoreProperties) {
    instance_dispatch_table(physicalDevice)
        ->GetPhysicalDeviceExternalSemaphoreProperties(physicalDevice, pExternalSemaphoreInfo, pExternalSemaphoreProperties);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkDestroyDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackEXT callback,
                                                                           const VkAllocationCallbacks* pAllocator) {
    instance_dispatch_table(instance)->DestroyDebugReportCallbackEXT(instance, callback, pAllocator);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkDebugReportMessageEXT(VkInstance instance, VkDebugReportFlagsEXT flags,
                                                                   VkDebugReportObjectTypeEXT objectType, uint64_t object,
                                                                   size_t location, int32_t messageCode, const char* pLayerPrefix,
                                                                   const char* pMessage) {
    instance_dispatch_table(instance)->DebugReportMessageEXT(instance, flags, objectType, object, location, messageCode,
                                                             pLayerPrefix, pMessage);
}

// Autogen device functions

EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkWaitForPresentKHR(VkDevice device, VkSwapchainKHR swapchain, uint64_t presentId,
                                                                   uint64_t timeout) {
    VkResult result = device_dispatch_table(device)->WaitForPresentKHR(device, swapchain, presentId, timeout);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCreateRenderPass2(VkDevice device, const VkRenderPassCreateInfo2* pCreateInfo,
                                                                   const VkAllocationCallbacks* pAllocator,
                                                                   VkRenderPass* pRenderPass) {
    VkResult result = device_dispatch_table(device)->CreateRenderPass2(device, pCreateInfo, pAllocator, pRenderPass);
    vk_json::s_pipe.setRenderPass2(device, pCreateInfo, pRenderPass);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkMapMemory2(VkDevice device, const VkMemoryMapInfo* pMemoryMapInfo, void** ppData) {
    VkResult result = device_dispatch_table(device)->MapMemory2(device, pMemoryMapInfo, ppData);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetSwapchainStatusKHR(VkDevice device, VkSwapchainKHR swapchain) {
    VkResult result = device_dispatch_table(device)->GetSwapchainStatusKHR(device, swapchain);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkSetLatencySleepModeNV(VkDevice device, VkSwapchainKHR swapchain,
                                                                       const VkLatencySleepModeInfoNV* pSleepModeInfo) {
    VkResult result = device_dispatch_table(device)->SetLatencySleepModeNV(device, swapchain, pSleepModeInfo);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkUnmapMemory2(VkDevice device, const VkMemoryUnmapInfo* pMemoryUnmapInfo) {
    VkResult result = device_dispatch_table(device)->UnmapMemory2(device, pMemoryUnmapInfo);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCreateDescriptorUpdateTemplateKHR(
    VkDevice device, const VkDescriptorUpdateTemplateCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator,
    VkDescriptorUpdateTemplate* pDescriptorUpdateTemplate) {
    VkResult result = device_dispatch_table(device)->CreateDescriptorUpdateTemplateKHR(device, pCreateInfo, pAllocator,
                                                                                       pDescriptorUpdateTemplate);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCreatePipelineBinariesKHR(VkDevice device,
                                                                           const VkPipelineBinaryCreateInfoKHR* pCreateInfo,
                                                                           const VkAllocationCallbacks* pAllocator,
                                                                           VkPipelineBinaryHandlesInfoKHR* pBinaries) {
    VkResult result = device_dispatch_table(device)->CreatePipelineBinariesKHR(device, pCreateInfo, pAllocator, pBinaries);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetPipelineBinaryDataKHR(VkDevice device, const VkPipelineBinaryDataInfoKHR* pInfo,
                                                                          VkPipelineBinaryKeyKHR* pPipelineBinaryKey,
                                                                          size_t* pPipelineBinaryDataSize,
                                                                          void* pPipelineBinaryData) {
    VkResult result = device_dispatch_table(device)->GetPipelineBinaryDataKHR(device, pInfo, pPipelineBinaryKey,
                                                                              pPipelineBinaryDataSize, pPipelineBinaryData);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkLatencySleepNV(VkDevice device, VkSwapchainKHR swapchain,
                                                                const VkLatencySleepInfoNV* pSleepInfo) {
    VkResult result = device_dispatch_table(device)->LatencySleepNV(device, swapchain, pSleepInfo);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetPipelineKeyKHR(VkDevice device,
                                                                   const VkPipelineCreateInfoKHR* pPipelineCreateInfo,
                                                                   VkPipelineBinaryKeyKHR* pPipelineKey) {
    VkResult result = device_dispatch_table(device)->GetPipelineKeyKHR(device, pPipelineCreateInfo, pPipelineKey);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCreateBuffer(VkDevice device, const VkBufferCreateInfo* pCreateInfo,
                                                              const VkAllocationCallbacks* pAllocator, VkBuffer* pBuffer) {
    VkResult result = device_dispatch_table(device)->CreateBuffer(device, pCreateInfo, pAllocator, pBuffer);
    if (pCreateInfo) {
        vk_json::s_pipe.objResInfo.bufferRequestCount++;
    }

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkImportFenceFdKHR(VkDevice device,
                                                                  const VkImportFenceFdInfoKHR* pImportFenceFdInfo) {
    VkResult result = device_dispatch_table(device)->ImportFenceFdKHR(device, pImportFenceFdInfo);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkReleaseCapturedPipelineDataKHR(VkDevice device,
                                                                                const VkReleaseCapturedPipelineDataInfoKHR* pInfo,
                                                                                const VkAllocationCallbacks* pAllocator) {
    VkResult result = device_dispatch_table(device)->ReleaseCapturedPipelineDataKHR(device, pInfo, pAllocator);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetFenceFdKHR(VkDevice device, const VkFenceGetFdInfoKHR* pGetFdInfo, int* pFd) {
    VkResult result = device_dispatch_table(device)->GetFenceFdKHR(device, pGetFdInfo, pFd);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCreateBufferView(VkDevice device, const VkBufferViewCreateInfo* pCreateInfo,
                                                                  const VkAllocationCallbacks* pAllocator, VkBufferView* pView) {
    VkResult result = device_dispatch_table(device)->CreateBufferView(device, pCreateInfo, pAllocator, pView);
    if (pCreateInfo) {
        vk_json::s_pipe.objResInfo.bufferViewRequestCount++;
    }

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetFramebufferTilePropertiesQCOM(VkDevice device, VkFramebuffer framebuffer,
                                                                                  uint32_t* pPropertiesCount,
                                                                                  VkTilePropertiesQCOM* pProperties) {
    VkResult result =
        device_dispatch_table(device)->GetFramebufferTilePropertiesQCOM(device, framebuffer, pPropertiesCount, pProperties);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetDynamicRenderingTilePropertiesQCOM(VkDevice device,
                                                                                       const VkRenderingInfo* pRenderingInfo,
                                                                                       VkTilePropertiesQCOM* pProperties) {
    VkResult result = device_dispatch_table(device)->GetDynamicRenderingTilePropertiesQCOM(device, pRenderingInfo, pProperties);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCreateImage(VkDevice device, const VkImageCreateInfo* pCreateInfo,
                                                             const VkAllocationCallbacks* pAllocator, VkImage* pImage) {
    VkResult result = device_dispatch_table(device)->CreateImage(device, pCreateInfo, pAllocator, pImage);
    if (pCreateInfo) {
        vk_json::s_pipe.objResInfo.imageRequestCount++;
    }

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkQueueBindSparse(VkQueue queue, uint32_t bindInfoCount,
                                                                 const VkBindSparseInfo* pBindInfo, VkFence fence) {
    VkResult result = device_dispatch_table(queue)->QueueBindSparse(queue, bindInfoCount, pBindInfo, fence);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetShaderInfoAMD(VkDevice device, VkPipeline pipeline,
                                                                  VkShaderStageFlagBits shaderStage, VkShaderInfoTypeAMD infoType,
                                                                  size_t* pInfoSize, void* pInfo) {
    VkResult result = device_dispatch_table(device)->GetShaderInfoAMD(device, pipeline, shaderStage, infoType, pInfoSize, pInfo);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCreateFence(VkDevice device, const VkFenceCreateInfo* pCreateInfo,
                                                             const VkAllocationCallbacks* pAllocator, VkFence* pFence) {
    VkResult result = device_dispatch_table(device)->CreateFence(device, pCreateInfo, pAllocator, pFence);
    if (pCreateInfo) {
        vk_json::s_pipe.objResInfo.fenceRequestCount++;
    }

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkDisplayPowerControlEXT(VkDevice device, VkDisplayKHR display,
                                                                        const VkDisplayPowerInfoEXT* pDisplayPowerInfo) {
    VkResult result = device_dispatch_table(device)->DisplayPowerControlEXT(device, display, pDisplayPowerInfo);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkResetFences(VkDevice device, uint32_t fenceCount, const VkFence* pFences) {
    VkResult result = device_dispatch_table(device)->ResetFences(device, fenceCount, pFences);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkRegisterDeviceEventEXT(VkDevice device,
                                                                        const VkDeviceEventInfoEXT* pDeviceEventInfo,
                                                                        const VkAllocationCallbacks* pAllocator, VkFence* pFence) {
    VkResult result = device_dispatch_table(device)->RegisterDeviceEventEXT(device, pDeviceEventInfo, pAllocator, pFence);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetFenceStatus(VkDevice device, VkFence fence) {
    VkResult result = device_dispatch_table(device)->GetFenceStatus(device, fence);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetSwapchainCounterEXT(VkDevice device, VkSwapchainKHR swapchain,
                                                                        VkSurfaceCounterFlagBitsEXT counter,
                                                                        uint64_t* pCounterValue) {
    VkResult result = device_dispatch_table(device)->GetSwapchainCounterEXT(device, swapchain, counter, pCounterValue);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkWaitForFences(VkDevice device, uint32_t fenceCount, const VkFence* pFences,
                                                               VkBool32 waitAll, uint64_t timeout) {
    VkResult result = device_dispatch_table(device)->WaitForFences(device, fenceCount, pFences, waitAll, timeout);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkRegisterDisplayEventEXT(VkDevice device, VkDisplayKHR display,
                                                                         const VkDisplayEventInfoEXT* pDisplayEventInfo,
                                                                         const VkAllocationCallbacks* pAllocator, VkFence* pFence) {
    VkResult result =
        device_dispatch_table(device)->RegisterDisplayEventEXT(device, display, pDisplayEventInfo, pAllocator, pFence);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCreateSemaphore(VkDevice device, const VkSemaphoreCreateInfo* pCreateInfo,
                                                                 const VkAllocationCallbacks* pAllocator, VkSemaphore* pSemaphore) {
    VkResult result = device_dispatch_table(device)->CreateSemaphore(device, pCreateInfo, pAllocator, pSemaphore);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCreateImageView(VkDevice device, const VkImageViewCreateInfo* pCreateInfo,
                                                                 const VkAllocationCallbacks* pAllocator, VkImageView* pView) {
    VkResult result = device_dispatch_table(device)->CreateImageView(device, pCreateInfo, pAllocator, pView);
    if (pCreateInfo) {
        vk_json::s_pipe.objResInfo.imageViewRequestCount++;
        if (pCreateInfo->subresourceRange.levelCount > vk_json::s_pipe.objResInfo.maxImageViewMipLevels) {
            vk_json::s_pipe.objResInfo.maxImageViewMipLevels = pCreateInfo->subresourceRange.levelCount;
        }
        if (pCreateInfo->subresourceRange.layerCount > vk_json::s_pipe.objResInfo.layeredImageViewRequestCount) {
            vk_json::s_pipe.objResInfo.layeredImageViewRequestCount = pCreateInfo->subresourceRange.layerCount;
        }
        if (pCreateInfo->subresourceRange.layerCount > vk_json::s_pipe.objResInfo.layeredImageViewRequestCount) {
            vk_json::s_pipe.objResInfo.layeredImageViewRequestCount = pCreateInfo->subresourceRange.layerCount;
        }
    }

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL
vkCreateIndirectCommandsLayoutEXT(VkDevice device, const VkIndirectCommandsLayoutCreateInfoEXT* pCreateInfo,
                                  const VkAllocationCallbacks* pAllocator, VkIndirectCommandsLayoutEXT* pIndirectCommandsLayout) {
    VkResult result =
        device_dispatch_table(device)->CreateIndirectCommandsLayoutEXT(device, pCreateInfo, pAllocator, pIndirectCommandsLayout);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL
vkCreateIndirectExecutionSetEXT(VkDevice device, const VkIndirectExecutionSetCreateInfoEXT* pCreateInfo,
                                const VkAllocationCallbacks* pAllocator, VkIndirectExecutionSetEXT* pIndirectExecutionSet) {
    VkResult result =
        device_dispatch_table(device)->CreateIndirectExecutionSetEXT(device, pCreateInfo, pAllocator, pIndirectExecutionSet);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL
vkGetRefreshCycleDurationGOOGLE(VkDevice device, VkSwapchainKHR swapchain, VkRefreshCycleDurationGOOGLE* pDisplayTimingProperties) {
    VkResult result = device_dispatch_table(device)->GetRefreshCycleDurationGOOGLE(device, swapchain, pDisplayTimingProperties);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkDeviceAddress VKAPI_CALL vkGetBufferDeviceAddressKHR(VkDevice device,
                                                                                  const VkBufferDeviceAddressInfo* pInfo) {
    VkDeviceAddress result = device_dispatch_table(device)->GetBufferDeviceAddressKHR(device, pInfo);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCreateEvent(VkDevice device, const VkEventCreateInfo* pCreateInfo,
                                                             const VkAllocationCallbacks* pAllocator, VkEvent* pEvent) {
    VkResult result = device_dispatch_table(device)->CreateEvent(device, pCreateInfo, pAllocator, pEvent);
    if (pCreateInfo) {
        vk_json::s_pipe.objResInfo.eventRequestCount++;
    }

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL
vkConvertCooperativeVectorMatrixNV(VkDevice device, const VkConvertCooperativeVectorMatrixInfoNV* pInfo) {
    VkResult result = device_dispatch_table(device)->ConvertCooperativeVectorMatrixNV(device, pInfo);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL
vkGetPastPresentationTimingGOOGLE(VkDevice device, VkSwapchainKHR swapchain, uint32_t* pPresentationTimingCount,
                                  VkPastPresentationTimingGOOGLE* pPresentationTimings) {
    VkResult result = device_dispatch_table(device)->GetPastPresentationTimingGOOGLE(device, swapchain, pPresentationTimingCount,
                                                                                     pPresentationTimings);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkSetEvent(VkDevice device, VkEvent event) {
    VkResult result = device_dispatch_table(device)->SetEvent(device, event);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetEventStatus(VkDevice device, VkEvent event) {
    VkResult result = device_dispatch_table(device)->GetEventStatus(device, event);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR uint64_t VKAPI_CALL vkGetBufferOpaqueCaptureAddressKHR(VkDevice device,
                                                                                  const VkBufferDeviceAddressInfo* pInfo) {
    uint64_t result = device_dispatch_table(device)->GetBufferOpaqueCaptureAddressKHR(device, pInfo);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkAcquireProfilingLockKHR(VkDevice device,
                                                                         const VkAcquireProfilingLockInfoKHR* pInfo) {
    VkResult result = device_dispatch_table(device)->AcquireProfilingLockKHR(device, pInfo);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR uint64_t VKAPI_CALL
vkGetDeviceMemoryOpaqueCaptureAddressKHR(VkDevice device, const VkDeviceMemoryOpaqueCaptureAddressInfo* pInfo) {
    uint64_t result = device_dispatch_table(device)->GetDeviceMemoryOpaqueCaptureAddressKHR(device, pInfo);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkResetEvent(VkDevice device, VkEvent event) {
    VkResult result = device_dispatch_table(device)->ResetEvent(device, event);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCreateShaderModule(VkDevice device, const VkShaderModuleCreateInfo* pCreateInfo,
                                                                    const VkAllocationCallbacks* pAllocator,
                                                                    VkShaderModule* pShaderModule) {
    VkResult result = device_dispatch_table(device)->CreateShaderModule(device, pCreateInfo, pAllocator, pShaderModule);
    vk_json::s_pipe.setShaderModuleInfo(pCreateInfo, pShaderModule);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCreatePipelineCache(VkDevice device, const VkPipelineCacheCreateInfo* pCreateInfo,
                                                                     const VkAllocationCallbacks* pAllocator,
                                                                     VkPipelineCache* pPipelineCache) {
    VkResult result = device_dispatch_table(device)->CreatePipelineCache(device, pCreateInfo, pAllocator, pPipelineCache);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkDeviceAddress VKAPI_CALL
vkGetPipelineIndirectDeviceAddressNV(VkDevice device, const VkPipelineIndirectDeviceAddressInfoNV* pInfo) {
    VkDeviceAddress result = device_dispatch_table(device)->GetPipelineIndirectDeviceAddressNV(device, pInfo);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetPipelineCacheData(VkDevice device, VkPipelineCache pipelineCache,
                                                                      size_t* pDataSize, void* pData) {
    VkResult result = device_dispatch_table(device)->GetPipelineCacheData(device, pipelineCache, pDataSize, pData);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkMergePipelineCaches(VkDevice device, VkPipelineCache dstCache,
                                                                     uint32_t srcCacheCount, const VkPipelineCache* pSrcCaches) {
    VkResult result = device_dispatch_table(device)->MergePipelineCaches(device, dstCache, srcCacheCount, pSrcCaches);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkDeferredOperationJoinKHR(VkDevice device, VkDeferredOperationKHR operation) {
    VkResult result = device_dispatch_table(device)->DeferredOperationJoinKHR(device, operation);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCreateDeferredOperationKHR(VkDevice device,
                                                                            const VkAllocationCallbacks* pAllocator,
                                                                            VkDeferredOperationKHR* pDeferredOperation) {
    VkResult result = device_dispatch_table(device)->CreateDeferredOperationKHR(device, pAllocator, pDeferredOperation);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkQueueSubmit(VkQueue queue, uint32_t submitCount, const VkSubmitInfo* pSubmits,
                                                             VkFence fence) {
    VkResult result = device_dispatch_table(queue)->QueueSubmit(queue, submitCount, pSubmits, fence);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkQueueWaitIdle(VkQueue queue) {
    VkResult result = device_dispatch_table(queue)->QueueWaitIdle(queue);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR uint32_t VKAPI_CALL vkGetDeferredOperationMaxConcurrencyKHR(VkDevice device,
                                                                                       VkDeferredOperationKHR operation) {
    uint32_t result = device_dispatch_table(device)->GetDeferredOperationMaxConcurrencyKHR(device, operation);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetDeferredOperationResultKHR(VkDevice device, VkDeferredOperationKHR operation) {
    VkResult result = device_dispatch_table(device)->GetDeferredOperationResultKHR(device, operation);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetMemoryFdKHR(VkDevice device, const VkMemoryGetFdInfoKHR* pGetFdInfo, int* pFd) {
    VkResult result = device_dispatch_table(device)->GetMemoryFdKHR(device, pGetFdInfo, pFd);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCreateQueryPool(VkDevice device, const VkQueryPoolCreateInfo* pCreateInfo,
                                                                 const VkAllocationCallbacks* pAllocator, VkQueryPool* pQueryPool) {
    VkResult result = device_dispatch_table(device)->CreateQueryPool(device, pCreateInfo, pAllocator, pQueryPool);
    if (pCreateInfo) {
        vk_json::s_pipe.objResInfo.queryPoolRequestCount++;
    }

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkDeviceWaitIdle(VkDevice device) {
    VkResult result = device_dispatch_table(device)->DeviceWaitIdle(device);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetMemoryFdPropertiesKHR(VkDevice device,
                                                                          VkExternalMemoryHandleTypeFlagBits handleType, int fd,
                                                                          VkMemoryFdPropertiesKHR* pMemoryFdProperties) {
    VkResult result = device_dispatch_table(device)->GetMemoryFdPropertiesKHR(device, handleType, fd, pMemoryFdProperties);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetQueryPoolResults(VkDevice device, VkQueryPool queryPool, uint32_t firstQuery,
                                                                     uint32_t queryCount, size_t dataSize, void* pData,
                                                                     VkDeviceSize stride, VkQueryResultFlags flags) {
    VkResult result = device_dispatch_table(device)->GetQueryPoolResults(device, queryPool, firstQuery, queryCount, dataSize, pData,
                                                                         stride, flags);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkBindBufferMemory2(VkDevice device, uint32_t bindInfoCount,
                                                                   const VkBindBufferMemoryInfo* pBindInfos) {
    VkResult result = device_dispatch_table(device)->BindBufferMemory2(device, bindInfoCount, pBindInfos);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkImportSemaphoreFdKHR(VkDevice device,
                                                                      const VkImportSemaphoreFdInfoKHR* pImportSemaphoreFdInfo) {
    VkResult result = device_dispatch_table(device)->ImportSemaphoreFdKHR(device, pImportSemaphoreFdInfo);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkBindImageMemory2(VkDevice device, uint32_t bindInfoCount,
                                                                  const VkBindImageMemoryInfo* pBindInfos) {
    VkResult result = device_dispatch_table(device)->BindImageMemory2(device, bindInfoCount, pBindInfos);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkAllocateMemory(VkDevice device, const VkMemoryAllocateInfo* pAllocateInfo,
                                                                const VkAllocationCallbacks* pAllocator, VkDeviceMemory* pMemory) {
    VkResult result = device_dispatch_table(device)->AllocateMemory(device, pAllocateInfo, pAllocator, pMemory);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkMapMemory(VkDevice device, VkDeviceMemory memory, VkDeviceSize offset,
                                                           VkDeviceSize size, VkMemoryMapFlags flags, void** ppData) {
    VkResult result = device_dispatch_table(device)->MapMemory(device, memory, offset, size, flags, ppData);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetSemaphoreFdKHR(VkDevice device, const VkSemaphoreGetFdInfoKHR* pGetFdInfo,
                                                                   int* pFd) {
    VkResult result = device_dispatch_table(device)->GetSemaphoreFdKHR(device, pGetFdInfo, pFd);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkFlushMappedMemoryRanges(VkDevice device, uint32_t memoryRangeCount,
                                                                         const VkMappedMemoryRange* pMemoryRanges) {
    VkResult result = device_dispatch_table(device)->FlushMappedMemoryRanges(device, memoryRangeCount, pMemoryRanges);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkInvalidateMappedMemoryRanges(VkDevice device, uint32_t memoryRangeCount,
                                                                              const VkMappedMemoryRange* pMemoryRanges) {
    VkResult result = device_dispatch_table(device)->InvalidateMappedMemoryRanges(device, memoryRangeCount, pMemoryRanges);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCreateRenderPass2KHR(VkDevice device, const VkRenderPassCreateInfo2* pCreateInfo,
                                                                      const VkAllocationCallbacks* pAllocator,
                                                                      VkRenderPass* pRenderPass) {
    VkResult result = device_dispatch_table(device)->CreateRenderPass2KHR(device, pCreateInfo, pAllocator, pRenderPass);

    return result;
}
#if defined(VK_USE_PLATFORM_METAL_EXT)
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetMemoryMetalHandleEXT(VkDevice device,
                                                                         const VkMemoryGetMetalHandleInfoEXT* pGetMetalHandleInfo,
                                                                         void** pHandle) {
    VkResult result = device_dispatch_table(device)->GetMemoryMetalHandleEXT(device, pGetMetalHandleInfo, pHandle);

    return result;
}
#endif  // VK_USE_PLATFORM_METAL_EXT
#if defined(VK_USE_PLATFORM_METAL_EXT)
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL
vkGetMemoryMetalHandlePropertiesEXT(VkDevice device, VkExternalMemoryHandleTypeFlagBits handleType, const void* pHandle,
                                    VkMemoryMetalHandlePropertiesEXT* pMemoryMetalHandleProperties) {
    VkResult result =
        device_dispatch_table(device)->GetMemoryMetalHandlePropertiesEXT(device, handleType, pHandle, pMemoryMetalHandleProperties);

    return result;
}
#endif  // VK_USE_PLATFORM_METAL_EXT
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkSetDebugUtilsObjectNameEXT(VkDevice device,
                                                                            const VkDebugUtilsObjectNameInfoEXT* pNameInfo) {
    VkResult result = device_dispatch_table(device)->SetDebugUtilsObjectNameEXT(device, pNameInfo);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkBindImageMemory(VkDevice device, VkImage image, VkDeviceMemory memory,
                                                                 VkDeviceSize memoryOffset) {
    VkResult result = device_dispatch_table(device)->BindImageMemory(device, image, memory, memoryOffset);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkBindBufferMemory(VkDevice device, VkBuffer buffer, VkDeviceMemory memory,
                                                                  VkDeviceSize memoryOffset) {
    VkResult result = device_dispatch_table(device)->BindBufferMemory(device, buffer, memory, memoryOffset);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetSemaphoreCounterValue(VkDevice device, VkSemaphore semaphore,
                                                                          uint64_t* pValue) {
    VkResult result = device_dispatch_table(device)->GetSemaphoreCounterValue(device, semaphore, pValue);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL
vkGetPipelineExecutablePropertiesKHR(VkDevice device, const VkPipelineInfoKHR* pPipelineInfo, uint32_t* pExecutableCount,
                                     VkPipelineExecutablePropertiesKHR* pProperties) {
    VkResult result =
        device_dispatch_table(device)->GetPipelineExecutablePropertiesKHR(device, pPipelineInfo, pExecutableCount, pProperties);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetSwapchainImagesKHR(VkDevice device, VkSwapchainKHR swapchain,
                                                                       uint32_t* pSwapchainImageCount, VkImage* pSwapchainImages) {
    VkResult result =
        device_dispatch_table(device)->GetSwapchainImagesKHR(device, swapchain, pSwapchainImageCount, pSwapchainImages);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkWaitSemaphores(VkDevice device, const VkSemaphoreWaitInfo* pWaitInfo,
                                                                uint64_t timeout) {
    VkResult result = device_dispatch_table(device)->WaitSemaphores(device, pWaitInfo, timeout);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkSetDebugUtilsObjectTagEXT(VkDevice device,
                                                                           const VkDebugUtilsObjectTagInfoEXT* pTagInfo) {
    VkResult result = device_dispatch_table(device)->SetDebugUtilsObjectTagEXT(device, pTagInfo);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCreatePipelineLayout(VkDevice device,
                                                                      const VkPipelineLayoutCreateInfo* pCreateInfo,
                                                                      const VkAllocationCallbacks* pAllocator,
                                                                      VkPipelineLayout* pPipelineLayout) {
    VkResult result = device_dispatch_table(device)->CreatePipelineLayout(device, pCreateInfo, pAllocator, pPipelineLayout);
    vk_json::s_pipe.objResInfo.pipelineLayoutRequestCount++;
    vk_json::s_pipe.setPipelineLayout(device, pCreateInfo, pPipelineLayout);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkSignalSemaphore(VkDevice device, const VkSemaphoreSignalInfo* pSignalInfo) {
    VkResult result = device_dispatch_table(device)->SignalSemaphore(device, pSignalInfo);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL
vkGetPipelineExecutableStatisticsKHR(VkDevice device, const VkPipelineExecutableInfoKHR* pExecutableInfo, uint32_t* pStatisticCount,
                                     VkPipelineExecutableStatisticKHR* pStatistics) {
    VkResult result =
        device_dispatch_table(device)->GetPipelineExecutableStatisticsKHR(device, pExecutableInfo, pStatisticCount, pStatistics);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCopyMemoryToImageEXT(VkDevice device,
                                                                      const VkCopyMemoryToImageInfo* pCopyMemoryToImageInfo) {
    VkResult result = device_dispatch_table(device)->CopyMemoryToImageEXT(device, pCopyMemoryToImageInfo);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkAcquireNextImageKHR(VkDevice device, VkSwapchainKHR swapchain, uint64_t timeout,
                                                                     VkSemaphore semaphore, VkFence fence, uint32_t* pImageIndex) {
    VkResult result = device_dispatch_table(device)->AcquireNextImageKHR(device, swapchain, timeout, semaphore, fence, pImageIndex);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL
vkGetMemoryHostPointerPropertiesEXT(VkDevice device, VkExternalMemoryHandleTypeFlagBits handleType, const void* pHostPointer,
                                    VkMemoryHostPointerPropertiesEXT* pMemoryHostPointerProperties) {
    VkResult result = device_dispatch_table(device)->GetMemoryHostPointerPropertiesEXT(device, handleType, pHostPointer,
                                                                                       pMemoryHostPointerProperties);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetPipelineExecutableInternalRepresentationsKHR(
    VkDevice device, const VkPipelineExecutableInfoKHR* pExecutableInfo, uint32_t* pInternalRepresentationCount,
    VkPipelineExecutableInternalRepresentationKHR* pInternalRepresentations) {
    VkResult result = device_dispatch_table(device)->GetPipelineExecutableInternalRepresentationsKHR(
        device, pExecutableInfo, pInternalRepresentationCount, pInternalRepresentations);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCopyImageToImageEXT(VkDevice device,
                                                                     const VkCopyImageToImageInfo* pCopyImageToImageInfo) {
    VkResult result = device_dispatch_table(device)->CopyImageToImageEXT(device, pCopyImageToImageInfo);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCopyImageToMemoryEXT(VkDevice device,
                                                                      const VkCopyImageToMemoryInfo* pCopyImageToMemoryInfo) {
    VkResult result = device_dispatch_table(device)->CopyImageToMemoryEXT(device, pCopyImageToMemoryInfo);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkTransitionImageLayoutEXT(VkDevice device, uint32_t transitionCount,
                                                                          const VkHostImageLayoutTransitionInfo* pTransitions) {
    VkResult result = device_dispatch_table(device)->TransitionImageLayoutEXT(device, transitionCount, pTransitions);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL
vkGetBufferOpaqueCaptureDescriptorDataEXT(VkDevice device, const VkBufferCaptureDescriptorDataInfoEXT* pInfo, void* pData) {
    VkResult result = device_dispatch_table(device)->GetBufferOpaqueCaptureDescriptorDataEXT(device, pInfo, pData);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkDeviceAddress VKAPI_CALL vkGetBufferDeviceAddress(VkDevice device,
                                                                               const VkBufferDeviceAddressInfo* pInfo) {
    VkDeviceAddress result = device_dispatch_table(device)->GetBufferDeviceAddress(device, pInfo);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL
vkGetImageOpaqueCaptureDescriptorDataEXT(VkDevice device, const VkImageCaptureDescriptorDataInfoEXT* pInfo, void* pData) {
    VkResult result = device_dispatch_table(device)->GetImageOpaqueCaptureDescriptorDataEXT(device, pInfo, pData);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR uint64_t VKAPI_CALL vkGetBufferOpaqueCaptureAddress(VkDevice device,
                                                                               const VkBufferDeviceAddressInfo* pInfo) {
    uint64_t result = device_dispatch_table(device)->GetBufferOpaqueCaptureAddress(device, pInfo);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkDebugMarkerSetObjectTagEXT(VkDevice device,
                                                                            const VkDebugMarkerObjectTagInfoEXT* pTagInfo) {
    VkResult result = device_dispatch_table(device)->DebugMarkerSetObjectTagEXT(device, pTagInfo);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL
vkGetImageViewOpaqueCaptureDescriptorDataEXT(VkDevice device, const VkImageViewCaptureDescriptorDataInfoEXT* pInfo, void* pData) {
    VkResult result = device_dispatch_table(device)->GetImageViewOpaqueCaptureDescriptorDataEXT(device, pInfo, pData);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkMapMemory2KHR(VkDevice device, const VkMemoryMapInfo* pMemoryMapInfo,
                                                               void** ppData) {
    VkResult result = device_dispatch_table(device)->MapMemory2KHR(device, pMemoryMapInfo, ppData);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR uint64_t VKAPI_CALL
vkGetDeviceMemoryOpaqueCaptureAddress(VkDevice device, const VkDeviceMemoryOpaqueCaptureAddressInfo* pInfo) {
    uint64_t result = device_dispatch_table(device)->GetDeviceMemoryOpaqueCaptureAddress(device, pInfo);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCreateSamplerYcbcrConversion(VkDevice device,
                                                                              const VkSamplerYcbcrConversionCreateInfo* pCreateInfo,
                                                                              const VkAllocationCallbacks* pAllocator,
                                                                              VkSamplerYcbcrConversion* pYcbcrConversion) {
    VkResult result =
        device_dispatch_table(device)->CreateSamplerYcbcrConversion(device, pCreateInfo, pAllocator, pYcbcrConversion);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetEncodedVideoSessionParametersKHR(
    VkDevice device, const VkVideoEncodeSessionParametersGetInfoKHR* pVideoSessionParametersInfo,
    VkVideoEncodeSessionParametersFeedbackInfoKHR* pFeedbackInfo, size_t* pDataSize, void* pData) {
    VkResult result = device_dispatch_table(device)->GetEncodedVideoSessionParametersKHR(device, pVideoSessionParametersInfo,
                                                                                         pFeedbackInfo, pDataSize, pData);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL
vkGetSamplerOpaqueCaptureDescriptorDataEXT(VkDevice device, const VkSamplerCaptureDescriptorDataInfoEXT* pInfo, void* pData) {
    VkResult result = device_dispatch_table(device)->GetSamplerOpaqueCaptureDescriptorDataEXT(device, pInfo, pData);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkUnmapMemory2KHR(VkDevice device, const VkMemoryUnmapInfo* pMemoryUnmapInfo) {
    VkResult result = device_dispatch_table(device)->UnmapMemory2KHR(device, pMemoryUnmapInfo);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkDebugMarkerSetObjectNameEXT(VkDevice device,
                                                                             const VkDebugMarkerObjectNameInfoEXT* pNameInfo) {
    VkResult result = device_dispatch_table(device)->DebugMarkerSetObjectNameEXT(device, pNameInfo);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL
vkInitializePerformanceApiINTEL(VkDevice device, const VkInitializePerformanceApiInfoINTEL* pInitializeInfo) {
    VkResult result = device_dispatch_table(device)->InitializePerformanceApiINTEL(device, pInitializeInfo);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetAccelerationStructureOpaqueCaptureDescriptorDataEXT(
    VkDevice device, const VkAccelerationStructureCaptureDescriptorDataInfoEXT* pInfo, void* pData) {
    VkResult result = device_dispatch_table(device)->GetAccelerationStructureOpaqueCaptureDescriptorDataEXT(device, pInfo, pData);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCmdSetPerformanceMarkerINTEL(VkCommandBuffer commandBuffer,
                                                                              const VkPerformanceMarkerInfoINTEL* pMarkerInfo) {
    VkResult result = device_dispatch_table(commandBuffer)->CmdSetPerformanceMarkerINTEL(commandBuffer, pMarkerInfo);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL
vkCmdSetPerformanceStreamMarkerINTEL(VkCommandBuffer commandBuffer, const VkPerformanceStreamMarkerInfoINTEL* pMarkerInfo) {
    VkResult result = device_dispatch_table(commandBuffer)->CmdSetPerformanceStreamMarkerINTEL(commandBuffer, pMarkerInfo);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL
vkCmdSetPerformanceOverrideINTEL(VkCommandBuffer commandBuffer, const VkPerformanceOverrideInfoINTEL* pOverrideInfo) {
    VkResult result = device_dispatch_table(commandBuffer)->CmdSetPerformanceOverrideINTEL(commandBuffer, pOverrideInfo);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCreateFramebuffer(VkDevice device, const VkFramebufferCreateInfo* pCreateInfo,
                                                                   const VkAllocationCallbacks* pAllocator,
                                                                   VkFramebuffer* pFramebuffer) {
    VkResult result = device_dispatch_table(device)->CreateFramebuffer(device, pCreateInfo, pAllocator, pFramebuffer);
    if (pCreateInfo) {
        vk_json::s_pipe.objResInfo.framebufferRequestCount++;
    }

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL
vkAcquirePerformanceConfigurationINTEL(VkDevice device, const VkPerformanceConfigurationAcquireInfoINTEL* pAcquireInfo,
                                       VkPerformanceConfigurationINTEL* pConfiguration) {
    VkResult result = device_dispatch_table(device)->AcquirePerformanceConfigurationINTEL(device, pAcquireInfo, pConfiguration);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL
vkReleasePerformanceConfigurationINTEL(VkDevice device, VkPerformanceConfigurationINTEL configuration) {
    VkResult result = device_dispatch_table(device)->ReleasePerformanceConfigurationINTEL(device, configuration);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL
vkCreateDescriptorUpdateTemplate(VkDevice device, const VkDescriptorUpdateTemplateCreateInfo* pCreateInfo,
                                 const VkAllocationCallbacks* pAllocator, VkDescriptorUpdateTemplate* pDescriptorUpdateTemplate) {
    VkResult result =
        device_dispatch_table(device)->CreateDescriptorUpdateTemplate(device, pCreateInfo, pAllocator, pDescriptorUpdateTemplate);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetPerformanceParameterINTEL(VkDevice device,
                                                                              VkPerformanceParameterTypeINTEL parameter,
                                                                              VkPerformanceValueINTEL* pValue) {
    VkResult result = device_dispatch_table(device)->GetPerformanceParameterINTEL(device, parameter, pValue);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetDeviceGroupSurfacePresentModesKHR(VkDevice device, VkSurfaceKHR surface,
                                                                                      VkDeviceGroupPresentModeFlagsKHR* pModes) {
    VkResult result = device_dispatch_table(device)->GetDeviceGroupSurfacePresentModesKHR(device, surface, pModes);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetCalibratedTimestampsKHR(VkDevice device, uint32_t timestampCount,
                                                                            const VkCalibratedTimestampInfoKHR* pTimestampInfos,
                                                                            uint64_t* pTimestamps, uint64_t* pMaxDeviation) {
    VkResult result = device_dispatch_table(device)->GetCalibratedTimestampsKHR(device, timestampCount, pTimestampInfos,
                                                                                pTimestamps, pMaxDeviation);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL
vkQueueSetPerformanceConfigurationINTEL(VkQueue queue, VkPerformanceConfigurationINTEL configuration) {
    VkResult result = device_dispatch_table(queue)->QueueSetPerformanceConfigurationINTEL(queue, configuration);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCreateSampler(VkDevice device, const VkSamplerCreateInfo* pCreateInfo,
                                                               const VkAllocationCallbacks* pAllocator, VkSampler* pSampler) {
    VkResult result = device_dispatch_table(device)->CreateSampler(device, pCreateInfo, pAllocator, pSampler);
    vk_json::s_pipe.objResInfo.samplerRequestCount++;
    vk_json::s_pipe.setSamplerInfo(pCreateInfo, pSampler);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL
vkGetDeviceGroupPresentCapabilitiesKHR(VkDevice device, VkDeviceGroupPresentCapabilitiesKHR* pDeviceGroupPresentCapabilities) {
    VkResult result = device_dispatch_table(device)->GetDeviceGroupPresentCapabilitiesKHR(device, pDeviceGroupPresentCapabilities);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetCalibratedTimestampsEXT(VkDevice device, uint32_t timestampCount,
                                                                            const VkCalibratedTimestampInfoKHR* pTimestampInfos,
                                                                            uint64_t* pTimestamps, uint64_t* pMaxDeviation) {
    VkResult result = device_dispatch_table(device)->GetCalibratedTimestampsEXT(device, timestampCount, pTimestampInfos,
                                                                                pTimestamps, pMaxDeviation);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCreateOpticalFlowSessionNV(VkDevice device,
                                                                            const VkOpticalFlowSessionCreateInfoNV* pCreateInfo,
                                                                            const VkAllocationCallbacks* pAllocator,
                                                                            VkOpticalFlowSessionNV* pSession) {
    VkResult result = device_dispatch_table(device)->CreateOpticalFlowSessionNV(device, pCreateInfo, pAllocator, pSession);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkBindOpticalFlowSessionImageNV(VkDevice device, VkOpticalFlowSessionNV session,
                                                                               VkOpticalFlowSessionBindingPointNV bindingPoint,
                                                                               VkImageView view, VkImageLayout layout) {
    VkResult result = device_dispatch_table(device)->BindOpticalFlowSessionImageNV(device, session, bindingPoint, view, layout);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL
vkCreateAccelerationStructureKHR(VkDevice device, const VkAccelerationStructureCreateInfoKHR* pCreateInfo,
                                 const VkAllocationCallbacks* pAllocator, VkAccelerationStructureKHR* pAccelerationStructure) {
    VkResult result =
        device_dispatch_table(device)->CreateAccelerationStructureKHR(device, pCreateInfo, pAllocator, pAccelerationStructure);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkAcquireNextImage2KHR(VkDevice device,
                                                                      const VkAcquireNextImageInfoKHR* pAcquireInfo,
                                                                      uint32_t* pImageIndex) {
    VkResult result = device_dispatch_table(device)->AcquireNextImage2KHR(device, pAcquireInfo, pImageIndex);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCreateRenderPass(VkDevice device, const VkRenderPassCreateInfo* pCreateInfo,
                                                                  const VkAllocationCallbacks* pAllocator,
                                                                  VkRenderPass* pRenderPass) {
    VkResult result = device_dispatch_table(device)->CreateRenderPass(device, pCreateInfo, pAllocator, pRenderPass);
    vk_json::s_pipe.objResInfo.renderPassRequestCount++;
    vk_json::s_pipe.setRenderPass(device, pCreateInfo, pRenderPass);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL
vkBuildAccelerationStructuresKHR(VkDevice device, VkDeferredOperationKHR deferredOperation, uint32_t infoCount,
                                 const VkAccelerationStructureBuildGeometryInfoKHR* pInfos,
                                 const VkAccelerationStructureBuildRangeInfoKHR* const* ppBuildRangeInfos) {
    VkResult result = device_dispatch_table(device)->BuildAccelerationStructuresKHR(device, deferredOperation, infoCount, pInfos,
                                                                                    ppBuildRangeInfos);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCopyMemoryToAccelerationStructureKHR(
    VkDevice device, VkDeferredOperationKHR deferredOperation, const VkCopyMemoryToAccelerationStructureInfoKHR* pInfo) {
    VkResult result = device_dispatch_table(device)->CopyMemoryToAccelerationStructureKHR(device, deferredOperation, pInfo);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCopyAccelerationStructureKHR(VkDevice device,
                                                                              VkDeferredOperationKHR deferredOperation,
                                                                              const VkCopyAccelerationStructureInfoKHR* pInfo) {
    VkResult result = device_dispatch_table(device)->CopyAccelerationStructureKHR(device, deferredOperation, pInfo);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCopyAccelerationStructureToMemoryKHR(
    VkDevice device, VkDeferredOperationKHR deferredOperation, const VkCopyAccelerationStructureToMemoryInfoKHR* pInfo) {
    VkResult result = device_dispatch_table(device)->CopyAccelerationStructureToMemoryKHR(device, deferredOperation, pInfo);

    return result;
}
#if defined(VK_ENABLE_BETA_EXTENSIONS)
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCreateCudaModuleNV(VkDevice device, const VkCudaModuleCreateInfoNV* pCreateInfo,
                                                                    const VkAllocationCallbacks* pAllocator,
                                                                    VkCudaModuleNV* pModule) {
    VkResult result = device_dispatch_table(device)->CreateCudaModuleNV(device, pCreateInfo, pAllocator, pModule);

    return result;
}
#endif  // VK_ENABLE_BETA_EXTENSIONS
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkWriteAccelerationStructuresPropertiesKHR(
    VkDevice device, uint32_t accelerationStructureCount, const VkAccelerationStructureKHR* pAccelerationStructures,
    VkQueryType queryType, size_t dataSize, void* pData, size_t stride) {
    VkResult result = device_dispatch_table(device)->WriteAccelerationStructuresPropertiesKHR(
        device, accelerationStructureCount, pAccelerationStructures, queryType, dataSize, pData, stride);

    return result;
}
#if defined(VK_ENABLE_BETA_EXTENSIONS)
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCreateCudaFunctionNV(VkDevice device,
                                                                      const VkCudaFunctionCreateInfoNV* pCreateInfo,
                                                                      const VkAllocationCallbacks* pAllocator,
                                                                      VkCudaFunctionNV* pFunction) {
    VkResult result = device_dispatch_table(device)->CreateCudaFunctionNV(device, pCreateInfo, pAllocator, pFunction);

    return result;
}
#endif  // VK_ENABLE_BETA_EXTENSIONS
#if defined(VK_ENABLE_BETA_EXTENSIONS)
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetCudaModuleCacheNV(VkDevice device, VkCudaModuleNV module, size_t* pCacheSize,
                                                                      void* pCacheData) {
    VkResult result = device_dispatch_table(device)->GetCudaModuleCacheNV(device, module, pCacheSize, pCacheData);

    return result;
}
#endif  // VK_ENABLE_BETA_EXTENSIONS
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI(VkDevice device,
                                                                                               VkRenderPass renderpass,
                                                                                               VkExtent2D* pMaxWorkgroupSize) {
    VkResult result =
        device_dispatch_table(device)->GetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI(device, renderpass, pMaxWorkgroupSize);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCreateCommandPool(VkDevice device, const VkCommandPoolCreateInfo* pCreateInfo,
                                                                   const VkAllocationCallbacks* pAllocator,
                                                                   VkCommandPool* pCommandPool) {
    VkResult result = device_dispatch_table(device)->CreateCommandPool(device, pCreateInfo, pAllocator, pCommandPool);
    if (pCreateInfo) {
        vk_json::s_pipe.objResInfo.commandPoolRequestCount++;
    }

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkResetCommandPool(VkDevice device, VkCommandPool commandPool,
                                                                  VkCommandPoolResetFlags flags) {
    VkResult result = device_dispatch_table(device)->ResetCommandPool(device, commandPool, flags);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCopyMemoryToImage(VkDevice device,
                                                                   const VkCopyMemoryToImageInfo* pCopyMemoryToImageInfo) {
    VkResult result = device_dispatch_table(device)->CopyMemoryToImage(device, pCopyMemoryToImageInfo);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkDeviceAddress VKAPI_CALL
vkGetAccelerationStructureDeviceAddressKHR(VkDevice device, const VkAccelerationStructureDeviceAddressInfoKHR* pInfo) {
    VkDeviceAddress result = device_dispatch_table(device)->GetAccelerationStructureDeviceAddressKHR(device, pInfo);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCreateCuModuleNVX(VkDevice device, const VkCuModuleCreateInfoNVX* pCreateInfo,
                                                                   const VkAllocationCallbacks* pAllocator,
                                                                   VkCuModuleNVX* pModule) {
    VkResult result = device_dispatch_table(device)->CreateCuModuleNVX(device, pCreateInfo, pAllocator, pModule);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCopyImageToImage(VkDevice device,
                                                                  const VkCopyImageToImageInfo* pCopyImageToImageInfo) {
    VkResult result = device_dispatch_table(device)->CopyImageToImage(device, pCopyImageToImageInfo);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCopyImageToMemory(VkDevice device,
                                                                   const VkCopyImageToMemoryInfo* pCopyImageToMemoryInfo) {
    VkResult result = device_dispatch_table(device)->CopyImageToMemory(device, pCopyImageToMemoryInfo);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkReleaseSwapchainImagesEXT(VkDevice device,
                                                                           const VkReleaseSwapchainImagesInfoEXT* pReleaseInfo) {
    VkResult result = device_dispatch_table(device)->ReleaseSwapchainImagesEXT(device, pReleaseInfo);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkTransitionImageLayout(VkDevice device, uint32_t transitionCount,
                                                                       const VkHostImageLayoutTransitionInfo* pTransitions) {
    VkResult result = device_dispatch_table(device)->TransitionImageLayout(device, transitionCount, pTransitions);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetMemoryRemoteAddressNV(
    VkDevice device, const VkMemoryGetRemoteAddressInfoNV* pMemoryGetRemoteAddressInfo, VkRemoteAddressNV* pAddress) {
    VkResult result = device_dispatch_table(device)->GetMemoryRemoteAddressNV(device, pMemoryGetRemoteAddressInfo, pAddress);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCreateCuFunctionNVX(VkDevice device, const VkCuFunctionCreateInfoNVX* pCreateInfo,
                                                                     const VkAllocationCallbacks* pAllocator,
                                                                     VkCuFunctionNVX* pFunction) {
    VkResult result = device_dispatch_table(device)->CreateCuFunctionNVX(device, pCreateInfo, pAllocator, pFunction);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCreateMicromapEXT(VkDevice device, const VkMicromapCreateInfoEXT* pCreateInfo,
                                                                   const VkAllocationCallbacks* pAllocator,
                                                                   VkMicromapEXT* pMicromap) {
    VkResult result = device_dispatch_table(device)->CreateMicromapEXT(device, pCreateInfo, pAllocator, pMicromap);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL
vkCreateAccelerationStructureNV(VkDevice device, const VkAccelerationStructureCreateInfoNV* pCreateInfo,
                                const VkAllocationCallbacks* pAllocator, VkAccelerationStructureNV* pAccelerationStructure) {
    VkResult result =
        device_dispatch_table(device)->CreateAccelerationStructureNV(device, pCreateInfo, pAllocator, pAccelerationStructure);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCreatePrivateDataSlotEXT(VkDevice device,
                                                                          const VkPrivateDataSlotCreateInfo* pCreateInfo,
                                                                          const VkAllocationCallbacks* pAllocator,
                                                                          VkPrivateDataSlot* pPrivateDataSlot) {
    VkResult result = device_dispatch_table(device)->CreatePrivateDataSlotEXT(device, pCreateInfo, pAllocator, pPrivateDataSlot);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkQueueSubmit2KHR(VkQueue queue, uint32_t submitCount, const VkSubmitInfo2* pSubmits,
                                                                 VkFence fence) {
    VkResult result = device_dispatch_table(queue)->QueueSubmit2KHR(queue, submitCount, pSubmits, fence);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCreateSharedSwapchainsKHR(VkDevice device, uint32_t swapchainCount,
                                                                           const VkSwapchainCreateInfoKHR* pCreateInfos,
                                                                           const VkAllocationCallbacks* pAllocator,
                                                                           VkSwapchainKHR* pSwapchains) {
    VkResult result =
        device_dispatch_table(device)->CreateSharedSwapchainsKHR(device, swapchainCount, pCreateInfos, pAllocator, pSwapchains);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkBuildMicromapsEXT(VkDevice device, VkDeferredOperationKHR deferredOperation,
                                                                   uint32_t infoCount, const VkMicromapBuildInfoEXT* pInfos) {
    VkResult result = device_dispatch_table(device)->BuildMicromapsEXT(device, deferredOperation, infoCount, pInfos);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetDeviceFaultInfoEXT(VkDevice device, VkDeviceFaultCountsEXT* pFaultCounts,
                                                                       VkDeviceFaultInfoEXT* pFaultInfo) {
    VkResult result = device_dispatch_table(device)->GetDeviceFaultInfoEXT(device, pFaultCounts, pFaultInfo);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkSetPrivateDataEXT(VkDevice device, VkObjectType objectType, uint64_t objectHandle,
                                                                   VkPrivateDataSlot privateDataSlot, uint64_t data) {
    VkResult result = device_dispatch_table(device)->SetPrivateDataEXT(device, objectType, objectHandle, privateDataSlot, data);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkAllocateCommandBuffers(VkDevice device,
                                                                        const VkCommandBufferAllocateInfo* pAllocateInfo,
                                                                        VkCommandBuffer* pCommandBuffers) {
    VkResult result = device_dispatch_table(device)->AllocateCommandBuffers(device, pAllocateInfo, pCommandBuffers);
    if (pAllocateInfo) {
        vk_json::s_pipe.objResInfo.commandBufferRequestCount += pAllocateInfo->commandBufferCount;
    }

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetImageViewAddressNVX(VkDevice device, VkImageView imageView,
                                                                        VkImageViewAddressPropertiesNVX* pProperties) {
    VkResult result = device_dispatch_table(device)->GetImageViewAddressNVX(device, imageView, pProperties);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR uint32_t VKAPI_CALL vkGetImageViewHandleNVX(VkDevice device, const VkImageViewHandleInfoNVX* pInfo) {
    uint32_t result = device_dispatch_table(device)->GetImageViewHandleNVX(device, pInfo);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCopyMicromapToMemoryEXT(VkDevice device, VkDeferredOperationKHR deferredOperation,
                                                                         const VkCopyMicromapToMemoryInfoEXT* pInfo) {
    VkResult result = device_dispatch_table(device)->CopyMicromapToMemoryEXT(device, deferredOperation, pInfo);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL
vkCreateSamplerYcbcrConversionKHR(VkDevice device, const VkSamplerYcbcrConversionCreateInfo* pCreateInfo,
                                  const VkAllocationCallbacks* pAllocator, VkSamplerYcbcrConversion* pYcbcrConversion) {
    VkResult result =
        device_dispatch_table(device)->CreateSamplerYcbcrConversionKHR(device, pCreateInfo, pAllocator, pYcbcrConversion);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkBindAccelerationStructureMemoryNV(
    VkDevice device, uint32_t bindInfoCount, const VkBindAccelerationStructureMemoryInfoNV* pBindInfos) {
    VkResult result = device_dispatch_table(device)->BindAccelerationStructureMemoryNV(device, bindInfoCount, pBindInfos);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCopyMicromapEXT(VkDevice device, VkDeferredOperationKHR deferredOperation,
                                                                 const VkCopyMicromapInfoEXT* pInfo) {
    VkResult result = device_dispatch_table(device)->CopyMicromapEXT(device, deferredOperation, pInfo);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkBeginCommandBuffer(VkCommandBuffer commandBuffer,
                                                                    const VkCommandBufferBeginInfo* pBeginInfo) {
    VkResult result = device_dispatch_table(commandBuffer)->BeginCommandBuffer(commandBuffer, pBeginInfo);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkBindBufferMemory2KHR(VkDevice device, uint32_t bindInfoCount,
                                                                      const VkBindBufferMemoryInfo* pBindInfos) {
    VkResult result = device_dispatch_table(device)->BindBufferMemory2KHR(device, bindInfoCount, pBindInfos);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCreateShadersEXT(VkDevice device, uint32_t createInfoCount,
                                                                  const VkShaderCreateInfoEXT* pCreateInfos,
                                                                  const VkAllocationCallbacks* pAllocator, VkShaderEXT* pShaders) {
    VkResult result = device_dispatch_table(device)->CreateShadersEXT(device, createInfoCount, pCreateInfos, pAllocator, pShaders);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR uint64_t VKAPI_CALL vkGetImageViewHandle64NVX(VkDevice device, const VkImageViewHandleInfoNVX* pInfo) {
    uint64_t result = device_dispatch_table(device)->GetImageViewHandle64NVX(device, pInfo);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCreatePrivateDataSlot(VkDevice device,
                                                                       const VkPrivateDataSlotCreateInfo* pCreateInfo,
                                                                       const VkAllocationCallbacks* pAllocator,
                                                                       VkPrivateDataSlot* pPrivateDataSlot) {
    VkResult result = device_dispatch_table(device)->CreatePrivateDataSlot(device, pCreateInfo, pAllocator, pPrivateDataSlot);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkWriteMicromapsPropertiesEXT(VkDevice device, uint32_t micromapCount,
                                                                             const VkMicromapEXT* pMicromaps, VkQueryType queryType,
                                                                             size_t dataSize, void* pData, size_t stride) {
    VkResult result = device_dispatch_table(device)->WriteMicromapsPropertiesEXT(device, micromapCount, pMicromaps, queryType,
                                                                                 dataSize, pData, stride);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCopyMemoryToMicromapEXT(VkDevice device, VkDeferredOperationKHR deferredOperation,
                                                                         const VkCopyMemoryToMicromapInfoEXT* pInfo) {
    VkResult result = device_dispatch_table(device)->CopyMemoryToMicromapEXT(device, deferredOperation, pInfo);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkEndCommandBuffer(VkCommandBuffer commandBuffer) {
    VkResult result = device_dispatch_table(commandBuffer)->EndCommandBuffer(commandBuffer);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkResetCommandBuffer(VkCommandBuffer commandBuffer,
                                                                    VkCommandBufferResetFlags flags) {
    VkResult result = device_dispatch_table(commandBuffer)->ResetCommandBuffer(commandBuffer, flags);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkSetPrivateData(VkDevice device, VkObjectType objectType, uint64_t objectHandle,
                                                                VkPrivateDataSlot privateDataSlot, uint64_t data) {
    VkResult result = device_dispatch_table(device)->SetPrivateData(device, objectType, objectHandle, privateDataSlot, data);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkBindImageMemory2KHR(VkDevice device, uint32_t bindInfoCount,
                                                                     const VkBindImageMemoryInfo* pBindInfos) {
    VkResult result = device_dispatch_table(device)->BindImageMemory2KHR(device, bindInfoCount, pBindInfos);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCreateRayTracingPipelinesNV(VkDevice device, VkPipelineCache pipelineCache,
                                                                             uint32_t createInfoCount,
                                                                             const VkRayTracingPipelineCreateInfoNV* pCreateInfos,
                                                                             const VkAllocationCallbacks* pAllocator,
                                                                             VkPipeline* pPipelines) {
    VkResult result = device_dispatch_table(device)->CreateRayTracingPipelinesNV(device, pipelineCache, createInfoCount,
                                                                                 pCreateInfos, pAllocator, pPipelines);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCreateDescriptorSetLayout(VkDevice device,
                                                                           const VkDescriptorSetLayoutCreateInfo* pCreateInfo,
                                                                           const VkAllocationCallbacks* pAllocator,
                                                                           VkDescriptorSetLayout* pSetLayout) {
    VkResult result = device_dispatch_table(device)->CreateDescriptorSetLayout(device, pCreateInfo, pAllocator, pSetLayout);
    vk_json::s_pipe.objResInfo.descriptorSetLayoutRequestCount++;
    vk_json::s_pipe.objResInfo.descriptorSetLayoutBindingRequestCount += pCreateInfo->bindingCount;
    vk_json::s_pipe.addLayout(device, *pSetLayout, pCreateInfo);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetShaderBinaryDataEXT(VkDevice device, VkShaderEXT shader, size_t* pDataSize,
                                                                        void* pData) {
    VkResult result = device_dispatch_table(device)->GetShaderBinaryDataEXT(device, shader, pDataSize, pData);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCreateDescriptorPool(VkDevice device,
                                                                      const VkDescriptorPoolCreateInfo* pCreateInfo,
                                                                      const VkAllocationCallbacks* pAllocator,
                                                                      VkDescriptorPool* pDescriptorPool) {
    VkResult result = device_dispatch_table(device)->CreateDescriptorPool(device, pCreateInfo, pAllocator, pDescriptorPool);
    if (pCreateInfo) {
        vk_json::s_pipe.objResInfo.descriptorPoolRequestCount++;
    }

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetRayTracingShaderGroupHandlesKHR(VkDevice device, VkPipeline pipeline,
                                                                                    uint32_t firstGroup, uint32_t groupCount,
                                                                                    size_t dataSize, void* pData) {
    VkResult result = device_dispatch_table(device)->GetRayTracingShaderGroupHandlesKHR(device, pipeline, firstGroup, groupCount,
                                                                                        dataSize, pData);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetRayTracingShaderGroupHandlesNV(VkDevice device, VkPipeline pipeline,
                                                                                   uint32_t firstGroup, uint32_t groupCount,
                                                                                   size_t dataSize, void* pData) {
    VkResult result =
        device_dispatch_table(device)->GetRayTracingShaderGroupHandlesNV(device, pipeline, firstGroup, groupCount, dataSize, pData);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkAllocateDescriptorSets(VkDevice device,
                                                                        const VkDescriptorSetAllocateInfo* pAllocateInfo,
                                                                        VkDescriptorSet* pDescriptorSets) {
    VkResult result = device_dispatch_table(device)->AllocateDescriptorSets(device, pAllocateInfo, pDescriptorSets);
    if (pAllocateInfo) {
        vk_json::s_pipe.objResInfo.descriptorSetRequestCount += pAllocateInfo->descriptorSetCount;
    }

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL
vkGetImageDrmFormatModifierPropertiesEXT(VkDevice device, VkImage image, VkImageDrmFormatModifierPropertiesEXT* pProperties) {
    VkResult result = device_dispatch_table(device)->GetImageDrmFormatModifierPropertiesEXT(device, image, pProperties);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkQueueSubmit2(VkQueue queue, uint32_t submitCount, const VkSubmitInfo2* pSubmits,
                                                              VkFence fence) {
    VkResult result = device_dispatch_table(queue)->QueueSubmit2(queue, submitCount, pSubmits, fence);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetAccelerationStructureHandleNV(VkDevice device,
                                                                                  VkAccelerationStructureNV accelerationStructure,
                                                                                  size_t dataSize, void* pData) {
    VkResult result =
        device_dispatch_table(device)->GetAccelerationStructureHandleNV(device, accelerationStructure, dataSize, pData);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkResetDescriptorPool(VkDevice device, VkDescriptorPool descriptorPool,
                                                                     VkDescriptorPoolResetFlags flags) {
    VkResult result = device_dispatch_table(device)->ResetDescriptorPool(device, descriptorPool, flags);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkFreeDescriptorSets(VkDevice device, VkDescriptorPool descriptorPool,
                                                                    uint32_t descriptorSetCount,
                                                                    const VkDescriptorSet* pDescriptorSets) {
    VkResult result =
        device_dispatch_table(device)->FreeDescriptorSets(device, descriptorPool, descriptorSetCount, pDescriptorSets);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCompileDeferredNV(VkDevice device, VkPipeline pipeline, uint32_t shader) {
    VkResult result = device_dispatch_table(device)->CompileDeferredNV(device, pipeline, shader);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCreateVideoSessionKHR(VkDevice device,
                                                                       const VkVideoSessionCreateInfoKHR* pCreateInfo,
                                                                       const VkAllocationCallbacks* pAllocator,
                                                                       VkVideoSessionKHR* pVideoSession) {
    VkResult result = device_dispatch_table(device)->CreateVideoSessionKHR(device, pCreateInfo, pAllocator, pVideoSession);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkDeviceAddress VKAPI_CALL vkGetBufferDeviceAddressEXT(VkDevice device,
                                                                                  const VkBufferDeviceAddressInfo* pInfo) {
    VkDeviceAddress result = device_dispatch_table(device)->GetBufferDeviceAddressEXT(device, pInfo);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetSemaphoreCounterValueKHR(VkDevice device, VkSemaphore semaphore,
                                                                             uint64_t* pValue) {
    VkResult result = device_dispatch_table(device)->GetSemaphoreCounterValueKHR(device, semaphore, pValue);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL
vkGetVideoSessionMemoryRequirementsKHR(VkDevice device, VkVideoSessionKHR videoSession, uint32_t* pMemoryRequirementsCount,
                                       VkVideoSessionMemoryRequirementsKHR* pMemoryRequirements) {
    VkResult result = device_dispatch_table(device)->GetVideoSessionMemoryRequirementsKHR(
        device, videoSession, pMemoryRequirementsCount, pMemoryRequirements);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCreateValidationCacheEXT(VkDevice device,
                                                                          const VkValidationCacheCreateInfoEXT* pCreateInfo,
                                                                          const VkAllocationCallbacks* pAllocator,
                                                                          VkValidationCacheEXT* pValidationCache) {
    VkResult result = device_dispatch_table(device)->CreateValidationCacheEXT(device, pCreateInfo, pAllocator, pValidationCache);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkWaitSemaphoresKHR(VkDevice device, const VkSemaphoreWaitInfo* pWaitInfo,
                                                                   uint64_t timeout) {
    VkResult result = device_dispatch_table(device)->WaitSemaphoresKHR(device, pWaitInfo, timeout);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL
vkBindVideoSessionMemoryKHR(VkDevice device, VkVideoSessionKHR videoSession, uint32_t bindSessionMemoryInfoCount,
                            const VkBindVideoSessionMemoryInfoKHR* pBindSessionMemoryInfos) {
    VkResult result = device_dispatch_table(device)->BindVideoSessionMemoryKHR(device, videoSession, bindSessionMemoryInfoCount,
                                                                               pBindSessionMemoryInfos);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCreateRayTracingPipelinesKHR(
    VkDevice device, VkDeferredOperationKHR deferredOperation, VkPipelineCache pipelineCache, uint32_t createInfoCount,
    const VkRayTracingPipelineCreateInfoKHR* pCreateInfos, const VkAllocationCallbacks* pAllocator, VkPipeline* pPipelines) {
    VkResult result = device_dispatch_table(device)->CreateRayTracingPipelinesKHR(
        device, deferredOperation, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkSignalSemaphoreKHR(VkDevice device, const VkSemaphoreSignalInfo* pSignalInfo) {
    VkResult result = device_dispatch_table(device)->SignalSemaphoreKHR(device, pSignalInfo);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetValidationCacheDataEXT(VkDevice device, VkValidationCacheEXT validationCache,
                                                                           size_t* pDataSize, void* pData) {
    VkResult result = device_dispatch_table(device)->GetValidationCacheDataEXT(device, validationCache, pDataSize, pData);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkMergeValidationCachesEXT(VkDevice device, VkValidationCacheEXT dstCache,
                                                                          uint32_t srcCacheCount,
                                                                          const VkValidationCacheEXT* pSrcCaches) {
    VkResult result = device_dispatch_table(device)->MergeValidationCachesEXT(device, dstCache, srcCacheCount, pSrcCaches);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL
vkCreateVideoSessionParametersKHR(VkDevice device, const VkVideoSessionParametersCreateInfoKHR* pCreateInfo,
                                  const VkAllocationCallbacks* pAllocator, VkVideoSessionParametersKHR* pVideoSessionParameters) {
    VkResult result =
        device_dispatch_table(device)->CreateVideoSessionParametersKHR(device, pCreateInfo, pAllocator, pVideoSessionParameters);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkGetRayTracingCaptureReplayShaderGroupHandlesKHR(
    VkDevice device, VkPipeline pipeline, uint32_t firstGroup, uint32_t groupCount, size_t dataSize, void* pData) {
    VkResult result = device_dispatch_table(device)->GetRayTracingCaptureReplayShaderGroupHandlesKHR(device, pipeline, firstGroup,
                                                                                                     groupCount, dataSize, pData);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCreateSwapchainKHR(VkDevice device, const VkSwapchainCreateInfoKHR* pCreateInfo,
                                                                    const VkAllocationCallbacks* pAllocator,
                                                                    VkSwapchainKHR* pSwapchain) {
    VkResult result = device_dispatch_table(device)->CreateSwapchainKHR(device, pCreateInfo, pAllocator, pSwapchain);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkUpdateVideoSessionParametersKHR(
    VkDevice device, VkVideoSessionParametersKHR videoSessionParameters, const VkVideoSessionParametersUpdateInfoKHR* pUpdateInfo) {
    VkResult result = device_dispatch_table(device)->UpdateVideoSessionParametersKHR(device, videoSessionParameters, pUpdateInfo);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCreateGraphicsPipelines(VkDevice device, VkPipelineCache pipelineCache,
                                                                         uint32_t createInfoCount,
                                                                         const VkGraphicsPipelineCreateInfo* pCreateInfos,
                                                                         const VkAllocationCallbacks* pAllocator,
                                                                         VkPipeline* pPipelines) {
    VkResult result = device_dispatch_table(device)->CreateGraphicsPipelines(device, pipelineCache, createInfoCount, pCreateInfos,
                                                                             pAllocator, pPipelines);
    vk_json::s_pipe.objResInfo.graphicsPipelineRequestCount += createInfoCount;
    vk_json::s_pipe.dumpGraphicsPipeline(device, createInfoCount, pCreateInfos, pPipelines);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL vkCreateComputePipelines(VkDevice device, VkPipelineCache pipelineCache,
                                                                        uint32_t createInfoCount,
                                                                        const VkComputePipelineCreateInfo* pCreateInfos,
                                                                        const VkAllocationCallbacks* pAllocator,
                                                                        VkPipeline* pPipelines) {
    VkResult result = device_dispatch_table(device)->CreateComputePipelines(device, pipelineCache, createInfoCount, pCreateInfos,
                                                                            pAllocator, pPipelines);
    vk_json::s_pipe.objResInfo.computePipelineRequestCount += createInfoCount;
    vk_json::s_pipe.dumpComputePipeline(device, createInfoCount, pCreateInfos, pPipelines);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL
vkCreateIndirectCommandsLayoutNV(VkDevice device, const VkIndirectCommandsLayoutCreateInfoNV* pCreateInfo,
                                 const VkAllocationCallbacks* pAllocator, VkIndirectCommandsLayoutNV* pIndirectCommandsLayout) {
    VkResult result =
        device_dispatch_table(device)->CreateIndirectCommandsLayoutNV(device, pCreateInfo, pAllocator, pIndirectCommandsLayout);

    return result;
}
EXPORT_FUNCTION VKAPI_ATTR VkDeviceSize VKAPI_CALL vkGetRayTracingShaderGroupStackSizeKHR(VkDevice device, VkPipeline pipeline,
                                                                                          uint32_t group,
                                                                                          VkShaderGroupShaderKHR groupShader) {
    VkDeviceSize result = device_dispatch_table(device)->GetRayTracingShaderGroupStackSizeKHR(device, pipeline, group, groupShader);

    return result;
}

EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetDepthBounds(VkCommandBuffer commandBuffer, float minDepthBounds,
                                                               float maxDepthBounds) {
    device_dispatch_table(commandBuffer)->CmdSetDepthBounds(commandBuffer, minDepthBounds, maxDepthBounds);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetBufferMemoryRequirements(VkDevice device, VkBuffer buffer,
                                                                         VkMemoryRequirements* pMemoryRequirements) {
    device_dispatch_table(device)->GetBufferMemoryRequirements(device, buffer, pMemoryRequirements);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdBeginRenderPass2(VkCommandBuffer commandBuffer,
                                                                 const VkRenderPassBeginInfo* pRenderPassBegin,
                                                                 const VkSubpassBeginInfo* pSubpassBeginInfo) {
    device_dispatch_table(commandBuffer)->CmdBeginRenderPass2(commandBuffer, pRenderPassBegin, pSubpassBeginInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetStencilCompareMask(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask,
                                                                      uint32_t compareMask) {
    device_dispatch_table(commandBuffer)->CmdSetStencilCompareMask(commandBuffer, faceMask, compareMask);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetImageMemoryRequirements(VkDevice device, VkImage image,
                                                                        VkMemoryRequirements* pMemoryRequirements) {
    device_dispatch_table(device)->GetImageMemoryRequirements(device, image, pMemoryRequirements);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdEndConditionalRenderingEXT(VkCommandBuffer commandBuffer) {
    device_dispatch_table(commandBuffer)->CmdEndConditionalRenderingEXT(commandBuffer);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdBeginConditionalRenderingEXT(
    VkCommandBuffer commandBuffer, const VkConditionalRenderingBeginInfoEXT* pConditionalRenderingBegin) {
    device_dispatch_table(commandBuffer)->CmdBeginConditionalRenderingEXT(commandBuffer, pConditionalRenderingBegin);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetStencilWriteMask(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask,
                                                                    uint32_t writeMask) {
    device_dispatch_table(commandBuffer)->CmdSetStencilWriteMask(commandBuffer, faceMask, writeMask);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdNextSubpass2(VkCommandBuffer commandBuffer,
                                                             const VkSubpassBeginInfo* pSubpassBeginInfo,
                                                             const VkSubpassEndInfo* pSubpassEndInfo) {
    device_dispatch_table(commandBuffer)->CmdNextSubpass2(commandBuffer, pSubpassBeginInfo, pSubpassEndInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdEndRenderPass2(VkCommandBuffer commandBuffer,
                                                               const VkSubpassEndInfo* pSubpassEndInfo) {
    device_dispatch_table(commandBuffer)->CmdEndRenderPass2(commandBuffer, pSubpassEndInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetStencilReference(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask,
                                                                    uint32_t reference) {
    device_dispatch_table(commandBuffer)->CmdSetStencilReference(commandBuffer, faceMask, reference);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdBindDescriptorSets(VkCommandBuffer commandBuffer,
                                                                   VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout,
                                                                   uint32_t firstSet, uint32_t descriptorSetCount,
                                                                   const VkDescriptorSet* pDescriptorSets,
                                                                   uint32_t dynamicOffsetCount, const uint32_t* pDynamicOffsets) {
    device_dispatch_table(commandBuffer)
        ->CmdBindDescriptorSets(commandBuffer, pipelineBindPoint, layout, firstSet, descriptorSetCount, pDescriptorSets,
                                dynamicOffsetCount, pDynamicOffsets);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkUpdateDescriptorSetWithTemplateKHR(VkDevice device, VkDescriptorSet descriptorSet,
                                                                                VkDescriptorUpdateTemplate descriptorUpdateTemplate,
                                                                                const void* pData) {
    device_dispatch_table(device)->UpdateDescriptorSetWithTemplateKHR(device, descriptorSet, descriptorUpdateTemplate, pData);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkSetLatencyMarkerNV(VkDevice device, VkSwapchainKHR swapchain,
                                                                const VkSetLatencyMarkerInfoNV* pLatencyMarkerInfo) {
    device_dispatch_table(device)->SetLatencyMarkerNV(device, swapchain, pLatencyMarkerInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdBindIndexBuffer(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset,
                                                                VkIndexType indexType) {
    device_dispatch_table(commandBuffer)->CmdBindIndexBuffer(commandBuffer, buffer, offset, indexType);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkDestroyPipelineBinaryKHR(VkDevice device, VkPipelineBinaryKHR pipelineBinary,
                                                                      const VkAllocationCallbacks* pAllocator) {
    device_dispatch_table(device)->DestroyPipelineBinaryKHR(device, pipelineBinary, pAllocator);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetLatencyTimingsNV(VkDevice device, VkSwapchainKHR swapchain,
                                                                 VkGetLatencyMarkerInfoNV* pLatencyMarkerInfo) {
    device_dispatch_table(device)->GetLatencyTimingsNV(device, swapchain, pLatencyMarkerInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkQueueNotifyOutOfBandNV(VkQueue queue,
                                                                    const VkOutOfBandQueueTypeInfoNV* pQueueTypeInfo) {
    device_dispatch_table(queue)->QueueNotifyOutOfBandNV(queue, pQueueTypeInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkDestroyDescriptorUpdateTemplateKHR(VkDevice device,
                                                                                VkDescriptorUpdateTemplate descriptorUpdateTemplate,
                                                                                const VkAllocationCallbacks* pAllocator) {
    device_dispatch_table(device)->DestroyDescriptorUpdateTemplateKHR(device, descriptorUpdateTemplate, pAllocator);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdDraw(VkCommandBuffer commandBuffer, uint32_t vertexCount, uint32_t instanceCount,
                                                     uint32_t firstVertex, uint32_t firstInstance) {
    device_dispatch_table(commandBuffer)->CmdDraw(commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdCopyBuffer2(VkCommandBuffer commandBuffer,
                                                            const VkCopyBufferInfo2* pCopyBufferInfo) {
    device_dispatch_table(commandBuffer)->CmdCopyBuffer2(commandBuffer, pCopyBufferInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdBindVertexBuffers(VkCommandBuffer commandBuffer, uint32_t firstBinding,
                                                                  uint32_t bindingCount, const VkBuffer* pBuffers,
                                                                  const VkDeviceSize* pOffsets) {
    device_dispatch_table(commandBuffer)->CmdBindVertexBuffers(commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkDestroyBuffer(VkDevice device, VkBuffer buffer,
                                                           const VkAllocationCallbacks* pAllocator) {
    device_dispatch_table(device)->DestroyBuffer(device, buffer, pAllocator);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdDrawIndexed(VkCommandBuffer commandBuffer, uint32_t indexCount,
                                                            uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset,
                                                            uint32_t firstInstance) {
    device_dispatch_table(commandBuffer)
        ->CmdDrawIndexed(commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL
vkGetDescriptorSetLayoutHostMappingInfoVALVE(VkDevice device, const VkDescriptorSetBindingReferenceVALVE* pBindingReference,
                                             VkDescriptorSetLayoutHostMappingInfoVALVE* pHostMapping) {
    device_dispatch_table(device)->GetDescriptorSetLayoutHostMappingInfoVALVE(device, pBindingReference, pHostMapping);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetViewportWScalingNV(VkCommandBuffer commandBuffer, uint32_t firstViewport,
                                                                      uint32_t viewportCount,
                                                                      const VkViewportWScalingNV* pViewportWScalings) {
    device_dispatch_table(commandBuffer)->CmdSetViewportWScalingNV(commandBuffer, firstViewport, viewportCount, pViewportWScalings);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetDepthClampEnableEXT(VkCommandBuffer commandBuffer, VkBool32 depthClampEnable) {
    device_dispatch_table(commandBuffer)->CmdSetDepthClampEnableEXT(commandBuffer, depthClampEnable);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetDescriptorSetHostMappingVALVE(VkDevice device, VkDescriptorSet descriptorSet,
                                                                              void** ppData) {
    device_dispatch_table(device)->GetDescriptorSetHostMappingVALVE(device, descriptorSet, ppData);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetRasterizationSamplesEXT(VkCommandBuffer commandBuffer,
                                                                           VkSampleCountFlagBits rasterizationSamples) {
    device_dispatch_table(commandBuffer)->CmdSetRasterizationSamplesEXT(commandBuffer, rasterizationSamples);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdDrawIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset,
                                                             uint32_t drawCount, uint32_t stride) {
    device_dispatch_table(commandBuffer)->CmdDrawIndirect(commandBuffer, buffer, offset, drawCount, stride);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetPolygonModeEXT(VkCommandBuffer commandBuffer, VkPolygonMode polygonMode) {
    device_dispatch_table(commandBuffer)->CmdSetPolygonModeEXT(commandBuffer, polygonMode);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdDispatchIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer,
                                                                 VkDeviceSize offset) {
    device_dispatch_table(commandBuffer)->CmdDispatchIndirect(commandBuffer, buffer, offset);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetAttachmentFeedbackLoopEnableEXT(VkCommandBuffer commandBuffer,
                                                                                   VkImageAspectFlags aspectMask) {
    device_dispatch_table(commandBuffer)->CmdSetAttachmentFeedbackLoopEnableEXT(commandBuffer, aspectMask);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdDrawIndexedIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer,
                                                                    VkDeviceSize offset, uint32_t drawCount, uint32_t stride) {
    device_dispatch_table(commandBuffer)->CmdDrawIndexedIndirect(commandBuffer, buffer, offset, drawCount, stride);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdCopyImage2(VkCommandBuffer commandBuffer, const VkCopyImageInfo2* pCopyImageInfo) {
    device_dispatch_table(commandBuffer)->CmdCopyImage2(commandBuffer, pCopyImageInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetSampleMaskEXT(VkCommandBuffer commandBuffer, VkSampleCountFlagBits samples,
                                                                 const VkSampleMask* pSampleMask) {
    device_dispatch_table(commandBuffer)->CmdSetSampleMaskEXT(commandBuffer, samples, pSampleMask);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdCopyBufferToImage2(VkCommandBuffer commandBuffer,
                                                                   const VkCopyBufferToImageInfo2* pCopyBufferToImageInfo) {
    device_dispatch_table(commandBuffer)->CmdCopyBufferToImage2(commandBuffer, pCopyBufferToImageInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetAlphaToCoverageEnableEXT(VkCommandBuffer commandBuffer,
                                                                            VkBool32 alphaToCoverageEnable) {
    device_dispatch_table(commandBuffer)->CmdSetAlphaToCoverageEnableEXT(commandBuffer, alphaToCoverageEnable);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkDestroyBufferView(VkDevice device, VkBufferView bufferView,
                                                               const VkAllocationCallbacks* pAllocator) {
    device_dispatch_table(device)->DestroyBufferView(device, bufferView, pAllocator);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdDispatch(VkCommandBuffer commandBuffer, uint32_t groupCountX, uint32_t groupCountY,
                                                         uint32_t groupCountZ) {
    device_dispatch_table(commandBuffer)->CmdDispatch(commandBuffer, groupCountX, groupCountY, groupCountZ);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetLogicOpEnableEXT(VkCommandBuffer commandBuffer, VkBool32 logicOpEnable) {
    device_dispatch_table(commandBuffer)->CmdSetLogicOpEnableEXT(commandBuffer, logicOpEnable);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdCopyImageToBuffer2(VkCommandBuffer commandBuffer,
                                                                   const VkCopyImageToBufferInfo2* pCopyImageToBufferInfo) {
    device_dispatch_table(commandBuffer)->CmdCopyImageToBuffer2(commandBuffer, pCopyImageToBufferInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdBlitImage2(VkCommandBuffer commandBuffer, const VkBlitImageInfo2* pBlitImageInfo) {
    device_dispatch_table(commandBuffer)->CmdBlitImage2(commandBuffer, pBlitImageInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetAlphaToOneEnableEXT(VkCommandBuffer commandBuffer, VkBool32 alphaToOneEnable) {
    device_dispatch_table(commandBuffer)->CmdSetAlphaToOneEnableEXT(commandBuffer, alphaToOneEnable);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdResolveImage2(VkCommandBuffer commandBuffer,
                                                              const VkResolveImageInfo2* pResolveImageInfo) {
    device_dispatch_table(commandBuffer)->CmdResolveImage2(commandBuffer, pResolveImageInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetLineStippleKHR(VkCommandBuffer commandBuffer, uint32_t lineStippleFactor,
                                                                  uint16_t lineStipplePattern) {
    device_dispatch_table(commandBuffer)->CmdSetLineStippleKHR(commandBuffer, lineStippleFactor, lineStipplePattern);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetColorBlendEnableEXT(VkCommandBuffer commandBuffer, uint32_t firstAttachment,
                                                                       uint32_t attachmentCount,
                                                                       const VkBool32* pColorBlendEnables) {
    device_dispatch_table(commandBuffer)
        ->CmdSetColorBlendEnableEXT(commandBuffer, firstAttachment, attachmentCount, pColorBlendEnables);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetColorBlendEquationEXT(VkCommandBuffer commandBuffer, uint32_t firstAttachment,
                                                                         uint32_t attachmentCount,
                                                                         const VkColorBlendEquationEXT* pColorBlendEquations) {
    device_dispatch_table(commandBuffer)
        ->CmdSetColorBlendEquationEXT(commandBuffer, firstAttachment, attachmentCount, pColorBlendEquations);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL
vkGetImageSparseMemoryRequirements(VkDevice device, VkImage image, uint32_t* pSparseMemoryRequirementCount,
                                   VkSparseImageMemoryRequirements* pSparseMemoryRequirements) {
    device_dispatch_table(device)->GetImageSparseMemoryRequirements(device, image, pSparseMemoryRequirementCount,
                                                                    pSparseMemoryRequirements);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdCopyImage(VkCommandBuffer commandBuffer, VkImage srcImage,
                                                          VkImageLayout srcImageLayout, VkImage dstImage,
                                                          VkImageLayout dstImageLayout, uint32_t regionCount,
                                                          const VkImageCopy* pRegions) {
    device_dispatch_table(commandBuffer)
        ->CmdCopyImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdCopyBuffer(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkBuffer dstBuffer,
                                                           uint32_t regionCount, const VkBufferCopy* pRegions) {
    device_dispatch_table(commandBuffer)->CmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, regionCount, pRegions);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdBindDescriptorSets2KHR(VkCommandBuffer commandBuffer,
                                                                       const VkBindDescriptorSetsInfo* pBindDescriptorSetsInfo) {
    device_dispatch_table(commandBuffer)->CmdBindDescriptorSets2KHR(commandBuffer, pBindDescriptorSetsInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdPushDescriptorSet2KHR(VkCommandBuffer commandBuffer,
                                                                      const VkPushDescriptorSetInfo* pPushDescriptorSetInfo) {
    device_dispatch_table(commandBuffer)->CmdPushDescriptorSet2KHR(commandBuffer, pPushDescriptorSetInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdBlitImage(VkCommandBuffer commandBuffer, VkImage srcImage,
                                                          VkImageLayout srcImageLayout, VkImage dstImage,
                                                          VkImageLayout dstImageLayout, uint32_t regionCount,
                                                          const VkImageBlit* pRegions, VkFilter filter) {
    device_dispatch_table(commandBuffer)
        ->CmdBlitImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions, filter);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetColorWriteMaskEXT(VkCommandBuffer commandBuffer, uint32_t firstAttachment,
                                                                     uint32_t attachmentCount,
                                                                     const VkColorComponentFlags* pColorWriteMasks) {
    device_dispatch_table(commandBuffer)
        ->CmdSetColorWriteMaskEXT(commandBuffer, firstAttachment, attachmentCount, pColorWriteMasks);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdPushConstants2KHR(VkCommandBuffer commandBuffer,
                                                                  const VkPushConstantsInfo* pPushConstantsInfo) {
    device_dispatch_table(commandBuffer)->CmdPushConstants2KHR(commandBuffer, pPushConstantsInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetTessellationDomainOriginEXT(VkCommandBuffer commandBuffer,
                                                                               VkTessellationDomainOrigin domainOrigin) {
    device_dispatch_table(commandBuffer)->CmdSetTessellationDomainOriginEXT(commandBuffer, domainOrigin);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdPushDescriptorSetWithTemplate2KHR(
    VkCommandBuffer commandBuffer, const VkPushDescriptorSetWithTemplateInfo* pPushDescriptorSetWithTemplateInfo) {
    device_dispatch_table(commandBuffer)->CmdPushDescriptorSetWithTemplate2KHR(commandBuffer, pPushDescriptorSetWithTemplateInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetRasterizationStreamEXT(VkCommandBuffer commandBuffer,
                                                                          uint32_t rasterizationStream) {
    device_dispatch_table(commandBuffer)->CmdSetRasterizationStreamEXT(commandBuffer, rasterizationStream);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetImageSubresourceLayout(VkDevice device, VkImage image,
                                                                       const VkImageSubresource* pSubresource,
                                                                       VkSubresourceLayout* pLayout) {
    device_dispatch_table(device)->GetImageSubresourceLayout(device, image, pSubresource, pLayout);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetConservativeRasterizationModeEXT(
    VkCommandBuffer commandBuffer, VkConservativeRasterizationModeEXT conservativeRasterizationMode) {
    device_dispatch_table(commandBuffer)->CmdSetConservativeRasterizationModeEXT(commandBuffer, conservativeRasterizationMode);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdCopyBufferToImage(VkCommandBuffer commandBuffer, VkBuffer srcBuffer,
                                                                  VkImage dstImage, VkImageLayout dstImageLayout,
                                                                  uint32_t regionCount, const VkBufferImageCopy* pRegions) {
    device_dispatch_table(commandBuffer)
        ->CmdCopyBufferToImage(commandBuffer, srcBuffer, dstImage, dstImageLayout, regionCount, pRegions);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetExtraPrimitiveOverestimationSizeEXT(VkCommandBuffer commandBuffer,
                                                                                       float extraPrimitiveOverestimationSize) {
    device_dispatch_table(commandBuffer)
        ->CmdSetExtraPrimitiveOverestimationSizeEXT(commandBuffer, extraPrimitiveOverestimationSize);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkDestroyImage(VkDevice device, VkImage image, const VkAllocationCallbacks* pAllocator) {
    device_dispatch_table(device)->DestroyImage(device, image, pAllocator);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetDepthClipEnableEXT(VkCommandBuffer commandBuffer, VkBool32 depthClipEnable) {
    device_dispatch_table(commandBuffer)->CmdSetDepthClipEnableEXT(commandBuffer, depthClipEnable);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdBeginRenderingKHR(VkCommandBuffer commandBuffer,
                                                                  const VkRenderingInfo* pRenderingInfo) {
    device_dispatch_table(commandBuffer)->CmdBeginRenderingKHR(commandBuffer, pRenderingInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetSampleLocationsEnableEXT(VkCommandBuffer commandBuffer,
                                                                            VkBool32 sampleLocationsEnable) {
    device_dispatch_table(commandBuffer)->CmdSetSampleLocationsEnableEXT(commandBuffer, sampleLocationsEnable);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetDescriptorBufferOffsets2EXT(
    VkCommandBuffer commandBuffer, const VkSetDescriptorBufferOffsetsInfoEXT* pSetDescriptorBufferOffsetsInfo) {
    device_dispatch_table(commandBuffer)->CmdSetDescriptorBufferOffsets2EXT(commandBuffer, pSetDescriptorBufferOffsetsInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdCopyImageToBuffer(VkCommandBuffer commandBuffer, VkImage srcImage,
                                                                  VkImageLayout srcImageLayout, VkBuffer dstBuffer,
                                                                  uint32_t regionCount, const VkBufferImageCopy* pRegions) {
    device_dispatch_table(commandBuffer)
        ->CmdCopyImageToBuffer(commandBuffer, srcImage, srcImageLayout, dstBuffer, regionCount, pRegions);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetColorBlendAdvancedEXT(VkCommandBuffer commandBuffer, uint32_t firstAttachment,
                                                                         uint32_t attachmentCount,
                                                                         const VkColorBlendAdvancedEXT* pColorBlendAdvanced) {
    device_dispatch_table(commandBuffer)
        ->CmdSetColorBlendAdvancedEXT(commandBuffer, firstAttachment, attachmentCount, pColorBlendAdvanced);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetProvokingVertexModeEXT(VkCommandBuffer commandBuffer,
                                                                          VkProvokingVertexModeEXT provokingVertexMode) {
    device_dispatch_table(commandBuffer)->CmdSetProvokingVertexModeEXT(commandBuffer, provokingVertexMode);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkDestroyFence(VkDevice device, VkFence fence, const VkAllocationCallbacks* pAllocator) {
    device_dispatch_table(device)->DestroyFence(device, fence, pAllocator);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdEndRenderingKHR(VkCommandBuffer commandBuffer) {
    device_dispatch_table(commandBuffer)->CmdEndRenderingKHR(commandBuffer);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdBindDescriptorBufferEmbeddedSamplers2EXT(
    VkCommandBuffer commandBuffer, const VkBindDescriptorBufferEmbeddedSamplersInfoEXT* pBindDescriptorBufferEmbeddedSamplersInfo) {
    device_dispatch_table(commandBuffer)
        ->CmdBindDescriptorBufferEmbeddedSamplers2EXT(commandBuffer, pBindDescriptorBufferEmbeddedSamplersInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdUpdateBuffer(VkCommandBuffer commandBuffer, VkBuffer dstBuffer,
                                                             VkDeviceSize dstOffset, VkDeviceSize dataSize, const void* pData) {
    device_dispatch_table(commandBuffer)->CmdUpdateBuffer(commandBuffer, dstBuffer, dstOffset, dataSize, pData);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetLineRasterizationModeEXT(VkCommandBuffer commandBuffer,
                                                                            VkLineRasterizationModeEXT lineRasterizationMode) {
    device_dispatch_table(commandBuffer)->CmdSetLineRasterizationModeEXT(commandBuffer, lineRasterizationMode);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetLineStippleEnableEXT(VkCommandBuffer commandBuffer,
                                                                        VkBool32 stippledLineEnable) {
    device_dispatch_table(commandBuffer)->CmdSetLineStippleEnableEXT(commandBuffer, stippledLineEnable);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetDepthClipNegativeOneToOneEXT(VkCommandBuffer commandBuffer,
                                                                                VkBool32 negativeOneToOne) {
    device_dispatch_table(commandBuffer)->CmdSetDepthClipNegativeOneToOneEXT(commandBuffer, negativeOneToOne);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdFillBuffer(VkCommandBuffer commandBuffer, VkBuffer dstBuffer,
                                                           VkDeviceSize dstOffset, VkDeviceSize size, uint32_t data) {
    device_dispatch_table(commandBuffer)->CmdFillBuffer(commandBuffer, dstBuffer, dstOffset, size, data);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetGeneratedCommandsMemoryRequirementsEXT(
    VkDevice device, const VkGeneratedCommandsMemoryRequirementsInfoEXT* pInfo, VkMemoryRequirements2* pMemoryRequirements) {
    device_dispatch_table(device)->GetGeneratedCommandsMemoryRequirementsEXT(device, pInfo, pMemoryRequirements);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetViewportWScalingEnableNV(VkCommandBuffer commandBuffer,
                                                                            VkBool32 viewportWScalingEnable) {
    device_dispatch_table(commandBuffer)->CmdSetViewportWScalingEnableNV(commandBuffer, viewportWScalingEnable);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetViewportSwizzleNV(VkCommandBuffer commandBuffer, uint32_t firstViewport,
                                                                     uint32_t viewportCount,
                                                                     const VkViewportSwizzleNV* pViewportSwizzles) {
    device_dispatch_table(commandBuffer)->CmdSetViewportSwizzleNV(commandBuffer, firstViewport, viewportCount, pViewportSwizzles);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdClearColorImage(VkCommandBuffer commandBuffer, VkImage image,
                                                                VkImageLayout imageLayout, const VkClearColorValue* pColor,
                                                                uint32_t rangeCount, const VkImageSubresourceRange* pRanges) {
    device_dispatch_table(commandBuffer)->CmdClearColorImage(commandBuffer, image, imageLayout, pColor, rangeCount, pRanges);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetCoverageToColorEnableNV(VkCommandBuffer commandBuffer,
                                                                           VkBool32 coverageToColorEnable) {
    device_dispatch_table(commandBuffer)->CmdSetCoverageToColorEnableNV(commandBuffer, coverageToColorEnable);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdPreprocessGeneratedCommandsEXT(
    VkCommandBuffer commandBuffer, const VkGeneratedCommandsInfoEXT* pGeneratedCommandsInfo, VkCommandBuffer stateCommandBuffer) {
    device_dispatch_table(commandBuffer)
        ->CmdPreprocessGeneratedCommandsEXT(commandBuffer, pGeneratedCommandsInfo, stateCommandBuffer);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetCoverageToColorLocationNV(VkCommandBuffer commandBuffer,
                                                                             uint32_t coverageToColorLocation) {
    device_dispatch_table(commandBuffer)->CmdSetCoverageToColorLocationNV(commandBuffer, coverageToColorLocation);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdResolveImage(VkCommandBuffer commandBuffer, VkImage srcImage,
                                                             VkImageLayout srcImageLayout, VkImage dstImage,
                                                             VkImageLayout dstImageLayout, uint32_t regionCount,
                                                             const VkImageResolve* pRegions) {
    device_dispatch_table(commandBuffer)
        ->CmdResolveImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetCoverageModulationModeNV(VkCommandBuffer commandBuffer,
                                                                            VkCoverageModulationModeNV coverageModulationMode) {
    device_dispatch_table(commandBuffer)->CmdSetCoverageModulationModeNV(commandBuffer, coverageModulationMode);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdClearDepthStencilImage(VkCommandBuffer commandBuffer, VkImage image,
                                                                       VkImageLayout imageLayout,
                                                                       const VkClearDepthStencilValue* pDepthStencil,
                                                                       uint32_t rangeCount,
                                                                       const VkImageSubresourceRange* pRanges) {
    device_dispatch_table(commandBuffer)
        ->CmdClearDepthStencilImage(commandBuffer, image, imageLayout, pDepthStencil, rangeCount, pRanges);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdExecuteGeneratedCommandsEXT(
    VkCommandBuffer commandBuffer, VkBool32 isPreprocessed, const VkGeneratedCommandsInfoEXT* pGeneratedCommandsInfo) {
    device_dispatch_table(commandBuffer)->CmdExecuteGeneratedCommandsEXT(commandBuffer, isPreprocessed, pGeneratedCommandsInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdClearAttachments(VkCommandBuffer commandBuffer, uint32_t attachmentCount,
                                                                 const VkClearAttachment* pAttachments, uint32_t rectCount,
                                                                 const VkClearRect* pRects) {
    device_dispatch_table(commandBuffer)->CmdClearAttachments(commandBuffer, attachmentCount, pAttachments, rectCount, pRects);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetCoverageModulationTableEnableNV(VkCommandBuffer commandBuffer,
                                                                                   VkBool32 coverageModulationTableEnable) {
    device_dispatch_table(commandBuffer)->CmdSetCoverageModulationTableEnableNV(commandBuffer, coverageModulationTableEnable);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdCopyMemoryIndirectNV(VkCommandBuffer commandBuffer,
                                                                     VkDeviceAddress copyBufferAddress, uint32_t copyCount,
                                                                     uint32_t stride) {
    device_dispatch_table(commandBuffer)->CmdCopyMemoryIndirectNV(commandBuffer, copyBufferAddress, copyCount, stride);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkDestroySemaphore(VkDevice device, VkSemaphore semaphore,
                                                              const VkAllocationCallbacks* pAllocator) {
    device_dispatch_table(device)->DestroySemaphore(device, semaphore, pAllocator);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkDestroyIndirectExecutionSetEXT(VkDevice device,
                                                                            VkIndirectExecutionSetEXT indirectExecutionSet,
                                                                            const VkAllocationCallbacks* pAllocator) {
    device_dispatch_table(device)->DestroyIndirectExecutionSetEXT(device, indirectExecutionSet, pAllocator);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetCoverageModulationTableNV(VkCommandBuffer commandBuffer,
                                                                             uint32_t coverageModulationTableCount,
                                                                             const float* pCoverageModulationTable) {
    device_dispatch_table(commandBuffer)
        ->CmdSetCoverageModulationTableNV(commandBuffer, coverageModulationTableCount, pCoverageModulationTable);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkDestroyIndirectCommandsLayoutEXT(VkDevice device,
                                                                              VkIndirectCommandsLayoutEXT indirectCommandsLayout,
                                                                              const VkAllocationCallbacks* pAllocator) {
    device_dispatch_table(device)->DestroyIndirectCommandsLayoutEXT(device, indirectCommandsLayout, pAllocator);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetShadingRateImageEnableNV(VkCommandBuffer commandBuffer,
                                                                            VkBool32 shadingRateImageEnable) {
    device_dispatch_table(commandBuffer)->CmdSetShadingRateImageEnableNV(commandBuffer, shadingRateImageEnable);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdCopyMemoryToImageIndirectNV(VkCommandBuffer commandBuffer,
                                                                            VkDeviceAddress copyBufferAddress, uint32_t copyCount,
                                                                            uint32_t stride, VkImage dstImage,
                                                                            VkImageLayout dstImageLayout,
                                                                            const VkImageSubresourceLayers* pImageSubresources) {
    device_dispatch_table(commandBuffer)
        ->CmdCopyMemoryToImageIndirectNV(commandBuffer, copyBufferAddress, copyCount, stride, dstImage, dstImageLayout,
                                         pImageSubresources);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetCoverageReductionModeNV(VkCommandBuffer commandBuffer,
                                                                           VkCoverageReductionModeNV coverageReductionMode) {
    device_dispatch_table(commandBuffer)->CmdSetCoverageReductionModeNV(commandBuffer, coverageReductionMode);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetRepresentativeFragmentTestEnableNV(VkCommandBuffer commandBuffer,
                                                                                      VkBool32 representativeFragmentTestEnable) {
    device_dispatch_table(commandBuffer)->CmdSetRepresentativeFragmentTestEnableNV(commandBuffer, representativeFragmentTestEnable);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkUpdateIndirectExecutionSetShaderEXT(
    VkDevice device, VkIndirectExecutionSetEXT indirectExecutionSet, uint32_t executionSetWriteCount,
    const VkWriteIndirectExecutionSetShaderEXT* pExecutionSetWrites) {
    device_dispatch_table(device)->UpdateIndirectExecutionSetShaderEXT(device, indirectExecutionSet, executionSetWriteCount,
                                                                       pExecutionSetWrites);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkUpdateIndirectExecutionSetPipelineEXT(
    VkDevice device, VkIndirectExecutionSetEXT indirectExecutionSet, uint32_t executionSetWriteCount,
    const VkWriteIndirectExecutionSetPipelineEXT* pExecutionSetWrites) {
    device_dispatch_table(device)->UpdateIndirectExecutionSetPipelineEXT(device, indirectExecutionSet, executionSetWriteCount,
                                                                         pExecutionSetWrites);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetEvent(VkCommandBuffer commandBuffer, VkEvent event,
                                                         VkPipelineStageFlags stageMask) {
    device_dispatch_table(commandBuffer)->CmdSetEvent(commandBuffer, event, stageMask);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkDestroyEvent(VkDevice device, VkEvent event, const VkAllocationCallbacks* pAllocator) {
    device_dispatch_table(device)->DestroyEvent(device, event, pAllocator);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdResetEvent(VkCommandBuffer commandBuffer, VkEvent event,
                                                           VkPipelineStageFlags stageMask) {
    device_dispatch_table(commandBuffer)->CmdResetEvent(commandBuffer, event, stageMask);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkDestroyImageView(VkDevice device, VkImageView imageView,
                                                              const VkAllocationCallbacks* pAllocator) {
    device_dispatch_table(device)->DestroyImageView(device, imageView, pAllocator);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdConvertCooperativeVectorMatrixNV(
    VkCommandBuffer commandBuffer, uint32_t infoCount, const VkConvertCooperativeVectorMatrixInfoNV* pInfos) {
    device_dispatch_table(commandBuffer)->CmdConvertCooperativeVectorMatrixNV(commandBuffer, infoCount, pInfos);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL
vkCmdWaitEvents(VkCommandBuffer commandBuffer, uint32_t eventCount, const VkEvent* pEvents, VkPipelineStageFlags srcStageMask,
                VkPipelineStageFlags dstStageMask, uint32_t memoryBarrierCount, const VkMemoryBarrier* pMemoryBarriers,
                uint32_t bufferMemoryBarrierCount, const VkBufferMemoryBarrier* pBufferMemoryBarriers,
                uint32_t imageMemoryBarrierCount, const VkImageMemoryBarrier* pImageMemoryBarriers) {
    device_dispatch_table(commandBuffer)
        ->CmdWaitEvents(commandBuffer, eventCount, pEvents, srcStageMask, dstStageMask, memoryBarrierCount, pMemoryBarriers,
                        bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetLineStippleEXT(VkCommandBuffer commandBuffer, uint32_t lineStippleFactor,
                                                                  uint16_t lineStipplePattern) {
    device_dispatch_table(commandBuffer)->CmdSetLineStippleEXT(commandBuffer, lineStippleFactor, lineStipplePattern);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL
vkCmdPipelineBarrier(VkCommandBuffer commandBuffer, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask,
                     VkDependencyFlags dependencyFlags, uint32_t memoryBarrierCount, const VkMemoryBarrier* pMemoryBarriers,
                     uint32_t bufferMemoryBarrierCount, const VkBufferMemoryBarrier* pBufferMemoryBarriers,
                     uint32_t imageMemoryBarrierCount, const VkImageMemoryBarrier* pImageMemoryBarriers) {
    device_dispatch_table(commandBuffer)
        ->CmdPipelineBarrier(commandBuffer, srcStageMask, dstStageMask, dependencyFlags, memoryBarrierCount, pMemoryBarriers,
                             bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdDecompressMemoryNV(VkCommandBuffer commandBuffer, uint32_t decompressRegionCount,
                                                                   const VkDecompressMemoryRegionNV* pDecompressMemoryRegions) {
    device_dispatch_table(commandBuffer)->CmdDecompressMemoryNV(commandBuffer, decompressRegionCount, pDecompressMemoryRegions);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdBeginQuery(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t query,
                                                           VkQueryControlFlags flags) {
    device_dispatch_table(commandBuffer)->CmdBeginQuery(commandBuffer, queryPool, query, flags);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkReleaseProfilingLockKHR(VkDevice device) {
    device_dispatch_table(device)->ReleaseProfilingLockKHR(device);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkResetQueryPoolEXT(VkDevice device, VkQueryPool queryPool, uint32_t firstQuery,
                                                               uint32_t queryCount) {
    device_dispatch_table(device)->ResetQueryPoolEXT(device, queryPool, firstQuery, queryCount);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdBeginRendering(VkCommandBuffer commandBuffer,
                                                               const VkRenderingInfo* pRenderingInfo) {
    device_dispatch_table(commandBuffer)->CmdBeginRendering(commandBuffer, pRenderingInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkDestroyShaderModule(VkDevice device, VkShaderModule shaderModule,
                                                                 const VkAllocationCallbacks* pAllocator) {
    device_dispatch_table(device)->DestroyShaderModule(device, shaderModule, pAllocator);
    vk_json::s_pipe.deleteShaderModuleInfo(shaderModule);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetCullModeEXT(VkCommandBuffer commandBuffer, VkCullModeFlags cullMode) {
    device_dispatch_table(commandBuffer)->CmdSetCullModeEXT(commandBuffer, cullMode);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdDecompressMemoryIndirectCountNV(VkCommandBuffer commandBuffer,
                                                                                VkDeviceAddress indirectCommandsAddress,
                                                                                VkDeviceAddress indirectCommandsCountAddress,
                                                                                uint32_t stride) {
    device_dispatch_table(commandBuffer)
        ->CmdDecompressMemoryIndirectCountNV(commandBuffer, indirectCommandsAddress, indirectCommandsCountAddress, stride);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetDeviceMaskKHR(VkCommandBuffer commandBuffer, uint32_t deviceMask) {
    device_dispatch_table(commandBuffer)->CmdSetDeviceMaskKHR(commandBuffer, deviceMask);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetDeviceGroupPeerMemoryFeaturesKHR(VkDevice device, uint32_t heapIndex,
                                                                                 uint32_t localDeviceIndex,
                                                                                 uint32_t remoteDeviceIndex,
                                                                                 VkPeerMemoryFeatureFlags* pPeerMemoryFeatures) {
    device_dispatch_table(device)->GetDeviceGroupPeerMemoryFeaturesKHR(device, heapIndex, localDeviceIndex, remoteDeviceIndex,
                                                                       pPeerMemoryFeatures);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetFrontFace(VkCommandBuffer commandBuffer, VkFrontFace frontFace) {
    device_dispatch_table(commandBuffer)->CmdSetFrontFace(commandBuffer, frontFace);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdEndRendering(VkCommandBuffer commandBuffer) {
    device_dispatch_table(commandBuffer)->CmdEndRendering(commandBuffer);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdEndQuery(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t query) {
    device_dispatch_table(commandBuffer)->CmdEndQuery(commandBuffer, queryPool, query);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetPrimitiveTopology(VkCommandBuffer commandBuffer,
                                                                     VkPrimitiveTopology primitiveTopology) {
    device_dispatch_table(commandBuffer)->CmdSetPrimitiveTopology(commandBuffer, primitiveTopology);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetCullMode(VkCommandBuffer commandBuffer, VkCullModeFlags cullMode) {
    device_dispatch_table(commandBuffer)->CmdSetCullMode(commandBuffer, cullMode);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdResetQueryPool(VkCommandBuffer commandBuffer, VkQueryPool queryPool,
                                                               uint32_t firstQuery, uint32_t queryCount) {
    device_dispatch_table(commandBuffer)->CmdResetQueryPool(commandBuffer, queryPool, firstQuery, queryCount);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdWriteTimestamp(VkCommandBuffer commandBuffer, VkPipelineStageFlagBits pipelineStage,
                                                               VkQueryPool queryPool, uint32_t query) {
    device_dispatch_table(commandBuffer)->CmdWriteTimestamp(commandBuffer, pipelineStage, queryPool, query);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetViewportWithCount(VkCommandBuffer commandBuffer, uint32_t viewportCount,
                                                                     const VkViewport* pViewports) {
    device_dispatch_table(commandBuffer)->CmdSetViewportWithCount(commandBuffer, viewportCount, pViewports);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetFrontFaceEXT(VkCommandBuffer commandBuffer, VkFrontFace frontFace) {
    device_dispatch_table(commandBuffer)->CmdSetFrontFaceEXT(commandBuffer, frontFace);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdDispatchBaseKHR(VkCommandBuffer commandBuffer, uint32_t baseGroupX,
                                                                uint32_t baseGroupY, uint32_t baseGroupZ, uint32_t groupCountX,
                                                                uint32_t groupCountY, uint32_t groupCountZ) {
    device_dispatch_table(commandBuffer)
        ->CmdDispatchBaseKHR(commandBuffer, baseGroupX, baseGroupY, baseGroupZ, groupCountX, groupCountY, groupCountZ);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdCopyQueryPoolResults(VkCommandBuffer commandBuffer, VkQueryPool queryPool,
                                                                     uint32_t firstQuery, uint32_t queryCount, VkBuffer dstBuffer,
                                                                     VkDeviceSize dstOffset, VkDeviceSize stride,
                                                                     VkQueryResultFlags flags) {
    device_dispatch_table(commandBuffer)
        ->CmdCopyQueryPoolResults(commandBuffer, queryPool, firstQuery, queryCount, dstBuffer, dstOffset, stride, flags);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetScissorWithCount(VkCommandBuffer commandBuffer, uint32_t scissorCount,
                                                                    const VkRect2D* pScissors) {
    device_dispatch_table(commandBuffer)->CmdSetScissorWithCount(commandBuffer, scissorCount, pScissors);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetPrimitiveTopologyEXT(VkCommandBuffer commandBuffer,
                                                                        VkPrimitiveTopology primitiveTopology) {
    device_dispatch_table(commandBuffer)->CmdSetPrimitiveTopologyEXT(commandBuffer, primitiveTopology);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkDestroyPipelineCache(VkDevice device, VkPipelineCache pipelineCache,
                                                                  const VkAllocationCallbacks* pAllocator) {
    device_dispatch_table(device)->DestroyPipelineCache(device, pipelineCache, pAllocator);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetDiscardRectangleEXT(VkCommandBuffer commandBuffer,
                                                                       uint32_t firstDiscardRectangle,
                                                                       uint32_t discardRectangleCount,
                                                                       const VkRect2D* pDiscardRectangles) {
    device_dispatch_table(commandBuffer)
        ->CmdSetDiscardRectangleEXT(commandBuffer, firstDiscardRectangle, discardRectangleCount, pDiscardRectangles);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetViewportWithCountEXT(VkCommandBuffer commandBuffer, uint32_t viewportCount,
                                                                        const VkViewport* pViewports) {
    device_dispatch_table(commandBuffer)->CmdSetViewportWithCountEXT(commandBuffer, viewportCount, pViewports);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetPipelineIndirectMemoryRequirementsNV(VkDevice device,
                                                                                     const VkComputePipelineCreateInfo* pCreateInfo,
                                                                                     VkMemoryRequirements2* pMemoryRequirements) {
    device_dispatch_table(device)->GetPipelineIndirectMemoryRequirementsNV(device, pCreateInfo, pMemoryRequirements);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdPushConstants(VkCommandBuffer commandBuffer, VkPipelineLayout layout,
                                                              VkShaderStageFlags stageFlags, uint32_t offset, uint32_t size,
                                                              const void* pValues) {
    device_dispatch_table(commandBuffer)->CmdPushConstants(commandBuffer, layout, stageFlags, offset, size, pValues);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdBindVertexBuffers2(VkCommandBuffer commandBuffer, uint32_t firstBinding,
                                                                   uint32_t bindingCount, const VkBuffer* pBuffers,
                                                                   const VkDeviceSize* pOffsets, const VkDeviceSize* pSizes,
                                                                   const VkDeviceSize* pStrides) {
    device_dispatch_table(commandBuffer)
        ->CmdBindVertexBuffers2(commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets, pSizes, pStrides);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetScissorWithCountEXT(VkCommandBuffer commandBuffer, uint32_t scissorCount,
                                                                       const VkRect2D* pScissors) {
    device_dispatch_table(commandBuffer)->CmdSetScissorWithCountEXT(commandBuffer, scissorCount, pScissors);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdUpdatePipelineIndirectBufferNV(VkCommandBuffer commandBuffer,
                                                                               VkPipelineBindPoint pipelineBindPoint,
                                                                               VkPipeline pipeline) {
    device_dispatch_table(commandBuffer)->CmdUpdatePipelineIndirectBufferNV(commandBuffer, pipelineBindPoint, pipeline);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetDepthTestEnable(VkCommandBuffer commandBuffer, VkBool32 depthTestEnable) {
    device_dispatch_table(commandBuffer)->CmdSetDepthTestEnable(commandBuffer, depthTestEnable);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdBindVertexBuffers2EXT(VkCommandBuffer commandBuffer, uint32_t firstBinding,
                                                                      uint32_t bindingCount, const VkBuffer* pBuffers,
                                                                      const VkDeviceSize* pOffsets, const VkDeviceSize* pSizes,
                                                                      const VkDeviceSize* pStrides) {
    device_dispatch_table(commandBuffer)
        ->CmdBindVertexBuffers2EXT(commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets, pSizes, pStrides);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdBeginRenderPass(VkCommandBuffer commandBuffer,
                                                                const VkRenderPassBeginInfo* pRenderPassBegin,
                                                                VkSubpassContents contents) {
    device_dispatch_table(commandBuffer)->CmdBeginRenderPass(commandBuffer, pRenderPassBegin, contents);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetDepthCompareOp(VkCommandBuffer commandBuffer, VkCompareOp depthCompareOp) {
    device_dispatch_table(commandBuffer)->CmdSetDepthCompareOp(commandBuffer, depthCompareOp);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetDepthWriteEnable(VkCommandBuffer commandBuffer, VkBool32 depthWriteEnable) {
    device_dispatch_table(commandBuffer)->CmdSetDepthWriteEnable(commandBuffer, depthWriteEnable);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetDepthWriteEnableEXT(VkCommandBuffer commandBuffer, VkBool32 depthWriteEnable) {
    device_dispatch_table(commandBuffer)->CmdSetDepthWriteEnableEXT(commandBuffer, depthWriteEnable);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdBindIndexBuffer2(VkCommandBuffer commandBuffer, VkBuffer buffer,
                                                                 VkDeviceSize offset, VkDeviceSize size, VkIndexType indexType) {
    device_dispatch_table(commandBuffer)->CmdBindIndexBuffer2(commandBuffer, buffer, offset, size, indexType);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetDepthTestEnableEXT(VkCommandBuffer commandBuffer, VkBool32 depthTestEnable) {
    device_dispatch_table(commandBuffer)->CmdSetDepthTestEnableEXT(commandBuffer, depthTestEnable);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkTrimCommandPoolKHR(VkDevice device, VkCommandPool commandPool,
                                                                VkCommandPoolTrimFlags flags) {
    device_dispatch_table(device)->TrimCommandPoolKHR(device, commandPool, flags);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetDepthCompareOpEXT(VkCommandBuffer commandBuffer, VkCompareOp depthCompareOp) {
    device_dispatch_table(commandBuffer)->CmdSetDepthCompareOpEXT(commandBuffer, depthCompareOp);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetDepthBoundsTestEnable(VkCommandBuffer commandBuffer,
                                                                         VkBool32 depthBoundsTestEnable) {
    device_dispatch_table(commandBuffer)->CmdSetDepthBoundsTestEnable(commandBuffer, depthBoundsTestEnable);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetRenderingAreaGranularity(VkDevice device,
                                                                         const VkRenderingAreaInfo* pRenderingAreaInfo,
                                                                         VkExtent2D* pGranularity) {
    device_dispatch_table(device)->GetRenderingAreaGranularity(device, pRenderingAreaInfo, pGranularity);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetDiscardRectangleEnableEXT(VkCommandBuffer commandBuffer,
                                                                             VkBool32 discardRectangleEnable) {
    device_dispatch_table(commandBuffer)->CmdSetDiscardRectangleEnableEXT(commandBuffer, discardRectangleEnable);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdNextSubpass(VkCommandBuffer commandBuffer, VkSubpassContents contents) {
    device_dispatch_table(commandBuffer)->CmdNextSubpass(commandBuffer, contents);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetStencilTestEnable(VkCommandBuffer commandBuffer, VkBool32 stencilTestEnable) {
    device_dispatch_table(commandBuffer)->CmdSetStencilTestEnable(commandBuffer, stencilTestEnable);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetStencilOp(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask,
                                                             VkStencilOp failOp, VkStencilOp passOp, VkStencilOp depthFailOp,
                                                             VkCompareOp compareOp) {
    device_dispatch_table(commandBuffer)->CmdSetStencilOp(commandBuffer, faceMask, failOp, passOp, depthFailOp, compareOp);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetDiscardRectangleModeEXT(VkCommandBuffer commandBuffer,
                                                                           VkDiscardRectangleModeEXT discardRectangleMode) {
    device_dispatch_table(commandBuffer)->CmdSetDiscardRectangleModeEXT(commandBuffer, discardRectangleMode);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdEndRenderPass(VkCommandBuffer commandBuffer) {
    device_dispatch_table(commandBuffer)->CmdEndRenderPass(commandBuffer);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetDepthBoundsTestEnableEXT(VkCommandBuffer commandBuffer,
                                                                            VkBool32 depthBoundsTestEnable) {
    device_dispatch_table(commandBuffer)->CmdSetDepthBoundsTestEnableEXT(commandBuffer, depthBoundsTestEnable);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetDeviceImageSubresourceLayout(VkDevice device,
                                                                             const VkDeviceImageSubresourceInfo* pInfo,
                                                                             VkSubresourceLayout2* pLayout) {
    device_dispatch_table(device)->GetDeviceImageSubresourceLayout(device, pInfo, pLayout);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdExecuteCommands(VkCommandBuffer commandBuffer, uint32_t commandBufferCount,
                                                                const VkCommandBuffer* pCommandBuffers) {
    device_dispatch_table(commandBuffer)->CmdExecuteCommands(commandBuffer, commandBufferCount, pCommandBuffers);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetStencilTestEnableEXT(VkCommandBuffer commandBuffer, VkBool32 stencilTestEnable) {
    device_dispatch_table(commandBuffer)->CmdSetStencilTestEnableEXT(commandBuffer, stencilTestEnable);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetDeviceQueue(VkDevice device, uint32_t queueFamilyIndex, uint32_t queueIndex,
                                                            VkQueue* pQueue) {
    device_dispatch_table(device)->GetDeviceQueue(device, queueFamilyIndex, queueIndex, pQueue);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetStencilOpEXT(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask,
                                                                VkStencilOp failOp, VkStencilOp passOp, VkStencilOp depthFailOp,
                                                                VkCompareOp compareOp) {
    device_dispatch_table(commandBuffer)->CmdSetStencilOpEXT(commandBuffer, faceMask, failOp, passOp, depthFailOp, compareOp);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetImageSubresourceLayout2(VkDevice device, VkImage image,
                                                                        const VkImageSubresource2* pSubresource,
                                                                        VkSubresourceLayout2* pLayout) {
    device_dispatch_table(device)->GetImageSubresourceLayout2(device, image, pSubresource, pLayout);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdPushDescriptorSet(VkCommandBuffer commandBuffer,
                                                                  VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout,
                                                                  uint32_t set, uint32_t descriptorWriteCount,
                                                                  const VkWriteDescriptorSet* pDescriptorWrites) {
    device_dispatch_table(commandBuffer)
        ->CmdPushDescriptorSet(commandBuffer, pipelineBindPoint, layout, set, descriptorWriteCount, pDescriptorWrites);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkDestroyDeferredOperationKHR(VkDevice device, VkDeferredOperationKHR operation,
                                                                         const VkAllocationCallbacks* pAllocator) {
    device_dispatch_table(device)->DestroyDeferredOperationKHR(device, operation, pAllocator);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdPushDescriptorSetWithTemplate(VkCommandBuffer commandBuffer,
                                                                              VkDescriptorUpdateTemplate descriptorUpdateTemplate,
                                                                              VkPipelineLayout layout, uint32_t set,
                                                                              const void* pData) {
    device_dispatch_table(commandBuffer)
        ->CmdPushDescriptorSetWithTemplate(commandBuffer, descriptorUpdateTemplate, layout, set, pData);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetShaderModuleIdentifierEXT(VkDevice device, VkShaderModule shaderModule,
                                                                          VkShaderModuleIdentifierEXT* pIdentifier) {
    device_dispatch_table(device)->GetShaderModuleIdentifierEXT(device, shaderModule, pIdentifier);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkDestroyQueryPool(VkDevice device, VkQueryPool queryPool,
                                                              const VkAllocationCallbacks* pAllocator) {
    device_dispatch_table(device)->DestroyQueryPool(device, queryPool, pAllocator);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetShaderModuleCreateInfoIdentifierEXT(VkDevice device,
                                                                                    const VkShaderModuleCreateInfo* pCreateInfo,
                                                                                    VkShaderModuleIdentifierEXT* pIdentifier) {
    device_dispatch_table(device)->GetShaderModuleCreateInfoIdentifierEXT(device, pCreateInfo, pIdentifier);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkResetQueryPool(VkDevice device, VkQueryPool queryPool, uint32_t firstQuery,
                                                            uint32_t queryCount) {
    device_dispatch_table(device)->ResetQueryPool(device, queryPool, firstQuery, queryCount);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL
vkCmdSetRenderingAttachmentLocations(VkCommandBuffer commandBuffer, const VkRenderingAttachmentLocationInfo* pLocationInfo) {
    device_dispatch_table(commandBuffer)->CmdSetRenderingAttachmentLocations(commandBuffer, pLocationInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkDestroyIndirectCommandsLayoutNV(VkDevice device,
                                                                             VkIndirectCommandsLayoutNV indirectCommandsLayout,
                                                                             const VkAllocationCallbacks* pAllocator) {
    device_dispatch_table(device)->DestroyIndirectCommandsLayoutNV(device, indirectCommandsLayout, pAllocator);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkFreeMemory(VkDevice device, VkDeviceMemory memory,
                                                        const VkAllocationCallbacks* pAllocator) {
    device_dispatch_table(device)->FreeMemory(device, memory, pAllocator);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkSetHdrMetadataEXT(VkDevice device, uint32_t swapchainCount,
                                                               const VkSwapchainKHR* pSwapchains,
                                                               const VkHdrMetadataEXT* pMetadata) {
    device_dispatch_table(device)->SetHdrMetadataEXT(device, swapchainCount, pSwapchains, pMetadata);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetRenderingInputAttachmentIndices(
    VkCommandBuffer commandBuffer, const VkRenderingInputAttachmentIndexInfo* pInputAttachmentIndexInfo) {
    device_dispatch_table(commandBuffer)->CmdSetRenderingInputAttachmentIndices(commandBuffer, pInputAttachmentIndexInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkUnmapMemory(VkDevice device, VkDeviceMemory memory) {
    device_dispatch_table(device)->UnmapMemory(device, memory);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL
vkCmdPushDescriptorSetWithTemplateKHR(VkCommandBuffer commandBuffer, VkDescriptorUpdateTemplate descriptorUpdateTemplate,
                                      VkPipelineLayout layout, uint32_t set, const void* pData) {
    device_dispatch_table(commandBuffer)
        ->CmdPushDescriptorSetWithTemplateKHR(commandBuffer, descriptorUpdateTemplate, layout, set, pData);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdPushDescriptorSetKHR(VkCommandBuffer commandBuffer,
                                                                     VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout,
                                                                     uint32_t set, uint32_t descriptorWriteCount,
                                                                     const VkWriteDescriptorSet* pDescriptorWrites) {
    device_dispatch_table(commandBuffer)
        ->CmdPushDescriptorSetKHR(commandBuffer, pipelineBindPoint, layout, set, descriptorWriteCount, pDescriptorWrites);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdNextSubpass2KHR(VkCommandBuffer commandBuffer,
                                                                const VkSubpassBeginInfo* pSubpassBeginInfo,
                                                                const VkSubpassEndInfo* pSubpassEndInfo) {
    device_dispatch_table(commandBuffer)->CmdNextSubpass2KHR(commandBuffer, pSubpassBeginInfo, pSubpassEndInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetDeviceMemoryCommitment(VkDevice device, VkDeviceMemory memory,
                                                                       VkDeviceSize* pCommittedMemoryInBytes) {
    device_dispatch_table(device)->GetDeviceMemoryCommitment(device, memory, pCommittedMemoryInBytes);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdBeginRenderPass2KHR(VkCommandBuffer commandBuffer,
                                                                    const VkRenderPassBeginInfo* pRenderPassBegin,
                                                                    const VkSubpassBeginInfo* pSubpassBeginInfo) {
    device_dispatch_table(commandBuffer)->CmdBeginRenderPass2KHR(commandBuffer, pRenderPassBegin, pSubpassBeginInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdEndRenderPass2KHR(VkCommandBuffer commandBuffer,
                                                                  const VkSubpassEndInfo* pSubpassEndInfo) {
    device_dispatch_table(commandBuffer)->CmdEndRenderPass2KHR(commandBuffer, pSubpassEndInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetDescriptorSetLayoutBindingOffsetEXT(VkDevice device, VkDescriptorSetLayout layout,
                                                                                    uint32_t binding, VkDeviceSize* pOffset) {
    device_dispatch_table(device)->GetDescriptorSetLayoutBindingOffsetEXT(device, layout, binding, pOffset);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdCopyBuffer2KHR(VkCommandBuffer commandBuffer,
                                                               const VkCopyBufferInfo2* pCopyBufferInfo) {
    device_dispatch_table(commandBuffer)->CmdCopyBuffer2KHR(commandBuffer, pCopyBufferInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetRayTracingPipelineStackSizeKHR(VkCommandBuffer commandBuffer,
                                                                                  uint32_t pipelineStackSize) {
    device_dispatch_table(commandBuffer)->CmdSetRayTracingPipelineStackSizeKHR(commandBuffer, pipelineStackSize);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdDrawMultiEXT(VkCommandBuffer commandBuffer, uint32_t drawCount,
                                                             const VkMultiDrawInfoEXT* pVertexInfo, uint32_t instanceCount,
                                                             uint32_t firstInstance, uint32_t stride) {
    device_dispatch_table(commandBuffer)
        ->CmdDrawMultiEXT(commandBuffer, drawCount, pVertexInfo, instanceCount, firstInstance, stride);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkDestroySwapchainKHR(VkDevice device, VkSwapchainKHR swapchain,
                                                                 const VkAllocationCallbacks* pAllocator) {
    device_dispatch_table(device)->DestroySwapchainKHR(device, swapchain, pAllocator);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkQueueBeginDebugUtilsLabelEXT(VkQueue queue, const VkDebugUtilsLabelEXT* pLabelInfo) {
    device_dispatch_table(queue)->QueueBeginDebugUtilsLabelEXT(queue, pLabelInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetDescriptorEXT(VkDevice device, const VkDescriptorGetInfoEXT* pDescriptorInfo,
                                                              size_t dataSize, void* pDescriptor) {
    device_dispatch_table(device)->GetDescriptorEXT(device, pDescriptorInfo, dataSize, pDescriptor);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdDrawMultiIndexedEXT(VkCommandBuffer commandBuffer, uint32_t drawCount,
                                                                    const VkMultiDrawIndexedInfoEXT* pIndexInfo,
                                                                    uint32_t instanceCount, uint32_t firstInstance, uint32_t stride,
                                                                    const int32_t* pVertexOffset) {
    device_dispatch_table(commandBuffer)
        ->CmdDrawMultiIndexedEXT(commandBuffer, drawCount, pIndexInfo, instanceCount, firstInstance, stride, pVertexOffset);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkQueueEndDebugUtilsLabelEXT(VkQueue queue) {
    device_dispatch_table(queue)->QueueEndDebugUtilsLabelEXT(queue);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdBindDescriptorSets2(VkCommandBuffer commandBuffer,
                                                                    const VkBindDescriptorSetsInfo* pBindDescriptorSetsInfo) {
    device_dispatch_table(commandBuffer)->CmdBindDescriptorSets2(commandBuffer, pBindDescriptorSetsInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdBindDescriptorBuffersEXT(VkCommandBuffer commandBuffer, uint32_t bufferCount,
                                                                         const VkDescriptorBufferBindingInfoEXT* pBindingInfos) {
    device_dispatch_table(commandBuffer)->CmdBindDescriptorBuffersEXT(commandBuffer, bufferCount, pBindingInfos);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetDescriptorBufferOffsetsEXT(VkCommandBuffer commandBuffer,
                                                                              VkPipelineBindPoint pipelineBindPoint,
                                                                              VkPipelineLayout layout, uint32_t firstSet,
                                                                              uint32_t setCount, const uint32_t* pBufferIndices,
                                                                              const VkDeviceSize* pOffsets) {
    device_dispatch_table(commandBuffer)
        ->CmdSetDescriptorBufferOffsetsEXT(commandBuffer, pipelineBindPoint, layout, firstSet, setCount, pBufferIndices, pOffsets);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkDestroyPipelineLayout(VkDevice device, VkPipelineLayout pipelineLayout,
                                                                   const VkAllocationCallbacks* pAllocator) {
    device_dispatch_table(device)->DestroyPipelineLayout(device, pipelineLayout, pAllocator);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdPushConstants2(VkCommandBuffer commandBuffer,
                                                               const VkPushConstantsInfo* pPushConstantsInfo) {
    device_dispatch_table(commandBuffer)->CmdPushConstants2(commandBuffer, pPushConstantsInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkQueueInsertDebugUtilsLabelEXT(VkQueue queue, const VkDebugUtilsLabelEXT* pLabelInfo) {
    device_dispatch_table(queue)->QueueInsertDebugUtilsLabelEXT(queue, pLabelInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdBeginDebugUtilsLabelEXT(VkCommandBuffer commandBuffer,
                                                                        const VkDebugUtilsLabelEXT* pLabelInfo) {
    device_dispatch_table(commandBuffer)->CmdBeginDebugUtilsLabelEXT(commandBuffer, pLabelInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdEndDebugUtilsLabelEXT(VkCommandBuffer commandBuffer) {
    device_dispatch_table(commandBuffer)->CmdEndDebugUtilsLabelEXT(commandBuffer);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdPushDescriptorSet2(VkCommandBuffer commandBuffer,
                                                                   const VkPushDescriptorSetInfo* pPushDescriptorSetInfo) {
    device_dispatch_table(commandBuffer)->CmdPushDescriptorSet2(commandBuffer, pPushDescriptorSetInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdCopyBufferToImage2KHR(VkCommandBuffer commandBuffer,
                                                                      const VkCopyBufferToImageInfo2* pCopyBufferToImageInfo) {
    device_dispatch_table(commandBuffer)->CmdCopyBufferToImage2KHR(commandBuffer, pCopyBufferToImageInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdInsertDebugUtilsLabelEXT(VkCommandBuffer commandBuffer,
                                                                         const VkDebugUtilsLabelEXT* pLabelInfo) {
    device_dispatch_table(commandBuffer)->CmdInsertDebugUtilsLabelEXT(commandBuffer, pLabelInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdWriteBufferMarkerAMD(VkCommandBuffer commandBuffer,
                                                                     VkPipelineStageFlagBits pipelineStage, VkBuffer dstBuffer,
                                                                     VkDeviceSize dstOffset, uint32_t marker) {
    device_dispatch_table(commandBuffer)->CmdWriteBufferMarkerAMD(commandBuffer, pipelineStage, dstBuffer, dstOffset, marker);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdBindDescriptorBufferEmbeddedSamplersEXT(VkCommandBuffer commandBuffer,
                                                                                        VkPipelineBindPoint pipelineBindPoint,
                                                                                        VkPipelineLayout layout, uint32_t set) {
    device_dispatch_table(commandBuffer)->CmdBindDescriptorBufferEmbeddedSamplersEXT(commandBuffer, pipelineBindPoint, layout, set);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdPushDescriptorSetWithTemplate2(
    VkCommandBuffer commandBuffer, const VkPushDescriptorSetWithTemplateInfo* pPushDescriptorSetWithTemplateInfo) {
    device_dispatch_table(commandBuffer)->CmdPushDescriptorSetWithTemplate2(commandBuffer, pPushDescriptorSetWithTemplateInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdCopyImage2KHR(VkCommandBuffer commandBuffer,
                                                              const VkCopyImageInfo2* pCopyImageInfo) {
    device_dispatch_table(commandBuffer)->CmdCopyImage2KHR(commandBuffer, pCopyImageInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdWriteBufferMarker2AMD(VkCommandBuffer commandBuffer, VkPipelineStageFlags2 stage,
                                                                      VkBuffer dstBuffer, VkDeviceSize dstOffset, uint32_t marker) {
    device_dispatch_table(commandBuffer)->CmdWriteBufferMarker2AMD(commandBuffer, stage, dstBuffer, dstOffset, marker);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdCopyImageToBuffer2KHR(VkCommandBuffer commandBuffer,
                                                                      const VkCopyImageToBufferInfo2* pCopyImageToBufferInfo) {
    device_dispatch_table(commandBuffer)->CmdCopyImageToBuffer2KHR(commandBuffer, pCopyImageToBufferInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdDecodeVideoKHR(VkCommandBuffer commandBuffer,
                                                               const VkVideoDecodeInfoKHR* pDecodeInfo) {
    device_dispatch_table(commandBuffer)->CmdDecodeVideoKHR(commandBuffer, pDecodeInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetImageSubresourceLayout2EXT(VkDevice device, VkImage image,
                                                                           const VkImageSubresource2* pSubresource,
                                                                           VkSubresourceLayout2* pLayout) {
    device_dispatch_table(device)->GetImageSubresourceLayout2EXT(device, image, pSubresource, pLayout);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdBlitImage2KHR(VkCommandBuffer commandBuffer,
                                                              const VkBlitImageInfo2* pBlitImageInfo) {
    device_dispatch_table(commandBuffer)->CmdBlitImage2KHR(commandBuffer, pBlitImageInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdResolveImage2KHR(VkCommandBuffer commandBuffer,
                                                                 const VkResolveImageInfo2* pResolveImageInfo) {
    device_dispatch_table(commandBuffer)->CmdResolveImage2KHR(commandBuffer, pResolveImageInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetDeviceGroupPeerMemoryFeatures(VkDevice device, uint32_t heapIndex,
                                                                              uint32_t localDeviceIndex, uint32_t remoteDeviceIndex,
                                                                              VkPeerMemoryFeatureFlags* pPeerMemoryFeatures) {
    device_dispatch_table(device)->GetDeviceGroupPeerMemoryFeatures(device, heapIndex, localDeviceIndex, remoteDeviceIndex,
                                                                    pPeerMemoryFeatures);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetDeviceMask(VkCommandBuffer commandBuffer, uint32_t deviceMask) {
    device_dispatch_table(commandBuffer)->CmdSetDeviceMask(commandBuffer, deviceMask);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetVertexInputEXT(
    VkCommandBuffer commandBuffer, uint32_t vertexBindingDescriptionCount,
    const VkVertexInputBindingDescription2EXT* pVertexBindingDescriptions, uint32_t vertexAttributeDescriptionCount,
    const VkVertexInputAttributeDescription2EXT* pVertexAttributeDescriptions) {
    device_dispatch_table(commandBuffer)
        ->CmdSetVertexInputEXT(commandBuffer, vertexBindingDescriptionCount, pVertexBindingDescriptions,
                               vertexAttributeDescriptionCount, pVertexAttributeDescriptions);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdDebugMarkerBeginEXT(VkCommandBuffer commandBuffer,
                                                                    const VkDebugMarkerMarkerInfoEXT* pMarkerInfo) {
    device_dispatch_table(commandBuffer)->CmdDebugMarkerBeginEXT(commandBuffer, pMarkerInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdBindShadingRateImageNV(VkCommandBuffer commandBuffer, VkImageView imageView,
                                                                       VkImageLayout imageLayout) {
    device_dispatch_table(commandBuffer)->CmdBindShadingRateImageNV(commandBuffer, imageView, imageLayout);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkDestroySamplerYcbcrConversion(VkDevice device,
                                                                           VkSamplerYcbcrConversion ycbcrConversion,
                                                                           const VkAllocationCallbacks* pAllocator) {
    device_dispatch_table(device)->DestroySamplerYcbcrConversion(device, ycbcrConversion, pAllocator);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdEncodeVideoKHR(VkCommandBuffer commandBuffer,
                                                               const VkVideoEncodeInfoKHR* pEncodeInfo) {
    device_dispatch_table(commandBuffer)->CmdEncodeVideoKHR(commandBuffer, pEncodeInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdDebugMarkerEndEXT(VkCommandBuffer commandBuffer) {
    device_dispatch_table(commandBuffer)->CmdDebugMarkerEndEXT(commandBuffer);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkUninitializePerformanceApiINTEL(VkDevice device) {
    device_dispatch_table(device)->UninitializePerformanceApiINTEL(device);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdDispatchBase(VkCommandBuffer commandBuffer, uint32_t baseGroupX,
                                                             uint32_t baseGroupY, uint32_t baseGroupZ, uint32_t groupCountX,
                                                             uint32_t groupCountY, uint32_t groupCountZ) {
    device_dispatch_table(commandBuffer)
        ->CmdDispatchBase(commandBuffer, baseGroupX, baseGroupY, baseGroupZ, groupCountX, groupCountY, groupCountZ);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdDebugMarkerInsertEXT(VkCommandBuffer commandBuffer,
                                                                     const VkDebugMarkerMarkerInfoEXT* pMarkerInfo) {
    device_dispatch_table(commandBuffer)->CmdDebugMarkerInsertEXT(commandBuffer, pMarkerInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL
vkCmdSetViewportShadingRatePaletteNV(VkCommandBuffer commandBuffer, uint32_t firstViewport, uint32_t viewportCount,
                                     const VkShadingRatePaletteNV* pShadingRatePalettes) {
    device_dispatch_table(commandBuffer)
        ->CmdSetViewportShadingRatePaletteNV(commandBuffer, firstViewport, viewportCount, pShadingRatePalettes);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetDepthBias2EXT(VkCommandBuffer commandBuffer,
                                                                 const VkDepthBiasInfoEXT* pDepthBiasInfo) {
    device_dispatch_table(commandBuffer)->CmdSetDepthBias2EXT(commandBuffer, pDepthBiasInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetCoarseSampleOrderNV(VkCommandBuffer commandBuffer,
                                                                       VkCoarseSampleOrderTypeNV sampleOrderType,
                                                                       uint32_t customSampleOrderCount,
                                                                       const VkCoarseSampleOrderCustomNV* pCustomSampleOrders) {
    device_dispatch_table(commandBuffer)
        ->CmdSetCoarseSampleOrderNV(commandBuffer, sampleOrderType, customSampleOrderCount, pCustomSampleOrders);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdBindTransformFeedbackBuffersEXT(VkCommandBuffer commandBuffer,
                                                                                uint32_t firstBinding, uint32_t bindingCount,
                                                                                const VkBuffer* pBuffers,
                                                                                const VkDeviceSize* pOffsets,
                                                                                const VkDeviceSize* pSizes) {
    device_dispatch_table(commandBuffer)
        ->CmdBindTransformFeedbackBuffersEXT(commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets, pSizes);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkDestroyOpticalFlowSessionNV(VkDevice device, VkOpticalFlowSessionNV session,
                                                                         const VkAllocationCallbacks* pAllocator) {
    device_dispatch_table(device)->DestroyOpticalFlowSessionNV(device, session, pAllocator);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetRasterizerDiscardEnable(VkCommandBuffer commandBuffer,
                                                                           VkBool32 rasterizerDiscardEnable) {
    device_dispatch_table(commandBuffer)->CmdSetRasterizerDiscardEnable(commandBuffer, rasterizerDiscardEnable);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkUpdateDescriptorSetWithTemplate(VkDevice device, VkDescriptorSet descriptorSet,
                                                                             VkDescriptorUpdateTemplate descriptorUpdateTemplate,
                                                                             const void* pData) {
    device_dispatch_table(device)->UpdateDescriptorSetWithTemplate(device, descriptorSet, descriptorUpdateTemplate, pData);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetSampleLocationsEXT(VkCommandBuffer commandBuffer,
                                                                      const VkSampleLocationsInfoEXT* pSampleLocationsInfo) {
    device_dispatch_table(commandBuffer)->CmdSetSampleLocationsEXT(commandBuffer, pSampleLocationsInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkDestroyDescriptorUpdateTemplate(VkDevice device,
                                                                             VkDescriptorUpdateTemplate descriptorUpdateTemplate,
                                                                             const VkAllocationCallbacks* pAllocator) {
    device_dispatch_table(device)->DestroyDescriptorUpdateTemplate(device, descriptorUpdateTemplate, pAllocator);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetPrimitiveRestartEnable(VkCommandBuffer commandBuffer,
                                                                          VkBool32 primitiveRestartEnable) {
    device_dispatch_table(commandBuffer)->CmdSetPrimitiveRestartEnable(commandBuffer, primitiveRestartEnable);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdBuildAccelerationStructuresKHR(
    VkCommandBuffer commandBuffer, uint32_t infoCount, const VkAccelerationStructureBuildGeometryInfoKHR* pInfos,
    const VkAccelerationStructureBuildRangeInfoKHR* const* ppBuildRangeInfos) {
    device_dispatch_table(commandBuffer)->CmdBuildAccelerationStructuresKHR(commandBuffer, infoCount, pInfos, ppBuildRangeInfos);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkDestroyFramebuffer(VkDevice device, VkFramebuffer framebuffer,
                                                                const VkAllocationCallbacks* pAllocator) {
    device_dispatch_table(device)->DestroyFramebuffer(device, framebuffer, pAllocator);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdOpticalFlowExecuteNV(VkCommandBuffer commandBuffer, VkOpticalFlowSessionNV session,
                                                                     const VkOpticalFlowExecuteInfoNV* pExecuteInfo) {
    device_dispatch_table(commandBuffer)->CmdOpticalFlowExecuteNV(commandBuffer, session, pExecuteInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetDepthBiasEnable(VkCommandBuffer commandBuffer, VkBool32 depthBiasEnable) {
    device_dispatch_table(commandBuffer)->CmdSetDepthBiasEnable(commandBuffer, depthBiasEnable);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdBeginTransformFeedbackEXT(VkCommandBuffer commandBuffer,
                                                                          uint32_t firstCounterBuffer, uint32_t counterBufferCount,
                                                                          const VkBuffer* pCounterBuffers,
                                                                          const VkDeviceSize* pCounterBufferOffsets) {
    device_dispatch_table(commandBuffer)
        ->CmdBeginTransformFeedbackEXT(commandBuffer, firstCounterBuffer, counterBufferCount, pCounterBuffers,
                                       pCounterBufferOffsets);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkDestroyAccelerationStructureKHR(VkDevice device,
                                                                             VkAccelerationStructureKHR accelerationStructure,
                                                                             const VkAllocationCallbacks* pAllocator) {
    device_dispatch_table(device)->DestroyAccelerationStructureKHR(device, accelerationStructure, pAllocator);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdBuildAccelerationStructuresIndirectKHR(
    VkCommandBuffer commandBuffer, uint32_t infoCount, const VkAccelerationStructureBuildGeometryInfoKHR* pInfos,
    const VkDeviceAddress* pIndirectDeviceAddresses, const uint32_t* pIndirectStrides,
    const uint32_t* const* ppMaxPrimitiveCounts) {
    device_dispatch_table(commandBuffer)
        ->CmdBuildAccelerationStructuresIndirectKHR(commandBuffer, infoCount, pInfos, pIndirectDeviceAddresses, pIndirectStrides,
                                                    ppMaxPrimitiveCounts);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetImageSparseMemoryRequirements2KHR(
    VkDevice device, const VkImageSparseMemoryRequirementsInfo2* pInfo, uint32_t* pSparseMemoryRequirementCount,
    VkSparseImageMemoryRequirements2* pSparseMemoryRequirements) {
    device_dispatch_table(device)->GetImageSparseMemoryRequirements2KHR(device, pInfo, pSparseMemoryRequirementCount,
                                                                        pSparseMemoryRequirements);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdEndTransformFeedbackEXT(VkCommandBuffer commandBuffer, uint32_t firstCounterBuffer,
                                                                        uint32_t counterBufferCount,
                                                                        const VkBuffer* pCounterBuffers,
                                                                        const VkDeviceSize* pCounterBufferOffsets) {
    device_dispatch_table(commandBuffer)
        ->CmdEndTransformFeedbackEXT(commandBuffer, firstCounterBuffer, counterBufferCount, pCounterBuffers, pCounterBufferOffsets);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkSetLocalDimmingAMD(VkDevice device, VkSwapchainKHR swapChain,
                                                                VkBool32 localDimmingEnable) {
    device_dispatch_table(device)->SetLocalDimmingAMD(device, swapChain, localDimmingEnable);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdBindIndexBuffer2KHR(VkCommandBuffer commandBuffer, VkBuffer buffer,
                                                                    VkDeviceSize offset, VkDeviceSize size, VkIndexType indexType) {
    device_dispatch_table(commandBuffer)->CmdBindIndexBuffer2KHR(commandBuffer, buffer, offset, size, indexType);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetImageMemoryRequirements2KHR(VkDevice device,
                                                                            const VkImageMemoryRequirementsInfo2* pInfo,
                                                                            VkMemoryRequirements2* pMemoryRequirements) {
    device_dispatch_table(device)->GetImageMemoryRequirements2KHR(device, pInfo, pMemoryRequirements);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkDestroyRenderPass(VkDevice device, VkRenderPass renderPass,
                                                               const VkAllocationCallbacks* pAllocator) {
    device_dispatch_table(device)->DestroyRenderPass(device, renderPass, pAllocator);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdBeginQueryIndexedEXT(VkCommandBuffer commandBuffer, VkQueryPool queryPool,
                                                                     uint32_t query, VkQueryControlFlags flags, uint32_t index) {
    device_dispatch_table(commandBuffer)->CmdBeginQueryIndexedEXT(commandBuffer, queryPool, query, flags, index);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetRenderingAreaGranularityKHR(VkDevice device,
                                                                            const VkRenderingAreaInfo* pRenderingAreaInfo,
                                                                            VkExtent2D* pGranularity) {
    device_dispatch_table(device)->GetRenderingAreaGranularityKHR(device, pRenderingAreaInfo, pGranularity);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetRenderAreaGranularity(VkDevice device, VkRenderPass renderPass,
                                                                      VkExtent2D* pGranularity) {
    device_dispatch_table(device)->GetRenderAreaGranularity(device, renderPass, pGranularity);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetBufferMemoryRequirements2KHR(VkDevice device,
                                                                             const VkBufferMemoryRequirementsInfo2* pInfo,
                                                                             VkMemoryRequirements2* pMemoryRequirements) {
    device_dispatch_table(device)->GetBufferMemoryRequirements2KHR(device, pInfo, pMemoryRequirements);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkDestroySampler(VkDevice device, VkSampler sampler,
                                                            const VkAllocationCallbacks* pAllocator) {
    device_dispatch_table(device)->DestroySampler(device, sampler, pAllocator);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetDeviceImageSubresourceLayoutKHR(VkDevice device,
                                                                                const VkDeviceImageSubresourceInfo* pInfo,
                                                                                VkSubresourceLayout2* pLayout) {
    device_dispatch_table(device)->GetDeviceImageSubresourceLayoutKHR(device, pInfo, pLayout);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdEndQueryIndexedEXT(VkCommandBuffer commandBuffer, VkQueryPool queryPool,
                                                                   uint32_t query, uint32_t index) {
    device_dispatch_table(commandBuffer)->CmdEndQueryIndexedEXT(commandBuffer, queryPool, query, index);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetImageMemoryRequirements2(VkDevice device,
                                                                         const VkImageMemoryRequirementsInfo2* pInfo,
                                                                         VkMemoryRequirements2* pMemoryRequirements) {
    device_dispatch_table(device)->GetImageMemoryRequirements2(device, pInfo, pMemoryRequirements);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetDeviceBufferMemoryRequirements(VkDevice device,
                                                                               const VkDeviceBufferMemoryRequirements* pInfo,
                                                                               VkMemoryRequirements2* pMemoryRequirements) {
    device_dispatch_table(device)->GetDeviceBufferMemoryRequirements(device, pInfo, pMemoryRequirements);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetBufferMemoryRequirements2(VkDevice device,
                                                                          const VkBufferMemoryRequirementsInfo2* pInfo,
                                                                          VkMemoryRequirements2* pMemoryRequirements) {
    device_dispatch_table(device)->GetBufferMemoryRequirements2(device, pInfo, pMemoryRequirements);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetImageSubresourceLayout2KHR(VkDevice device, VkImage image,
                                                                           const VkImageSubresource2* pSubresource,
                                                                           VkSubresourceLayout2* pLayout) {
    device_dispatch_table(device)->GetImageSubresourceLayout2KHR(device, image, pSubresource, pLayout);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetDeviceImageMemoryRequirements(VkDevice device,
                                                                              const VkDeviceImageMemoryRequirements* pInfo,
                                                                              VkMemoryRequirements2* pMemoryRequirements) {
    device_dispatch_table(device)->GetDeviceImageMemoryRequirements(device, pInfo, pMemoryRequirements);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdDrawIndirectByteCountEXT(VkCommandBuffer commandBuffer, uint32_t instanceCount,
                                                                         uint32_t firstInstance, VkBuffer counterBuffer,
                                                                         VkDeviceSize counterBufferOffset, uint32_t counterOffset,
                                                                         uint32_t vertexStride) {
    device_dispatch_table(commandBuffer)
        ->CmdDrawIndirectByteCountEXT(commandBuffer, instanceCount, firstInstance, counterBuffer, counterBufferOffset,
                                      counterOffset, vertexStride);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetImageSparseMemoryRequirements2(
    VkDevice device, const VkImageSparseMemoryRequirementsInfo2* pInfo, uint32_t* pSparseMemoryRequirementCount,
    VkSparseImageMemoryRequirements2* pSparseMemoryRequirements) {
    device_dispatch_table(device)->GetImageSparseMemoryRequirements2(device, pInfo, pSparseMemoryRequirementCount,
                                                                     pSparseMemoryRequirements);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetDeviceImageSparseMemoryRequirements(
    VkDevice device, const VkDeviceImageMemoryRequirements* pInfo, uint32_t* pSparseMemoryRequirementCount,
    VkSparseImageMemoryRequirements2* pSparseMemoryRequirements) {
    device_dispatch_table(device)->GetDeviceImageSparseMemoryRequirements(device, pInfo, pSparseMemoryRequirementCount,
                                                                          pSparseMemoryRequirements);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdCopyAccelerationStructureKHR(VkCommandBuffer commandBuffer,
                                                                             const VkCopyAccelerationStructureInfoKHR* pInfo) {
    device_dispatch_table(commandBuffer)->CmdCopyAccelerationStructureKHR(commandBuffer, pInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetFragmentShadingRateEnumNV(
    VkCommandBuffer commandBuffer, VkFragmentShadingRateNV shadingRate, const VkFragmentShadingRateCombinerOpKHR combinerOps[2]) {
    device_dispatch_table(commandBuffer)->CmdSetFragmentShadingRateEnumNV(commandBuffer, shadingRate, combinerOps);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL
vkCmdCopyAccelerationStructureToMemoryKHR(VkCommandBuffer commandBuffer, const VkCopyAccelerationStructureToMemoryInfoKHR* pInfo) {
    device_dispatch_table(commandBuffer)->CmdCopyAccelerationStructureToMemoryKHR(commandBuffer, pInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkDestroyCommandPool(VkDevice device, VkCommandPool commandPool,
                                                                const VkAllocationCallbacks* pAllocator) {
    device_dispatch_table(device)->DestroyCommandPool(device, commandPool, pAllocator);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkAntiLagUpdateAMD(VkDevice device, const VkAntiLagDataAMD* pData) {
    device_dispatch_table(device)->AntiLagUpdateAMD(device, pData);
}
#if defined(VK_ENABLE_BETA_EXTENSIONS)
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkDestroyCudaModuleNV(VkDevice device, VkCudaModuleNV module,
                                                                 const VkAllocationCallbacks* pAllocator) {
    device_dispatch_table(device)->DestroyCudaModuleNV(device, module, pAllocator);
}
#endif  // VK_ENABLE_BETA_EXTENSIONS
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSubpassShadingHUAWEI(VkCommandBuffer commandBuffer) {
    device_dispatch_table(commandBuffer)->CmdSubpassShadingHUAWEI(commandBuffer);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL
vkCmdCopyMemoryToAccelerationStructureKHR(VkCommandBuffer commandBuffer, const VkCopyMemoryToAccelerationStructureInfoKHR* pInfo) {
    device_dispatch_table(commandBuffer)->CmdCopyMemoryToAccelerationStructureKHR(commandBuffer, pInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdBindInvocationMaskHUAWEI(VkCommandBuffer commandBuffer, VkImageView imageView,
                                                                         VkImageLayout imageLayout) {
    device_dispatch_table(commandBuffer)->CmdBindInvocationMaskHUAWEI(commandBuffer, imageView, imageLayout);
}
#if defined(VK_ENABLE_BETA_EXTENSIONS)
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdCudaLaunchKernelNV(VkCommandBuffer commandBuffer,
                                                                   const VkCudaLaunchInfoNV* pLaunchInfo) {
    device_dispatch_table(commandBuffer)->CmdCudaLaunchKernelNV(commandBuffer, pLaunchInfo);
}
#endif  // VK_ENABLE_BETA_EXTENSIONS
#if defined(VK_ENABLE_BETA_EXTENSIONS)
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkDestroyCudaFunctionNV(VkDevice device, VkCudaFunctionNV function,
                                                                   const VkAllocationCallbacks* pAllocator) {
    device_dispatch_table(device)->DestroyCudaFunctionNV(device, function, pAllocator);
}
#endif  // VK_ENABLE_BETA_EXTENSIONS
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdWriteAccelerationStructuresPropertiesKHR(
    VkCommandBuffer commandBuffer, uint32_t accelerationStructureCount, const VkAccelerationStructureKHR* pAccelerationStructures,
    VkQueryType queryType, VkQueryPool queryPool, uint32_t firstQuery) {
    device_dispatch_table(commandBuffer)
        ->CmdWriteAccelerationStructuresPropertiesKHR(commandBuffer, accelerationStructureCount, pAccelerationStructures, queryType,
                                                      queryPool, firstQuery);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetClusterAccelerationStructureBuildSizesNV(
    VkDevice device, const VkClusterAccelerationStructureInputInfoNV* pInfo, VkAccelerationStructureBuildSizesInfoKHR* pSizeInfo) {
    device_dispatch_table(device)->GetClusterAccelerationStructureBuildSizesNV(device, pInfo, pSizeInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdBuildClusterAccelerationStructureIndirectNV(
    VkCommandBuffer commandBuffer, const VkClusterAccelerationStructureCommandsInfoNV* pCommandInfos) {
    device_dispatch_table(commandBuffer)->CmdBuildClusterAccelerationStructureIndirectNV(commandBuffer, pCommandInfos);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL
vkGetDeviceAccelerationStructureCompatibilityKHR(VkDevice device, const VkAccelerationStructureVersionInfoKHR* pVersionInfo,
                                                 VkAccelerationStructureCompatibilityKHR* pCompatibility) {
    device_dispatch_table(device)->GetDeviceAccelerationStructureCompatibilityKHR(device, pVersionInfo, pCompatibility);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetEvent2KHR(VkCommandBuffer commandBuffer, VkEvent event,
                                                             const VkDependencyInfo* pDependencyInfo) {
    device_dispatch_table(commandBuffer)->CmdSetEvent2KHR(commandBuffer, event, pDependencyInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetAccelerationStructureBuildSizesKHR(
    VkDevice device, VkAccelerationStructureBuildTypeKHR buildType, const VkAccelerationStructureBuildGeometryInfoKHR* pBuildInfo,
    const uint32_t* pMaxPrimitiveCounts, VkAccelerationStructureBuildSizesInfoKHR* pSizeInfo) {
    device_dispatch_table(device)->GetAccelerationStructureBuildSizesKHR(device, buildType, pBuildInfo, pMaxPrimitiveCounts,
                                                                         pSizeInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdResetEvent2KHR(VkCommandBuffer commandBuffer, VkEvent event,
                                                               VkPipelineStageFlags2 stageMask) {
    device_dispatch_table(commandBuffer)->CmdResetEvent2KHR(commandBuffer, event, stageMask);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdWaitEvents2KHR(VkCommandBuffer commandBuffer, uint32_t eventCount,
                                                               const VkEvent* pEvents, const VkDependencyInfo* pDependencyInfos) {
    device_dispatch_table(commandBuffer)->CmdWaitEvents2KHR(commandBuffer, eventCount, pEvents, pDependencyInfos);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdPipelineBarrier2KHR(VkCommandBuffer commandBuffer,
                                                                    const VkDependencyInfo* pDependencyInfo) {
    device_dispatch_table(commandBuffer)->CmdPipelineBarrier2KHR(commandBuffer, pDependencyInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkDestroyCuFunctionNVX(VkDevice device, VkCuFunctionNVX function,
                                                                  const VkAllocationCallbacks* pAllocator) {
    device_dispatch_table(device)->DestroyCuFunctionNVX(device, function, pAllocator);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkDestroyCuModuleNVX(VkDevice device, VkCuModuleNVX module,
                                                                const VkAllocationCallbacks* pAllocator) {
    device_dispatch_table(device)->DestroyCuModuleNVX(device, module, pAllocator);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdWriteTimestamp2KHR(VkCommandBuffer commandBuffer, VkPipelineStageFlags2 stage,
                                                                   VkQueryPool queryPool, uint32_t query) {
    device_dispatch_table(commandBuffer)->CmdWriteTimestamp2KHR(commandBuffer, stage, queryPool, query);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdCuLaunchKernelNVX(VkCommandBuffer commandBuffer,
                                                                  const VkCuLaunchInfoNVX* pLaunchInfo) {
    device_dispatch_table(commandBuffer)->CmdCuLaunchKernelNVX(commandBuffer, pLaunchInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkDestroyMicromapEXT(VkDevice device, VkMicromapEXT micromap,
                                                                const VkAllocationCallbacks* pAllocator) {
    device_dispatch_table(device)->DestroyMicromapEXT(device, micromap, pAllocator);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkDestroyPrivateDataSlotEXT(VkDevice device, VkPrivateDataSlot privateDataSlot,
                                                                       const VkAllocationCallbacks* pAllocator) {
    device_dispatch_table(device)->DestroyPrivateDataSlotEXT(device, privateDataSlot, pAllocator);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdBuildMicromapsEXT(VkCommandBuffer commandBuffer, uint32_t infoCount,
                                                                  const VkMicromapBuildInfoEXT* pInfos) {
    device_dispatch_table(commandBuffer)->CmdBuildMicromapsEXT(commandBuffer, infoCount, pInfos);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdDrawMeshTasksIndirectNV(VkCommandBuffer commandBuffer, VkBuffer buffer,
                                                                        VkDeviceSize offset, uint32_t drawCount, uint32_t stride) {
    device_dispatch_table(commandBuffer)->CmdDrawMeshTasksIndirectNV(commandBuffer, buffer, offset, drawCount, stride);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetFragmentShadingRateKHR(VkCommandBuffer commandBuffer,
                                                                          const VkExtent2D* pFragmentSize,
                                                                          const VkFragmentShadingRateCombinerOpKHR combinerOps[2]) {
    device_dispatch_table(commandBuffer)->CmdSetFragmentShadingRateKHR(commandBuffer, pFragmentSize, combinerOps);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkDestroyAccelerationStructureNV(VkDevice device,
                                                                            VkAccelerationStructureNV accelerationStructure,
                                                                            const VkAllocationCallbacks* pAllocator) {
    device_dispatch_table(device)->DestroyAccelerationStructureNV(device, accelerationStructure, pAllocator);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkFreeCommandBuffers(VkDevice device, VkCommandPool commandPool,
                                                                uint32_t commandBufferCount,
                                                                const VkCommandBuffer* pCommandBuffers) {
    device_dispatch_table(device)->FreeCommandBuffers(device, commandPool, commandBufferCount, pCommandBuffers);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetAccelerationStructureMemoryRequirementsNV(
    VkDevice device, const VkAccelerationStructureMemoryRequirementsInfoNV* pInfo, VkMemoryRequirements2KHR* pMemoryRequirements) {
    device_dispatch_table(device)->GetAccelerationStructureMemoryRequirementsNV(device, pInfo, pMemoryRequirements);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdDrawMeshTasksNV(VkCommandBuffer commandBuffer, uint32_t taskCount,
                                                                uint32_t firstTask) {
    device_dispatch_table(commandBuffer)->CmdDrawMeshTasksNV(commandBuffer, taskCount, firstTask);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetPrivateDataEXT(VkDevice device, VkObjectType objectType, uint64_t objectHandle,
                                                               VkPrivateDataSlot privateDataSlot, uint64_t* pData) {
    device_dispatch_table(device)->GetPrivateDataEXT(device, objectType, objectHandle, privateDataSlot, pData);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdDrawMeshTasksIndirectCountNV(VkCommandBuffer commandBuffer, VkBuffer buffer,
                                                                             VkDeviceSize offset, VkBuffer countBuffer,
                                                                             VkDeviceSize countBufferOffset, uint32_t maxDrawCount,
                                                                             uint32_t stride) {
    device_dispatch_table(commandBuffer)
        ->CmdDrawMeshTasksIndirectCountNV(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdDrawIndirectCountAMD(VkCommandBuffer commandBuffer, VkBuffer buffer,
                                                                     VkDeviceSize offset, VkBuffer countBuffer,
                                                                     VkDeviceSize countBufferOffset, uint32_t maxDrawCount,
                                                                     uint32_t stride) {
    device_dispatch_table(commandBuffer)
        ->CmdDrawIndirectCountAMD(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkDestroySamplerYcbcrConversionKHR(VkDevice device,
                                                                              VkSamplerYcbcrConversion ycbcrConversion,
                                                                              const VkAllocationCallbacks* pAllocator) {
    device_dispatch_table(device)->DestroySamplerYcbcrConversionKHR(device, ycbcrConversion, pAllocator);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdBuildAccelerationStructureNV(
    VkCommandBuffer commandBuffer, const VkAccelerationStructureInfoNV* pInfo, VkBuffer instanceData, VkDeviceSize instanceOffset,
    VkBool32 update, VkAccelerationStructureNV dst, VkAccelerationStructureNV src, VkBuffer scratch, VkDeviceSize scratchOffset) {
    device_dispatch_table(commandBuffer)
        ->CmdBuildAccelerationStructureNV(commandBuffer, pInfo, instanceData, instanceOffset, update, dst, src, scratch,
                                          scratchOffset);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkDestroyPrivateDataSlot(VkDevice device, VkPrivateDataSlot privateDataSlot,
                                                                    const VkAllocationCallbacks* pAllocator) {
    device_dispatch_table(device)->DestroyPrivateDataSlot(device, privateDataSlot, pAllocator);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdCopyAccelerationStructureNV(VkCommandBuffer commandBuffer,
                                                                            VkAccelerationStructureNV dst,
                                                                            VkAccelerationStructureNV src,
                                                                            VkCopyAccelerationStructureModeKHR mode) {
    device_dispatch_table(commandBuffer)->CmdCopyAccelerationStructureNV(commandBuffer, dst, src, mode);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdDrawIndexedIndirectCountAMD(VkCommandBuffer commandBuffer, VkBuffer buffer,
                                                                            VkDeviceSize offset, VkBuffer countBuffer,
                                                                            VkDeviceSize countBufferOffset, uint32_t maxDrawCount,
                                                                            uint32_t stride) {
    device_dispatch_table(commandBuffer)
        ->CmdDrawIndexedIndirectCountAMD(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdCopyMicromapEXT(VkCommandBuffer commandBuffer, const VkCopyMicromapInfoEXT* pInfo) {
    device_dispatch_table(commandBuffer)->CmdCopyMicromapEXT(commandBuffer, pInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkTrimCommandPool(VkDevice device, VkCommandPool commandPool,
                                                             VkCommandPoolTrimFlags flags) {
    device_dispatch_table(device)->TrimCommandPool(device, commandPool, flags);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdCopyMicromapToMemoryEXT(VkCommandBuffer commandBuffer,
                                                                        const VkCopyMicromapToMemoryInfoEXT* pInfo) {
    device_dispatch_table(commandBuffer)->CmdCopyMicromapToMemoryEXT(commandBuffer, pInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetPrivateData(VkDevice device, VkObjectType objectType, uint64_t objectHandle,
                                                            VkPrivateDataSlot privateDataSlot, uint64_t* pData) {
    device_dispatch_table(device)->GetPrivateData(device, objectType, objectHandle, privateDataSlot, pData);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL
vkCmdSetRenderingAttachmentLocationsKHR(VkCommandBuffer commandBuffer, const VkRenderingAttachmentLocationInfo* pLocationInfo) {
    device_dispatch_table(commandBuffer)->CmdSetRenderingAttachmentLocationsKHR(commandBuffer, pLocationInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL
vkCmdTraceRaysNV(VkCommandBuffer commandBuffer, VkBuffer raygenShaderBindingTableBuffer, VkDeviceSize raygenShaderBindingOffset,
                 VkBuffer missShaderBindingTableBuffer, VkDeviceSize missShaderBindingOffset, VkDeviceSize missShaderBindingStride,
                 VkBuffer hitShaderBindingTableBuffer, VkDeviceSize hitShaderBindingOffset, VkDeviceSize hitShaderBindingStride,
                 VkBuffer callableShaderBindingTableBuffer, VkDeviceSize callableShaderBindingOffset,
                 VkDeviceSize callableShaderBindingStride, uint32_t width, uint32_t height, uint32_t depth) {
    device_dispatch_table(commandBuffer)
        ->CmdTraceRaysNV(commandBuffer, raygenShaderBindingTableBuffer, raygenShaderBindingOffset, missShaderBindingTableBuffer,
                         missShaderBindingOffset, missShaderBindingStride, hitShaderBindingTableBuffer, hitShaderBindingOffset,
                         hitShaderBindingStride, callableShaderBindingTableBuffer, callableShaderBindingOffset,
                         callableShaderBindingStride, width, height, depth);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetRenderingInputAttachmentIndicesKHR(
    VkCommandBuffer commandBuffer, const VkRenderingInputAttachmentIndexInfo* pInputAttachmentIndexInfo) {
    device_dispatch_table(commandBuffer)->CmdSetRenderingInputAttachmentIndicesKHR(commandBuffer, pInputAttachmentIndexInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdCopyMemoryToMicromapEXT(VkCommandBuffer commandBuffer,
                                                                        const VkCopyMemoryToMicromapInfoEXT* pInfo) {
    device_dispatch_table(commandBuffer)->CmdCopyMemoryToMicromapEXT(commandBuffer, pInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetEvent2(VkCommandBuffer commandBuffer, VkEvent event,
                                                          const VkDependencyInfo* pDependencyInfo) {
    device_dispatch_table(commandBuffer)->CmdSetEvent2(commandBuffer, event, pDependencyInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdWriteMicromapsPropertiesEXT(VkCommandBuffer commandBuffer, uint32_t micromapCount,
                                                                            const VkMicromapEXT* pMicromaps, VkQueryType queryType,
                                                                            VkQueryPool queryPool, uint32_t firstQuery) {
    device_dispatch_table(commandBuffer)
        ->CmdWriteMicromapsPropertiesEXT(commandBuffer, micromapCount, pMicromaps, queryType, queryPool, firstQuery);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetPatchControlPointsEXT(VkCommandBuffer commandBuffer,
                                                                         uint32_t patchControlPoints) {
    device_dispatch_table(commandBuffer)->CmdSetPatchControlPointsEXT(commandBuffer, patchControlPoints);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetDeviceMicromapCompatibilityEXT(
    VkDevice device, const VkMicromapVersionInfoEXT* pVersionInfo, VkAccelerationStructureCompatibilityKHR* pCompatibility) {
    device_dispatch_table(device)->GetDeviceMicromapCompatibilityEXT(device, pVersionInfo, pCompatibility);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkDestroyShaderEXT(VkDevice device, VkShaderEXT shader,
                                                              const VkAllocationCallbacks* pAllocator) {
    device_dispatch_table(device)->DestroyShaderEXT(device, shader, pAllocator);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdResetEvent2(VkCommandBuffer commandBuffer, VkEvent event,
                                                            VkPipelineStageFlags2 stageMask) {
    device_dispatch_table(commandBuffer)->CmdResetEvent2(commandBuffer, event, stageMask);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetExclusiveScissorEnableNV(VkCommandBuffer commandBuffer,
                                                                            uint32_t firstExclusiveScissor,
                                                                            uint32_t exclusiveScissorCount,
                                                                            const VkBool32* pExclusiveScissorEnables) {
    device_dispatch_table(commandBuffer)
        ->CmdSetExclusiveScissorEnableNV(commandBuffer, firstExclusiveScissor, exclusiveScissorCount, pExclusiveScissorEnables);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetDepthBiasEnableEXT(VkCommandBuffer commandBuffer, VkBool32 depthBiasEnable) {
    device_dispatch_table(commandBuffer)->CmdSetDepthBiasEnableEXT(commandBuffer, depthBiasEnable);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetExclusiveScissorNV(VkCommandBuffer commandBuffer, uint32_t firstExclusiveScissor,
                                                                      uint32_t exclusiveScissorCount,
                                                                      const VkRect2D* pExclusiveScissors) {
    device_dispatch_table(commandBuffer)
        ->CmdSetExclusiveScissorNV(commandBuffer, firstExclusiveScissor, exclusiveScissorCount, pExclusiveScissors);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkDestroyDescriptorSetLayout(VkDevice device, VkDescriptorSetLayout descriptorSetLayout,
                                                                        const VkAllocationCallbacks* pAllocator) {
    device_dispatch_table(device)->DestroyDescriptorSetLayout(device, descriptorSetLayout, pAllocator);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdWaitEvents2(VkCommandBuffer commandBuffer, uint32_t eventCount,
                                                            const VkEvent* pEvents, const VkDependencyInfo* pDependencyInfos) {
    device_dispatch_table(commandBuffer)->CmdWaitEvents2(commandBuffer, eventCount, pEvents, pDependencyInfos);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetRasterizerDiscardEnableEXT(VkCommandBuffer commandBuffer,
                                                                              VkBool32 rasterizerDiscardEnable) {
    device_dispatch_table(commandBuffer)->CmdSetRasterizerDiscardEnableEXT(commandBuffer, rasterizerDiscardEnable);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetMicromapBuildSizesEXT(VkDevice device,
                                                                      VkAccelerationStructureBuildTypeKHR buildType,
                                                                      const VkMicromapBuildInfoEXT* pBuildInfo,
                                                                      VkMicromapBuildSizesInfoEXT* pSizeInfo) {
    device_dispatch_table(device)->GetMicromapBuildSizesEXT(device, buildType, pBuildInfo, pSizeInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdBindShadersEXT(VkCommandBuffer commandBuffer, uint32_t stageCount,
                                                               const VkShaderStageFlagBits* pStages, const VkShaderEXT* pShaders) {
    device_dispatch_table(commandBuffer)->CmdBindShadersEXT(commandBuffer, stageCount, pStages, pShaders);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdBuildPartitionedAccelerationStructuresNV(
    VkCommandBuffer commandBuffer, const VkBuildPartitionedAccelerationStructureInfoNV* pBuildInfo) {
    device_dispatch_table(commandBuffer)->CmdBuildPartitionedAccelerationStructuresNV(commandBuffer, pBuildInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdDrawIndirectCount(VkCommandBuffer commandBuffer, VkBuffer buffer,
                                                                  VkDeviceSize offset, VkBuffer countBuffer,
                                                                  VkDeviceSize countBufferOffset, uint32_t maxDrawCount,
                                                                  uint32_t stride) {
    device_dispatch_table(commandBuffer)
        ->CmdDrawIndirectCount(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetDepthClampRangeEXT(VkCommandBuffer commandBuffer,
                                                                      VkDepthClampModeEXT depthClampMode,
                                                                      const VkDepthClampRangeEXT* pDepthClampRange) {
    device_dispatch_table(commandBuffer)->CmdSetDepthClampRangeEXT(commandBuffer, depthClampMode, pDepthClampRange);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdPipelineBarrier2(VkCommandBuffer commandBuffer,
                                                                 const VkDependencyInfo* pDependencyInfo) {
    device_dispatch_table(commandBuffer)->CmdPipelineBarrier2(commandBuffer, pDependencyInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetLogicOpEXT(VkCommandBuffer commandBuffer, VkLogicOp logicOp) {
    device_dispatch_table(commandBuffer)->CmdSetLogicOpEXT(commandBuffer, logicOp);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL
vkGetPartitionedAccelerationStructuresBuildSizesNV(VkDevice device, const VkPartitionedAccelerationStructureInstancesInputNV* pInfo,
                                                   VkAccelerationStructureBuildSizesInfoKHR* pSizeInfo) {
    device_dispatch_table(device)->GetPartitionedAccelerationStructuresBuildSizesNV(device, pInfo, pSizeInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetPrimitiveRestartEnableEXT(VkCommandBuffer commandBuffer,
                                                                             VkBool32 primitiveRestartEnable) {
    device_dispatch_table(commandBuffer)->CmdSetPrimitiveRestartEnableEXT(commandBuffer, primitiveRestartEnable);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdDrawIndexedIndirectCount(VkCommandBuffer commandBuffer, VkBuffer buffer,
                                                                         VkDeviceSize offset, VkBuffer countBuffer,
                                                                         VkDeviceSize countBufferOffset, uint32_t maxDrawCount,
                                                                         uint32_t stride) {
    device_dispatch_table(commandBuffer)
        ->CmdDrawIndexedIndirectCount(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdWriteTimestamp2(VkCommandBuffer commandBuffer, VkPipelineStageFlags2 stage,
                                                                VkQueryPool queryPool, uint32_t query) {
    device_dispatch_table(commandBuffer)->CmdWriteTimestamp2(commandBuffer, stage, queryPool, query);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetCheckpointNV(VkCommandBuffer commandBuffer, const void* pCheckpointMarker) {
    device_dispatch_table(commandBuffer)->CmdSetCheckpointNV(commandBuffer, pCheckpointMarker);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkDestroyDescriptorPool(VkDevice device, VkDescriptorPool descriptorPool,
                                                                   const VkAllocationCallbacks* pAllocator) {
    device_dispatch_table(device)->DestroyDescriptorPool(device, descriptorPool, pAllocator);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetColorWriteEnableEXT(VkCommandBuffer commandBuffer, uint32_t attachmentCount,
                                                                       const VkBool32* pColorWriteEnables) {
    device_dispatch_table(commandBuffer)->CmdSetColorWriteEnableEXT(commandBuffer, attachmentCount, pColorWriteEnables);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetQueueCheckpointDataNV(VkQueue queue, uint32_t* pCheckpointDataCount,
                                                                      VkCheckpointDataNV* pCheckpointData) {
    device_dispatch_table(queue)->GetQueueCheckpointDataNV(queue, pCheckpointDataCount, pCheckpointData);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdWriteAccelerationStructuresPropertiesNV(
    VkCommandBuffer commandBuffer, uint32_t accelerationStructureCount, const VkAccelerationStructureNV* pAccelerationStructures,
    VkQueryType queryType, VkQueryPool queryPool, uint32_t firstQuery) {
    device_dispatch_table(commandBuffer)
        ->CmdWriteAccelerationStructuresPropertiesNV(commandBuffer, accelerationStructureCount, pAccelerationStructures, queryType,
                                                     queryPool, firstQuery);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetLineStipple(VkCommandBuffer commandBuffer, uint32_t lineStippleFactor,
                                                               uint16_t lineStipplePattern) {
    device_dispatch_table(commandBuffer)->CmdSetLineStipple(commandBuffer, lineStippleFactor, lineStipplePattern);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdDrawClusterHUAWEI(VkCommandBuffer commandBuffer, uint32_t groupCountX,
                                                                  uint32_t groupCountY, uint32_t groupCountZ) {
    device_dispatch_table(commandBuffer)->CmdDrawClusterHUAWEI(commandBuffer, groupCountX, groupCountY, groupCountZ);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetQueueCheckpointData2NV(VkQueue queue, uint32_t* pCheckpointDataCount,
                                                                       VkCheckpointData2NV* pCheckpointData) {
    device_dispatch_table(queue)->GetQueueCheckpointData2NV(queue, pCheckpointDataCount, pCheckpointData);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdDrawClusterIndirectHUAWEI(VkCommandBuffer commandBuffer, VkBuffer buffer,
                                                                          VkDeviceSize offset) {
    device_dispatch_table(commandBuffer)->CmdDrawClusterIndirectHUAWEI(commandBuffer, buffer, offset);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkUpdateDescriptorSets(VkDevice device, uint32_t descriptorWriteCount,
                                                                  const VkWriteDescriptorSet* pDescriptorWrites,
                                                                  uint32_t descriptorCopyCount,
                                                                  const VkCopyDescriptorSet* pDescriptorCopies) {
    device_dispatch_table(device)->UpdateDescriptorSets(device, descriptorWriteCount, pDescriptorWrites, descriptorCopyCount,
                                                        pDescriptorCopies);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdDrawMeshTasksIndirectCountEXT(VkCommandBuffer commandBuffer, VkBuffer buffer,
                                                                              VkDeviceSize offset, VkBuffer countBuffer,
                                                                              VkDeviceSize countBufferOffset, uint32_t maxDrawCount,
                                                                              uint32_t stride) {
    device_dispatch_table(commandBuffer)
        ->CmdDrawMeshTasksIndirectCountEXT(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkDestroyVideoSessionKHR(VkDevice device, VkVideoSessionKHR videoSession,
                                                                    const VkAllocationCallbacks* pAllocator) {
    device_dispatch_table(device)->DestroyVideoSessionKHR(device, videoSession, pAllocator);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkDestroyValidationCacheEXT(VkDevice device, VkValidationCacheEXT validationCache,
                                                                       const VkAllocationCallbacks* pAllocator) {
    device_dispatch_table(device)->DestroyValidationCacheEXT(device, validationCache, pAllocator);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdTraceRaysKHR(VkCommandBuffer commandBuffer,
                                                             const VkStridedDeviceAddressRegionKHR* pRaygenShaderBindingTable,
                                                             const VkStridedDeviceAddressRegionKHR* pMissShaderBindingTable,
                                                             const VkStridedDeviceAddressRegionKHR* pHitShaderBindingTable,
                                                             const VkStridedDeviceAddressRegionKHR* pCallableShaderBindingTable,
                                                             uint32_t width, uint32_t height, uint32_t depth) {
    device_dispatch_table(commandBuffer)
        ->CmdTraceRaysKHR(commandBuffer, pRaygenShaderBindingTable, pMissShaderBindingTable, pHitShaderBindingTable,
                          pCallableShaderBindingTable, width, height, depth);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdTraceRaysIndirect2KHR(VkCommandBuffer commandBuffer,
                                                                      VkDeviceAddress indirectDeviceAddress) {
    device_dispatch_table(commandBuffer)->CmdTraceRaysIndirect2KHR(commandBuffer, indirectDeviceAddress);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdDrawIndirectCountKHR(VkCommandBuffer commandBuffer, VkBuffer buffer,
                                                                     VkDeviceSize offset, VkBuffer countBuffer,
                                                                     VkDeviceSize countBufferOffset, uint32_t maxDrawCount,
                                                                     uint32_t stride) {
    device_dispatch_table(commandBuffer)
        ->CmdDrawIndirectCountKHR(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdBindPipeline(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint,
                                                             VkPipeline pipeline) {
    device_dispatch_table(commandBuffer)->CmdBindPipeline(commandBuffer, pipelineBindPoint, pipeline);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetDescriptorSetLayoutSupportKHR(VkDevice device,
                                                                              const VkDescriptorSetLayoutCreateInfo* pCreateInfo,
                                                                              VkDescriptorSetLayoutSupport* pSupport) {
    device_dispatch_table(device)->GetDescriptorSetLayoutSupportKHR(device, pCreateInfo, pSupport);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetDeviceImageSparseMemoryRequirementsKHR(
    VkDevice device, const VkDeviceImageMemoryRequirements* pInfo, uint32_t* pSparseMemoryRequirementCount,
    VkSparseImageMemoryRequirements2* pSparseMemoryRequirements) {
    device_dispatch_table(device)->GetDeviceImageSparseMemoryRequirementsKHR(device, pInfo, pSparseMemoryRequirementCount,
                                                                             pSparseMemoryRequirements);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkSetDeviceMemoryPriorityEXT(VkDevice device, VkDeviceMemory memory, float priority) {
    device_dispatch_table(device)->SetDeviceMemoryPriorityEXT(device, memory, priority);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetDeviceQueue2(VkDevice device, const VkDeviceQueueInfo2* pQueueInfo,
                                                             VkQueue* pQueue) {
    device_dispatch_table(device)->GetDeviceQueue2(device, pQueueInfo, pQueue);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetDeviceImageMemoryRequirementsKHR(VkDevice device,
                                                                                 const VkDeviceImageMemoryRequirements* pInfo,
                                                                                 VkMemoryRequirements2* pMemoryRequirements) {
    device_dispatch_table(device)->GetDeviceImageMemoryRequirementsKHR(device, pInfo, pMemoryRequirements);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdDrawMeshTasksEXT(VkCommandBuffer commandBuffer, uint32_t groupCountX,
                                                                 uint32_t groupCountY, uint32_t groupCountZ) {
    device_dispatch_table(commandBuffer)->CmdDrawMeshTasksEXT(commandBuffer, groupCountX, groupCountY, groupCountZ);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetDeviceBufferMemoryRequirementsKHR(VkDevice device,
                                                                                  const VkDeviceBufferMemoryRequirements* pInfo,
                                                                                  VkMemoryRequirements2* pMemoryRequirements) {
    device_dispatch_table(device)->GetDeviceBufferMemoryRequirementsKHR(device, pInfo, pMemoryRequirements);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetViewport(VkCommandBuffer commandBuffer, uint32_t firstViewport,
                                                            uint32_t viewportCount, const VkViewport* pViewports) {
    device_dispatch_table(commandBuffer)->CmdSetViewport(commandBuffer, firstViewport, viewportCount, pViewports);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetGeneratedCommandsMemoryRequirementsNV(
    VkDevice device, const VkGeneratedCommandsMemoryRequirementsInfoNV* pInfo, VkMemoryRequirements2* pMemoryRequirements) {
    device_dispatch_table(device)->GetGeneratedCommandsMemoryRequirementsNV(device, pInfo, pMemoryRequirements);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdDrawMeshTasksIndirectEXT(VkCommandBuffer commandBuffer, VkBuffer buffer,
                                                                         VkDeviceSize offset, uint32_t drawCount, uint32_t stride) {
    device_dispatch_table(commandBuffer)->CmdDrawMeshTasksIndirectEXT(commandBuffer, buffer, offset, drawCount, stride);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdDrawIndexedIndirectCountKHR(VkCommandBuffer commandBuffer, VkBuffer buffer,
                                                                            VkDeviceSize offset, VkBuffer countBuffer,
                                                                            VkDeviceSize countBufferOffset, uint32_t maxDrawCount,
                                                                            uint32_t stride) {
    device_dispatch_table(commandBuffer)
        ->CmdDrawIndexedIndirectCountKHR(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetScissor(VkCommandBuffer commandBuffer, uint32_t firstScissor,
                                                           uint32_t scissorCount, const VkRect2D* pScissors) {
    device_dispatch_table(commandBuffer)->CmdSetScissor(commandBuffer, firstScissor, scissorCount, pScissors);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdTraceRaysIndirectKHR(
    VkCommandBuffer commandBuffer, const VkStridedDeviceAddressRegionKHR* pRaygenShaderBindingTable,
    const VkStridedDeviceAddressRegionKHR* pMissShaderBindingTable, const VkStridedDeviceAddressRegionKHR* pHitShaderBindingTable,
    const VkStridedDeviceAddressRegionKHR* pCallableShaderBindingTable, VkDeviceAddress indirectDeviceAddress) {
    device_dispatch_table(commandBuffer)
        ->CmdTraceRaysIndirectKHR(commandBuffer, pRaygenShaderBindingTable, pMissShaderBindingTable, pHitShaderBindingTable,
                                  pCallableShaderBindingTable, indirectDeviceAddress);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkDestroyVideoSessionParametersKHR(VkDevice device,
                                                                              VkVideoSessionParametersKHR videoSessionParameters,
                                                                              const VkAllocationCallbacks* pAllocator) {
    device_dispatch_table(device)->DestroyVideoSessionParametersKHR(device, videoSessionParameters, pAllocator);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL
vkCmdPreprocessGeneratedCommandsNV(VkCommandBuffer commandBuffer, const VkGeneratedCommandsInfoNV* pGeneratedCommandsInfo) {
    device_dispatch_table(commandBuffer)->CmdPreprocessGeneratedCommandsNV(commandBuffer, pGeneratedCommandsInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetLineWidth(VkCommandBuffer commandBuffer, float lineWidth) {
    device_dispatch_table(commandBuffer)->CmdSetLineWidth(commandBuffer, lineWidth);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdExecuteGeneratedCommandsNV(
    VkCommandBuffer commandBuffer, VkBool32 isPreprocessed, const VkGeneratedCommandsInfoNV* pGeneratedCommandsInfo) {
    device_dispatch_table(commandBuffer)->CmdExecuteGeneratedCommandsNV(commandBuffer, isPreprocessed, pGeneratedCommandsInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetDepthBias(VkCommandBuffer commandBuffer, float depthBiasConstantFactor,
                                                             float depthBiasClamp, float depthBiasSlopeFactor) {
    device_dispatch_table(commandBuffer)
        ->CmdSetDepthBias(commandBuffer, depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetDescriptorSetLayoutSizeEXT(VkDevice device, VkDescriptorSetLayout layout,
                                                                           VkDeviceSize* pLayoutSizeInBytes) {
    device_dispatch_table(device)->GetDescriptorSetLayoutSizeEXT(device, layout, pLayoutSizeInBytes);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkGetDescriptorSetLayoutSupport(VkDevice device,
                                                                           const VkDescriptorSetLayoutCreateInfo* pCreateInfo,
                                                                           VkDescriptorSetLayoutSupport* pSupport) {
    device_dispatch_table(device)->GetDescriptorSetLayoutSupport(device, pCreateInfo, pSupport);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdEndVideoCodingKHR(VkCommandBuffer commandBuffer,
                                                                  const VkVideoEndCodingInfoKHR* pEndCodingInfo) {
    device_dispatch_table(commandBuffer)->CmdEndVideoCodingKHR(commandBuffer, pEndCodingInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdBindPipelineShaderGroupNV(VkCommandBuffer commandBuffer,
                                                                          VkPipelineBindPoint pipelineBindPoint,
                                                                          VkPipeline pipeline, uint32_t groupIndex) {
    device_dispatch_table(commandBuffer)->CmdBindPipelineShaderGroupNV(commandBuffer, pipelineBindPoint, pipeline, groupIndex);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkDestroyPipeline(VkDevice device, VkPipeline pipeline,
                                                             const VkAllocationCallbacks* pAllocator) {
    device_dispatch_table(device)->DestroyPipeline(device, pipeline, pAllocator);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdBeginVideoCodingKHR(VkCommandBuffer commandBuffer,
                                                                    const VkVideoBeginCodingInfoKHR* pBeginInfo) {
    device_dispatch_table(commandBuffer)->CmdBeginVideoCodingKHR(commandBuffer, pBeginInfo);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdSetBlendConstants(VkCommandBuffer commandBuffer, const float blendConstants[4]) {
    device_dispatch_table(commandBuffer)->CmdSetBlendConstants(commandBuffer, blendConstants);
}
EXPORT_FUNCTION VKAPI_ATTR void VKAPI_CALL vkCmdControlVideoCodingKHR(VkCommandBuffer commandBuffer,
                                                                      const VkVideoCodingControlInfoKHR* pCodingControlInfo) {
    device_dispatch_table(commandBuffer)->CmdControlVideoCodingKHR(commandBuffer, pCodingControlInfo);
}

EXPORT_FUNCTION VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vkGetInstanceProcAddr(VkInstance instance, const char* pName) {
    if (strcmp(pName, "vkGetPhysicalDeviceExternalFencePropertiesKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceExternalFencePropertiesKHR);
    if (strcmp(pName, "vkReleaseDisplayEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkReleaseDisplayEXT);
    if (strcmp(pName, "vkGetPhysicalDeviceCooperativeMatrixPropertiesKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceCooperativeMatrixPropertiesKHR);
    if (strcmp(pName, "vkGetPhysicalDeviceSurfaceCapabilities2EXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceSurfaceCapabilities2EXT);
    if (strcmp(pName, "vkCreateInstance") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCreateInstance);
    if (strcmp(pName, "vkEnumeratePhysicalDevices") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkEnumeratePhysicalDevices);
    if (strcmp(pName, "vkGetPhysicalDeviceCooperativeMatrixPropertiesNV") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceCooperativeMatrixPropertiesNV);
    if (strcmp(pName, "vkDestroyInstance") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkDestroyInstance);
    if (strcmp(pName, "vkGetPhysicalDeviceFeatures") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceFeatures);
    if (strcmp(pName, "vkGetPhysicalDeviceImageFormatProperties") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceImageFormatProperties);
    if (strcmp(pName, "vkGetPhysicalDeviceSparseImageFormatProperties") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceSparseImageFormatProperties);
    if (strcmp(pName, "vkGetPhysicalDeviceFormatProperties") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceFormatProperties);
    if (strcmp(pName, "vkGetPhysicalDeviceProperties") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceProperties);
    if (strcmp(pName, "vkGetPhysicalDeviceQueueFamilyProperties") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceQueueFamilyProperties);
    if (strcmp(pName, "vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV);
    if (strcmp(pName, "vkGetPhysicalDeviceMemoryProperties") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceMemoryProperties);
    if (strcmp(pName, "vkGetInstanceProcAddr") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkGetInstanceProcAddr);
    if (strcmp(pName, "vkCreateDevice") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCreateDevice);
    if (strcmp(pName, "vkGetPhysicalDeviceCooperativeVectorPropertiesNV") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceCooperativeVectorPropertiesNV);
    if (strcmp(pName, "vkGetPhysicalDeviceExternalImageFormatPropertiesNV") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceExternalImageFormatPropertiesNV);
    if (strcmp(pName, "vkCreateHeadlessSurfaceEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCreateHeadlessSurfaceEXT);
    if (strcmp(pName, "vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR);
    if (strcmp(pName, "vkGetPhysicalDeviceFeatures2KHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceFeatures2KHR);
    if (strcmp(pName, "vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR);
    if (strcmp(pName, "vkGetPhysicalDeviceProperties2KHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceProperties2KHR);
    if (strcmp(pName, "vkGetPhysicalDeviceFormatProperties2KHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceFormatProperties2KHR);
    if (strcmp(pName, "vkGetPhysicalDeviceImageFormatProperties2KHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceImageFormatProperties2KHR);
    if (strcmp(pName, "vkEnumerateDeviceLayerProperties") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkEnumerateDeviceLayerProperties);
    if (strcmp(pName, "vkGetPhysicalDeviceQueueFamilyProperties2KHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceQueueFamilyProperties2KHR);
    if (strcmp(pName, "vkGetPhysicalDeviceMemoryProperties2KHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceMemoryProperties2KHR);
    if (strcmp(pName, "vkGetPhysicalDeviceSparseImageFormatProperties2KHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceSparseImageFormatProperties2KHR);
    if (strcmp(pName, "vkGetPhysicalDeviceSurfaceCapabilities2KHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceSurfaceCapabilities2KHR);
    if (strcmp(pName, "vkGetPhysicalDeviceSurfaceFormats2KHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceSurfaceFormats2KHR);
    if (strcmp(pName, "vkGetPhysicalDeviceDisplayProperties2KHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceDisplayProperties2KHR);
    if (strcmp(pName, "vkEnumeratePhysicalDeviceGroupsKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkEnumeratePhysicalDeviceGroupsKHR);
    if (strcmp(pName, "vkGetPhysicalDeviceDisplayPlaneProperties2KHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceDisplayPlaneProperties2KHR);
    if (strcmp(pName, "vkGetPhysicalDeviceExternalBufferPropertiesKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceExternalBufferPropertiesKHR);
    if (strcmp(pName, "vkGetDisplayModeProperties2KHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetDisplayModeProperties2KHR);
    if (strcmp(pName, "vkGetDisplayPlaneCapabilities2KHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetDisplayPlaneCapabilities2KHR);
    if (strcmp(pName, "vkGetPhysicalDeviceExternalSemaphorePropertiesKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceExternalSemaphorePropertiesKHR);
    if (strcmp(pName, "vkGetPhysicalDeviceCooperativeMatrixFlexibleDimensionsPropertiesNV") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceCooperativeMatrixFlexibleDimensionsPropertiesNV);
    if (strcmp(pName, "vkSetDebugUtilsObjectNameEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkSetDebugUtilsObjectNameEXT);
    if (strcmp(pName, "vkSetDebugUtilsObjectTagEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkSetDebugUtilsObjectTagEXT);
    if (strcmp(pName, "vkQueueBeginDebugUtilsLabelEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkQueueBeginDebugUtilsLabelEXT);
    if (strcmp(pName, "vkQueueEndDebugUtilsLabelEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkQueueEndDebugUtilsLabelEXT);
    if (strcmp(pName, "vkQueueInsertDebugUtilsLabelEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkQueueInsertDebugUtilsLabelEXT);
    if (strcmp(pName, "vkCmdBeginDebugUtilsLabelEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdBeginDebugUtilsLabelEXT);
    if (strcmp(pName, "vkCmdEndDebugUtilsLabelEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdEndDebugUtilsLabelEXT);
    if (strcmp(pName, "vkCmdInsertDebugUtilsLabelEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdInsertDebugUtilsLabelEXT);
    if (strcmp(pName, "vkCreateDebugUtilsMessengerEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCreateDebugUtilsMessengerEXT);
    if (strcmp(pName, "vkDestroyDebugUtilsMessengerEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkDestroyDebugUtilsMessengerEXT);
    if (strcmp(pName, "vkSubmitDebugUtilsMessageEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkSubmitDebugUtilsMessageEXT);
    if (strcmp(pName, "vkGetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR);
    if (strcmp(pName, "vkGetPhysicalDeviceCalibrateableTimeDomainsKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceCalibrateableTimeDomainsKHR);
    if (strcmp(pName, "vkGetPhysicalDeviceOpticalFlowImageFormatsNV") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceOpticalFlowImageFormatsNV);
    if (strcmp(pName, "vkGetPhysicalDeviceCalibrateableTimeDomainsEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceCalibrateableTimeDomainsEXT);
    if (strcmp(pName, "vkEnumeratePhysicalDeviceGroups") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkEnumeratePhysicalDeviceGroups);
    if (strcmp(pName, "vkGetPhysicalDevicePresentRectanglesKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDevicePresentRectanglesKHR);
    if (strcmp(pName, "vkGetPhysicalDeviceMultisamplePropertiesEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceMultisamplePropertiesEXT);
    if (strcmp(pName, "vkAcquireDrmDisplayEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkAcquireDrmDisplayEXT);
    if (strcmp(pName, "vkGetDrmDisplayEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkGetDrmDisplayEXT);
    if (strcmp(pName, "vkGetPhysicalDeviceDisplayPropertiesKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceDisplayPropertiesKHR);
    if (strcmp(pName, "vkGetPhysicalDeviceDisplayPlanePropertiesKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceDisplayPlanePropertiesKHR);
    if (strcmp(pName, "vkGetPhysicalDeviceToolProperties") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceToolProperties);
    if (strcmp(pName, "vkGetDisplayPlaneSupportedDisplaysKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetDisplayPlaneSupportedDisplaysKHR);
    if (strcmp(pName, "vkGetDisplayModePropertiesKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetDisplayModePropertiesKHR);
    if (strcmp(pName, "vkGetDisplayPlaneCapabilitiesKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetDisplayPlaneCapabilitiesKHR);
    if (strcmp(pName, "vkGetPhysicalDeviceExternalBufferProperties") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceExternalBufferProperties);
    if (strcmp(pName, "vkCreateDisplayModeKHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCreateDisplayModeKHR);
    if (strcmp(pName, "vkGetPhysicalDeviceFeatures2") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceFeatures2);
    if (strcmp(pName, "vkCreateDisplayPlaneSurfaceKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCreateDisplayPlaneSurfaceKHR);
    if (strcmp(pName, "vkGetPhysicalDeviceProperties2") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceProperties2);
    if (strcmp(pName, "vkGetPhysicalDeviceFormatProperties2") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceFormatProperties2);
    if (strcmp(pName, "vkGetPhysicalDeviceFragmentShadingRatesKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceFragmentShadingRatesKHR);
    if (strcmp(pName, "vkGetPhysicalDeviceImageFormatProperties2") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceImageFormatProperties2);
    if (strcmp(pName, "vkGetPhysicalDeviceQueueFamilyProperties2") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceQueueFamilyProperties2);
    if (strcmp(pName, "vkGetPhysicalDeviceSparseImageFormatProperties2") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceSparseImageFormatProperties2);
    if (strcmp(pName, "vkGetPhysicalDeviceMemoryProperties2") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceMemoryProperties2);
    if (strcmp(pName, "vkGetPhysicalDeviceExternalFenceProperties") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceExternalFenceProperties);
    if (strcmp(pName, "vkGetPhysicalDeviceSurfaceSupportKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceSurfaceSupportKHR);
    if (strcmp(pName, "vkGetPhysicalDeviceSurfaceFormatsKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceSurfaceFormatsKHR);
    if (strcmp(pName, "vkDestroySurfaceKHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkDestroySurfaceKHR);
    if (strcmp(pName, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
    if (strcmp(pName, "vkGetPhysicalDeviceVideoCapabilitiesKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceVideoCapabilitiesKHR);
    if (strcmp(pName, "vkGetPhysicalDeviceSurfacePresentModesKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceSurfacePresentModesKHR);
    if (strcmp(pName, "vkGetPhysicalDeviceVideoFormatPropertiesKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceVideoFormatPropertiesKHR);
    if (strcmp(pName, "vkGetPhysicalDeviceToolPropertiesEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceToolPropertiesEXT);
    if (strcmp(pName, "vkGetPhysicalDeviceExternalSemaphoreProperties") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceExternalSemaphoreProperties);
    if (strcmp(pName, "vkCreateDebugReportCallbackEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCreateDebugReportCallbackEXT);
    if (strcmp(pName, "vkDestroyDebugReportCallbackEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkDestroyDebugReportCallbackEXT);
    if (strcmp(pName, "vkDebugReportMessageEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkDebugReportMessageEXT);

    if (instance_dispatch_table(instance)->GetInstanceProcAddr == NULL) return NULL;
    return instance_dispatch_table(instance)->GetInstanceProcAddr(instance, pName);
}

EXPORT_FUNCTION VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vkGetDeviceProcAddr(VkDevice device, const char* pName) {
    if (strcmp(pName, "vkWaitForPresentKHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkWaitForPresentKHR);
    if (strcmp(pName, "vkCmdSetDepthBounds") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetDepthBounds);
    if (strcmp(pName, "vkCreateRenderPass2") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCreateRenderPass2);
    if (strcmp(pName, "vkGetBufferMemoryRequirements") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetBufferMemoryRequirements);
    if (strcmp(pName, "vkCmdBeginRenderPass2") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdBeginRenderPass2);
    if (strcmp(pName, "vkMapMemory2") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkMapMemory2);
    if (strcmp(pName, "vkCmdSetStencilCompareMask") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetStencilCompareMask);
    if (strcmp(pName, "vkGetImageMemoryRequirements") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetImageMemoryRequirements);
    if (strcmp(pName, "vkCmdEndConditionalRenderingEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdEndConditionalRenderingEXT);
    if (strcmp(pName, "vkGetSwapchainStatusKHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkGetSwapchainStatusKHR);
    if (strcmp(pName, "vkCmdBeginConditionalRenderingEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdBeginConditionalRenderingEXT);
    if (strcmp(pName, "vkCmdSetStencilWriteMask") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetStencilWriteMask);
    if (strcmp(pName, "vkCmdNextSubpass2") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdNextSubpass2);
    if (strcmp(pName, "vkSetLatencySleepModeNV") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkSetLatencySleepModeNV);
    if (strcmp(pName, "vkUnmapMemory2") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkUnmapMemory2);
    if (strcmp(pName, "vkCreateDescriptorUpdateTemplateKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCreateDescriptorUpdateTemplateKHR);
    if (strcmp(pName, "vkCreatePipelineBinariesKHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCreatePipelineBinariesKHR);
    if (strcmp(pName, "vkCmdEndRenderPass2") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdEndRenderPass2);
    if (strcmp(pName, "vkCmdSetStencilReference") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetStencilReference);
    if (strcmp(pName, "vkGetPipelineBinaryDataKHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkGetPipelineBinaryDataKHR);
    if (strcmp(pName, "vkLatencySleepNV") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkLatencySleepNV);
    if (strcmp(pName, "vkCmdBindDescriptorSets") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdBindDescriptorSets);
    if (strcmp(pName, "vkUpdateDescriptorSetWithTemplateKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkUpdateDescriptorSetWithTemplateKHR);
    if (strcmp(pName, "vkSetLatencyMarkerNV") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkSetLatencyMarkerNV);
    if (strcmp(pName, "vkCmdBindIndexBuffer") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdBindIndexBuffer);
    if (strcmp(pName, "vkDestroyPipelineBinaryKHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkDestroyPipelineBinaryKHR);
    if (strcmp(pName, "vkGetLatencyTimingsNV") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkGetLatencyTimingsNV);
    if (strcmp(pName, "vkGetPipelineKeyKHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkGetPipelineKeyKHR);
    if (strcmp(pName, "vkCreateBuffer") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCreateBuffer);
    if (strcmp(pName, "vkQueueNotifyOutOfBandNV") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkQueueNotifyOutOfBandNV);
    if (strcmp(pName, "vkImportFenceFdKHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkImportFenceFdKHR);
    if (strcmp(pName, "vkDestroyDescriptorUpdateTemplateKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkDestroyDescriptorUpdateTemplateKHR);
    if (strcmp(pName, "vkReleaseCapturedPipelineDataKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkReleaseCapturedPipelineDataKHR);
    if (strcmp(pName, "vkCmdDraw") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdDraw);
    if (strcmp(pName, "vkCmdCopyBuffer2") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdCopyBuffer2);
    if (strcmp(pName, "vkGetFenceFdKHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkGetFenceFdKHR);
    if (strcmp(pName, "vkCmdBindVertexBuffers") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdBindVertexBuffers);
    if (strcmp(pName, "vkDestroyBuffer") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkDestroyBuffer);
    if (strcmp(pName, "vkCreateBufferView") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCreateBufferView);
    if (strcmp(pName, "vkCmdDrawIndexed") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdDrawIndexed);
    if (strcmp(pName, "vkGetDescriptorSetLayoutHostMappingInfoVALVE") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetDescriptorSetLayoutHostMappingInfoVALVE);
    if (strcmp(pName, "vkCmdSetViewportWScalingNV") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetViewportWScalingNV);
    if (strcmp(pName, "vkCmdSetDepthClampEnableEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetDepthClampEnableEXT);
    if (strcmp(pName, "vkGetDescriptorSetHostMappingVALVE") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetDescriptorSetHostMappingVALVE);
    if (strcmp(pName, "vkCmdSetRasterizationSamplesEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetRasterizationSamplesEXT);
    if (strcmp(pName, "vkCmdDrawIndirect") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdDrawIndirect);
    if (strcmp(pName, "vkGetFramebufferTilePropertiesQCOM") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetFramebufferTilePropertiesQCOM);
    if (strcmp(pName, "vkCmdSetPolygonModeEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetPolygonModeEXT);
    if (strcmp(pName, "vkCmdDispatchIndirect") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdDispatchIndirect);
    if (strcmp(pName, "vkCmdSetAttachmentFeedbackLoopEnableEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetAttachmentFeedbackLoopEnableEXT);
    if (strcmp(pName, "vkCmdDrawIndexedIndirect") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdDrawIndexedIndirect);
    if (strcmp(pName, "vkCmdCopyImage2") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdCopyImage2);
    if (strcmp(pName, "vkCmdSetSampleMaskEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetSampleMaskEXT);
    if (strcmp(pName, "vkCmdCopyBufferToImage2") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdCopyBufferToImage2);
    if (strcmp(pName, "vkCmdSetAlphaToCoverageEnableEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetAlphaToCoverageEnableEXT);
    if (strcmp(pName, "vkDestroyBufferView") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkDestroyBufferView);
    if (strcmp(pName, "vkCmdDispatch") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdDispatch);
    if (strcmp(pName, "vkCmdSetLogicOpEnableEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetLogicOpEnableEXT);
    if (strcmp(pName, "vkGetDynamicRenderingTilePropertiesQCOM") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetDynamicRenderingTilePropertiesQCOM);
    if (strcmp(pName, "vkCmdCopyImageToBuffer2") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdCopyImageToBuffer2);
    if (strcmp(pName, "vkCmdBlitImage2") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdBlitImage2);
    if (strcmp(pName, "vkCmdSetAlphaToOneEnableEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetAlphaToOneEnableEXT);
    if (strcmp(pName, "vkCmdResolveImage2") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdResolveImage2);
    if (strcmp(pName, "vkCmdSetLineStippleKHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetLineStippleKHR);
    if (strcmp(pName, "vkCmdSetColorBlendEnableEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetColorBlendEnableEXT);
    if (strcmp(pName, "vkCmdSetColorBlendEquationEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetColorBlendEquationEXT);
    if (strcmp(pName, "vkGetImageSparseMemoryRequirements") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetImageSparseMemoryRequirements);
    if (strcmp(pName, "vkCmdCopyImage") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdCopyImage);
    if (strcmp(pName, "vkCmdCopyBuffer") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdCopyBuffer);
    if (strcmp(pName, "vkCmdBindDescriptorSets2KHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdBindDescriptorSets2KHR);
    if (strcmp(pName, "vkCmdPushDescriptorSet2KHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdPushDescriptorSet2KHR);
    if (strcmp(pName, "vkCmdBlitImage") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdBlitImage);
    if (strcmp(pName, "vkCmdSetColorWriteMaskEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetColorWriteMaskEXT);
    if (strcmp(pName, "vkCmdPushConstants2KHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdPushConstants2KHR);
    if (strcmp(pName, "vkCmdSetTessellationDomainOriginEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetTessellationDomainOriginEXT);
    if (strcmp(pName, "vkCreateImage") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCreateImage);
    if (strcmp(pName, "vkCmdPushDescriptorSetWithTemplate2KHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdPushDescriptorSetWithTemplate2KHR);
    if (strcmp(pName, "vkQueueBindSparse") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkQueueBindSparse);
    if (strcmp(pName, "vkGetShaderInfoAMD") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkGetShaderInfoAMD);
    if (strcmp(pName, "vkCmdSetRasterizationStreamEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetRasterizationStreamEXT);
    if (strcmp(pName, "vkGetImageSubresourceLayout") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkGetImageSubresourceLayout);
    if (strcmp(pName, "vkCmdSetConservativeRasterizationModeEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetConservativeRasterizationModeEXT);
    if (strcmp(pName, "vkCmdCopyBufferToImage") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdCopyBufferToImage);
    if (strcmp(pName, "vkCmdSetExtraPrimitiveOverestimationSizeEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetExtraPrimitiveOverestimationSizeEXT);
    if (strcmp(pName, "vkDestroyImage") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkDestroyImage);
    if (strcmp(pName, "vkCmdSetDepthClipEnableEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetDepthClipEnableEXT);
    if (strcmp(pName, "vkCreateFence") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCreateFence);
    if (strcmp(pName, "vkCmdBeginRenderingKHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdBeginRenderingKHR);
    if (strcmp(pName, "vkCmdSetSampleLocationsEnableEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetSampleLocationsEnableEXT);
    if (strcmp(pName, "vkCmdSetDescriptorBufferOffsets2EXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetDescriptorBufferOffsets2EXT);
    if (strcmp(pName, "vkCmdCopyImageToBuffer") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdCopyImageToBuffer);
    if (strcmp(pName, "vkGetDeviceProcAddr") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkGetDeviceProcAddr);
    if (strcmp(pName, "vkCmdSetColorBlendAdvancedEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetColorBlendAdvancedEXT);
    if (strcmp(pName, "vkCmdSetProvokingVertexModeEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetProvokingVertexModeEXT);
    if (strcmp(pName, "vkDestroyFence") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkDestroyFence);
    if (strcmp(pName, "vkCmdEndRenderingKHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdEndRenderingKHR);
    if (strcmp(pName, "vkCmdBindDescriptorBufferEmbeddedSamplers2EXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdBindDescriptorBufferEmbeddedSamplers2EXT);
    if (strcmp(pName, "vkCmdUpdateBuffer") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdUpdateBuffer);
    if (strcmp(pName, "vkDisplayPowerControlEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkDisplayPowerControlEXT);
    if (strcmp(pName, "vkCmdSetLineRasterizationModeEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetLineRasterizationModeEXT);
    if (strcmp(pName, "vkCmdSetLineStippleEnableEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetLineStippleEnableEXT);
    if (strcmp(pName, "vkResetFences") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkResetFences);
    if (strcmp(pName, "vkRegisterDeviceEventEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkRegisterDeviceEventEXT);
    if (strcmp(pName, "vkCmdSetDepthClipNegativeOneToOneEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetDepthClipNegativeOneToOneEXT);
    if (strcmp(pName, "vkCmdFillBuffer") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdFillBuffer);
    if (strcmp(pName, "vkGetGeneratedCommandsMemoryRequirementsEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetGeneratedCommandsMemoryRequirementsEXT);
    if (strcmp(pName, "vkGetFenceStatus") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkGetFenceStatus);
    if (strcmp(pName, "vkCmdSetViewportWScalingEnableNV") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetViewportWScalingEnableNV);
    if (strcmp(pName, "vkCmdSetViewportSwizzleNV") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetViewportSwizzleNV);
    if (strcmp(pName, "vkCmdClearColorImage") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdClearColorImage);
    if (strcmp(pName, "vkCmdSetCoverageToColorEnableNV") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetCoverageToColorEnableNV);
    if (strcmp(pName, "vkGetSwapchainCounterEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkGetSwapchainCounterEXT);
    if (strcmp(pName, "vkWaitForFences") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkWaitForFences);
    if (strcmp(pName, "vkRegisterDisplayEventEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkRegisterDisplayEventEXT);
    if (strcmp(pName, "vkCmdPreprocessGeneratedCommandsEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdPreprocessGeneratedCommandsEXT);
    if (strcmp(pName, "vkCmdSetCoverageToColorLocationNV") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetCoverageToColorLocationNV);
    if (strcmp(pName, "vkCmdResolveImage") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdResolveImage);
    if (strcmp(pName, "vkCreateSemaphore") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCreateSemaphore);
    if (strcmp(pName, "vkCmdSetCoverageModulationModeNV") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetCoverageModulationModeNV);
    if (strcmp(pName, "vkCmdClearDepthStencilImage") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdClearDepthStencilImage);
    if (strcmp(pName, "vkCmdExecuteGeneratedCommandsEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdExecuteGeneratedCommandsEXT);
    if (strcmp(pName, "vkCmdClearAttachments") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdClearAttachments);
    if (strcmp(pName, "vkCreateImageView") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCreateImageView);
    if (strcmp(pName, "vkCmdSetCoverageModulationTableEnableNV") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetCoverageModulationTableEnableNV);
    if (strcmp(pName, "vkCreateIndirectCommandsLayoutEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCreateIndirectCommandsLayoutEXT);
    if (strcmp(pName, "vkCmdCopyMemoryIndirectNV") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdCopyMemoryIndirectNV);
    if (strcmp(pName, "vkDestroySemaphore") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkDestroySemaphore);
    if (strcmp(pName, "vkDestroyIndirectExecutionSetEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkDestroyIndirectExecutionSetEXT);
    if (strcmp(pName, "vkCmdSetCoverageModulationTableNV") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetCoverageModulationTableNV);
    if (strcmp(pName, "vkDestroyIndirectCommandsLayoutEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkDestroyIndirectCommandsLayoutEXT);
    if (strcmp(pName, "vkCmdSetShadingRateImageEnableNV") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetShadingRateImageEnableNV);
    if (strcmp(pName, "vkCmdCopyMemoryToImageIndirectNV") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdCopyMemoryToImageIndirectNV);
    if (strcmp(pName, "vkCreateIndirectExecutionSetEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCreateIndirectExecutionSetEXT);
    if (strcmp(pName, "vkCmdSetCoverageReductionModeNV") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetCoverageReductionModeNV);
    if (strcmp(pName, "vkDestroyDevice") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkDestroyDevice);
    if (strcmp(pName, "vkGetRefreshCycleDurationGOOGLE") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetRefreshCycleDurationGOOGLE);
    if (strcmp(pName, "vkCmdSetRepresentativeFragmentTestEnableNV") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetRepresentativeFragmentTestEnableNV);
    if (strcmp(pName, "vkGetBufferDeviceAddressKHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkGetBufferDeviceAddressKHR);
    if (strcmp(pName, "vkUpdateIndirectExecutionSetShaderEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkUpdateIndirectExecutionSetShaderEXT);
    if (strcmp(pName, "vkCreateEvent") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCreateEvent);
    if (strcmp(pName, "vkUpdateIndirectExecutionSetPipelineEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkUpdateIndirectExecutionSetPipelineEXT);
    if (strcmp(pName, "vkEnumerateInstanceExtensionProperties") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkEnumerateInstanceExtensionProperties);
    if (strcmp(pName, "vkCmdSetEvent") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetEvent);
    if (strcmp(pName, "vkDestroyEvent") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkDestroyEvent);
    if (strcmp(pName, "vkConvertCooperativeVectorMatrixNV") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkConvertCooperativeVectorMatrixNV);
    if (strcmp(pName, "vkGetPastPresentationTimingGOOGLE") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPastPresentationTimingGOOGLE);
    if (strcmp(pName, "vkSetEvent") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkSetEvent);
    if (strcmp(pName, "vkCmdResetEvent") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdResetEvent);
    if (strcmp(pName, "vkGetEventStatus") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkGetEventStatus);
    if (strcmp(pName, "vkDestroyImageView") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkDestroyImageView);
    if (strcmp(pName, "vkCmdConvertCooperativeVectorMatrixNV") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdConvertCooperativeVectorMatrixNV);
    if (strcmp(pName, "vkGetBufferOpaqueCaptureAddressKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetBufferOpaqueCaptureAddressKHR);
    if (strcmp(pName, "vkAcquireProfilingLockKHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkAcquireProfilingLockKHR);
    if (strcmp(pName, "vkCmdWaitEvents") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdWaitEvents);
    if (strcmp(pName, "vkGetDeviceMemoryOpaqueCaptureAddressKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetDeviceMemoryOpaqueCaptureAddressKHR);
    if (strcmp(pName, "vkCmdSetLineStippleEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetLineStippleEXT);
    if (strcmp(pName, "vkEnumerateInstanceLayerProperties") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkEnumerateInstanceLayerProperties);
    if (strcmp(pName, "vkResetEvent") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkResetEvent);
    if (strcmp(pName, "vkCreateShaderModule") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCreateShaderModule);
    if (strcmp(pName, "vkCmdPipelineBarrier") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdPipelineBarrier);
    if (strcmp(pName, "vkCmdDecompressMemoryNV") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdDecompressMemoryNV);
    if (strcmp(pName, "vkCmdBeginQuery") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdBeginQuery);
    if (strcmp(pName, "vkReleaseProfilingLockKHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkReleaseProfilingLockKHR);
    if (strcmp(pName, "vkResetQueryPoolEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkResetQueryPoolEXT);
    if (strcmp(pName, "vkCmdBeginRendering") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdBeginRendering);
    if (strcmp(pName, "vkDestroyShaderModule") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkDestroyShaderModule);
    if (strcmp(pName, "vkCmdSetCullModeEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetCullModeEXT);
    if (strcmp(pName, "vkCmdDecompressMemoryIndirectCountNV") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdDecompressMemoryIndirectCountNV);
    if (strcmp(pName, "vkCmdSetDeviceMaskKHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetDeviceMaskKHR);
    if (strcmp(pName, "vkGetDeviceGroupPeerMemoryFeaturesKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetDeviceGroupPeerMemoryFeaturesKHR);
    if (strcmp(pName, "vkCmdSetFrontFace") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetFrontFace);
    if (strcmp(pName, "vkCmdEndRendering") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdEndRendering);
    if (strcmp(pName, "vkCmdEndQuery") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdEndQuery);
    if (strcmp(pName, "vkCmdSetPrimitiveTopology") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetPrimitiveTopology);
    if (strcmp(pName, "vkCmdSetCullMode") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetCullMode);
    if (strcmp(pName, "vkCreatePipelineCache") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCreatePipelineCache);
    if (strcmp(pName, "vkCmdResetQueryPool") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdResetQueryPool);
    if (strcmp(pName, "vkCmdWriteTimestamp") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdWriteTimestamp);
    if (strcmp(pName, "vkCmdSetViewportWithCount") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetViewportWithCount);
    if (strcmp(pName, "vkCmdSetFrontFaceEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetFrontFaceEXT);
    if (strcmp(pName, "vkCmdDispatchBaseKHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdDispatchBaseKHR);
    if (strcmp(pName, "vkCmdCopyQueryPoolResults") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdCopyQueryPoolResults);
    if (strcmp(pName, "vkCmdSetScissorWithCount") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetScissorWithCount);
    if (strcmp(pName, "vkCmdSetPrimitiveTopologyEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetPrimitiveTopologyEXT);
    if (strcmp(pName, "vkDestroyPipelineCache") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkDestroyPipelineCache);
    if (strcmp(pName, "vkCmdSetDiscardRectangleEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetDiscardRectangleEXT);
    if (strcmp(pName, "vkGetPipelineIndirectDeviceAddressNV") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPipelineIndirectDeviceAddressNV);
    if (strcmp(pName, "vkCmdSetViewportWithCountEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetViewportWithCountEXT);
    if (strcmp(pName, "vkGetPipelineIndirectMemoryRequirementsNV") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPipelineIndirectMemoryRequirementsNV);
    if (strcmp(pName, "vkGetPipelineCacheData") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkGetPipelineCacheData);
    if (strcmp(pName, "vkCmdPushConstants") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdPushConstants);
    if (strcmp(pName, "vkCmdBindVertexBuffers2") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdBindVertexBuffers2);
    if (strcmp(pName, "vkCmdSetScissorWithCountEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetScissorWithCountEXT);
    if (strcmp(pName, "vkCmdUpdatePipelineIndirectBufferNV") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdUpdatePipelineIndirectBufferNV);
    if (strcmp(pName, "vkCmdSetDepthTestEnable") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetDepthTestEnable);
    if (strcmp(pName, "vkMergePipelineCaches") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkMergePipelineCaches);
    if (strcmp(pName, "vkCmdBindVertexBuffers2EXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdBindVertexBuffers2EXT);
    if (strcmp(pName, "vkCmdBeginRenderPass") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdBeginRenderPass);
    if (strcmp(pName, "vkCmdSetDepthCompareOp") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetDepthCompareOp);
    if (strcmp(pName, "vkCmdSetDepthWriteEnable") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetDepthWriteEnable);
    if (strcmp(pName, "vkCmdSetDepthWriteEnableEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetDepthWriteEnableEXT);
    if (strcmp(pName, "vkCmdBindIndexBuffer2") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdBindIndexBuffer2);
    if (strcmp(pName, "vkCmdSetDepthTestEnableEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetDepthTestEnableEXT);
    if (strcmp(pName, "vkTrimCommandPoolKHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkTrimCommandPoolKHR);
    if (strcmp(pName, "vkCmdSetDepthCompareOpEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetDepthCompareOpEXT);
    if (strcmp(pName, "vkCmdSetDepthBoundsTestEnable") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetDepthBoundsTestEnable);
    if (strcmp(pName, "vkGetRenderingAreaGranularity") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetRenderingAreaGranularity);
    if (strcmp(pName, "vkCmdSetDiscardRectangleEnableEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetDiscardRectangleEnableEXT);
    if (strcmp(pName, "vkCmdNextSubpass") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdNextSubpass);
    if (strcmp(pName, "vkCmdSetStencilTestEnable") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetStencilTestEnable);
    if (strcmp(pName, "vkCmdSetStencilOp") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetStencilOp);
    if (strcmp(pName, "vkCmdSetDiscardRectangleModeEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetDiscardRectangleModeEXT);
    if (strcmp(pName, "vkCmdEndRenderPass") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdEndRenderPass);
    if (strcmp(pName, "vkCmdSetDepthBoundsTestEnableEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetDepthBoundsTestEnableEXT);
    if (strcmp(pName, "vkGetDeviceImageSubresourceLayout") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetDeviceImageSubresourceLayout);
    if (strcmp(pName, "vkCmdExecuteCommands") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdExecuteCommands);
    if (strcmp(pName, "vkCmdSetStencilTestEnableEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetStencilTestEnableEXT);
    if (strcmp(pName, "vkGetDeviceQueue") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkGetDeviceQueue);
    if (strcmp(pName, "vkCmdSetStencilOpEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetStencilOpEXT);
    if (strcmp(pName, "vkGetImageSubresourceLayout2") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetImageSubresourceLayout2);
    if (strcmp(pName, "vkDeferredOperationJoinKHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkDeferredOperationJoinKHR);
    if (strcmp(pName, "vkCreateDeferredOperationKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCreateDeferredOperationKHR);
    if (strcmp(pName, "vkCmdPushDescriptorSet") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdPushDescriptorSet);
    if (strcmp(pName, "vkQueueSubmit") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkQueueSubmit);
    if (strcmp(pName, "vkQueueWaitIdle") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkQueueWaitIdle);
    if (strcmp(pName, "vkDestroyDeferredOperationKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkDestroyDeferredOperationKHR);
    if (strcmp(pName, "vkCmdPushDescriptorSetWithTemplate") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdPushDescriptorSetWithTemplate);
    if (strcmp(pName, "vkGetDeferredOperationMaxConcurrencyKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetDeferredOperationMaxConcurrencyKHR);
    if (strcmp(pName, "vkGetShaderModuleIdentifierEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetShaderModuleIdentifierEXT);
    if (strcmp(pName, "vkGetDeferredOperationResultKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetDeferredOperationResultKHR);
    if (strcmp(pName, "vkGetMemoryFdKHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkGetMemoryFdKHR);
    if (strcmp(pName, "vkCreateQueryPool") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCreateQueryPool);
    if (strcmp(pName, "vkDestroyQueryPool") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkDestroyQueryPool);
    if (strcmp(pName, "vkGetShaderModuleCreateInfoIdentifierEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetShaderModuleCreateInfoIdentifierEXT);
    if (strcmp(pName, "vkDeviceWaitIdle") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkDeviceWaitIdle);
    if (strcmp(pName, "vkGetMemoryFdPropertiesKHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkGetMemoryFdPropertiesKHR);
    if (strcmp(pName, "vkGetQueryPoolResults") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkGetQueryPoolResults);
    if (strcmp(pName, "vkBindBufferMemory2") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkBindBufferMemory2);
    if (strcmp(pName, "vkResetQueryPool") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkResetQueryPool);
    if (strcmp(pName, "vkCmdSetRenderingAttachmentLocations") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetRenderingAttachmentLocations);
    if (strcmp(pName, "vkDestroyIndirectCommandsLayoutNV") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkDestroyIndirectCommandsLayoutNV);
    if (strcmp(pName, "vkFreeMemory") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkFreeMemory);
    if (strcmp(pName, "vkImportSemaphoreFdKHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkImportSemaphoreFdKHR);
    if (strcmp(pName, "vkBindImageMemory2") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkBindImageMemory2);
    if (strcmp(pName, "vkSetHdrMetadataEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkSetHdrMetadataEXT);
    if (strcmp(pName, "vkAllocateMemory") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkAllocateMemory);
    if (strcmp(pName, "vkCmdSetRenderingInputAttachmentIndices") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetRenderingInputAttachmentIndices);
    if (strcmp(pName, "vkMapMemory") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkMapMemory);
    if (strcmp(pName, "vkGetSemaphoreFdKHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkGetSemaphoreFdKHR);
    if (strcmp(pName, "vkUnmapMemory") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkUnmapMemory);
    if (strcmp(pName, "vkCmdPushDescriptorSetWithTemplateKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdPushDescriptorSetWithTemplateKHR);
    if (strcmp(pName, "vkCmdPushDescriptorSetKHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdPushDescriptorSetKHR);
    if (strcmp(pName, "vkFlushMappedMemoryRanges") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkFlushMappedMemoryRanges);
    if (strcmp(pName, "vkInvalidateMappedMemoryRanges") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkInvalidateMappedMemoryRanges);
    if (strcmp(pName, "vkCreateRenderPass2KHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCreateRenderPass2KHR);
#if defined(VK_USE_PLATFORM_METAL_EXT)
    if (strcmp(pName, "vkGetMemoryMetalHandleEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkGetMemoryMetalHandleEXT);
#endif  // VK_USE_PLATFORM_METAL_EXT
    if (strcmp(pName, "vkCmdNextSubpass2KHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdNextSubpass2KHR);
    if (strcmp(pName, "vkGetDeviceMemoryCommitment") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkGetDeviceMemoryCommitment);
    if (strcmp(pName, "vkCmdBeginRenderPass2KHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdBeginRenderPass2KHR);
#if defined(VK_USE_PLATFORM_METAL_EXT)
    if (strcmp(pName, "vkGetMemoryMetalHandlePropertiesEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetMemoryMetalHandlePropertiesEXT);
#endif  // VK_USE_PLATFORM_METAL_EXT
    if (strcmp(pName, "vkBindImageMemory") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkBindImageMemory);
    if (strcmp(pName, "vkBindBufferMemory") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkBindBufferMemory);
    if (strcmp(pName, "vkGetSemaphoreCounterValue") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkGetSemaphoreCounterValue);
    if (strcmp(pName, "vkCmdEndRenderPass2KHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdEndRenderPass2KHR);
    if (strcmp(pName, "vkGetPipelineExecutablePropertiesKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPipelineExecutablePropertiesKHR);
    if (strcmp(pName, "vkGetSwapchainImagesKHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkGetSwapchainImagesKHR);
    if (strcmp(pName, "vkGetDescriptorSetLayoutBindingOffsetEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetDescriptorSetLayoutBindingOffsetEXT);
    if (strcmp(pName, "vkCmdCopyBuffer2KHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdCopyBuffer2KHR);
    if (strcmp(pName, "vkCmdSetRayTracingPipelineStackSizeKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetRayTracingPipelineStackSizeKHR);
    if (strcmp(pName, "vkCmdDrawMultiEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdDrawMultiEXT);
    if (strcmp(pName, "vkDestroySwapchainKHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkDestroySwapchainKHR);
    if (strcmp(pName, "vkWaitSemaphores") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkWaitSemaphores);
    if (strcmp(pName, "vkGetDescriptorEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkGetDescriptorEXT);
    if (strcmp(pName, "vkCreatePipelineLayout") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCreatePipelineLayout);
    if (strcmp(pName, "vkSignalSemaphore") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkSignalSemaphore);
    if (strcmp(pName, "vkCmdDrawMultiIndexedEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdDrawMultiIndexedEXT);
    if (strcmp(pName, "vkGetPipelineExecutableStatisticsKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPipelineExecutableStatisticsKHR);
    if (strcmp(pName, "vkCmdBindDescriptorSets2") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdBindDescriptorSets2);
    if (strcmp(pName, "vkCmdBindDescriptorBuffersEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdBindDescriptorBuffersEXT);
    if (strcmp(pName, "vkCmdSetDescriptorBufferOffsetsEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetDescriptorBufferOffsetsEXT);
    if (strcmp(pName, "vkCopyMemoryToImageEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCopyMemoryToImageEXT);
    if (strcmp(pName, "vkAcquireNextImageKHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkAcquireNextImageKHR);
    if (strcmp(pName, "vkGetMemoryHostPointerPropertiesEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetMemoryHostPointerPropertiesEXT);
    if (strcmp(pName, "vkDestroyPipelineLayout") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkDestroyPipelineLayout);
    if (strcmp(pName, "vkCmdPushConstants2") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdPushConstants2);
    if (strcmp(pName, "vkGetPipelineExecutableInternalRepresentationsKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPipelineExecutableInternalRepresentationsKHR);
    if (strcmp(pName, "vkQueuePresentKHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkQueuePresentKHR);
    if (strcmp(pName, "vkCmdPushDescriptorSet2") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdPushDescriptorSet2);
    if (strcmp(pName, "vkCopyImageToImageEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCopyImageToImageEXT);
    if (strcmp(pName, "vkCmdCopyBufferToImage2KHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdCopyBufferToImage2KHR);
    if (strcmp(pName, "vkCmdWriteBufferMarkerAMD") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdWriteBufferMarkerAMD);
    if (strcmp(pName, "vkCmdBindDescriptorBufferEmbeddedSamplersEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdBindDescriptorBufferEmbeddedSamplersEXT);
    if (strcmp(pName, "vkCmdPushDescriptorSetWithTemplate2") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdPushDescriptorSetWithTemplate2);
    if (strcmp(pName, "vkCopyImageToMemoryEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCopyImageToMemoryEXT);
    if (strcmp(pName, "vkCmdCopyImage2KHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdCopyImage2KHR);
    if (strcmp(pName, "vkCmdWriteBufferMarker2AMD") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdWriteBufferMarker2AMD);
    if (strcmp(pName, "vkTransitionImageLayoutEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkTransitionImageLayoutEXT);
    if (strcmp(pName, "vkGetBufferOpaqueCaptureDescriptorDataEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetBufferOpaqueCaptureDescriptorDataEXT);
    if (strcmp(pName, "vkCmdCopyImageToBuffer2KHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdCopyImageToBuffer2KHR);
    if (strcmp(pName, "vkCmdDecodeVideoKHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdDecodeVideoKHR);
    if (strcmp(pName, "vkGetBufferDeviceAddress") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkGetBufferDeviceAddress);
    if (strcmp(pName, "vkGetImageSubresourceLayout2EXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetImageSubresourceLayout2EXT);
    if (strcmp(pName, "vkGetImageOpaqueCaptureDescriptorDataEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetImageOpaqueCaptureDescriptorDataEXT);
    if (strcmp(pName, "vkCmdBlitImage2KHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdBlitImage2KHR);
    if (strcmp(pName, "vkGetBufferOpaqueCaptureAddress") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetBufferOpaqueCaptureAddress);
    if (strcmp(pName, "vkDebugMarkerSetObjectTagEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkDebugMarkerSetObjectTagEXT);
    if (strcmp(pName, "vkGetImageViewOpaqueCaptureDescriptorDataEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetImageViewOpaqueCaptureDescriptorDataEXT);
    if (strcmp(pName, "vkCmdResolveImage2KHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdResolveImage2KHR);
    if (strcmp(pName, "vkGetDeviceGroupPeerMemoryFeatures") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetDeviceGroupPeerMemoryFeatures);
    if (strcmp(pName, "vkMapMemory2KHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkMapMemory2KHR);
    if (strcmp(pName, "vkGetDeviceMemoryOpaqueCaptureAddress") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetDeviceMemoryOpaqueCaptureAddress);
    if (strcmp(pName, "vkCreateSamplerYcbcrConversion") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCreateSamplerYcbcrConversion);
    if (strcmp(pName, "vkGetEncodedVideoSessionParametersKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetEncodedVideoSessionParametersKHR);
    if (strcmp(pName, "vkGetSamplerOpaqueCaptureDescriptorDataEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetSamplerOpaqueCaptureDescriptorDataEXT);
    if (strcmp(pName, "vkCmdSetDeviceMask") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetDeviceMask);
    if (strcmp(pName, "vkUnmapMemory2KHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkUnmapMemory2KHR);
    if (strcmp(pName, "vkDebugMarkerSetObjectNameEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkDebugMarkerSetObjectNameEXT);
    if (strcmp(pName, "vkCmdSetVertexInputEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetVertexInputEXT);
    if (strcmp(pName, "vkCmdDebugMarkerBeginEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdDebugMarkerBeginEXT);
    if (strcmp(pName, "vkCmdBindShadingRateImageNV") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdBindShadingRateImageNV);
    if (strcmp(pName, "vkDestroySamplerYcbcrConversion") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkDestroySamplerYcbcrConversion);
    if (strcmp(pName, "vkCmdEncodeVideoKHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdEncodeVideoKHR);
    if (strcmp(pName, "vkCmdDebugMarkerEndEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdDebugMarkerEndEXT);
    if (strcmp(pName, "vkUninitializePerformanceApiINTEL") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkUninitializePerformanceApiINTEL);
    if (strcmp(pName, "vkCmdDispatchBase") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdDispatchBase);
    if (strcmp(pName, "vkCmdDebugMarkerInsertEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdDebugMarkerInsertEXT);
    if (strcmp(pName, "vkInitializePerformanceApiINTEL") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkInitializePerformanceApiINTEL);
    if (strcmp(pName, "vkGetAccelerationStructureOpaqueCaptureDescriptorDataEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetAccelerationStructureOpaqueCaptureDescriptorDataEXT);
    if (strcmp(pName, "vkCmdSetViewportShadingRatePaletteNV") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetViewportShadingRatePaletteNV);
    if (strcmp(pName, "vkCmdSetDepthBias2EXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetDepthBias2EXT);
    if (strcmp(pName, "vkCmdSetPerformanceMarkerINTEL") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetPerformanceMarkerINTEL);
    if (strcmp(pName, "vkCmdSetPerformanceStreamMarkerINTEL") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetPerformanceStreamMarkerINTEL);
    if (strcmp(pName, "vkCmdSetPerformanceOverrideINTEL") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetPerformanceOverrideINTEL);
    if (strcmp(pName, "vkCreateFramebuffer") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCreateFramebuffer);
    if (strcmp(pName, "vkAcquirePerformanceConfigurationINTEL") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkAcquirePerformanceConfigurationINTEL);
    if (strcmp(pName, "vkCmdSetCoarseSampleOrderNV") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetCoarseSampleOrderNV);
    if (strcmp(pName, "vkCmdBindTransformFeedbackBuffersEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdBindTransformFeedbackBuffersEXT);
    if (strcmp(pName, "vkReleasePerformanceConfigurationINTEL") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkReleasePerformanceConfigurationINTEL);
    if (strcmp(pName, "vkCreateDescriptorUpdateTemplate") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCreateDescriptorUpdateTemplate);
    if (strcmp(pName, "vkDestroyOpticalFlowSessionNV") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkDestroyOpticalFlowSessionNV);
    if (strcmp(pName, "vkGetPerformanceParameterINTEL") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPerformanceParameterINTEL);
    if (strcmp(pName, "vkGetDeviceGroupSurfacePresentModesKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetDeviceGroupSurfacePresentModesKHR);
    if (strcmp(pName, "vkGetCalibratedTimestampsKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetCalibratedTimestampsKHR);
    if (strcmp(pName, "vkQueueSetPerformanceConfigurationINTEL") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkQueueSetPerformanceConfigurationINTEL);
    if (strcmp(pName, "vkCreateSampler") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCreateSampler);
    if (strcmp(pName, "vkCmdSetRasterizerDiscardEnable") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetRasterizerDiscardEnable);
    if (strcmp(pName, "vkGetDeviceGroupPresentCapabilitiesKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetDeviceGroupPresentCapabilitiesKHR);
    if (strcmp(pName, "vkGetCalibratedTimestampsEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetCalibratedTimestampsEXT);
    if (strcmp(pName, "vkCreateOpticalFlowSessionNV") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCreateOpticalFlowSessionNV);
    if (strcmp(pName, "vkBindOpticalFlowSessionImageNV") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkBindOpticalFlowSessionImageNV);
    if (strcmp(pName, "vkUpdateDescriptorSetWithTemplate") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkUpdateDescriptorSetWithTemplate);
    if (strcmp(pName, "vkCreateAccelerationStructureKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCreateAccelerationStructureKHR);
    if (strcmp(pName, "vkCmdSetSampleLocationsEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetSampleLocationsEXT);
    if (strcmp(pName, "vkDestroyDescriptorUpdateTemplate") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkDestroyDescriptorUpdateTemplate);
    if (strcmp(pName, "vkCmdSetPrimitiveRestartEnable") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetPrimitiveRestartEnable);
    if (strcmp(pName, "vkAcquireNextImage2KHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkAcquireNextImage2KHR);
    if (strcmp(pName, "vkCmdBuildAccelerationStructuresKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdBuildAccelerationStructuresKHR);
    if (strcmp(pName, "vkDestroyFramebuffer") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkDestroyFramebuffer);
    if (strcmp(pName, "vkCmdOpticalFlowExecuteNV") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdOpticalFlowExecuteNV);
    if (strcmp(pName, "vkCreateRenderPass") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCreateRenderPass);
    if (strcmp(pName, "vkCmdSetDepthBiasEnable") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetDepthBiasEnable);
    if (strcmp(pName, "vkBuildAccelerationStructuresKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkBuildAccelerationStructuresKHR);
    if (strcmp(pName, "vkCmdBeginTransformFeedbackEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdBeginTransformFeedbackEXT);
    if (strcmp(pName, "vkDestroyAccelerationStructureKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkDestroyAccelerationStructureKHR);
    if (strcmp(pName, "vkCmdBuildAccelerationStructuresIndirectKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdBuildAccelerationStructuresIndirectKHR);
    if (strcmp(pName, "vkGetImageSparseMemoryRequirements2KHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetImageSparseMemoryRequirements2KHR);
    if (strcmp(pName, "vkCmdEndTransformFeedbackEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdEndTransformFeedbackEXT);
    if (strcmp(pName, "vkSetLocalDimmingAMD") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkSetLocalDimmingAMD);
    if (strcmp(pName, "vkCmdBindIndexBuffer2KHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdBindIndexBuffer2KHR);
    if (strcmp(pName, "vkGetImageMemoryRequirements2KHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetImageMemoryRequirements2KHR);
    if (strcmp(pName, "vkDestroyRenderPass") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkDestroyRenderPass);
    if (strcmp(pName, "vkCmdBeginQueryIndexedEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdBeginQueryIndexedEXT);
    if (strcmp(pName, "vkGetRenderingAreaGranularityKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetRenderingAreaGranularityKHR);
    if (strcmp(pName, "vkGetRenderAreaGranularity") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkGetRenderAreaGranularity);
    if (strcmp(pName, "vkGetBufferMemoryRequirements2KHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetBufferMemoryRequirements2KHR);
    if (strcmp(pName, "vkDestroySampler") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkDestroySampler);
    if (strcmp(pName, "vkGetDeviceImageSubresourceLayoutKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetDeviceImageSubresourceLayoutKHR);
    if (strcmp(pName, "vkCmdEndQueryIndexedEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdEndQueryIndexedEXT);
    if (strcmp(pName, "vkGetImageMemoryRequirements2") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetImageMemoryRequirements2);
    if (strcmp(pName, "vkGetDeviceBufferMemoryRequirements") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetDeviceBufferMemoryRequirements);
    if (strcmp(pName, "vkCopyMemoryToAccelerationStructureKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCopyMemoryToAccelerationStructureKHR);
    if (strcmp(pName, "vkCopyAccelerationStructureKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCopyAccelerationStructureKHR);
    if (strcmp(pName, "vkGetBufferMemoryRequirements2") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetBufferMemoryRequirements2);
    if (strcmp(pName, "vkGetImageSubresourceLayout2KHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetImageSubresourceLayout2KHR);
    if (strcmp(pName, "vkGetDeviceImageMemoryRequirements") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetDeviceImageMemoryRequirements);
    if (strcmp(pName, "vkCmdDrawIndirectByteCountEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdDrawIndirectByteCountEXT);
    if (strcmp(pName, "vkGetImageSparseMemoryRequirements2") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetImageSparseMemoryRequirements2);
    if (strcmp(pName, "vkGetDeviceImageSparseMemoryRequirements") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetDeviceImageSparseMemoryRequirements);
    if (strcmp(pName, "vkCopyAccelerationStructureToMemoryKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCopyAccelerationStructureToMemoryKHR);
#if defined(VK_ENABLE_BETA_EXTENSIONS)
    if (strcmp(pName, "vkCreateCudaModuleNV") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCreateCudaModuleNV);
#endif  // VK_ENABLE_BETA_EXTENSIONS
    if (strcmp(pName, "vkWriteAccelerationStructuresPropertiesKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkWriteAccelerationStructuresPropertiesKHR);
    if (strcmp(pName, "vkCmdCopyAccelerationStructureKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdCopyAccelerationStructureKHR);
#if defined(VK_ENABLE_BETA_EXTENSIONS)
    if (strcmp(pName, "vkCreateCudaFunctionNV") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCreateCudaFunctionNV);
#endif  // VK_ENABLE_BETA_EXTENSIONS
#if defined(VK_ENABLE_BETA_EXTENSIONS)
    if (strcmp(pName, "vkGetCudaModuleCacheNV") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkGetCudaModuleCacheNV);
#endif  // VK_ENABLE_BETA_EXTENSIONS
    if (strcmp(pName, "vkCmdSetFragmentShadingRateEnumNV") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetFragmentShadingRateEnumNV);
    if (strcmp(pName, "vkCmdCopyAccelerationStructureToMemoryKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdCopyAccelerationStructureToMemoryKHR);
    if (strcmp(pName, "vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI);
    if (strcmp(pName, "vkCreateCommandPool") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCreateCommandPool);
    if (strcmp(pName, "vkDestroyCommandPool") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkDestroyCommandPool);
    if (strcmp(pName, "vkAntiLagUpdateAMD") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkAntiLagUpdateAMD);
#if defined(VK_ENABLE_BETA_EXTENSIONS)
    if (strcmp(pName, "vkDestroyCudaModuleNV") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkDestroyCudaModuleNV);
#endif  // VK_ENABLE_BETA_EXTENSIONS
    if (strcmp(pName, "vkCmdSubpassShadingHUAWEI") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSubpassShadingHUAWEI);
    if (strcmp(pName, "vkResetCommandPool") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkResetCommandPool);
    if (strcmp(pName, "vkCmdCopyMemoryToAccelerationStructureKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdCopyMemoryToAccelerationStructureKHR);
    if (strcmp(pName, "vkCmdBindInvocationMaskHUAWEI") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdBindInvocationMaskHUAWEI);
    if (strcmp(pName, "vkCopyMemoryToImage") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCopyMemoryToImage);
    if (strcmp(pName, "vkGetAccelerationStructureDeviceAddressKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetAccelerationStructureDeviceAddressKHR);
#if defined(VK_ENABLE_BETA_EXTENSIONS)
    if (strcmp(pName, "vkCmdCudaLaunchKernelNV") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdCudaLaunchKernelNV);
#endif  // VK_ENABLE_BETA_EXTENSIONS
    if (strcmp(pName, "vkCreateCuModuleNVX") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCreateCuModuleNVX);
#if defined(VK_ENABLE_BETA_EXTENSIONS)
    if (strcmp(pName, "vkDestroyCudaFunctionNV") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkDestroyCudaFunctionNV);
#endif  // VK_ENABLE_BETA_EXTENSIONS
    if (strcmp(pName, "vkCmdWriteAccelerationStructuresPropertiesKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdWriteAccelerationStructuresPropertiesKHR);
    if (strcmp(pName, "vkGetClusterAccelerationStructureBuildSizesNV") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetClusterAccelerationStructureBuildSizesNV);
    if (strcmp(pName, "vkCopyImageToImage") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCopyImageToImage);
    if (strcmp(pName, "vkCmdBuildClusterAccelerationStructureIndirectNV") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdBuildClusterAccelerationStructureIndirectNV);
    if (strcmp(pName, "vkGetDeviceAccelerationStructureCompatibilityKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetDeviceAccelerationStructureCompatibilityKHR);
    if (strcmp(pName, "vkCopyImageToMemory") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCopyImageToMemory);
    if (strcmp(pName, "vkCmdSetEvent2KHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetEvent2KHR);
    if (strcmp(pName, "vkReleaseSwapchainImagesEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkReleaseSwapchainImagesEXT);
    if (strcmp(pName, "vkTransitionImageLayout") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkTransitionImageLayout);
    if (strcmp(pName, "vkGetAccelerationStructureBuildSizesKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetAccelerationStructureBuildSizesKHR);
    if (strcmp(pName, "vkGetMemoryRemoteAddressNV") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkGetMemoryRemoteAddressNV);
    if (strcmp(pName, "vkCmdResetEvent2KHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdResetEvent2KHR);
    if (strcmp(pName, "vkCmdWaitEvents2KHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdWaitEvents2KHR);
    if (strcmp(pName, "vkCreateCuFunctionNVX") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCreateCuFunctionNVX);
    if (strcmp(pName, "vkCmdPipelineBarrier2KHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdPipelineBarrier2KHR);
    if (strcmp(pName, "vkDestroyCuFunctionNVX") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkDestroyCuFunctionNVX);
    if (strcmp(pName, "vkCreateMicromapEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCreateMicromapEXT);
    if (strcmp(pName, "vkDestroyCuModuleNVX") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkDestroyCuModuleNVX);
    if (strcmp(pName, "vkGetPipelinePropertiesEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkGetPipelinePropertiesEXT);
    if (strcmp(pName, "vkCmdWriteTimestamp2KHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdWriteTimestamp2KHR);
    if (strcmp(pName, "vkCmdCuLaunchKernelNVX") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdCuLaunchKernelNVX);
    if (strcmp(pName, "vkCreateAccelerationStructureNV") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCreateAccelerationStructureNV);
    if (strcmp(pName, "vkDestroyMicromapEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkDestroyMicromapEXT);
    if (strcmp(pName, "vkCreatePrivateDataSlotEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCreatePrivateDataSlotEXT);
    if (strcmp(pName, "vkQueueSubmit2KHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkQueueSubmit2KHR);
    if (strcmp(pName, "vkDestroyPrivateDataSlotEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkDestroyPrivateDataSlotEXT);
    if (strcmp(pName, "vkCreateSharedSwapchainsKHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCreateSharedSwapchainsKHR);
    if (strcmp(pName, "vkBuildMicromapsEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkBuildMicromapsEXT);
    if (strcmp(pName, "vkCmdBuildMicromapsEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdBuildMicromapsEXT);
    if (strcmp(pName, "vkGetDeviceFaultInfoEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkGetDeviceFaultInfoEXT);
    if (strcmp(pName, "vkCmdDrawMeshTasksIndirectNV") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdDrawMeshTasksIndirectNV);
    if (strcmp(pName, "vkCmdSetFragmentShadingRateKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetFragmentShadingRateKHR);
    if (strcmp(pName, "vkSetPrivateDataEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkSetPrivateDataEXT);
    if (strcmp(pName, "vkDestroyAccelerationStructureNV") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkDestroyAccelerationStructureNV);
    if (strcmp(pName, "vkAllocateCommandBuffers") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkAllocateCommandBuffers);
    if (strcmp(pName, "vkGetImageViewAddressNVX") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkGetImageViewAddressNVX);
    if (strcmp(pName, "vkFreeCommandBuffers") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkFreeCommandBuffers);
    if (strcmp(pName, "vkGetAccelerationStructureMemoryRequirementsNV") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetAccelerationStructureMemoryRequirementsNV);
    if (strcmp(pName, "vkCmdDrawMeshTasksNV") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdDrawMeshTasksNV);
    if (strcmp(pName, "vkGetImageViewHandleNVX") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkGetImageViewHandleNVX);
    if (strcmp(pName, "vkGetPrivateDataEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkGetPrivateDataEXT);
    if (strcmp(pName, "vkCopyMicromapToMemoryEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCopyMicromapToMemoryEXT);
    if (strcmp(pName, "vkCreateSamplerYcbcrConversionKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCreateSamplerYcbcrConversionKHR);
    if (strcmp(pName, "vkCmdDrawMeshTasksIndirectCountNV") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdDrawMeshTasksIndirectCountNV);
    if (strcmp(pName, "vkBindAccelerationStructureMemoryNV") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkBindAccelerationStructureMemoryNV);
    if (strcmp(pName, "vkCopyMicromapEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCopyMicromapEXT);
    if (strcmp(pName, "vkCmdDrawIndirectCountAMD") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdDrawIndirectCountAMD);
    if (strcmp(pName, "vkBeginCommandBuffer") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkBeginCommandBuffer);
    if (strcmp(pName, "vkBindBufferMemory2KHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkBindBufferMemory2KHR);
    if (strcmp(pName, "vkCreateShadersEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCreateShadersEXT);
    if (strcmp(pName, "vkGetImageViewHandle64NVX") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkGetImageViewHandle64NVX);
    if (strcmp(pName, "vkDestroySamplerYcbcrConversionKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkDestroySamplerYcbcrConversionKHR);
    if (strcmp(pName, "vkCreatePrivateDataSlot") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCreatePrivateDataSlot);
    if (strcmp(pName, "vkCmdBuildAccelerationStructureNV") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdBuildAccelerationStructureNV);
    if (strcmp(pName, "vkWriteMicromapsPropertiesEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkWriteMicromapsPropertiesEXT);
    if (strcmp(pName, "vkDestroyPrivateDataSlot") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkDestroyPrivateDataSlot);
    if (strcmp(pName, "vkCopyMemoryToMicromapEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCopyMemoryToMicromapEXT);
    if (strcmp(pName, "vkEndCommandBuffer") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkEndCommandBuffer);
    if (strcmp(pName, "vkCmdCopyAccelerationStructureNV") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdCopyAccelerationStructureNV);
    if (strcmp(pName, "vkCmdDrawIndexedIndirectCountAMD") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdDrawIndexedIndirectCountAMD);
    if (strcmp(pName, "vkCmdCopyMicromapEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdCopyMicromapEXT);
    if (strcmp(pName, "vkResetCommandBuffer") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkResetCommandBuffer);
    if (strcmp(pName, "vkTrimCommandPool") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkTrimCommandPool);
    if (strcmp(pName, "vkSetPrivateData") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkSetPrivateData);
    if (strcmp(pName, "vkBindImageMemory2KHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkBindImageMemory2KHR);
    if (strcmp(pName, "vkCmdCopyMicromapToMemoryEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdCopyMicromapToMemoryEXT);
    if (strcmp(pName, "vkGetPrivateData") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkGetPrivateData);
    if (strcmp(pName, "vkCmdSetRenderingAttachmentLocationsKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetRenderingAttachmentLocationsKHR);
    if (strcmp(pName, "vkCreateRayTracingPipelinesNV") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCreateRayTracingPipelinesNV);
    if (strcmp(pName, "vkCmdTraceRaysNV") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdTraceRaysNV);
    if (strcmp(pName, "vkCmdSetRenderingInputAttachmentIndicesKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetRenderingInputAttachmentIndicesKHR);
    if (strcmp(pName, "vkCmdCopyMemoryToMicromapEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdCopyMemoryToMicromapEXT);
    if (strcmp(pName, "vkCmdSetEvent2") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetEvent2);
    if (strcmp(pName, "vkCmdWriteMicromapsPropertiesEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdWriteMicromapsPropertiesEXT);
    if (strcmp(pName, "vkCmdSetPatchControlPointsEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetPatchControlPointsEXT);
    if (strcmp(pName, "vkGetDeviceMicromapCompatibilityEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetDeviceMicromapCompatibilityEXT);
    if (strcmp(pName, "vkDestroyShaderEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkDestroyShaderEXT);
    if (strcmp(pName, "vkCmdResetEvent2") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdResetEvent2);
    if (strcmp(pName, "vkCmdSetExclusiveScissorEnableNV") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetExclusiveScissorEnableNV);
    if (strcmp(pName, "vkCreateDescriptorSetLayout") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCreateDescriptorSetLayout);
    if (strcmp(pName, "vkCmdSetDepthBiasEnableEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetDepthBiasEnableEXT);
    if (strcmp(pName, "vkCmdSetExclusiveScissorNV") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetExclusiveScissorNV);
    if (strcmp(pName, "vkGetShaderBinaryDataEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkGetShaderBinaryDataEXT);
    if (strcmp(pName, "vkDestroyDescriptorSetLayout") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkDestroyDescriptorSetLayout);
    if (strcmp(pName, "vkCmdWaitEvents2") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdWaitEvents2);
    if (strcmp(pName, "vkCmdSetRasterizerDiscardEnableEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetRasterizerDiscardEnableEXT);
    if (strcmp(pName, "vkCreateDescriptorPool") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCreateDescriptorPool);
    if (strcmp(pName, "vkGetMicromapBuildSizesEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkGetMicromapBuildSizesEXT);
    if (strcmp(pName, "vkCmdBindShadersEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdBindShadersEXT);
    if (strcmp(pName, "vkGetRayTracingShaderGroupHandlesKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetRayTracingShaderGroupHandlesKHR);
    if (strcmp(pName, "vkCmdBuildPartitionedAccelerationStructuresNV") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdBuildPartitionedAccelerationStructuresNV);
    if (strcmp(pName, "vkCmdDrawIndirectCount") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdDrawIndirectCount);
    if (strcmp(pName, "vkCmdSetDepthClampRangeEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetDepthClampRangeEXT);
    if (strcmp(pName, "vkCmdPipelineBarrier2") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdPipelineBarrier2);
    if (strcmp(pName, "vkCmdSetLogicOpEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetLogicOpEXT);
    if (strcmp(pName, "vkGetPartitionedAccelerationStructuresBuildSizesNV") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetPartitionedAccelerationStructuresBuildSizesNV);
    if (strcmp(pName, "vkCmdSetPrimitiveRestartEnableEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetPrimitiveRestartEnableEXT);
    if (strcmp(pName, "vkCmdDrawIndexedIndirectCount") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdDrawIndexedIndirectCount);
    if (strcmp(pName, "vkGetRayTracingShaderGroupHandlesNV") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetRayTracingShaderGroupHandlesNV);
    if (strcmp(pName, "vkAllocateDescriptorSets") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkAllocateDescriptorSets);
    if (strcmp(pName, "vkCmdWriteTimestamp2") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdWriteTimestamp2);
    if (strcmp(pName, "vkGetImageDrmFormatModifierPropertiesEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetImageDrmFormatModifierPropertiesEXT);
    if (strcmp(pName, "vkQueueSubmit2") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkQueueSubmit2);
    if (strcmp(pName, "vkCmdSetCheckpointNV") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetCheckpointNV);
    if (strcmp(pName, "vkDestroyDescriptorPool") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkDestroyDescriptorPool);
    if (strcmp(pName, "vkGetAccelerationStructureHandleNV") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetAccelerationStructureHandleNV);
    if (strcmp(pName, "vkResetDescriptorPool") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkResetDescriptorPool);
    if (strcmp(pName, "vkCmdSetColorWriteEnableEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetColorWriteEnableEXT);
    if (strcmp(pName, "vkGetQueueCheckpointDataNV") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkGetQueueCheckpointDataNV);
    if (strcmp(pName, "vkFreeDescriptorSets") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkFreeDescriptorSets);
    if (strcmp(pName, "vkCmdWriteAccelerationStructuresPropertiesNV") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdWriteAccelerationStructuresPropertiesNV);
    if (strcmp(pName, "vkCmdSetLineStipple") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetLineStipple);
    if (strcmp(pName, "vkCmdDrawClusterHUAWEI") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdDrawClusterHUAWEI);
    if (strcmp(pName, "vkGetQueueCheckpointData2NV") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkGetQueueCheckpointData2NV);
    if (strcmp(pName, "vkCmdDrawClusterIndirectHUAWEI") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdDrawClusterIndirectHUAWEI);
    if (strcmp(pName, "vkUpdateDescriptorSets") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkUpdateDescriptorSets);
    if (strcmp(pName, "vkCompileDeferredNV") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCompileDeferredNV);
    if (strcmp(pName, "vkCreateVideoSessionKHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCreateVideoSessionKHR);
    if (strcmp(pName, "vkGetBufferDeviceAddressEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkGetBufferDeviceAddressEXT);
    if (strcmp(pName, "vkCmdDrawMeshTasksIndirectCountEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdDrawMeshTasksIndirectCountEXT);
    if (strcmp(pName, "vkDestroyVideoSessionKHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkDestroyVideoSessionKHR);
    if (strcmp(pName, "vkDestroyValidationCacheEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkDestroyValidationCacheEXT);
    if (strcmp(pName, "vkGetSemaphoreCounterValueKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetSemaphoreCounterValueKHR);
    if (strcmp(pName, "vkGetVideoSessionMemoryRequirementsKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetVideoSessionMemoryRequirementsKHR);
    if (strcmp(pName, "vkCmdTraceRaysKHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdTraceRaysKHR);
    if (strcmp(pName, "vkCmdTraceRaysIndirect2KHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdTraceRaysIndirect2KHR);
    if (strcmp(pName, "vkCreateValidationCacheEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCreateValidationCacheEXT);
    if (strcmp(pName, "vkWaitSemaphoresKHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkWaitSemaphoresKHR);
    if (strcmp(pName, "vkBindVideoSessionMemoryKHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkBindVideoSessionMemoryKHR);
    if (strcmp(pName, "vkCmdDrawIndirectCountKHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdDrawIndirectCountKHR);
    if (strcmp(pName, "vkCreateRayTracingPipelinesKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCreateRayTracingPipelinesKHR);
    if (strcmp(pName, "vkCmdBindPipeline") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdBindPipeline);
    if (strcmp(pName, "vkGetDescriptorSetLayoutSupportKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetDescriptorSetLayoutSupportKHR);
    if (strcmp(pName, "vkGetDeviceImageSparseMemoryRequirementsKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetDeviceImageSparseMemoryRequirementsKHR);
    if (strcmp(pName, "vkSetDeviceMemoryPriorityEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkSetDeviceMemoryPriorityEXT);
    if (strcmp(pName, "vkGetDeviceQueue2") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkGetDeviceQueue2);
    if (strcmp(pName, "vkSignalSemaphoreKHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkSignalSemaphoreKHR);
    if (strcmp(pName, "vkGetValidationCacheDataEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkGetValidationCacheDataEXT);
    if (strcmp(pName, "vkGetDeviceImageMemoryRequirementsKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetDeviceImageMemoryRequirementsKHR);
    if (strcmp(pName, "vkMergeValidationCachesEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkMergeValidationCachesEXT);
    if (strcmp(pName, "vkCmdDrawMeshTasksEXT") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdDrawMeshTasksEXT);
    if (strcmp(pName, "vkGetDeviceBufferMemoryRequirementsKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetDeviceBufferMemoryRequirementsKHR);
    if (strcmp(pName, "vkCmdSetViewport") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetViewport);
    if (strcmp(pName, "vkGetGeneratedCommandsMemoryRequirementsNV") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetGeneratedCommandsMemoryRequirementsNV);
    if (strcmp(pName, "vkCmdDrawMeshTasksIndirectEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdDrawMeshTasksIndirectEXT);
    if (strcmp(pName, "vkCreateVideoSessionParametersKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCreateVideoSessionParametersKHR);
    if (strcmp(pName, "vkCmdDrawIndexedIndirectCountKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdDrawIndexedIndirectCountKHR);
    if (strcmp(pName, "vkCmdSetScissor") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetScissor);
    if (strcmp(pName, "vkGetRayTracingCaptureReplayShaderGroupHandlesKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetRayTracingCaptureReplayShaderGroupHandlesKHR);
    if (strcmp(pName, "vkCmdTraceRaysIndirectKHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdTraceRaysIndirectKHR);
    if (strcmp(pName, "vkDestroyVideoSessionParametersKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkDestroyVideoSessionParametersKHR);
    if (strcmp(pName, "vkCmdPreprocessGeneratedCommandsNV") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdPreprocessGeneratedCommandsNV);
    if (strcmp(pName, "vkCmdSetLineWidth") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetLineWidth);
    if (strcmp(pName, "vkCreateSwapchainKHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCreateSwapchainKHR);
    if (strcmp(pName, "vkUpdateVideoSessionParametersKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkUpdateVideoSessionParametersKHR);
    if (strcmp(pName, "vkCreateGraphicsPipelines") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCreateGraphicsPipelines);
    if (strcmp(pName, "vkCreateComputePipelines") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCreateComputePipelines);
    if (strcmp(pName, "vkCmdExecuteGeneratedCommandsNV") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdExecuteGeneratedCommandsNV);
    if (strcmp(pName, "vkCmdSetDepthBias") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetDepthBias);
    if (strcmp(pName, "vkGetDescriptorSetLayoutSizeEXT") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetDescriptorSetLayoutSizeEXT);
    if (strcmp(pName, "vkGetDescriptorSetLayoutSupport") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetDescriptorSetLayoutSupport);
    if (strcmp(pName, "vkCmdEndVideoCodingKHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdEndVideoCodingKHR);
    if (strcmp(pName, "vkCmdBindPipelineShaderGroupNV") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCmdBindPipelineShaderGroupNV);
    if (strcmp(pName, "vkDestroyPipeline") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkDestroyPipeline);
    if (strcmp(pName, "vkCmdBeginVideoCodingKHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdBeginVideoCodingKHR);
    if (strcmp(pName, "vkCmdSetBlendConstants") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdSetBlendConstants);
    if (strcmp(pName, "vkCreateIndirectCommandsLayoutNV") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkCreateIndirectCommandsLayoutNV);
    if (strcmp(pName, "vkGetRayTracingShaderGroupStackSizeKHR") == 0)
        return reinterpret_cast<PFN_vkVoidFunction>(vkGetRayTracingShaderGroupStackSizeKHR);
    if (strcmp(pName, "vkCmdControlVideoCodingKHR") == 0) return reinterpret_cast<PFN_vkVoidFunction>(vkCmdControlVideoCodingKHR);

    if (device_dispatch_table(device)->GetDeviceProcAddr == NULL) return NULL;
    return device_dispatch_table(device)->GetDeviceProcAddr(device, pName);
}
