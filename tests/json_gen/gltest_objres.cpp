/*
 * Copyright (c) 2025 The Khronos Group Inc.
 * Copyright (c) 2025 RasterGrid Kft.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <vulkan/vulkan.h>
#include <vulkan/utility/vk_struct_helper.hpp>

#include <gtest/gtest.h>

#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <regex>
#include <array>

class ObjectReservation : public testing::Test {
  public:
    ObjectReservation() { clean_data_files(); }
    ObjectReservation(const ObjectReservation&) = delete;
    ObjectReservation(ObjectReservation&&) = delete;
    ~ObjectReservation() { clean_data_files(); }

    void TEST_DESCRIPTION(const char* desc) { RecordProperty("description", desc); }

    std::string get_header(size_t i) {
        std::filesystem::path header_path = std::string("./gltest_objres_objectResInfo_") + std::to_string(i) + ".hpp";
        std::ifstream header_stream{header_path};
        return std::string(std::istreambuf_iterator<char>{header_stream}, std::istreambuf_iterator<char>{});
    }

    void write_id(std::string& ref, int32_t id) {
        for (auto pos = ref.find('@', 0); pos != std::string::npos; pos = ref.find('@', pos)) {
            ref.replace(pos, 1, std::to_string(id));
        }
    }

  protected:
    static inline int32_t device_counter = 0;

  private:
    void clean_data_files() {
        std::for_each(std::filesystem::directory_iterator{"."}, std::filesystem::directory_iterator{},
                      [](const std::filesystem::directory_entry& entry) {
                          if (std::regex_search(entry.path().generic_string(), std::regex{R"(gltest_json_)"})) {
                              std::filesystem::remove(entry);
                          }
                      });
    }
};

TEST_F(ObjectReservation, Simple) {
    TEST_DESCRIPTION("Tests whether all types of object reservations are registered.");

    auto instance_ci = std::make_unique<VkInstanceCreateInfo>(vku::InitStructHelper());
    VkInstance instance;
    vkCreateInstance(instance_ci.get(), nullptr, &instance);

    uint32_t phys_dev_count = 1;
    std::vector<VkPhysicalDevice> phys_devs(phys_dev_count);
    vkEnumeratePhysicalDevices(instance, &phys_dev_count, phys_devs.data());

    auto device_ci = std::make_unique<VkDeviceCreateInfo>(vku::InitStructHelper());
    std::vector<const char*> enabled_device_extensions{"VK_KHR_swapchain"};
    device_ci->enabledExtensionCount = (uint32_t)enabled_device_extensions.size();
    device_ci->ppEnabledExtensionNames = enabled_device_extensions.data();
    VkDevice device;
    vkCreateDevice(phys_devs[0], device_ci.get(), nullptr, &device);

    auto shader_module_ci = std::make_unique<VkShaderModuleCreateInfo>(vku::InitStructHelper());
    std::vector<uint32_t> ref_spirv{1, 2, 3, 4};
    shader_module_ci->codeSize = ref_spirv.size() * sizeof(uint32_t);
    shader_module_ci->pCode = ref_spirv.data();
    VkShaderModule shader_module;
    vkCreateShaderModule(device, shader_module_ci.get(), nullptr, &shader_module);

    auto buf_ci = std::make_unique<VkBufferCreateInfo>(vku::InitStructHelper());
    VkBuffer buf;
    vkCreateBuffer(device, buf_ci.get(), NULL, &buf);

    auto buf_view_ci = std::make_unique<VkBufferViewCreateInfo>(vku::InitStructHelper());
    VkBufferView buf_view;
    vkCreateBufferView(device, buf_view_ci.get(), nullptr, &buf_view);

    auto fb_ci = std::make_unique<VkFramebufferCreateInfo>(vku::InitStructHelper());
    VkFramebuffer fb;
    vkCreateFramebuffer(device, fb_ci.get(), nullptr, &fb);

    auto swap_chain_ci = std::make_unique<VkSwapchainCreateInfoKHR>(vku::InitStructHelper());
    VkSwapchainKHR swap_chain;
    vkCreateSwapchainKHR(device, swap_chain_ci.get(), nullptr, &swap_chain);

    auto img_ci = std::make_unique<VkImageCreateInfo>(vku::InitStructHelper());
    VkImage img;
    vkCreateImage(device, img_ci.get(), nullptr, &img);

    auto img_view_ci = std::make_unique<VkImageViewCreateInfo>(vku::InitStructHelper());
    img_view_ci->subresourceRange.layerCount = 2;
    img_view_ci->subresourceRange.levelCount = 1;
    VkImageView img_view;
    vkCreateImageView(device, img_view_ci.get(), nullptr, &img_view);

    auto alloc_mem_info = std::make_unique<VkMemoryAllocateInfo>(vku::InitStructHelper());
    VkDeviceMemory mem;
    vkAllocateMemory(device, alloc_mem_info.get(), NULL, &mem);

    // Create Ycbcr sampler conversion
    auto ycbcr_ci = std::make_unique<VkSamplerYcbcrConversionCreateInfo>(vku::InitStructHelper());
    VkSamplerYcbcrConversion ycbcr;
    vkCreateSamplerYcbcrConversion(device, ycbcr_ci.get(), nullptr, &ycbcr);

    // Create Ycbcr sampler
    auto sampler_ci = std::make_unique<VkSamplerCreateInfo>(vku::InitStructHelper());
    VkSampler sampler;
    vkCreateSampler(device, sampler_ci.get(), nullptr, &sampler);

    // Create descriptor set layout
    auto ds_layout_ci = std::make_unique<VkDescriptorSetLayoutCreateInfo>(vku::InitStructHelper());
    auto binding = std::make_unique<VkDescriptorSetLayoutBinding>();
    ds_layout_ci->bindingCount = 1;
    ds_layout_ci->pBindings = binding.get();
    binding->descriptorCount = 1;
    binding->descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    binding->pImmutableSamplers = &sampler;
    VkDescriptorSetLayout ds_layout;
    vkCreateDescriptorSetLayout(device, ds_layout_ci.get(), nullptr, &ds_layout);

    // Create pipeline layout
    auto pipeline_layout_ci = std::make_unique<VkPipelineLayoutCreateInfo>(vku::InitStructHelper());
    pipeline_layout_ci->setLayoutCount = 1;
    pipeline_layout_ci->pSetLayouts = &ds_layout;
    VkPipelineLayout pipeline_layout;
    vkCreatePipelineLayout(device, pipeline_layout_ci.get(), nullptr, &pipeline_layout);

    auto pipeline_cache_ci = std::make_unique<VkPipelineCacheCreateInfo>(vku::InitStructHelper());
    VkPipelineCache pipeline_cache;
    vkCreatePipelineCache(device, pipeline_cache_ci.get(), NULL, &pipeline_cache);

    // Create compute pipeline
    auto pipeline_stage_ci = std::make_unique<VkPipelineShaderStageCreateInfo>(vku::InitStructHelper());
    pipeline_stage_ci->module = shader_module;
    pipeline_stage_ci->stage = VK_SHADER_STAGE_COMPUTE_BIT;
    pipeline_stage_ci->pName = "main";
    auto compute_pipeline_ci = std::make_unique<VkComputePipelineCreateInfo>(vku::InitStructHelper());
    compute_pipeline_ci->layout = pipeline_layout;
    compute_pipeline_ci->stage = *pipeline_stage_ci;
    VkPipeline compute_pipeline;
    vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, compute_pipeline_ci.get(), NULL, &compute_pipeline);

    auto renderpass_ci = std::make_unique<VkRenderPassCreateInfo>(vku::InitStructHelper());
    VkAttachmentDescription attachment_desc{};
    VkSubpassDescription subpass_desc{};
    attachment_desc.samples = VK_SAMPLE_COUNT_1_BIT;
    renderpass_ci->subpassCount = 1;
    renderpass_ci->pSubpasses = &subpass_desc;
    renderpass_ci->attachmentCount = 1;
    renderpass_ci->pAttachments = &attachment_desc;
    VkRenderPass render_pass;
    vkCreateRenderPass(device, renderpass_ci.get(), nullptr, &render_pass);

    auto graphics_pipeline_ci = std::make_unique<VkGraphicsPipelineCreateInfo>(vku::InitStructHelper());
    pipeline_stage_ci->stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    graphics_pipeline_ci->layout = pipeline_layout;
    graphics_pipeline_ci->stageCount = 1;
    graphics_pipeline_ci->pStages = pipeline_stage_ci.get();
    graphics_pipeline_ci->renderPass = render_pass;
    VkPipeline graphics_pipeline;
    vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, graphics_pipeline_ci.get(), NULL, &graphics_pipeline);

    auto desc_pool_ci = std::make_unique<VkDescriptorPoolCreateInfo>(vku::InitStructHelper());
    VkDescriptorPool desc_pool;
    vkCreateDescriptorPool(device, desc_pool_ci.get(), nullptr, &desc_pool);

    auto ds_alloc_info = std::make_unique<VkDescriptorSetAllocateInfo>(vku::InitStructHelper());
    ds_alloc_info->descriptorPool = desc_pool;
    ds_alloc_info->descriptorSetCount = 1;
    ds_alloc_info->pSetLayouts = &ds_layout;
    VkDescriptorSet desc_set;
    vkAllocateDescriptorSets(device, ds_alloc_info.get(), &desc_set);

    auto cmd_pool_ci = std::make_unique<VkCommandPoolCreateInfo>(vku::InitStructHelper());
    VkCommandPool cmd_pool;
    vkCreateCommandPool(device, cmd_pool_ci.get(), NULL, &cmd_pool);

    auto cmd_buf_ci = std::make_unique<VkCommandBufferAllocateInfo>(vku::InitStructHelper());
    cmd_buf_ci->commandBufferCount = 1;
    VkCommandBuffer cmd_buf;
    vkAllocateCommandBuffers(device, cmd_buf_ci.get(), &cmd_buf);

    auto fence_ci = std::make_unique<VkFenceCreateInfo>(vku::InitStructHelper());
    VkFence fence;
    vkCreateFence(device, fence_ci.get(), NULL, &fence);

    auto semaphore_ci = std::make_unique<VkSemaphoreCreateInfo>(vku::InitStructHelper());
    VkSemaphore semaphore;
    vkCreateSemaphore(device, semaphore_ci.get(), nullptr, &semaphore);

    auto event_ci = std::make_unique<VkEventCreateInfo>(vku::InitStructHelper());
    VkEvent event;
    vkCreateEvent(device, event_ci.get(), nullptr, &event);

    std::array<VkQueryPoolCreateInfo, 3> query_pool_cis{
        {vku::InitStructHelper(), vku::InitStructHelper(), vku::InitStructHelper()}};
    query_pool_cis[0].queryType = VK_QUERY_TYPE_OCCLUSION;
    query_pool_cis[0].queryCount = 1;
    query_pool_cis[1].queryType = VK_QUERY_TYPE_PIPELINE_STATISTICS;
    query_pool_cis[1].queryCount = 1;
    query_pool_cis[2].queryType = VK_QUERY_TYPE_TIMESTAMP;
    query_pool_cis[2].queryCount = 1;
    std::array<VkQueryPool, 3> query_pools;
    vkCreateQueryPool(device, &query_pool_cis[0], nullptr, &query_pools[0]);
    vkCreateQueryPool(device, &query_pool_cis[1], nullptr, &query_pools[1]);
    vkCreateQueryPool(device, &query_pool_cis[2], nullptr, &query_pools[2]);

    vkDestroyQueryPool(device, query_pools[2], nullptr);
    vkDestroyQueryPool(device, query_pools[1], nullptr);
    vkDestroyQueryPool(device, query_pools[0], nullptr);
    vkDestroyEvent(device, event, nullptr);
    vkDestroySemaphore(device, semaphore, nullptr);
    vkDestroyFence(device, fence, nullptr);
    vkFreeCommandBuffers(device, cmd_pool, 1, &cmd_buf);
    vkDestroyCommandPool(device, cmd_pool, nullptr);
    vkFreeDescriptorSets(device, desc_pool, 1, &desc_set);
    vkDestroyDescriptorPool(device, desc_pool, nullptr);
    vkDestroyPipeline(device, graphics_pipeline, nullptr);
    vkDestroyRenderPass(device, render_pass, nullptr);
    vkDestroyPipeline(device, compute_pipeline, nullptr);
    vkDestroyPipelineCache(device, pipeline_cache, nullptr);
    vkDestroyPipelineLayout(device, pipeline_layout, nullptr);
    vkDestroyDescriptorSetLayout(device, ds_layout, nullptr);
    vkDestroySampler(device, sampler, nullptr);
    vkDestroySamplerYcbcrConversion(device, ycbcr, nullptr);
    vkFreeMemory(device, mem, nullptr);
    vkDestroyImageView(device, img_view, nullptr);
    vkDestroyImage(device, img, nullptr);
    vkDestroySwapchainKHR(device, swap_chain, nullptr);
    vkDestroyFramebuffer(device, fb, nullptr);
    vkDestroyBufferView(device, buf_view, nullptr);
    vkDestroyBuffer(device, buf, nullptr);
    vkDestroyShaderModule(device, shader_module, nullptr);
    vkDestroyDevice(device, nullptr);
    vkDestroyInstance(instance, nullptr);

    int32_t actual_counter = device_counter++;
    auto header = get_header(actual_counter);

    std::string ref = {R"(#ifndef gltest_objres_objectResInfo_@_HPP
#define gltest_objres_objectResInfo_@_HPP

#include <vulkan/vulkan_sc_core.h>

static VkDeviceObjectReservationCreateInfo g_objectResCreateInfo_@ {};
static void SetObjectResCreateInfo()
{
	g_objectResCreateInfo_@.sType                                      = VK_STRUCTURE_TYPE_DEVICE_OBJECT_RESERVATION_CREATE_INFO;
	g_objectResCreateInfo_@.pNext                                      = nullptr;
	g_objectResCreateInfo_@.semaphoreRequestCount                      = 1;
	g_objectResCreateInfo_@.commandBufferRequestCount                  = 1;
	g_objectResCreateInfo_@.fenceRequestCount                          = 1;
	g_objectResCreateInfo_@.deviceMemoryRequestCount                   = 1;
	g_objectResCreateInfo_@.bufferRequestCount                         = 1;
	g_objectResCreateInfo_@.imageRequestCount                          = 1;
	g_objectResCreateInfo_@.eventRequestCount                          = 1;
	g_objectResCreateInfo_@.queryPoolRequestCount                      = 3;
	g_objectResCreateInfo_@.bufferViewRequestCount                     = 1;
	g_objectResCreateInfo_@.imageViewRequestCount                      = 1;
	g_objectResCreateInfo_@.layeredImageViewRequestCount               = 1;
	g_objectResCreateInfo_@.pipelineCacheRequestCount                  = 1;
	g_objectResCreateInfo_@.pipelineLayoutRequestCount                 = 1;
	g_objectResCreateInfo_@.renderPassRequestCount                     = 1;
	g_objectResCreateInfo_@.graphicsPipelineRequestCount               = 1;
	g_objectResCreateInfo_@.computePipelineRequestCount                = 1;
	g_objectResCreateInfo_@.descriptorSetLayoutRequestCount            = 1;
	g_objectResCreateInfo_@.samplerRequestCount                        = 1;
	g_objectResCreateInfo_@.descriptorPoolRequestCount                 = 1;
	g_objectResCreateInfo_@.descriptorSetRequestCount                  = 1;
	g_objectResCreateInfo_@.framebufferRequestCount                    = 1;
	g_objectResCreateInfo_@.commandPoolRequestCount                    = 1;
	g_objectResCreateInfo_@.samplerYcbcrConversionRequestCount         = 1;
	g_objectResCreateInfo_@.swapchainRequestCount                      = 1;
	g_objectResCreateInfo_@.subpassDescriptionRequestCount             = 1;
	g_objectResCreateInfo_@.attachmentDescriptionRequestCount          = 1;
	g_objectResCreateInfo_@.descriptorSetLayoutBindingRequestCount     = 1;
	g_objectResCreateInfo_@.descriptorSetLayoutBindingLimit            = 1;
	g_objectResCreateInfo_@.maxImageViewMipLevels                      = 1;
	g_objectResCreateInfo_@.maxImageViewArrayLayers                    = 2;
	g_objectResCreateInfo_@.maxLayeredImageViewMipLevels               = 1;
	g_objectResCreateInfo_@.maxOcclusionQueriesPerPool                 = 1;
	g_objectResCreateInfo_@.maxPipelineStatisticsQueriesPerPool        = 1;
	g_objectResCreateInfo_@.maxTimestampQueriesPerPool                 = 1;
	g_objectResCreateInfo_@.maxImmutableSamplersPerDescriptorSetLayout = 1;
}

#endif
)"};
    write_id(ref, actual_counter);
    EXPECT_EQ(header, ref);
}

TEST_F(ObjectReservation, HighWatermark) {
    TEST_DESCRIPTION("Tests high watermark behavior on request counts.");

    auto instance_ci = std::make_unique<VkInstanceCreateInfo>(vku::InitStructHelper());
    VkInstance instance;
    vkCreateInstance(instance_ci.get(), nullptr, &instance);

    uint32_t phys_dev_count = 1;
    std::vector<VkPhysicalDevice> phys_devs(phys_dev_count);
    vkEnumeratePhysicalDevices(instance, &phys_dev_count, phys_devs.data());

    auto device_ci = std::make_unique<VkDeviceCreateInfo>(vku::InitStructHelper());
    std::vector<const char*> enabled_device_extensions{"VK_KHR_swapchain"};
    device_ci->enabledExtensionCount = (uint32_t)enabled_device_extensions.size();
    device_ci->ppEnabledExtensionNames = enabled_device_extensions.data();
    VkDevice device;
    vkCreateDevice(phys_devs[0], device_ci.get(), nullptr, &device);

    auto shader_module_ci = std::make_unique<VkShaderModuleCreateInfo>(vku::InitStructHelper());
    std::vector<uint32_t> ref_spirv{1, 2, 3, 4};
    shader_module_ci->codeSize = ref_spirv.size() * sizeof(uint32_t);
    shader_module_ci->pCode = ref_spirv.data();
    VkShaderModule shader_module;
    vkCreateShaderModule(device, shader_module_ci.get(), nullptr, &shader_module);

    auto buf_ci = std::make_unique<VkBufferCreateInfo>(vku::InitStructHelper());
    VkBuffer buf;
    vkCreateBuffer(device, buf_ci.get(), NULL, &buf);
    vkDestroyBuffer(device, buf, nullptr);
    vkCreateBuffer(device, buf_ci.get(), NULL, &buf);

    auto buf_view_ci = std::make_unique<VkBufferViewCreateInfo>(vku::InitStructHelper());
    VkBufferView buf_view;
    vkCreateBufferView(device, buf_view_ci.get(), nullptr, &buf_view);
    vkDestroyBufferView(device, buf_view, nullptr);
    vkCreateBufferView(device, buf_view_ci.get(), nullptr, &buf_view);

    auto fb_ci = std::make_unique<VkFramebufferCreateInfo>(vku::InitStructHelper());
    VkFramebuffer fb;
    vkCreateFramebuffer(device, fb_ci.get(), nullptr, &fb);
    vkDestroyFramebuffer(device, fb, nullptr);
    vkCreateFramebuffer(device, fb_ci.get(), nullptr, &fb);

    auto swap_chain_ci = std::make_unique<VkSwapchainCreateInfoKHR>(vku::InitStructHelper());
    VkSwapchainKHR swap_chain;
    vkCreateSwapchainKHR(device, swap_chain_ci.get(), nullptr, &swap_chain);
    vkDestroySwapchainKHR(device, swap_chain, nullptr);
    vkCreateSwapchainKHR(device, swap_chain_ci.get(), nullptr, &swap_chain);

    auto img_ci = std::make_unique<VkImageCreateInfo>(vku::InitStructHelper());
    VkImage img;
    vkCreateImage(device, img_ci.get(), nullptr, &img);
    vkDestroyImage(device, img, nullptr);
    vkCreateImage(device, img_ci.get(), nullptr, &img);

    auto img_view_ci = std::make_unique<VkImageViewCreateInfo>(vku::InitStructHelper());
    img_view_ci->subresourceRange.layerCount = 2;
    img_view_ci->subresourceRange.levelCount = 1;
    VkImageView img_view;
    vkCreateImageView(device, img_view_ci.get(), nullptr, &img_view);
    vkDestroyImageView(device, img_view, nullptr);
    vkCreateImageView(device, img_view_ci.get(), nullptr, &img_view);

    auto alloc_mem_info = std::make_unique<VkMemoryAllocateInfo>(vku::InitStructHelper());
    VkDeviceMemory mem;
    vkAllocateMemory(device, alloc_mem_info.get(), NULL, &mem);
    vkFreeMemory(device, mem, nullptr);
    vkAllocateMemory(device, alloc_mem_info.get(), NULL, &mem);

    // Create Ycbcr sampler conversion
    auto ycbcr_ci = std::make_unique<VkSamplerYcbcrConversionCreateInfo>(vku::InitStructHelper());
    VkSamplerYcbcrConversion ycbcr;
    vkCreateSamplerYcbcrConversion(device, ycbcr_ci.get(), nullptr, &ycbcr);
    vkDestroySamplerYcbcrConversion(device, ycbcr, nullptr);
    vkCreateSamplerYcbcrConversion(device, ycbcr_ci.get(), nullptr, &ycbcr);

    // Create Ycbcr sampler
    auto sampler_ci = std::make_unique<VkSamplerCreateInfo>(vku::InitStructHelper());
    VkSampler sampler;
    vkCreateSampler(device, sampler_ci.get(), nullptr, &sampler);
    vkDestroySampler(device, sampler, nullptr);
    vkCreateSampler(device, sampler_ci.get(), nullptr, &sampler);

    // Create descriptor set layout
    auto ds_layout_ci = std::make_unique<VkDescriptorSetLayoutCreateInfo>(vku::InitStructHelper());
    auto binding = std::make_unique<VkDescriptorSetLayoutBinding>();
    ds_layout_ci->bindingCount = 1;
    ds_layout_ci->pBindings = binding.get();
    binding->descriptorCount = 1;
    binding->descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    binding->pImmutableSamplers = &sampler;
    VkDescriptorSetLayout ds_layout;
    vkCreateDescriptorSetLayout(device, ds_layout_ci.get(), nullptr, &ds_layout);
    vkDestroyDescriptorSetLayout(device, ds_layout, nullptr);
    vkCreateDescriptorSetLayout(device, ds_layout_ci.get(), nullptr, &ds_layout);

    // Create pipeline layout
    auto pipeline_layout_ci = std::make_unique<VkPipelineLayoutCreateInfo>(vku::InitStructHelper());
    pipeline_layout_ci->setLayoutCount = 1;
    pipeline_layout_ci->pSetLayouts = &ds_layout;
    VkPipelineLayout pipeline_layout;
    vkCreatePipelineLayout(device, pipeline_layout_ci.get(), nullptr, &pipeline_layout);
    vkDestroyPipelineLayout(device, pipeline_layout, nullptr);
    vkCreatePipelineLayout(device, pipeline_layout_ci.get(), nullptr, &pipeline_layout);

    auto pipeline_cache_ci = std::make_unique<VkPipelineCacheCreateInfo>(vku::InitStructHelper());
    VkPipelineCache pipeline_cache;
    vkCreatePipelineCache(device, pipeline_cache_ci.get(), NULL, &pipeline_cache);
    vkDestroyPipelineCache(device, pipeline_cache, nullptr);
    vkCreatePipelineCache(device, pipeline_cache_ci.get(), NULL, &pipeline_cache);

    // Create compute pipeline
    auto pipeline_stage_ci = std::make_unique<VkPipelineShaderStageCreateInfo>(vku::InitStructHelper());
    pipeline_stage_ci->module = shader_module;
    pipeline_stage_ci->stage = VK_SHADER_STAGE_COMPUTE_BIT;
    pipeline_stage_ci->pName = "main";
    auto compute_pipeline_ci = std::make_unique<VkComputePipelineCreateInfo>(vku::InitStructHelper());
    compute_pipeline_ci->layout = pipeline_layout;
    compute_pipeline_ci->stage = *pipeline_stage_ci;
    VkPipeline compute_pipeline;
    vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, compute_pipeline_ci.get(), NULL, &compute_pipeline);
    vkDestroyPipeline(device, compute_pipeline, nullptr);
    vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, compute_pipeline_ci.get(), NULL, &compute_pipeline);

    auto renderpass_ci = std::make_unique<VkRenderPassCreateInfo>(vku::InitStructHelper());
    VkAttachmentDescription attachment_desc{};
    VkSubpassDescription subpass_desc{};
    attachment_desc.samples = VK_SAMPLE_COUNT_1_BIT;
    renderpass_ci->subpassCount = 1;
    renderpass_ci->pSubpasses = &subpass_desc;
    renderpass_ci->attachmentCount = 1;
    renderpass_ci->pAttachments = &attachment_desc;
    VkRenderPass render_pass;
    vkCreateRenderPass(device, renderpass_ci.get(), nullptr, &render_pass);
    vkDestroyRenderPass(device, render_pass, nullptr);
    vkCreateRenderPass(device, renderpass_ci.get(), nullptr, &render_pass);

    auto graphics_pipeline_ci = std::make_unique<VkGraphicsPipelineCreateInfo>(vku::InitStructHelper());
    pipeline_stage_ci->stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    graphics_pipeline_ci->layout = pipeline_layout;
    graphics_pipeline_ci->stageCount = 1;
    graphics_pipeline_ci->pStages = pipeline_stage_ci.get();
    graphics_pipeline_ci->renderPass = render_pass;
    VkPipeline graphics_pipeline;
    vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, graphics_pipeline_ci.get(), NULL, &graphics_pipeline);
    vkDestroyPipeline(device, graphics_pipeline, nullptr);
    vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, graphics_pipeline_ci.get(), NULL, &graphics_pipeline);

    auto desc_pool_ci = std::make_unique<VkDescriptorPoolCreateInfo>(vku::InitStructHelper());
    VkDescriptorPool desc_pool;
    vkCreateDescriptorPool(device, desc_pool_ci.get(), nullptr, &desc_pool);
    vkDestroyDescriptorPool(device, desc_pool, nullptr);
    vkCreateDescriptorPool(device, desc_pool_ci.get(), nullptr, &desc_pool);

    auto ds_alloc_info = std::make_unique<VkDescriptorSetAllocateInfo>(vku::InitStructHelper());
    ds_alloc_info->descriptorPool = desc_pool;
    ds_alloc_info->descriptorSetCount = 1;
    ds_alloc_info->pSetLayouts = &ds_layout;
    VkDescriptorSet desc_set;
    vkAllocateDescriptorSets(device, ds_alloc_info.get(), &desc_set);
    vkFreeDescriptorSets(device, desc_pool, 1, &desc_set);
    vkAllocateDescriptorSets(device, ds_alloc_info.get(), &desc_set);

    auto cmd_pool_ci = std::make_unique<VkCommandPoolCreateInfo>(vku::InitStructHelper());
    VkCommandPool cmd_pool;
    vkCreateCommandPool(device, cmd_pool_ci.get(), NULL, &cmd_pool);
    vkDestroyCommandPool(device, cmd_pool, nullptr);
    vkCreateCommandPool(device, cmd_pool_ci.get(), NULL, &cmd_pool);

    auto cmd_buf_ci = std::make_unique<VkCommandBufferAllocateInfo>(vku::InitStructHelper());
    cmd_buf_ci->commandBufferCount = 1;
    VkCommandBuffer cmd_buf;
    vkAllocateCommandBuffers(device, cmd_buf_ci.get(), &cmd_buf);
    vkFreeCommandBuffers(device, cmd_pool, 1, &cmd_buf);
    vkAllocateCommandBuffers(device, cmd_buf_ci.get(), &cmd_buf);

    auto fence_ci = std::make_unique<VkFenceCreateInfo>(vku::InitStructHelper());
    VkFence fence;
    vkCreateFence(device, fence_ci.get(), NULL, &fence);
    vkDestroyFence(device, fence, nullptr);
    vkCreateFence(device, fence_ci.get(), NULL, &fence);

    auto semaphore_ci = std::make_unique<VkSemaphoreCreateInfo>(vku::InitStructHelper());
    VkSemaphore semaphore;
    vkCreateSemaphore(device, semaphore_ci.get(), nullptr, &semaphore);
    vkDestroySemaphore(device, semaphore, nullptr);
    vkCreateSemaphore(device, semaphore_ci.get(), nullptr, &semaphore);

    auto event_ci = std::make_unique<VkEventCreateInfo>(vku::InitStructHelper());
    VkEvent event;
    vkCreateEvent(device, event_ci.get(), nullptr, &event);
    vkDestroyEvent(device, event, nullptr);
    vkCreateEvent(device, event_ci.get(), nullptr, &event);

    std::array<VkQueryPoolCreateInfo, 3> query_pool_cis{
        {vku::InitStructHelper(), vku::InitStructHelper(), vku::InitStructHelper()}};
    query_pool_cis[0].queryType = VK_QUERY_TYPE_OCCLUSION;
    query_pool_cis[0].queryCount = 1;
    query_pool_cis[1].queryType = VK_QUERY_TYPE_PIPELINE_STATISTICS;
    query_pool_cis[1].queryCount = 1;
    query_pool_cis[2].queryType = VK_QUERY_TYPE_TIMESTAMP;
    query_pool_cis[2].queryCount = 1;
    std::array<VkQueryPool, 3> query_pools;
    vkCreateQueryPool(device, &query_pool_cis[0], nullptr, &query_pools[0]);
    vkDestroyQueryPool(device, query_pools[0], nullptr);
    vkCreateQueryPool(device, &query_pool_cis[0], nullptr, &query_pools[0]);
    vkCreateQueryPool(device, &query_pool_cis[1], nullptr, &query_pools[1]);
    vkDestroyQueryPool(device, query_pools[1], nullptr);
    vkCreateQueryPool(device, &query_pool_cis[1], nullptr, &query_pools[1]);
    vkCreateQueryPool(device, &query_pool_cis[2], nullptr, &query_pools[2]);
    vkDestroyQueryPool(device, query_pools[2], nullptr);
    vkCreateQueryPool(device, &query_pool_cis[2], nullptr, &query_pools[2]);

    vkDestroyQueryPool(device, query_pools[2], nullptr);
    vkDestroyQueryPool(device, query_pools[1], nullptr);
    vkDestroyQueryPool(device, query_pools[0], nullptr);
    vkDestroyEvent(device, event, nullptr);
    vkDestroySemaphore(device, semaphore, nullptr);
    vkDestroyFence(device, fence, nullptr);
    vkFreeCommandBuffers(device, cmd_pool, 1, &cmd_buf);
    vkDestroyCommandPool(device, cmd_pool, nullptr);
    vkFreeDescriptorSets(device, desc_pool, 1, &desc_set);
    vkDestroyDescriptorPool(device, desc_pool, nullptr);
    vkDestroyPipeline(device, graphics_pipeline, nullptr);
    vkDestroyRenderPass(device, render_pass, nullptr);
    vkDestroyPipeline(device, compute_pipeline, nullptr);
    vkDestroyPipelineCache(device, pipeline_cache, nullptr);
    vkDestroyPipelineLayout(device, pipeline_layout, nullptr);
    vkDestroyDescriptorSetLayout(device, ds_layout, nullptr);
    vkDestroySampler(device, sampler, nullptr);
    vkDestroySamplerYcbcrConversion(device, ycbcr, nullptr);
    vkFreeMemory(device, mem, nullptr);
    vkDestroyImageView(device, img_view, nullptr);
    vkDestroyImage(device, img, nullptr);
    vkDestroySwapchainKHR(device, swap_chain, nullptr);
    vkDestroyFramebuffer(device, fb, nullptr);
    vkDestroyBufferView(device, buf_view, nullptr);
    vkDestroyBuffer(device, buf, nullptr);
    vkDestroyShaderModule(device, shader_module, nullptr);
    vkDestroyDevice(device, nullptr);
    vkDestroyInstance(instance, nullptr);

    int32_t actual_counter = device_counter++;
    auto header = get_header(actual_counter);

    std::string ref = {R"(#ifndef gltest_objres_objectResInfo_@_HPP
#define gltest_objres_objectResInfo_@_HPP

#include <vulkan/vulkan_sc_core.h>

static VkDeviceObjectReservationCreateInfo g_objectResCreateInfo_@ {};
static void SetObjectResCreateInfo()
{
	g_objectResCreateInfo_@.sType                                      = VK_STRUCTURE_TYPE_DEVICE_OBJECT_RESERVATION_CREATE_INFO;
	g_objectResCreateInfo_@.pNext                                      = nullptr;
	g_objectResCreateInfo_@.semaphoreRequestCount                      = 1;
	g_objectResCreateInfo_@.commandBufferRequestCount                  = 2;
	g_objectResCreateInfo_@.fenceRequestCount                          = 1;
	g_objectResCreateInfo_@.deviceMemoryRequestCount                   = 2;
	g_objectResCreateInfo_@.bufferRequestCount                         = 1;
	g_objectResCreateInfo_@.imageRequestCount                          = 1;
	g_objectResCreateInfo_@.eventRequestCount                          = 1;
	g_objectResCreateInfo_@.queryPoolRequestCount                      = 6;
	g_objectResCreateInfo_@.bufferViewRequestCount                     = 1;
	g_objectResCreateInfo_@.imageViewRequestCount                      = 1;
	g_objectResCreateInfo_@.layeredImageViewRequestCount               = 1;
	g_objectResCreateInfo_@.pipelineCacheRequestCount                  = 1;
	g_objectResCreateInfo_@.pipelineLayoutRequestCount                 = 1;
	g_objectResCreateInfo_@.renderPassRequestCount                     = 1;
	g_objectResCreateInfo_@.graphicsPipelineRequestCount               = 1;
	g_objectResCreateInfo_@.computePipelineRequestCount                = 1;
	g_objectResCreateInfo_@.descriptorSetLayoutRequestCount            = 1;
	g_objectResCreateInfo_@.samplerRequestCount                        = 1;
	g_objectResCreateInfo_@.descriptorPoolRequestCount                 = 2;
	g_objectResCreateInfo_@.descriptorSetRequestCount                  = 1;
	g_objectResCreateInfo_@.framebufferRequestCount                    = 1;
	g_objectResCreateInfo_@.commandPoolRequestCount                    = 2;
	g_objectResCreateInfo_@.samplerYcbcrConversionRequestCount         = 1;
	g_objectResCreateInfo_@.swapchainRequestCount                      = 1;
	g_objectResCreateInfo_@.subpassDescriptionRequestCount             = 1;
	g_objectResCreateInfo_@.attachmentDescriptionRequestCount          = 1;
	g_objectResCreateInfo_@.descriptorSetLayoutBindingRequestCount     = 1;
	g_objectResCreateInfo_@.descriptorSetLayoutBindingLimit            = 1;
	g_objectResCreateInfo_@.maxImageViewMipLevels                      = 1;
	g_objectResCreateInfo_@.maxImageViewArrayLayers                    = 2;
	g_objectResCreateInfo_@.maxLayeredImageViewMipLevels               = 1;
	g_objectResCreateInfo_@.maxOcclusionQueriesPerPool                 = 1;
	g_objectResCreateInfo_@.maxPipelineStatisticsQueriesPerPool        = 1;
	g_objectResCreateInfo_@.maxTimestampQueriesPerPool                 = 1;
	g_objectResCreateInfo_@.maxImmutableSamplersPerDescriptorSetLayout = 1;
}

#endif
)"};
    write_id(ref, actual_counter);
    EXPECT_EQ(header, ref);
}

TEST_F(ObjectReservation, NonTrivial) {
    TEST_DESCRIPTION("Tests non-trivial max object tracking.");

    auto instance_ci = std::make_unique<VkInstanceCreateInfo>(vku::InitStructHelper());
    VkInstance instance;
    vkCreateInstance(instance_ci.get(), nullptr, &instance);

    uint32_t phys_dev_count = 1;
    std::vector<VkPhysicalDevice> phys_devs(phys_dev_count);
    vkEnumeratePhysicalDevices(instance, &phys_dev_count, phys_devs.data());

    auto device_ci = std::make_unique<VkDeviceCreateInfo>(vku::InitStructHelper());
    std::vector<const char*> enabled_device_extensions{"VK_KHR_swapchain"};
    device_ci->enabledExtensionCount = (uint32_t)enabled_device_extensions.size();
    device_ci->ppEnabledExtensionNames = enabled_device_extensions.data();
    VkDevice device;
    vkCreateDevice(phys_devs[0], device_ci.get(), nullptr, &device);

    auto sampler_ci = std::make_unique<VkSamplerCreateInfo>(vku::InitStructHelper());
    std::array<VkSampler, 4> samplers;
    vkCreateSampler(device, sampler_ci.get(), nullptr, &samplers[0]);
    vkCreateSampler(device, sampler_ci.get(), nullptr, &samplers[1]);
    vkCreateSampler(device, sampler_ci.get(), nullptr, &samplers[2]);
    vkCreateSampler(device, sampler_ci.get(), nullptr, &samplers[3]);

    // Create descriptor set layout
    auto ds_layout_ci = std::make_unique<VkDescriptorSetLayoutCreateInfo>(vku::InitStructHelper());
    std::array<VkDescriptorSetLayoutBinding, 3> bindings;
    ds_layout_ci->bindingCount = (uint32_t)bindings.size();
    ds_layout_ci->pBindings = bindings.data();
    bindings[0].binding = 0;
    bindings[0].descriptorCount = 1;
    bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    bindings[0].pImmutableSamplers = nullptr;
    bindings[1].binding = 2;
    bindings[1].descriptorCount = 4;
    bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    bindings[1].pImmutableSamplers = samplers.data();
    bindings[2].binding = 6;
    bindings[2].descriptorCount = 8;
    bindings[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    bindings[2].pImmutableSamplers = nullptr;
    VkDescriptorSetLayout ds_layout;
    vkCreateDescriptorSetLayout(device, ds_layout_ci.get(), nullptr, &ds_layout);
    vkDestroyDescriptorSetLayout(device, ds_layout, nullptr);
    vkCreateDescriptorSetLayout(device, ds_layout_ci.get(), nullptr, &ds_layout);

    vkDestroyDescriptorSetLayout(device, ds_layout, nullptr);
    vkDestroySampler(device, samplers[3], nullptr);
    vkDestroySampler(device, samplers[2], nullptr);
    vkDestroySampler(device, samplers[1], nullptr);
    vkDestroySampler(device, samplers[0], nullptr);
    vkDestroyDevice(device, nullptr);
    vkDestroyInstance(instance, nullptr);

    int32_t actual_counter = device_counter++;
    auto header = get_header(actual_counter);

    std::string ref = {R"(#ifndef gltest_objres_objectResInfo_@_HPP
#define gltest_objres_objectResInfo_@_HPP

#include <vulkan/vulkan_sc_core.h>

static VkDeviceObjectReservationCreateInfo g_objectResCreateInfo_@ {};
static void SetObjectResCreateInfo()
{
	g_objectResCreateInfo_@.sType                                      = VK_STRUCTURE_TYPE_DEVICE_OBJECT_RESERVATION_CREATE_INFO;
	g_objectResCreateInfo_@.pNext                                      = nullptr;
	g_objectResCreateInfo_@.semaphoreRequestCount                      = 0;
	g_objectResCreateInfo_@.commandBufferRequestCount                  = 0;
	g_objectResCreateInfo_@.fenceRequestCount                          = 0;
	g_objectResCreateInfo_@.deviceMemoryRequestCount                   = 0;
	g_objectResCreateInfo_@.bufferRequestCount                         = 0;
	g_objectResCreateInfo_@.imageRequestCount                          = 0;
	g_objectResCreateInfo_@.eventRequestCount                          = 0;
	g_objectResCreateInfo_@.queryPoolRequestCount                      = 0;
	g_objectResCreateInfo_@.bufferViewRequestCount                     = 0;
	g_objectResCreateInfo_@.imageViewRequestCount                      = 0;
	g_objectResCreateInfo_@.layeredImageViewRequestCount               = 0;
	g_objectResCreateInfo_@.pipelineCacheRequestCount                  = 0;
	g_objectResCreateInfo_@.pipelineLayoutRequestCount                 = 0;
	g_objectResCreateInfo_@.renderPassRequestCount                     = 0;
	g_objectResCreateInfo_@.graphicsPipelineRequestCount               = 0;
	g_objectResCreateInfo_@.computePipelineRequestCount                = 0;
	g_objectResCreateInfo_@.descriptorSetLayoutRequestCount            = 1;
	g_objectResCreateInfo_@.samplerRequestCount                        = 4;
	g_objectResCreateInfo_@.descriptorPoolRequestCount                 = 0;
	g_objectResCreateInfo_@.descriptorSetRequestCount                  = 0;
	g_objectResCreateInfo_@.framebufferRequestCount                    = 0;
	g_objectResCreateInfo_@.commandPoolRequestCount                    = 0;
	g_objectResCreateInfo_@.samplerYcbcrConversionRequestCount         = 0;
	g_objectResCreateInfo_@.swapchainRequestCount                      = 0;
	g_objectResCreateInfo_@.subpassDescriptionRequestCount             = 0;
	g_objectResCreateInfo_@.attachmentDescriptionRequestCount          = 0;
	g_objectResCreateInfo_@.descriptorSetLayoutBindingRequestCount     = 3;
	g_objectResCreateInfo_@.descriptorSetLayoutBindingLimit            = 7;
	g_objectResCreateInfo_@.maxImageViewMipLevels                      = 0;
	g_objectResCreateInfo_@.maxImageViewArrayLayers                    = 0;
	g_objectResCreateInfo_@.maxLayeredImageViewMipLevels               = 0;
	g_objectResCreateInfo_@.maxOcclusionQueriesPerPool                 = 0;
	g_objectResCreateInfo_@.maxPipelineStatisticsQueriesPerPool        = 0;
	g_objectResCreateInfo_@.maxTimestampQueriesPerPool                 = 0;
	g_objectResCreateInfo_@.maxImmutableSamplersPerDescriptorSetLayout = 4;
}

#endif
)"};
    write_id(ref, actual_counter);
    EXPECT_EQ(header, ref);
}
