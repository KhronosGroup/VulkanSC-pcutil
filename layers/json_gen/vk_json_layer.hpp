/*
 * Copyright (c) 2020-2025 The Khronos Group Inc.
 * Copyright (c) 2020-2025 LunarG, Inc.
 * Copyright (c) 2020-2025 Advanced Micro Devices, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once
#define VK_NO_PROTOTYPES
#include <vulkan/vulkan_core.h>
#undef VK_NO_PROTOTYPES
#include <array>
#include <vector>
#include <memory>
#include <unordered_set>
#include <atomic>
#include <mutex>
#include <string_view>
#include <optional>
#include <variant>

#include "allocator.h"
#include <vulkan/utility/vk_concurrent_unordered_map.hpp>
#include <vulkan/utility/vk_safe_struct.hpp>

namespace vk_json {

struct DeviceData;

struct LayerSettings {};

struct PhysicalDeviceData {
    VkPhysicalDevice physical_device = VK_NULL_HANDLE;
    bool lower_has_pipeline_props = false;
    uint32_t api_version;
};

struct DeviceFeatures {
    DeviceFeatures(const VkDeviceCreateInfo* ci);
    DeviceFeatures() = default;

    bool pipeline_props;
};

struct YcbcrData {
    YcbcrData(const VkSamplerYcbcrConversionCreateInfo* ci, DeviceData& device_data);
    YcbcrData() = default;

    vku::safe_VkSamplerYcbcrConversionCreateInfo create_info;
    std::uintptr_t id;
};

struct SamplerData {
    SamplerData(const VkSamplerCreateInfo* ci, DeviceData& device_data);
    SamplerData() = default;

    vku::safe_VkSamplerCreateInfo create_info;
    std::optional<YcbcrData> ycbcr_data;
    std::uintptr_t id;
};

struct DescriptorSetLayoutData {
    DescriptorSetLayoutData(const VkDescriptorSetLayoutCreateInfo* ci, DeviceData& device_data);
    DescriptorSetLayoutData() = default;

    vku::safe_VkDescriptorSetLayoutCreateInfo create_info;
    std::vector<SamplerData> immutable_sampler_data;
    std::uintptr_t id;
};

struct PipelineLayoutData {
    PipelineLayoutData(const VkPipelineLayoutCreateInfo* ci, DeviceData& device_data);
    PipelineLayoutData() = default;

    vku::safe_VkPipelineLayoutCreateInfo create_info;
    std::vector<DescriptorSetLayoutData> descriptor_set_layout_data;
    std::uintptr_t id;
};

struct ShaderModuleData {
    ShaderModuleData(const VkShaderModuleCreateInfo* ci);
    ShaderModuleData() = default;

    vku::safe_VkShaderModuleCreateInfo create_info;
};

struct RenderPassData {
    RenderPassData(const VkRenderPassCreateInfo* ci);
    RenderPassData() = default;

    vku::safe_VkRenderPassCreateInfo create_info;
};

struct RenderPass2Data {
    RenderPass2Data(const VkRenderPassCreateInfo2* ci);
    RenderPass2Data() = default;

    vku::safe_VkRenderPassCreateInfo2 create_info;
};

struct GraphicsPipelineData {
    GraphicsPipelineData(const VkGraphicsPipelineCreateInfo* ci, DeviceData& device_data);
    GraphicsPipelineData() = default;

    vku::safe_VkGraphicsPipelineCreateInfo create_info;
    PipelineLayoutData pipeline_layout_data;
    std::variant<RenderPassData, RenderPass2Data> renderpass_data;
    std::vector<ShaderModuleData> shader_module_data;
    std::array<uint8_t, VK_UUID_SIZE> uuid;
    std::uintptr_t id;
};

struct ComputePipelineData {
    ComputePipelineData(const VkComputePipelineCreateInfo* ci, DeviceData& device_data);
    ComputePipelineData() = default;

    vku::safe_VkComputePipelineCreateInfo create_info;
    PipelineLayoutData pipeline_layout_data;
    ShaderModuleData shader_module_data;
    std::array<uint8_t, VK_UUID_SIZE> uuid;
    std::uintptr_t id;
};

struct ObjectResCreateInfo {
    uint32_t pipelineCacheCreateInfoCount;
    const VkPipelineCacheCreateInfo* pPipelineCacheCreateInfos;
    uint32_t pipelinePoolSizeCount;
    const VkPipelinePoolSize* pPipelinePoolSizes;
    std::atomic<uint32_t> semaphoreRequestCount;
    std::atomic<uint32_t> commandBufferRequestCount;
    std::atomic<uint32_t> fenceRequestCount;
    std::atomic<uint32_t> deviceMemoryRequestCount;
    std::atomic<uint32_t> bufferRequestCount;
    std::atomic<uint32_t> imageRequestCount;
    std::atomic<uint32_t> eventRequestCount;
    std::atomic<uint32_t> queryPoolRequestCount;
    std::atomic<uint32_t> bufferViewRequestCount;
    std::atomic<uint32_t> imageViewRequestCount;
    std::atomic<uint32_t> layeredImageViewRequestCount;
    std::atomic<uint32_t> pipelineCacheRequestCount;
    std::atomic<uint32_t> pipelineLayoutRequestCount;
    std::atomic<uint32_t> renderPassRequestCount;
    std::atomic<uint32_t> graphicsPipelineRequestCount;
    std::atomic<uint32_t> computePipelineRequestCount;
    std::atomic<uint32_t> descriptorSetLayoutRequestCount;
    std::atomic<uint32_t> samplerRequestCount;
    std::atomic<uint32_t> descriptorPoolRequestCount;
    std::atomic<uint32_t> descriptorSetRequestCount;
    std::atomic<uint32_t> framebufferRequestCount;
    std::atomic<uint32_t> commandPoolRequestCount;
    std::atomic<uint32_t> samplerYcbcrConversionRequestCount;
    std::atomic<uint32_t> surfaceRequestCount;
    std::atomic<uint32_t> swapchainRequestCount;
    std::atomic<uint32_t> displayModeRequestCount;
    std::atomic<uint32_t> subpassDescriptionRequestCount;
    std::atomic<uint32_t> attachmentDescriptionRequestCount;
    std::atomic<uint32_t> descriptorSetLayoutBindingRequestCount;
    std::atomic<uint32_t> descriptorSetLayoutBindingLimit;
    std::atomic<uint32_t> maxImageViewMipLevels;
    std::atomic<uint32_t> maxImageViewArrayLayers;
    std::atomic<uint32_t> maxLayeredImageViewMipLevels;
    std::atomic<uint32_t> maxOcclusionQueriesPerPool;
    std::atomic<uint32_t> maxPipelineStatisticsQueriesPerPool;
    std::atomic<uint32_t> maxTimestampQueriesPerPool;
    std::atomic<uint32_t> maxImmutableSamplersPerDescriptorSetLayout;
};

#define DECLARE_HOOK(fn) PFN_vk##fn fn
struct InstanceData {
    InstanceData(VkInstance instance, PFN_vkGetInstanceProcAddr gpa, const VkAllocationCallbacks* alloc);
    InstanceData() = delete;
    InstanceData(const InstanceData&) = delete;
    InstanceData& operator=(const InstanceData&) = delete;

    std::shared_ptr<PhysicalDeviceData> GetPhysicalDeviceData(VkPhysicalDevice physical_device) const;

    VkInstance instance;
    uint32_t api_version;
    LayerSettings layer_settings;
    const VkAllocationCallbacks* allocator;
    struct InstanceDispatchTable {
        DECLARE_HOOK(GetInstanceProcAddr);
        DECLARE_HOOK(CreateInstance);
        DECLARE_HOOK(DestroyInstance);
        DECLARE_HOOK(CreateDevice);
        DECLARE_HOOK(EnumeratePhysicalDevices);
        DECLARE_HOOK(EnumerateDeviceExtensionProperties);
        DECLARE_HOOK(EnumerateInstanceExtensionProperties);
        DECLARE_HOOK(GetPhysicalDeviceFeatures2);
        DECLARE_HOOK(GetPhysicalDeviceFeatures2KHR);
        DECLARE_HOOK(GetPhysicalDeviceProperties);
    } vtable;

    vku::concurrent::unordered_map<VkPhysicalDevice, std::shared_ptr<PhysicalDeviceData>> physical_device_map;
};

struct DeviceData {
    DeviceData(VkDevice device, const VkDeviceCreateInfo* ci, PFN_vkGetDeviceProcAddr gpa, bool enable,
               const VkAllocationCallbacks* alloc);
    DeviceData() = delete;
    DeviceData(const DeviceData&) = delete;
    DeviceData& operator=(const DeviceData&) = delete;

    VkDevice device;
    vku::safe_VkDeviceCreateInfo create_info;
    struct DeviceDispatchTable {
        DECLARE_HOOK(GetDeviceProcAddr);
        DECLARE_HOOK(DestroyDevice);
        DECLARE_HOOK(CreateShaderModule);
        DECLARE_HOOK(DestroyShaderModule);
        DECLARE_HOOK(CreateGraphicsPipelines);
        DECLARE_HOOK(CreateComputePipelines);
        DECLARE_HOOK(CreateDescriptorSetLayout);
        DECLARE_HOOK(CreatePipelineLayout);
        DECLARE_HOOK(CreateRenderPass);
        DECLARE_HOOK(CreateRenderPass2);
        DECLARE_HOOK(CreateSampler);
        DECLARE_HOOK(CreateSamplerYcbcrConversion);
        DECLARE_HOOK(CreateImage);
        DECLARE_HOOK(DestroyImage);
        DECLARE_HOOK(CreateImageView);
        DECLARE_HOOK(DestroyImageView);
        DECLARE_HOOK(AllocateCommandBuffers);
        // FreeCommandBuffers does not release memory, only CommandPool reset does.
        DECLARE_HOOK(CreateFence);
        DECLARE_HOOK(DestroyFence);
        DECLARE_HOOK(CreateBuffer);
        DECLARE_HOOK(DestroyBuffer);
        DECLARE_HOOK(CreateBufferView);
        DECLARE_HOOK(DestroyBufferView);
        DECLARE_HOOK(CreateEvent);
        DECLARE_HOOK(DestroyEvent);
        DECLARE_HOOK(CreateQueryPool);
        // DestroyQueryPool does not exist in SC.
        DECLARE_HOOK(CreateDescriptorPool);
        // DestroyDescriptorPool does not exist in SC.
        DECLARE_HOOK(AllocateDescriptorSets);
        DECLARE_HOOK(FreeDescriptorSets);
        DECLARE_HOOK(CreateFramebuffer);
        DECLARE_HOOK(DestroyFramebuffer);
        DECLARE_HOOK(CreateCommandPool);
        // DestroyCommandPool does not exist in SC.
        // Releasing memory upon command pool reset requires command buffer tracking
    } vtable;
    const VkAllocationCallbacks* allocator;

    void writePCJSON(GraphicsPipelineData& graphics_pipeline_data);
    void writePCJSON(ComputePipelineData& gcompute_pipeline_data);

    vku::concurrent::unordered_map<VkSamplerYcbcrConversion, std::shared_ptr<YcbcrData>> ycbcr_map;
    vku::concurrent::unordered_map<VkSampler, std::shared_ptr<SamplerData>> sampler_map;
    vku::concurrent::unordered_map<VkDescriptorSetLayout, std::shared_ptr<DescriptorSetLayoutData>> descriptor_set_layout_map;
    vku::concurrent::unordered_map<VkPipelineLayout, std::shared_ptr<PipelineLayoutData>> pipeline_layout_map;
    vku::concurrent::unordered_map<VkShaderModule, std::shared_ptr<ShaderModuleData>> shader_module_map;
    vku::concurrent::unordered_map<VkPipeline, std::shared_ptr<GraphicsPipelineData>> graphics_pipeline_map;
    vku::concurrent::unordered_map<VkPipeline, std::shared_ptr<ComputePipelineData>> compute_pipeline_map;
    vku::concurrent::unordered_map<VkRenderPass, std::shared_ptr<RenderPassData>> renderpass_map;
    vku::concurrent::unordered_map<VkRenderPass, std::shared_ptr<RenderPass2Data>> renderpass2_map;
    ObjectResCreateInfo obj_res_info;
    std::atomic<std::uintptr_t> obj_counter;
    std::mutex CreateImageView_mutex;
};
#undef DECLARE_HOOK

std::string_view getProcessName();

std::string_view getBaseDirectoryPath();

struct OwningVpjShaderFilenames;
OwningVpjShaderFilenames get_shader_filenames(const VkGraphicsPipelineCreateInfo& ci, const std::string& prefix,
                                              const uint32_t index);
OwningVpjShaderFilenames get_shader_filenames(const VkComputePipelineCreateInfo& ci, const std::string& prefix,
                                              const uint32_t index);
}  // namespace vk_json
