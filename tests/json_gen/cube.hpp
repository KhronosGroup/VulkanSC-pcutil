/*
 * Copyright (c) 2025 The Khronos Group Inc.
 * Copyright (c) 2025 RasterGrid Kft.
 * Copyright (c) 2021, NVIDIA CORPORATION
 *
 * SPDX-License-Identifier: Apache-2.0
 */

 #pragma once

#include <vulkan/vulkan.h>

#include <gtest/gtest.h>

#include "linmath.h"


class Cube {
    public:
    Cube();
    Cube(const Cube &) = delete;
    Cube(Cube &&) = delete;
    ~Cube();

    static const int pipeline_increment = 1;

    void run();

    private:

    // TODO: remove safety critical high watermarks
    static constexpr uint32_t MAX_INSTANCE_LAYER_COUNT = 16;
    static constexpr uint32_t MAX_INSTANCE_EXTENSION_COUNT = 64;
    static constexpr uint32_t MAX_PHYSICAL_DEVICE_COUNT = 16;
    static constexpr uint32_t MAX_PHYSICAL_DEVICE_EXTENSION_COUNT = 512;
    static constexpr uint32_t MAX_QUEUE_FAMILY_COUNT = 32;
    static constexpr uint32_t MAX_SWAPCHAIN_IMAGE_COUNT = 16;
    static constexpr uint32_t MAX_PRESENT_MODE_COUNT = 16;
    static constexpr uint32_t MAX_DISPLAY_COUNT = 32;
    static constexpr uint32_t MAX_DISPLAY_PLANE_COUNT = 32;
    static constexpr uint32_t MAX_SURFACE_FORMAT_COUNT = 256;
    static constexpr uint32_t MAX_DEBUG_MESSAGE_LENGTH = 8192;
    static constexpr uint32_t MAX_PIPELINE_POOL_SIZE = 1048576;
    static constexpr uint32_t MAX_COMMAND_POOL_SIZE = 1048576;
    static constexpr uint32_t DEMO_TEXTURE_COUNT = 1;

    // Allow a maximum of two outstanding presentation operations.
    static constexpr uint32_t FRAME_LAG = 2;
    typedef enum WSI_PLATFORM {
        WSI_PLATFORM_AUTO = 0,
        WSI_PLATFORM_DISPLAY,
        WSI_PLATFORM_FILE,
        WSI_PLATFORM_INVALID,  // Sentinel just to indicate invalid user input
    } WSI_PLATFORM;

    typedef struct {
        VkImage image;
        VkCommandBuffer cmd;
        VkCommandBuffer graphics_to_present_cmd;
        VkImageView view;
        VkBuffer uniform_buffer;
        VkDeviceMemory uniform_memory;
        void *uniform_memory_ptr;
        VkFramebuffer framebuffer;
        VkDescriptorSet descriptor_set;
    } SwapchainImageResources;

    struct vktexcube_vs_uniform {
        // Must start with MVP
        float mvp[4][4];
        float position[12 * 3][4];
        float attr[12 * 3][4];
    };

#if defined(VK_USE_PLATFORM_DISPLAY_KHR)
    VkDisplayKHR display;
#endif
    WSI_PLATFORM wsi_platform;
    VkSurfaceKHR surface;
    bool prepared;
    bool use_staging_buffer;
    bool separate_present_queue;
    bool is_minimized;
    bool invalid_gpu_selection;
    int32_t gpu_number;

    bool VK_KHR_incremental_present_enabled;
    bool VK_NV_acquire_winrt_display_supported;

    VkInstance inst;
    VkPhysicalDevice gpu;
    VkDevice device;
    VkQueue graphics_queue;
    VkQueue present_queue;
    uint32_t graphics_queue_family_index;
    uint32_t present_queue_family_index;
    VkSemaphore image_acquired_semaphores[MAX_SWAPCHAIN_IMAGE_COUNT];
    VkSemaphore draw_complete_semaphores[MAX_SWAPCHAIN_IMAGE_COUNT];
    VkSemaphore image_ownership_semaphores[MAX_SWAPCHAIN_IMAGE_COUNT];
    VkPhysicalDeviceProperties gpu_props;
    VkQueueFamilyProperties queue_props[MAX_QUEUE_FAMILY_COUNT];
    VkPhysicalDeviceMemoryProperties memory_properties;

