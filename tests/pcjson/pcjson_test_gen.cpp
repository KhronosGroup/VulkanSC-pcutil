/*
 * Copyright (c) 2025 The Khronos Group Inc.
 * Copyright (c) 2025 RasterGrid Kft.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <vulkan/pcjson/vksc_pipeline_json.h>
#include <vulkan/utility/vk_struct_helper.hpp>
#include <vulkan/utility/vk_safe_struct.hpp>
#include <vulkan/vulkan_sc.h>

#include <gtest/gtest.h>

#include <assert.h>
#include <stddef.h>
#include <iostream>
#include <string.h>
#include <vector>
#include <string>
#include <limits>

#include <json/json.h>

#include "json_validator.h"
#include "json_struct_helpers.h"

using namespace std::string_literals;

class Gen : public testing::Test {
  public:
    Gen() : generator_{vpjCreateGenerator()}, msg_{nullptr} {}
    Gen(const Gen&) = delete;
    Gen(Gen&&) = delete;
    ~Gen() { vpjDestroyGenerator(generator_); }

    void TEST_DESCRIPTION(const char* desc) { RecordProperty("description", desc); }

    void CHECK_GEN(bool allow_warnings = false) {
        if (msg_ && std::strlen(msg_)) {
            if (!allow_warnings) {
                FAIL() << msg_;
            } else {
                if (strstr(msg_, "ERROR")) {
                    FAIL() << msg_;
                }
            }
        }
    }

  protected:
    VpjGenerator generator_;
    const char* msg_;
};

TEST_F(Gen, VkPhysicalDeviceFeatures2) {
    TEST_DESCRIPTION("Tests generating of a reasonably complex physical device features 2 JSON");

    for (auto seed : {0, 1, 2}) {
        auto [pdf_in, ref_json] = getVkPhysicalDeviceFeatures2(seed);

        const char* result_json = nullptr;
        EXPECT_TRUE(vpjGenerateSingleStructJson(generator_, &pdf_in, &result_json, &msg_));
        CHECK_GEN();
        EXPECT_TRUE(CompareJson(result_json, ref_json));
    }
}

TEST_F(Gen, VkGraphicsPipelineCreateInfo) {
    TEST_DESCRIPTION("Tests generating of a reasonably complex graphics pipeline create info JSON");

    for (auto seed : {0}) {
        auto [ci_in, ref_json] = getVkGraphicsPipelineCreateInfo(seed);

        const char* result_json = nullptr;
        EXPECT_TRUE(vpjGenerateSingleStructJson(generator_, &ci_in, &result_json, &msg_));
        CHECK_GEN();
        EXPECT_TRUE(CompareJson(result_json, ref_json));
    }
}

TEST_F(Gen, VkComputePipelineCreateInfo) {
    TEST_DESCRIPTION("Tests generating of a reasonably complex compute pipeline create info JSON");

    for (auto seed : {0, 1}) {
        auto [ci_in, ref_json] = getVkComputePipelineCreateInfo(seed);

        const char* result_json = nullptr;
        EXPECT_TRUE(vpjGenerateSingleStructJson(generator_, &ci_in, &result_json, &msg_));
        CHECK_GEN();
        EXPECT_TRUE(CompareJson(result_json, ref_json));
    }
}

TEST_F(Gen, VkSamplerYcbcrConversionCreateInfo) {
    TEST_DESCRIPTION("Tests generating of a reasonably complex ycbcr conversion create info JSON");

    for (auto seed : {0, 1, 2}) {
        auto [ci_in, ref_json] = getVkSamplerYcbcrConversionCreateInfo(seed);

        const char* result_json = nullptr;
        EXPECT_TRUE(vpjGenerateSingleStructJson(generator_, &ci_in, &result_json, &msg_));
        CHECK_GEN();
        EXPECT_TRUE(CompareJson(result_json, ref_json));
    }
}

TEST_F(Gen, VkSamplerCreateInfo) {
    TEST_DESCRIPTION("Tests generating of a reasonably complex sampler create info JSON");

    for (auto seed : {0, 1, 2}) {
        for (auto param : std::vector<SamplerParams>{{}, {VkSamplerYcbcrConversion(12345)}}) {
            auto [ci_in, ref_json] = getVkSamplerCreateInfo(seed, param);

            const char* result_json = nullptr;
            EXPECT_TRUE(vpjGenerateSingleStructJson(generator_, &ci_in, &result_json, &msg_));
            CHECK_GEN(true);
            EXPECT_TRUE(CompareJson(result_json, ref_json));
        }
    }
}

TEST_F(Gen, VkDescriptorSetLayoutCreateInfo) {
    TEST_DESCRIPTION("Tests generating of a reasonably complex descriptor set layout create info JSON");

    for (auto seed : {0, 1, 2, 3}) {
        auto [ci_in, ref_json] = getVkDescriptorSetLayoutCreateInfo(seed);

        const char* result_json = nullptr;
        EXPECT_TRUE(vpjGenerateSingleStructJson(generator_, &ci_in, &result_json, &msg_));
        CHECK_GEN();
        EXPECT_TRUE(CompareJson(result_json, ref_json));
    }
}

TEST_F(Gen, VkPipelineLayoutCreateInfo) {
    TEST_DESCRIPTION("Tests generating of a reasonably complex pipeline layout create info JSON");

    for (auto seed : {0, 1, 2, 3}) {
        auto [ci_in, ref_json] = getVkPipelineLayoutCreateInfo(seed);

        const char* result_json = nullptr;
        EXPECT_TRUE(vpjGenerateSingleStructJson(generator_, &ci_in, &result_json, &msg_));
        CHECK_GEN();
        EXPECT_TRUE(CompareJson(result_json, ref_json));
    }
}

TEST_F(Gen, VkRenderPassCreateInfo) {
    TEST_DESCRIPTION("Tests generating of a reasonably complex render pass create info JSON");

    for (auto seed : {0, 1, 2, 3}) {
        auto [ci_in, ref_json] = getVkRenderPassCreateInfo(seed);

        const char* result_json = nullptr;
        EXPECT_TRUE(vpjGenerateSingleStructJson(generator_, &ci_in, &result_json, &msg_));
        CHECK_GEN();
        EXPECT_TRUE(CompareJson(result_json, ref_json));
    }
}

TEST_F(Gen, VkRenderPassCreateInfo2) {
    TEST_DESCRIPTION("Tests generating of a reasonably complex render pass 2 layout create info JSON");

    for (auto seed : {0, 1}) {
        auto [ci_in, ref_json] = getVkRenderPassCreateInfo2(seed);

        const char* result_json = nullptr;
        EXPECT_TRUE(vpjGenerateSingleStructJson(generator_, &ci_in, &result_json, &msg_));
        CHECK_GEN();
        EXPECT_TRUE(CompareJson(result_json, ref_json));
    }
}

TEST_F(Gen, VkShaderModuleCreateInfo) {
    TEST_DESCRIPTION("Tests generating of a reasonably complex shader module create info JSON");

    for (auto seed : {0}) {  // Test case 1 only in parser. We never generate arrays for pCode.
        auto [ci_in, ref_json] = getVkShaderModuleCreateInfo(seed);

        const char* result_json = nullptr;
        EXPECT_TRUE(vpjGenerateSingleStructJson(generator_, &ci_in, &result_json, &msg_));
        CHECK_GEN();
        EXPECT_TRUE(CompareJson(result_json, ref_json));
    }
}

// TODO: Use getVkDeviceObjectReservationCreateInfo once safe struct issue is resolved.
// see: https://github.com/KhronosGroup/Vulkan-Utility-Libraries/issues/343
TEST_F(Gen, VkDeviceObjectReservationCreateInfo) {
    TEST_DESCRIPTION("Tests generating of a reasonably complex object reservation create info JSON");

    std::string ref_json = {R"({
        "sType" : "VK_STRUCTURE_TYPE_DEVICE_OBJECT_RESERVATION_CREATE_INFO",
        "pNext": "NULL",
        "pipelineCacheCreateInfoCount": 1,
        "pPipelineCacheCreateInfos": [
            {
                "sType": "VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO",
                "pNext": "NULL",
                "flags": "VK_PIPELINE_CACHE_CREATE_READ_ONLY_BIT | VK_PIPELINE_CACHE_CREATE_USE_APPLICATION_STORAGE_BIT",
                "initialDataSize": 3,
                "pInitialData": "abcd"
            }
        ],
        "pipelinePoolSizeCount": 1,
        "pPipelinePoolSizes": [
            {
                "sType": "VK_STRUCTURE_TYPE_PIPELINE_POOL_SIZE",
                "pNext": "NULL",
                "poolEntryCount": 1,
                "poolEntrySize": 1048576
            }
        ],
        "semaphoreRequestCount": 0,
        "commandBufferRequestCount": 1,
        "fenceRequestCount": 1,
        "deviceMemoryRequestCount": 2,
        "bufferRequestCount": 2,
        "imageRequestCount": 0,
        "eventRequestCount": 0,
        "queryPoolRequestCount": 0,
        "bufferViewRequestCount": 0,
        "imageViewRequestCount": 0,
        "layeredImageViewRequestCount": 0,
        "pipelineCacheRequestCount": 1,
        "pipelineLayoutRequestCount": 1,
        "renderPassRequestCount": 1,
        "graphicsPipelineRequestCount": 0,
        "computePipelineRequestCount": 1,
        "descriptorSetLayoutRequestCount": 1,
        "samplerRequestCount": 0,
        "descriptorPoolRequestCount": 1,
        "descriptorSetRequestCount": 1,
        "framebufferRequestCount": 0,
        "commandPoolRequestCount": 2,
        "samplerYcbcrConversionRequestCount": 0,
        "surfaceRequestCount": 0,
        "swapchainRequestCount": 1,
        "displayModeRequestCount": 0,
        "subpassDescriptionRequestCount": 1,
        "attachmentDescriptionRequestCount": 2,
        "descriptorSetLayoutBindingRequestCount": 2,
        "descriptorSetLayoutBindingLimit": 2,
        "maxImageViewMipLevels": 1,
        "maxImageViewArrayLayers": 1,
        "maxLayeredImageViewMipLevels": 0,
        "maxOcclusionQueriesPerPool": 0,
        "maxPipelineStatisticsQueriesPerPool": 0,
        "maxTimestampQueriesPerPool": 0,
        "maxImmutableSamplersPerDescriptorSetLayout": 0
    })"};

    VkDeviceObjectReservationCreateInfo dor_ci{};

    dor_ci.sType = VK_STRUCTURE_TYPE_DEVICE_OBJECT_RESERVATION_CREATE_INFO;
    dor_ci.pNext = nullptr;

    VkPipelineCacheCreateInfo pipelineCacheCreateInfos[1] = {};
    dor_ci.pipelineCacheCreateInfoCount = 1;
    pipelineCacheCreateInfos[0].sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    pipelineCacheCreateInfos[0].pNext = nullptr;
    pipelineCacheCreateInfos[0].flags =
        VK_PIPELINE_CACHE_CREATE_READ_ONLY_BIT | VK_PIPELINE_CACHE_CREATE_USE_APPLICATION_STORAGE_BIT;

    uint8_t initialData[3] = {0b01101001, 0b10110111, 0b00011101};
    pipelineCacheCreateInfos[0].initialDataSize = 3;
    pipelineCacheCreateInfos->pInitialData = initialData;

    dor_ci.pPipelineCacheCreateInfos = pipelineCacheCreateInfos;

    VkPipelinePoolSize pipelinePoolSizes[1] = {};
    dor_ci.pipelinePoolSizeCount = 1;
    pipelinePoolSizes[0].sType = VK_STRUCTURE_TYPE_PIPELINE_POOL_SIZE;
    pipelinePoolSizes[0].pNext = nullptr;
    pipelinePoolSizes[0].poolEntryCount = 1;
    pipelinePoolSizes[0].poolEntrySize = 1048576;
    dor_ci.pPipelinePoolSizes = pipelinePoolSizes;

    dor_ci.semaphoreRequestCount = 0;
    dor_ci.commandBufferRequestCount = 1;
    dor_ci.fenceRequestCount = 1;
    dor_ci.deviceMemoryRequestCount = 2;
    dor_ci.bufferRequestCount = 2;
    dor_ci.imageRequestCount = 0;
    dor_ci.eventRequestCount = 0;
    dor_ci.queryPoolRequestCount = 0;
    dor_ci.bufferViewRequestCount = 0;
    dor_ci.imageViewRequestCount = 0;
    dor_ci.layeredImageViewRequestCount = 0;
    dor_ci.pipelineCacheRequestCount = 1;
    dor_ci.pipelineLayoutRequestCount = 1;
    dor_ci.renderPassRequestCount = 1;
    dor_ci.graphicsPipelineRequestCount = 0;
    dor_ci.computePipelineRequestCount = 1;
    dor_ci.descriptorSetLayoutRequestCount = 1;
    dor_ci.samplerRequestCount = 0;
    dor_ci.descriptorPoolRequestCount = 1;
    dor_ci.descriptorSetRequestCount = 1;
    dor_ci.framebufferRequestCount = 0;
    dor_ci.commandPoolRequestCount = 2;
    dor_ci.samplerYcbcrConversionRequestCount = 0;
    dor_ci.surfaceRequestCount = 0;
    dor_ci.swapchainRequestCount = 1;
    dor_ci.displayModeRequestCount = 0;
    dor_ci.subpassDescriptionRequestCount = 1;
    dor_ci.attachmentDescriptionRequestCount = 2;
    dor_ci.descriptorSetLayoutBindingRequestCount = 2;
    dor_ci.descriptorSetLayoutBindingLimit = 2;
    dor_ci.maxImageViewMipLevels = 1;
    dor_ci.maxImageViewArrayLayers = 1;
    dor_ci.maxLayeredImageViewMipLevels = 0;
    dor_ci.maxOcclusionQueriesPerPool = 0;
    dor_ci.maxPipelineStatisticsQueriesPerPool = 0;
    dor_ci.maxTimestampQueriesPerPool = 0;
    dor_ci.maxImmutableSamplersPerDescriptorSetLayout = 0;

    const char* result_json = nullptr;

    EXPECT_TRUE(vpjGenerateSingleStructJson(generator_, &dor_ci, &result_json, &msg_));
    CHECK_GEN();
    EXPECT_TRUE(CompareJson(result_json, ref_json));
}

TEST_F(Gen, VkPipelineOfflineCreateInfo) {
    TEST_DESCRIPTION("Tests generating of a reasonably complex pipeline offline create info JSON");

    for (auto seed : {0}) {
        auto [ci_in, ref_json] = getVkPipelineOfflineCreateInfo(seed);

        const char* result_json = nullptr;
        EXPECT_TRUE(vpjGenerateSingleStructJson(generator_, &ci_in, &result_json, &msg_));
        CHECK_GEN();
        EXPECT_TRUE(CompareJson(result_json, ref_json));
    }
}

TEST_F(Gen, ComputePipelineJSON) {
    TEST_DESCRIPTION("Tests generation of a reasonably complex compute pipeline JSON");

    VpjData data{};

    const char* ycbcr_names[1] = {"YcbcrConversion1"};
    auto [ycbcr_ci, ycbcr_json] = getVkSamplerYcbcrConversionCreateInfo(0);

    const char* sampler_names[2] = {"ImmutableSampler1", "YcbcrSampler1"};
    auto [immut_sampler_ci, immut_sampler_json] = getVkSamplerCreateInfo(0);
    auto [ycbcr_sampler_ci, ycbcr_sampler_json] = getVkSamplerCreateInfo(0, {VkSamplerYcbcrConversion(0), ycbcr_names[0]});
    VkSamplerCreateInfo sampler_ci[2] = {*immut_sampler_ci.ptr(), *ycbcr_sampler_ci.ptr()};

    const char* dsl_names[1] = {"DescriptorSetLayout1"};
    auto [dsl_ci, dsl_json] =
        getVkDescriptorSetLayoutCreateInfo(0, {{VkSampler(0), sampler_names[0]}, {VkSampler(1), sampler_names[1]}});

    auto [pl_ci, pl_json] = getVkPipelineLayoutCreateInfo(0, {{VkDescriptorSetLayout(0), dsl_names[0]}});

    auto [cp_ci, cp_json] = getVkComputePipelineCreateInfo(0);

    auto [pdf, pdf_json] = getVkPhysicalDeviceFeatures2(0);

    auto [shaderFileNames, shaderFileNames_json] = getShaderFileNames({{VK_SHADER_STAGE_COMPUTE_BIT, "shader.comp.spv"}});

    const char* enabled_extensions[1] = {"VK_EXT_robustness2"};

    data.enabledExtensionCount = 1;
    data.ppEnabledExtensions = enabled_extensions;
    data.computePipelineState.pComputePipeline = &cp_ci;
    data.computePipelineState.pPipelineLayout = &pl_ci;
    data.computePipelineState.immutableSamplerCount = 2;
    data.computePipelineState.ppImmutableSamplerNames = sampler_names;
    data.computePipelineState.pImmutableSamplers = sampler_ci;
    data.computePipelineState.ycbcrSamplerCount = 1;
    data.computePipelineState.ppYcbcrSamplerNames = ycbcr_names;
    data.computePipelineState.pYcbcrSamplers = &ycbcr_ci;
    data.computePipelineState.descriptorSetLayoutCount = 1;
    data.computePipelineState.pDescriptorSetLayouts = &dsl_ci;
    data.computePipelineState.ppDescriptorSetLayoutNames = dsl_names;
    data.computePipelineState.pPhysicalDeviceFeatures = &pdf;
    data.computePipelineState.shaderFileNameCount = (uint32_t)shaderFileNames.size();
    data.computePipelineState.pShaderFileNames = shaderFileNames.data();

    data.pipelineUUID[0] = 85;
    data.pipelineUUID[1] = 43;
    data.pipelineUUID[2] = 255;
    data.pipelineUUID[3] = 24;
    data.pipelineUUID[4] = 155;
    data.pipelineUUID[5] = 64;
    data.pipelineUUID[6] = 62;
    data.pipelineUUID[7] = 24;

    const std::string ref_json = R"({
        "ComputePipelineState" :
        {
            "ComputePipeline" : )" +
                                 cp_json +
                                 R"(,
            "DescriptorSetLayouts" :
            [
                {
                    ")" + dsl_names[0] +
                                 R"(" : )" + dsl_json + R"(
                }
            ],
            "ImmutableSamplers" :
            [
                {
                    ")" + sampler_names[0] +
                                 R"(" : )" + immut_sampler_json + R"(
                },
                {
                    ")" + sampler_names[1] +
                                 R"(" : )" + ycbcr_sampler_json + R"(
                }
            ],
            "PhysicalDeviceFeatures" : )" +
                                 pdf_json + R"(,
            "PipelineLayout" : )" +
                                 pl_json +
                                 R"(,
            "ShaderFileNames" : )" +
                                 shaderFileNames_json +
                                 R"(,
            "YcbcrSamplers" :
            [
                {
                    ")" + ycbcr_names[0] +
                                 R"(" : )" + ycbcr_json + R"(
                }
            ]
        },
        "EnabledExtensions" :
        [
            "VK_EXT_robustness2"
        ],
        "PipelineUUID" : [85, 43, 255, 24, 155, 64, 62, 24, 0, 0, 0, 0, 0, 0, 0, 0]
    })";
    const char* result_json = nullptr;

    EXPECT_TRUE(vpjGeneratePipelineJson(generator_, &data, &result_json, &msg_));
    CHECK_GEN();
    EXPECT_TRUE(ValidatePipelineJson(std::string(result_json)));
    EXPECT_TRUE(CompareJson(result_json, ref_json));
}

TEST_F(Gen, GraphicsPipelineJSON) {
    TEST_DESCRIPTION("Tests generation of a reasonably complex graphics pipeline JSON");

    VpjData data{};

    const char* ycbcr_names[1] = {"YcbcrConversion1"};
    auto [ycbcr_ci, ycbcr_json] = getVkSamplerYcbcrConversionCreateInfo(1);

    const char* sampler_names[2] = {"ImmutableSampler1", "YcbcrSampler1"};
    auto [immut_sampler_ci, immut_sampler_json] = getVkSamplerCreateInfo(1);
    auto [ycbcr_sampler_ci, ycbcr_sampler_json] = getVkSamplerCreateInfo(1, {VkSamplerYcbcrConversion(0), ycbcr_names[0]});
    VkSamplerCreateInfo sampler_ci[2] = {*immut_sampler_ci.ptr(), *ycbcr_sampler_ci.ptr()};

    const char* dsl_names[1] = {"DescriptorSetLayout1"};
    auto [dsl_ci, dsl_json] =
        getVkDescriptorSetLayoutCreateInfo(1, {{VkSampler(0), sampler_names[0]}, {VkSampler(1), sampler_names[1]}});

    auto [pl_ci, pl_json] = getVkPipelineLayoutCreateInfo(1, {{VkDescriptorSetLayout(0), dsl_names[0]}});

    auto [renderPass, renderPass_json] = getVkRenderPassCreateInfo(1);

    auto [gp_ci, gp_json] = getVkGraphicsPipelineCreateInfo(1);

    auto [pdf, pdf_json] = getVkPhysicalDeviceFeatures2(1);

    auto [shaderFileNames, shaderFileNames_json] = getShaderFileNames({
        {VK_SHADER_STAGE_VERTEX_BIT, "shader.vert.spv"},
        {VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT, "shader.tess_ctrl.spv"},
        {VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT, "shader.tess_eval.spv"},
        {VK_SHADER_STAGE_FRAGMENT_BIT, "shader.frag.spv"},
    });

    const char* enabled_extensions[1] = {"VK_EXT_robustness2"};

    data.enabledExtensionCount = 1;
    data.ppEnabledExtensions = enabled_extensions;
    data.graphicsPipelineState.pGraphicsPipeline = &gp_ci;
    data.graphicsPipelineState.pRenderPass = &renderPass;
    data.graphicsPipelineState.pPipelineLayout = &pl_ci;
    data.graphicsPipelineState.immutableSamplerCount = 2;
    data.graphicsPipelineState.ppImmutableSamplerNames = sampler_names;
    data.graphicsPipelineState.pImmutableSamplers = sampler_ci;
    data.graphicsPipelineState.ycbcrSamplerCount = 1;
    data.graphicsPipelineState.ppYcbcrSamplerNames = ycbcr_names;
    data.graphicsPipelineState.pYcbcrSamplers = &ycbcr_ci;
    data.graphicsPipelineState.descriptorSetLayoutCount = 1;
    data.graphicsPipelineState.pDescriptorSetLayouts = &dsl_ci;
    data.graphicsPipelineState.ppDescriptorSetLayoutNames = dsl_names;
    data.graphicsPipelineState.pPhysicalDeviceFeatures = &pdf;
    data.graphicsPipelineState.shaderFileNameCount = (uint32_t)shaderFileNames.size();
    data.graphicsPipelineState.pShaderFileNames = shaderFileNames.data();

    data.pipelineUUID[0] = 85;
    data.pipelineUUID[1] = 43;
    data.pipelineUUID[2] = 255;
    data.pipelineUUID[3] = 24;
    data.pipelineUUID[4] = 155;
    data.pipelineUUID[5] = 64;
    data.pipelineUUID[6] = 62;
    data.pipelineUUID[7] = 24;

    const std::string ref_json = R"({
        "GraphicsPipelineState" :
        {
            "Renderpass": )" + renderPass_json +
                                 R"(,
            "GraphicsPipeline" : )" +
                                 gp_json +
                                 R"(,
            "DescriptorSetLayouts" :
            [
                {
                    ")" + dsl_names[0] +
                                 R"(" : )" + dsl_json + R"(
                }
            ],
            "ImmutableSamplers" :
            [
                {
                    ")" + sampler_names[0] +
                                 R"(" : )" + immut_sampler_json + R"(
                },
                {
                    ")" + sampler_names[1] +
                                 R"(" : )" + ycbcr_sampler_json + R"(
                }
            ],
            "PhysicalDeviceFeatures" : )" +
                                 pdf_json + R"(,
            "PipelineLayout" : )" +
                                 pl_json +
                                 R"(,
            "ShaderFileNames" : )" +
                                 shaderFileNames_json +
                                 R"(,
            "YcbcrSamplers" :
            [
                {
                    ")" + ycbcr_names[0] +
                                 R"(" : )" + ycbcr_json + R"(
                }
            ]
        },
        "EnabledExtensions" :
        [
            "VK_EXT_robustness2"
        ],
        "PipelineUUID" : [85, 43, 255, 24, 155, 64, 62, 24, 0, 0, 0, 0, 0, 0, 0, 0]
    })";
    const char* result_json = nullptr;

    EXPECT_TRUE(vpjGeneratePipelineJson(generator_, &data, &result_json, &msg_));
    CHECK_GEN();
    EXPECT_TRUE(ValidatePipelineJson(std::string(result_json)));
    EXPECT_TRUE(CompareJson(result_json, ref_json));
}

TEST_F(Gen, ComputePipelineJSONWithMD5) {
    TEST_DESCRIPTION("Tests generating pipeline JSON with MD5 UUID for a compute pipeline");

    VpjData data{};

    const char* ycbcr_names[1] = {"YcbcrConversion1"};
    auto [ycbcr_ci, ycbcr_json] = getVkSamplerYcbcrConversionCreateInfo(0);

    const char* sampler_names[2] = {"ImmutableSampler1", "YcbcrSampler1"};
    auto [immut_sampler_ci, immut_sampler_json] = getVkSamplerCreateInfo(0);
    auto [ycbcr_sampler_ci, ycbcr_sampler_json] = getVkSamplerCreateInfo(0, {VkSamplerYcbcrConversion(0), ycbcr_names[0]});
    VkSamplerCreateInfo sampler_ci[2] = {*immut_sampler_ci.ptr(), *ycbcr_sampler_ci.ptr()};

    const char* dsl_names[1] = {"DescriptorSetLayout1"};
    auto [dsl_ci, dsl_json] =
        getVkDescriptorSetLayoutCreateInfo(0, {{VkSampler(0), sampler_names[0]}, {VkSampler(1), sampler_names[1]}});

    auto [pl_ci, pl_json] = getVkPipelineLayoutCreateInfo(0, {{VkDescriptorSetLayout(0), dsl_names[0]}});

    auto [cp_ci, cp_json] = getVkComputePipelineCreateInfo(0);

    auto [pdf, pdf_json] = getVkPhysicalDeviceFeatures2(0);

    std::vector<uint32_t> ref_spirv{1, 2, 3, 4};
    auto [shaderFileNames, shaderFileNames_json] = getShaderFileNames(
        {{VK_SHADER_STAGE_COMPUTE_BIT, "shader.comp.spv", ref_spirv.size() * sizeof(uint32_t), ref_spirv.data()}});

    const char* enabled_extensions[1] = {"VK_EXT_robustness2"};

    data.enabledExtensionCount = 1;
    data.ppEnabledExtensions = enabled_extensions;
    data.computePipelineState.pComputePipeline = &cp_ci;
    data.computePipelineState.pPipelineLayout = &pl_ci;
    data.computePipelineState.immutableSamplerCount = 2;
    data.computePipelineState.ppImmutableSamplerNames = sampler_names;
    data.computePipelineState.pImmutableSamplers = sampler_ci;
    data.computePipelineState.ycbcrSamplerCount = 1;
    data.computePipelineState.ppYcbcrSamplerNames = ycbcr_names;
    data.computePipelineState.pYcbcrSamplers = &ycbcr_ci;
    data.computePipelineState.descriptorSetLayoutCount = 1;
    data.computePipelineState.pDescriptorSetLayouts = &dsl_ci;
    data.computePipelineState.ppDescriptorSetLayoutNames = dsl_names;
    data.computePipelineState.pPhysicalDeviceFeatures = &pdf;
    data.computePipelineState.shaderFileNameCount = (uint32_t)shaderFileNames.size();
    data.computePipelineState.pShaderFileNames = shaderFileNames.data();

    const std::string ref_json = R"({
        "ComputePipelineState" :
        {
            "ComputePipeline" : )" +
                                 cp_json +
                                 R"(,
            "DescriptorSetLayouts" :
            [
                {
                    ")" + dsl_names[0] +
                                 R"(" : )" + dsl_json + R"(
                }
            ],
            "ImmutableSamplers" :
            [
                {
                    ")" + sampler_names[0] +
                                 R"(" : )" + immut_sampler_json + R"(
                },
                {
                    ")" + sampler_names[1] +
                                 R"(" : )" + ycbcr_sampler_json + R"(
                }
            ],
            "PhysicalDeviceFeatures" : )" +
                                 pdf_json + R"(,
            "PipelineLayout" : )" +
                                 pl_json +
                                 R"(,
            "ShaderFileNames" : )" +
                                 shaderFileNames_json +
                                 R"(,
            "YcbcrSamplers" :
            [
                {
                    ")" + ycbcr_names[0] +
                                 R"(" : )" + ycbcr_json + R"(
                }
            ]
        },
        "EnabledExtensions" :
        [
            "VK_EXT_robustness2"
        ],
        "PipelineUUID" : [192, 60, 140, 236, 89, 238, 218, 181, 232, 250, 156, 167, 106, 52, 14, 100]
    })";
    const char* result_json = nullptr;

    vpjSetMD5PipelineUUIDGeneration(generator_, true);
    EXPECT_TRUE(vpjGeneratePipelineJson(generator_, &data, &result_json, &msg_));
    CHECK_GEN();
    EXPECT_TRUE(ValidatePipelineJson(std::string(result_json)));
    EXPECT_TRUE(CompareJson(result_json, ref_json));
}

TEST_F(Gen, GraphicsPipelineJSONWithMD5) {
    TEST_DESCRIPTION("Tests generating pipeline JSON with MD5 UUID for a graphics pipeline");

    VpjData data{};

    const char* ycbcr_names[1] = {"YcbcrConversion1"};
    auto [ycbcr_ci, ycbcr_json] = getVkSamplerYcbcrConversionCreateInfo(1);

    const char* sampler_names[2] = {"ImmutableSampler1", "YcbcrSampler1"};
    auto [immut_sampler_ci, immut_sampler_json] = getVkSamplerCreateInfo(1);
    auto [ycbcr_sampler_ci, ycbcr_sampler_json] = getVkSamplerCreateInfo(1, {VkSamplerYcbcrConversion(0), ycbcr_names[0]});
    VkSamplerCreateInfo sampler_ci[2] = {*immut_sampler_ci.ptr(), *ycbcr_sampler_ci.ptr()};

    const char* dsl_names[1] = {"DescriptorSetLayout1"};
    auto [dsl_ci, dsl_json] =
        getVkDescriptorSetLayoutCreateInfo(1, {{VkSampler(0), sampler_names[0]}, {VkSampler(1), sampler_names[1]}});

    auto [pl_ci, pl_json] = getVkPipelineLayoutCreateInfo(1, {{VkDescriptorSetLayout(0), dsl_names[0]}});

    auto [renderPass, renderPass_json] = getVkRenderPassCreateInfo(1);

    auto [gp_ci, gp_json] = getVkGraphicsPipelineCreateInfo(1);

    auto [pdf, pdf_json] = getVkPhysicalDeviceFeatures2(1);

    std::vector<uint32_t> ref_spirv{1, 2, 3, 4};
    auto [shaderFileNames, shaderFileNames_json] = getShaderFileNames({
        {VK_SHADER_STAGE_VERTEX_BIT, "shader.vert.spv", ref_spirv.size() * sizeof(uint32_t), ref_spirv.data()},
        {VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT, "shader.tess_ctrl.spv", ref_spirv.size() * sizeof(uint32_t), ref_spirv.data()},
        {VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT, "shader.tess_eval.spv", ref_spirv.size() * sizeof(uint32_t),
         ref_spirv.data()},
        {VK_SHADER_STAGE_FRAGMENT_BIT, "shader.frag.spv", ref_spirv.size() * sizeof(uint32_t), ref_spirv.data()},
    });

    const char* enabled_extensions[1] = {"VK_EXT_robustness2"};

    data.enabledExtensionCount = 1;
    data.ppEnabledExtensions = enabled_extensions;
    data.graphicsPipelineState.pGraphicsPipeline = &gp_ci;
    data.graphicsPipelineState.pRenderPass = &renderPass;
    data.graphicsPipelineState.pPipelineLayout = &pl_ci;
    data.graphicsPipelineState.immutableSamplerCount = 2;
    data.graphicsPipelineState.ppImmutableSamplerNames = sampler_names;
    data.graphicsPipelineState.pImmutableSamplers = sampler_ci;
    data.graphicsPipelineState.ycbcrSamplerCount = 1;
    data.graphicsPipelineState.ppYcbcrSamplerNames = ycbcr_names;
    data.graphicsPipelineState.pYcbcrSamplers = &ycbcr_ci;
    data.graphicsPipelineState.descriptorSetLayoutCount = 1;
    data.graphicsPipelineState.pDescriptorSetLayouts = &dsl_ci;
    data.graphicsPipelineState.ppDescriptorSetLayoutNames = dsl_names;
    data.graphicsPipelineState.pPhysicalDeviceFeatures = &pdf;
    data.graphicsPipelineState.shaderFileNameCount = (uint32_t)shaderFileNames.size();
    data.graphicsPipelineState.pShaderFileNames = shaderFileNames.data();

    const std::string ref_json = R"({
        "GraphicsPipelineState" :
        {
            "Renderpass": )" + renderPass_json +
                                 R"(,
            "GraphicsPipeline" : )" +
                                 gp_json +
                                 R"(,
            "DescriptorSetLayouts" :
            [
                {
                    ")" + dsl_names[0] +
                                 R"(" : )" + dsl_json + R"(
                }
            ],
            "ImmutableSamplers" :
            [
                {
                    ")" + sampler_names[0] +
                                 R"(" : )" + immut_sampler_json + R"(
                },
                {
                    ")" + sampler_names[1] +
                                 R"(" : )" + ycbcr_sampler_json + R"(
                }
            ],
            "PhysicalDeviceFeatures" : )" +
                                 pdf_json + R"(,
            "PipelineLayout" : )" +
                                 pl_json +
                                 R"(,
            "ShaderFileNames" : )" +
                                 shaderFileNames_json +
                                 R"(,
            "YcbcrSamplers" :
            [
                {
                    ")" + ycbcr_names[0] +
                                 R"(" : )" + ycbcr_json + R"(
                }
            ]
        },
        "EnabledExtensions" :
        [
            "VK_EXT_robustness2"
        ],
        "PipelineUUID" : [198, 45, 78, 88, 61, 34, 44, 163, 15, 83, 194, 0, 94, 84, 30, 19]
    })";
    const char* result_json = nullptr;

    vpjSetMD5PipelineUUIDGeneration(generator_, true);
    EXPECT_TRUE(vpjGeneratePipelineJson(generator_, &data, &result_json, &msg_));
    CHECK_GEN();
    EXPECT_TRUE(ValidatePipelineJson(std::string(result_json)));
    EXPECT_TRUE(CompareJson(result_json, ref_json));
}

TEST_F(Gen, ObjectNameRemapping) {
    TEST_DESCRIPTION("Tests object name remapping in a compute pipeline JSON");

    const std::string ref_json{R"({
        "ComputePipelineState" : 
        {
            "ComputePipeline" : 
            {
                "basePipelineHandle" : "",
                "basePipelineIndex" : 0,
                "flags" : 0,
                "layout" : "",
                "pNext" : "NULL",
                "sType" : "VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO",
                "stage" : 
                {
                    "flags" : 0,
                    "module" : "",
                    "pName" : "main",
                    "pNext" : "NULL",
                    "pSpecializationInfo" : "NULL",
                    "sType" : "VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO",
                    "stage" : "VK_SHADER_STAGE_COMPUTE_BIT"
                }
            },
            "ShaderFileNames" :
            [
                {
                    "stage" : "VK_SHADER_STAGE_COMPUTE_BIT",
                    "filename" : "saxpy.comp.spv"
                }
            ],
)"
                               R"(
            "DescriptorSetLayouts" : 
            [
                {
                    "descriptor_layout_1" : 
                    {
                        "bindingCount" : 1,
                        "flags" : 0,
                        "pBindings" : 
                        [
                            {
                                "binding" : 0,
                                "descriptorCount" : 2,
                                "descriptorType" : "VK_DESCRIPTOR_TYPE_SAMPLER",
                                "pImmutableSamplers" : 
                                [
                                    "immutable_sampler_3",
                                    "immutable_sampler_1"
                                ],
                                "stageFlags" : 0
                            }
                        ],
                        "pNext" : "NULL",
                        "sType" : "VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO"
                    }
                },
                {
                    "descriptor_layout_2" : 
                    {
                        "bindingCount" : 0,
                        "flags" : 0,
                        "pBindings" : "NULL",
                        "pNext" : "NULL",
                        "sType" : "VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO"
                    }
                },
                {
                    "descriptor_layout_3" : 
                    {
                        "bindingCount" : 1,
                        "flags" : 0,
                        "pBindings" : 
                        [
                            {
                                "binding" : 0,
                                "descriptorCount" : 2,
                                "descriptorType" : "VK_DESCRIPTOR_TYPE_SAMPLER",
                                "pImmutableSamplers" : 
                                [
                                    "immutable_sampler_1",
                                    "immutable_sampler_2"
                                ],
                                "stageFlags" : 0
                            }
                        ],
                        "pNext" : "NULL",
                        "sType" : "VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO"
                    }
                }
            ],
)"
                               R"(
            "ImmutableSamplers" : 
            [
                {
                    "immutable_sampler_1" : 
                    {
                        "addressModeU" : "VK_SAMPLER_ADDRESS_MODE_REPEAT",
                        "addressModeV" : "VK_SAMPLER_ADDRESS_MODE_REPEAT",
                        "addressModeW" : "VK_SAMPLER_ADDRESS_MODE_REPEAT",
                        "anisotropyEnable" : "VK_FALSE",
                        "borderColor" : "VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK",
                        "compareEnable" : "VK_FALSE",
                        "compareOp" : "VK_COMPARE_OP_NEVER",
                        "flags" : 0,
                        "magFilter" : "VK_FILTER_NEAREST",
                        "maxAnisotropy" : 0.0,
                        "maxLod" : 0.0,
                        "minFilter" : "VK_FILTER_NEAREST",
                        "minLod" : 0.0,
                        "mipLodBias" : 0.0,
                        "mipmapMode" : "VK_SAMPLER_MIPMAP_MODE_NEAREST",
                        "pNext" : 
                        {
                            "conversion" : "ycbcr_conversion_3",
                            "pNext" : "NULL",
                            "sType" : "VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_INFO"
                        },
                        "sType" : "VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO",
                        "unnormalizedCoordinates" : "VK_FALSE"
                    }
                },
                {
                    "immutable_sampler_2" : 
                    {
                        "addressModeU" : "VK_SAMPLER_ADDRESS_MODE_REPEAT",
                        "addressModeV" : "VK_SAMPLER_ADDRESS_MODE_REPEAT",
                        "addressModeW" : "VK_SAMPLER_ADDRESS_MODE_REPEAT",
                        "anisotropyEnable" : "VK_FALSE",
                        "borderColor" : "VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK",
                        "compareEnable" : "VK_FALSE",
                        "compareOp" : "VK_COMPARE_OP_NEVER",
                        "flags" : 0,
                        "magFilter" : "VK_FILTER_NEAREST",
                        "maxAnisotropy" : 0.0,
                        "maxLod" : 0.0,
                        "minFilter" : "VK_FILTER_NEAREST",
                        "minLod" : 0.0,
                        "mipLodBias" : 0.0,
                        "mipmapMode" : "VK_SAMPLER_MIPMAP_MODE_NEAREST",
                        "pNext" : "NULL",
                        "sType" : "VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO",
                        "unnormalizedCoordinates" : "VK_FALSE"
                    }
                },
                {
                    "immutable_sampler_3" : 
                    {
                        "addressModeU" : "VK_SAMPLER_ADDRESS_MODE_REPEAT",
                        "addressModeV" : "VK_SAMPLER_ADDRESS_MODE_REPEAT",
                        "addressModeW" : "VK_SAMPLER_ADDRESS_MODE_REPEAT",
                        "anisotropyEnable" : "VK_FALSE",
                        "borderColor" : "VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK",
                        "compareEnable" : "VK_FALSE",
                        "compareOp" : "VK_COMPARE_OP_NEVER",
                        "flags" : 0,
                        "magFilter" : "VK_FILTER_NEAREST",
                        "maxAnisotropy" : 0.0,
                        "maxLod" : 0.0,
                        "minFilter" : "VK_FILTER_NEAREST",
                        "minLod" : 0.0,
                        "mipLodBias" : 0.0,
                        "mipmapMode" : "VK_SAMPLER_MIPMAP_MODE_NEAREST",
                        "pNext" : 
                        {
                            "conversion" : "ycbcr_conversion_1",
                            "pNext" : "NULL",
                            "sType" : "VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_INFO"
                        },
                        "sType" : "VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO",
                        "unnormalizedCoordinates" : "VK_FALSE"
                    }
                }
            ],
)"
                               R"(
            "PipelineLayout" : 
            {
                "flags" : 0,
                "pNext" : "NULL",
                "pPushConstantRanges" : "NULL",
                "pSetLayouts" : 
                [
                    "descriptor_layout_3",
                    "descriptor_layout_1",
                    "descriptor_layout_2"
                ],
                "pushConstantRangeCount" : 0,
                "sType" : "VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO",
                "setLayoutCount" : 3
            },
            "YcbcrSamplers" : 
            [
                {
                    "ycbcr_conversion_1" : 
                    {
                        "chromaFilter" : "VK_FILTER_NEAREST",
                        "components" : 
                        {
                            "a" : "VK_COMPONENT_SWIZZLE_IDENTITY",
                            "b" : "VK_COMPONENT_SWIZZLE_IDENTITY",
                            "g" : "VK_COMPONENT_SWIZZLE_IDENTITY",
                            "r" : "VK_COMPONENT_SWIZZLE_IDENTITY"
                        },
                        "forceExplicitReconstruction" : "VK_FALSE",
                        "format" : "VK_FORMAT_UNDEFINED",
                        "pNext" : "NULL",
                        "sType" : "VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_CREATE_INFO",
                        "xChromaOffset" : "VK_CHROMA_LOCATION_COSITED_EVEN",
                        "yChromaOffset" : "VK_CHROMA_LOCATION_COSITED_EVEN",
                        "ycbcrModel" : "VK_SAMPLER_YCBCR_MODEL_CONVERSION_RGB_IDENTITY",
                        "ycbcrRange" : "VK_SAMPLER_YCBCR_RANGE_ITU_FULL"
                    }
                },
                {
                    "ycbcr_conversion_2" : 
                    {
                        "chromaFilter" : "VK_FILTER_NEAREST",
                        "components" : 
                        {
                            "a" : "VK_COMPONENT_SWIZZLE_IDENTITY",
                            "b" : "VK_COMPONENT_SWIZZLE_IDENTITY",
                            "g" : "VK_COMPONENT_SWIZZLE_IDENTITY",
                            "r" : "VK_COMPONENT_SWIZZLE_IDENTITY"
                        },
                        "forceExplicitReconstruction" : "VK_FALSE",
                        "format" : "VK_FORMAT_UNDEFINED",
                        "pNext" : "NULL",
                        "sType" : "VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_CREATE_INFO",
                        "xChromaOffset" : "VK_CHROMA_LOCATION_COSITED_EVEN",
                        "yChromaOffset" : "VK_CHROMA_LOCATION_COSITED_EVEN",
                        "ycbcrModel" : "VK_SAMPLER_YCBCR_MODEL_CONVERSION_RGB_IDENTITY",
                        "ycbcrRange" : "VK_SAMPLER_YCBCR_RANGE_ITU_FULL"
                    }
                },
                {
                    "ycbcr_conversion_3" : 
                    {
                        "chromaFilter" : "VK_FILTER_NEAREST",
                        "components" : 
                        {
                            "a" : "VK_COMPONENT_SWIZZLE_IDENTITY",
                            "b" : "VK_COMPONENT_SWIZZLE_IDENTITY",
                            "g" : "VK_COMPONENT_SWIZZLE_IDENTITY",
                            "r" : "VK_COMPONENT_SWIZZLE_IDENTITY"
                        },
                        "forceExplicitReconstruction" : "VK_FALSE",
                        "format" : "VK_FORMAT_UNDEFINED",
                        "pNext" : "NULL",
                        "sType" : "VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_CREATE_INFO",
                        "xChromaOffset" : "VK_CHROMA_LOCATION_COSITED_EVEN",
                        "yChromaOffset" : "VK_CHROMA_LOCATION_COSITED_EVEN",
                        "ycbcrModel" : "VK_SAMPLER_YCBCR_MODEL_CONVERSION_RGB_IDENTITY",
                        "ycbcrRange" : "VK_SAMPLER_YCBCR_RANGE_ITU_FULL"
                    }
                }
            ]
        },
        "EnabledExtensions" : [],
        "PipelineUUID" : 
        [
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0
        ]
    })"};

    VpjData data{};

    VkComputePipelineCreateInfo computePipeline_ci{VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO};
    computePipeline_ci.stage.pName = "main";
    computePipeline_ci.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;

    data.computePipelineState.pComputePipeline = &computePipeline_ci;

    VkPipelineLayoutCreateInfo pipelineLayout_ci{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};

    VkDescriptorSetLayout setLayouts[3] = {VkDescriptorSetLayout(2), VkDescriptorSetLayout(0), VkDescriptorSetLayout(1)};
    pipelineLayout_ci.setLayoutCount = 3;
    pipelineLayout_ci.pSetLayouts = setLayouts;

    data.computePipelineState.pPipelineLayout = &pipelineLayout_ci;

    VkSamplerYcbcrConversionCreateInfo ycbcrSamplers[3] = {};
    const char* ycbcrSamplerNames[3] = {"ycbcr_conversion_1", "ycbcr_conversion_2", "ycbcr_conversion_3"};

    ycbcrSamplers[0].sType = VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_CREATE_INFO;
    ycbcrSamplers[1].sType = VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_CREATE_INFO;
    ycbcrSamplers[2].sType = VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_CREATE_INFO;

    data.computePipelineState.ycbcrSamplerCount = 3;
    data.computePipelineState.pYcbcrSamplers = ycbcrSamplers;
    data.computePipelineState.ppYcbcrSamplerNames = ycbcrSamplerNames;

    VkSamplerYcbcrConversionInfo ycbcrConversionInfo[2] = {};

    ycbcrConversionInfo[0].sType = VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_INFO;
    ycbcrConversionInfo[0].conversion = VkSamplerYcbcrConversion(0);
    ycbcrConversionInfo[1].sType = VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_INFO;
    ycbcrConversionInfo[1].conversion = VkSamplerYcbcrConversion(2);

    VkSamplerCreateInfo immutableSampler_cis[3] = {};
    const char* immutableSamplerNames[3] = {"immutable_sampler_1", "immutable_sampler_2", "immutable_sampler_3"};

    immutableSampler_cis[0].sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    immutableSampler_cis[0].pNext = &ycbcrConversionInfo[1];

    immutableSampler_cis[1].sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

    immutableSampler_cis[2].sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    immutableSampler_cis[2].pNext = &ycbcrConversionInfo[0];

    data.computePipelineState.immutableSamplerCount = 3;
    data.computePipelineState.pImmutableSamplers = immutableSampler_cis;
    data.computePipelineState.ppImmutableSamplerNames = immutableSamplerNames;

    VkSampler immutableSamplers[3] = {VkSampler(2), VkSampler(0), VkSampler(1)};
    VkDescriptorSetLayoutBinding descriptorSetLayoutBindings[2] = {};

    descriptorSetLayoutBindings[0].descriptorCount = 2;
    descriptorSetLayoutBindings[0].pImmutableSamplers = &immutableSamplers[0];

    descriptorSetLayoutBindings[1].descriptorCount = 2;
    descriptorSetLayoutBindings[1].pImmutableSamplers = &immutableSamplers[1];

    const char* descriptorLayoutNames[3] = {"descriptor_layout_1", "descriptor_layout_2", "descriptor_layout_3"};
    VkDescriptorSetLayoutCreateInfo descriptorSetLayouts[3] = {};

    descriptorSetLayouts[0].sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayouts[0].bindingCount = 1;
    descriptorSetLayouts[0].pBindings = &descriptorSetLayoutBindings[0];

    descriptorSetLayouts[1].sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;

    descriptorSetLayouts[2].sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayouts[2].bindingCount = 1;
    descriptorSetLayouts[2].pBindings = &descriptorSetLayoutBindings[1];

    data.computePipelineState.descriptorSetLayoutCount = 3;
    data.computePipelineState.pDescriptorSetLayouts = descriptorSetLayouts;
    data.computePipelineState.ppDescriptorSetLayoutNames = descriptorLayoutNames;

    VpjShaderFileName shaderFileNames[1] = {};
    shaderFileNames[0].pFilename = "saxpy.comp.spv";
    shaderFileNames[0].stage = VK_SHADER_STAGE_COMPUTE_BIT;

    data.computePipelineState.shaderFileNameCount = 1;
    data.computePipelineState.pShaderFileNames = shaderFileNames;

    const char* result_json = nullptr;

    EXPECT_TRUE(vpjGeneratePipelineJson(generator_, &data, &result_json, &msg_));
    CHECK_GEN();
    EXPECT_TRUE(ValidatePipelineJson(std::string(result_json)));
    EXPECT_TRUE(CompareJson(result_json, ref_json));
}

TEST_F(Gen, ZeroShaderFilenamesCompute) {
    TEST_DESCRIPTION("Tests whether generation fails for compute pipelines when no shader filenames are provided");

    VpjData data{};

    const char* ycbcr_names[1] = {"YcbcrConversion1"};
    auto [ycbcr_ci, ycbcr_json] = getVkSamplerYcbcrConversionCreateInfo(0);

    const char* sampler_names[2] = {"ImmutableSampler1", "YcbcrSampler1"};
    auto [immut_sampler_ci, immut_sampler_json] = getVkSamplerCreateInfo(0);
    auto [ycbcr_sampler_ci, ycbcr_sampler_json] = getVkSamplerCreateInfo(0, {VkSamplerYcbcrConversion(0), ycbcr_names[0]});
    VkSamplerCreateInfo sampler_ci[2] = {*immut_sampler_ci.ptr(), *ycbcr_sampler_ci.ptr()};

    const char* dsl_names[1] = {"DescriptorSetLayout1"};
    auto [dsl_ci, dsl_json] =
        getVkDescriptorSetLayoutCreateInfo(0, {{VkSampler(0), sampler_names[0]}, {VkSampler(1), sampler_names[1]}});

    auto [pl_ci, pl_json] = getVkPipelineLayoutCreateInfo(0, {{VkDescriptorSetLayout(0), dsl_names[0]}});

    auto [cp_ci, cp_json] = getVkComputePipelineCreateInfo(0);

    auto [pdf, pdf_json] = getVkPhysicalDeviceFeatures2(0);

    const char* enabled_extensions[1] = {"VK_EXT_robustness2"};

    data.enabledExtensionCount = 1;
    data.ppEnabledExtensions = enabled_extensions;
    data.computePipelineState.pComputePipeline = &cp_ci;
    data.computePipelineState.pPipelineLayout = &pl_ci;
    data.computePipelineState.immutableSamplerCount = 2;
    data.computePipelineState.ppImmutableSamplerNames = sampler_names;
    data.computePipelineState.pImmutableSamplers = sampler_ci;
    data.computePipelineState.ycbcrSamplerCount = 1;
    data.computePipelineState.ppYcbcrSamplerNames = ycbcr_names;
    data.computePipelineState.pYcbcrSamplers = &ycbcr_ci;
    data.computePipelineState.descriptorSetLayoutCount = 1;
    data.computePipelineState.pDescriptorSetLayouts = &dsl_ci;
    data.computePipelineState.ppDescriptorSetLayoutNames = dsl_names;
    data.computePipelineState.pPhysicalDeviceFeatures = &pdf;
    data.computePipelineState.shaderFileNameCount = 0;
    data.computePipelineState.pShaderFileNames = nullptr;

    const char* result_json = nullptr;

    EXPECT_FALSE(vpjGeneratePipelineJson(generator_, &data, &result_json, &msg_));
}

TEST_F(Gen, ZeroShaderFilenamesGraphics) {
    TEST_DESCRIPTION("Tests whether generation fails for graphics pipelines when no shader filenames are provided");

    VpjData data{};

    const char* ycbcr_names[1] = {"YcbcrConversion1"};
    auto [ycbcr_ci, ycbcr_json] = getVkSamplerYcbcrConversionCreateInfo(1);

    const char* sampler_names[2] = {"ImmutableSampler1", "YcbcrSampler1"};
    auto [immut_sampler_ci, immut_sampler_json] = getVkSamplerCreateInfo(1);
    auto [ycbcr_sampler_ci, ycbcr_sampler_json] = getVkSamplerCreateInfo(1, {VkSamplerYcbcrConversion(0), ycbcr_names[0]});
    VkSamplerCreateInfo sampler_ci[2] = {*immut_sampler_ci.ptr(), *ycbcr_sampler_ci.ptr()};

    const char* dsl_names[1] = {"DescriptorSetLayout1"};
    auto [dsl_ci, dsl_json] =
        getVkDescriptorSetLayoutCreateInfo(1, {{VkSampler(0), sampler_names[0]}, {VkSampler(1), sampler_names[1]}});

    auto [pl_ci, pl_json] = getVkPipelineLayoutCreateInfo(1, {{VkDescriptorSetLayout(0), dsl_names[0]}});

    auto [renderPass, renderPass_json] = getVkRenderPassCreateInfo(1);

    auto [gp_ci, gp_json] = getVkGraphicsPipelineCreateInfo(1);

    auto [pdf, pdf_json] = getVkPhysicalDeviceFeatures2(1);

    const char* enabled_extensions[1] = {"VK_EXT_robustness2"};

    data.enabledExtensionCount = 1;
    data.ppEnabledExtensions = enabled_extensions;
    data.graphicsPipelineState.pGraphicsPipeline = &gp_ci;
    data.graphicsPipelineState.pRenderPass = &renderPass;
    data.graphicsPipelineState.pPipelineLayout = &pl_ci;
    data.graphicsPipelineState.immutableSamplerCount = 2;
    data.graphicsPipelineState.ppImmutableSamplerNames = sampler_names;
    data.graphicsPipelineState.pImmutableSamplers = sampler_ci;
    data.graphicsPipelineState.ycbcrSamplerCount = 1;
    data.graphicsPipelineState.ppYcbcrSamplerNames = ycbcr_names;
    data.graphicsPipelineState.pYcbcrSamplers = &ycbcr_ci;
    data.graphicsPipelineState.descriptorSetLayoutCount = 1;
    data.graphicsPipelineState.pDescriptorSetLayouts = &dsl_ci;
    data.graphicsPipelineState.ppDescriptorSetLayoutNames = dsl_names;
    data.graphicsPipelineState.pPhysicalDeviceFeatures = &pdf;
    data.graphicsPipelineState.shaderFileNameCount = 0;
    data.graphicsPipelineState.pShaderFileNames = nullptr;

    const char* result_json = nullptr;

    EXPECT_FALSE(vpjGeneratePipelineJson(generator_, &data, &result_json, &msg_));
}
