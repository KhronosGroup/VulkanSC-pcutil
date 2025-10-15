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

class GenLayerObjResTest : public testing::Test {
  public:
    GenLayerObjResTest() = default;
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

  protected:
    static inline int32_t device_counter = -1;
};

TEST_F(GenLayerObjResTest, ComputeSimple) {
    TEST_DESCRIPTION("Tests whether generated object reservation counts for a compute pipeline are as expected");

    auto instance = SAXPY::create_instance();
    auto device = SAXPY::create_device(instance);
    ++device_counter;

    {
        SAXPY saxpy{instance, device};
        saxpy.run();
    }
    vkDestroyDevice(device, nullptr);
    vkDestroyInstance(instance, nullptr);

    auto header = get_header(device_counter);

    std::string ref = R"(#ifndef gltest_objres_objectResInfo_0_HPP
#define gltest_objres_objectResInfo_0_HPP

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
)";
    EXPECT_EQ(header, ref);
}

TEST_F(GenLayerObjResTest, ComputeMultiDeviceHighWatermark) {
    TEST_DESCRIPTION("Tests generated object reservation counts for a compute pipelines in a simple multi-device scenario");

    auto instance = SAXPY::create_instance();
    auto device1 = SAXPY::create_device(instance);
    auto device_counter1 = ++device_counter;
    auto device2 = SAXPY::create_device(instance);
    auto device_counter2 = ++device_counter;

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
    std::string ref1 = R"(#ifndef gltest_objres_objectResInfo_1_HPP
#define gltest_objres_objectResInfo_1_HPP

#include <vulkan/vulkan_sc_core.h>

static VkDeviceObjectReservationCreateInfo g_objectResCreateInfo_1 {};
static void SetObjectResCreateInfo()
{
	g_objectResCreateInfo_1.sType                                      = VK_STRUCTURE_TYPE_DEVICE_OBJECT_RESERVATION_CREATE_INFO;
	g_objectResCreateInfo_1.pNext                                      = nullptr;
	g_objectResCreateInfo_1.semaphoreRequestCount                      = 0;
	g_objectResCreateInfo_1.commandBufferRequestCount                  = 1;
	g_objectResCreateInfo_1.fenceRequestCount                          = 1;
	g_objectResCreateInfo_1.deviceMemoryRequestCount                   = 2;
	g_objectResCreateInfo_1.bufferRequestCount                         = 2;
	g_objectResCreateInfo_1.imageRequestCount                          = 0;
	g_objectResCreateInfo_1.eventRequestCount                          = 0;
	g_objectResCreateInfo_1.queryPoolRequestCount                      = 0;
	g_objectResCreateInfo_1.bufferViewRequestCount                     = 0;
	g_objectResCreateInfo_1.imageViewRequestCount                      = 0;
	g_objectResCreateInfo_1.layeredImageViewRequestCount               = 0;
	g_objectResCreateInfo_1.pipelineCacheRequestCount                  = 1;
	g_objectResCreateInfo_1.pipelineLayoutRequestCount                 = 1;
	g_objectResCreateInfo_1.renderPassRequestCount                     = 0;
	g_objectResCreateInfo_1.graphicsPipelineRequestCount               = 0;
	g_objectResCreateInfo_1.computePipelineRequestCount                = 1;
	g_objectResCreateInfo_1.descriptorSetLayoutRequestCount            = 1;
	g_objectResCreateInfo_1.samplerRequestCount                        = 0;
	g_objectResCreateInfo_1.descriptorPoolRequestCount                 = 1;
	g_objectResCreateInfo_1.descriptorSetRequestCount                  = 1;
	g_objectResCreateInfo_1.framebufferRequestCount                    = 0;
	g_objectResCreateInfo_1.commandPoolRequestCount                    = 1;
	g_objectResCreateInfo_1.samplerYcbcrConversionRequestCount         = 0;
	g_objectResCreateInfo_1.swapchainRequestCount                      = 0;
	g_objectResCreateInfo_1.subpassDescriptionRequestCount             = 0;
	g_objectResCreateInfo_1.attachmentDescriptionRequestCount          = 0;
	g_objectResCreateInfo_1.descriptorSetLayoutBindingRequestCount     = 2;
	g_objectResCreateInfo_1.descriptorSetLayoutBindingLimit            = 2;
	g_objectResCreateInfo_1.maxImageViewMipLevels                      = 0;
	g_objectResCreateInfo_1.maxImageViewArrayLayers                    = 0;
	g_objectResCreateInfo_1.maxLayeredImageViewMipLevels               = 0;
	g_objectResCreateInfo_1.maxOcclusionQueriesPerPool                 = 0;
	g_objectResCreateInfo_1.maxPipelineStatisticsQueriesPerPool        = 0;
	g_objectResCreateInfo_1.maxTimestampQueriesPerPool                 = 0;
	g_objectResCreateInfo_1.maxImmutableSamplersPerDescriptorSetLayout = 0;
}

#endif
)";
    EXPECT_EQ(header1, ref1);

    auto header2 = get_header(device_counter2);
    std::string ref2 = R"(#ifndef gltest_objres_objectResInfo_2_HPP
