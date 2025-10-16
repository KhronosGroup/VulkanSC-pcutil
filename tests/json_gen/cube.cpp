/*
 * Copyright (c) 2025 The Khronos Group Inc.
 * Copyright (c) 2025 RasterGrid Kft.
 * Copyright (c) 2021, NVIDIA CORPORATION
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "cube.hpp"

#include "vkresult_gtest_adapter.hpp"

#if defined(NDEBUG) && defined(__GNUC__)
#define U_ASSERT_ONLY __attribute__((unused))
#else
#define U_ASSERT_ONLY
#endif

#define APP_SHORT_NAME "vkcube"
#define APP_LONG_NAME "Vulkan Cube"
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

static std::vector<const char *> tex_files{"logo.ppm"};

Cube::Cube() : surface{VK_NULL_HANDLE}, image_acquired_semaphores{}, draw_complete_semaphores{}, image_ownership_semaphores{} {
    demo_init();
}

    Cube::~Cube() { demo_cleanup(); }

    VkPipeline Cube::get_pipeline() const { return pipeline; }

    void Cube::demo_init() {
        vec3 eye = {0.0f, 3.0f, 5.0f};
        vec3 origin = {0, 0, 0};
        vec3 up = {0.0f, 1.0f, 0.0};

        gpu_number = 0;
        invalid_gpu_selection = false;
        width = 256;
        height = 256;
        use_staging_buffer = true;
        presentMode = VK_PRESENT_MODE_FIFO_KHR;
        use_break = false;
        validate = false;
        frameCount = 1;
        suppress_popups = true;
        VK_KHR_incremental_present_enabled = false;
        wsi_platform = WSI_PLATFORM_FILE;

        demo_init_vk();

        spin_angle = 4.0f;
        spin_increment = 0.2f;
        pause = false;

        mat4x4_perspective(projection_matrix, (float)degreesToRadians(45.0f), 1.0f, 0.1f, 100.0f);
        mat4x4_look_at(view_matrix, eye, origin, up);
        mat4x4_identity(model_matrix);

        projection_matrix[1][1] *= -1;  // Flip projection matrix from GL to Vulkan orientation.
    }

    void Cube::demo_init_vk() {
        VkResult err;
        uint32_t instance_extension_count = 0;
        enabled_extension_count = 0;
        enabled_layer_count = 0;
        is_minimized = false;
        cmd_pool = VK_NULL_HANDLE;

        /* Look for instance extensions */
        VkBool32 surfaceExtFound = false;
        VkBool32 platformSurfaceExtFound = false;
        VkBool32 releaseDispExtFound = false;
        memset(extension_names, 0, sizeof(extension_names));

        err = vkEnumerateInstanceExtensionProperties(NULL, &instance_extension_count, NULL);
        assert(!err);

        if (instance_extension_count > 0) {
            VkExtensionProperties instance_extensions[MAX_INSTANCE_EXTENSION_COUNT];
            err = vkEnumerateInstanceExtensionProperties(NULL, &instance_extension_count, instance_extensions);
            assert(!err);
            for (uint32_t i = 0; i < instance_extension_count; i++) {
                if (!strcmp(VK_KHR_SURFACE_EXTENSION_NAME, instance_extensions[i].extensionName)) {
                    surfaceExtFound = true;
                    extension_names[enabled_extension_count++] = VK_KHR_SURFACE_EXTENSION_NAME;
                }
#if defined(VK_USE_PLATFORM_DISPLAY_KHR)
                if (wsi_platform == WSI_PLATFORM_AUTO || wsi_platform == WSI_PLATFORM_DISPLAY) {
                    if (!strcmp(VK_KHR_DISPLAY_EXTENSION_NAME, instance_extensions[i].extensionName)) {
                        platformSurfaceExtFound = true;
                        extension_names[enabled_extension_count++] = VK_KHR_DISPLAY_EXTENSION_NAME;
                    }
                    if (!strcmp(VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME, instance_extensions[i].extensionName)) {
                        releaseDispExtFound = true;
                        extension_names[enabled_extension_count++] = VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME;
                    }
                }
#endif
                if (!strcmp(VK_EXT_DEBUG_UTILS_EXTENSION_NAME, instance_extensions[i].extensionName)) {
                    if (validate) {
                        extension_names[enabled_extension_count++] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
                    }
                }
                assert(enabled_extension_count < 64);
            }
        }

        if (wsi_platform != WSI_PLATFORM_FILE) {
            if (!surfaceExtFound) {
                ERR_EXIT("vkEnumerateInstanceExtensionProperties failed to find the " VK_KHR_SURFACE_EXTENSION_NAME
                            " extension.\n\n"
                            "Do you have a compatible Vulkan installable client driver (ICD) installed?\n"
                            "Please look at the Getting Started guide for additional information.\n",
                            "vkCreateInstance Failure");
            }
            if (!platformSurfaceExtFound) {
#if defined(VK_USE_PLATFORM_DISPLAY_KHR)
                if (wsi_platform == WSI_PLATFORM_DISPLAY) {
                    ERR_EXIT("vkEnumerateInstanceExtensionProperties failed to find the " VK_KHR_DISPLAY_EXTENSION_NAME
                                " extension.\n\n"
                                "Do you have a compatible Vulkan installable client driver (ICD) installed?\n"
                                "Please look at the Getting Started guide for additional information.\n",
                                "vkCreateInstance Failure");
                }
#endif
                ERR_EXIT(
                    "vkEnumerateInstanceExtensionProperties failed to find any supported WSI surface extension.\n\n"
                    "Do you have a compatible Vulkan installable client driver (ICD) installed?\n"
                    "Please look at the Getting Started guide for additional information.\n",
                    "vkCreateInstance Failure");
            }
        }

        bool auto_wsi_platform = wsi_platform == WSI_PLATFORM_AUTO;

        demo_check_and_set_wsi_platform();

        // Print a message to indicate the automatically set WSI platform
        if (auto_wsi_platform && wsi_platform != WSI_PLATFORM_AUTO) {
            fprintf(stderr, "Selected WSI platform: %s\n", wsi_to_string(wsi_platform));
        }

        const VkApplicationInfo app = {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pNext = NULL,
            .pApplicationName = APP_SHORT_NAME,
            .applicationVersion = 0,
            .pEngineName = APP_SHORT_NAME,
            .engineVersion = 0,
            .apiVersion = VK_API_VERSION_1_0,
        };
        VkInstanceCreateInfo inst_info = {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .pApplicationInfo = &app,
            .enabledLayerCount = enabled_layer_count,
            .ppEnabledLayerNames = NULL,
            .enabledExtensionCount = enabled_extension_count,
            .ppEnabledExtensionNames = (const char *const *)extension_names,
        };

        err = vkCreateInstance(&inst_info, NULL, &inst);
        if (err == VK_ERROR_INCOMPATIBLE_DRIVER) {
            ERR_EXIT(
                "Cannot find a compatible Vulkan installable client driver (ICD).\n\n"
                "Please look at the Getting Started guide for additional information.\n",
                "vkCreateInstance Failure");
        } else if (err == VK_ERROR_EXTENSION_NOT_PRESENT) {
            ERR_EXIT(
                "Cannot find a specified extension library.\n"
                "Make sure your layers path is set appropriately.\n",
                "vkCreateInstance Failure");
        } else if (err) {
            ERR_EXIT(
                "vkCreateInstance failed.\n\n"
                "Do you have a compatible Vulkan installable client driver (ICD) installed?\n"
                "Please look at the Getting Started guide for additional information.\n",
                "vkCreateInstance Failure");
        }

        initialize_entry_points(inst);

        /* Make initial call to query gpu_count, then second call for gpu info */
        uint32_t gpu_count = 0;
        err = vkEnumeratePhysicalDevices(inst, &gpu_count, NULL);
        assert(!err);

        if (gpu_count <= 0) {
            ERR_EXIT("vkEnumeratePhysicalDevices reported zero accessible devices.\n", "vkEnumeratePhysicalDevices Failure");
        }

        VkPhysicalDevice physical_devices[MAX_PHYSICAL_DEVICE_COUNT];
        err = vkEnumeratePhysicalDevices(inst, &gpu_count, physical_devices);
        assert(!err);
        if (invalid_gpu_selection || (gpu_number >= 0 && !((uint32_t)gpu_number < gpu_count))) {
            fprintf(stderr, "GPU %d specified is not present, GPU count = %u\n", gpu_number, gpu_count);
            ERR_EXIT("Specified GPU number is not present", "User Error");
        }

        if (wsi_platform == WSI_PLATFORM_DISPLAY) {
#if defined(VK_USE_PLATFORM_DISPLAY_KHR)
            gpu_number = find_display_gpu(gpu_number, gpu_count, physical_devices);
            if (gpu_number < 0) {
                printf("Cannot find any display!\n");
                fflush(stdout);
                exit(1);
            }
#else
            printf(
                "WSI selection was set to DISPLAY but vksccube was not compiled with support for the DISPLAY platform, exiting "
                "\n");
            fflush(stdout);
            exit(1);
#endif
        } else {
            /* Try to auto select most suitable device */
            if (gpu_number == -1) {
                uint32_t count_device_type[VK_PHYSICAL_DEVICE_TYPE_CPU + 1];
                memset(count_device_type, 0, sizeof(count_device_type));

                VkPhysicalDeviceProperties physicalDeviceProperties;
                for (uint32_t i = 0; i < gpu_count; i++) {
                    vkGetPhysicalDeviceProperties(physical_devices[i], &physicalDeviceProperties);
                    assert(physicalDeviceProperties.deviceType <= VK_PHYSICAL_DEVICE_TYPE_CPU);
                    count_device_type[physicalDeviceProperties.deviceType]++;
                }

                VkPhysicalDeviceType search_for_device_type = VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
                if (count_device_type[VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU]) {
                    search_for_device_type = VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
                } else if (count_device_type[VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU]) {
                    search_for_device_type = VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU;
                } else if (count_device_type[VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU]) {
                    search_for_device_type = VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU;
                } else if (count_device_type[VK_PHYSICAL_DEVICE_TYPE_CPU]) {
                    search_for_device_type = VK_PHYSICAL_DEVICE_TYPE_CPU;
                } else if (count_device_type[VK_PHYSICAL_DEVICE_TYPE_OTHER]) {
                    search_for_device_type = VK_PHYSICAL_DEVICE_TYPE_OTHER;
                }

                for (uint32_t i = 0; i < gpu_count; i++) {
                    vkGetPhysicalDeviceProperties(physical_devices[i], &physicalDeviceProperties);
                    if (physicalDeviceProperties.deviceType == search_for_device_type) {
                        gpu_number = i;
                        break;
                    }
                }
            }
        }

        assert(gpu_number >= 0);
        gpu = physical_devices[gpu_number];
        {
            VkPhysicalDeviceProperties physicalDeviceProperties;
            vkGetPhysicalDeviceProperties(gpu, &physicalDeviceProperties);
            fprintf(stderr, "Selected GPU %d: %s, type: %s\n", gpu_number, physicalDeviceProperties.deviceName,
                    to_string(physicalDeviceProperties.deviceType));
        }

        // Get Memory information and properties as soon as we picked a physical device
        vkGetPhysicalDeviceMemoryProperties(gpu, &memory_properties);

        /* Look for device extensions */
        uint32_t device_extension_count = 0;
        VkBool32 swapchainExtFound = 0;
        VkBool32 acquireDispExtFound = 0;
        enabled_extension_count = 0;
        memset(extension_names, 0, sizeof(extension_names));

        err = vkEnumerateDeviceExtensionProperties(gpu, NULL, &device_extension_count, NULL);
        assert(!err);

        if (device_extension_count > 0) {
            VkExtensionProperties device_extensions[MAX_PHYSICAL_DEVICE_EXTENSION_COUNT];
            err = vkEnumerateDeviceExtensionProperties(gpu, NULL, &device_extension_count, device_extensions);
            assert(!err);

            for (uint32_t i = 0; i < device_extension_count; i++) {
                if (!strcmp(VK_KHR_SWAPCHAIN_EXTENSION_NAME, device_extensions[i].extensionName)) {
                    swapchainExtFound = 1;
                    extension_names[enabled_extension_count++] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
                }
#ifdef VK_USE_PLATFORM_WIN32_KHR
                if (!strcmp(VK_NV_ACQUIRE_WINRT_DISPLAY_EXTENSION_NAME, device_extensions[i].extensionName) &&
                    (wsi_platform == WSI_PLATFORM_AUTO || wsi_platform == WSI_PLATFORM_DISPLAY)) {
                    acquireDispExtFound = 1;
                    extension_names[enabled_extension_count++] = VK_NV_ACQUIRE_WINRT_DISPLAY_EXTENSION_NAME;
                }
#endif
            }

            if (VK_KHR_incremental_present_enabled) {
                // Even though the user "enabled" the extension via the command
                // line, we must make sure that it's enumerated for use with the
                // device.  Therefore, disable it here, and re-enable it again if
                // enumerated.
                VK_KHR_incremental_present_enabled = false;
                for (uint32_t i = 0; i < device_extension_count; i++) {
                    if (!strcmp(VK_KHR_INCREMENTAL_PRESENT_EXTENSION_NAME, device_extensions[i].extensionName)) {
                        extension_names[enabled_extension_count++] = VK_KHR_INCREMENTAL_PRESENT_EXTENSION_NAME;
                        VK_KHR_incremental_present_enabled = true;
                        DbgMsg("VK_KHR_incremental_present extension enabled\n");
                    }
                    assert(enabled_extension_count < 64);
                }
                if (!VK_KHR_incremental_present_enabled) {
                    DbgMsg("VK_KHR_incremental_present extension NOT AVAILABLE\n");
                }
            }
        }

        if (!swapchainExtFound) {
            ERR_EXIT("vkEnumerateDeviceExtensionProperties failed to find the " VK_KHR_SWAPCHAIN_EXTENSION_NAME
                        " extension.\n\nDo you have a compatible Vulkan installable client driver (ICD) installed?\n"
                        "Please look at the Getting Started guide for additional information.\n",
                        "vkCreateInstance Failure");
        }

        if (acquireDispExtFound && releaseDispExtFound) {
            VK_NV_acquire_winrt_display_supported = true;
        }

        vkGetPhysicalDeviceProperties(gpu, &gpu_props);

        /* Call with NULL data to get count */
        vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queue_family_count, NULL);
        assert(queue_family_count >= 1);

        vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queue_family_count, queue_props);

        // Query fine-grained feature support for this device.
        //  If app has specific feature requirements it should check supported
        //  features based on this query
        VkPhysicalDeviceFeatures physDevFeatures;
        vkGetPhysicalDeviceFeatures(gpu, &physDevFeatures);
    }

    void Cube::demo_check_and_set_wsi_platform() {
#if defined(VK_USE_PLATFORM_DISPLAY_KHR)
        if (wsi_platform == WSI_PLATFORM_DISPLAY || wsi_platform == WSI_PLATFORM_AUTO) {
            bool display_extension_available = false;
            for (uint32_t i = 0; i < enabled_extension_count; i++) {
                if (strcmp(extension_names[i], VK_KHR_DISPLAY_EXTENSION_NAME) == 0) {
                    display_extension_available = true;
                    break;
                }
            }
            if (display_extension_available) {
                // Because DISPLAY doesn't require additional libraries, we can assume that it works if we got here
                wsi_platform = WSI_PLATFORM_DISPLAY;
                return;
            }
        }
#endif
    }

    void Cube::demo_init_vk_swapchain() {
        VkResult U_ASSERT_ONLY err;

        demo_create_surface();

        if (wsi_platform == WSI_PLATFORM_FILE) {
            // Search for the first graphics queue
            uint32_t graphicsQueueFamilyIndex = UINT32_MAX;
            for (uint32_t i = 0; i < queue_family_count; i++) {
                if ((queue_props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
                    graphicsQueueFamilyIndex = i;
                    break;
                }
            }
            if (graphicsQueueFamilyIndex == UINT32_MAX) {
                ERR_EXIT("Failed to find graphics queue", "Vulkan init failure");
            }
            graphics_queue_family_index = graphicsQueueFamilyIndex;
            separate_present_queue = false;
        } else {
            // Iterate over each queue to learn whether it supports presenting:
            VkBool32 supportsPresent[MAX_QUEUE_FAMILY_COUNT];
            for (uint32_t i = 0; i < queue_family_count; i++) {
                vkGetPhysicalDeviceSurfaceSupportKHR(gpu, i, surface, &supportsPresent[i]);
            }

            // Search for a graphics and a present queue in the array of queue
            // families, try to find one that supports both
            uint32_t graphicsQueueFamilyIndex = UINT32_MAX;
            uint32_t presentQueueFamilyIndex = UINT32_MAX;
            for (uint32_t i = 0; i < queue_family_count; i++) {
                if ((queue_props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
                    if (graphicsQueueFamilyIndex == UINT32_MAX) {
                        graphicsQueueFamilyIndex = i;
                    }

                    if (supportsPresent[i] == VK_TRUE) {
                        graphicsQueueFamilyIndex = i;
                        presentQueueFamilyIndex = i;
                        break;
                    }
                }
            }

            if (presentQueueFamilyIndex == UINT32_MAX) {
                // If didn't find a queue that supports both graphics and present, then
                // find a separate present queue.
                for (uint32_t i = 0; i < queue_family_count; ++i) {
                    if (supportsPresent[i] == VK_TRUE) {
                        presentQueueFamilyIndex = i;
                        break;
                    }
                }
            }

            // Generate error if could not find both a graphics and a present queue
            if (graphicsQueueFamilyIndex == UINT32_MAX || presentQueueFamilyIndex == UINT32_MAX) {
                ERR_EXIT("Could not find both graphics and present queues\n", "Swapchain Initialization Failure");
            }

            graphics_queue_family_index = graphicsQueueFamilyIndex;
            present_queue_family_index = presentQueueFamilyIndex;
            separate_present_queue = (graphics_queue_family_index != present_queue_family_index);
        }

        demo_create_device();

        vkGetDeviceQueue(device, graphics_queue_family_index, 0, &graphics_queue);

        if (!separate_present_queue) {
            present_queue = graphics_queue;
        } else {
            vkGetDeviceQueue(device, present_queue_family_index, 0, &present_queue);
        }

        if (wsi_platform != WSI_PLATFORM_FILE) {
            // Get the list of VkFormat's that are supported:
            uint32_t formatCount;
            err = pfnGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &formatCount, NULL);
            assert(!err);
            VkSurfaceFormatKHR surfFormats[MAX_SURFACE_FORMAT_COUNT];
            err = pfnGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &formatCount, surfFormats);
            assert(!err);
            VkSurfaceFormatKHR surfaceFormat = pick_surface_format(surfFormats, formatCount);
            format = surfaceFormat.format;
            color_space = surfaceFormat.colorSpace;
        }

        quit = false;
        curFrame = 0;

        // Create fences that we can use to throttle if we get too far
        // ahead of the image presents
        VkFenceCreateInfo fence_ci = {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, .pNext = NULL, .flags = VK_FENCE_CREATE_SIGNALED_BIT};
        for (uint32_t i = 0; i < FRAME_LAG; i++) {
            err = vkCreateFence(device, &fence_ci, NULL, &fences[i]);
            assert(!err);
        }
        frame_index = 0;
        image_index = 0;
        first_swapchain_frame = true;
    }

    void Cube::demo_prepare() {
        if (wsi_platform != WSI_PLATFORM_FILE) {
            demo_prepare_buffers();
        } else {
            if (width == -1) {
                width = 512;
            }
            if (height == -1) {
                height = 512;
            }
            if (frameCount == INT32_MAX) {
                frameCount = 1;
            }

            demo_prepare_offscreen_texture();
        }

        if (is_minimized) {
            prepared = false;
            return;
        }

        VkResult U_ASSERT_ONLY err;
        if (cmd_pool == VK_NULL_HANDLE) {
            const VkCommandPoolCreateInfo cmd_pool_info = {.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                                                            .pNext = NULL,
                                                            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
                                                            .queueFamilyIndex = graphics_queue_family_index};
            err = vkCreateCommandPool(device, &cmd_pool_info, NULL, &cmd_pool);
            assert(!err);
        }

        const VkCommandBufferAllocateInfo cmd_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext = NULL,
            .commandPool = cmd_pool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1,
        };
        err = vkAllocateCommandBuffers(device, &cmd_info, &cmd);
        assert(!err);
        VkCommandBufferBeginInfo cmd_buf_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = NULL,
            .flags = 0,
            .pInheritanceInfo = NULL,
        };
        err = vkBeginCommandBuffer(cmd, &cmd_buf_info);
        assert(!err);

        demo_prepare_depth();
        demo_prepare_textures();
        demo_prepare_cube_data_buffers();

        demo_prepare_descriptor_layout();
        demo_prepare_render_pass();
        demo_prepare_pipeline();

        for (uint32_t i = 0; i < swapchainImageCount; i++) {
            err = vkAllocateCommandBuffers(device, &cmd_info, &swapchain_image_resources[i].cmd);
            assert(!err);
        }

        if (separate_present_queue) {
            const VkCommandPoolCreateInfo present_cmd_pool_info = {.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                                                                    .pNext = NULL,
                                                                    .flags = 0,
                                                                    .queueFamilyIndex = present_queue_family_index};
            err = vkCreateCommandPool(device, &present_cmd_pool_info, NULL, &present_cmd_pool);
            assert(!err);
            const VkCommandBufferAllocateInfo present_cmd_info = {
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                .pNext = NULL,
                .commandPool = present_cmd_pool,
                .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                .commandBufferCount = 1,
            };
            for (uint32_t i = 0; i < swapchainImageCount; i++) {
                err =
                    vkAllocateCommandBuffers(device, &present_cmd_info, &swapchain_image_resources[i].graphics_to_present_cmd);
                assert(!err);
                demo_build_image_ownership_cmd(i);
            }
        }

        demo_prepare_descriptor_pool();
        demo_prepare_descriptor_set();

        demo_prepare_framebuffers();

        /*
            * Prepare functions above may generate pipeline commands
            * that need to be flushed before beginning the render loop.
            */
        demo_flush_init_cmd();
        if (staging_texture.buffer) {
            demo_destroy_texture(&staging_texture);
        }

        for (uint32_t i = 0; i < swapchainImageCount; i++) {
            current_buffer = i;
            demo_draw_build_cmd(swapchain_image_resources[i].cmd);
        }

        current_buffer = 0;
        prepared = true;
        first_swapchain_frame = true;
    }

    void Cube::demo_prepare_buffers() {
        VkResult U_ASSERT_ONLY err;

        // Check the surface capabilities and formats
        VkSurfaceCapabilitiesKHR surfCapabilities;
        err = pfnGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, surface, &surfCapabilities);
        if (err != VK_SUCCESS) {
            ERR_EXIT("Failed to get surface capabilities", "vkGetPhysicalDeviceSurfaceCapabilitiesKHR Failure");
        }

        uint32_t presentModeCount;
        err = pfnGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &presentModeCount, NULL);
        if (err != VK_SUCCESS) {
            ERR_EXIT("Failed to get surface present mode count", "vkGetPhysicalDeviceSurfacePresentModesKHR Failure");
        }
        VkPresentModeKHR presentModes[MAX_PRESENT_MODE_COUNT];
        err = pfnGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &presentModeCount, presentModes);
        if (err != VK_SUCCESS) {
            ERR_EXIT("Failed to get surface present modes", "vkGetPhysicalDeviceSurfacePresentModesKHR Failure");
        }

        VkExtent2D swapchainExtent;
        // width and height are either both 0xFFFFFFFF, or both not 0xFFFFFFFF.
        if (surfCapabilities.currentExtent.width == 0xFFFFFFFF) {
            // If the surface size is undefined, the size is set to the size
            // of the images requested, which must fit within the minimum and
            // maximum values.
            swapchainExtent.width = width;
            swapchainExtent.height = height;

            if (swapchainExtent.width < surfCapabilities.minImageExtent.width) {
                swapchainExtent.width = surfCapabilities.minImageExtent.width;
            } else if (swapchainExtent.width > surfCapabilities.maxImageExtent.width) {
                swapchainExtent.width = surfCapabilities.maxImageExtent.width;
            }

            if (swapchainExtent.height < surfCapabilities.minImageExtent.height) {
                swapchainExtent.height = surfCapabilities.minImageExtent.height;
            } else if (swapchainExtent.height > surfCapabilities.maxImageExtent.height) {
                swapchainExtent.height = surfCapabilities.maxImageExtent.height;
            }
        } else {
            // If the surface size is defined, the swap chain size must match
            swapchainExtent = surfCapabilities.currentExtent;
            width = surfCapabilities.currentExtent.width;
            height = surfCapabilities.currentExtent.height;
        }

        if (surfCapabilities.maxImageExtent.width == 0 || surfCapabilities.maxImageExtent.height == 0) {
            is_minimized = true;
            return;
        } else {
            is_minimized = false;
        }

        // The FIFO present mode is guaranteed by the spec to be supported
        // and to have no tearing.  It's a great default present mode to use.
        VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;

        //  There are times when you may wish to use another present mode.  The
        //  following code shows how to select them, and the comments provide some
        //  reasons you may wish to use them.
        //
        // It should be noted that Vulkan 1.0 doesn't provide a method for
        // synchronizing rendering with the presentation engine's display.  There
        // is a method provided for throttling rendering with the display, but
        // there are some presentation engines for which this method will not work.
        // If an application doesn't throttle its rendering, and if it renders much
        // faster than the refresh rate of the display, this can waste power on
        // mobile devices.  That is because power is being spent rendering images
        // that may never be seen.

        // VK_PRESENT_MODE_IMMEDIATE_KHR is for applications that don't care about
        // tearing, or have some way of synchronizing their rendering with the
        // display.
        // VK_PRESENT_MODE_MAILBOX_KHR may be useful for applications that
        // generally render a new presentable image every refresh cycle, but are
        // occasionally early.  In this case, the application wants the new image
        // to be displayed instead of the previously-queued-for-presentation image
        // that has not yet been displayed.
        // VK_PRESENT_MODE_FIFO_RELAXED_KHR is for applications that generally
        // render a new presentable image every refresh cycle, but are occasionally
        // late.  In this case (perhaps because of stuttering/latency concerns),
        // the application wants the late image to be immediately displayed, even
        // though that may mean some tearing.

        if (presentMode != swapchainPresentMode) {
            for (size_t i = 0; i < presentModeCount; ++i) {
                if (presentModes[i] == presentMode) {
                    swapchainPresentMode = presentMode;
                    break;
                }
            }
        }
        if (swapchainPresentMode != presentMode) {
            ERR_EXIT("Present mode specified is not supported", "Present mode unsupported");
        }

        // Determine the number of VkImages to use in the swap chain.
        // Application desires to acquire 3 images at a time for triple
        // buffering
        uint32_t desiredNumOfSwapchainImages = 3;
        // At least FRAME_LAG images are required for correct CPU - GPU
        // synchronization
        if (desiredNumOfSwapchainImages < FRAME_LAG) {
            desiredNumOfSwapchainImages = FRAME_LAG;
        }
        if (desiredNumOfSwapchainImages < surfCapabilities.minImageCount) {
            desiredNumOfSwapchainImages = surfCapabilities.minImageCount;
        }
        // If maxImageCount is 0, we can ask for as many images as we want;
        // otherwise we're limited to maxImageCount
        if ((surfCapabilities.maxImageCount > 0) && (desiredNumOfSwapchainImages > surfCapabilities.maxImageCount)) {
            // Application must settle for fewer images than desired:
            desiredNumOfSwapchainImages = surfCapabilities.maxImageCount;
        }

        VkSurfaceTransformFlagBitsKHR preTransform;
        if (surfCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
            preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        } else {
            preTransform = surfCapabilities.currentTransform;
        }

        // Find a supported composite alpha mode - one of these is guaranteed to be set
        VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        VkCompositeAlphaFlagBitsKHR compositeAlphaFlags[4] = {
            VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
            VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
            VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
        };
        for (uint32_t i = 0; i < ARRAY_SIZE(compositeAlphaFlags); i++) {
            if (surfCapabilities.supportedCompositeAlpha & compositeAlphaFlags[i]) {
                compositeAlpha = compositeAlphaFlags[i];
                break;
            }
        }

        VkSwapchainCreateInfoKHR swapchain_ci = {.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
                                                    .pNext = NULL,
                                                    .surface = surface,
                                                    .minImageCount = desiredNumOfSwapchainImages,
                                                    .imageFormat = format,
                                                    .imageColorSpace = color_space,
                                                    .imageExtent =
                                                        {
                                                            .width = swapchainExtent.width,
                                                            .height = swapchainExtent.height,
                                                        },
                                                    .imageArrayLayers = 1,
                                                    .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                                                    .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
                                                    .queueFamilyIndexCount = 0,
                                                    .pQueueFamilyIndices = NULL,
                                                    .preTransform = preTransform,
                                                    .compositeAlpha = compositeAlpha,
                                                    .presentMode = swapchainPresentMode,
                                                    .clipped = true,
                                                    .oldSwapchain = VK_NULL_HANDLE};
        uint32_t i;
        err = pfnCreateSwapchainKHR(device, &swapchain_ci, NULL, &swapchain);
        assert(!err);

        err = pfnGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, NULL);
        assert(!err);
        if (swapchainImageCount > MAX_SWAPCHAIN_IMAGE_COUNT) {
            ERR_EXIT("Swapchain image count exceeds application assumed maximum", "App logic assumption violation");
        }

        VkImage swapchainImages[MAX_SWAPCHAIN_IMAGE_COUNT];
        err = pfnGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, swapchainImages);
        assert(!err);

        for (i = 0; i < swapchainImageCount; i++) {
            VkImageViewCreateInfo color_image_view = {.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                                                        .pNext = NULL,
                                                        .flags = 0,
                                                        .viewType = VK_IMAGE_VIEW_TYPE_2D,
                                                        .format = format,
                                                        .components =
                                                            {
                                                                .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                                                                .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                                                                .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                                                                .a = VK_COMPONENT_SWIZZLE_IDENTITY,
                                                            },
                                                        .subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                                                                            .baseMipLevel = 0,
                                                                            .levelCount = 1,
                                                                            .baseArrayLayer = 0,
                                                                            .layerCount = 1}};

            swapchain_image_resources[i].image = swapchainImages[i];

            color_image_view.image = swapchain_image_resources[i].image;

            err = vkCreateImageView(device, &color_image_view, NULL, &swapchain_image_resources[i].view);
            assert(!err);
        }

        // Create semaphores to synchronize acquiring presentable buffers before
        // rendering and waiting for drawing to be complete before presenting
        VkSemaphoreCreateInfo semaphoreCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
        };

        for (i = 0; i < swapchainImageCount; i++) {
            err = vkCreateSemaphore(device, &semaphoreCreateInfo, NULL, &image_acquired_semaphores[i]);
            assert(!err);

            err = vkCreateSemaphore(device, &semaphoreCreateInfo, NULL, &draw_complete_semaphores[i]);
            assert(!err);

            if (separate_present_queue) {
                err = vkCreateSemaphore(device, &semaphoreCreateInfo, NULL, &image_ownership_semaphores[i]);
                assert(!err);
            }
        }
    }

    VkResult Cube::demo_prepare_offscreen_texture() {
        VkResult U_ASSERT_ONLY err;
        bool U_ASSERT_ONLY pass;

        swapchainImageCount = 1;

        const VkBufferCreateInfo buffer_create_info = {.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                                                        .pNext = NULL,
                                                        .flags = 0,
                                                        .size = (VkDeviceSize)(width * height * 4),
                                                        .usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                                        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
                                                        .queueFamilyIndexCount = 0,
                                                        .pQueueFamilyIndices = NULL};

        err = vkCreateBuffer(device, &buffer_create_info, NULL, &offscreen_texture.staging.buffer);
        assert(!err);

        VkMemoryRequirements mem_reqs;
        vkGetBufferMemoryRequirements(device, offscreen_texture.staging.buffer, &mem_reqs);

        offscreen_texture.staging.mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        offscreen_texture.staging.mem_alloc.pNext = NULL;
        offscreen_texture.staging.mem_alloc.allocationSize = mem_reqs.size;
        offscreen_texture.staging.mem_alloc.memoryTypeIndex = 0;

        VkFlags requirements = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        pass = memory_type_from_properties(mem_reqs.memoryTypeBits, requirements,
                                            &offscreen_texture.staging.mem_alloc.memoryTypeIndex);
        assert(pass);

        err = vkAllocateMemory(device, &offscreen_texture.staging.mem_alloc, NULL, &offscreen_texture.staging.mem);
        assert(!err);

        /* bind memory */
        err = vkBindBufferMemory(device, offscreen_texture.staging.buffer, offscreen_texture.staging.mem, 0);
        assert(!err);

        const VkFormat display_format = VK_FORMAT_R8G8B8A8_UNORM;
        const VkImageCreateInfo image = {.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
                                            .pNext = NULL,
                                            .flags = 0,
                                            .imageType = VK_IMAGE_TYPE_2D,
                                            .format = display_format,
                                            .extent = {(uint32_t)width, (uint32_t)height, (uint32_t)1},
                                            .mipLevels = 1,
                                            .arrayLayers = 1,
                                            .samples = VK_SAMPLE_COUNT_1_BIT,
                                            .tiling = VK_IMAGE_TILING_OPTIMAL,
                                            .usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT};

        VkImageViewCreateInfo view = {.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                                        .pNext = NULL,
                                        .flags = 0,
                                        .image = VK_NULL_HANDLE,
                                        .viewType = VK_IMAGE_VIEW_TYPE_2D,
                                        .format = display_format,
                                        .subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                                                            .baseMipLevel = 0,
                                                            .levelCount = 1,
                                                            .baseArrayLayer = 0,
                                                            .layerCount = 1}};

        format = display_format;

        /* create image */
        err = vkCreateImage(device, &image, NULL, &offscreen_texture.img.image);
        assert(!err);

        vkGetImageMemoryRequirements(device, offscreen_texture.img.image, &mem_reqs);
        assert(!err);

        offscreen_texture.img.mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        offscreen_texture.img.mem_alloc.pNext = NULL;
        offscreen_texture.img.mem_alloc.allocationSize = mem_reqs.size;
        offscreen_texture.img.mem_alloc.memoryTypeIndex = 0;

        pass = memory_type_from_properties(mem_reqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                            &offscreen_texture.img.mem_alloc.memoryTypeIndex);
        assert(pass);

        /* allocate memory */
        err = vkAllocateMemory(device, &offscreen_texture.img.mem_alloc, NULL, &offscreen_texture.img.mem);
        assert(!err);

        /* bind memory */
        err = vkBindImageMemory(device, offscreen_texture.img.image, offscreen_texture.img.mem, 0);
        assert(!err);

        /* create image view */
        view.image = offscreen_texture.img.image;
        err = vkCreateImageView(device, &view, NULL, &offscreen_texture.img.view);
        assert(!err);

        swapchain_image_resources[0].uniform_memory = offscreen_texture.img.mem;
        swapchain_image_resources[0].image = offscreen_texture.img.image;
        swapchain_image_resources[0].view = offscreen_texture.img.view;

        return err;
    }

    void Cube::demo_prepare_depth() {
        const VkFormat depth_format = VK_FORMAT_D16_UNORM;
        const VkImageCreateInfo image = {.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
                                            .pNext = NULL,
                                            .flags = 0,
                                            .imageType = VK_IMAGE_TYPE_2D,
                                            .format = depth_format,
                                            .extent = {(uint32_t)width, (uint32_t)height, (uint32_t)1},
                                            .mipLevels = 1,
                                            .arrayLayers = 1,
                                            .samples = VK_SAMPLE_COUNT_1_BIT,
                                            .tiling = VK_IMAGE_TILING_OPTIMAL,
                                            .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT};

        VkImageViewCreateInfo view_info = {.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                                            .pNext = NULL,
                                            .flags = 0,
                                            .image = VK_NULL_HANDLE,
                                            .viewType = VK_IMAGE_VIEW_TYPE_2D,
                                            .format = depth_format,
                                            .subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
                                                                .baseMipLevel = 0,
                                                                .levelCount = 1,
                                                                .baseArrayLayer = 0,
                                                                .layerCount = 1}};

        if (force_errors) {
            // Intentionally force a bad pNext value to generate a validation layer error
            view_info.pNext = &image;
        }

        VkMemoryRequirements mem_reqs;
        VkResult U_ASSERT_ONLY err;
        bool U_ASSERT_ONLY pass;

        depth.format = depth_format;

        /* create image */
        err = vkCreateImage(device, &image, NULL, &depth.image);
        assert(!err);

        vkGetImageMemoryRequirements(device, depth.image, &mem_reqs);
        assert(!err);

        depth.mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        depth.mem_alloc.pNext = NULL;
        depth.mem_alloc.allocationSize = mem_reqs.size;
        depth.mem_alloc.memoryTypeIndex = 0;

        pass = memory_type_from_properties(mem_reqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                            &depth.mem_alloc.memoryTypeIndex);
        assert(pass);

        /* allocate memory */
        err = vkAllocateMemory(device, &depth.mem_alloc, NULL, &depth.mem);
        assert(!err);

        /* bind memory */
        err = vkBindImageMemory(device, depth.image, depth.mem, 0);
        assert(!err);

        /* create image view */
        view_info.image = depth.image;
        err = vkCreateImageView(device, &view_info, NULL, &depth.view);
        assert(!err);
    }

    void Cube::demo_set_image_layout(VkImage image, VkImageAspectFlags aspectMask, VkImageLayout old_image_layout,
                                VkImageLayout new_image_layout, VkAccessFlagBits srcAccessMask, VkPipelineStageFlags src_stages,
                                VkPipelineStageFlags dest_stages) {
        assert(cmd);

        VkImageMemoryBarrier image_memory_barrier = {.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                                                        .pNext = NULL,
                                                        .srcAccessMask = srcAccessMask,
                                                        .dstAccessMask = 0,
                                                        .oldLayout = old_image_layout,
                                                        .newLayout = new_image_layout,
                                                        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                                        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                                        .image = image,
                                                        .subresourceRange = {aspectMask, 0, 1, 0, 1}};

        switch (new_image_layout) {
            case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
                /* Make sure anything that was copying from this image has completed */
                image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                break;

            case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
                image_memory_barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                break;

            case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
                image_memory_barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                break;

            case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
                image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
                break;

            case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
                image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                break;

            case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
                image_memory_barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
                break;

            default:
                image_memory_barrier.dstAccessMask = 0;
                break;
        }

        VkImageMemoryBarrier *pmemory_barrier = &image_memory_barrier;

        vkCmdPipelineBarrier(cmd, src_stages, dest_stages, 0, 0, NULL, 0, NULL, 1, pmemory_barrier);
    }

    void Cube::demo_prepare_textures() {
        const VkFormat tex_format = VK_FORMAT_R8G8B8A8_UNORM;
        VkFormatProperties props;
        uint32_t i;

        vkGetPhysicalDeviceFormatProperties(gpu, tex_format, &props);

        for (i = 0; i < DEMO_TEXTURE_COUNT; i++) {
            VkResult U_ASSERT_ONLY err;

            if ((props.linearTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) && !use_staging_buffer) {
                /* Device can texture using linear textures */
                demo_prepare_texture_image(tex_files[i], &textures[i], VK_IMAGE_TILING_LINEAR, VK_IMAGE_USAGE_SAMPLED_BIT,
                                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
                // Nothing in the pipeline needs to be complete to start, and don't allow fragment
                // shader to run until layout transition completes
                demo_set_image_layout(textures[i].image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED,
                                        textures[i].imageLayout, VK_ACCESS_NONE, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
                staging_texture.image = 0;
            } else if (props.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) {
                /* Must use staging buffer to copy linear texture to optimized */

                memset(&staging_texture, 0, sizeof(staging_texture));
                demo_prepare_texture_buffer(tex_files[i], &staging_texture);

                demo_prepare_texture_image(tex_files[i], &textures[i], VK_IMAGE_TILING_OPTIMAL,
                                            (VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT),
                                            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

                demo_set_image_layout(textures[i].image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED,
                                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_ACCESS_NONE, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                        VK_PIPELINE_STAGE_TRANSFER_BIT);

                VkBufferImageCopy copy_region = {
                    .bufferOffset = 0,
                    .bufferRowLength = (uint32_t)staging_texture.tex_width,
                    .bufferImageHeight = (uint32_t)staging_texture.tex_height,
                    .imageSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1},
                    .imageOffset = {0, 0, 0},
                    .imageExtent = {(uint32_t)staging_texture.tex_width, (uint32_t)staging_texture.tex_height, (uint32_t)1},
                };

                vkCmdCopyBufferToImage(cmd, staging_texture.buffer, textures[i].image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
                                        &copy_region);

                demo_set_image_layout(textures[i].image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                        textures[i].imageLayout, VK_ACCESS_TRANSFER_WRITE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
                                        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
            } else {
                /* Can't support VK_FORMAT_R8G8B8A8_UNORM !? */
                assert(!"No support for R8G8B8A8_UNORM as texture image format");
            }

            const VkSamplerCreateInfo sampler = {
                .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
                .pNext = NULL,
                .magFilter = VK_FILTER_NEAREST,
                .minFilter = VK_FILTER_NEAREST,
                .mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST,
                .addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
                .addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
                .addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
                .mipLodBias = 0.0f,
                .anisotropyEnable = VK_FALSE,
                .maxAnisotropy = 1,
                .compareOp = VK_COMPARE_OP_NEVER,
                .minLod = 0.0f,
                .maxLod = 0.0f,
                .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
                .unnormalizedCoordinates = VK_FALSE,
            };

            VkImageViewCreateInfo view = {.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                                            .pNext = NULL,
                                            .flags = 0,
                                            .image = VK_NULL_HANDLE,
                                            .viewType = VK_IMAGE_VIEW_TYPE_2D,
                                            .format = tex_format,
                                            .components =
                                                {
                                                    VK_COMPONENT_SWIZZLE_IDENTITY,
                                                    VK_COMPONENT_SWIZZLE_IDENTITY,
                                                    VK_COMPONENT_SWIZZLE_IDENTITY,
                                                    VK_COMPONENT_SWIZZLE_IDENTITY,
                                                },
                                            .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}};

            /* create sampler */
            err = vkCreateSampler(device, &sampler, NULL, &textures[i].sampler);
            assert(!err);

            /* create image view */
            view.image = textures[i].image;
            err = vkCreateImageView(device, &view, NULL, &textures[i].view);
            err = vkCreateImageView(device, &view, NULL, &textures[i].view);
            assert(!err);
        }
    }

    void Cube::demo_prepare_descriptor_layout() {
        const VkDescriptorSetLayoutBinding layout_bindings[2] = {
            [0] =
                {
                    .binding = 0,
                    .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                    .descriptorCount = 1,
                    .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                    .pImmutableSamplers = NULL,
                },
            [1] =
                {
                    .binding = 1,
                    .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                    .descriptorCount = DEMO_TEXTURE_COUNT,
                    .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                    .pImmutableSamplers = NULL,
                },
        };
        const VkDescriptorSetLayoutCreateInfo descriptor_layout = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .pNext = NULL,
            .bindingCount = 2,
            .pBindings = layout_bindings,
        };
        VkResult U_ASSERT_ONLY err;

        err = vkCreateDescriptorSetLayout(device, &descriptor_layout, NULL, &desc_layout);
        assert(!err);

        const VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext = NULL,
            .setLayoutCount = 1,
            .pSetLayouts = &desc_layout,
        };

        err = vkCreatePipelineLayout(device, &pPipelineLayoutCreateInfo, NULL, &pipeline_layout);
        assert(!err);
    }

    void Cube::demo_prepare_render_pass() {
        // The initial layout for the color and depth attachments will be LAYOUT_UNDEFINED
        // because at the start of the renderpass, we don't care about their contents.
        // At the start of the subpass, the color attachment's layout will be transitioned
        // to LAYOUT_COLOR_ATTACHMENT_OPTIMAL and the depth stencil attachment's layout
        // will be transitioned to LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL.  At the end of
        // the renderpass, the color attachment's layout will be transitioned to
        // LAYOUT_PRESENT_SRC_KHR or LAYOUT_TRANSFER_SRC_OPTIMAL depending on it is
        // presented or saved to file. This is all done as part of the renderpass.
        VkImageLayout final_layout =
            (wsi_platform != WSI_PLATFORM_FILE) ? VK_IMAGE_LAYOUT_PRESENT_SRC_KHR : VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        const VkAttachmentDescription attachments[2] = {
            [0] =
                {
                    .flags = 0,
                    .format = format,
                    .samples = VK_SAMPLE_COUNT_1_BIT,
                    .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                    .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                    .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                    .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                    .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                    .finalLayout = final_layout,
                },
            [1] =
                {
                    .flags = 0,
                    .format = depth.format,
                    .samples = VK_SAMPLE_COUNT_1_BIT,
                    .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                    .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                    .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                    .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                    .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                    .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                },
        };
        const VkAttachmentReference color_reference = {
            .attachment = 0,
            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        };
        const VkAttachmentReference depth_reference = {
            .attachment = 1,
            .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        };
        const VkSubpassDescription subpass = {
            .flags = 0,
            .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
            .inputAttachmentCount = 0,
            .pInputAttachments = NULL,
            .colorAttachmentCount = 1,
            .pColorAttachments = &color_reference,
            .pResolveAttachments = NULL,
            .pDepthStencilAttachment = &depth_reference,
            .preserveAttachmentCount = 0,
            .pPreserveAttachments = NULL,
        };

        VkSubpassDependency attachmentDependencies[2] = {
            [0] =
                {
                    // Depth buffer is shared between swapchain images
                    .srcSubpass = VK_SUBPASS_EXTERNAL,
                    .dstSubpass = 0,
                    .srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
                    .dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
                    .srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                    .dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                    .dependencyFlags = 0,
                },
            [1] =
                {
                    // Image Layout Transition
                    .srcSubpass = VK_SUBPASS_EXTERNAL,
                    .dstSubpass = 0,
                    .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                    .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                    .srcAccessMask = 0,
                    .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
                    .dependencyFlags = 0,
                },
        };

        const VkRenderPassCreateInfo rp_info = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .attachmentCount = 2,
            .pAttachments = attachments,
            .subpassCount = 1,
            .pSubpasses = &subpass,
            .dependencyCount = 2,
            .pDependencies = attachmentDependencies,
        };
        VkResult U_ASSERT_ONLY err;

        err = vkCreateRenderPass(device, &rp_info, NULL, &render_pass);
        assert(!err);
    }

    VkShaderModule Cube::demo_prepare_shader_module(const char *, const uint32_t *code, size_t size) {
        VkShaderModule module;
        VkShaderModuleCreateInfo moduleCreateInfo;
        VkResult U_ASSERT_ONLY err;

        moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        moduleCreateInfo.pNext = NULL;
        moduleCreateInfo.flags = 0;
        moduleCreateInfo.codeSize = size;
        moduleCreateInfo.pCode = code;

        err = vkCreateShaderModule(device, &moduleCreateInfo, NULL, &module);
        assert(!err);

        return module;
    }

    void Cube::demo_prepare_vs() {
        const uint32_t vs_code[] = {
#include "cube.vert.inc"
        };
        vert_shader_module = demo_prepare_shader_module("cube.vert", vs_code, sizeof(vs_code));
    }

    void Cube::demo_prepare_fs() {
        const uint32_t fs_code[] = {
#include "cube.frag.inc"
        };
        frag_shader_module = demo_prepare_shader_module("cube.frag", fs_code, sizeof(fs_code));
    }

    void Cube::demo_prepare_pipeline() {
#define NUM_DYNAMIC_STATES 2 /*Viewport + Scissor*/

        VkGraphicsPipelineCreateInfo pipeline_info;
        VkPipelineCacheCreateInfo pipelineCache;
        VkPipelineVertexInputStateCreateInfo vi;
        VkPipelineInputAssemblyStateCreateInfo ia;
        VkPipelineRasterizationStateCreateInfo rs;
        VkPipelineColorBlendStateCreateInfo cb;
        VkPipelineDepthStencilStateCreateInfo ds;
        VkPipelineViewportStateCreateInfo vp;
        VkPipelineMultisampleStateCreateInfo ms;
        VkDynamicState dynamicStateEnables[NUM_DYNAMIC_STATES];
        VkPipelineDynamicStateCreateInfo dynamicState;
        VkResult U_ASSERT_ONLY err;

        memset(dynamicStateEnables, 0, sizeof dynamicStateEnables);
        memset(&dynamicState, 0, sizeof dynamicState);
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.pDynamicStates = dynamicStateEnables;

        memset(&pipeline_info, 0, sizeof(pipeline_info));
        pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipeline_info.layout = pipeline_layout;

        memset(&vi, 0, sizeof(vi));
        vi.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

        memset(&ia, 0, sizeof(ia));
        ia.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        ia.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

        memset(&rs, 0, sizeof(rs));
        rs.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rs.polygonMode = VK_POLYGON_MODE_FILL;
        rs.cullMode = VK_CULL_MODE_BACK_BIT;
        rs.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rs.depthClampEnable = VK_FALSE;
        rs.rasterizerDiscardEnable = VK_FALSE;
        rs.depthBiasEnable = VK_FALSE;
        rs.lineWidth = 1.0f;

        memset(&cb, 0, sizeof(cb));
        cb.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        VkPipelineColorBlendAttachmentState att_state[1];
        memset(att_state, 0, sizeof(att_state));
        att_state[0].colorWriteMask = 0xf;
        att_state[0].blendEnable = VK_FALSE;
        cb.attachmentCount = 1;
        cb.pAttachments = att_state;

        memset(&vp, 0, sizeof(vp));
        vp.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        vp.viewportCount = 1;
        dynamicStateEnables[dynamicState.dynamicStateCount++] = VK_DYNAMIC_STATE_VIEWPORT;
        vp.scissorCount = 1;
        dynamicStateEnables[dynamicState.dynamicStateCount++] = VK_DYNAMIC_STATE_SCISSOR;

        memset(&ds, 0, sizeof(ds));
        ds.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        ds.depthTestEnable = VK_TRUE;
        ds.depthWriteEnable = VK_TRUE;
        ds.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
        ds.depthBoundsTestEnable = VK_FALSE;
        ds.back.failOp = VK_STENCIL_OP_KEEP;
        ds.back.passOp = VK_STENCIL_OP_KEEP;
        ds.back.compareOp = VK_COMPARE_OP_ALWAYS;
        ds.stencilTestEnable = VK_FALSE;
        ds.front = ds.back;

        memset(&ms, 0, sizeof(ms));
        ms.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        ms.pSampleMask = NULL;
        ms.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        demo_prepare_vs();
        demo_prepare_fs();

        // Two stages: vs and fs
        VkPipelineShaderStageCreateInfo shaderStages[2];
        memset(&shaderStages, 0, 2 * sizeof(VkPipelineShaderStageCreateInfo));

        shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shaderStages[0].module = vert_shader_module;
        shaderStages[0].pName = "main";

        shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shaderStages[1].module = frag_shader_module;
        shaderStages[1].pName = "main";

        memset(&pipelineCache, 0, sizeof(pipelineCache));
        pipelineCache.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;

        err = vkCreatePipelineCache(device, &pipelineCache, NULL, &pipeline_cache);
        assert(!err);

        pipeline_info.pVertexInputState = &vi;
        pipeline_info.pInputAssemblyState = &ia;
        pipeline_info.pRasterizationState = &rs;
        pipeline_info.pColorBlendState = &cb;
        pipeline_info.pMultisampleState = &ms;
        pipeline_info.pViewportState = &vp;
        pipeline_info.pDepthStencilState = &ds;
        pipeline_info.stageCount = ARRAY_SIZE(shaderStages);
        pipeline_info.pStages = shaderStages;
        pipeline_info.renderPass = render_pass;
        pipeline_info.pDynamicState = &dynamicState;

        err = vkCreateGraphicsPipelines(device, pipeline_cache, 1, &pipeline_info, NULL, &pipeline);
        assert(!err);

        vkDestroyShaderModule(device, frag_shader_module, NULL);
        vkDestroyShaderModule(device, vert_shader_module, NULL);
    }

    void Cube::demo_prepare_descriptor_pool() {
        const VkDescriptorPoolSize type_counts[2] = {
            [0] =
                {
                    .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                    .descriptorCount = FRAME_LAG,
                },
            [1] =
                {
                    .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                    .descriptorCount = FRAME_LAG * DEMO_TEXTURE_COUNT,
                },
        };
        const VkDescriptorPoolCreateInfo descriptor_pool = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .pNext = NULL,
            .maxSets = FRAME_LAG,
            .poolSizeCount = 2,
            .pPoolSizes = type_counts,
        };
        VkResult U_ASSERT_ONLY err;

        err = vkCreateDescriptorPool(device, &descriptor_pool, NULL, &desc_pool);
        assert(!err);
    }

    void Cube::demo_prepare_descriptor_set() {
        VkDescriptorImageInfo tex_descs[DEMO_TEXTURE_COUNT];
        VkWriteDescriptorSet writes[2];
        VkResult U_ASSERT_ONLY err;

        VkDescriptorSetAllocateInfo alloc_info = {.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
                                                    .pNext = NULL,
                                                    .descriptorPool = desc_pool,
                                                    .descriptorSetCount = 1,
                                                    .pSetLayouts = &desc_layout};

        VkDescriptorBufferInfo buffer_info;
        buffer_info.offset = 0;
        buffer_info.range = sizeof(struct vktexcube_vs_uniform);

        memset(&tex_descs, 0, sizeof(tex_descs));
        for (unsigned int i = 0; i < DEMO_TEXTURE_COUNT; i++) {
            tex_descs[i].sampler = textures[i].sampler;
            tex_descs[i].imageView = textures[i].view;
            tex_descs[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        }

        memset(&writes, 0, sizeof(writes));

        writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writes[0].descriptorCount = 1;
        writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        writes[0].pBufferInfo = &buffer_info;

        writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writes[1].dstBinding = 1;
        writes[1].descriptorCount = DEMO_TEXTURE_COUNT;
        writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        writes[1].pImageInfo = tex_descs;

        for (unsigned int i = 0; i < swapchainImageCount; i++) {
            err = vkAllocateDescriptorSets(device, &alloc_info, &swapchain_image_resources[i].descriptor_set);
            assert(!err);
            buffer_info.buffer = swapchain_image_resources[i].uniform_buffer;
            writes[0].dstSet = swapchain_image_resources[i].descriptor_set;
            writes[1].dstSet = swapchain_image_resources[i].descriptor_set;
            vkUpdateDescriptorSets(device, 2, writes, 0, NULL);
        }
    }

    void Cube::demo_prepare_framebuffers() {
        VkImageView attachments[2];
        attachments[1] = depth.view;

        const VkFramebufferCreateInfo fb_info = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext = NULL,
            .renderPass = render_pass,
            .attachmentCount = 2,
            .pAttachments = attachments,
            .width = (uint32_t)width,
            .height = (uint32_t)height,
            .layers = 1,
        };
        VkResult U_ASSERT_ONLY err;
        uint32_t i;

        for (i = 0; i < swapchainImageCount; i++) {
            attachments[0] = swapchain_image_resources[i].view;
            err = vkCreateFramebuffer(device, &fb_info, NULL, &swapchain_image_resources[i].framebuffer);
            assert(!err);
        }
    }

    void Cube::demo_build_image_ownership_cmd(int i) {
        VkResult U_ASSERT_ONLY err;

        const VkCommandBufferBeginInfo cmd_buf_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = NULL,
            .flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
            .pInheritanceInfo = NULL,
        };
        err = vkBeginCommandBuffer(swapchain_image_resources[i].graphics_to_present_cmd, &cmd_buf_info);
        assert(!err);

        VkImageMemoryBarrier image_ownership_barrier = {.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                                                        .pNext = NULL,
                                                        .srcAccessMask = 0,
                                                        .dstAccessMask = 0,
                                                        .oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                                        .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                                        .srcQueueFamilyIndex = graphics_queue_family_index,
                                                        .dstQueueFamilyIndex = present_queue_family_index,
                                                        .image = swapchain_image_resources[i].image,
                                                        .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}};

        vkCmdPipelineBarrier(swapchain_image_resources[i].graphics_to_present_cmd, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                                VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, NULL, 0, NULL, 1, &image_ownership_barrier);
        err = vkEndCommandBuffer(swapchain_image_resources[i].graphics_to_present_cmd);
        assert(!err);
    }

    void Cube::demo_draw_build_cmd(VkCommandBuffer cmd_buf) {
        const VkCommandBufferBeginInfo cmd_buf_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = NULL,
            .flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
            .pInheritanceInfo = NULL,
        };
        VkClearValue clear_values[2];
        clear_values[0].color.float32[0] = 0.2f;
        clear_values[0].color.float32[1] = 0.2f;
        clear_values[0].color.float32[2] = 0.2f;
        clear_values[0].color.float32[3] = 0.2f;
        clear_values[1].depthStencil = {1.0f, 0};
        const VkRenderPassBeginInfo rp_begin = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .pNext = NULL,
            .renderPass = render_pass,
            .framebuffer = swapchain_image_resources[current_buffer].framebuffer,
            .renderArea = VkRect2D{.offset = VkOffset2D{0, 0}, .extent = VkExtent2D{(uint32_t)width, (uint32_t)height}},
            .clearValueCount = 2,
            .pClearValues = clear_values,
        };
        VkResult U_ASSERT_ONLY err;

        err = vkBeginCommandBuffer(cmd_buf, &cmd_buf_info);

        assert(!err);
        vkCmdBeginRenderPass(cmd_buf, &rp_begin, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
        vkCmdBindDescriptorSets(cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout, 0, 1,
                                &swapchain_image_resources[current_buffer].descriptor_set, 0, NULL);
        VkViewport viewport;
        memset(&viewport, 0, sizeof(viewport));
        float viewport_dimension;
        if (width < height) {
            viewport_dimension = (float)width;
            viewport.y = (height - width) / 2.0f;
        } else {
            viewport_dimension = (float)height;
            viewport.x = (width - height) / 2.0f;
        }
        viewport.height = viewport_dimension;
        viewport.width = viewport_dimension;
        viewport.minDepth = (float)0.0f;
        viewport.maxDepth = (float)1.0f;
        vkCmdSetViewport(cmd_buf, 0, 1, &viewport);

        VkRect2D scissor;
        memset(&scissor, 0, sizeof(scissor));
        scissor.extent.width = width;
        scissor.extent.height = height;
        scissor.offset.x = 0;
        scissor.offset.y = 0;
        vkCmdSetScissor(cmd_buf, 0, 1, &scissor);

        vkCmdDraw(cmd_buf, 12 * 3, 1, 0, 0);

        // Note that ending the renderpass changes the image's layout from
        // COLOR_ATTACHMENT_OPTIMAL to PRESENT_SRC_KHR
        vkCmdEndRenderPass(cmd_buf);

        if (separate_present_queue) {
            // We have to transfer ownership from the graphics queue family to the
            // present queue family to be able to present.  Note that we don't have
            // to transfer from present queue family back to graphics queue family at
            // the start of the next frame because we don't care about the image's
            // contents at that point.
            VkImageMemoryBarrier image_ownership_barrier = {.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                                                            .pNext = NULL,
                                                            .srcAccessMask = 0,
                                                            .dstAccessMask = 0,
                                                            .oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                                            .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                                            .srcQueueFamilyIndex = graphics_queue_family_index,
                                                            .dstQueueFamilyIndex = present_queue_family_index,
                                                            .image = swapchain_image_resources[current_buffer].image,
                                                            .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}};

            vkCmdPipelineBarrier(cmd_buf, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0,
                                    NULL, 0, NULL, 1, &image_ownership_barrier);
        }

        if (wsi_platform == WSI_PLATFORM_FILE) {
            VkImageMemoryBarrier image_wait_barrier = {.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                                                        .pNext = NULL,
                                                        .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                                                        .dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT,
                                                        .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                                                        .newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                                                        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                                        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                                        .image = offscreen_texture.img.image,
                                                        .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}};

            vkCmdPipelineBarrier(cmd_buf, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0,
                                    NULL, 1, &image_wait_barrier);

            VkBufferImageCopy copy_region = {
                .bufferOffset = 0,
                .bufferRowLength = (uint32_t)width,
                .bufferImageHeight = (uint32_t)height,
                .imageSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1},
                .imageOffset = {0, 0, 0},
                .imageExtent = {(uint32_t)width, (uint32_t)height, (uint32_t)1},
            };

            vkCmdCopyImageToBuffer(cmd_buf, offscreen_texture.img.image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                                    offscreen_texture.staging.buffer, 1, &copy_region);
        }

        err = vkEndCommandBuffer(cmd_buf);
        assert(!err);
    }

    void Cube::demo_flush_init_cmd() {
        VkResult U_ASSERT_ONLY err;

        // This function could get called twice if the texture uses a staging buffer
        // In that case the second call should be ignored
        if (cmd == VK_NULL_HANDLE) return;

        err = vkEndCommandBuffer(cmd);
        assert(!err);

        VkFence fence;
        VkFenceCreateInfo fence_ci = {.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, .pNext = NULL, .flags = 0};
        err = vkCreateFence(device, &fence_ci, NULL, &fence);
        assert(!err);

        const VkCommandBuffer cmd_bufs[] = {cmd};
        VkSubmitInfo submit_info = {.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                                    .pNext = NULL,
                                    .waitSemaphoreCount = 0,
                                    .pWaitSemaphores = NULL,
                                    .pWaitDstStageMask = NULL,
                                    .commandBufferCount = 1,
                                    .pCommandBuffers = cmd_bufs,
                                    .signalSemaphoreCount = 0,
                                    .pSignalSemaphores = NULL};

        err = vkQueueSubmit(graphics_queue, 1, &submit_info, fence);
        assert(!err);

        err = vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX);
        assert(!err);

        vkFreeCommandBuffers(device, cmd_pool, 1, cmd_bufs);
        vkDestroyFence(device, fence, NULL);
        cmd = VK_NULL_HANDLE;
    }

    void Cube::demo_draw() {
        VkResult U_ASSERT_ONLY err;

        // Ensure no more than FRAME_LAG renderings are outstanding
        vkWaitForFences(device, 1, &fences[frame_index], VK_TRUE, UINT64_MAX);
        vkResetFences(device, 1, &fences[frame_index]);

        if (wsi_platform != WSI_PLATFORM_FILE) {
            do {
                // Get the index of the next available swapchain image:
                err = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, image_acquired_semaphores[image_index],
                                            VK_NULL_HANDLE, &current_buffer);

                if (err == VK_ERROR_OUT_OF_DATE_KHR) {
                    // swapchain is out of date (e.g. the window was resized) and
                    // must be recreated:
                    demo_resize();
                } else if (err == VK_SUBOPTIMAL_KHR) {
                    // swapchain is not as optimal as it could be, but the platform's
                    // presentation engine will still present the image correctly.
                    break;
                } else if (err == VK_ERROR_SURFACE_LOST_KHR) {
                    vkDestroySurfaceKHR(inst, surface, NULL);
                    demo_create_surface();
                    demo_resize();
                } else {
                    assert(!err);
                }
            } while (err != VK_SUCCESS);
        }

        demo_update_data_buffer();

        // Wait for the image acquired semaphore to be signaled to ensure
        // that the image won't be rendered to until the presentation
        // engine has fully released ownership to the application, and it is
        // okay to render to the image.
        // In case of saving the rendered images to file the graphics queue
        // is fully executed after every frame, so we do not have to synchronize
        // the graphics queue with the present queue
        VkPipelineStageFlags pipe_stage_flags;
        VkSubmitInfo submit_info;
        uint32_t semaphore_count = (wsi_platform != WSI_PLATFORM_FILE) ? 1 : 0;
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.pNext = NULL;
        pipe_stage_flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        submit_info.pWaitDstStageMask = &pipe_stage_flags;
        submit_info.waitSemaphoreCount = semaphore_count;
        submit_info.pWaitSemaphores = &image_acquired_semaphores[image_index];
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &swapchain_image_resources[current_buffer].cmd;
        submit_info.signalSemaphoreCount = semaphore_count;
        submit_info.pSignalSemaphores = &draw_complete_semaphores[image_index];
        err = vkQueueSubmit(graphics_queue, 1, &submit_info, fences[frame_index]);
        assert(!err);

        if (wsi_platform == WSI_PLATFORM_FILE) {
            // In case of image dump instead of present we wait for graphics queue execution
            // to safely read back the offscreen backbuffer
            vkQueueWaitIdle(graphics_queue);
            return;
        }

        if (separate_present_queue) {
            // If we are using separate queues, change image ownership to the
            // present queue before presenting, waiting for the draw complete
            // semaphore and signalling the ownership released semaphore when finished
            VkFence nullFence = VK_NULL_HANDLE;
            pipe_stage_flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            submit_info.waitSemaphoreCount = 1;
            submit_info.pWaitSemaphores = &draw_complete_semaphores[image_index];
            submit_info.commandBufferCount = 1;
            submit_info.pCommandBuffers = &swapchain_image_resources[current_buffer].graphics_to_present_cmd;
            submit_info.signalSemaphoreCount = 1;
            submit_info.pSignalSemaphores = &image_ownership_semaphores[image_index];
            err = vkQueueSubmit(present_queue, 1, &submit_info, nullFence);
            assert(!err);
        }

        // If we are using separate queues we have to wait for image ownership,
        // otherwise wait for draw complete
        VkPresentInfoKHR present = {
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .pNext = NULL,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores =
                (separate_present_queue) ? &image_ownership_semaphores[image_index] : &draw_complete_semaphores[image_index],
            .swapchainCount = 1,
            .pSwapchains = &swapchain,
            .pImageIndices = &current_buffer,
        };

        VkRectLayerKHR rect;
        VkPresentRegionKHR region;
        VkPresentRegionsKHR regions;
        if (VK_KHR_incremental_present_enabled) {
            // If using VK_KHR_incremental_present, we provide a hint of the region
            // that contains changed content relative to the previously-presented
            // image.  The implementation can use this hint in order to save
            // work/power (by only copying the region in the hint).  The
            // implementation is free to ignore the hint though, and so we must
            // ensure that the entire image has the correctly-drawn content.
            uint32_t eighthOfWidth = width / 8;
            uint32_t eighthOfHeight = height / 8;

            if (first_swapchain_frame) {
                rect.offset.x = 0;
                rect.offset.y = 0;
                rect.extent.width = width;
                rect.extent.height = height;
            } else {
                rect.offset.x = eighthOfWidth;
                rect.offset.y = eighthOfHeight;
                rect.extent.width = eighthOfWidth * 6;
                rect.extent.height = eighthOfHeight * 6;
            }
            rect.layer = 0;

            region.rectangleCount = 1;
            region.pRectangles = &rect;

            regions.sType = VK_STRUCTURE_TYPE_PRESENT_REGIONS_KHR;
            regions.pNext = present.pNext;
            regions.swapchainCount = present.swapchainCount;
            regions.pRegions = &region;
            present.pNext = &regions;
        }

        err = vkQueuePresentKHR(present_queue, &present);
        frame_index += 1;
        frame_index %= FRAME_LAG;
        image_index += 1;
        image_index %= swapchainImageCount;
        first_swapchain_frame = false;

        if (err == VK_ERROR_OUT_OF_DATE_KHR) {
            // swapchain is out of date (e.g. the window was resized) and
            // must be recreated:
            demo_resize();
        } else if (err == VK_SUBOPTIMAL_KHR) {
            // SUBOPTIMAL could be due to a resize
            VkSurfaceCapabilitiesKHR surfCapabilities;
            err = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, surface, &surfCapabilities);
            if (err != VK_SUCCESS) {
                ERR_EXIT("Failed to get surface capabilities", "vkGetPhysicalDeviceSurfaceCapabilitiesKHR Failure");
            }
            if (surfCapabilities.currentExtent.width != (uint32_t)width ||
                surfCapabilities.currentExtent.height != (uint32_t)height) {
                demo_resize();
            }
        } else if (err == VK_ERROR_SURFACE_LOST_KHR) {
            vkDestroySurfaceKHR(inst, surface, NULL);
            demo_create_surface();
            demo_resize();
        } else {
            assert(!err);
        }
    }

    void Cube::demo_update_data_buffer() {
        mat4x4 MVP, Model, VP;
        int matrixSize = sizeof(MVP);

        mat4x4_mul(VP, projection_matrix, view_matrix);

        // Rotate around the Y axis
        mat4x4_dup(Model, model_matrix);
        mat4x4_rotate_Y(model_matrix, Model, (float)degreesToRadians(spin_angle));
        mat4x4_orthonormalize(model_matrix, model_matrix);
        mat4x4_mul(MVP, VP, model_matrix);

        memcpy(swapchain_image_resources[current_buffer].uniform_memory_ptr, (const void *)&MVP[0][0], matrixSize);
    }

    void Cube::demo_resize() {
        ERR_EXIT("We don't ever expect to get resize events in the supported WSI platforms", "Unexpected error");
    }

    void Cube::demo_run_image_dump() {
        for (int32_t i = 0; i < frameCount; i++) {
            demo_draw();
            save_frame(i);
        }
    }

    void Cube::demo_run_display() {
        while (!quit) {
            demo_draw();
            curFrame++;

            if (frameCount != INT32_MAX && curFrame == frameCount) {
                quit = true;
            }
        }
    }

    void Cube::demo_create_surface() {
        VkResult U_ASSERT_ONLY err = VK_SUCCESS;

// Create a WSI surface for the window:
#if defined(VK_USE_PLATFORM_DISPLAY_KHR)
        if (wsi_platform == WSI_PLATFORM_DISPLAY) {
            err = demo_create_display_surface();
        }
#endif
        if (err != VK_SUCCESS) {
            ERR_EXIT("Could not create display surface", "Surface Initialization Failure");
        }
    }

    VkResult Cube::demo_create_display_surface() {
#if defined(VK_USE_PLATFORM_DISPLAY_KHR)
        VkResult U_ASSERT_ONLY err;
        uint32_t display_count;
        uint32_t mode_count;
        uint32_t plane_count;
        VkDisplayPropertiesKHR display_props;
        VkDisplayModeKHR mode;
        VkDisplayModePropertiesKHR mode_props;
        VkDisplayPlanePropertiesKHR plane_props[MAX_DISPLAY_PLANE_COUNT];
        VkBool32 found_plane = VK_FALSE;
        uint32_t plane_index;
        VkExtent2D image_extent;
        VkDisplaySurfaceCreateInfoKHR create_info;

        // Get the first display
        display_count = 1;
        err = vkGetPhysicalDeviceDisplayPropertiesKHR(gpu, &display_count, &display_props);
        assert(!err || (err == VK_INCOMPLETE));

        display = display_props.display;

#ifdef VK_USE_PLATFORM_WIN32_KHR
        // If we can, and need to, acquire the display if supported
        if (VK_NV_acquire_winrt_display_supported && wsi_platform != WSI_PLATFORM_DISPLAY) {
            err = pfnAcquireWinrtDisplayNV(gpu, display);
            if (err != VK_SUCCESS) {
                ERR_EXIT("Failed to get acqurie display", "vkAcquireWinrtDisplayNV Failure");
            }
        }
#endif

        // Get the first mode of the display
        err = vkGetDisplayModePropertiesKHR(gpu, display, &mode_count, NULL);
        assert(!err);

        if (mode_count == 0) {
            printf("Cannot find any mode for the display!\n");
            fflush(stdout);
            exit(1);
        }

        mode_count = 1;
        err = vkGetDisplayModePropertiesKHR(gpu, display, &mode_count, &mode_props);
        assert(!err || (err == VK_INCOMPLETE));
        if (width != -1 && height != -1) {
            VkDisplayModeCreateInfoKHR mode_create_info;
            mode_create_info.sType = VK_STRUCTURE_TYPE_DISPLAY_MODE_CREATE_INFO_KHR;
            mode_create_info.pNext = NULL;
            mode_create_info.flags = 0;
            mode_create_info.parameters.visibleRegion.width = (uint32_t)width;
            mode_create_info.parameters.visibleRegion.height = (uint32_t)height;
            mode_create_info.parameters.refreshRate = mode_props.parameters.refreshRate;
            err = vkCreateDisplayModeKHR(gpu, display, &mode_create_info, NULL, &mode);
            assert(!err);
        } else {
            mode = mode_props.displayMode;
            width = mode_props.parameters.visibleRegion.width;
            height = mode_props.parameters.visibleRegion.height;
        }

        // Get the list of planes
        err = vkGetPhysicalDeviceDisplayPlanePropertiesKHR(gpu, &plane_count, NULL);
        assert(!err);

        if (plane_count == 0) {
            printf("Cannot find any plane!\n");
            fflush(stdout);
            exit(1);
        }

        err = vkGetPhysicalDeviceDisplayPlanePropertiesKHR(gpu, &plane_count, plane_props);
        assert(!err);

        // Find a plane compatible with the display
        for (plane_index = 0; plane_index < plane_count; plane_index++) {
            uint32_t supported_count;
            VkDisplayKHR supported_displays[MAX_DISPLAY_COUNT];

            // Disqualify planes that are bound to a different display
            if ((plane_props[plane_index].currentDisplay != VK_NULL_HANDLE) &&
                (plane_props[plane_index].currentDisplay != display)) {
                continue;
            }

            err = vkGetDisplayPlaneSupportedDisplaysKHR(gpu, plane_index, &supported_count, NULL);
            assert(!err);

            if (supported_count == 0) {
                continue;
            }

            err = vkGetDisplayPlaneSupportedDisplaysKHR(gpu, plane_index, &supported_count, supported_displays);
            assert(!err);

            for (uint32_t i = 0; i < supported_count; i++) {
                if (supported_displays[i] == display) {
                    found_plane = VK_TRUE;
                    break;
                }
            }

            if (found_plane) {
                break;
            }
        }

        if (!found_plane) {
            printf("Cannot find a plane compatible with the display!\n");
            fflush(stdout);
            exit(1);
        }

        VkDisplayPlaneCapabilitiesKHR planeCaps;
        vkGetDisplayPlaneCapabilitiesKHR(gpu, mode_props.displayMode, plane_index, &planeCaps);
        // Find a supported alpha mode
        VkDisplayPlaneAlphaFlagBitsKHR alphaMode = VK_DISPLAY_PLANE_ALPHA_OPAQUE_BIT_KHR;
        VkDisplayPlaneAlphaFlagBitsKHR alphaModes[4] = {
            VK_DISPLAY_PLANE_ALPHA_OPAQUE_BIT_KHR,
            VK_DISPLAY_PLANE_ALPHA_GLOBAL_BIT_KHR,
            VK_DISPLAY_PLANE_ALPHA_PER_PIXEL_BIT_KHR,
            VK_DISPLAY_PLANE_ALPHA_PER_PIXEL_PREMULTIPLIED_BIT_KHR,
        };
        for (uint32_t i = 0; i < sizeof(alphaModes); i++) {
            if (planeCaps.supportedAlpha & alphaModes[i]) {
                alphaMode = alphaModes[i];
                break;
            }
        }
        image_extent.width = (uint32_t)width;
        image_extent.height = (uint32_t)height;

        create_info.sType = VK_STRUCTURE_TYPE_DISPLAY_SURFACE_CREATE_INFO_KHR;
        create_info.pNext = NULL;
        create_info.flags = 0;
        create_info.displayMode = mode;
        create_info.planeIndex = plane_index;
        create_info.planeStackIndex = plane_props[plane_index].currentStackIndex;
        create_info.transform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        create_info.alphaMode = alphaMode;
        create_info.globalAlpha = 1.0f;
        create_info.imageExtent = image_extent;

        return vkCreateDisplayPlaneSurfaceKHR(inst, &create_info, NULL, &surface);
#else
        return VK_ERROR_UNKNOWN;
#endif
    }

    void Cube::demo_create_device() {
        VkResult U_ASSERT_ONLY err;
        float queue_priorities[1] = {0.0};
        VkDeviceQueueCreateInfo queues[2];
        queues[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queues[0].pNext = NULL;
        queues[0].queueFamilyIndex = graphics_queue_family_index;
        queues[0].queueCount = 1;
        queues[0].pQueuePriorities = queue_priorities;
        queues[0].flags = 0;

        VkPhysicalDevicePipelinePropertiesFeaturesEXT pipeline_props_feature = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_PROPERTIES_FEATURES_EXT,
            .pNext = NULL,
            .pipelinePropertiesIdentifier = VK_TRUE};
        VkDeviceCreateInfo device_info = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext = &pipeline_props_feature,
            .queueCreateInfoCount = 1,
            .pQueueCreateInfos = queues,
            .enabledLayerCount = 0,
            .ppEnabledLayerNames = NULL,
            .enabledExtensionCount = enabled_extension_count,
            .ppEnabledExtensionNames = (const char *const *)extension_names,
            .pEnabledFeatures = NULL,  // If specific features are required, pass them in here
        };
        if (separate_present_queue) {
            queues[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queues[1].pNext = NULL;
            queues[1].queueFamilyIndex = present_queue_family_index;
            queues[1].queueCount = 1;
            queues[1].pQueuePriorities = queue_priorities;
            queues[1].flags = 0;
            device_info.queueCreateInfoCount = 2;
        }
        err = vkCreateDevice(gpu, &device_info, NULL, &device);
        if (err) {
            ERR_EXIT("vkCreateDevice failed.\n", "vkCreateDevice Failure");
        }
    }

    void Cube::run() {
        demo_init_vk_swapchain();
        demo_prepare();
        demo_run_image_dump();
    }

    void Cube::demo_destroy_texture(texture_object *tex_objs) {
        /* clean up staging resources */
        if (tex_objs->image) vkDestroyImage(device, tex_objs->image, NULL);
        if (tex_objs->buffer) vkDestroyBuffer(device, tex_objs->buffer, NULL);
    }

    void Cube::demo_prepare_texture_image(const char *filename, texture_object *tex_obj, VkImageTiling tiling,
                                    VkImageUsageFlags usage, VkFlags required_props) {
        const VkFormat tex_format = VK_FORMAT_R8G8B8A8_UNORM;
        int32_t tex_width;
        int32_t tex_height;
        VkResult U_ASSERT_ONLY err;
        bool U_ASSERT_ONLY pass;

        if (!loadTexture(filename, NULL, NULL, &tex_width, &tex_height)) {
            ERR_EXIT("Failed to load textures", "Load Texture Failure");
        }

        tex_obj->tex_width = tex_width;
        tex_obj->tex_height = tex_height;

        const VkImageCreateInfo image_create_info = {.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
                                                        .pNext = NULL,
                                                        .flags = 0,
                                                        .imageType = VK_IMAGE_TYPE_2D,
                                                        .format = tex_format,
                                                        .extent = {(uint32_t)tex_width, (uint32_t)tex_height, (uint32_t)1},
                                                        .mipLevels = 1,
                                                        .arrayLayers = 1,
                                                        .samples = VK_SAMPLE_COUNT_1_BIT,
                                                        .tiling = tiling,
                                                        .usage = usage,
                                                        .initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED};

        VkMemoryRequirements mem_reqs;

        err = vkCreateImage(device, &image_create_info, NULL, &tex_obj->image);
        assert(!err);

        vkGetImageMemoryRequirements(device, tex_obj->image, &mem_reqs);

        tex_obj->mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        tex_obj->mem_alloc.pNext = NULL;
        tex_obj->mem_alloc.allocationSize = mem_reqs.size;
        tex_obj->mem_alloc.memoryTypeIndex = 0;

        pass = memory_type_from_properties(mem_reqs.memoryTypeBits, required_props, &tex_obj->mem_alloc.memoryTypeIndex);
        assert(pass);

        /* allocate memory */
        err = vkAllocateMemory(device, &tex_obj->mem_alloc, NULL, &(tex_obj->mem));
        assert(!err);

        /* bind memory */
        err = vkBindImageMemory(device, tex_obj->image, tex_obj->mem, 0);
        assert(!err);

        if (required_props & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
            const VkImageSubresource subres = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .mipLevel = 0,
                .arrayLayer = 0,
            };
            VkSubresourceLayout layout;
            void *data;

            vkGetImageSubresourceLayout(device, tex_obj->image, &subres, &layout);

            err = vkMapMemory(device, tex_obj->mem, 0, tex_obj->mem_alloc.allocationSize, 0, &data);
            assert(!err);

            if (!loadTexture(filename, reinterpret_cast<uint8_t *>(data), &layout, &tex_width, &tex_height)) {
                fprintf(stderr, "Error loading texture: %s\n", filename);
            }

            vkUnmapMemory(device, tex_obj->mem);
        }

        tex_obj->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    }

    void Cube::demo_prepare_texture_buffer(const char *filename, texture_object *tex_obj) {
        int32_t tex_width;
        int32_t tex_height;
        VkResult U_ASSERT_ONLY err;
        bool U_ASSERT_ONLY pass;

        if (!loadTexture(filename, NULL, NULL, &tex_width, &tex_height)) {
            ERR_EXIT("Failed to load textures", "Load Texture Failure");
        }

        tex_obj->tex_width = tex_width;
        tex_obj->tex_height = tex_height;

        const VkBufferCreateInfo buffer_create_info = {.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                                                        .pNext = NULL,
                                                        .flags = 0,
                                                        .size = (VkDeviceSize)(tex_width * tex_height * 4),
                                                        .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                                        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
                                                        .queueFamilyIndexCount = 0,
                                                        .pQueueFamilyIndices = NULL};

        err = vkCreateBuffer(device, &buffer_create_info, NULL, &tex_obj->buffer);
        assert(!err);

        VkMemoryRequirements mem_reqs;
        vkGetBufferMemoryRequirements(device, tex_obj->buffer, &mem_reqs);

        tex_obj->mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        tex_obj->mem_alloc.pNext = NULL;
        tex_obj->mem_alloc.allocationSize = mem_reqs.size;
        tex_obj->mem_alloc.memoryTypeIndex = 0;

        VkFlags requirements = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        pass = memory_type_from_properties(mem_reqs.memoryTypeBits, requirements, &tex_obj->mem_alloc.memoryTypeIndex);
        assert(pass);

        err = vkAllocateMemory(device, &tex_obj->mem_alloc, NULL, &(tex_obj->mem));
        assert(!err);

        /* bind memory */
        err = vkBindBufferMemory(device, tex_obj->buffer, tex_obj->mem, 0);
        assert(!err);

        VkSubresourceLayout layout;
        memset(&layout, 0, sizeof(layout));
        layout.rowPitch = tex_width * 4;

        void *data;
        err = vkMapMemory(device, tex_obj->mem, 0, tex_obj->mem_alloc.allocationSize, 0, &data);
        assert(!err);

        if (!loadTexture(filename, reinterpret_cast<uint8_t *>(data), &layout, &tex_width, &tex_height)) {
            fprintf(stderr, "Error loading texture: %s\n", filename);
        }

        vkUnmapMemory(device, tex_obj->mem);
    }

    void Cube::demo_prepare_cube_data_buffers() {
        VkBufferCreateInfo buf_info;
        VkMemoryRequirements mem_reqs;
        VkMemoryAllocateInfo mem_alloc;
        mat4x4 MVP, VP;
        VkResult U_ASSERT_ONLY err;
        bool U_ASSERT_ONLY pass;
        vktexcube_vs_uniform data;

        mat4x4_mul(VP, projection_matrix, view_matrix);
        mat4x4_mul(MVP, VP, model_matrix);
        memcpy(data.mvp, MVP, sizeof(MVP));
        //    dumpMatrix("MVP", MVP);

        for (unsigned int i = 0; i < 12 * 3; i++) {
            data.position[i][0] = g_vertex_buffer_data[i * 3];
            data.position[i][1] = g_vertex_buffer_data[i * 3 + 1];
            data.position[i][2] = g_vertex_buffer_data[i * 3 + 2];
            data.position[i][3] = 1.0f;
            data.attr[i][0] = g_uv_buffer_data[2 * i];
            data.attr[i][1] = g_uv_buffer_data[2 * i + 1];
            data.attr[i][2] = 0;
            data.attr[i][3] = 0;
        }

        memset(&buf_info, 0, sizeof(buf_info));
        buf_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buf_info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        buf_info.size = sizeof(data);

        for (unsigned int i = 0; i < swapchainImageCount; i++) {
            err = vkCreateBuffer(device, &buf_info, NULL, &swapchain_image_resources[i].uniform_buffer);
            assert(!err);

            vkGetBufferMemoryRequirements(device, swapchain_image_resources[i].uniform_buffer, &mem_reqs);

            mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            mem_alloc.pNext = NULL;
            mem_alloc.allocationSize = mem_reqs.size;
            mem_alloc.memoryTypeIndex = 0;

            pass = memory_type_from_properties(mem_reqs.memoryTypeBits,
                                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                                &mem_alloc.memoryTypeIndex);
            assert(pass);

            err = vkAllocateMemory(device, &mem_alloc, NULL, &swapchain_image_resources[i].uniform_memory);
            assert(!err);

            err = vkMapMemory(device, swapchain_image_resources[i].uniform_memory, 0, VK_WHOLE_SIZE, 0,
                                &swapchain_image_resources[i].uniform_memory_ptr);
            assert(!err);

            memcpy(swapchain_image_resources[i].uniform_memory_ptr, &data, sizeof data);

            err = vkBindBufferMemory(device, swapchain_image_resources[i].uniform_buffer,
                                        swapchain_image_resources[i].uniform_memory, 0);
            assert(!err);
        }
    }

    void Cube::demo_cleanup() {
        uint32_t i;

        prepared = false;
        vkDeviceWaitIdle(device);

        // Wait for fences from present operations
        for (i = 0; i < FRAME_LAG; i++) {
            vkWaitForFences(device, 1, &fences[i], VK_TRUE, UINT64_MAX);
            vkDestroyFence(device, fences[i], NULL);
        }

        for (i = 0; i < swapchainImageCount; i++) {
            vkDestroySemaphore(device, image_acquired_semaphores[i], NULL);
            vkDestroySemaphore(device, draw_complete_semaphores[i], NULL);
            if (separate_present_queue) {
                vkDestroySemaphore(device, image_ownership_semaphores[i], NULL);
            }
        }

        // If the window is currently minimized, demo_resize has already done some cleanup for us.
        if (!is_minimized) {
            for (i = 0; i < swapchainImageCount; i++) {
                vkDestroyFramebuffer(device, swapchain_image_resources[i].framebuffer, NULL);
            }

            vkDestroyPipeline(device, pipeline, NULL);
            vkDestroyPipelineCache(device, pipeline_cache, NULL);
            vkDestroyRenderPass(device, render_pass, NULL);
            vkDestroyPipelineLayout(device, pipeline_layout, NULL);
            vkDestroyDescriptorSetLayout(device, desc_layout, NULL);

            for (i = 0; i < DEMO_TEXTURE_COUNT; i++) {
                vkDestroyImageView(device, textures[i].view, NULL);
                vkDestroyImage(device, textures[i].image, NULL);
                vkDestroySampler(device, textures[i].sampler, NULL);
            }

            vkDestroyImageView(device, depth.view, NULL);
            vkDestroyImage(device, depth.image, NULL);

            for (i = 0; i < swapchainImageCount; i++) {
                vkDestroyImageView(device, swapchain_image_resources[i].view, NULL);
                vkFreeCommandBuffers(device, cmd_pool, 1, &swapchain_image_resources[i].cmd);
                vkDestroyBuffer(device, swapchain_image_resources[i].uniform_buffer, NULL);
                vkUnmapMemory(device, swapchain_image_resources[i].uniform_memory);
            }

            if (wsi_platform == WSI_PLATFORM_FILE) {
                vkDestroyImage(device, offscreen_texture.img.image, NULL);
                vkDestroyBuffer(device, offscreen_texture.staging.buffer, NULL);
            }
        }
        vkDeviceWaitIdle(device);
        vkDestroyDevice(device, NULL);
#if defined(VK_USE_PLATFORM_DISPLAY_KHR)
        if (VK_NV_acquire_winrt_display_supported && wsi_platform == WSI_PLATFORM_DISPLAY) {
            pfnReleaseDisplayEXT(gpu, display);
        }
#endif
        vkDestroySurfaceKHR(inst, surface, NULL);
        vkDestroyInstance(inst, NULL);
    }

    bool Cube::memory_type_from_properties(uint32_t typeBits, VkFlags requirements_mask, uint32_t *typeIndex) {
        // Search memtypes to find first index with those properties
        for (uint32_t i = 0; i < VK_MAX_MEMORY_TYPES; i++) {
            if ((typeBits & 1) == 1) {
                // Type is available, does it match user properties?
                if ((memory_properties.memoryTypes[i].propertyFlags & requirements_mask) == requirements_mask) {
                    *typeIndex = i;
                    return true;
                }
            }
            typeBits >>= 1;
        }
        // No memory types matched, return failure
        return false;
    }

    bool Cube::loadTexture(const char *filename, uint8_t *rgba_data, VkSubresourceLayout *layout, int32_t *width, int32_t *height) {
        (void)filename;
        static const uint8_t logo_ppm[] = {
#include "logo.ppm.h"
        };
        static const size_t logo_ppm_len = sizeof(logo_ppm);
        char *cPtr;
        cPtr = (char *)logo_ppm;
        if ((unsigned char *)cPtr >= (logo_ppm + logo_ppm_len) || strncmp(cPtr, "P6\n", 3)) {
            return false;
        }
        while (strncmp(cPtr++, "\n", 1))
            ;
        sscanf(cPtr, "%u %u", width, height);
        if (rgba_data == NULL) {
            return true;
        }
        while (strncmp(cPtr++, "\n", 1))
            ;
        if ((unsigned char *)cPtr >= (logo_ppm + logo_ppm_len) || strncmp(cPtr, "255\n", 4)) {
            return false;
        }
        while (strncmp(cPtr++, "\n", 1))
            ;
        for (int y = 0; y < *height; y++) {
            uint8_t *rowPtr = rgba_data;
            for (int x = 0; x < *width; x++) {
                memcpy(rowPtr, cPtr, 3);
                rowPtr[3] = 255; /* Alpha of 1 */
                rowPtr += 4;
                cPtr += 3;
            }
            rgba_data += layout->rowPitch;
        }
        return true;
    }

    const char *Cube::wsi_to_string(WSI_PLATFORM wsi_platform) {
        switch (wsi_platform) {
            case (WSI_PLATFORM_AUTO):
                return "auto";
#if defined(VK_USE_PLATFORM_DISPLAY_KHR)
            case (WSI_PLATFORM_DISPLAY):
                return "display";
#endif
            case (WSI_PLATFORM_FILE):
                return "file";
            default:
                return "unknown";
        }
    };

    void Cube::initialize_entry_points(VkInstance instance) {
        fpnSetDebugUtilsObjectNameEXT =
            (PFN_vkSetDebugUtilsObjectNameEXT)vkGetInstanceProcAddr(instance, "vkSetDebugUtilsObjectNameEXT");
        pfnCmdBeginDebugUtilsLabelEXT =
            (PFN_vkCmdBeginDebugUtilsLabelEXT)vkGetInstanceProcAddr(instance, "vkCmdBeginDebugUtilsLabelEXT");
        pfnCmdEndDebugUtilsLabelEXT =
            (PFN_vkCmdEndDebugUtilsLabelEXT)vkGetInstanceProcAddr(instance, "vkCmdEndDebugUtilsLabelEXT");
        pfnCreateDebugUtilsMessengerEXT =
            (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        pfnGetPhysicalDeviceSurfaceSupportKHR =
            (PFN_vkGetPhysicalDeviceSurfaceSupportKHR)vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceSupportKHR");
        pfnGetPhysicalDeviceSurfaceFormatsKHR =
            (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceFormatsKHR");
        pfnGetPhysicalDeviceSurfaceCapabilitiesKHR = (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)vkGetInstanceProcAddr(
            instance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
        pfnGetPhysicalDeviceSurfacePresentModesKHR = (PFN_vkGetPhysicalDeviceSurfacePresentModesKHR)vkGetInstanceProcAddr(
            instance, "vkGetPhysicalDeviceSurfacePresentModesKHR");
        pfnCreateSwapchainKHR = (PFN_vkCreateSwapchainKHR)vkGetInstanceProcAddr(instance, "vkCreateSwapchainKHR");
        pfnGetSwapchainImagesKHR = (PFN_vkGetSwapchainImagesKHR)vkGetInstanceProcAddr(instance, "vkGetSwapchainImagesKHR");
        pfnReleaseDisplayEXT = (PFN_vkReleaseDisplayEXT)vkGetInstanceProcAddr(instance, "vkReleaseDisplayEXT");
#ifdef VK_USE_PLATFORM_WIN32_KHR
        pfnAcquireWinrtDisplayNV = (PFN_vkAcquireWinrtDisplayNV)vkGetInstanceProcAddr(instance, "vkAcquireWinrtDisplayNV");
#endif
    }

    char const *Cube::to_string(VkPhysicalDeviceType const type) {
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

    template <typename... Ts>
    void Cube::DbgMsg(Ts... t) {
        std::cout << (t << ...) << std::endl;
    }

    VkSurfaceFormatKHR Cube::pick_surface_format(const VkSurfaceFormatKHR *surfaceFormats, uint32_t count) {
        // Prefer non-SRGB formats...
        for (uint32_t i = 0; i < count; i++) {
            const VkFormat format = surfaceFormats[i].format;

            if (format == VK_FORMAT_R8G8B8A8_UNORM || format == VK_FORMAT_B8G8R8A8_UNORM ||
                format == VK_FORMAT_A2B10G10R10_UNORM_PACK32 || format == VK_FORMAT_A2R10G10B10_UNORM_PACK32 ||
                format == VK_FORMAT_A1R5G5B5_UNORM_PACK16 || format == VK_FORMAT_R5G6B5_UNORM_PACK16 ||
                format == VK_FORMAT_R16G16B16A16_SFLOAT) {
                return surfaceFormats[i];
            }
        }

        printf("Can't find our preferred formats... Falling back to first exposed format. Rendering may be incorrect.\n");

        assert(count >= 1);
        return surfaceFormats[0];
    }

    void Cube::save_frame(uint32_t frame_id) {
        VkResult U_ASSERT_ONLY err;

        void *data;
        err =
            vkMapMemory(device, offscreen_texture.staging.mem, 0, offscreen_texture.staging.mem_alloc.allocationSize, 0, &data);
        assert(!err);

        static constexpr size_t MAX_FILENAME_LENGTH = 100;
        char filename[MAX_FILENAME_LENGTH];

        snprintf(filename, MAX_FILENAME_LENGTH, "frame_%05d.ppm", frame_id);
        save_rgba_to_ppm(filename, width, height, reinterpret_cast<const unsigned char *>(data));

        vkUnmapMemory(device, offscreen_texture.staging.mem);
    }

    void Cube::save_rgba_to_ppm(const char *filename, int width, int height, const unsigned char *rgba_data) {
        FILE *f = fopen(filename, "wb");
        if (!f) {
            perror("fopen");
            ERR_EXIT("Failed to open ppm output file", "Save RGBA to PPM failure");
        }

        char header[64];
        int header_len = snprintf(header, sizeof(header), "P6\n%d %d\n255\n", width, height);
        fwrite(header, 1, header_len, f);

        unsigned char *rgb_data = (unsigned char *)malloc(width * height * 3);
        if (!rgb_data) {
            perror("malloc");
            fclose(f);
            ERR_EXIT("Failed to malloc temp buffer for rgba/rgb conversion", "Save RGBA to PPM failure");
        }

        for (int i = 0; i < width * height; ++i) {
            rgb_data[i * 3 + 0] = rgba_data[i * 4 + 0];  // R
            rgb_data[i * 3 + 1] = rgba_data[i * 4 + 1];  // G
            rgb_data[i * 3 + 2] = rgba_data[i * 4 + 2];  // B
        }

        fwrite(rgb_data, 1, width * height * 3, f);

        free(rgb_data);
        fclose(f);
    }
