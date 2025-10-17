/*
 * Copyright (c) 2025 The Khronos Group Inc.
 * Copyright (c) 2025 RasterGrid Kft.
 * Copyright (c) 2021, NVIDIA CORPORATION
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <vulkan/vulkan.h>

#include <gtest/gtest.h>

#include "saxpy.hpp"
#include "cube.hpp"

#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <regex>

class GenLayerObjResTest : public testing::Test {
  public:
    GenLayerObjResTest() {
        std::for_each(std::filesystem::directory_iterator{"."}, std::filesystem::directory_iterator{},
                      [](const std::filesystem::directory_entry& entry) {
                          if (std::regex_search(entry.path().generic_string(), std::regex{R"(gltest_objres_objectResInfo_)"})) {
                              std::filesystem::remove(entry);
                          }
                      });
    }
    GenLayerObjResTest(const GenLayerObjResTest&) = delete;
    GenLayerObjResTest(GenLayerObjResTest&&) = delete;
    ~GenLayerObjResTest() = default;

    void TEST_DESCRIPTION(const char* desc) { RecordProperty("description", desc); }

    std::string get_header(size_t i) {
        std::filesystem::path header_path = std::string("./gltest_objres_objectResInfo_") + std::to_string(i) + ".hpp";
        auto header_size = std::filesystem::file_size(header_path);
        std::string header_str(header_size, '\0');
        std::ifstream header_stream{header_path};
        header_stream.read(header_str.data(), header_size);
        return header_str;
    }

    void write_id(std::string& ref, int32_t id) {
        for(auto pos = ref.find('@', 0); pos != std::string::npos; pos = ref.find('@', pos) ) {
            ref.replace(pos, 1, std::to_string(id));
        }
    }

  protected:
    static inline int32_t device_counter = 0;
};

TEST_F(GenLayerObjResTest, ComputeSimple) {
    TEST_DESCRIPTION("Tests whether generated object reservation counts for a compute pipeline are as expected");

    auto instance = SAXPY::create_instance();
    auto device = SAXPY::create_device(instance);
    auto actual_counter = device_counter++;

    {
        SAXPY saxpy{instance, device};
        saxpy.run();
    }
    vkDestroyDevice(device, nullptr);
    vkDestroyInstance(instance, nullptr);

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
	g_objectResCreateInfo_@.commandBufferRequestCount                  = 1;
	g_objectResCreateInfo_@.fenceRequestCount                          = 1;
	g_objectResCreateInfo_@.deviceMemoryRequestCount                   = 2;
	g_objectResCreateInfo_@.bufferRequestCount                         = 2;
	g_objectResCreateInfo_@.imageRequestCount                          = 0;
	g_objectResCreateInfo_@.eventRequestCount                          = 0;
	g_objectResCreateInfo_@.queryPoolRequestCount                      = 0;
	g_objectResCreateInfo_@.bufferViewRequestCount                     = 0;
	g_objectResCreateInfo_@.imageViewRequestCount                      = 0;
	g_objectResCreateInfo_@.layeredImageViewRequestCount               = 0;
	g_objectResCreateInfo_@.pipelineCacheRequestCount                  = 1;
	g_objectResCreateInfo_@.pipelineLayoutRequestCount                 = 1;
	g_objectResCreateInfo_@.renderPassRequestCount                     = 0;
	g_objectResCreateInfo_@.graphicsPipelineRequestCount               = 0;
	g_objectResCreateInfo_@.computePipelineRequestCount                = 1;
	g_objectResCreateInfo_@.descriptorSetLayoutRequestCount            = 1;
	g_objectResCreateInfo_@.samplerRequestCount                        = 0;
	g_objectResCreateInfo_@.descriptorPoolRequestCount                 = 1;
	g_objectResCreateInfo_@.descriptorSetRequestCount                  = 1;
	g_objectResCreateInfo_@.framebufferRequestCount                    = 0;
	g_objectResCreateInfo_@.commandPoolRequestCount                    = 1;
	g_objectResCreateInfo_@.samplerYcbcrConversionRequestCount         = 0;
	g_objectResCreateInfo_@.swapchainRequestCount                      = 0;
	g_objectResCreateInfo_@.subpassDescriptionRequestCount             = 0;
	g_objectResCreateInfo_@.attachmentDescriptionRequestCount          = 0;
	g_objectResCreateInfo_@.descriptorSetLayoutBindingRequestCount     = 2;
	g_objectResCreateInfo_@.descriptorSetLayoutBindingLimit            = 2;
	g_objectResCreateInfo_@.maxImageViewMipLevels                      = 0;
	g_objectResCreateInfo_@.maxImageViewArrayLayers                    = 0;
	g_objectResCreateInfo_@.maxLayeredImageViewMipLevels               = 0;
	g_objectResCreateInfo_@.maxOcclusionQueriesPerPool                 = 0;
	g_objectResCreateInfo_@.maxPipelineStatisticsQueriesPerPool        = 0;
	g_objectResCreateInfo_@.maxTimestampQueriesPerPool                 = 0;
	g_objectResCreateInfo_@.maxImmutableSamplersPerDescriptorSetLayout = 0;
}

#endif
)"};
    write_id(ref, actual_counter);
    EXPECT_EQ(header, ref);
}

TEST_F(GenLayerObjResTest, ComputeMultiDeviceHighWatermark) {
    TEST_DESCRIPTION("Tests generated object reservation counts for a compute pipelines in a simple multi-device scenario");

    auto instance = SAXPY::create_instance();
    auto device1 = SAXPY::create_device(instance);
    auto device_counter1 = device_counter++;
    auto device2 = SAXPY::create_device(instance);
    auto device_counter2 = device_counter++;

    {
        SAXPY saxpy{instance, device2};
        saxpy.run();

        {
            SAXPY saxpy2{instance, device2};
            saxpy2.run();
        }

        SAXPY saxpy2{instance, device2};
        saxpy2.run();
        SAXPY saxpy3{instance, device1};
        saxpy3.run();
        SAXPY saxpy4{instance, device2};
        saxpy4.run();
    }
    vkDestroyDevice(device2, nullptr);
    vkDestroyDevice(device1, nullptr);
    vkDestroyInstance(instance, nullptr);

    auto header1 = get_header(device_counter1);
    std::string ref1 = {R"(#ifndef gltest_objres_objectResInfo_@_HPP
#define gltest_objres_objectResInfo_@_HPP

#include <vulkan/vulkan_sc_core.h>

static VkDeviceObjectReservationCreateInfo g_objectResCreateInfo_@ {};
static void SetObjectResCreateInfo()
{
	g_objectResCreateInfo_@.sType                                      = VK_STRUCTURE_TYPE_DEVICE_OBJECT_RESERVATION_CREATE_INFO;
	g_objectResCreateInfo_@.pNext                                      = nullptr;
	g_objectResCreateInfo_@.semaphoreRequestCount                      = 0;
	g_objectResCreateInfo_@.commandBufferRequestCount                  = 1;
	g_objectResCreateInfo_@.fenceRequestCount                          = 1;
	g_objectResCreateInfo_@.deviceMemoryRequestCount                   = 2;
	g_objectResCreateInfo_@.bufferRequestCount                         = 2;
	g_objectResCreateInfo_@.imageRequestCount                          = 0;
	g_objectResCreateInfo_@.eventRequestCount                          = 0;
	g_objectResCreateInfo_@.queryPoolRequestCount                      = 0;
	g_objectResCreateInfo_@.bufferViewRequestCount                     = 0;
	g_objectResCreateInfo_@.imageViewRequestCount                      = 0;
	g_objectResCreateInfo_@.layeredImageViewRequestCount               = 0;
	g_objectResCreateInfo_@.pipelineCacheRequestCount                  = 1;
	g_objectResCreateInfo_@.pipelineLayoutRequestCount                 = 1;
	g_objectResCreateInfo_@.renderPassRequestCount                     = 0;
	g_objectResCreateInfo_@.graphicsPipelineRequestCount               = 0;
	g_objectResCreateInfo_@.computePipelineRequestCount                = 1;
	g_objectResCreateInfo_@.descriptorSetLayoutRequestCount            = 1;
	g_objectResCreateInfo_@.samplerRequestCount                        = 0;
	g_objectResCreateInfo_@.descriptorPoolRequestCount                 = 1;
	g_objectResCreateInfo_@.descriptorSetRequestCount                  = 1;
	g_objectResCreateInfo_@.framebufferRequestCount                    = 0;
	g_objectResCreateInfo_@.commandPoolRequestCount                    = 1;
	g_objectResCreateInfo_@.samplerYcbcrConversionRequestCount         = 0;
	g_objectResCreateInfo_@.swapchainRequestCount                      = 0;
	g_objectResCreateInfo_@.subpassDescriptionRequestCount             = 0;
	g_objectResCreateInfo_@.attachmentDescriptionRequestCount          = 0;
	g_objectResCreateInfo_@.descriptorSetLayoutBindingRequestCount     = 2;
	g_objectResCreateInfo_@.descriptorSetLayoutBindingLimit            = 2;
	g_objectResCreateInfo_@.maxImageViewMipLevels                      = 0;
	g_objectResCreateInfo_@.maxImageViewArrayLayers                    = 0;
	g_objectResCreateInfo_@.maxLayeredImageViewMipLevels               = 0;
	g_objectResCreateInfo_@.maxOcclusionQueriesPerPool                 = 0;
	g_objectResCreateInfo_@.maxPipelineStatisticsQueriesPerPool        = 0;
	g_objectResCreateInfo_@.maxTimestampQueriesPerPool                 = 0;
	g_objectResCreateInfo_@.maxImmutableSamplersPerDescriptorSetLayout = 0;
}

#endif
)"};
    write_id(ref1, device_counter1);
    EXPECT_EQ(header1, ref1);

    auto header2 = get_header(device_counter2);
    std::string ref2 = {R"(#ifndef gltest_objres_objectResInfo_@_HPP
#define gltest_objres_objectResInfo_@_HPP

#include <vulkan/vulkan_sc_core.h>

static VkDeviceObjectReservationCreateInfo g_objectResCreateInfo_@ {};
static void SetObjectResCreateInfo()
{
	g_objectResCreateInfo_@.sType                                      = VK_STRUCTURE_TYPE_DEVICE_OBJECT_RESERVATION_CREATE_INFO;
	g_objectResCreateInfo_@.pNext                                      = nullptr;
	g_objectResCreateInfo_@.semaphoreRequestCount                      = 0;
	g_objectResCreateInfo_@.commandBufferRequestCount                  = 4;
	g_objectResCreateInfo_@.fenceRequestCount                          = 3;
	g_objectResCreateInfo_@.deviceMemoryRequestCount                   = 8;
	g_objectResCreateInfo_@.bufferRequestCount                         = 6;
	g_objectResCreateInfo_@.imageRequestCount                          = 0;
	g_objectResCreateInfo_@.eventRequestCount                          = 0;
	g_objectResCreateInfo_@.queryPoolRequestCount                      = 0;
	g_objectResCreateInfo_@.bufferViewRequestCount                     = 0;
	g_objectResCreateInfo_@.imageViewRequestCount                      = 0;
	g_objectResCreateInfo_@.layeredImageViewRequestCount               = 0;
	g_objectResCreateInfo_@.pipelineCacheRequestCount                  = 3;
	g_objectResCreateInfo_@.pipelineLayoutRequestCount                 = 3;
	g_objectResCreateInfo_@.renderPassRequestCount                     = 0;
	g_objectResCreateInfo_@.graphicsPipelineRequestCount               = 0;
	g_objectResCreateInfo_@.computePipelineRequestCount                = 3;
	g_objectResCreateInfo_@.descriptorSetLayoutRequestCount            = 3;
	g_objectResCreateInfo_@.samplerRequestCount                        = 0;
	g_objectResCreateInfo_@.descriptorPoolRequestCount                 = 4;
	g_objectResCreateInfo_@.descriptorSetRequestCount                  = 4;
	g_objectResCreateInfo_@.framebufferRequestCount                    = 0;
	g_objectResCreateInfo_@.commandPoolRequestCount                    = 4;
	g_objectResCreateInfo_@.samplerYcbcrConversionRequestCount         = 0;
	g_objectResCreateInfo_@.swapchainRequestCount                      = 0;
	g_objectResCreateInfo_@.subpassDescriptionRequestCount             = 0;
	g_objectResCreateInfo_@.attachmentDescriptionRequestCount          = 0;
	g_objectResCreateInfo_@.descriptorSetLayoutBindingRequestCount     = 6;
	g_objectResCreateInfo_@.descriptorSetLayoutBindingLimit            = 2;
	g_objectResCreateInfo_@.maxImageViewMipLevels                      = 0;
	g_objectResCreateInfo_@.maxImageViewArrayLayers                    = 0;
	g_objectResCreateInfo_@.maxLayeredImageViewMipLevels               = 0;
	g_objectResCreateInfo_@.maxOcclusionQueriesPerPool                 = 0;
	g_objectResCreateInfo_@.maxPipelineStatisticsQueriesPerPool        = 0;
	g_objectResCreateInfo_@.maxTimestampQueriesPerPool                 = 0;
	g_objectResCreateInfo_@.maxImmutableSamplersPerDescriptorSetLayout = 0;
}

#endif
)"};
    write_id(ref2, device_counter2);
    EXPECT_EQ(header2, ref2);
}

TEST_F(GenLayerObjResTest, GraphicsSimple) {
    TEST_DESCRIPTION("Tests whether generated object reservation counts for a graphics pipeline are as expected");

    {
        auto cube = Cube{};
        cube.run();
    }

    auto actual_counter = device_counter++;
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
	g_objectResCreateInfo_@.commandBufferRequestCount                  = 2;
	g_objectResCreateInfo_@.fenceRequestCount                          = 3;
	g_objectResCreateInfo_@.deviceMemoryRequestCount                   = 6;
	g_objectResCreateInfo_@.bufferRequestCount                         = 3;
	g_objectResCreateInfo_@.imageRequestCount                          = 3;
	g_objectResCreateInfo_@.eventRequestCount                          = 0;
	g_objectResCreateInfo_@.queryPoolRequestCount                      = 0;
	g_objectResCreateInfo_@.bufferViewRequestCount                     = 0;
	g_objectResCreateInfo_@.imageViewRequestCount                      = 4;
	g_objectResCreateInfo_@.layeredImageViewRequestCount               = 0;
	g_objectResCreateInfo_@.pipelineCacheRequestCount                  = 1;
	g_objectResCreateInfo_@.pipelineLayoutRequestCount                 = 1;
	g_objectResCreateInfo_@.renderPassRequestCount                     = 1;
	g_objectResCreateInfo_@.graphicsPipelineRequestCount               = 1;
	g_objectResCreateInfo_@.computePipelineRequestCount                = 0;
	g_objectResCreateInfo_@.descriptorSetLayoutRequestCount            = 1;
	g_objectResCreateInfo_@.samplerRequestCount                        = 1;
	g_objectResCreateInfo_@.descriptorPoolRequestCount                 = 1;
	g_objectResCreateInfo_@.descriptorSetRequestCount                  = 1;
	g_objectResCreateInfo_@.framebufferRequestCount                    = 1;
	g_objectResCreateInfo_@.commandPoolRequestCount                    = 1;
	g_objectResCreateInfo_@.samplerYcbcrConversionRequestCount         = 0;
	g_objectResCreateInfo_@.swapchainRequestCount                      = 0;
	g_objectResCreateInfo_@.subpassDescriptionRequestCount             = 0;
	g_objectResCreateInfo_@.attachmentDescriptionRequestCount          = 0;
	g_objectResCreateInfo_@.descriptorSetLayoutBindingRequestCount     = 2;
	g_objectResCreateInfo_@.descriptorSetLayoutBindingLimit            = 2;
	g_objectResCreateInfo_@.maxImageViewMipLevels                      = 1;
	g_objectResCreateInfo_@.maxImageViewArrayLayers                    = 1;
	g_objectResCreateInfo_@.maxLayeredImageViewMipLevels               = 0;
	g_objectResCreateInfo_@.maxOcclusionQueriesPerPool                 = 0;
	g_objectResCreateInfo_@.maxPipelineStatisticsQueriesPerPool        = 0;
	g_objectResCreateInfo_@.maxTimestampQueriesPerPool                 = 0;
	g_objectResCreateInfo_@.maxImmutableSamplersPerDescriptorSetLayout = 0;
}

#endif
)"};
    write_id(ref, actual_counter);
    EXPECT_EQ(header, ref);
}
