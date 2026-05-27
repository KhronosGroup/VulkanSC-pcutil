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

    CHECK_PARSE(vpjParsePipelineJson(this->parser_, json.c_str(), &data, &msg_));

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

TEST_F(Parse, ComputePipelineJSONAbove32Bit) {
    TEST_DESCRIPTION(
        "Tests parsing of a reasonably simple compute pipeline JSON with handles values not representable as 32-bit values");

    VpjData data{};
    constexpr uint64_t base = 4'294'967'295;

    const char* ycbcr_names[1] = {"YcbcrConversion1"};
    auto [ycbcr_ci, ycbcr_json] = getVkSamplerYcbcrConversionCreateInfo(0);

    const char* sampler_names[2] = {"ImmutableSampler1", "YcbcrSampler1"};
    auto [immut_sampler_ci, immut_sampler_json] = getVkSamplerCreateInfo(0);
    auto [ycbcr_sampler_ci, ycbcr_sampler_json] = getVkSamplerCreateInfo(0, {VkSamplerYcbcrConversion(base + 1), ycbcr_names[0]});
    VkSamplerCreateInfo sampler_ci[2] = {*immut_sampler_ci.ptr(), *ycbcr_sampler_ci.ptr()};

    const char* dsl_names[1] = {"DescriptorSetLayout1"};
    auto [dsl_ci, dsl_json] =
        getVkDescriptorSetLayoutCreateInfo(0, {{VkSampler(base + 1), sampler_names[0]}, {VkSampler(base + 2), sampler_names[1]}});

    auto [pl_ci, pl_json] = getVkPipelineLayoutCreateInfo(0, {{VkDescriptorSetLayout(base + 1), dsl_names[0]}});

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

TEST_F(Parse, GraphicsPipelineJSONAbove32Bit) {
    TEST_DESCRIPTION(
        "Tests parsing of a reasonably simple graphics pipeline JSON with handles values not representable as 32-bit values");

    VpjData data{};
    constexpr uint64_t base = 4'294'967'295;

    const char* ycbcr_names[1] = {"YcbcrConversion1"};
    auto [ycbcr_ci, ycbcr_json] = getVkSamplerYcbcrConversionCreateInfo(1);

    const char* sampler_names[2] = {"ImmutableSampler1", "YcbcrSampler1"};
    auto [immut_sampler_ci, immut_sampler_json] = getVkSamplerCreateInfo(1);
    auto [ycbcr_sampler_ci, ycbcr_sampler_json] = getVkSamplerCreateInfo(1, {VkSamplerYcbcrConversion(base + 1), ycbcr_names[0]});
    VkSamplerCreateInfo sampler_ci[2] = {*immut_sampler_ci.ptr(), *ycbcr_sampler_ci.ptr()};

    const char* dsl_names[1] = {"DescriptorSetLayout1"};
    auto [dsl_ci, dsl_json] =
        getVkDescriptorSetLayoutCreateInfo(1, {{VkSampler(base + 1), sampler_names[0]}, {VkSampler(base + 2), sampler_names[1]}});

    auto [pl_ci, pl_json] = getVkPipelineLayoutCreateInfo(1, {{VkDescriptorSetLayout(base + 1), dsl_names[0]}});

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

TEST_F(Parse, AcceptLegacyInvalidInputData) {
    TEST_DESCRIPTION("Tests parsing legacy pipeline JSON produced by the old generator used in old versions of the CTS");

    const std::string json{R"({
        "GraphicsPipelineState" :
        {
            "Renderpass" : 
            {
                "sType" : "VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO",
                "pNext":"NULL",
                "flags" : "0",
                "attachmentCount" : 1,
                "pAttachments": 
                [
                {
                    "flags" : "0",
                    "format" : "VK_FORMAT_R8G8B8A8_UNORM",
                    "samples" : "VK_SAMPLE_COUNT_1_BIT",
                    "loadOp" : "VK_ATTACHMENT_LOAD_OP_CLEAR",
                    "storeOp" : "VK_ATTACHMENT_STORE_OP_STORE",
                    "stencilLoadOp" : "VK_ATTACHMENT_LOAD_OP_DONT_CARE",
                    "stencilStoreOp" : "VK_ATTACHMENT_STORE_OP_DONT_CARE",
                    "initialLayout" : "VK_IMAGE_LAYOUT_UNDEFINED",
                    "finalLayout" : "VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL"
                }
                ],
                "subpassCount" : 1,
                "pSubpasses": 
                [
                {
                    "flags" : "0",
                    "pipelineBindPoint" : "VK_PIPELINE_BIND_POINT_GRAPHICS",
                    "inputAttachmentCount" : 0,
                    "pInputAttachments": 
                    "NULL",
                    "colorAttachmentCount" : 1,
                    "pColorAttachments": 
                    [
                    {
                        "attachment" : 0,
                        "layout" : "VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL"
                    }
                    ],
                    "pResolveAttachments": 
                    "NULL",
                    "pDepthStencilAttachment": 
                    "NULL",
                    "preserveAttachmentCount" : 0,
                    "pPreserveAttachments":
                    [
                    ]
                }
                ],
                "dependencyCount" : 0,
                "pDependencies": 
                "NULL"
            },
)"
                           R"(
            "DescriptorSetLayouts" :
            [
                {
                    "8":
                    {
                        "sType" : "VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO",
                        "pNext":"NULL",
                        "flags" : "0",
                        "bindingCount" : 3,
                        "pBindings": 
                        [
                        {
                            "binding" : 0,
                            "descriptorType" : "VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER",
                            "descriptorCount" : 1,
                            "stageFlags" : "VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT | VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT | VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT | VK_SHADER_STAGE_TASK_BIT_NV | VK_SHADER_STAGE_MESH_BIT_NV | VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_MISS_BIT_KHR | VK_SHADER_STAGE_INTERSECTION_BIT_KHR | VK_SHADER_STAGE_CALLABLE_BIT_KHR | VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI",
                            "pImmutableSamplers":
                            "NULL"
                        },
                        {
                            "binding" : 1,
                            "descriptorType" : "VK_DESCRIPTOR_TYPE_STORAGE_BUFFER",
                            "descriptorCount" : 1,
                            "stageFlags" : "VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT | VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT | VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT | VK_SHADER_STAGE_TASK_BIT_NV | VK_SHADER_STAGE_MESH_BIT_NV | VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_MISS_BIT_KHR | VK_SHADER_STAGE_INTERSECTION_BIT_KHR | VK_SHADER_STAGE_CALLABLE_BIT_KHR | VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI",
                            "pImmutableSamplers":
                            "NULL"
                        },
                        {
                            "binding" : 2,
                            "descriptorType" : "VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER",
                            "descriptorCount" : 1,
                            "stageFlags" : "VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT | VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT | VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT | VK_SHADER_STAGE_TASK_BIT_NV | VK_SHADER_STAGE_MESH_BIT_NV | VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_MISS_BIT_KHR | VK_SHADER_STAGE_INTERSECTION_BIT_KHR | VK_SHADER_STAGE_CALLABLE_BIT_KHR | VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI",
                            "pImmutableSamplers":
                            "NULL"
                        }
                        ]
                    }
                }
            ],
)"
                           R"(
            "PipelineLayout" : 
            {
                "sType" : "VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO",
                "pNext":"NULL",
                "flags" : 0,
                "setLayoutCount" : 1,
                "pSetLayouts":
                [
                8
                ],
                "pushConstantRangeCount" : 0,
                "pPushConstantRanges": 
                "NULL"
            },
)"
                           R"(
            "GraphicsPipeline" : 
            {
                "sType" : "VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO",
                "pNext":
                {
                    "sType" : "VK_STRUCTURE_TYPE_PIPELINE_OFFLINE_CREATE_INFO",
                    "pNext":"NULL",
                    "pipelineIdentifier":
                    [
                    122,
                    93,
                    143,
                    188,
                    110,
                    183,
                    153,
                    137,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0
                    ],
                    "matchControl" : "VK_PIPELINE_MATCH_CONTROL_APPLICATION_UUID_EXACT_MATCH",
                    "poolEntrySize" : "42"
                },
                "flags" : "0",
                "stageCount" : 2,
                "pStages": 
                [
                {
                    "sType" : "VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO",
                    "pNext":"NULL",
                    "flags" : "0",
                    "stage" : "VK_SHADER_STAGE_VERTEX_BIT",
                    "module" : 21,
                    "pName" : "main",
                    "pSpecializationInfo": 
                    "NULL"
                },
                {
                    "sType" : "VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO",
                    "pNext":"NULL",
                    "flags" : "0",
                    "stage" : "VK_SHADER_STAGE_FRAGMENT_BIT",
                    "module" : 22,
                    "pName" : "main",
                    "pSpecializationInfo": 
                    "NULL"
                }
                ],
)"
                           R"(
                "pVertexInputState": 
                {
                    "sType" : "VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO",
                    "pNext":"NULL",
                    "flags" : 0,
                    "vertexBindingDescriptionCount" : 1,
                    "pVertexBindingDescriptions": 
                    [
                    {
                        "binding" : 0,
                        "stride" : 16,
                        "inputRate" : "VK_VERTEX_INPUT_RATE_VERTEX"
                    }
                    ],
                    "vertexAttributeDescriptionCount" : 1,
                    "pVertexAttributeDescriptions": 
                    [
                    {
                        "location" : 0,
                        "binding" : 0,
                        "format" : "VK_FORMAT_R32G32B32A32_SFLOAT",
                        "offset" : 0
                    }
                    ]
                },
                "pInputAssemblyState": 
                {
                    "sType" : "VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO",
                    "pNext":"NULL",
                    "flags" : 0,
                    "topology" : "VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP",
                    "primitiveRestartEnable" : "VK_FALSE"
                },
                "pTessellationState": 
                "NULL",
                "pViewportState": 
                {
                    "sType" : "VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO",
                    "pNext":"NULL",
                    "flags" : 0,
                    "viewportCount" : 1,
                    "pViewports": 
                    [
                    {
                        "x" : 0,
                        "y" : 0,
                        "width" : 16,
                        "height" : 16,
                        "minDepth" : 0,
                        "maxDepth" : 1
                    }
                    ],
                    "scissorCount" : 1,
                    "pScissors": 
                    [
                    {
                        "offset": 
                        {
                            "x" : 0,
                            "y" : 0
                        },
                        "extent": 
                        {
                            "width" : 16,
                            "height" : 16
                        }
                    }
                    ]
                },
)"
                           R"(
                "pRasterizationState": 
                {
                    "sType" : "VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO",
                    "pNext":"NULL",
                    "flags" : 0,
                    "depthClampEnable" : "VK_FALSE",
                    "rasterizerDiscardEnable" : "VK_FALSE",
                    "polygonMode" : "VK_POLYGON_MODE_FILL",
                    "cullMode" : "0",
                    "frontFace" : "VK_FRONT_FACE_COUNTER_CLOCKWISE",
                    "depthBiasEnable" : "VK_FALSE",
                    "depthBiasConstantFactor" : 0,
                    "depthBiasClamp" : 0,
                    "depthBiasSlopeFactor" : 0,
                    "lineWidth" : 1
                },
                "pMultisampleState": 
                {
                    "sType" : "VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO",
                    "pNext":"NULL",
                    "flags" : 0,
                    "rasterizationSamples" : "VK_SAMPLE_COUNT_1_BIT",
                    "sampleShadingEnable" : "VK_FALSE",
                    "minSampleShading" : 1,
                    "pSampleMask":
                    "NULL",
                    "alphaToCoverageEnable" : "VK_FALSE",
                    "alphaToOneEnable" : "VK_FALSE"
                },
                "pDepthStencilState": 
                {
                    "sType" : "VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO",
                    "pNext":"NULL",
                    "flags" : 0,
                    "depthTestEnable" : "VK_FALSE",
                    "depthWriteEnable" : "VK_FALSE",
                    "depthCompareOp" : "VK_COMPARE_OP_LESS_OR_EQUAL",
                    "depthBoundsTestEnable" : "VK_FALSE",
                    "stencilTestEnable" : "VK_FALSE",
                    "front": 
                    {
                        "failOp" : "VK_STENCIL_OP_KEEP",
                        "passOp" : "VK_STENCIL_OP_KEEP",
                        "depthFailOp" : "VK_STENCIL_OP_KEEP",
                        "compareOp" : "VK_COMPARE_OP_NEVER",
                        "compareMask" : 0,
                        "writeMask" : 0,
                        "reference" : 0
                    },
                    "back": 
                    {
                        "failOp" : "VK_STENCIL_OP_KEEP",
                        "passOp" : "VK_STENCIL_OP_KEEP",
                        "depthFailOp" : "VK_STENCIL_OP_KEEP",
                        "compareOp" : "VK_COMPARE_OP_NEVER",
                        "compareMask" : 0,
                        "writeMask" : 0,
                        "reference" : 0
                    },
                    "minDepthBounds" : 0,
                    "maxDepthBounds" : 1
                },
                "pColorBlendState": 
                {
                    "sType" : "VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO",
                    "pNext":"NULL",
                    "flags" : 0,
                    "logicOpEnable" : "VK_FALSE",
                    "logicOp" : "VK_LOGIC_OP_CLEAR",
                    "attachmentCount" : 1,
                    "pAttachments": 
                    [
                    {
                        "blendEnable" : "VK_FALSE",
                        "srcColorBlendFactor" : "VK_BLEND_FACTOR_ZERO",
                        "dstColorBlendFactor" : "VK_BLEND_FACTOR_ZERO",
                        "colorBlendOp" : "VK_BLEND_OP_ADD",
                        "srcAlphaBlendFactor" : "VK_BLEND_FACTOR_ZERO",
                        "dstAlphaBlendFactor" : "VK_BLEND_FACTOR_ZERO",
                        "alphaBlendOp" : "VK_BLEND_OP_ADD",
                        "colorWriteMask" : "VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT"
                    }
                    ],
                    "blendConstants":
                    [
                    0,
                    0,
                    0,
                    0
                    ]
                },
                "pDynamicState": 
                "NULL",
                "layout" : 20,
                "renderPass" : 18,
                "subpass" : 0,
                "basePipelineHandle" : "",
                "basePipelineIndex" : 0
            },
)"
                           R"(
            "ShaderFileNames" :
            [
                {
                    "stage" : "VK_SHADER_STAGE_VERTEX_BIT",
                    "filename" : "shader_0_21.vert.spv"
                },
                {
                    "stage" : "VK_SHADER_STAGE_FRAGMENT_BIT",
                    "filename" : "shader_0_22.frag.spv"
                }
            ],
)"
                           R"(
            "PhysicalDeviceFeatures" : 
            {
                "sType" : "VK_STRUCTURE_TYPE_FAULT_CALLBACK_INFO",
                "pNext":
                {
                    "sType" : "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_SC_1_0_FEATURES",
                    "pNext":
                    {
                        "sType" : "VK_STRUCTURE_TYPE_DEVICE_OBJECT_RESERVATION_CREATE_INFO",
                        "pNext":
                        {
                            "sType" : "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2",
                            "pNext":"NULL",
                            "features": 
                            {
                                "robustBufferAccess" : "VK_TRUE",
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
                                "fragmentStoresAndAtomics" : "VK_TRUE",
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
                        },
)"
                           R"(
                        "pipelineCacheCreateInfoCount" : 0,
                        "pPipelineCacheCreateInfos": 
                        "NULL",
                        "pipelinePoolSizeCount" : 0,
                        "pPipelinePoolSizes": 
                        "NULL",
                        "semaphoreRequestCount" : 0,
                        "commandBufferRequestCount" : 0,
                        "fenceRequestCount" : 0,
                        "deviceMemoryRequestCount" : 0,
                        "bufferRequestCount" : 0,
                        "imageRequestCount" : 0,
                        "eventRequestCount" : 0,
                        "queryPoolRequestCount" : 0,
                        "bufferViewRequestCount" : 0,
                        "imageViewRequestCount" : 0,
                        "layeredImageViewRequestCount" : 0,
                        "pipelineCacheRequestCount" : 0,
                        "pipelineLayoutRequestCount" : 0,
                        "renderPassRequestCount" : 0,
                        "graphicsPipelineRequestCount" : 0,
                        "computePipelineRequestCount" : 0,
                        "descriptorSetLayoutRequestCount" : 0,
                        "samplerRequestCount" : 0,
                        "descriptorPoolRequestCount" : 0,
                        "descriptorSetRequestCount" : 0,
                        "framebufferRequestCount" : 0,
                        "commandPoolRequestCount" : 0,
                        "samplerYcbcrConversionRequestCount" : 0,
                        "surfaceRequestCount" : 0,
                        "swapchainRequestCount" : 0,
                        "displayModeRequestCount" : 0,
                        "subpassDescriptionRequestCount" : 0,
                        "attachmentDescriptionRequestCount" : 0,
                        "descriptorSetLayoutBindingRequestCount" : 0,
                        "descriptorSetLayoutBindingLimit" : 0,
                        "maxImageViewMipLevels" : 0,
                        "maxImageViewArrayLayers" : 0,
                        "maxLayeredImageViewMipLevels" : 0,
                        "maxOcclusionQueriesPerPool" : 0,
                        "maxPipelineStatisticsQueriesPerPool" : 0,
                        "maxTimestampQueriesPerPool" : 0,
                        "maxImmutableSamplersPerDescriptorSetLayout" : 0
                    },
                    "shaderAtomicInstructions" : "VK_FALSE"
                },
)"
                           R"(
                "features": 
                {
                    "robustBufferAccess" : "VK_FALSE",
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
            }
        },
)"
                           R"(
        "EnabledExtensions" : 
        [
            "VK_KHR_swapchain",
            "VK_KHR_shader_clock",
            "VK_KHR_global_priority",
            "VK_KHR_swapchain_mutable_format",
            "VK_KHR_shader_terminate_invocation",
            "VK_KHR_fragment_shading_rate",
            "VK_KHR_synchronization2",
            "VK_KHR_copy_commands2",
            "VK_KHR_vertex_attribute_divisor",
            "VK_KHR_index_type_uint8",
            "VK_KHR_line_rasterization",
            "VK_KHR_calibrated_timestamps",
            "VK_EXT_depth_range_unrestricted",
            "VK_EXT_discard_rectangles",
            "VK_EXT_conservative_rasterization",
            "VK_EXT_depth_clip_enable",
            "VK_EXT_hdr_metadata",
            "VK_EXT_queue_family_foreign",
            "VK_EXT_sample_locations",
            "VK_EXT_blend_operation_advanced",
            "VK_EXT_post_depth_coverage",
            "VK_EXT_external_memory_host",
            "VK_EXT_pci_bus_info",
            "VK_EXT_subgroup_size_control",
            "VK_EXT_shader_image_atomic_int64",
            "VK_EXT_memory_budget",
            "VK_EXT_fragment_shader_interlock",
            "VK_EXT_ycbcr_image_arrays",
            "VK_EXT_line_rasterization",
            "VK_EXT_shader_atomic_float",
            "VK_EXT_index_type_uint8",
            "VK_EXT_extended_dynamic_state",
            "VK_EXT_shader_demote_to_helper_invocation",
            "VK_EXT_texel_buffer_alignment",
            "VK_EXT_robustness2",
            "VK_EXT_custom_border_color",
            "VK_EXT_ycbcr_2plane_444_formats",
            "VK_EXT_image_robustness",
            "VK_EXT_4444_formats",
            "VK_EXT_vertex_input_dynamic_state",
            "VK_EXT_extended_dynamic_state2",
            "VK_EXT_color_write_enable"
        ],
)"
                           R"(
        "PipelineUUID" : 
        [
            122,
            93,
            143,
            188,
            110,
            183,
            153,
            137,
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
    vpjSetAcceptLegacyInvalidInputData(this->parser_, true);
    EXPECT_TRUE(vpjParsePipelineJson(this->parser_, json.c_str(), &data, &msg_));

    // Verify feature structs
    auto features2 = reinterpret_cast<const VkPhysicalDeviceFeatures2*>(data.graphicsPipelineState.pPhysicalDeviceFeatures);
    EXPECT_EQ(features2->sType, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2);

    auto features_sc10 = reinterpret_cast<const VkPhysicalDeviceVulkanSC10Features*>(features2->pNext);
    EXPECT_EQ(features_sc10->sType, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_SC_1_0_FEATURES);
    EXPECT_EQ(features_sc10->pNext, nullptr);

    // Verify that only valid shader stage flags are kept
    const VkShaderStageFlags valid_shader_stage_flags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT |
                                                        VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT | VK_SHADER_STAGE_GEOMETRY_BIT |
                                                        VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT;

    auto dsl_ci = reinterpret_cast<const VkDescriptorSetLayoutCreateInfo*>(data.graphicsPipelineState.pDescriptorSetLayouts);
    for (uint32_t i = 0; i < data.graphicsPipelineState.descriptorSetLayoutCount; ++i) {
        for (uint32_t binding_idx = 0; binding_idx < dsl_ci[i].bindingCount; ++i) {
            auto parsed_shader_stage_flags = dsl_ci[i].pBindings[binding_idx].stageFlags;
            EXPECT_EQ(parsed_shader_stage_flags, (parsed_shader_stage_flags & valid_shader_stage_flags));
        }
    }

    // Verify that pool entry size stored as string instead of integer was parsed correctly
    auto gp_ci = reinterpret_cast<const VkGraphicsPipelineCreateInfo*>(data.graphicsPipelineState.pGraphicsPipeline);
    auto po_ci = reinterpret_cast<const VkPipelineOfflineCreateInfo*>(gp_ci->pNext);
    EXPECT_EQ(po_ci->sType, VK_STRUCTURE_TYPE_PIPELINE_OFFLINE_CREATE_INFO);
    EXPECT_EQ(po_ci->poolEntrySize, 42);
}
