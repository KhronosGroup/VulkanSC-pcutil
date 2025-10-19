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

#include "vkresult_gtest_adapter.hpp"

#include <string>
#include <vector>
#include <array>

class UUID : public testing::Test {
  public:
    UUID() = default;
    UUID(const UUID&) = delete;
    UUID(UUID&&) = delete;
    ~UUID() = default;

    void TEST_DESCRIPTION(const char* desc) { RecordProperty("description", desc); }

    std::array<uint8_t, VK_UUID_SIZE> get_uuid(VkDevice device, VkPipeline pipeline) {
        PFN_vkGetPipelinePropertiesEXT vkGetPipelinePropertiesEXT =
            (PFN_vkGetPipelinePropertiesEXT)vkGetDeviceProcAddr(device, "vkGetPipelinePropertiesEXT");
        VKCHECK(vkGetPipelinePropertiesEXT ? VK_SUCCESS : VK_ERROR_EXTENSION_NOT_PRESENT);

        VkPipelineInfoEXT pipeline_info_ext{.sType = VK_STRUCTURE_TYPE_PIPELINE_INFO_EXT, .pNext = nullptr, .pipeline = pipeline};
        VkPipelinePropertiesIdentifierEXT pipeline_props{.sType = VK_STRUCTURE_TYPE_PIPELINE_PROPERTIES_IDENTIFIER_EXT,
                                                         .pNext = nullptr,
                                                         .pipelineIdentifier = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
        VKCHECK(vkGetPipelinePropertiesEXT(device, &pipeline_info_ext, reinterpret_cast<VkBaseOutStructure*>(&pipeline_props)));

        std::array<uint8_t, VK_UUID_SIZE> result;
        std::copy(pipeline_props.pipelineIdentifier, pipeline_props.pipelineIdentifier + VK_UUID_SIZE, result.begin());
        return result;
    }
};

TEST_F(UUID, ComputeSimple) {
    TEST_DESCRIPTION("Tests whether generated UUID for a compute pipeline is as expected");

    auto instance = SAXPY::create_instance();
    auto device = SAXPY::create_device(instance);

    SAXPY saxpy{instance, device};
    saxpy.run();

    auto uuid = get_uuid(device, saxpy.get_pipeline());

    std::array<uint8_t, VK_UUID_SIZE> ref{{192, 167, 88, 108, 121, 232, 13, 22, 245, 130, 62, 52, 149, 71, 254, 59}};
    EXPECT_EQ(uuid, ref);
}

TEST_F(UUID, ComputeReproducible) {
    TEST_DESCRIPTION("Tests whether generated UUIDs for identical compute pipelines match");

    auto instance = SAXPY::create_instance();
    auto device1 = SAXPY::create_device(instance);
    auto device2 = device1;
    auto device3 = SAXPY::create_device(instance);

    SAXPY saxpy1{instance, device1}, saxpy2{instance, device2}, saxpy3{instance, device3};

    saxpy1.run();
    saxpy2.run();
    saxpy3.run();

    auto uuid1 = get_uuid(device1, saxpy1.get_pipeline());
    auto uuid2 = get_uuid(device2, saxpy2.get_pipeline());
    auto uuid3 = get_uuid(device3, saxpy3.get_pipeline());

    EXPECT_EQ(uuid1, uuid2);
    EXPECT_EQ(uuid1, uuid3);
}

TEST_F(UUID, DifferentCompute) {
    TEST_DESCRIPTION("Tests whether generated UUIDs for different compute pipelines differ");

    auto instance = SAXPY::create_instance();
    auto device1 = SAXPY::create_device(instance);
    auto device2 = device1;

    SAXPY saxpy1{instance, device1}, saxpy2{instance, device2, 8};

    saxpy1.run();
    saxpy2.run();

    auto uuid1 = get_uuid(device1, saxpy1.get_pipeline());
    auto uuid2 = get_uuid(device2, saxpy2.get_pipeline());

    EXPECT_NE(uuid1, uuid2);
}
