/*
 * Copyright (c) 2025 The Khronos Group Inc.
 * Copyright (c) 2025 RasterGrid Kft.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <vulkan/pcjson/vksc_pipeline_json.h>
#include <vulkan/vulkan_sc.h>

#include <gtest/gtest.h>

#include <assert.h>
#include <stddef.h>
#include <iostream>
#include <string.h>
#include <vector>
#include <string>
#include <math.h>

#include "json_validator.h"
#include "json_struct_helpers.h"

class Parse : public testing::Test {
  public:
    Parse() : parser_{vpjCreateParser()}, msg_{nullptr} {}
    Parse(const Parse&) = delete;
    Parse(Parse&&) = delete;
    ~Parse() { vpjDestroyParser(parser_); }

    void TEST_DESCRIPTION(const char* desc) { RecordProperty("description", desc); }
    void CHECK_PARSE(bool success) {
        EXPECT_TRUE(success);
        if (msg_ && strlen(msg_)) {
            FAIL() << msg_;
        }
    }

  protected:
    VpjParser parser_;
    const char* msg_;
};

TEST_F(Parse, BasicTypesVkBool32) {
    TEST_DESCRIPTION("Tests value equivalence of written and parsed VkBool32 values");

    VkPhysicalDeviceFeatures2 data;
    const char* msg = nullptr;
    // Generates JSON representing a VkPhysicalDeviceFeatures2 struct with the desired
    // VkBool32 value (JSON fragment) written into the robustBufferAccess field.
    auto get_json = [](const char* val) {
        std::string result = {R"({
            "sType" : "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2",
                "pNext": {
                    "sType": "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES",
                    "pNext": {
                        "sType": "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SCALAR_BLOCK_LAYOUT_FEATURES",
                        "pNext": "NULL",
                        "scalarBlockLayout": "VK_TRUE"
                    },
                    "storageBuffer16BitAccess": "VK_FALSE",
                    "uniformAndStorageBuffer16BitAccess": "VK_FALSE",
                    "storagePushConstant16": "VK_FALSE",
                    "storageInputOutput16": "VK_FALSE",
                    "multiview": "VK_FALSE",
                    "multiviewGeometryShader": "VK_FALSE",
                    "multiviewTessellationShader": "VK_FALSE",
                    "variablePointersStorageBuffer": "VK_FALSE",
                    "variablePointers": "VK_FALSE",
                    "protectedMemory": "VK_FALSE",
                    "samplerYcbcrConversion": "VK_TRUE",
                    "shaderDrawParameters": "VK_FALSE"
                },
            "features": 
            {
                "robustBufferAccess" : )"};
        result += val;
        result += R"(,
                "fullDrawIndexUint32" : "VK_FALSE",
                "imageCubeArray" : "VK_FALSE",
                "independentBlend" : "VK_FALSE",
                "geometryShader" : "VK_FALSE",
                "tessellationShader" : "VK_FALSE",
                "sampleRateShading" : "VK_FALSE",
                "dualSrcBlend" : "VK_FALSE",
                "logicOp" : "VK_FALSE",
                "multiDrawIndirect" : "VK_FALSE",
                "drawIndirectFirstInstance" : "VK_FALSE",
                "depthClamp" : "VK_FALSE",
                "depthBiasClamp" : "VK_FALSE",
                "fillModeNonSolid" : "VK_FALSE",
                "depthBounds" : "VK_FALSE",
                "wideLines" : "VK_FALSE",
                "largePoints" : "VK_FALSE",
                "alphaToOne" : "VK_FALSE",
                "multiViewport" : "VK_FALSE",
                "samplerAnisotropy" : "VK_FALSE",
                "textureCompressionETC2" : "VK_FALSE",
                "textureCompressionASTC_LDR" : "VK_FALSE",
                "textureCompressionBC" : "VK_FALSE",
                "occlusionQueryPrecise" : "VK_FALSE",
                "pipelineStatisticsQuery" : "VK_FALSE",
                "vertexPipelineStoresAndAtomics" : "VK_FALSE",
                "fragmentStoresAndAtomics" : "VK_FALSE",
                "shaderTessellationAndGeometryPointSize" : "VK_FALSE",
                "shaderImageGatherExtended" : "VK_FALSE",
                "shaderStorageImageExtendedFormats" : "VK_FALSE",
                "shaderStorageImageMultisample" : "VK_FALSE",
                "shaderStorageImageReadWithoutFormat" : "VK_FALSE",
                "shaderStorageImageWriteWithoutFormat" : "VK_FALSE",
                "shaderUniformBufferArrayDynamicIndexing" : "VK_FALSE",
                "shaderSampledImageArrayDynamicIndexing" : "VK_FALSE",
                "shaderStorageBufferArrayDynamicIndexing" : "VK_FALSE",
                "shaderStorageImageArrayDynamicIndexing" : "VK_FALSE",
                "shaderClipDistance" : "VK_FALSE",
                "shaderCullDistance" : "VK_FALSE",
                "shaderFloat64" : "VK_FALSE",
                "shaderInt64" : "VK_FALSE",
                "shaderInt16" : "VK_FALSE",
                "shaderResourceResidency" : "VK_FALSE",
                "shaderResourceMinLod" : "VK_FALSE",
                "sparseBinding" : "VK_FALSE",
                "sparseResidencyBuffer" : "VK_FALSE",
                "sparseResidencyImage2D" : "VK_FALSE",
                "sparseResidencyImage3D" : "VK_FALSE",
                "sparseResidency2Samples" : "VK_FALSE",
                "sparseResidency4Samples" : "VK_FALSE",
                "sparseResidency8Samples" : "VK_FALSE",
                "sparseResidency16Samples" : "VK_FALSE",
                "sparseResidencyAliased" : "VK_FALSE",
                "variableMultisampleRate" : "VK_FALSE",
                "inheritedQueries" : "VK_FALSE"
            }
        })";
        return result;
    };

    auto test_eq = [&](auto frag, auto expect) {
        std::string json = get_json(frag);
        bool success = vpjParseSingleStructJson(this->parser_, json.c_str(), &data, &msg);
        EXPECT_TRUE(success);
        if (msg && strlen(msg)) {
            std::cerr << msg << std::endl;
        }
        EXPECT_EQ(data.features.robustBufferAccess, expect);

        EXPECT_EQ(data.features.sparseResidencyImage2D, VK_FALSE);

        const auto v11f = reinterpret_cast<const VkPhysicalDeviceVulkan11Features*>(data.pNext);
        EXPECT_EQ(v11f->sType, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES);
        EXPECT_EQ(v11f->samplerYcbcrConversion, VK_TRUE);
        EXPECT_EQ(v11f->shaderDrawParameters, VK_FALSE);

        const auto sblf = reinterpret_cast<const VkPhysicalDeviceScalarBlockLayoutFeatures*>(v11f->pNext);
        EXPECT_EQ(sblf->sType, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SCALAR_BLOCK_LAYOUT_FEATURES);
        EXPECT_EQ(sblf->scalarBlockLayout, VK_TRUE);
    };

    test_eq(R"("VK_TRUE")", VK_TRUE);
    test_eq(R"(0)", 0);
    test_eq(R"("VK_FALSE")", VK_FALSE);
    test_eq(R"(4294967295)", 4294967295);
}

TEST_F(Parse, VkPhysicalDeviceFeatures2) {
    TEST_DESCRIPTION("Tests parsing of a reasonably complex physical device features 2 JSON");

    for (auto seed : {0, 1, 2}) {
        auto [ref_pdf, json_in] = getVkPhysicalDeviceFeatures2(seed);

        VkPhysicalDeviceFeatures2 res_pdf{};
        CHECK_PARSE(vpjParseSingleStructJson(this->parser_, json_in.c_str(), &res_pdf, &msg_));
        CompareStruct(ref_pdf, res_pdf);
    }
}

TEST_F(Parse, VkGraphicsPipelineCreateInfo) {
    TEST_DESCRIPTION("Tests parsing of a reasonably complex compute pipeline create info JSON");

    for (auto seed : {0}) {
        auto [ref_ci, json_in] = getVkGraphicsPipelineCreateInfo(seed);

        VkGraphicsPipelineCreateInfo res_ci{};
        CHECK_PARSE(vpjParseSingleStructJson(this->parser_, json_in.c_str(), &res_ci, &msg_));
        CompareStruct(ref_ci, res_ci);
    }
}

TEST_F(Parse, VkComputePipelineCreateInfo) {
    TEST_DESCRIPTION("Tests parsing of a reasonably complex compute pipeline create info JSON");

    for (auto seed : {0, 1}) {
        auto [ref_ci, json_in] = getVkComputePipelineCreateInfo(seed);

        VkComputePipelineCreateInfo res_ci{};
        CHECK_PARSE(vpjParseSingleStructJson(this->parser_, json_in.c_str(), &res_ci, &msg_));
        CompareStruct(ref_ci, res_ci);
    }
}

TEST_F(Parse, VkSamplerYcbcrConversionCreateInfo) {
    TEST_DESCRIPTION("Tests parsing of a reasonably complex ycbcr conversion create info JSON");

    for (auto seed : {0, 1, 2}) {
        auto [ref_ci, json_in] = getVkSamplerYcbcrConversionCreateInfo(seed);

        VkSamplerYcbcrConversionCreateInfo res_ci{};
        CHECK_PARSE(vpjParseSingleStructJson(this->parser_, json_in.c_str(), &res_ci, &msg_));
        CompareStruct(ref_ci, res_ci);
    }
}

TEST_F(Parse, VkSamplerCreateInfo) {
    TEST_DESCRIPTION("Tests parsing of a reasonably complex sampler create info JSON");

    for (auto seed : {0, 1, 2}) {
        for (auto param : std::vector<SamplerParams>{{}, {VkSamplerYcbcrConversion(12345)}}) {
            auto [ref_ci, json_in] = getVkSamplerCreateInfo(seed, param);

            VkSamplerCreateInfo res_ci{};
            CHECK_PARSE(vpjParseSingleStructJson(this->parser_, json_in.c_str(), &res_ci, &msg_));
            CompareStruct(ref_ci, res_ci);
        }
    }
}

TEST_F(Parse, VkDescriptorSetLayoutCreateInfo) {
    TEST_DESCRIPTION("Tests parsing of a reasonably complex descriptor set layout create info JSON");

    for (auto seed : {0, 1, 2, 3}) {
        auto [ref_ci, json_in] = getVkDescriptorSetLayoutCreateInfo(seed);

        VkDescriptorSetLayoutCreateInfo res_ci{};
        CHECK_PARSE(vpjParseSingleStructJson(this->parser_, json_in.c_str(), &res_ci, &msg_));
        CompareStruct(ref_ci, res_ci);
    }
}

TEST_F(Parse, VkPipelineLayoutCreateInfo) {
    TEST_DESCRIPTION("Tests parsing of a reasonably complex pipeline layout create info JSON");

    for (auto seed : {0, 1, 2, 3}) {
        auto [ref_ci, json_in] = getVkPipelineLayoutCreateInfo(seed);

        VkPipelineLayoutCreateInfo res_ci{};
        CHECK_PARSE(vpjParseSingleStructJson(this->parser_, json_in.c_str(), &res_ci, &msg_));
        CompareStruct(ref_ci, res_ci);
    }
}

TEST_F(Parse, VkRenderPassCreateInfo) {
    TEST_DESCRIPTION("Tests parsing of a reasonably complex render pass create info JSON");

    for (auto seed : {0, 1, 2, 3}) {
        auto [ref_ci, json_in] = getVkRenderPassCreateInfo(seed);

        VkRenderPassCreateInfo res_ci{};
        CHECK_PARSE(vpjParseSingleStructJson(this->parser_, json_in.c_str(), &res_ci, &msg_));
        CompareStruct(ref_ci, res_ci);
    }
}

TEST_F(Parse, VkRenderPassCreateInfo2) {
    TEST_DESCRIPTION("Tests parsing of a reasonably complex render pass 2 layout create info JSON");

    for (auto seed : {0, 1}) {
        auto [ref_ci, json_in] = getVkRenderPassCreateInfo2(seed);

        VkRenderPassCreateInfo2 res_ci{};
        CHECK_PARSE(vpjParseSingleStructJson(this->parser_, json_in.c_str(), &res_ci, &msg_));
        CompareStruct(ref_ci, res_ci);
    }
}

TEST_F(Parse, VkShaderModuleCreateInfo) {
    TEST_DESCRIPTION("Tests parsing of a reasonably complex shader module create info JSON");

    for (auto seed : {0, 1}) {
        auto [ref_ci, json_in] = getVkShaderModuleCreateInfo(seed);

        VkShaderModuleCreateInfo res_ci{};
        CHECK_PARSE(vpjParseSingleStructJson(this->parser_, json_in.c_str(), &res_ci, &msg_));
        CompareStruct(ref_ci, res_ci);
    }
}

TEST_F(Parse, VkDeviceObjectReservationCreateInfo) {
    TEST_DESCRIPTION("Tests parsing of a reasonably complex object reservation create info JSON");

    for (auto seed : {0}) {
        auto [ref_ci, json_in] = getVkDeviceObjectReservationCreateInfo(seed);

        VkDeviceObjectReservationCreateInfo res_ci{};
        CHECK_PARSE(vpjParseSingleStructJson(this->parser_, json_in.c_str(), &res_ci, &msg_));
        CompareStruct(ref_ci, res_ci);
    }
}

TEST_F(Parse, VkPipelineOfflineCreateInfo) {
    TEST_DESCRIPTION("Tests parsing of a reasonably complex pipeline offline create info JSON");

    for (auto seed : {0}) {
        auto [ref_ci, json_in] = getVkPipelineOfflineCreateInfo(seed);

        VkPipelineOfflineCreateInfo res_ci{};
        CHECK_PARSE(vpjParseSingleStructJson(this->parser_, json_in.c_str(), &res_ci, &msg_));
        CompareStruct(ref_ci, res_ci);
    }
}

TEST_F(Parse, ComputePipelineJSON) {
    TEST_DESCRIPTION("Tests parsing of a reasonably simple compute pipeline JSON");

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

    const std::string json = R"({
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

    VpjData ref_data{};
    ref_data.enabledExtensionCount = 1;
    ref_data.ppEnabledExtensions = enabled_extensions;
    ref_data.computePipelineState.pComputePipeline = &cp_ci;
    ref_data.computePipelineState.pPipelineLayout = &pl_ci;
    ref_data.computePipelineState.immutableSamplerCount = 2;
    ref_data.computePipelineState.ppImmutableSamplerNames = sampler_names;
    ref_data.computePipelineState.pImmutableSamplers = sampler_ci;
    ref_data.computePipelineState.ycbcrSamplerCount = 1;
    ref_data.computePipelineState.ppYcbcrSamplerNames = ycbcr_names;
    ref_data.computePipelineState.pYcbcrSamplers = &ycbcr_ci;
    ref_data.computePipelineState.descriptorSetLayoutCount = 1;
    ref_data.computePipelineState.pDescriptorSetLayouts = &dsl_ci;
    ref_data.computePipelineState.ppDescriptorSetLayoutNames = dsl_names;
    ref_data.computePipelineState.pPhysicalDeviceFeatures = &pdf;
    ref_data.computePipelineState.shaderFileNameCount = (uint32_t)shaderFileNames.size();
    ref_data.computePipelineState.pShaderFileNames = shaderFileNames.data();

    ref_data.pipelineUUID[0] = 85;
    ref_data.pipelineUUID[1] = 43;
    ref_data.pipelineUUID[2] = 255;
    ref_data.pipelineUUID[3] = 24;
    ref_data.pipelineUUID[4] = 155;
    ref_data.pipelineUUID[5] = 64;
    ref_data.pipelineUUID[6] = 62;
    ref_data.pipelineUUID[7] = 24;

    EXPECT_TRUE(vpjParsePipelineJson(this->parser_, json.c_str(), &data, &msg_));
    CHECK_PARSE(true);

    CompareData<VpjComputePipelineState>(ref_data, data);
}

TEST_F(Parse, GraphicsPipelineJSON) {
    TEST_DESCRIPTION("Tests parsing of a reasonably simple graphics pipeline JSON");

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

    const std::string json = R"({
        "GraphicsPipelineState" :
        {
            "Renderpass": )" +
                             renderPass_json +
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

    VpjData ref_data{};
    ref_data.enabledExtensionCount = 1;
    ref_data.ppEnabledExtensions = enabled_extensions;
    ref_data.graphicsPipelineState.pGraphicsPipeline = &gp_ci;
    ref_data.graphicsPipelineState.pRenderPass = &renderPass;
    ref_data.graphicsPipelineState.pPipelineLayout = &pl_ci;
    ref_data.graphicsPipelineState.immutableSamplerCount = 2;
    ref_data.graphicsPipelineState.ppImmutableSamplerNames = sampler_names;
    ref_data.graphicsPipelineState.pImmutableSamplers = sampler_ci;
    ref_data.graphicsPipelineState.ycbcrSamplerCount = 1;
    ref_data.graphicsPipelineState.ppYcbcrSamplerNames = ycbcr_names;
    ref_data.graphicsPipelineState.pYcbcrSamplers = &ycbcr_ci;
    ref_data.graphicsPipelineState.descriptorSetLayoutCount = 1;
    ref_data.graphicsPipelineState.pDescriptorSetLayouts = &dsl_ci;
    ref_data.graphicsPipelineState.ppDescriptorSetLayoutNames = dsl_names;
    ref_data.graphicsPipelineState.pPhysicalDeviceFeatures = &pdf;
    ref_data.graphicsPipelineState.shaderFileNameCount = (uint32_t)shaderFileNames.size();
    ref_data.graphicsPipelineState.pShaderFileNames = shaderFileNames.data();

    ref_data.pipelineUUID[0] = 85;
    ref_data.pipelineUUID[1] = 43;
    ref_data.pipelineUUID[2] = 255;
    ref_data.pipelineUUID[3] = 24;
    ref_data.pipelineUUID[4] = 155;
    ref_data.pipelineUUID[5] = 64;
    ref_data.pipelineUUID[6] = 62;
    ref_data.pipelineUUID[7] = 24;

    EXPECT_TRUE(vpjParsePipelineJson(this->parser_, json.c_str(), &data, &msg_));
    CHECK_PARSE(true);

    CompareData<VpjGraphicsPipelineState>(ref_data, data);
}

TEST_F(Parse, ObjectNameRemapping) {
    TEST_DESCRIPTION("Tests parsing of a reasonably simple compute pipeline JSON");

    const std::string json{R"({
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

    VpjData data;
    EXPECT_TRUE(vpjParsePipelineJson(this->parser_, json.c_str(), &data, &msg_));
    CHECK_PARSE(true);

    EXPECT_STREQ(data.computePipelineState.ppYcbcrSamplerNames[0], "ycbcr_conversion_1");
    EXPECT_STREQ(data.computePipelineState.ppYcbcrSamplerNames[1], "ycbcr_conversion_2");
    EXPECT_STREQ(data.computePipelineState.ppYcbcrSamplerNames[2], "ycbcr_conversion_3");

    EXPECT_STREQ(data.computePipelineState.ppImmutableSamplerNames[0], "immutable_sampler_1");
    EXPECT_STREQ(data.computePipelineState.ppImmutableSamplerNames[1], "immutable_sampler_2");
    EXPECT_STREQ(data.computePipelineState.ppImmutableSamplerNames[2], "immutable_sampler_3");

    EXPECT_STREQ(data.computePipelineState.ppDescriptorSetLayoutNames[0], "descriptor_layout_1");
    EXPECT_STREQ(data.computePipelineState.ppDescriptorSetLayoutNames[1], "descriptor_layout_2");
    EXPECT_STREQ(data.computePipelineState.ppDescriptorSetLayoutNames[2], "descriptor_layout_3");

    const auto pipelineLayout = reinterpret_cast<const VkPipelineLayoutCreateInfo*>(data.computePipelineState.pPipelineLayout);
    EXPECT_EQ(uint64_t(pipelineLayout->pSetLayouts[0]), 2);
    EXPECT_EQ(uint64_t(pipelineLayout->pSetLayouts[1]), 0);
    EXPECT_EQ(uint64_t(pipelineLayout->pSetLayouts[2]), 1);

    EXPECT_EQ(data.computePipelineState.immutableSamplerCount, 3);

    const auto immutableSamplers = reinterpret_cast<const VkSamplerCreateInfo*>(data.computePipelineState.pImmutableSamplers);

    const auto ycbcrConversionInfo_0 = reinterpret_cast<const VkSamplerYcbcrConversionInfo*>(immutableSamplers[2].pNext);
    EXPECT_EQ(uint64_t(ycbcrConversionInfo_0->conversion), 0);

    const auto ycbcrConversionInfo_1 = reinterpret_cast<const VkSamplerYcbcrConversionInfo*>(immutableSamplers[0].pNext);
    EXPECT_EQ(uint64_t(ycbcrConversionInfo_1->conversion), 2);

    const auto descriptorSetLayouts =
        reinterpret_cast<const VkDescriptorSetLayoutCreateInfo*>(data.computePipelineState.pDescriptorSetLayouts);
    EXPECT_EQ(descriptorSetLayouts[0].pBindings[0].descriptorCount, 2);

    EXPECT_EQ(uint64_t(descriptorSetLayouts[0].pBindings[0].pImmutableSamplers[0]), 2);
    EXPECT_EQ(uint64_t(descriptorSetLayouts[0].pBindings[0].pImmutableSamplers[1]), 0);

    EXPECT_EQ(descriptorSetLayouts[2].pBindings[0].descriptorCount, 2);

    EXPECT_EQ(uint64_t(descriptorSetLayouts[2].pBindings[0].pImmutableSamplers[0]), 0);
    EXPECT_EQ(uint64_t(descriptorSetLayouts[2].pBindings[0].pImmutableSamplers[1]), 1);
}