    uint32_t enabled_extension_count;
    uint32_t enabled_layer_count;
    char *extension_names[64];
    char *enabled_layers[64];

    int width, height;
    VkFormat format;
    VkColorSpaceKHR color_space;

    uint32_t swapchainImageCount;
    VkSwapchainKHR swapchain;
    SwapchainImageResources swapchain_image_resources[MAX_SWAPCHAIN_IMAGE_COUNT];
    VkPresentModeKHR presentMode;
    VkFence fences[FRAME_LAG];
    int frame_index;
    int image_index;
    bool first_swapchain_frame;

    VkCommandPool cmd_pool;
    VkCommandPool present_cmd_pool;

    struct {
        VkFormat format;

        VkImage image;
        VkMemoryAllocateInfo mem_alloc;
        VkDeviceMemory mem;
        VkImageView view;
    } depth;

    struct {
        struct {
            VkImage image;
            VkMemoryAllocateInfo mem_alloc;
            VkDeviceMemory mem;
            VkImageView view;
        } img;

        struct {
            VkBuffer buffer;
            VkMemoryAllocateInfo mem_alloc;
            VkDeviceMemory mem;
        } staging;
    } offscreen_texture;

    struct texture_object {
        VkSampler sampler;

        VkImage image;
        VkBuffer buffer;
        VkImageLayout imageLayout;

        VkMemoryAllocateInfo mem_alloc;
        VkDeviceMemory mem;
        VkImageView view;
        int32_t tex_width, tex_height;
    };

    texture_object textures[DEMO_TEXTURE_COUNT];
    texture_object staging_texture;

    VkCommandBuffer cmd;  // Buffer for initialization commands
    VkPipelineLayout pipeline_layout;
    VkDescriptorSetLayout desc_layout;
    VkPipelineCache pipeline_cache;
    char *pipeline_cache_path;
    VkRenderPass render_pass;
    VkPipeline pipeline;

    mat4x4 projection_matrix;
    mat4x4 view_matrix;
    mat4x4 model_matrix;

    float spin_angle;
    float spin_increment;
    bool pause;

    VkShaderModule vert_shader_module;
    VkShaderModule frag_shader_module;

    VkDescriptorPool desc_pool;

    bool quit;
    int32_t curFrame;
    int32_t frameCount;
    bool validate;
    bool use_break;
    bool suppress_popups;
    bool force_errors;

    VkDebugUtilsMessengerEXT dbg_messenger;

    uint32_t current_buffer;
    uint32_t queue_family_count;

    PFN_vkSetDebugUtilsObjectNameEXT fpnSetDebugUtilsObjectNameEXT;
    PFN_vkCmdBeginDebugUtilsLabelEXT pfnCmdBeginDebugUtilsLabelEXT;
    PFN_vkCmdEndDebugUtilsLabelEXT pfnCmdEndDebugUtilsLabelEXT;
    PFN_vkCreateDebugUtilsMessengerEXT pfnCreateDebugUtilsMessengerEXT;
    PFN_vkGetPhysicalDeviceSurfaceSupportKHR pfnGetPhysicalDeviceSurfaceSupportKHR;
    PFN_vkGetPhysicalDeviceSurfaceFormatsKHR pfnGetPhysicalDeviceSurfaceFormatsKHR;
    PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR pfnGetPhysicalDeviceSurfaceCapabilitiesKHR;
    PFN_vkGetPhysicalDeviceSurfacePresentModesKHR pfnGetPhysicalDeviceSurfacePresentModesKHR;
    PFN_vkCreateSwapchainKHR pfnCreateSwapchainKHR;
    PFN_vkGetSwapchainImagesKHR pfnGetSwapchainImagesKHR;
    PFN_vkReleaseDisplayEXT pfnReleaseDisplayEXT;