#define gltest_objres_objectResInfo_2_HPP

#include <vulkan/vulkan_sc_core.h>

static VkDeviceObjectReservationCreateInfo g_objectResCreateInfo_2 {};
static void SetObjectResCreateInfo()
{
	g_objectResCreateInfo_2.sType                                      = VK_STRUCTURE_TYPE_DEVICE_OBJECT_RESERVATION_CREATE_INFO;
	g_objectResCreateInfo_2.pNext                                      = nullptr;
	g_objectResCreateInfo_2.semaphoreRequestCount                      = 0;
	g_objectResCreateInfo_2.commandBufferRequestCount                  = 4;
	g_objectResCreateInfo_2.fenceRequestCount                          = 3;
	g_objectResCreateInfo_2.deviceMemoryRequestCount                   = 8;
	g_objectResCreateInfo_2.bufferRequestCount                         = 6;
	g_objectResCreateInfo_2.imageRequestCount                          = 0;
	g_objectResCreateInfo_2.eventRequestCount                          = 0;
	g_objectResCreateInfo_2.queryPoolRequestCount                      = 0;
	g_objectResCreateInfo_2.bufferViewRequestCount                     = 0;
	g_objectResCreateInfo_2.imageViewRequestCount                      = 0;
	g_objectResCreateInfo_2.layeredImageViewRequestCount               = 0;
	g_objectResCreateInfo_2.pipelineCacheRequestCount                  = 3;
	g_objectResCreateInfo_2.pipelineLayoutRequestCount                 = 3;
	g_objectResCreateInfo_2.renderPassRequestCount                     = 0;
	g_objectResCreateInfo_2.graphicsPipelineRequestCount               = 0;
	g_objectResCreateInfo_2.computePipelineRequestCount                = 3;
	g_objectResCreateInfo_2.descriptorSetLayoutRequestCount            = 3;
	g_objectResCreateInfo_2.samplerRequestCount                        = 0;
	g_objectResCreateInfo_2.descriptorPoolRequestCount                 = 4;
	g_objectResCreateInfo_2.descriptorSetRequestCount                  = 4;
	g_objectResCreateInfo_2.framebufferRequestCount                    = 0;
	g_objectResCreateInfo_2.commandPoolRequestCount                    = 4;
	g_objectResCreateInfo_2.samplerYcbcrConversionRequestCount         = 0;
	g_objectResCreateInfo_2.swapchainRequestCount                      = 0;
	g_objectResCreateInfo_2.subpassDescriptionRequestCount             = 0;
	g_objectResCreateInfo_2.attachmentDescriptionRequestCount          = 0;
	g_objectResCreateInfo_2.descriptorSetLayoutBindingRequestCount     = 6;
	g_objectResCreateInfo_2.descriptorSetLayoutBindingLimit            = 2;
	g_objectResCreateInfo_2.maxImageViewMipLevels                      = 0;
	g_objectResCreateInfo_2.maxImageViewArrayLayers                    = 0;
	g_objectResCreateInfo_2.maxLayeredImageViewMipLevels               = 0;
	g_objectResCreateInfo_2.maxOcclusionQueriesPerPool                 = 0;
	g_objectResCreateInfo_2.maxPipelineStatisticsQueriesPerPool        = 0;
	g_objectResCreateInfo_2.maxTimestampQueriesPerPool                 = 0;
	g_objectResCreateInfo_2.maxImmutableSamplersPerDescriptorSetLayout = 0;
}

