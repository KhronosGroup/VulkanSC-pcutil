/*
 * Copyright (c) 2025 The Khronos Group Inc.
 * Copyright (c) 2025 RasterGrid Kft.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "pcwriter.hpp"
#include <vulkan/vulkan_sc.h>

#include <gtest/gtest.h>
#include "object_type_string_helper.h"
#include "sample_pipeline.h"
#include "pctest_writer_helper.h"

#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <inttypes.h>
#include <array>

#define MAX_QUEUE_FAMILY_COUNT 32
#define MAX_SWAPCHAIN_IMAGE_COUNT 16
#define MAX_INSTANCE_LAYER_COUNT 16
#define MAX_INSTANCE_EXTENSION_COUNT 64
#define APP_SHORT_NAME "pcwriter_test"
#define MAX_DEBUG_MESSAGE_LENGTH 8192
#define MAX_PHYSICAL_DEVICE_COUNT 16
#define MAX_PHYSICAL_DEVICE_EXTENSION_COUNT 256
#define U_ASSERT_ONLY
#define MAX_PIPELINE_POOL_SIZE 1048576

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

char const *to_string(VkPhysicalDeviceType const type) {
    switch (type) {
        case VK_PHYSICAL_DEVICE_TYPE_OTHER:
            return "Other";
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
            return "IntegratedGpu";
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
            return "DiscreteGpu";
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
            return "VirtualGpu";
        case VK_PHYSICAL_DEVICE_TYPE_CPU:
            return "Cpu";
        default:
            return "Unknown";
    }
}

struct ErrorMonitor {
    ErrorMonitor() { Reset(); }

    void AddDesiredMessage(const std::string &msg) { desired_messages_.insert(msg); }

    void AddIgnoredMessage(const std::string &msg) { ignored_messages_.insert(msg); }

    void Reset() {
        desired_messages_.clear();
        ignored_messages_.clear();
        occured_desired_messages_.clear();

        undesired_message_occurs_ = false;

        // generally ignore warning about using debug validation layer
        ignored_messages_.insert("WARNING-CreateInstance-debug-warning");
    }

    bool Check() { return (desired_messages_.size() == occured_desired_messages_.size()) && !undesired_message_occurs_; }

    bool ProcessMsg(const char *vuid_cstr, const char *msg) {
        std::string vuid = vuid_cstr ? vuid_cstr : "";
        if (vuid == "Loader Message") {
            return true;
        }

        auto msg_with_vuid = vuid + " " + msg;

        for (const auto &ignored_message : ignored_messages_) {
            if (msg_with_vuid.find(ignored_message) != std::string::npos) {
                return true;
            }
        }

        for (const auto &desired_message : desired_messages_) {
            if (msg_with_vuid.find(desired_message) != std::string::npos) {
                occured_desired_messages_.insert(desired_message);
                return true;
            }
        }

        undesired_message_occurs_ = true;
        return false;
    }

    bool undesired_message_occurs_ = false;

    std::set<std::string> desired_messages_;
    std::set<std::string> ignored_messages_;

    std::set<std::string> occured_desired_messages_;
};

VKAPI_ATTR VkBool32 VKAPI_CALL debug_messenger_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                        VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                        void *pUserData) {
    ErrorMonitor *error_monitor = reinterpret_cast<ErrorMonitor *>(pUserData);

    bool processed = error_monitor->ProcessMsg(pCallbackData->pMessageIdName, pCallbackData->pMessage);

    if (processed) {
        return false;
    }

    char prefix[64] = "";
    static char message[MAX_DEBUG_MESSAGE_LENGTH];
    memset(message, 0, MAX_DEBUG_MESSAGE_LENGTH);

    if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
        strcat(prefix, "VERBOSE : ");
    } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
        strcat(prefix, "INFO : ");
    } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        strcat(prefix, "WARNING : ");
    } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        strcat(prefix, "ERROR : ");
    }

    if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) {
        strcat(prefix, "GENERAL");
    } else {
        if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) {
            strcat(prefix, "VALIDATION");
        }
        if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) {
            if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) {
                strcat(prefix, "|");
            }
            strcat(prefix, "PERFORMANCE");
        }
    }

    sprintf(message, "%s - Message Id Number: %d | Message Id Name: %s\n\t%s\n", prefix, pCallbackData->messageIdNumber,
            pCallbackData->pMessageIdName == NULL ? "" : pCallbackData->pMessageIdName, pCallbackData->pMessage);
    if (pCallbackData->objectCount > 0) {
        char tmp_message[500];
        sprintf(tmp_message, "\n\tObjects - %d\n", pCallbackData->objectCount);
        strcat(message, tmp_message);
        for (uint32_t object = 0; object < pCallbackData->objectCount; ++object) {
            sprintf(tmp_message, "\t\tObject[%d] - %s", object, string_VkObjectType(pCallbackData->pObjects[object].objectType));
            strcat(message, tmp_message);

            VkObjectType t = pCallbackData->pObjects[object].objectType;
            if (t == VK_OBJECT_TYPE_INSTANCE || t == VK_OBJECT_TYPE_PHYSICAL_DEVICE || t == VK_OBJECT_TYPE_DEVICE ||
                t == VK_OBJECT_TYPE_COMMAND_BUFFER || t == VK_OBJECT_TYPE_QUEUE) {
                sprintf(tmp_message, ", Handle %p", (void *)(uintptr_t)(pCallbackData->pObjects[object].objectHandle));
                strcat(message, tmp_message);
            } else {
                sprintf(tmp_message, ", Handle Ox%" PRIx64, (pCallbackData->pObjects[object].objectHandle));
                strcat(message, tmp_message);
            }

            if (NULL != pCallbackData->pObjects[object].pObjectName && strlen(pCallbackData->pObjects[object].pObjectName) > 0) {
                sprintf(tmp_message, ", Name \"%s\"", pCallbackData->pObjects[object].pObjectName);
                strcat(message, tmp_message);
            }
            sprintf(tmp_message, "\n");
            strcat(message, tmp_message);
        }
    }
    if (pCallbackData->cmdBufLabelCount > 0) {
        char tmp_message[500];
        sprintf(tmp_message, "\n\tCommand Buffer Labels - %d\n", pCallbackData->cmdBufLabelCount);
        strcat(message, tmp_message);
        for (uint32_t cmd_buf_label = 0; cmd_buf_label < pCallbackData->cmdBufLabelCount; ++cmd_buf_label) {
            sprintf(tmp_message, "\t\tLabel[%d] - %s { %f, %f, %f, %f}\n", cmd_buf_label,
                    pCallbackData->pCmdBufLabels[cmd_buf_label].pLabelName, pCallbackData->pCmdBufLabels[cmd_buf_label].color[0],
                    pCallbackData->pCmdBufLabels[cmd_buf_label].color[1], pCallbackData->pCmdBufLabels[cmd_buf_label].color[2],
                    pCallbackData->pCmdBufLabels[cmd_buf_label].color[3]);
            strcat(message, tmp_message);
        }
    }

    printf("%s\n", message);
    fflush(stdout);

    // Don't bail out, but keep going.
    return false;
}

struct TestEnviroment {
    int32_t gpu_number = 0;

    VkInstance inst = VK_NULL_HANDLE;
    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    uint32_t graphics_queue_family_index;
    uint32_t present_queue_family_index;
    VkPhysicalDeviceProperties gpu_props;
    VkQueueFamilyProperties queue_props[MAX_QUEUE_FAMILY_COUNT];
    VkPhysicalDeviceMemoryProperties memory_properties;

    uint32_t enabled_extension_count = 0;
    uint32_t enabled_layer_count = 0;
    const char *extension_names[64];
    const char *enabled_layers[64];

    VkDebugUtilsMessengerEXT dbg_messenger;

    PFN_vkCreateDebugUtilsMessengerEXT pfnCreateDebugUtilsMessengerEXT;
    PFN_vkDestroyDebugUtilsMessengerEXT pfnDestroyDebugUtilsMessengerEXT;

    uint32_t queue_family_count = 0;

    ErrorMonitor error_monitor;
};

static VkBool32 test_check_layers(uint32_t check_count, const char **check_names, uint32_t layer_count, VkLayerProperties *layers) {
    for (uint32_t i = 0; i < check_count; i++) {
        VkBool32 found = 0;
        for (uint32_t j = 0; j < layer_count; j++) {
            if (!strcmp(check_names[i], layers[j].layerName)) {
                found = 1;
                break;
            }
        }
        if (!found) {
            fprintf(stderr, "Cannot find layer: %s\n", check_names[i]);
            return 0;
        }
    }
    return 1;
}

static bool test_init_vk(TestEnviroment &test_env) {
    VkResult err;
    uint32_t instance_extension_count = 0;
    uint32_t instance_layer_count = 0;
    const char *instance_validation_layers[] = {"VK_LAYER_KHRONOS_validation"};
    test_env.enabled_extension_count = 0;
    test_env.enabled_layer_count = 0;

    // Look for validation layers
    VkBool32 validation_found = 0;
    err = vkEnumerateInstanceLayerProperties(&instance_layer_count, NULL);
    assert(!err);

    if (instance_layer_count > 0) {
        VkLayerProperties instance_layers[MAX_INSTANCE_LAYER_COUNT];
        err = vkEnumerateInstanceLayerProperties(&instance_layer_count, instance_layers);
        assert(!err);

        validation_found = test_check_layers(ARRAY_SIZE(instance_validation_layers), instance_validation_layers,
                                             instance_layer_count, instance_layers);
        if (validation_found) {
            test_env.enabled_layer_count = ARRAY_SIZE(instance_validation_layers);
            test_env.enabled_layers[0] = "VK_LAYER_KHRONOS_validation";
        }
    }

    if (!validation_found) {
        printf(
            "vkEnumerateInstanceLayerProperties failed to find required validation layer.\n\n"
            "Please look at the Getting Started guide for additional information.\n"
            "vkCreateInstance Failure");
        return false;
    }

    /* Look for instance extensions */
    memset(test_env.extension_names, 0, sizeof(test_env.extension_names));

    err = vkEnumerateInstanceExtensionProperties(NULL, &instance_extension_count, NULL);
    assert(!err);

    if (instance_extension_count > 0) {
        VkExtensionProperties instance_extensions[MAX_INSTANCE_EXTENSION_COUNT];
        err = vkEnumerateInstanceExtensionProperties(NULL, &instance_extension_count, instance_extensions);
        assert(!err);
        for (uint32_t i = 0; i < instance_extension_count; i++) {
            if (!strcmp(VK_EXT_DEBUG_UTILS_EXTENSION_NAME, instance_extensions[i].extensionName)) {
                test_env.extension_names[test_env.enabled_extension_count++] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
            }
            assert(test_env.enabled_extension_count < 64);
        }
    }

    VkApplicationInfo app = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
    app.pNext = NULL;
    app.pApplicationName = APP_SHORT_NAME;
    app.applicationVersion = 0;
    app.pEngineName = APP_SHORT_NAME;
    app.engineVersion = 0;
    app.apiVersion = VKSC_API_VERSION_1_0;

    VkInstanceCreateInfo inst_info = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    inst_info.pNext = NULL;
    inst_info.flags = 0;
    inst_info.pApplicationInfo = &app;
    inst_info.enabledLayerCount = test_env.enabled_layer_count;
    inst_info.ppEnabledLayerNames = (const char *const *)instance_validation_layers;
    inst_info.enabledExtensionCount = test_env.enabled_extension_count;
    inst_info.ppEnabledExtensionNames = (const char *const *)test_env.extension_names;

    /*
     * This is info for a temp callback to use during CreateInstance.
     * After the instance is created, we use the instance-based
     * function to register the final callback.
     */
    VkDebugUtilsMessengerCreateInfoEXT dbg_messenger_create_info;

    // VK_EXT_debug_utils style
    dbg_messenger_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    dbg_messenger_create_info.pNext = NULL;
    dbg_messenger_create_info.flags = 0;
    dbg_messenger_create_info.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    dbg_messenger_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    dbg_messenger_create_info.pfnUserCallback = debug_messenger_callback;
    dbg_messenger_create_info.pUserData = &test_env.error_monitor;
    inst_info.pNext = &dbg_messenger_create_info;

    err = vkCreateInstance(&inst_info, NULL, &test_env.inst);
    if (err == VK_ERROR_INCOMPATIBLE_DRIVER) {
        printf(
            "Cannot find a compatible Vulkan installable client driver (ICD).\n\n"
            "Please look at the Getting Started guide for additional information.\n"
            "vkCreateInstance Failure");
        return false;
    } else if (err == VK_ERROR_EXTENSION_NOT_PRESENT) {
        printf(
            "Cannot find a specified extension library.\n"
            "Make sure your layers path is set appropriately.\n"
            "vkCreateInstance Failure");
        return false;
    } else if (err) {
        printf(
            "vkCreateInstance failed.\n\n"
            "Do you have a compatible Vulkan installable client driver (ICD) installed?\n"
            "Please look at the Getting Started guide for additional information.\n"
            "vkCreateInstance Failure");
        return false;
    }

    test_env.pfnCreateDebugUtilsMessengerEXT =
        (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(test_env.inst, "vkCreateDebugUtilsMessengerEXT");
    test_env.pfnDestroyDebugUtilsMessengerEXT =
        (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(test_env.inst, "vkDestroyDebugUtilsMessengerEXT");

    /* Make initial call to query gpu_count, then second call for gpu info */
    uint32_t gpu_count = 0;
    err = vkEnumeratePhysicalDevices(test_env.inst, &gpu_count, NULL);
    assert(!err);

    if (gpu_count <= 0) {
        printf(
            "vkEnumeratePhysicalDevices reported zero accessible devices.\n"
            "vkEnumeratePhysicalDevices Failure");
        return false;
    }

    VkPhysicalDevice physical_devices[MAX_PHYSICAL_DEVICE_COUNT];
    err = vkEnumeratePhysicalDevices(test_env.inst, &gpu_count, physical_devices);
    assert(!err);

    assert(test_env.gpu_number >= 0);
    test_env.gpu = physical_devices[test_env.gpu_number];
    {
        VkPhysicalDeviceProperties physicalDeviceProperties;
        vkGetPhysicalDeviceProperties(test_env.gpu, &physicalDeviceProperties);
        fprintf(stderr, "Selected GPU %d: %s, type: %s\n", test_env.gpu_number, physicalDeviceProperties.deviceName,
                to_string(physicalDeviceProperties.deviceType));
    }

    // Get Memory information and properties as soon as we picked a physical device
    vkGetPhysicalDeviceMemoryProperties(test_env.gpu, &test_env.memory_properties);

    /* Look for device extensions */
    test_env.enabled_extension_count = 0;
    memset(test_env.extension_names, 0, sizeof(test_env.extension_names));

    err = test_env.pfnCreateDebugUtilsMessengerEXT(test_env.inst, &dbg_messenger_create_info, NULL, &test_env.dbg_messenger);
    switch (err) {
        case VK_SUCCESS:
            break;
        case VK_ERROR_OUT_OF_HOST_MEMORY:
            printf(
                "CreateDebugUtilsMessengerEXT: out of host memory\n"
                "CreateDebugUtilsMessengerEXT Failure");
            return false;
        default:
            printf(
                "CreateDebugUtilsMessengerEXT: unknown failure\n"
                "CreateDebugUtilsMessengerEXT Failure");
            return false;
    }

    vkGetPhysicalDeviceProperties(test_env.gpu, &test_env.gpu_props);

    /* Call with NULL data to get count */
    vkGetPhysicalDeviceQueueFamilyProperties(test_env.gpu, &test_env.queue_family_count, NULL);
    assert(test_env.queue_family_count >= 1);

    vkGetPhysicalDeviceQueueFamilyProperties(test_env.gpu, &test_env.queue_family_count, test_env.queue_props);

    // Query fine-grained feature support for this device.
    //  If app has specific feature requirements it should check supported
    //  features based on this query
    VkPhysicalDeviceFeatures physDevFeatures;
    vkGetPhysicalDeviceFeatures(test_env.gpu, &physDevFeatures);

    // Search for the first graphics queue
    uint32_t graphicsQueueFamilyIndex = UINT32_MAX;
    for (uint32_t i = 0; i < test_env.queue_family_count; i++) {
        if ((test_env.queue_props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
            graphicsQueueFamilyIndex = i;
            break;
        }
    }
    if (graphicsQueueFamilyIndex == UINT32_MAX) {
        printf(
            "Failed to find graphics queue\n"
            "Vulkan init failure");
        return false;
    }
    test_env.graphics_queue_family_index = graphicsQueueFamilyIndex;

    return true;
}

void test_teardown_vk(TestEnviroment &test_env) {
    vkDestroyDevice(test_env.device, nullptr);
    test_env.pfnDestroyDebugUtilsMessengerEXT(test_env.inst, test_env.dbg_messenger, nullptr);
    vkDestroyInstance(test_env.inst, nullptr);
}

static bool test_create_device(TestEnviroment &test_env, const std::vector<uint8_t> &cache) {
    VkResult U_ASSERT_ONLY err;

    float queue_priorities[1] = {0.0};

    VkDeviceQueueCreateInfo queues[1];
    queues[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queues[0].pNext = NULL;
    queues[0].queueFamilyIndex = test_env.graphics_queue_family_index;
    queues[0].queueCount = 1;
    queues[0].pQueuePriorities = queue_priorities;
    queues[0].flags = 0;

    VkPipelineCacheCreateInfo pipelineCache = {VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO};
    pipelineCache.flags = VK_PIPELINE_CACHE_CREATE_READ_ONLY_BIT | VK_PIPELINE_CACHE_CREATE_USE_APPLICATION_STORAGE_BIT;
    pipelineCache.initialDataSize = cache.size();
    pipelineCache.pInitialData = cache.data();

    VkPipelinePoolSize pipelinePoolSize = {VK_STRUCTURE_TYPE_PIPELINE_POOL_SIZE};
    pipelinePoolSize.pNext = NULL;
    pipelinePoolSize.poolEntrySize = MAX_PIPELINE_POOL_SIZE;
    pipelinePoolSize.poolEntryCount = 1;

    VkDeviceObjectReservationCreateInfo objectReservation = {VK_STRUCTURE_TYPE_DEVICE_OBJECT_RESERVATION_CREATE_INFO};
    objectReservation.pNext = NULL;
    objectReservation.pipelineCacheCreateInfoCount = 1;
    objectReservation.pPipelineCacheCreateInfos = &pipelineCache;
    objectReservation.pipelinePoolSizeCount = 1;
    objectReservation.pPipelinePoolSizes = &pipelinePoolSize;
    objectReservation.semaphoreRequestCount = 3;
    objectReservation.commandBufferRequestCount = MAX_SWAPCHAIN_IMAGE_COUNT * 2 + 1;
    objectReservation.fenceRequestCount = 1;
    objectReservation.deviceMemoryRequestCount = MAX_SWAPCHAIN_IMAGE_COUNT + 3;
    objectReservation.bufferRequestCount = MAX_SWAPCHAIN_IMAGE_COUNT + 1;
    objectReservation.imageRequestCount = 3;
    objectReservation.eventRequestCount = 0;
    objectReservation.queryPoolRequestCount = 0;
    objectReservation.bufferViewRequestCount = 0;
    objectReservation.imageViewRequestCount = MAX_SWAPCHAIN_IMAGE_COUNT + 1;
    objectReservation.layeredImageViewRequestCount = 0;
    objectReservation.pipelineCacheRequestCount = 1;
    objectReservation.pipelineLayoutRequestCount = 1;
    objectReservation.renderPassRequestCount = 1;
    objectReservation.graphicsPipelineRequestCount = 1;
    objectReservation.computePipelineRequestCount = 0;
    objectReservation.descriptorSetLayoutRequestCount = 1;
    objectReservation.samplerRequestCount = 0;
    objectReservation.descriptorPoolRequestCount = 1;
    objectReservation.descriptorSetRequestCount = MAX_SWAPCHAIN_IMAGE_COUNT;
    objectReservation.framebufferRequestCount = MAX_SWAPCHAIN_IMAGE_COUNT;
    objectReservation.commandPoolRequestCount = 2;
    objectReservation.samplerYcbcrConversionRequestCount = 0;
    objectReservation.surfaceRequestCount = 0;
    objectReservation.swapchainRequestCount = 1;
    objectReservation.displayModeRequestCount = 0;
    objectReservation.subpassDescriptionRequestCount = 1;
    objectReservation.attachmentDescriptionRequestCount = 2;
    objectReservation.descriptorSetLayoutBindingRequestCount = 2;
    objectReservation.descriptorSetLayoutBindingLimit = 2;
    objectReservation.maxImageViewMipLevels = 1;
    objectReservation.maxImageViewArrayLayers = 1;
    objectReservation.maxLayeredImageViewMipLevels = 0;
    objectReservation.maxOcclusionQueriesPerPool = 0;
    objectReservation.maxPipelineStatisticsQueriesPerPool = 0;
    objectReservation.maxTimestampQueriesPerPool = 0;
    objectReservation.maxImmutableSamplersPerDescriptorSetLayout = 0;

    VkPhysicalDeviceVulkanSC10Features sc10Features = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_SC_1_0_FEATURES};
    sc10Features.pNext = &objectReservation;
    sc10Features.shaderAtomicInstructions = VK_FALSE;

    VkDeviceCreateInfo device = {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
    device.pNext = &sc10Features;
    device.queueCreateInfoCount = 1;
    device.pQueueCreateInfos = queues;
    device.enabledLayerCount = 0;
    device.ppEnabledLayerNames = NULL;
    device.enabledExtensionCount = test_env.enabled_extension_count;
    device.ppEnabledExtensionNames = (const char *const *)test_env.extension_names;
    device.pEnabledFeatures = NULL;  // If specific features are required, pass them in here

    err = vkCreateDevice(test_env.gpu, &device, NULL, &test_env.device);
    if (err == VK_ERROR_INVALID_PIPELINE_CACHE_DATA) {
        printf(
            "vkCreateDevice failed with VK_ERROR_INVALID_PIPELINE_CACHE_DATA."
            "vkCreateDevice Failure");
        return false;
    } else if (err) {
        printf(
            "vkCreateDevice failed.\n"
            "vkCreateDevice Failure");
        return false;
    }

    return true;
}

static const std::array<uint8_t, VK_UUID_SIZE> EmulationPipelineCacheUUID = {0x27, 0xA8, 0x32, 0x5A, 0x71, 0x38, 0x4C, 0xA3,
                                                                             0x89, 0x15, 0x68, 0x72, 0xE8, 0x61, 0x42, 0x1A};

#define VK_DEVICE_ID_PORTABLE 0xD19CDA7A

TEST(PCWriterDeviceTest, EmptyPipeline) {
    TestEnviroment test_env;

    test_env.error_monitor.Reset();

    test_init_vk(test_env);

    std::vector<uint8_t> cache(10 * 1024 * 1024);

    VKSCPipelineCacheHeaderWriter pcw;

    pcw.setVendorID(VK_VENDOR_ID_KHRONOS);
    pcw.setDeviceID(VK_DEVICE_ID_PORTABLE);
    pcw.setPipelineCacheUUID(EmulationPipelineCacheUUID.data());

    pcw.writeHeaderSafetyCriticalOne(cache.size(), cache.data());
    pcw.writePipelineIndex(cache.size(), cache.data());

    EXPECT_TRUE(test_create_device(test_env, cache));
    EXPECT_TRUE(test_env.error_monitor.Check());

    test_teardown_vk(test_env);
}

TEST(PCWriterDeviceTest, ComputePipeline) {
    TestEnviroment test_env;

    test_env.error_monitor.Reset();

    test_init_vk(test_env);

    std::vector<uint8_t> cache(PCWriterTestData::default_cache_size_);
    auto compute_spirv = CompileSPV(kSampleComputeShaderSpv);

    PCWriterTestData test_data(__LINE__);
    test_data.GenPipelineEntries(1);
    test_data.GenShaderStages(0, 1);

    test_data.SetJson(0, kSampleComputePipelineJson);
    test_data.SetShaderStageCode(0, 0, compute_spirv);

    VKSCPipelineCacheHeaderWriter pcw;
    WriteTestDataToCache(pcw, cache, test_data);

    EXPECT_TRUE(test_create_device(test_env, cache));
    EXPECT_TRUE(test_env.error_monitor.Check());

    test_teardown_vk(test_env);
}

TEST(PCWriterDeviceTest, GraphicsPipeline) {
    TestEnviroment test_env;

    test_env.error_monitor.Reset();

    test_init_vk(test_env);

    PCWriterTestData test_data(__LINE__);
    test_data.GenPipelineEntries(1);
    test_data.GenShaderStages(0, 2);

    auto vertex_spirv = CompileSPV(kSampleVertexShaderSpv);
    auto fragment_spirv = CompileSPV(kSampleFragmentShaderSpv);

    test_data.SetJson(0, kSampleGraphicsPipelineJson);

    test_data.SetShaderStageCode(0, 0, vertex_spirv);
    test_data.SetShaderStageCode(0, 1, fragment_spirv);

    std::vector<uint8_t> cache(PCWriterTestData::default_cache_size_);

    VKSCPipelineCacheHeaderWriter pcw;
    WriteTestDataToCache(pcw, cache, test_data);

    EXPECT_TRUE(test_create_device(test_env, cache));
    EXPECT_TRUE(test_env.error_monitor.Check());

    test_teardown_vk(test_env);
}

TEST(PCWriterDeviceTest, ComputePipelineMissingSPIRV) {
    TestEnviroment test_env;

    test_env.error_monitor.Reset();

    test_env.error_monitor.AddDesiredMessage("VUID-VkShaderModuleCreateInfo-pCode-08736");

    test_init_vk(test_env);

    std::vector<uint8_t> cache(PCWriterTestData::default_cache_size_);

    PCWriterTestData test_data(__LINE__);
    test_data.GenPipelineEntries(1);
    test_data.GenShaderStages(0, 1);

    test_data.SetJson(0, kSampleComputePipelineJson);

    VKSCPipelineCacheHeaderWriter pcw;
    WriteTestDataToCache(pcw, cache, test_data);

    EXPECT_TRUE(test_create_device(test_env, cache));
    EXPECT_TRUE(test_env.error_monitor.Check());

    test_teardown_vk(test_env);
}

TEST(PCWriterDeviceTest, ComputePipelineInvalidSPIRVSize) {
    TestEnviroment test_env;

    test_env.error_monitor.Reset();

    test_env.error_monitor.AddDesiredMessage("VUID-VkPipelineCacheStageValidationIndexEntry-codeSize-05086");
    test_env.error_monitor.AddDesiredMessage("UNASSIGNED-PipelineCacheData-SpirvDepValMissingInfo");

    test_init_vk(test_env);

    std::vector<uint8_t> cache(PCWriterTestData::default_cache_size_);

    PCWriterTestData test_data(__LINE__);
    test_data.GenPipelineEntries(1);
    test_data.GenShaderStages(0, 1);

    test_data.SetJson(0, kSampleComputePipelineJson);

    auto compute_spirv = CompileSPV(kSampleComputeShaderSpv);

    std::vector<uint8_t> spirv_uint8(4 * compute_spirv.size());

    memcpy(spirv_uint8.data(), compute_spirv.data(), spirv_uint8.size());

    spirv_uint8.push_back(0);
    spirv_uint8.push_back(0);

    test_data.SetShaderStageCode(0, 0, spirv_uint8);

    VKSCPipelineCacheHeaderWriter pcw;
    WriteTestDataToCache(pcw, cache, test_data);

    EXPECT_TRUE(test_create_device(test_env, cache));
    EXPECT_TRUE(test_env.error_monitor.Check());

    test_teardown_vk(test_env);
}