    //--------------------------------------------------------------------------------------
    // Mesh and VertexFormat Data
    //--------------------------------------------------------------------------------------
    // clang-format off
    const std::vector<float> g_vertex_buffer_data{
        -1.0f,-1.0f,-1.0f,  // -X side
        -1.0f,-1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,

        -1.0f,-1.0f,-1.0f,  // -Z side
        1.0f, 1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,

        -1.0f,-1.0f,-1.0f,  // -Y side
        1.0f,-1.0f,-1.0f,
        1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
        1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,

        -1.0f, 1.0f,-1.0f,  // +Y side
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f,-1.0f,

        1.0f, 1.0f,-1.0f,  // +X side
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,

        -1.0f, 1.0f, 1.0f,  // +Z side
        -1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
    };

    const std::vector<float> g_uv_buffer_data{
        0.0f, 1.0f,  // -X side
        1.0f, 1.0f,
        1.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f,

        1.0f, 1.0f,  // -Z side
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,

        1.0f, 0.0f,  // -Y side
        1.0f, 1.0f,
        0.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,

        1.0f, 0.0f,  // +Y side
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,

        1.0f, 0.0f,  // +X side
        0.0f, 0.0f,
        0.0f, 1.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,

        0.0f, 0.0f,  // +Z side
        0.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
    };
    // clang-format on

    void demo_init();
    void demo_init_vk();
    void demo_check_and_set_wsi_platform();
    void demo_init_vk_swapchain();

    void demo_prepare();
    void demo_prepare_buffers();
    VkResult demo_prepare_offscreen_texture();
    void demo_prepare_depth();
    void demo_set_image_layout(VkImage image, VkImageAspectFlags aspectMask, VkImageLayout old_image_layout,
                                VkImageLayout new_image_layout, VkAccessFlagBits srcAccessMask, VkPipelineStageFlags src_stages,
                                VkPipelineStageFlags dest_stages);
    void demo_prepare_textures();
    void demo_prepare_descriptor_layout();
    void demo_prepare_render_pass();
    VkShaderModule demo_prepare_shader_module(const char *, const uint32_t *code, size_t size);
    void demo_prepare_vs();
    void demo_prepare_fs();
    void demo_prepare_pipeline();
    void demo_prepare_descriptor_pool();
    void demo_prepare_descriptor_set();
    void demo_prepare_framebuffers();
    void demo_build_image_ownership_cmd(int i);
    void demo_draw_build_cmd(VkCommandBuffer cmd_buf);
    void demo_flush_init_cmd();
    void demo_draw();
    void demo_update_data_buffer();
    void demo_resize();
    void demo_run_image_dump();
    void demo_run_display();
    void demo_create_surface();
    VkResult demo_create_display_surface();
    void demo_create_device();

    void demo_destroy_texture(texture_object *tex_objs);
    void demo_prepare_texture_image(const char *filename, texture_object *tex_obj, VkImageTiling tiling,
                                    VkImageUsageFlags usage, VkFlags required_props);
    void demo_prepare_texture_buffer(const char *filename, texture_object *tex_obj);
    void demo_prepare_cube_data_buffers();
    void demo_cleanup();
    bool memory_type_from_properties(uint32_t typeBits, VkFlags requirements_mask, uint32_t *typeIndex);
    bool loadTexture(const char *filename, uint8_t *rgba_data, VkSubresourceLayout *layout, int32_t *width, int32_t *height);
    const char *wsi_to_string(WSI_PLATFORM wsi_platform);
    void initialize_entry_points(VkInstance instance);
    char const *to_string(VkPhysicalDeviceType const type);
    template <typename... Ts>
    void DbgMsg(Ts... t);
    VkSurfaceFormatKHR pick_surface_format(const VkSurfaceFormatKHR *surfaceFormats, uint32_t count);
    void save_frame(uint32_t frame_id);
    void save_rgba_to_ppm(const char *filename, int width, int height, const unsigned char *rgba_data);
};