#endif
)";
    EXPECT_EQ(header2, ref2);
}

TEST_F(GenLayerObjResTest, GraphicsSimple) {
    TEST_DESCRIPTION("Tests whether generated object reservation counts for a graphics pipeline are as expected");

    {
        auto cube = Cube{};
        ++device_counter;
        cube.run();
    }

    auto header = get_header(device_counter);

    std::string ref = R"(#ifndef gltest_objres_objectResInfo_3_HPP
#define gltest_objres_objectResInfo_3_HPP

#include <vulkan/vulkan_sc_core.h>

static VkDeviceObjectReservationCreateInfo g_objectResCreateInfo_3 {};
static void SetObjectResCreateInfo()
{
	g_objectResCreateInfo_3.sType                                      = VK_STRUCTURE_TYPE_DEVICE_OBJECT_RESERVATION_CREATE_INFO;
	g_objectResCreateInfo_3.pNext                                      = nullptr;
	g_objectResCreateInfo_3.semaphoreRequestCount                      = 0;
	g_objectResCreateInfo_3.commandBufferRequestCount                  = 2;
	g_objectResCreateInfo_3.fenceRequestCount                          = 3;
	g_objectResCreateInfo_3.deviceMemoryRequestCount                   = 6;
	g_objectResCreateInfo_3.bufferRequestCount                         = 3;
	g_objectResCreateInfo_3.imageRequestCount                          = 3;
	g_objectResCreateInfo_3.eventRequestCount                          = 0;
	g_objectResCreateInfo_3.queryPoolRequestCount                      = 0;
	g_objectResCreateInfo_3.bufferViewRequestCount                     = 0;
	g_objectResCreateInfo_3.imageViewRequestCount                      = 4;
	g_objectResCreateInfo_3.layeredImageViewRequestCount               = 0;
	g_objectResCreateInfo_3.pipelineCacheRequestCount                  = 1;
	g_objectResCreateInfo_3.pipelineLayoutRequestCount                 = 1;
	g_objectResCreateInfo_3.renderPassRequestCount                     = 1;
	g_objectResCreateInfo_3.graphicsPipelineRequestCount               = 1;
	g_objectResCreateInfo_3.computePipelineRequestCount                = 0;
	g_objectResCreateInfo_3.descriptorSetLayoutRequestCount            = 1;
	g_objectResCreateInfo_3.samplerRequestCount                        = 1;
	g_objectResCreateInfo_3.descriptorPoolRequestCount                 = 1;
	g_objectResCreateInfo_3.descriptorSetRequestCount                  = 1;
	g_objectResCreateInfo_3.framebufferRequestCount                    = 1;
	g_objectResCreateInfo_3.commandPoolRequestCount                    = 1;
	g_objectResCreateInfo_3.samplerYcbcrConversionRequestCount         = 0;
	g_objectResCreateInfo_3.swapchainRequestCount                      = 0;
	g_objectResCreateInfo_3.subpassDescriptionRequestCount             = 0;
	g_objectResCreateInfo_3.attachmentDescriptionRequestCount          = 0;
	g_objectResCreateInfo_3.descriptorSetLayoutBindingRequestCount     = 2;
	g_objectResCreateInfo_3.descriptorSetLayoutBindingLimit            = 2;
	g_objectResCreateInfo_3.maxImageViewMipLevels                      = 1;
	g_objectResCreateInfo_3.maxImageViewArrayLayers                    = 1;
	g_objectResCreateInfo_3.maxLayeredImageViewMipLevels               = 0;
	g_objectResCreateInfo_3.maxOcclusionQueriesPerPool                 = 0;
	g_objectResCreateInfo_3.maxPipelineStatisticsQueriesPerPool        = 0;
	g_objectResCreateInfo_3.maxTimestampQueriesPerPool                 = 0;
	g_objectResCreateInfo_3.maxImmutableSamplersPerDescriptorSetLayout = 0;
}

#endif
)";
    EXPECT_EQ(header, ref);
}
