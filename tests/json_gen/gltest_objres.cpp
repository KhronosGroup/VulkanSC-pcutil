/*
 * Copyright (c) 2025 The Khronos Group Inc.
 * Copyright (c) 2025 RasterGrid Kft.
 * Copyright (c) 2021, NVIDIA CORPORATION
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <vulkan/vulkan.h>

#include <gtest/gtest.h>

#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <regex>

class GenLayerObjResTest : public testing::Test {
  public:
    GenLayerObjResTest() = default;
    GenLayerObjResTest(const GenLayerObjResTest&) = delete;
    GenLayerObjResTest(GenLayerObjResTest&&) = delete;
    ~GenLayerObjResTest() = default;

    void TEST_DESCRIPTION(const char* desc) { RecordProperty("description", desc); }

#define VKCHECK(func)                                                                                                    \
    do {                                                                                                                 \
        if (auto res = func) {                                                                                           \
            std::string msg{#func};                                                                                      \
            msg.append(std::to_string(res));                                                                             \
            throw testing::AssertionException{                                                                           \
                testing::TestPartResult{testing::TestPartResult::Type::kFatalFailure, __FILE__, __LINE__, msg.c_str()}}; \
        }                                                                                                                \
    } while (0)

    class SAXPY {
      public:
        SAXPY(size_t push_constant_size = 4)
            : instance{VK_NULL_HANDLE},
              device{VK_NULL_HANDLE},
              shader_module{VK_NULL_HANDLE},
              buf_x{VK_NULL_HANDLE},
              buf_y{VK_NULL_HANDLE},
              mem_x{VK_NULL_HANDLE},
              mem_y{VK_NULL_HANDLE},
              buf_ds_layout{VK_NULL_HANDLE},
              pipeline_layout{VK_NULL_HANDLE},
              pipeline_cache{VK_NULL_HANDLE},
              pipeline{VK_NULL_HANDLE},
              fence{VK_NULL_HANDLE},
              sizeof_push_constant{push_constant_size} {}
        SAXPY(const SAXPY&) = delete;
        SAXPY(SAXPY&&) = delete;
        ~SAXPY() {
            vkDestroyFence(device, fence, NULL);
            vkDestroyPipeline(device, pipeline, nullptr);
            vkDestroyPipelineCache(device, pipeline_cache, NULL);
            vkDestroyPipelineLayout(device, pipeline_layout, NULL);
            vkDestroyDescriptorSetLayout(device, buf_ds_layout, NULL);
            vkFreeMemory(device, mem_y, NULL);
            vkFreeMemory(device, mem_x, NULL);
            vkDestroyBuffer(device, buf_y, NULL);
            vkDestroyBuffer(device, buf_x, NULL);
            vkDestroyShaderModule(device, shader_module, NULL);
            vkDestroyDevice(device, NULL);
            vkDestroyInstance(instance, NULL);
        }

        VkDevice get_device() const { return device; }
        VkPipeline get_pipeline() const { return pipeline; }

        std::string run() {
            uint32_t inst_ext_count = 0;
            bool inst_supp_portability_khr = false;
            VKCHECK(vkEnumerateInstanceExtensionProperties(NULL, &inst_ext_count, NULL));
            std::vector<VkExtensionProperties> inst_exts(inst_ext_count);
            if (inst_ext_count != 0) {
                VKCHECK(vkEnumerateInstanceExtensionProperties(NULL, &inst_ext_count, inst_exts.data()));
            }
            const char* portability_khr_str = "VK_KHR_portability_enumeration";

            for (uint32_t i = 0; i < inst_ext_count; ++i) {
                if (strstr(inst_exts[i].extensionName, portability_khr_str) != NULL) inst_supp_portability_khr = true;
            }

            VkApplicationInfo app_info = {.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                                          .pNext = NULL,
                                          .pApplicationName = "Vulkan-C-SAXPY",
                                          .applicationVersion = VK_MAKE_VERSION(0, 1, 1),
                                          .pEngineName = NULL,
                                          .engineVersion = 0,
                                          .apiVersion = VK_MAKE_VERSION(1, 2, 0)};
            VkInstanceCreateInfo inst_info = {.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                                              .pNext = NULL,
                                              .flags = inst_supp_portability_khr
                                                           ? VkInstanceCreateFlags{VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR}
                                                           : VkInstanceCreateFlags{0},
                                              .pApplicationInfo = &app_info,
                                              .enabledLayerCount = 0,
                                              .ppEnabledLayerNames = NULL,
                                              .enabledExtensionCount = 1,
                                              .ppEnabledExtensionNames = &portability_khr_str};
            VKCHECK(vkCreateInstance(&inst_info, NULL, &instance));

            uint32_t phys_dev_count;
            VKCHECK(vkEnumeratePhysicalDevices(instance, &phys_dev_count, NULL));
            VKCHECK(phys_dev_count == 0);

            std::vector<VkPhysicalDevice> phys_devs(phys_dev_count);
            VKCHECK(vkEnumeratePhysicalDevices(instance, &phys_dev_count, phys_devs.data()));

            VkPhysicalDevice phys_dev = phys_devs[0];
            VkPhysicalDeviceProperties phys_props;
            vkGetPhysicalDeviceProperties(phys_dev, &phys_props);

            printf("Selected device: %s\n\n", phys_props.deviceName);
            fflush(NULL);

            uint32_t dev_ext_count = 0;
            bool dev_supp_synchronization2_khr = false;
            VKCHECK(vkEnumerateDeviceExtensionProperties(phys_dev, NULL, &dev_ext_count, NULL));
            std::vector<VkExtensionProperties> dev_exts(dev_ext_count);
            if (dev_ext_count != 0) {
                VKCHECK(vkEnumerateDeviceExtensionProperties(phys_dev, NULL, &dev_ext_count, dev_exts.data()));
            }
            const char* synchronization2_khr_str = "VK_KHR_synchronization2";
            for (uint32_t i = 0; i < dev_ext_count; ++i) {
                if (strstr(dev_exts[i].extensionName, synchronization2_khr_str) != NULL) dev_supp_synchronization2_khr = true;
            }
            VKCHECK(!dev_supp_synchronization2_khr);

            uint32_t queue_fam_prop_count;
            vkGetPhysicalDeviceQueueFamilyProperties(phys_dev, &queue_fam_prop_count, NULL);
            std::vector<VkQueueFamilyProperties> queue_fam_props(queue_fam_prop_count);
            vkGetPhysicalDeviceQueueFamilyProperties(phys_dev, &queue_fam_prop_count, queue_fam_props.data());

            uint32_t queue_fam = UINT32_MAX;
            for (uint32_t i = 0; i < queue_fam_prop_count; ++i) {
                if (queue_fam_props[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
                    queue_fam = i;
                    break;
                }
            }

            float queue_prio = 0.5f;
            VkDeviceQueueCreateInfo queue_info = {.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                                                  .pNext = NULL,
                                                  .flags = 0,
                                                  .queueFamilyIndex = queue_fam,
                                                  .queueCount = 1,
                                                  .pQueuePriorities = &queue_prio};
            VkPhysicalDevicePipelinePropertiesFeaturesEXT pipeline_props_feature = {
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_PROPERTIES_FEATURES_EXT,
                .pNext = NULL,
                .pipelinePropertiesIdentifier = VK_TRUE};
            VkPhysicalDeviceSynchronization2FeaturesKHR sync_feature = {
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES,
                .pNext = &pipeline_props_feature,
                .synchronization2 = VK_TRUE};
            VkDeviceCreateInfo dev_info = {.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
                                           .pNext = &sync_feature,
                                           .flags = 0,
                                           .queueCreateInfoCount = 1,
                                           .pQueueCreateInfos = &queue_info,
                                           .enabledLayerCount = 0,
                                           .ppEnabledLayerNames = NULL,
                                           .enabledExtensionCount = 1,
                                           .ppEnabledExtensionNames = &synchronization2_khr_str,
                                           .pEnabledFeatures = NULL};

            VKCHECK(vkCreateDevice(phys_dev, &dev_info, NULL, &device));

            const uint32_t cs_code[] = {
#include "saxpy.comp.inc"
            };

            VkShaderModuleCreateInfo shader_module_info = {.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
                                                           .pNext = NULL,
                                                           .flags = 0,
                                                           .codeSize = sizeof(cs_code),
                                                           .pCode = cs_code};
            VKCHECK(vkCreateShaderModule(device, &shader_module_info, NULL, &shader_module));

            VkQueue queue;
            vkGetDeviceQueue(device, queue_fam, 0, &queue);

            const uint32_t kMaxWorkGroupCountX = 65535;
            const uint32_t kMaxComputeWorkGroupInvocations = 128;
            const size_t length = kMaxWorkGroupCountX * kMaxComputeWorkGroupInvocations;

            VkBufferCreateInfo buf_info = {.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                                           .pNext = NULL,
                                           .flags = 0,
                                           .size = length * sizeof(float),
                                           .usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                           .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
                                           .queueFamilyIndexCount = 1,
                                           .pQueueFamilyIndices = &queue_fam};
            VKCHECK(vkCreateBuffer(device, &buf_info, NULL, &buf_x));
            VKCHECK(vkCreateBuffer(device, &buf_info, NULL, &buf_y));

            VkBufferMemoryRequirementsInfo2 buf_x_reqs = {
                .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_REQUIREMENTS_INFO_2, .pNext = NULL, .buffer = buf_x};
            VkBufferMemoryRequirementsInfo2 buf_y_reqs = {
                .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_REQUIREMENTS_INFO_2, .pNext = NULL, .buffer = buf_y};
            VkMemoryRequirements2 mem_x_reqs = {
                .sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2,
                .pNext = NULL,
            };
            VkMemoryRequirements2 mem_y_reqs = {
                .sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2,
                .pNext = NULL,
            };
            vkGetBufferMemoryRequirements2(device, &buf_x_reqs, &mem_x_reqs);
            vkGetBufferMemoryRequirements2(device, &buf_y_reqs, &mem_y_reqs);

            VkPhysicalDeviceMemoryProperties2 phys_dev_mem_props = {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2};
            vkGetPhysicalDeviceMemoryProperties2(phys_dev, &phys_dev_mem_props);

            uint32_t mem_type_index = UINT32_MAX;
            for (uint32_t i = 0; i < phys_dev_mem_props.memoryProperties.memoryTypeCount; ++i) {
                VkMemoryType type = phys_dev_mem_props.memoryProperties.memoryTypes[i];
                VkMemoryHeap heap = phys_dev_mem_props.memoryProperties.memoryHeaps[type.heapIndex];
                bool compatible = type.propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT &&
                                  type.propertyFlags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT &&
                                  mem_x_reqs.memoryRequirements.memoryTypeBits & (1 << i) &&
                                  mem_y_reqs.memoryRequirements.memoryTypeBits & (1 << i) &&
                                  heap.size > (mem_x_reqs.memoryRequirements.size + mem_y_reqs.memoryRequirements.size);

                if (compatible) {
                    mem_type_index = i;
                    break;
                }
            }

            VkMemoryAllocateInfo alloc_x_info = {.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                                                 .pNext = NULL,
                                                 .allocationSize = mem_x_reqs.memoryRequirements.size,
                                                 .memoryTypeIndex = mem_type_index};
            VkMemoryAllocateInfo alloc_y_info = {.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                                                 .pNext = NULL,
                                                 .allocationSize = mem_y_reqs.memoryRequirements.size,
                                                 .memoryTypeIndex = mem_type_index};
            VKCHECK(vkAllocateMemory(device, &alloc_x_info, NULL, &mem_x));
            VKCHECK(vkAllocateMemory(device, &alloc_y_info, NULL, &mem_y));

            float* map_x;
            float* map_y;
            VKCHECK(vkMapMemory(device, mem_x, 0, length * sizeof(float), 0, (void**)&map_x));
            VKCHECK(vkMapMemory(device, mem_y, 0, length * sizeof(float), 0, (void**)&map_y));
            // Omit init

            VkMappedMemoryRange map_ranges[2] = {{.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
                                                  .pNext = NULL,
                                                  .memory = mem_x,
                                                  .offset = 0,
                                                  .size = VK_WHOLE_SIZE},
                                                 {.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
                                                  .pNext = NULL,
                                                  .memory = mem_y,
                                                  .offset = 0,
                                                  .size = VK_WHOLE_SIZE}};
            VKCHECK(vkFlushMappedMemoryRanges(device, 2, map_ranges));

            float a = 1;

            VkBindBufferMemoryInfo buf_mem_bind_infos[2] = {{.sType = VK_STRUCTURE_TYPE_BIND_BUFFER_MEMORY_INFO,
                                                             .pNext = NULL,
                                                             .buffer = buf_x,
                                                             .memory = mem_x,
                                                             .memoryOffset = 0},
                                                            {.sType = VK_STRUCTURE_TYPE_BIND_BUFFER_MEMORY_INFO,
                                                             .pNext = NULL,
                                                             .buffer = buf_y,
                                                             .memory = mem_y,
                                                             .memoryOffset = 0}};
            VKCHECK(vkBindBufferMemory2(device, 2, buf_mem_bind_infos));

            VkDescriptorSetLayoutBinding buf_ds_layout_binds[2] = {{.binding = 0,
                                                                    .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                                                                    .descriptorCount = 1,
                                                                    .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
                                                                    NULL},
                                                                   {.binding = 1,
                                                                    .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                                                                    .descriptorCount = 1,
                                                                    .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
                                                                    NULL}};
            VkDescriptorSetLayoutCreateInfo buf_ds_layout_info = {.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
                                                                  .pNext = NULL,
                                                                  .flags = 0,
                                                                  .bindingCount = 2,
                                                                  .pBindings = buf_ds_layout_binds};
            VKCHECK(vkCreateDescriptorSetLayout(device, &buf_ds_layout_info, NULL, &buf_ds_layout));

            VkPushConstantRange push_const_range = {
                .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT, .offset = 0, .size = static_cast<uint32_t>(sizeof_push_constant)};
            VkPipelineLayoutCreateInfo pipeline_layout_info = {.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
                                                               .pNext = NULL,
                                                               .flags = 0,
                                                               .setLayoutCount = 1,
                                                               .pSetLayouts = &buf_ds_layout,
                                                               .pushConstantRangeCount = 1,
                                                               .pPushConstantRanges = &push_const_range};
            VKCHECK(vkCreatePipelineLayout(device, &pipeline_layout_info, NULL, &pipeline_layout));
            VkPipelineCacheCreateInfo pipeline_cache_info = {.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
                                                             .pNext = NULL,
                                                             .flags = 0,
                                                             .initialDataSize = 0,
                                                             .pInitialData = NULL};
            VKCHECK(vkCreatePipelineCache(device, &pipeline_cache_info, NULL, &pipeline_cache));

            VkPipelineShaderStageCreateInfo pipeline_stage_info = {.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                                                                   .pNext = NULL,
                                                                   .flags = 0,
                                                                   .stage = VK_SHADER_STAGE_COMPUTE_BIT,
                                                                   .module = shader_module,
                                                                   .pName = "main",
                                                                   .pSpecializationInfo = NULL};
            VkComputePipelineCreateInfo pipeline_info = {.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
                                                         .pNext = NULL,
                                                         .flags = 0,
                                                         .stage = pipeline_stage_info,
                                                         .layout = pipeline_layout,
                                                         .basePipelineHandle = VK_NULL_HANDLE,
                                                         .basePipelineIndex = -1};
            VKCHECK(vkCreateComputePipelines(device, pipeline_cache, 1, &pipeline_info, NULL, &pipeline));

            VkDescriptorPoolSize desc_pool_size = {
                .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                .descriptorCount = 2
            };
            VkDescriptorPoolCreateInfo desc_pool_info = {
                .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
                .pNext = NULL,
                .flags = 0,
                .maxSets = 1,
                .poolSizeCount = 1,
                .pPoolSizes = &desc_pool_size
            };
            VkDescriptorPool desc_pool;
            VKCHECK(vkCreateDescriptorPool(device, &desc_pool_info, NULL, &desc_pool));

            VkDescriptorSetAllocateInfo ds_alloc_info = {
                .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
                .pNext = NULL,
                .descriptorPool = desc_pool,
                .descriptorSetCount = 1,
                .pSetLayouts = &buf_ds_layout
            };
            VkDescriptorSet desc_set;
            VKCHECK(vkAllocateDescriptorSets(device, &ds_alloc_info, &desc_set));
            VkDescriptorBufferInfo desc_buf_x_info = {
                .buffer = buf_x,
                .offset = 0,
                .range = VK_WHOLE_SIZE
            };
            VkDescriptorBufferInfo desc_buf_y_info = {
                .buffer = buf_y,
                .offset = 0,
                .range = VK_WHOLE_SIZE
            };
            VkWriteDescriptorSet write_desc_sets[2] = {{
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .pNext = NULL,
                .dstSet = desc_set,
                .dstBinding = 0,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                .pImageInfo = NULL,
                .pBufferInfo = &desc_buf_x_info,
                .pTexelBufferView = NULL
            },{
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .pNext = NULL,
                .dstSet = desc_set,
                .dstBinding = 1,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                .pImageInfo = NULL,
                .pBufferInfo = &desc_buf_y_info,
                .pTexelBufferView = NULL
            }};
            vkUpdateDescriptorSets(device, 2, write_desc_sets, 0, NULL);

            VkCommandPoolCreateInfo cmd_pool_info = {
                .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .queueFamilyIndex = queue_fam
            };
            VkCommandPool cmd_pool;
            VKCHECK(vkCreateCommandPool(device, &cmd_pool_info, NULL, &cmd_pool));

            VkCommandBufferAllocateInfo cmd_buf_info = {
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                .pNext = NULL,
                .commandPool = cmd_pool,
                .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                .commandBufferCount = 1
            };

            VkCommandBuffer cmd_buf;
            VKCHECK(vkAllocateCommandBuffers(device, &cmd_buf_info, &cmd_buf));

            VkCommandBufferBeginInfo cmd_buf_begin_info = {
                VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                NULL,
                VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
                NULL
            };

            VkBufferMemoryBarrier buf_in_barriers[2] = {{
                .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
                .srcAccessMask = VK_ACCESS_HOST_WRITE_BIT,
                .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED, // no ownership transfer
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .buffer = buf_x,
                .offset = 0,
                .size = VK_WHOLE_SIZE
            },{
                .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
                .srcAccessMask = VK_ACCESS_HOST_WRITE_BIT,
                .dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED, // no ownership transfer
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .buffer = buf_y,
                .offset = 0,
                .size = VK_WHOLE_SIZE
            }};

            VkBufferMemoryBarrier buf_out_barrier = {
                .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
                .srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT,
                .dstAccessMask = VK_ACCESS_HOST_READ_BIT,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED, // no ownership transfer
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .buffer = buf_y,
                .offset = 0,
                .size = VK_WHOLE_SIZE
            };

            VKCHECK(vkBeginCommandBuffer(cmd_buf, &cmd_buf_begin_info));
            {
                vkCmdPipelineBarrier(cmd_buf, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0, 0, NULL, 2, buf_in_barriers, 0, NULL);
                vkCmdBindPipeline(cmd_buf, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
                vkCmdPushConstants(cmd_buf, pipeline_layout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(float), &a);
                vkCmdBindDescriptorSets(cmd_buf, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline_layout, 0, 1, &desc_set, 0, NULL);
                vkCmdDispatch(cmd_buf, kMaxWorkGroupCountX, 1, 1);
                vkCmdPipelineBarrier(cmd_buf, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_HOST_BIT, 0, 0, NULL, 1, &buf_out_barrier, 0, NULL);
            }
            VKCHECK(vkEndCommandBuffer(cmd_buf));

            VkFenceCreateInfo fence_info = {
                .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
                .pNext = NULL,
                .flags = 0
            };
            VKCHECK(vkCreateFence(device, &fence_info, NULL, &fence));
            VkSubmitInfo submit_info = {
                .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                .pNext = NULL,
                .waitSemaphoreCount = 0,
                .pWaitSemaphores = NULL,
                .commandBufferCount = 1,
                .pCommandBuffers = &cmd_buf,
                .signalSemaphoreCount = 0,
                .pSignalSemaphores = NULL
            };
            VKCHECK(vkQueueSubmit(queue, 1, &submit_info, fence));

            VKCHECK(vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX));

            std::filesystem::path header_path = "./gltest_objres_objectResInfo_0.hpp";
            auto header_size = std::filesystem::file_size(header_path);
            std::string header_str(header_size, '\0');
            std::ifstream header_stream{header_path};
            header_stream.read(header_str.data(), header_size);
            return header_str;
        }

      private:
        VkInstance instance;
        VkDevice device;
        VkShaderModule shader_module;
        VkBuffer buf_x, buf_y;
        VkDeviceMemory mem_x, mem_y;
        VkDescriptorSetLayout buf_ds_layout;
        VkPipelineLayout pipeline_layout;
        VkPipelineCache pipeline_cache;
        VkPipeline pipeline;
        VkFence fence;

        size_t sizeof_push_constant;
    };

    class Cube {};

  protected:
};

TEST_F(GenLayerObjResTest, Compute) {
    TEST_DESCRIPTION("Tests whether generated object reservation counts for a compute pipeline are as expected");

    SAXPY saxpy;
    auto header = saxpy.run();

    EXPECT_EQ(header, std::string(R"(#ifndef gltest_objres_objectResInfo_HPP
#define gltest_objres_objectResInfo_HPP

#include <vulkan/vulkan_sc_core.h>

static VkDeviceObjectReservationCreateInfo g_objectResCreateInfo_0 {};
static void SetObjectResCreateInfo()
{
	g_objectResCreateInfo_0.sType                                      = VK_STRUCTURE_TYPE_DEVICE_OBJECT_RESERVATION_CREATE_INFO;
	g_objectResCreateInfo_0.pNext                                      = nullptr;
	g_objectResCreateInfo_0.semaphoreRequestCount                      = 0;
	g_objectResCreateInfo_0.commandBufferRequestCount                  = 1;
	g_objectResCreateInfo_0.fenceRequestCount                          = 1;
	g_objectResCreateInfo_0.deviceMemoryRequestCount                   = 2;
	g_objectResCreateInfo_0.bufferRequestCount                         = 2;
	g_objectResCreateInfo_0.imageRequestCount                          = 0;
	g_objectResCreateInfo_0.eventRequestCount                          = 0;
	g_objectResCreateInfo_0.queryPoolRequestCount                      = 0;
	g_objectResCreateInfo_0.bufferViewRequestCount                     = 0;
	g_objectResCreateInfo_0.imageViewRequestCount                      = 0;
	g_objectResCreateInfo_0.layeredImageViewRequestCount               = 0;
	g_objectResCreateInfo_0.pipelineCacheRequestCount                  = 1;
	g_objectResCreateInfo_0.pipelineLayoutRequestCount                 = 1;
	g_objectResCreateInfo_0.renderPassRequestCount                     = 0;
	g_objectResCreateInfo_0.graphicsPipelineRequestCount               = 0;
	g_objectResCreateInfo_0.computePipelineRequestCount                = 1;
	g_objectResCreateInfo_0.descriptorSetLayoutRequestCount            = 1;
	g_objectResCreateInfo_0.samplerRequestCount                        = 0;
	g_objectResCreateInfo_0.descriptorPoolRequestCount                 = 1;
	g_objectResCreateInfo_0.descriptorSetRequestCount                  = 1;
	g_objectResCreateInfo_0.framebufferRequestCount                    = 0;
	g_objectResCreateInfo_0.commandPoolRequestCount                    = 1;
	g_objectResCreateInfo_0.samplerYcbcrConversionRequestCount         = 0;
	g_objectResCreateInfo_0.swapchainRequestCount                      = 0;
	g_objectResCreateInfo_0.subpassDescriptionRequestCount             = 0;
	g_objectResCreateInfo_0.attachmentDescriptionRequestCount          = 0;
	g_objectResCreateInfo_0.descriptorSetLayoutBindingRequestCount     = 2;
	g_objectResCreateInfo_0.descriptorSetLayoutBindingLimit            = 2;
	g_objectResCreateInfo_0.maxImageViewMipLevels                      = 0;
	g_objectResCreateInfo_0.maxImageViewArrayLayers                    = 0;
	g_objectResCreateInfo_0.maxLayeredImageViewMipLevels               = 0;
	g_objectResCreateInfo_0.maxOcclusionQueriesPerPool                 = 0;
	g_objectResCreateInfo_0.maxPipelineStatisticsQueriesPerPool        = 0;
	g_objectResCreateInfo_0.maxTimestampQueriesPerPool                 = 0;
	g_objectResCreateInfo_0.maxImmutableSamplersPerDescriptorSetLayout = 0;
}

#endif
)"));
}