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
    void EXPECT_UUIDEQ(uint8_t* it1, uint8_t* it2) {
        for (unsigned int i = 0; i < VK_UUID_SIZE; ++i) {
            EXPECT_EQ(it1[i], it2[i]);
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
            "pNext": "NULL",
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
    };

    test_eq(R"("VK_TRUE")", VK_TRUE);
    test_eq(R"(0)", 0);
    test_eq(R"("VK_FALSE")", VK_FALSE);
    test_eq(R"(4294967295)", 4294967295);
}

TEST_F(Parse, VkPhysicalDeviceFeatures2) {
    TEST_DESCRIPTION("Tests parsing of a reasonably complex physical device features 2 JSON");

    VkPhysicalDeviceFeatures2 pdf;
    std::string json = {R"({
        "sType" : "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2",
        "pNext": {
            "sType" : "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES",
            "pNext": "NULL",
            "synchronization2" : "VK_TRUE"
        },
        "features": {
            "robustBufferAccess": "VK_FALSE",
            "fullDrawIndexUint32" : "VK_TRUE",
            "imageCubeArray" : "VK_FALSE",
            "independentBlend" : "VK_TRUE",
            "geometryShader" : "VK_FALSE",
            "tessellationShader" : "VK_TRUE",
            "sampleRateShading" : "VK_FALSE",
            "dualSrcBlend" : "VK_TRUE",
            "logicOp" : "VK_FALSE",
            "multiDrawIndirect" : "VK_TRUE",
            "drawIndirectFirstInstance" : "VK_FALSE",
            "depthClamp" : "VK_FALSE",
            "depthBiasClamp" : "VK_TRUE",
            "fillModeNonSolid" : "VK_FALSE",
            "depthBounds" : "VK_TRUE",
            "wideLines" : "VK_FALSE",
            "largePoints" : "VK_TRUE",
            "alphaToOne" : "VK_FALSE",
            "multiViewport" : "VK_TRUE",
            "samplerAnisotropy" : "VK_FALSE",
            "textureCompressionETC2" : "VK_TRUE",
            "textureCompressionASTC_LDR" : "VK_FALSE",
            "textureCompressionBC" : "VK_TRUE",
            "occlusionQueryPrecise" : "VK_FALSE",
            "pipelineStatisticsQuery" : "VK_TRUE",
            "vertexPipelineStoresAndAtomics" : "VK_FALSE",
            "fragmentStoresAndAtomics" : "VK_FALSE",
            "shaderTessellationAndGeometryPointSize" : "VK_TRUE",
            "shaderImageGatherExtended" : "VK_FALSE",
            "shaderStorageImageExtendedFormats" : "VK_TRUE",
            "shaderStorageImageMultisample" : "VK_FALSE",
            "shaderStorageImageReadWithoutFormat" : "VK_TRUE",
            "shaderStorageImageWriteWithoutFormat" : "VK_FALSE",
            "shaderUniformBufferArrayDynamicIndexing" : "VK_TRUE",
            "shaderSampledImageArrayDynamicIndexing" : "VK_FALSE",
            "shaderStorageBufferArrayDynamicIndexing" : "VK_TRUE",
            "shaderStorageImageArrayDynamicIndexing" : "VK_FALSE",
            "shaderClipDistance" : "VK_TRUE",
            "shaderCullDistance" : "VK_FALSE",
            "shaderFloat64" : "VK_TRUE",
            "shaderInt64" : "VK_FALSE",
            "shaderInt16" : "VK_TRUE",
            "shaderResourceResidency" : "VK_FALSE",
            "shaderResourceMinLod" : "VK_TRUE",
            "sparseBinding" : "VK_FALSE",
            "sparseResidencyBuffer" : "VK_TRUE",
            "sparseResidencyImage2D" : "VK_FALSE",
            "sparseResidencyImage3D" : "VK_TRUE",
            "sparseResidency2Samples" : "VK_FALSE",
            "sparseResidency4Samples" : "VK_TRUE",
            "sparseResidency8Samples" : "VK_FALSE",
            "sparseResidency16Samples" : "VK_TRUE",
            "sparseResidencyAliased" : "VK_FALSE",
            "variableMultisampleRate" : "VK_TRUE",
            "inheritedQueries" : "VK_FALSE"
        }
    })"};

    CHECK_PARSE(vpjParseSingleStructJson(this->parser_, json.c_str(), &pdf, &msg_));
    const auto& pdfsync2 = *reinterpret_cast<const VkPhysicalDeviceSynchronization2Features*>(pdf.pNext);

    EXPECT_EQ(pdf.sType, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2);
    EXPECT_EQ(pdfsync2.sType, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES);
    EXPECT_EQ(pdfsync2.pNext, nullptr);
    EXPECT_EQ(pdfsync2.synchronization2, VK_TRUE);
    EXPECT_EQ(pdf.features.robustBufferAccess, VK_FALSE);
    EXPECT_EQ(pdf.features.fullDrawIndexUint32, VK_TRUE);
    EXPECT_EQ(pdf.features.imageCubeArray, VK_FALSE);
    EXPECT_EQ(pdf.features.independentBlend, VK_TRUE);
    EXPECT_EQ(pdf.features.geometryShader, VK_FALSE);
    EXPECT_EQ(pdf.features.tessellationShader, VK_TRUE);
    EXPECT_EQ(pdf.features.sampleRateShading, VK_FALSE);
    EXPECT_EQ(pdf.features.dualSrcBlend, VK_TRUE);
    EXPECT_EQ(pdf.features.logicOp, VK_FALSE);
    EXPECT_EQ(pdf.features.multiDrawIndirect, VK_TRUE);
    EXPECT_EQ(pdf.features.drawIndirectFirstInstance, VK_FALSE);
    EXPECT_EQ(pdf.features.depthClamp, VK_FALSE);
    EXPECT_EQ(pdf.features.depthBiasClamp, VK_TRUE);
    EXPECT_EQ(pdf.features.fillModeNonSolid, VK_FALSE);
    EXPECT_EQ(pdf.features.depthBounds, VK_TRUE);
    EXPECT_EQ(pdf.features.wideLines, VK_FALSE);
    EXPECT_EQ(pdf.features.largePoints, VK_TRUE);
    EXPECT_EQ(pdf.features.alphaToOne, VK_FALSE);
    EXPECT_EQ(pdf.features.multiViewport, VK_TRUE);
    EXPECT_EQ(pdf.features.samplerAnisotropy, VK_FALSE);
    EXPECT_EQ(pdf.features.textureCompressionETC2, VK_TRUE);
    EXPECT_EQ(pdf.features.textureCompressionASTC_LDR, VK_FALSE);
    EXPECT_EQ(pdf.features.textureCompressionBC, VK_TRUE);
    EXPECT_EQ(pdf.features.occlusionQueryPrecise, VK_FALSE);
    EXPECT_EQ(pdf.features.pipelineStatisticsQuery, VK_TRUE);
    EXPECT_EQ(pdf.features.vertexPipelineStoresAndAtomics, VK_FALSE);
    EXPECT_EQ(pdf.features.fragmentStoresAndAtomics, VK_FALSE);
    EXPECT_EQ(pdf.features.shaderTessellationAndGeometryPointSize, VK_TRUE);
    EXPECT_EQ(pdf.features.shaderImageGatherExtended, VK_FALSE);
    EXPECT_EQ(pdf.features.shaderStorageImageExtendedFormats, VK_TRUE);
    EXPECT_EQ(pdf.features.shaderStorageImageMultisample, VK_FALSE);
    EXPECT_EQ(pdf.features.shaderStorageImageReadWithoutFormat, VK_TRUE);
    EXPECT_EQ(pdf.features.shaderStorageImageWriteWithoutFormat, VK_FALSE);
    EXPECT_EQ(pdf.features.shaderUniformBufferArrayDynamicIndexing, VK_TRUE);
    EXPECT_EQ(pdf.features.shaderSampledImageArrayDynamicIndexing, VK_FALSE);
    EXPECT_EQ(pdf.features.shaderStorageBufferArrayDynamicIndexing, VK_TRUE);
    EXPECT_EQ(pdf.features.shaderStorageImageArrayDynamicIndexing, VK_FALSE);
    EXPECT_EQ(pdf.features.shaderClipDistance, VK_TRUE);
    EXPECT_EQ(pdf.features.shaderCullDistance, VK_FALSE);
    EXPECT_EQ(pdf.features.shaderFloat64, VK_TRUE);
    EXPECT_EQ(pdf.features.shaderInt64, VK_FALSE);
    EXPECT_EQ(pdf.features.shaderInt16, VK_TRUE);
    EXPECT_EQ(pdf.features.shaderResourceResidency, VK_FALSE);
    EXPECT_EQ(pdf.features.shaderResourceMinLod, VK_TRUE);
    EXPECT_EQ(pdf.features.sparseBinding, VK_FALSE);
    EXPECT_EQ(pdf.features.sparseResidencyBuffer, VK_TRUE);
    EXPECT_EQ(pdf.features.sparseResidencyImage2D, VK_FALSE);
    EXPECT_EQ(pdf.features.sparseResidencyImage3D, VK_TRUE);
    EXPECT_EQ(pdf.features.sparseResidency2Samples, VK_FALSE);
    EXPECT_EQ(pdf.features.sparseResidency4Samples, VK_TRUE);
    EXPECT_EQ(pdf.features.sparseResidency8Samples, VK_FALSE);
    EXPECT_EQ(pdf.features.sparseResidency16Samples, VK_TRUE);
    EXPECT_EQ(pdf.features.sparseResidencyAliased, VK_FALSE);
    EXPECT_EQ(pdf.features.variableMultisampleRate, VK_TRUE);
    EXPECT_EQ(pdf.features.inheritedQueries, VK_FALSE);
}

TEST_F(Parse, VkGraphicsPipelineCreateInfo) {
    TEST_DESCRIPTION("Tests parsing of a reasonably complex compute pipeline create info JSON");

    VkGraphicsPipelineCreateInfo gp_ci;
    std::string json = {R"({
            "sType" : "VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO",
            "pNext": {
                "sType": "VK_STRUCTURE_TYPE_PIPELINE_DISCARD_RECTANGLE_STATE_CREATE_INFO_EXT",
                "pNext" : "NULL",
                "flags": "0",
                "discardRectangleMode": "VK_DISCARD_RECTANGLE_MODE_EXCLUSIVE_EXT",
                "discardRectangleCount": 1,
                "pDiscardRectangles": [
                    {
                        "offset":
                        {
                            "x" : 0,
                            "y" : 0
                        },
                        "extent":
                        {
                            "width" : 51,
                            "height" : 51
                        }
                    }
                ]
            },
            "flags" : "0",
            "stageCount" : 5,
            "pStages": 
            [
            {
                "sType" : "VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO",
                "pNext": {
                    "pNext" : "NULL",
                    "requiredSubgroupSize" : 64,
                    "sType" : "VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_REQUIRED_SUBGROUP_SIZE_CREATE_INFO"
                },
                "flags" : "0",
                "stage" : "VK_SHADER_STAGE_VERTEX_BIT",
                "module" : 35,
                "pName" : "main",
                "pSpecializationInfo": 
                "NULL"
            },
            {
                "sType" : "VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO",
                "pNext":"NULL",
                "flags" : "0",
                "stage" : "VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT",
                "module" : 36,
                "pName" : "main",
                "pSpecializationInfo": 
                "NULL"
            },
            {
                "sType" : "VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO",
                "pNext":"NULL",
                "flags" : "0",
                "stage" : "VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT",
                "module" : 37,
                "pName" : "main",
                "pSpecializationInfo": 
                "NULL"
            },
            {
                "sType" : "VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO",
                "pNext":"NULL",
                "flags" : "0",
                "stage" : "VK_SHADER_STAGE_GEOMETRY_BIT",
                "module" : 38,
                "pName" : "main",
                "pSpecializationInfo": 
                "NULL"
            },
            {
                "sType" : "VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO",
                "pNext":"NULL",
                "flags" : "0",
                "stage" : "VK_SHADER_STAGE_FRAGMENT_BIT",
                "module" : 39,
                "pName" : "main",
                "pSpecializationInfo": 
                "NULL"
            }
            ],
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
                    "stride" : 32,
                    "inputRate" : "VK_VERTEX_INPUT_RATE_VERTEX"
                }
                ],
                "vertexAttributeDescriptionCount" : 2,
                "pVertexAttributeDescriptions": 
                [
                {
                    "location" : 0,
                    "binding" : 0,
                    "format" : "VK_FORMAT_R32G32B32A32_SFLOAT",
                    "offset" : 0
                },
                {
                    "location" : 1,
                    "binding" : 0,
                    "format" : "VK_FORMAT_R32G32B32A32_SFLOAT",
                    "offset" : 16
                }
                ]
            },
            "pInputAssemblyState": 
            {
                "sType" : "VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO",
                "pNext":"NULL",
                "flags" : 0,
                "topology" : "VK_PRIMITIVE_TOPOLOGY_PATCH_LIST",
                "primitiveRestartEnable" : "VK_FALSE"
            },
            "pTessellationState": 
            {
                "sType" : "VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO",
                "pNext":"NULL",
                "flags" : 0,
                "patchControlPoints" : 4
            },
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
                    "width" : 51,
                    "height" : 51,
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
                        "width" : 51,
                        "height" : 51
                    }
                }
                ]
            },
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
                "depthTestEnable" : "VK_TRUE",
                "depthWriteEnable" : "VK_TRUE",
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
            "pDynamicState": {
                "sType": "VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO",
                "pNext": "NULL",
                "flags": 0,
                "dynamicStateCount": 3,
                "pDynamicStates": [
                    "VK_DYNAMIC_STATE_VIEWPORT",
                    "VK_DYNAMIC_STATE_SCISSOR",
                    "VK_DYNAMIC_STATE_DISCARD_RECTANGLE_EXT",

                ]
            },
            "layout" : 8,
            "renderPass" : 6,
            "subpass" : 0,
            "basePipelineHandle" : "",
            "basePipelineIndex" : 0
        })"};

    CHECK_PARSE(vpjParseSingleStructJson(this->parser_, json.c_str(), &gp_ci, &msg_));
    const auto& pdrs_ci = *reinterpret_cast<const VkPipelineDiscardRectangleStateCreateInfoEXT*>(gp_ci.pNext);
    const auto& pvis_ci = *reinterpret_cast<const VkPipelineVertexInputStateCreateInfo*>(gp_ci.pVertexInputState);
    const auto& pias_ci = *reinterpret_cast<const VkPipelineInputAssemblyStateCreateInfo*>(gp_ci.pInputAssemblyState);
    const auto& pts_ci = *reinterpret_cast<const VkPipelineTessellationStateCreateInfo*>(gp_ci.pTessellationState);
    const auto& pvs_ci = *reinterpret_cast<const VkPipelineViewportStateCreateInfo*>(gp_ci.pViewportState);
    const auto& prs_ci = *reinterpret_cast<const VkPipelineRasterizationStateCreateInfo*>(gp_ci.pRasterizationState);
    const auto& pmss_ci = *reinterpret_cast<const VkPipelineMultisampleStateCreateInfo*>(gp_ci.pMultisampleState);
    const auto& pdss_ci = *reinterpret_cast<const VkPipelineDepthStencilStateCreateInfo*>(gp_ci.pDepthStencilState);
    const auto& pcbs_ci = *reinterpret_cast<const VkPipelineColorBlendStateCreateInfo*>(gp_ci.pColorBlendState);
    const auto& pds_ci = *reinterpret_cast<const VkPipelineDynamicStateCreateInfo*>(gp_ci.pDynamicState);
    const auto& pssrss_ci = *reinterpret_cast<const VkPipelineShaderStageRequiredSubgroupSizeCreateInfo*>(gp_ci.pStages[0].pNext);

    EXPECT_EQ(gp_ci.sType, VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO);
    EXPECT_EQ(pdrs_ci.sType, VK_STRUCTURE_TYPE_PIPELINE_DISCARD_RECTANGLE_STATE_CREATE_INFO_EXT);
    EXPECT_EQ(pdrs_ci.pNext, nullptr);
    EXPECT_EQ(pdrs_ci.flags, 0);
    EXPECT_EQ(pdrs_ci.discardRectangleMode, VK_DISCARD_RECTANGLE_MODE_EXCLUSIVE_EXT);
    EXPECT_EQ(pdrs_ci.discardRectangleCount, 1);
    EXPECT_EQ(pdrs_ci.pDiscardRectangles[0].offset.x, 0);
    EXPECT_EQ(pdrs_ci.pDiscardRectangles[0].offset.y, 0);
    EXPECT_EQ(pdrs_ci.pDiscardRectangles[0].extent.width, 51);
    EXPECT_EQ(pdrs_ci.pDiscardRectangles[0].extent.height, 51);
    EXPECT_EQ(gp_ci.flags, 0);
    EXPECT_EQ(gp_ci.stageCount, 5);
    EXPECT_EQ(gp_ci.pStages[0].sType, VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO);
    EXPECT_EQ(pssrss_ci.sType, VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_REQUIRED_SUBGROUP_SIZE_CREATE_INFO);
    EXPECT_EQ(pssrss_ci.pNext, nullptr);
    EXPECT_EQ(pssrss_ci.requiredSubgroupSize, 64);
    EXPECT_EQ(gp_ci.pStages[0].flags, 0);
    EXPECT_EQ(gp_ci.pStages[0].stage, VK_SHADER_STAGE_VERTEX_BIT);
    EXPECT_EQ(gp_ci.pStages[0].module, reinterpret_cast<void*>(35));
    EXPECT_STREQ(gp_ci.pStages[0].pName, "main");
    EXPECT_EQ(gp_ci.pStages[0].pSpecializationInfo, nullptr);
    EXPECT_EQ(gp_ci.pStages[1].sType, VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO);
    EXPECT_EQ(gp_ci.pStages[1].pNext, nullptr);
    EXPECT_EQ(gp_ci.pStages[1].flags, 0);
    EXPECT_EQ(gp_ci.pStages[1].stage, VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT);
    EXPECT_EQ(gp_ci.pStages[1].module, reinterpret_cast<void*>(36));
    EXPECT_STREQ(gp_ci.pStages[1].pName, "main");
    EXPECT_EQ(gp_ci.pStages[1].pSpecializationInfo, nullptr);
    EXPECT_EQ(gp_ci.pStages[2].sType, VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO);
    EXPECT_EQ(gp_ci.pStages[2].pNext, nullptr);
    EXPECT_EQ(gp_ci.pStages[2].flags, 0);
    EXPECT_EQ(gp_ci.pStages[2].stage, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT);
    EXPECT_EQ(gp_ci.pStages[2].module, reinterpret_cast<void*>(37));
    EXPECT_STREQ(gp_ci.pStages[2].pName, "main");
    EXPECT_EQ(gp_ci.pStages[2].pSpecializationInfo, nullptr);
    EXPECT_EQ(gp_ci.pStages[3].sType, VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO);
    EXPECT_EQ(gp_ci.pStages[3].pNext, nullptr);
    EXPECT_EQ(gp_ci.pStages[3].flags, 0);
    EXPECT_EQ(gp_ci.pStages[3].stage, VK_SHADER_STAGE_GEOMETRY_BIT);
    EXPECT_EQ(gp_ci.pStages[3].module, reinterpret_cast<void*>(38));
    EXPECT_STREQ(gp_ci.pStages[3].pName, "main");
    EXPECT_EQ(gp_ci.pStages[3].pSpecializationInfo, nullptr);
    EXPECT_EQ(gp_ci.pStages[4].sType, VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO);
    EXPECT_EQ(gp_ci.pStages[4].pNext, nullptr);
    EXPECT_EQ(gp_ci.pStages[4].flags, 0);
    EXPECT_EQ(gp_ci.pStages[4].stage, VK_SHADER_STAGE_FRAGMENT_BIT);
    EXPECT_EQ(gp_ci.pStages[4].module, reinterpret_cast<void*>(39));
    EXPECT_STREQ(gp_ci.pStages[4].pName, "main");
    EXPECT_EQ(gp_ci.pStages[4].pSpecializationInfo, nullptr);
    EXPECT_EQ(pvis_ci.sType, VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO);
    EXPECT_EQ(pvis_ci.pNext, nullptr);
    EXPECT_EQ(pvis_ci.flags, 0);
    EXPECT_EQ(pvis_ci.vertexBindingDescriptionCount, 1);
    EXPECT_EQ(pvis_ci.pVertexBindingDescriptions[0].binding, 0);
    EXPECT_EQ(pvis_ci.pVertexBindingDescriptions[0].stride, 32);
    EXPECT_EQ(pvis_ci.pVertexBindingDescriptions[0].inputRate, VK_VERTEX_INPUT_RATE_VERTEX);
    EXPECT_EQ(pvis_ci.vertexAttributeDescriptionCount, 2);
    EXPECT_EQ(pvis_ci.pVertexAttributeDescriptions[0].location, 0);
    EXPECT_EQ(pvis_ci.pVertexAttributeDescriptions[0].binding, 0);
    EXPECT_EQ(pvis_ci.pVertexAttributeDescriptions[0].format, VK_FORMAT_R32G32B32A32_SFLOAT);
    EXPECT_EQ(pvis_ci.pVertexAttributeDescriptions[0].offset, 0);
    EXPECT_EQ(pvis_ci.pVertexAttributeDescriptions[1].location, 1);
    EXPECT_EQ(pvis_ci.pVertexAttributeDescriptions[1].binding, 0);
    EXPECT_EQ(pvis_ci.pVertexAttributeDescriptions[1].format, VK_FORMAT_R32G32B32A32_SFLOAT);
    EXPECT_EQ(pvis_ci.pVertexAttributeDescriptions[1].offset, 16);
    EXPECT_EQ(pias_ci.sType, VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO);
    EXPECT_EQ(pias_ci.pNext, nullptr);
    EXPECT_EQ(pias_ci.flags, 0);
    EXPECT_EQ(pias_ci.topology, VK_PRIMITIVE_TOPOLOGY_PATCH_LIST);
    EXPECT_EQ(pias_ci.primitiveRestartEnable, VK_FALSE);
    EXPECT_EQ(pts_ci.sType, VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO);
    EXPECT_EQ(pts_ci.pNext, nullptr);
    EXPECT_EQ(pts_ci.flags, 0);
    EXPECT_EQ(pts_ci.patchControlPoints, 4);
    EXPECT_EQ(pvs_ci.sType, VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO);
    EXPECT_EQ(pvs_ci.pNext, nullptr);
    EXPECT_EQ(pvs_ci.flags, 0);
    EXPECT_EQ(pvs_ci.viewportCount, 1);
    EXPECT_EQ(pvs_ci.pViewports[0].x, 0);
    EXPECT_EQ(pvs_ci.pViewports[0].y, 0);
    EXPECT_EQ(pvs_ci.pViewports[0].width, 51);
    EXPECT_EQ(pvs_ci.pViewports[0].height, 51);
    EXPECT_EQ(pvs_ci.pViewports[0].minDepth, 0);
    EXPECT_EQ(pvs_ci.pViewports[0].maxDepth, 1);
    EXPECT_EQ(pvs_ci.scissorCount, 1);
    EXPECT_EQ(pvs_ci.pScissors[0].offset.x, 0);
    EXPECT_EQ(pvs_ci.pScissors[0].offset.y, 0);
    EXPECT_EQ(pvs_ci.pScissors[0].extent.width, 51);
    EXPECT_EQ(pvs_ci.pScissors[0].extent.height, 51);
    EXPECT_EQ(prs_ci.sType, VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO);
    EXPECT_EQ(prs_ci.pNext, nullptr);
    EXPECT_EQ(prs_ci.flags, 0);
    EXPECT_EQ(prs_ci.depthClampEnable, VK_FALSE);
    EXPECT_EQ(prs_ci.rasterizerDiscardEnable, VK_FALSE);
    EXPECT_EQ(prs_ci.polygonMode, VK_POLYGON_MODE_FILL);
    EXPECT_EQ(prs_ci.cullMode, 0);
    EXPECT_EQ(prs_ci.frontFace, VK_FRONT_FACE_COUNTER_CLOCKWISE);
    EXPECT_EQ(prs_ci.depthBiasEnable, VK_FALSE);
    EXPECT_EQ(prs_ci.depthBiasConstantFactor, 0);
    EXPECT_EQ(prs_ci.depthBiasClamp, 0);
    EXPECT_EQ(prs_ci.depthBiasSlopeFactor, 0);
    EXPECT_EQ(prs_ci.lineWidth, 1);
    EXPECT_EQ(pmss_ci.sType, VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO);
    EXPECT_EQ(pmss_ci.pNext, nullptr);
    EXPECT_EQ(pmss_ci.flags, 0);
    EXPECT_EQ(pmss_ci.rasterizationSamples, VK_SAMPLE_COUNT_1_BIT);
    EXPECT_EQ(pmss_ci.sampleShadingEnable, VK_FALSE);
    EXPECT_EQ(pmss_ci.minSampleShading, 1);
    EXPECT_EQ(pmss_ci.pSampleMask, nullptr);
    EXPECT_EQ(pmss_ci.alphaToCoverageEnable, VK_FALSE);
    EXPECT_EQ(pmss_ci.alphaToOneEnable, VK_FALSE);
    EXPECT_EQ(pdss_ci.sType, VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO);
    EXPECT_EQ(pdss_ci.pNext, nullptr);
    EXPECT_EQ(pdss_ci.flags, 0);
    EXPECT_EQ(pdss_ci.depthTestEnable, VK_TRUE);
    EXPECT_EQ(pdss_ci.depthWriteEnable, VK_TRUE);
    EXPECT_EQ(pdss_ci.depthCompareOp, VK_COMPARE_OP_LESS_OR_EQUAL);
    EXPECT_EQ(pdss_ci.depthBoundsTestEnable, VK_FALSE);
    EXPECT_EQ(pdss_ci.stencilTestEnable, VK_FALSE);
    EXPECT_EQ(pdss_ci.front.failOp, VK_STENCIL_OP_KEEP);
    EXPECT_EQ(pdss_ci.front.passOp, VK_STENCIL_OP_KEEP);
    EXPECT_EQ(pdss_ci.front.depthFailOp, VK_STENCIL_OP_KEEP);
    EXPECT_EQ(pdss_ci.front.compareOp, VK_COMPARE_OP_NEVER);
    EXPECT_EQ(pdss_ci.front.compareMask, 0);
    EXPECT_EQ(pdss_ci.front.writeMask, 0);
    EXPECT_EQ(pdss_ci.front.reference, 0);
    EXPECT_EQ(pdss_ci.back.failOp, VK_STENCIL_OP_KEEP);
    EXPECT_EQ(pdss_ci.back.passOp, VK_STENCIL_OP_KEEP);
    EXPECT_EQ(pdss_ci.back.depthFailOp, VK_STENCIL_OP_KEEP);
    EXPECT_EQ(pdss_ci.back.compareOp, VK_COMPARE_OP_NEVER);
    EXPECT_EQ(pdss_ci.back.compareMask, 0);
    EXPECT_EQ(pdss_ci.back.writeMask, 0);
    EXPECT_EQ(pdss_ci.back.reference, 0);
    EXPECT_EQ(pdss_ci.minDepthBounds, 0);
    EXPECT_EQ(pdss_ci.maxDepthBounds, 1);
    EXPECT_EQ(pcbs_ci.sType, VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO);
    EXPECT_EQ(pcbs_ci.pNext, nullptr);
    EXPECT_EQ(pcbs_ci.flags, 0);
    EXPECT_EQ(pcbs_ci.logicOpEnable, VK_FALSE);
    EXPECT_EQ(pcbs_ci.logicOp, VK_LOGIC_OP_CLEAR);
    EXPECT_EQ(pcbs_ci.attachmentCount, 1);
    EXPECT_EQ(pcbs_ci.pAttachments[0].blendEnable, VK_FALSE);
    EXPECT_EQ(pcbs_ci.pAttachments[0].srcColorBlendFactor, VK_BLEND_FACTOR_ZERO);
    EXPECT_EQ(pcbs_ci.pAttachments[0].dstColorBlendFactor, VK_BLEND_FACTOR_ZERO);
    EXPECT_EQ(pcbs_ci.pAttachments[0].colorBlendOp, VK_BLEND_OP_ADD);
    EXPECT_EQ(pcbs_ci.pAttachments[0].srcAlphaBlendFactor, VK_BLEND_FACTOR_ZERO);
    EXPECT_EQ(pcbs_ci.pAttachments[0].dstAlphaBlendFactor, VK_BLEND_FACTOR_ZERO);
    EXPECT_EQ(pcbs_ci.pAttachments[0].alphaBlendOp, VK_BLEND_OP_ADD);
    EXPECT_EQ(pcbs_ci.pAttachments[0].colorWriteMask,
              VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);
    EXPECT_EQ(pcbs_ci.blendConstants[0], 0);
    EXPECT_EQ(pcbs_ci.blendConstants[1], 0);
    EXPECT_EQ(pcbs_ci.blendConstants[2], 0);
    EXPECT_EQ(pcbs_ci.blendConstants[3], 0);
    EXPECT_EQ(pds_ci.sType, VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO);
    EXPECT_EQ(pds_ci.pNext, nullptr);
    EXPECT_EQ(pds_ci.flags, 0);
    EXPECT_EQ(pds_ci.dynamicStateCount, 3);
    EXPECT_EQ(pds_ci.pDynamicStates[0], VK_DYNAMIC_STATE_VIEWPORT);
    EXPECT_EQ(pds_ci.pDynamicStates[1], VK_DYNAMIC_STATE_SCISSOR);
    EXPECT_EQ(pds_ci.pDynamicStates[2], VK_DYNAMIC_STATE_DISCARD_RECTANGLE_EXT);
    EXPECT_EQ(gp_ci.layout, reinterpret_cast<void*>(8));
    EXPECT_EQ(gp_ci.renderPass, reinterpret_cast<void*>(6));
    EXPECT_EQ(gp_ci.subpass, 0);
    EXPECT_EQ(gp_ci.basePipelineHandle, VK_NULL_HANDLE);
    EXPECT_EQ(gp_ci.basePipelineIndex, 0);
}

TEST_F(Parse, VkComputePipelineCreateInfo) {
    TEST_DESCRIPTION("Tests parsing of a reasonably complex compute pipeline create info JSON");

    VkComputePipelineCreateInfo cp_ci;
    std::string json = {R"({
        "sType" : "VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO",
        "pNext": "NULL",
        "flags" : "0",
        "stage":
        {
            "sType" : "VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO",
            "pNext": {
                "sType": "VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_REQUIRED_SUBGROUP_SIZE_CREATE_INFO",
                "pNext": "NULL",
                "requiredSubgroupSize": 64
            },
            "flags" : "VK_PIPELINE_SHADER_STAGE_CREATE_REQUIRE_FULL_SUBGROUPS_BIT",
            "stage" : "VK_SHADER_STAGE_COMPUTE_BIT",
            "pName" : "main",
            "pSpecializationInfo": {
                "mapEntryCount": 1,
                "pMapEntries": [
                    {
                        "constantID": 0,
                        "offset": 0,
                        "size": 4
                    }
                ],
                "dataSize": 4,
                "pData": [ 0, 1, 2, 3 ]
            },
            "module": ""
        },
        "layout" : 9,
        "basePipelineHandle" : "",
        "basePipelineIndex" : 0
    })"};

    CHECK_PARSE(vpjParseSingleStructJson(this->parser_, json.c_str(), &cp_ci, &msg_));
    const auto& pssrss_ci = *reinterpret_cast<const VkPipelineShaderStageRequiredSubgroupSizeCreateInfo*>(cp_ci.stage.pNext);

    EXPECT_EQ(cp_ci.sType, VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO);
    EXPECT_EQ(cp_ci.pNext, nullptr);
    EXPECT_EQ(cp_ci.flags, 0);
    EXPECT_EQ(cp_ci.stage.sType, VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO);
    EXPECT_NE(cp_ci.stage.pNext, nullptr);
    EXPECT_EQ(pssrss_ci.sType, VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_REQUIRED_SUBGROUP_SIZE_CREATE_INFO);
    EXPECT_EQ(pssrss_ci.pNext, nullptr);
    EXPECT_EQ(pssrss_ci.requiredSubgroupSize, 64);
    EXPECT_EQ(cp_ci.stage.flags, VK_PIPELINE_SHADER_STAGE_CREATE_REQUIRE_FULL_SUBGROUPS_BIT);
    EXPECT_EQ(cp_ci.stage.stage, VK_SHADER_STAGE_COMPUTE_BIT);
    EXPECT_STREQ(cp_ci.stage.pName, "main");
    EXPECT_EQ(cp_ci.stage.pSpecializationInfo->mapEntryCount, 1);
    EXPECT_EQ(cp_ci.stage.pSpecializationInfo->pMapEntries[0].constantID, 0);
    EXPECT_EQ(cp_ci.stage.pSpecializationInfo->pMapEntries[0].offset, 0);
    EXPECT_EQ(cp_ci.stage.pSpecializationInfo->pMapEntries[0].size, 4);
    EXPECT_EQ(cp_ci.stage.pSpecializationInfo->dataSize, 4);
    EXPECT_EQ(reinterpret_cast<const uint8_t*>(cp_ci.stage.pSpecializationInfo->pData)[0], 0);
    EXPECT_EQ(reinterpret_cast<const uint8_t*>(cp_ci.stage.pSpecializationInfo->pData)[1], 1);
    EXPECT_EQ(reinterpret_cast<const uint8_t*>(cp_ci.stage.pSpecializationInfo->pData)[2], 2);
    EXPECT_EQ(reinterpret_cast<const uint8_t*>(cp_ci.stage.pSpecializationInfo->pData)[3], 3);
    EXPECT_EQ(cp_ci.stage.module, VK_NULL_HANDLE);
    EXPECT_EQ(cp_ci.layout, reinterpret_cast<void*>(9));
    EXPECT_EQ(cp_ci.basePipelineHandle, VK_NULL_HANDLE);
    EXPECT_EQ(cp_ci.basePipelineIndex, 0);
}

TEST_F(Parse, VkSamplerYcbcrConversionCreateInfo) {
    TEST_DESCRIPTION("Tests parsing of a reasonably complex ycbcr conversion create info JSON");

#ifdef VK_USE_PLATFORM_SCREEN_QNX
    std::string ycbcr_ci_pnext = R"({
            "sType" : "VK_STRUCTURE_TYPE_EXTERNAL_FORMAT_QNX",
            "pNext": "NULL",
            "externalFormat": 10
    })";
#else
    std::string ycbcr_ci_pnext = R"("NULL")";
#endif  // VK_USE_PLATFORM_SCREEN_QNX

    VkSamplerYcbcrConversionCreateInfo ycbcr_ci;
    std::string json = {R"({
        "sType" : "VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_CREATE_INFO",
        "pNext": )" + ycbcr_ci_pnext +
                        R"(,
        "format": "VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16",
        "ycbcrModel": "VK_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_2020",
        "ycbcrRange": "VK_SAMPLER_YCBCR_RANGE_ITU_NARROW",
        "components": {
            "r": "VK_COMPONENT_SWIZZLE_A",
            "g": "VK_COMPONENT_SWIZZLE_B",
            "b": "VK_COMPONENT_SWIZZLE_G",
            "a": "VK_COMPONENT_SWIZZLE_R",
        },
        "xChromaOffset": "VK_CHROMA_LOCATION_COSITED_EVEN",
        "yChromaOffset": "VK_CHROMA_LOCATION_MIDPOINT",
        "chromaFilter": "VK_FILTER_CUBIC_EXT",
        "forceExplicitReconstruction": "VK_TRUE"
    })"};

    CHECK_PARSE(vpjParseSingleStructJson(this->parser_, json.c_str(), &ycbcr_ci, &msg_));
#ifdef VK_USE_PLATFORM_SCREEN_QNX
    const auto& ef_qnx = *reinterpret_cast<const VkExternalFormatQNX*>(ycbcr_ci.pNext);
#endif  // VK_USE_PLATFORM_SCREEN_QNX

    EXPECT_EQ(ycbcr_ci.sType, VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_CREATE_INFO);
#ifdef VK_USE_PLATFORM_SCREEN_QNX
    EXPECT_EQ(ef_qnx.sType, VK_STRUCTURE_TYPE_EXTERNAL_FORMAT_QNX);
    EXPECT_EQ(ef_qnx.pNext, nullptr);
    EXPECT_EQ(ef_qnx.externalFormat, 10);
#else
    EXPECT_EQ(ycbcr_ci.pNext, nullptr);
#endif  // VK_USE_PLATFORM_SCREEN_QNX
    EXPECT_EQ(ycbcr_ci.format, VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16);
    EXPECT_EQ(ycbcr_ci.ycbcrModel, VK_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_2020);
    EXPECT_EQ(ycbcr_ci.ycbcrRange, VK_SAMPLER_YCBCR_RANGE_ITU_NARROW);
    EXPECT_EQ(ycbcr_ci.components.r, VK_COMPONENT_SWIZZLE_A);
    EXPECT_EQ(ycbcr_ci.components.g, VK_COMPONENT_SWIZZLE_B);
    EXPECT_EQ(ycbcr_ci.components.b, VK_COMPONENT_SWIZZLE_G);
    EXPECT_EQ(ycbcr_ci.components.a, VK_COMPONENT_SWIZZLE_R);
    EXPECT_EQ(ycbcr_ci.xChromaOffset, VK_CHROMA_LOCATION_COSITED_EVEN);
    EXPECT_EQ(ycbcr_ci.yChromaOffset, VK_CHROMA_LOCATION_MIDPOINT);
    EXPECT_EQ(ycbcr_ci.chromaFilter, VK_FILTER_CUBIC_EXT);
    EXPECT_EQ(ycbcr_ci.forceExplicitReconstruction, VK_TRUE);
}

TEST_F(Parse, VkSamplerCreateInfo) {
    TEST_DESCRIPTION("Tests parsing of a reasonably complex sampler create info JSON");

    VkSamplerCreateInfo sampler_ci;
    std::string json = {R"({
        "sType" : "VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO",
        "pNext": {
            "sType" : "VK_STRUCTURE_TYPE_SAMPLER_REDUCTION_MODE_CREATE_INFO",
            "pNext": "NULL",
            "reductionMode": "VK_SAMPLER_REDUCTION_MODE_MAX"
        },
        "flags": 0,
        "magFilter": "VK_FILTER_CUBIC_EXT",
        "minFilter": "VK_FILTER_NEAREST",
        "mipmapMode": "VK_SAMPLER_MIPMAP_MODE_LINEAR",
        "addressModeU": "VK_SAMPLER_ADDRESS_MODE_REPEAT",
        "addressModeV": "VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT",
        "addressModeW": "VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE",
        "mipLodBias": 0.8,
        "anisotropyEnable": "VK_FALSE",
        "maxAnisotropy": 1.8,
        "compareEnable": "VK_FALSE",
        "compareOp": 137,
        "minLod": "NaN",
        "maxLod": "VK_LOD_CLAMP_NONE",
        "borderColor": "VK_BORDER_COLOR_FLOAT_CUSTOM_EXT",
        "unnormalizedCoordinates": "VK_TRUE"
    })"};

    CHECK_PARSE(vpjParseSingleStructJson(this->parser_, json.c_str(), &sampler_ci, &msg_));
    const auto& srm_ci = *reinterpret_cast<const VkSamplerReductionModeCreateInfo*>(sampler_ci.pNext);

    EXPECT_EQ(sampler_ci.sType, VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO);
    EXPECT_EQ(srm_ci.sType, VK_STRUCTURE_TYPE_SAMPLER_REDUCTION_MODE_CREATE_INFO);
    EXPECT_EQ(srm_ci.pNext, nullptr);
    EXPECT_EQ(srm_ci.reductionMode, VK_SAMPLER_REDUCTION_MODE_MAX);
    EXPECT_EQ(sampler_ci.flags, 0);
    EXPECT_EQ(sampler_ci.magFilter, VK_FILTER_CUBIC_EXT);
    EXPECT_EQ(sampler_ci.minFilter, VK_FILTER_NEAREST);
    EXPECT_EQ(sampler_ci.mipmapMode, VK_SAMPLER_MIPMAP_MODE_LINEAR);
    EXPECT_EQ(sampler_ci.addressModeU, VK_SAMPLER_ADDRESS_MODE_REPEAT);
    EXPECT_EQ(sampler_ci.addressModeV, VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT);
    EXPECT_EQ(sampler_ci.addressModeW, VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE);
    EXPECT_EQ(sampler_ci.mipLodBias, 0.8f);
    EXPECT_EQ(sampler_ci.anisotropyEnable, VK_FALSE);
    EXPECT_EQ(sampler_ci.maxAnisotropy, 1.8f);
    EXPECT_EQ(sampler_ci.compareEnable, VK_FALSE);
    EXPECT_EQ(sampler_ci.compareOp, 137);
    EXPECT_TRUE(std::isnan(sampler_ci.minLod));
    EXPECT_EQ(sampler_ci.maxLod, VK_LOD_CLAMP_NONE);
    EXPECT_EQ(sampler_ci.borderColor, VK_BORDER_COLOR_FLOAT_CUSTOM_EXT);
    EXPECT_EQ(sampler_ci.unnormalizedCoordinates, VK_TRUE);
}

TEST_F(Parse, VkDescriptorSetLayoutCreateInfo) {
    TEST_DESCRIPTION("Tests parsing of a reasonably complex descriptor set layout create info JSON");

    VkDescriptorSetLayoutCreateInfo dsl_ci;
    std::string json = {R"({
        "sType" : "VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO",
        "pNext": {
            "sType" : "VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO",
            "pNext": "NULL",
            "bindingCount": 1,
            "pBindingFlags": [
                "VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT | VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT"
            ]
        },
        "flags": "VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT",
        "bindingCount": 1,
        "pBindings": [
            {
                "binding": 12345,
                "descriptorType": "VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER",
                "descriptorCount": 1,
                "stageFlags": "VK_SHADER_STAGE_ALL",
                "pImmutableSamplers": "NULL"
            }
        ]
    })"};

    CHECK_PARSE(vpjParseSingleStructJson(this->parser_, json.c_str(), &dsl_ci, &msg_));
    const auto& dslbf_ci = *reinterpret_cast<const VkDescriptorSetLayoutBindingFlagsCreateInfo*>(dsl_ci.pNext);

    EXPECT_EQ(dsl_ci.sType, VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO);
    EXPECT_EQ(dslbf_ci.sType, VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO);
    EXPECT_EQ(dslbf_ci.pNext, nullptr);
    EXPECT_EQ(dslbf_ci.bindingCount, 1);
    EXPECT_EQ(dslbf_ci.pBindingFlags[0], VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT | VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT);
    EXPECT_EQ(dsl_ci.flags, VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT);
    EXPECT_EQ(dsl_ci.bindingCount, 1);
    EXPECT_EQ(dsl_ci.pBindings[0].binding, 12345);
    EXPECT_EQ(dsl_ci.pBindings[0].descriptorType, VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER);
    EXPECT_EQ(dsl_ci.pBindings[0].descriptorCount, 1);
    EXPECT_EQ(dsl_ci.pBindings[0].stageFlags, VK_SHADER_STAGE_ALL);
    EXPECT_EQ(dsl_ci.pBindings[0].pImmutableSamplers, nullptr);
}

TEST_F(Parse, VkPipelineLayoutCreateInfo) {
    TEST_DESCRIPTION("Tests parsing of a reasonably complex pipeline layout create info JSON");

    VkPipelineLayoutCreateInfo pl_ci;
    std::string json = {R"({
        "sType" : "VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO",
        "pNext": "NULL",
        "flags": 0,
        "setLayoutCount": 1,
        "pSetLayouts": [
            54321
        ],
        "pushConstantRangeCount": 1,
        "pPushConstantRanges": [
            {
                "stageFlags": "VK_SHADER_STAGE_COMPUTE_BIT | VK_SHADER_STAGE_ALL_GRAPHICS",
                "offset": 0,
                "size": 8
            }
        ]
    })"};

    CHECK_PARSE(vpjParseSingleStructJson(this->parser_, json.c_str(), &pl_ci, &msg_));

    EXPECT_EQ(pl_ci.sType, VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO);
    EXPECT_EQ(pl_ci.pNext, nullptr);
    EXPECT_EQ(pl_ci.flags, 0);
    EXPECT_EQ(pl_ci.setLayoutCount, 1);
    EXPECT_EQ((size_t)pl_ci.pSetLayouts[0], 54321);
    EXPECT_EQ(pl_ci.pushConstantRangeCount, 1);
    EXPECT_EQ(pl_ci.pPushConstantRanges[0].stageFlags, VK_SHADER_STAGE_COMPUTE_BIT | VK_SHADER_STAGE_ALL_GRAPHICS);
    EXPECT_EQ(pl_ci.pPushConstantRanges[0].offset, 0);
    EXPECT_EQ(pl_ci.pPushConstantRanges[0].size, 8);
}

TEST_F(Parse, VkRenderPassCreateInfo) {
    TEST_DESCRIPTION("Tests parsing of a reasonably complex render pass create info JSON");

    VkRenderPassCreateInfo rp_ci;
    std::string json = {R"({
        "sType" : "VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO",
        "pNext": {
            "sType" : "VK_STRUCTURE_TYPE_RENDER_PASS_INPUT_ATTACHMENT_ASPECT_CREATE_INFO",
            "pNext": {
                "sType" : "VK_STRUCTURE_TYPE_RENDER_PASS_MULTIVIEW_CREATE_INFO",
                "pNext": "NULL",
                "subpassCount": 1,
                "pViewMasks": [ 1 ],
                "dependencyCount": 2,
                "pViewOffsets": [ 0, 1 ],
                "correlationMaskCount": 1,
                "pCorrelationMasks": [ 8 ]
            },
            "aspectReferenceCount": 1,
            "pAspectReferences": [
                {
                    "subpass": 1,
                    "inputAttachmentIndex": 2,
                    "aspectMask": "VK_IMAGE_ASPECT_COLOR_BIT"
                }
            ]
        },
        "flags": 0,
        "attachmentCount": 1,
        "pAttachments": [
            {
                "flags": "VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT",
                "format": "VK_FORMAT_R8G8_USCALED",
                "samples": "VK_SAMPLE_COUNT_8_BIT",
                "loadOp": "VK_ATTACHMENT_LOAD_OP_DONT_CARE",
                "storeOp": "VK_ATTACHMENT_STORE_OP_STORE",
                "stencilLoadOp": "VK_ATTACHMENT_LOAD_OP_LOAD",
                "stencilStoreOp": "VK_ATTACHMENT_STORE_OP_DONT_CARE",
                "initialLayout": "VK_IMAGE_LAYOUT_GENERAL",
                "finalLayout": "VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR"
            }
        ],
        "subpassCount": 2,
        "pSubpasses": [
            {
                "flags": 0,
                "pipelineBindPoint": "VK_PIPELINE_BIND_POINT_GRAPHICS",
                "inputAttachmentCount": 1,
                "pInputAttachments": [
                    {
                        "attachment": 567,
                        "layout": "VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL"
                    }
                ],
                "colorAttachmentCount": 1,
                "pColorAttachments": [
                    {
                        "attachment": "VK_ATTACHMENT_UNUSED",
                        "layout": "VK_IMAGE_LAYOUT_UNDEFINED"
                    }
                ],
                "pResolveAttachments": [
                    {
                        "attachment": "VK_ATTACHMENT_UNUSED",
                        "layout": "VK_IMAGE_LAYOUT_UNDEFINED"
                    }
                ],
                "pDepthStencilAttachment": {
                    "attachment": "VK_ATTACHMENT_UNUSED",
                    "layout": "VK_IMAGE_LAYOUT_UNDEFINED"
                },
                "preserveAttachmentCount": 0,
                "pPreserveAttachments": "NULL"
            },
            {
                "flags": 0,
                "pipelineBindPoint": "VK_PIPELINE_BIND_POINT_GRAPHICS",
                "inputAttachmentCount": 0,
                "pInputAttachments": "NULL",
                "colorAttachmentCount": 1,
                "pColorAttachments": [
                    {
                        "attachment": 567,
                        "layout": "VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL_KHR"
                    }
                ],
                "pResolveAttachments": [
                    {
                        "attachment": "VK_ATTACHMENT_UNUSED",
                        "layout": "VK_IMAGE_LAYOUT_UNDEFINED"
                    }
                ],
                "pDepthStencilAttachment": {
                    "attachment": "VK_ATTACHMENT_UNUSED",
                    "layout": "VK_IMAGE_LAYOUT_UNDEFINED"
                },
                "preserveAttachmentCount": 0,
                "pPreserveAttachments": "NULL"
            }
        ],
        "dependencyCount": 1,
        "pDependencies": [
            {
                "srcSubpass": "VK_SUBPASS_EXTERNAL",
                "dstSubpass": 2345,
                "srcStageMask": "VK_PIPELINE_STAGE_NONE_KHR",
                "dstStageMask": "VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT",
                "srcAccessMask": "VK_ACCESS_NONE_KHR",
                "dstAccessMask": "VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT",
                "dependencyFlags": "VK_DEPENDENCY_DEVICE_GROUP_BIT"
            }
        ]
    })"};

    CHECK_PARSE(vpjParseSingleStructJson(this->parser_, json.c_str(), &rp_ci, &msg_));
    const auto& rpiaa_ci = *reinterpret_cast<const VkRenderPassInputAttachmentAspectCreateInfo*>(rp_ci.pNext);
    const auto& rpmv_ci = *reinterpret_cast<const VkRenderPassMultiviewCreateInfo*>(rpiaa_ci.pNext);

    EXPECT_EQ(rp_ci.sType, VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO);
    EXPECT_EQ(rpiaa_ci.sType, VK_STRUCTURE_TYPE_RENDER_PASS_INPUT_ATTACHMENT_ASPECT_CREATE_INFO);
    EXPECT_EQ(rpiaa_ci.aspectReferenceCount, 1);
    EXPECT_EQ(rpiaa_ci.pAspectReferences[0].subpass, 1);
    EXPECT_EQ(rpiaa_ci.pAspectReferences[0].inputAttachmentIndex, 2);
    EXPECT_EQ(rpiaa_ci.pAspectReferences[0].aspectMask, VK_IMAGE_ASPECT_COLOR_BIT);
    EXPECT_EQ(rpmv_ci.sType, VK_STRUCTURE_TYPE_RENDER_PASS_MULTIVIEW_CREATE_INFO);
    EXPECT_EQ(rpmv_ci.pNext, nullptr);
    EXPECT_EQ(rpmv_ci.subpassCount, 1);
    EXPECT_EQ(rpmv_ci.pViewMasks[0], 1);
    EXPECT_EQ(rpmv_ci.dependencyCount, 2);
    EXPECT_EQ(rpmv_ci.pViewOffsets[0], 0);
    EXPECT_EQ(rpmv_ci.pViewOffsets[1], 1);
    EXPECT_EQ(rpmv_ci.correlationMaskCount, 1);
    EXPECT_EQ(rpmv_ci.pCorrelationMasks[0], 8);
    EXPECT_EQ(rp_ci.flags, 0);
    EXPECT_EQ(rp_ci.attachmentCount, 1);
    EXPECT_EQ(rp_ci.pAttachments[0].flags, VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT);
    EXPECT_EQ(rp_ci.pAttachments[0].format, VK_FORMAT_R8G8_USCALED);
    EXPECT_EQ(rp_ci.pAttachments[0].samples, VK_SAMPLE_COUNT_8_BIT);
    EXPECT_EQ(rp_ci.pAttachments[0].loadOp, VK_ATTACHMENT_LOAD_OP_DONT_CARE);
    EXPECT_EQ(rp_ci.pAttachments[0].storeOp, VK_ATTACHMENT_STORE_OP_STORE);
    EXPECT_EQ(rp_ci.pAttachments[0].stencilLoadOp, VK_ATTACHMENT_LOAD_OP_LOAD);
    EXPECT_EQ(rp_ci.pAttachments[0].stencilStoreOp, VK_ATTACHMENT_STORE_OP_DONT_CARE);
    EXPECT_EQ(rp_ci.pAttachments[0].initialLayout, VK_IMAGE_LAYOUT_GENERAL);
    EXPECT_EQ(rp_ci.pAttachments[0].finalLayout, VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR);
    EXPECT_EQ(rp_ci.subpassCount, 2);
    EXPECT_EQ(rp_ci.pSubpasses[0].flags, 0);
    EXPECT_EQ(rp_ci.pSubpasses[0].pipelineBindPoint, VK_PIPELINE_BIND_POINT_GRAPHICS);
    EXPECT_EQ(rp_ci.pSubpasses[0].inputAttachmentCount, 1);
    EXPECT_EQ(rp_ci.pSubpasses[0].pInputAttachments[0].attachment, 567);
    EXPECT_EQ(rp_ci.pSubpasses[0].pInputAttachments[0].layout, VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL);
    EXPECT_EQ(rp_ci.pSubpasses[0].colorAttachmentCount, 1);
    EXPECT_EQ(rp_ci.pSubpasses[0].pColorAttachments[0].attachment, VK_ATTACHMENT_UNUSED);
    EXPECT_EQ(rp_ci.pSubpasses[0].pColorAttachments[0].layout, VK_IMAGE_LAYOUT_UNDEFINED);
    EXPECT_EQ(rp_ci.pSubpasses[0].pResolveAttachments[0].attachment, VK_ATTACHMENT_UNUSED);
    EXPECT_EQ(rp_ci.pSubpasses[0].pResolveAttachments[0].layout, VK_IMAGE_LAYOUT_UNDEFINED);
    EXPECT_EQ(rp_ci.pSubpasses[0].pDepthStencilAttachment[0].attachment, VK_ATTACHMENT_UNUSED);
    EXPECT_EQ(rp_ci.pSubpasses[0].pDepthStencilAttachment[0].layout, VK_IMAGE_LAYOUT_UNDEFINED);
    EXPECT_EQ(rp_ci.pSubpasses[0].preserveAttachmentCount, 0);
    EXPECT_EQ(rp_ci.pSubpasses[0].pPreserveAttachments, nullptr);
    EXPECT_EQ(rp_ci.pSubpasses[1].flags, 0);
    EXPECT_EQ(rp_ci.pSubpasses[1].pipelineBindPoint, VK_PIPELINE_BIND_POINT_GRAPHICS);
    EXPECT_EQ(rp_ci.pSubpasses[1].inputAttachmentCount, 0);
    EXPECT_EQ(rp_ci.pSubpasses[1].pInputAttachments, nullptr);
    EXPECT_EQ(rp_ci.pSubpasses[1].colorAttachmentCount, 1);
    EXPECT_EQ(rp_ci.pSubpasses[1].pColorAttachments[0].attachment, 567);
    EXPECT_EQ(rp_ci.pSubpasses[1].pColorAttachments[0].layout, VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL_KHR);
    EXPECT_EQ(rp_ci.pSubpasses[1].pResolveAttachments[0].attachment, VK_ATTACHMENT_UNUSED);
    EXPECT_EQ(rp_ci.pSubpasses[1].pResolveAttachments[0].layout, VK_IMAGE_LAYOUT_UNDEFINED);
    EXPECT_EQ(rp_ci.pSubpasses[1].pDepthStencilAttachment[0].attachment, VK_ATTACHMENT_UNUSED);
    EXPECT_EQ(rp_ci.pSubpasses[1].pDepthStencilAttachment[0].layout, VK_IMAGE_LAYOUT_UNDEFINED);
    EXPECT_EQ(rp_ci.pSubpasses[1].preserveAttachmentCount, 0);
    EXPECT_EQ(rp_ci.pSubpasses[1].pPreserveAttachments, nullptr);
    EXPECT_EQ(rp_ci.dependencyCount, 1);
    EXPECT_EQ(rp_ci.pDependencies[0].srcSubpass, VK_SUBPASS_EXTERNAL);
    EXPECT_EQ(rp_ci.pDependencies[0].dstSubpass, 2345);
    EXPECT_EQ(rp_ci.pDependencies[0].srcStageMask, VK_PIPELINE_STAGE_NONE_KHR);
    EXPECT_EQ(rp_ci.pDependencies[0].dstStageMask, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
    EXPECT_EQ(rp_ci.pDependencies[0].srcAccessMask, VK_ACCESS_NONE_KHR);
    EXPECT_EQ(rp_ci.pDependencies[0].dstAccessMask, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
    EXPECT_EQ(rp_ci.pDependencies[0].dependencyFlags, VK_DEPENDENCY_DEVICE_GROUP_BIT);
}

TEST_F(Parse, VkRenderPassCreateInfo2) {
    TEST_DESCRIPTION("Tests parsing of a reasonably complex render pass 2 layout create info JSON");

    VkRenderPassCreateInfo2 rp2_ci;
    std::string json = {R"({
        "sType" : "VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2",
        "pNext": "NULL",
        "flags": 0,
        "attachmentCount": 1,
        "pAttachments": [
            {
                "sType" : "VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2",
                "pNext": {
                    "sType": "VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_STENCIL_LAYOUT",
                    "pNext": "NULL",
                    "stencilInitialLayout": "VK_IMAGE_LAYOUT_GENERAL",
                    "stencilFinalLayout": "VK_IMAGE_LAYOUT_GENERAL"
                },
                "flags": "VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT",
                "format": "VK_FORMAT_R8G8_USCALED",
                "samples": "VK_SAMPLE_COUNT_8_BIT",
                "loadOp": "VK_ATTACHMENT_LOAD_OP_DONT_CARE",
                "storeOp": "VK_ATTACHMENT_STORE_OP_STORE",
                "stencilLoadOp": "VK_ATTACHMENT_LOAD_OP_LOAD",
                "stencilStoreOp": "VK_ATTACHMENT_STORE_OP_DONT_CARE",
                "initialLayout": "VK_IMAGE_LAYOUT_GENERAL",
                "finalLayout": "VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR"
            }
        ],
        "subpassCount": 2,
        "pSubpasses": [
            {
                "sType" : "VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2",
                "pNext": {
                    "sType" : "VK_STRUCTURE_TYPE_FRAGMENT_SHADING_RATE_ATTACHMENT_INFO_KHR",
                    "pNext": {
                        "sType" : "VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_DEPTH_STENCIL_RESOLVE",
                        "pNext": "NULL",
                        "depthResolveMode": "VK_RESOLVE_MODE_MIN_BIT",
                        "stencilResolveMode": "VK_RESOLVE_MODE_MAX_BIT",
                        "pDepthStencilResolveAttachment": {
                            "sType": "VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2",
                            "pNext": "NULL",
                            "attachment": 8,
                            "layout": "VK_IMAGE_LAYOUT_GENERAL",
                            "aspectMask": "VK_IMAGE_ASPECT_COLOR_BIT"
                        }
                    },
                    "pFragmentShadingRateAttachment": {
                        "sType": "VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2",
                        "pNext": "NULL",
                        "attachment": 5,
                        "layout": "VK_IMAGE_LAYOUT_GENERAL",
                        "aspectMask": "VK_IMAGE_ASPECT_COLOR_BIT"
                    },
                    "shadingRateAttachmentTexelSize": {
                        "width": 4,
                        "height": 4
                    }
                },
                "flags": 0,
                "pipelineBindPoint": "VK_PIPELINE_BIND_POINT_GRAPHICS",
                "viewMask": 0,
                "inputAttachmentCount": 1,
                "pInputAttachments": [
                    {
                        "sType": "VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2",
                        "pNext": "NULL",
                        "attachment": 567,
                        "layout": "VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL",
                        "aspectMask": "VK_IMAGE_ASPECT_COLOR_BIT"
                    }
                ],
                "colorAttachmentCount": 1,
                "pColorAttachments": [
                    {
                        "sType": "VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2",
                        "pNext": "NULL",
                        "attachment": "VK_ATTACHMENT_UNUSED",
                        "layout": "VK_IMAGE_LAYOUT_UNDEFINED",
                        "aspectMask": "VK_IMAGE_ASPECT_COLOR_BIT"
                    }
                ],
                "pResolveAttachments": [
                    {
                        "sType": "VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2",
                        "pNext": "NULL",
                        "attachment": "VK_ATTACHMENT_UNUSED",
                        "layout": "VK_IMAGE_LAYOUT_UNDEFINED",
                        "aspectMask": "VK_IMAGE_ASPECT_COLOR_BIT"
                    }
                ],
                "pDepthStencilAttachment": {
                    "sType": "VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2",
                    "pNext": "NULL",
                    "attachment": "VK_ATTACHMENT_UNUSED",
                    "layout": "VK_IMAGE_LAYOUT_UNDEFINED",
                    "aspectMask": "VK_IMAGE_ASPECT_DEPTH_BIT"
                },
                "preserveAttachmentCount": 0,
                "pPreserveAttachments": "NULL"
            },
            {
                "sType" : "VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2",
                "pNext": "NULL",
                "flags": 0,
                "pipelineBindPoint": "VK_PIPELINE_BIND_POINT_GRAPHICS",
                "viewMask": 0,
                "inputAttachmentCount": 0,
                "pInputAttachments": "NULL",
                "colorAttachmentCount": 1,
                "pColorAttachments": [
                    {
                        "sType": "VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2",
                        "pNext": "NULL",
                        "attachment": 567,
                        "layout": "VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL_KHR",
                        "aspectMask": "VK_IMAGE_ASPECT_COLOR_BIT"
                    }
                ],
                "pResolveAttachments": [
                    {
                        "sType": "VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2",
                        "pNext": "NULL",
                        "attachment": "VK_ATTACHMENT_UNUSED",
                        "layout": "VK_IMAGE_LAYOUT_UNDEFINED",
                        "aspectMask": "VK_IMAGE_ASPECT_COLOR_BIT"
                    }
                ],
                "pDepthStencilAttachment": {
                    "sType": "VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2",
                    "pNext": "NULL",
                    "attachment": "VK_ATTACHMENT_UNUSED",
                    "layout": "VK_IMAGE_LAYOUT_UNDEFINED",
                    "aspectMask": "VK_IMAGE_ASPECT_DEPTH_BIT"
                },
                "preserveAttachmentCount": 0,
                "pPreserveAttachments": "NULL"
            }
        ],
        "dependencyCount": 1,
        "pDependencies": [
            {
                "sType" : "VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2",
                "pNext": {
                    "sType" : "VK_STRUCTURE_TYPE_MEMORY_BARRIER_2_KHR",
                    "pNext": "NULL",
                    "srcStageMask": "VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT",
                    "srcAccessMask": "VK_ACCESS_2_COLOR_ATTACHMENT_READ_NONCOHERENT_BIT_EXT",
                    "dstStageMask": "VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT",
                    "dstAccessMask": "VK_ACCESS_2_COMMAND_PREPROCESS_READ_BIT_NV"
                },
                "srcSubpass": "VK_SUBPASS_EXTERNAL",
                "dstSubpass": 2345,
                "srcStageMask": "VK_PIPELINE_STAGE_NONE_KHR",
                "dstStageMask": "VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT",
                "srcAccessMask": "VK_ACCESS_NONE_KHR",
                "dstAccessMask": "VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT",
                "dependencyFlags": "VK_DEPENDENCY_DEVICE_GROUP_BIT",
                "viewOffset": 0
            }
        ],
        "correlatedViewMaskCount": 1,
        "pCorrelatedViewMasks": [ 8 ]
    })"};

    CHECK_PARSE(vpjParseSingleStructJson(this->parser_, json.c_str(), &rp2_ci, &msg_));
    const auto& adsl = *reinterpret_cast<const VkAttachmentDescriptionStencilLayout*>(rp2_ci.pAttachments[0].pNext);
    const auto& fsrai = *reinterpret_cast<const VkFragmentShadingRateAttachmentInfoKHR*>(rp2_ci.pSubpasses[0].pNext);
    const auto& sddsr = *reinterpret_cast<const VkSubpassDescriptionDepthStencilResolve*>(fsrai.pNext);
    const auto& mb2 = *reinterpret_cast<const VkMemoryBarrier2KHR*>(rp2_ci.pDependencies[0].pNext);

    EXPECT_EQ(rp2_ci.sType, VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2);
    EXPECT_EQ(rp2_ci.pNext, nullptr);
    EXPECT_EQ(rp2_ci.flags, 0);
    EXPECT_EQ(rp2_ci.attachmentCount, 1);
    EXPECT_EQ(rp2_ci.pAttachments[0].sType, VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2);
    EXPECT_EQ(adsl.sType, VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_STENCIL_LAYOUT);
    EXPECT_EQ(adsl.pNext, nullptr);
    EXPECT_EQ(adsl.stencilInitialLayout, VK_IMAGE_LAYOUT_GENERAL);
    EXPECT_EQ(adsl.stencilFinalLayout, VK_IMAGE_LAYOUT_GENERAL);
    EXPECT_EQ(rp2_ci.pAttachments[0].flags, VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT);
    EXPECT_EQ(rp2_ci.pAttachments[0].format, VK_FORMAT_R8G8_USCALED);
    EXPECT_EQ(rp2_ci.pAttachments[0].samples, VK_SAMPLE_COUNT_8_BIT);
    EXPECT_EQ(rp2_ci.pAttachments[0].loadOp, VK_ATTACHMENT_LOAD_OP_DONT_CARE);
    EXPECT_EQ(rp2_ci.pAttachments[0].storeOp, VK_ATTACHMENT_STORE_OP_STORE);
    EXPECT_EQ(rp2_ci.pAttachments[0].stencilLoadOp, VK_ATTACHMENT_LOAD_OP_LOAD);
    EXPECT_EQ(rp2_ci.pAttachments[0].stencilStoreOp, VK_ATTACHMENT_STORE_OP_DONT_CARE);
    EXPECT_EQ(rp2_ci.pAttachments[0].initialLayout, VK_IMAGE_LAYOUT_GENERAL);
    EXPECT_EQ(rp2_ci.pAttachments[0].finalLayout, VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR);
    EXPECT_EQ(rp2_ci.subpassCount, 2);
    EXPECT_EQ(rp2_ci.pSubpasses[0].sType, VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2);
    EXPECT_EQ(fsrai.sType, VK_STRUCTURE_TYPE_FRAGMENT_SHADING_RATE_ATTACHMENT_INFO_KHR);
    EXPECT_EQ(sddsr.sType, VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_DEPTH_STENCIL_RESOLVE);
    EXPECT_EQ(sddsr.pNext, nullptr);
    EXPECT_EQ(sddsr.depthResolveMode, VK_RESOLVE_MODE_MIN_BIT);
    EXPECT_EQ(sddsr.stencilResolveMode, VK_RESOLVE_MODE_MAX_BIT);
    EXPECT_EQ(sddsr.pDepthStencilResolveAttachment->sType, VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2);
    EXPECT_EQ(sddsr.pDepthStencilResolveAttachment->pNext, nullptr);
    EXPECT_EQ(sddsr.pDepthStencilResolveAttachment->attachment, 8);
    EXPECT_EQ(sddsr.pDepthStencilResolveAttachment->layout, VK_IMAGE_LAYOUT_GENERAL);
    EXPECT_EQ(sddsr.pDepthStencilResolveAttachment->aspectMask, VK_IMAGE_ASPECT_COLOR_BIT);
    EXPECT_EQ(fsrai.pFragmentShadingRateAttachment->sType, VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2);
    EXPECT_EQ(fsrai.pFragmentShadingRateAttachment->pNext, nullptr);
    EXPECT_EQ(fsrai.pFragmentShadingRateAttachment->attachment, 5);
    EXPECT_EQ(fsrai.pFragmentShadingRateAttachment->layout, VK_IMAGE_LAYOUT_GENERAL);
    EXPECT_EQ(fsrai.pFragmentShadingRateAttachment->aspectMask, VK_IMAGE_ASPECT_COLOR_BIT);
    EXPECT_EQ(fsrai.shadingRateAttachmentTexelSize.width, 4);
    EXPECT_EQ(fsrai.shadingRateAttachmentTexelSize.height, 4);
    EXPECT_EQ(rp2_ci.pSubpasses[0].flags, 0);
    EXPECT_EQ(rp2_ci.pSubpasses[0].pipelineBindPoint, VK_PIPELINE_BIND_POINT_GRAPHICS);
    EXPECT_EQ(rp2_ci.pSubpasses[0].inputAttachmentCount, 1);
    EXPECT_EQ(rp2_ci.pSubpasses[0].pInputAttachments[0].attachment, 567);
    EXPECT_EQ(rp2_ci.pSubpasses[0].pInputAttachments[0].layout, VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL);
    EXPECT_EQ(rp2_ci.pSubpasses[0].colorAttachmentCount, 1);
    EXPECT_EQ(rp2_ci.pSubpasses[0].pColorAttachments[0].attachment, VK_ATTACHMENT_UNUSED);
    EXPECT_EQ(rp2_ci.pSubpasses[0].pColorAttachments[0].layout, VK_IMAGE_LAYOUT_UNDEFINED);
    EXPECT_EQ(rp2_ci.pSubpasses[0].pResolveAttachments[0].attachment, VK_ATTACHMENT_UNUSED);
    EXPECT_EQ(rp2_ci.pSubpasses[0].pResolveAttachments[0].layout, VK_IMAGE_LAYOUT_UNDEFINED);
    EXPECT_EQ(rp2_ci.pSubpasses[0].pDepthStencilAttachment[0].attachment, VK_ATTACHMENT_UNUSED);
    EXPECT_EQ(rp2_ci.pSubpasses[0].pDepthStencilAttachment[0].layout, VK_IMAGE_LAYOUT_UNDEFINED);
    EXPECT_EQ(rp2_ci.pSubpasses[0].preserveAttachmentCount, 0);
    EXPECT_EQ(rp2_ci.pSubpasses[0].pPreserveAttachments, nullptr);
    EXPECT_EQ(rp2_ci.pSubpasses[1].sType, VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2);
    EXPECT_EQ(rp2_ci.pSubpasses[1].pNext, nullptr);
    EXPECT_EQ(rp2_ci.pSubpasses[1].flags, 0);
    EXPECT_EQ(rp2_ci.pSubpasses[1].pipelineBindPoint, VK_PIPELINE_BIND_POINT_GRAPHICS);
    EXPECT_EQ(rp2_ci.pSubpasses[1].inputAttachmentCount, 0);
    EXPECT_EQ(rp2_ci.pSubpasses[1].pInputAttachments, nullptr);
    EXPECT_EQ(rp2_ci.pSubpasses[1].colorAttachmentCount, 1);
    EXPECT_EQ(rp2_ci.pSubpasses[1].pColorAttachments[0].attachment, 567);
    EXPECT_EQ(rp2_ci.pSubpasses[1].pColorAttachments[0].layout, VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL_KHR);
    EXPECT_EQ(rp2_ci.pSubpasses[1].pResolveAttachments[0].attachment, VK_ATTACHMENT_UNUSED);
    EXPECT_EQ(rp2_ci.pSubpasses[1].pResolveAttachments[0].layout, VK_IMAGE_LAYOUT_UNDEFINED);
    EXPECT_EQ(rp2_ci.pSubpasses[1].pDepthStencilAttachment[0].attachment, VK_ATTACHMENT_UNUSED);
    EXPECT_EQ(rp2_ci.pSubpasses[1].pDepthStencilAttachment[0].layout, VK_IMAGE_LAYOUT_UNDEFINED);
    EXPECT_EQ(rp2_ci.pSubpasses[1].preserveAttachmentCount, 0);
    EXPECT_EQ(rp2_ci.pSubpasses[1].pPreserveAttachments, nullptr);
    EXPECT_EQ(rp2_ci.dependencyCount, 1);
    EXPECT_EQ(rp2_ci.pDependencies[0].sType, VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2);
    EXPECT_EQ(mb2.sType, VK_STRUCTURE_TYPE_MEMORY_BARRIER_2_KHR);
    EXPECT_EQ(mb2.pNext, nullptr);
    EXPECT_EQ(mb2.srcStageMask, VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT);
    EXPECT_EQ(mb2.srcAccessMask, VK_ACCESS_2_COLOR_ATTACHMENT_READ_NONCOHERENT_BIT_EXT);
    EXPECT_EQ(mb2.dstStageMask, VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT);
    EXPECT_EQ(mb2.dstAccessMask, VK_ACCESS_2_COMMAND_PREPROCESS_READ_BIT_NV);
    EXPECT_EQ(rp2_ci.pDependencies[0].srcSubpass, VK_SUBPASS_EXTERNAL);
    EXPECT_EQ(rp2_ci.pDependencies[0].dstSubpass, 2345);
    EXPECT_EQ(rp2_ci.pDependencies[0].srcStageMask, VK_PIPELINE_STAGE_NONE_KHR);
    EXPECT_EQ(rp2_ci.pDependencies[0].dstStageMask, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
    EXPECT_EQ(rp2_ci.pDependencies[0].srcAccessMask, VK_ACCESS_NONE_KHR);
    EXPECT_EQ(rp2_ci.pDependencies[0].dstAccessMask, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
    EXPECT_EQ(rp2_ci.pDependencies[0].dependencyFlags, VK_DEPENDENCY_DEVICE_GROUP_BIT);
    EXPECT_EQ(rp2_ci.correlatedViewMaskCount, 1);
    EXPECT_EQ(rp2_ci.pCorrelatedViewMasks[0], 8);
}

TEST_F(Parse, VkShaderModuleCreateInfo) {
    TEST_DESCRIPTION("Tests parsing of a reasonably complex shader module create info JSON");

    VkShaderModuleCreateInfo sm_ci;
    std::string json = {R"({
        "sType" : "VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO",
        "pNext": "NULL",
        "flags": 0,
        "codeSize": 4,
        "pCode": [25, 123, 42, 8]
    })"};

    CHECK_PARSE(vpjParseSingleStructJson(this->parser_, json.c_str(), &sm_ci, &msg_));

    EXPECT_EQ(sm_ci.sType, VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO);
    EXPECT_EQ(sm_ci.pNext, nullptr);
    EXPECT_EQ(sm_ci.flags, 0);
    EXPECT_EQ(sm_ci.codeSize, 4);
    EXPECT_EQ(sm_ci.pCode[0], (25 << 0) + (123 << 8) + (42 << 16) + (8 << 24));
}

TEST_F(Parse, VkDeviceObjectReservationCreateInfo) {
    TEST_DESCRIPTION("Tests parsing of a reasonably complex object reservation create info JSON");

    VkDeviceObjectReservationCreateInfo dor_ci;
    std::string json = {R"({
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

    CHECK_PARSE(vpjParseSingleStructJson(this->parser_, json.c_str(), &dor_ci, &msg_));

    EXPECT_EQ(dor_ci.sType, VK_STRUCTURE_TYPE_DEVICE_OBJECT_RESERVATION_CREATE_INFO);
    EXPECT_EQ(dor_ci.pNext, nullptr);
    EXPECT_EQ(dor_ci.pipelineCacheCreateInfoCount, 1);
    EXPECT_EQ(dor_ci.pPipelineCacheCreateInfos[0].sType, VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO);
    EXPECT_EQ(dor_ci.pPipelineCacheCreateInfos[0].pNext, nullptr);
    EXPECT_EQ(dor_ci.pPipelineCacheCreateInfos[0].flags,
              VK_PIPELINE_CACHE_CREATE_READ_ONLY_BIT | VK_PIPELINE_CACHE_CREATE_USE_APPLICATION_STORAGE_BIT);
    EXPECT_EQ(dor_ci.pPipelineCacheCreateInfos[0].initialDataSize, 3);
    EXPECT_EQ(static_cast<const unsigned char*>(dor_ci.pPipelineCacheCreateInfos[0].pInitialData)[0], 0b01101001);
    EXPECT_EQ(static_cast<const unsigned char*>(dor_ci.pPipelineCacheCreateInfos[0].pInitialData)[1], 0b10110111);
    EXPECT_EQ(static_cast<const unsigned char*>(dor_ci.pPipelineCacheCreateInfos[0].pInitialData)[2], 0b00011101);
    EXPECT_EQ(dor_ci.pipelinePoolSizeCount, 1);
    EXPECT_EQ(dor_ci.pPipelinePoolSizes[0].sType, VK_STRUCTURE_TYPE_PIPELINE_POOL_SIZE);
    EXPECT_EQ(dor_ci.pPipelinePoolSizes[0].pNext, nullptr);
    EXPECT_EQ(dor_ci.pPipelinePoolSizes[0].poolEntryCount, 1);
    EXPECT_EQ(dor_ci.pPipelinePoolSizes[0].poolEntrySize, 1048576);
    EXPECT_EQ(dor_ci.semaphoreRequestCount, 0);
    EXPECT_EQ(dor_ci.commandBufferRequestCount, 1);
    EXPECT_EQ(dor_ci.fenceRequestCount, 1);
    EXPECT_EQ(dor_ci.deviceMemoryRequestCount, 2);
    EXPECT_EQ(dor_ci.bufferRequestCount, 2);
    EXPECT_EQ(dor_ci.imageRequestCount, 0);
    EXPECT_EQ(dor_ci.eventRequestCount, 0);
    EXPECT_EQ(dor_ci.queryPoolRequestCount, 0);
    EXPECT_EQ(dor_ci.bufferViewRequestCount, 0);
    EXPECT_EQ(dor_ci.imageViewRequestCount, 0);
    EXPECT_EQ(dor_ci.layeredImageViewRequestCount, 0);
    EXPECT_EQ(dor_ci.pipelineCacheRequestCount, 1);
    EXPECT_EQ(dor_ci.pipelineLayoutRequestCount, 1);
    EXPECT_EQ(dor_ci.renderPassRequestCount, 1);
    EXPECT_EQ(dor_ci.graphicsPipelineRequestCount, 0);
    EXPECT_EQ(dor_ci.computePipelineRequestCount, 1);
    EXPECT_EQ(dor_ci.descriptorSetLayoutRequestCount, 1);
    EXPECT_EQ(dor_ci.samplerRequestCount, 0);
    EXPECT_EQ(dor_ci.descriptorPoolRequestCount, 1);
    EXPECT_EQ(dor_ci.descriptorSetRequestCount, 1);
    EXPECT_EQ(dor_ci.framebufferRequestCount, 0);
    EXPECT_EQ(dor_ci.commandPoolRequestCount, 2);
    EXPECT_EQ(dor_ci.samplerYcbcrConversionRequestCount, 0);
    EXPECT_EQ(dor_ci.surfaceRequestCount, 0);
    EXPECT_EQ(dor_ci.swapchainRequestCount, 1);
    EXPECT_EQ(dor_ci.displayModeRequestCount, 0);
    EXPECT_EQ(dor_ci.subpassDescriptionRequestCount, 1);
    EXPECT_EQ(dor_ci.attachmentDescriptionRequestCount, 2);
    EXPECT_EQ(dor_ci.descriptorSetLayoutBindingRequestCount, 2);
    EXPECT_EQ(dor_ci.descriptorSetLayoutBindingLimit, 2);
    EXPECT_EQ(dor_ci.maxImageViewMipLevels, 1);
    EXPECT_EQ(dor_ci.maxImageViewArrayLayers, 1);
    EXPECT_EQ(dor_ci.maxLayeredImageViewMipLevels, 0);
    EXPECT_EQ(dor_ci.maxOcclusionQueriesPerPool, 0);
    EXPECT_EQ(dor_ci.maxPipelineStatisticsQueriesPerPool, 0);
    EXPECT_EQ(dor_ci.maxTimestampQueriesPerPool, 0);
    EXPECT_EQ(dor_ci.maxImmutableSamplersPerDescriptorSetLayout, 0);
}

TEST_F(Parse, VkPipelineOfflineCreateInfo) {
    TEST_DESCRIPTION("Tests parsing of a reasonably complex pipeline offline create info JSON");

    VkPipelineOfflineCreateInfo po_ci;
    std::string json = {R"({
        "sType" : "VK_STRUCTURE_TYPE_PIPELINE_OFFLINE_CREATE_INFO",
        "pNext": "NULL",
        "pipelineIdentifier": [
        85,
        43,
        255,
        24,
        155,
        64,
        62,
        24,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0
    ],
        "matchControl": "VK_PIPELINE_MATCH_CONTROL_APPLICATION_UUID_EXACT_MATCH",
        "poolEntrySize": 1048576
    })"};

    CHECK_PARSE(vpjParseSingleStructJson(this->parser_, json.c_str(), &po_ci, &msg_));

    EXPECT_EQ(po_ci.sType, VK_STRUCTURE_TYPE_PIPELINE_OFFLINE_CREATE_INFO);
    EXPECT_EQ(po_ci.pNext, nullptr);
    EXPECT_EQ(po_ci.pipelineIdentifier[0], 85);
    EXPECT_EQ(po_ci.pipelineIdentifier[1], 43);
    EXPECT_EQ(po_ci.pipelineIdentifier[2], 255);
    EXPECT_EQ(po_ci.pipelineIdentifier[3], 24);
    EXPECT_EQ(po_ci.pipelineIdentifier[4], 155);
    EXPECT_EQ(po_ci.pipelineIdentifier[5], 64);
    EXPECT_EQ(po_ci.pipelineIdentifier[6], 62);
    EXPECT_EQ(po_ci.pipelineIdentifier[7], 24);
    EXPECT_EQ(po_ci.pipelineIdentifier[8], 0);
    EXPECT_EQ(po_ci.pipelineIdentifier[9], 0);
    EXPECT_EQ(po_ci.pipelineIdentifier[10], 0);
    EXPECT_EQ(po_ci.pipelineIdentifier[11], 0);
    EXPECT_EQ(po_ci.pipelineIdentifier[12], 0);
    EXPECT_EQ(po_ci.pipelineIdentifier[13], 0);
    EXPECT_EQ(po_ci.pipelineIdentifier[14], 0);
    EXPECT_EQ(po_ci.pipelineIdentifier[15], 0);
    EXPECT_EQ(po_ci.matchControl, VK_PIPELINE_MATCH_CONTROL_APPLICATION_UUID_EXACT_MATCH);
    EXPECT_EQ(po_ci.poolEntrySize, 1048576);
}

TEST_F(Parse, ComputePipelineJSON) {
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
				"flags" : "VK_PIPELINE_SHADER_STAGE_CREATE_REQUIRE_FULL_SUBGROUPS_BIT",
				"module" : "",
				"pName" : "main",
				"pNext" : 
				{
					"pNext" : "NULL",
					"requiredSubgroupSize" : 64,
					"sType" : "VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_REQUIRED_SUBGROUP_SIZE_CREATE_INFO"
				},
				"pSpecializationInfo" : "NULL",
				"sType" : "VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO",
				"stage" : "VK_SHADER_STAGE_COMPUTE_BIT"
			}
		},
		"DescriptorSetLayouts" : 
		[
			{
				"DescriptorSetLayout1" : 
				{
					"bindingCount" : 3,
					"flags" : "VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT",
					"pBindings" : 
					[
						{
							"binding" : 0,
							"descriptorCount" : 1,
							"descriptorType" : "VK_DESCRIPTOR_TYPE_STORAGE_BUFFER",
							"pImmutableSamplers" : "NULL",
							"stageFlags" : "VK_SHADER_STAGE_COMPUTE_BIT"
						},
						{
							"binding" : 1,
							"descriptorCount" : 1,
							"descriptorType" : "VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER",
							"pImmutableSamplers" : 
							[
								"ImmutableSampler1"
							],
							"stageFlags" : "VK_SHADER_STAGE_COMPUTE_BIT"
						},
						{
							"binding" : 2,
							"descriptorCount" : 1,
							"descriptorType" : "VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER",
							"pImmutableSamplers" : 
							[
								"YcbcrSampler1"
							],
							"stageFlags" : "VK_SHADER_STAGE_COMPUTE_BIT"
						}
					],
					"pNext" : 
					{
						"bindingCount" : 1,
						"pBindingFlags" : 
						[
							"VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT | VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT"
						],
						"pNext" : "NULL",
						"sType" : "VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO"
					},
					"sType" : "VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO"
				}
			}
		],
		"ImmutableSamplers" : 
		[
			{
				"ImmutableSampler1" : 
				{
					"addressModeU" : "VK_SAMPLER_ADDRESS_MODE_REPEAT",
					"addressModeV" : "VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT",
					"addressModeW" : "VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE",
					"anisotropyEnable" : "VK_FALSE",
					"borderColor" : "VK_BORDER_COLOR_FLOAT_CUSTOM_EXT",
					"compareEnable" : "VK_FALSE",
					"compareOp" : "VK_COMPARE_OP_NEVER",
					"flags" : 0,
					"magFilter" : "VK_FILTER_CUBIC_EXT",
					"maxAnisotropy" : 2.0,
					"maxLod" : 1000.0,
					"minFilter" : "VK_FILTER_NEAREST",
					"minLod" : 0.5,
					"mipLodBias" : 0.5,
					"mipmapMode" : "VK_SAMPLER_MIPMAP_MODE_LINEAR",
					"pNext" : 
					{
						"pNext" : "NULL",
						"reductionMode" : "VK_SAMPLER_REDUCTION_MODE_MAX",
						"sType" : "VK_STRUCTURE_TYPE_SAMPLER_REDUCTION_MODE_CREATE_INFO"
					},
					"sType" : "VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO",
					"unnormalizedCoordinates" : "VK_TRUE"
				}
			},
			{
				"YcbcrSampler1" : 
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
						"conversion" : "YcbcrConversion1",
						"pNext" : "NULL",
						"sType" : "VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_INFO"
					},
					"sType" : "VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO",
					"unnormalizedCoordinates" : "VK_FALSE"
				}
			}
		],
		"PhysicalDeviceFeatures" : 
		{
			"features" : 
			{
				"alphaToOne" : "VK_FALSE",
				"depthBiasClamp" : "VK_FALSE",
				"depthBounds" : "VK_FALSE",
				"depthClamp" : "VK_FALSE",
				"drawIndirectFirstInstance" : "VK_FALSE",
				"dualSrcBlend" : "VK_FALSE",
				"fillModeNonSolid" : "VK_FALSE",
				"fragmentStoresAndAtomics" : "VK_FALSE",
				"fullDrawIndexUint32" : "VK_FALSE",
				"geometryShader" : "VK_FALSE",
				"imageCubeArray" : "VK_FALSE",
				"independentBlend" : "VK_FALSE",
				"inheritedQueries" : "VK_FALSE",
				"largePoints" : "VK_FALSE",
				"logicOp" : "VK_FALSE",
				"multiDrawIndirect" : "VK_FALSE",
				"multiViewport" : "VK_FALSE",
				"occlusionQueryPrecise" : "VK_FALSE",
				"pipelineStatisticsQuery" : "VK_FALSE",
				"robustBufferAccess" : "VK_TRUE",
				"sampleRateShading" : "VK_FALSE",
				"samplerAnisotropy" : "VK_FALSE",
				"shaderClipDistance" : "VK_FALSE",
				"shaderCullDistance" : "VK_FALSE",
				"shaderFloat64" : "VK_FALSE",
				"shaderImageGatherExtended" : "VK_FALSE",
				"shaderInt16" : "VK_FALSE",
				"shaderInt64" : "VK_FALSE",
				"shaderResourceMinLod" : "VK_FALSE",
				"shaderResourceResidency" : "VK_FALSE",
				"shaderSampledImageArrayDynamicIndexing" : "VK_FALSE",
				"shaderStorageBufferArrayDynamicIndexing" : "VK_FALSE",
				"shaderStorageImageArrayDynamicIndexing" : "VK_FALSE",
				"shaderStorageImageExtendedFormats" : "VK_FALSE",
				"shaderStorageImageMultisample" : "VK_FALSE",
				"shaderStorageImageReadWithoutFormat" : "VK_FALSE",
				"shaderStorageImageWriteWithoutFormat" : "VK_FALSE",
				"shaderTessellationAndGeometryPointSize" : "VK_FALSE",
				"shaderUniformBufferArrayDynamicIndexing" : "VK_FALSE",
				"sparseBinding" : "VK_FALSE",
				"sparseResidency16Samples" : "VK_FALSE",
				"sparseResidency2Samples" : "VK_FALSE",
				"sparseResidency4Samples" : "VK_FALSE",
				"sparseResidency8Samples" : "VK_FALSE",
				"sparseResidencyAliased" : "VK_FALSE",
				"sparseResidencyBuffer" : "VK_FALSE",
				"sparseResidencyImage2D" : "VK_FALSE",
				"sparseResidencyImage3D" : "VK_FALSE",
				"tessellationShader" : "VK_FALSE",
				"textureCompressionASTC_LDR" : "VK_FALSE",
				"textureCompressionBC" : "VK_FALSE",
				"textureCompressionETC2" : "VK_FALSE",
				"variableMultisampleRate" : "VK_FALSE",
				"vertexPipelineStoresAndAtomics" : "VK_FALSE",
				"wideLines" : "VK_FALSE"
			},
			"pNext" : 
			{
				"pNext" : "NULL",
				"sType" : "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES",
				"synchronization2" : "VK_TRUE"
			},
			"sType" : "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2"
		},
		"PipelineLayout" : 
		{
			"flags" : 0,
			"pNext" : "NULL",
			"pPushConstantRanges" : 
			[
				{
					"offset" : 0,
					"size" : 4,
					"stageFlags" : "VK_SHADER_STAGE_COMPUTE_BIT"
				}
			],
			"pSetLayouts" : 
			[
				"DescriptorSetLayout1"
			],
			"pushConstantRangeCount" : 1,
			"sType" : "VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO",
			"setLayoutCount" : 1
		},
		"ShaderFileNames" : 
		[
			{
				"filename" : "shader.comp.spv",
				"stage" : "VK_SHADER_STAGE_COMPUTE_BIT"
			}
		],
		"YcbcrSamplers" : 
		[
			{
				"YcbcrConversion1" : 
				{
					"chromaFilter" : "VK_FILTER_CUBIC_EXT",
					"components" : 
					{
						"a" : "VK_COMPONENT_SWIZZLE_R",
						"b" : "VK_COMPONENT_SWIZZLE_G",
						"g" : "VK_COMPONENT_SWIZZLE_B",
						"r" : "VK_COMPONENT_SWIZZLE_A"
					},
					"forceExplicitReconstruction" : "VK_TRUE",
					"format" : "VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16",
					"pNext" : "NULL",
					"sType" : "VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_CREATE_INFO",
					"xChromaOffset" : "VK_CHROMA_LOCATION_COSITED_EVEN",
					"yChromaOffset" : "VK_CHROMA_LOCATION_MIDPOINT",
					"ycbcrModel" : "VK_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_2020",
					"ycbcrRange" : "VK_SAMPLER_YCBCR_RANGE_ITU_NARROW"
				}
			}
		]
	},
	"EnabledExtensions" : 
	[
		"VK_EXT_robustness2"
	],
	"PipelineUUID" : 
	[
		85,
		43,
		255,
		24,
		155,
		64,
		62,
		24,
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
    EXPECT_TRUE(ValidatePipelineJson(json));

    VpjData data;
    EXPECT_TRUE(vpjParsePipelineJson(this->parser_, json.c_str(), &data, &msg_));
    CHECK_PARSE(true);
    const auto& dsl_ci =
        reinterpret_cast<const VkDescriptorSetLayoutCreateInfo*>(data.computePipelineState.pDescriptorSetLayouts)[0];
    const auto& dsl_names = data.computePipelineState.ppDescriptorSetLayoutNames;
    const auto& bf_ci = *reinterpret_cast<const VkDescriptorSetLayoutBindingFlagsCreateInfo*>(dsl_ci.pNext);
    const auto& pl_ci = *reinterpret_cast<const VkPipelineLayoutCreateInfo*>(data.computePipelineState.pPipelineLayout);
    const auto& cp_ci = *reinterpret_cast<const VkComputePipelineCreateInfo*>(data.computePipelineState.pComputePipeline);
    const auto& pssrss_ci = *reinterpret_cast<const VkPipelineShaderStageRequiredSubgroupSizeCreateInfo*>(cp_ci.stage.pNext);
    const auto& sfn = data.computePipelineState.pShaderFileNames;
    const auto& pdf = *reinterpret_cast<const VkPhysicalDeviceFeatures2*>(data.computePipelineState.pPhysicalDeviceFeatures);
    const auto& pdfsync2 = *reinterpret_cast<const VkPhysicalDeviceSynchronization2Features*>(pdf.pNext);
    const auto& sampler_cis = reinterpret_cast<const VkSamplerCreateInfo*>(data.computePipelineState.pImmutableSamplers);
    const auto& sampler_names = data.computePipelineState.ppImmutableSamplerNames;
    const auto& srm_ci = *reinterpret_cast<const VkSamplerReductionModeCreateInfo*>(sampler_cis[0].pNext);
    const auto& ycbcrci = *reinterpret_cast<const VkSamplerYcbcrConversionInfo*>(sampler_cis[1].pNext);
    const auto& ycbcr_names = data.computePipelineState.ppYcbcrSamplerNames;
    const auto& ycbcr_ci = reinterpret_cast<const VkSamplerYcbcrConversionCreateInfo*>(data.computePipelineState.pYcbcrSamplers)[0];

    EXPECT_EQ(data.computePipelineState.descriptorSetLayoutCount, 1);
    EXPECT_STREQ(dsl_names[0], "DescriptorSetLayout1");
    EXPECT_EQ(dsl_ci.sType, VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO);
    EXPECT_EQ(bf_ci.sType, VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO);
    EXPECT_EQ(bf_ci.pNext, nullptr);
    EXPECT_EQ(bf_ci.bindingCount, 1);
    EXPECT_EQ(bf_ci.pBindingFlags[0], VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT | VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT);
    EXPECT_EQ(dsl_ci.flags, VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT);
    EXPECT_EQ(dsl_ci.bindingCount, 3);
    EXPECT_EQ(dsl_ci.pBindings[0].binding, 0);
    EXPECT_EQ(dsl_ci.pBindings[0].descriptorType, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
    EXPECT_EQ(dsl_ci.pBindings[0].descriptorCount, 1);
    EXPECT_EQ(dsl_ci.pBindings[0].stageFlags, VK_SHADER_STAGE_COMPUTE_BIT);
    EXPECT_EQ(dsl_ci.pBindings[0].pImmutableSamplers, nullptr);
    EXPECT_EQ(dsl_ci.pBindings[1].binding, 1);
    EXPECT_EQ(dsl_ci.pBindings[1].descriptorType, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
    EXPECT_EQ((uintptr_t)dsl_ci.pBindings[1].pImmutableSamplers[0], 0);
    EXPECT_EQ(dsl_ci.pBindings[1].descriptorCount, 1);
    EXPECT_EQ(dsl_ci.pBindings[1].stageFlags, VK_SHADER_STAGE_COMPUTE_BIT);
    EXPECT_EQ(dsl_ci.pBindings[2].binding, 2);
    EXPECT_EQ(dsl_ci.pBindings[2].descriptorType, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
    EXPECT_EQ((uintptr_t)dsl_ci.pBindings[2].pImmutableSamplers[0], 1);
    EXPECT_EQ(dsl_ci.pBindings[2].descriptorCount, 1);
    EXPECT_EQ(dsl_ci.pBindings[2].stageFlags, VK_SHADER_STAGE_COMPUTE_BIT);
    EXPECT_EQ(data.computePipelineState.immutableSamplerCount, 2);
    EXPECT_STREQ(sampler_names[0], "ImmutableSampler1");
    EXPECT_STREQ(sampler_names[1], "YcbcrSampler1");
    EXPECT_EQ(sampler_cis[0].sType, VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO);
    EXPECT_EQ(srm_ci.sType, VK_STRUCTURE_TYPE_SAMPLER_REDUCTION_MODE_CREATE_INFO);
    EXPECT_EQ(srm_ci.pNext, nullptr);
    EXPECT_EQ(srm_ci.reductionMode, VK_SAMPLER_REDUCTION_MODE_MAX);
    EXPECT_EQ(sampler_cis[0].addressModeU, VK_SAMPLER_ADDRESS_MODE_REPEAT);
    EXPECT_EQ(sampler_cis[0].addressModeV, VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT);
    EXPECT_EQ(sampler_cis[0].addressModeW, VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE);
    EXPECT_EQ(sampler_cis[0].anisotropyEnable, VK_FALSE);
    EXPECT_EQ(sampler_cis[0].borderColor, VK_BORDER_COLOR_FLOAT_CUSTOM_EXT);
    EXPECT_EQ(sampler_cis[0].compareEnable, VK_FALSE);
    EXPECT_EQ(sampler_cis[0].compareOp, VK_COMPARE_OP_NEVER);
    EXPECT_EQ(sampler_cis[0].flags, 0);
    EXPECT_EQ(sampler_cis[0].magFilter, VK_FILTER_CUBIC_EXT);
    EXPECT_EQ(sampler_cis[0].maxAnisotropy, 2.0);
    EXPECT_EQ(sampler_cis[0].maxLod, 1000.0);
    EXPECT_EQ(sampler_cis[0].minFilter, VK_FILTER_NEAREST);
    EXPECT_EQ(sampler_cis[0].minLod, 0.5);
    EXPECT_EQ(sampler_cis[0].mipLodBias, 0.5);
    EXPECT_EQ(sampler_cis[0].mipmapMode, VK_SAMPLER_MIPMAP_MODE_LINEAR);
    EXPECT_EQ(sampler_cis[0].unnormalizedCoordinates, VK_TRUE);
    EXPECT_EQ(sampler_cis[1].sType, VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO);
    EXPECT_EQ(ycbcrci.sType, VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_INFO);
    EXPECT_EQ(ycbcrci.pNext, nullptr);
    EXPECT_EQ((uintptr_t)ycbcrci.conversion, 0);
    EXPECT_EQ(sampler_cis[1].addressModeU, VK_SAMPLER_ADDRESS_MODE_REPEAT);
    EXPECT_EQ(sampler_cis[1].addressModeV, VK_SAMPLER_ADDRESS_MODE_REPEAT);
    EXPECT_EQ(sampler_cis[1].addressModeW, VK_SAMPLER_ADDRESS_MODE_REPEAT);
    EXPECT_EQ(sampler_cis[1].anisotropyEnable, VK_FALSE);
    EXPECT_EQ(sampler_cis[1].borderColor, VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK);
    EXPECT_EQ(sampler_cis[1].compareEnable, VK_FALSE);
    EXPECT_EQ(sampler_cis[1].compareOp, VK_COMPARE_OP_NEVER);
    EXPECT_EQ(sampler_cis[1].flags, 0);
    EXPECT_EQ(sampler_cis[1].magFilter, VK_FILTER_NEAREST);
    EXPECT_EQ(sampler_cis[1].maxAnisotropy, 0.0);
    EXPECT_EQ(sampler_cis[1].maxLod, 0.0);
    EXPECT_EQ(sampler_cis[1].minFilter, VK_FILTER_NEAREST);
    EXPECT_EQ(sampler_cis[1].minLod, 0.0);
    EXPECT_EQ(sampler_cis[1].mipLodBias, 0.0);
    EXPECT_EQ(sampler_cis[1].mipmapMode, VK_SAMPLER_MIPMAP_MODE_NEAREST);
    EXPECT_EQ(sampler_cis[1].unnormalizedCoordinates, VK_FALSE);
    EXPECT_EQ(pl_ci.sType, VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO);
    EXPECT_EQ(pl_ci.pNext, nullptr);
    EXPECT_EQ(pl_ci.flags, 0);
    EXPECT_EQ(pl_ci.setLayoutCount, 1);
    EXPECT_EQ(pl_ci.pSetLayouts[0], reinterpret_cast<void*>(0));
    EXPECT_EQ(pl_ci.pushConstantRangeCount, 1);
    EXPECT_EQ(pl_ci.pPushConstantRanges[0].stageFlags, VK_SHADER_STAGE_COMPUTE_BIT);
    EXPECT_EQ(pl_ci.pPushConstantRanges[0].offset, 0);
    EXPECT_EQ(pl_ci.pPushConstantRanges[0].size, 4);
    EXPECT_EQ(cp_ci.sType, VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO);
    EXPECT_EQ(cp_ci.flags, 0);
    EXPECT_EQ(cp_ci.stage.sType, VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO);
    EXPECT_EQ(pssrss_ci.sType, VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_REQUIRED_SUBGROUP_SIZE_CREATE_INFO);
    EXPECT_EQ(pssrss_ci.pNext, nullptr);
    EXPECT_EQ(pssrss_ci.requiredSubgroupSize, 64);
    EXPECT_EQ(cp_ci.stage.flags, VK_PIPELINE_SHADER_STAGE_CREATE_REQUIRE_FULL_SUBGROUPS_BIT);
    EXPECT_EQ(cp_ci.stage.stage, VK_SHADER_STAGE_COMPUTE_BIT);
    EXPECT_STREQ(cp_ci.stage.pName, "main");
    EXPECT_EQ(cp_ci.stage.pSpecializationInfo, nullptr);
    EXPECT_EQ(cp_ci.stage.module, VK_NULL_HANDLE);
    // cp_ci.layout can't be meaningfully reconstructed
    EXPECT_EQ(cp_ci.basePipelineHandle, VK_NULL_HANDLE);
    EXPECT_EQ(cp_ci.basePipelineIndex, 0);
    EXPECT_EQ(sfn[0].stage, VK_SHADER_STAGE_COMPUTE_BIT);
    EXPECT_STREQ(sfn[0].pFilename, "shader.comp.spv");
    EXPECT_EQ(data.computePipelineState.ycbcrSamplerCount, 1);
    EXPECT_STREQ(ycbcr_names[0], "YcbcrConversion1");
    EXPECT_EQ(ycbcr_ci.sType, VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_CREATE_INFO);
    EXPECT_EQ(ycbcr_ci.pNext, nullptr);
    EXPECT_EQ(ycbcr_ci.chromaFilter, VK_FILTER_CUBIC_EXT);
    EXPECT_EQ(ycbcr_ci.components.a, VK_COMPONENT_SWIZZLE_R);
    EXPECT_EQ(ycbcr_ci.components.b, VK_COMPONENT_SWIZZLE_G);
    EXPECT_EQ(ycbcr_ci.components.g, VK_COMPONENT_SWIZZLE_B);
    EXPECT_EQ(ycbcr_ci.components.r, VK_COMPONENT_SWIZZLE_A);
    EXPECT_EQ(ycbcr_ci.forceExplicitReconstruction, VK_TRUE);
    EXPECT_EQ(ycbcr_ci.format, VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16);
    EXPECT_EQ(ycbcr_ci.xChromaOffset, VK_CHROMA_LOCATION_COSITED_EVEN);
    EXPECT_EQ(ycbcr_ci.yChromaOffset, VK_CHROMA_LOCATION_MIDPOINT);
    EXPECT_EQ(ycbcr_ci.ycbcrModel, VK_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_2020);
    EXPECT_EQ(ycbcr_ci.ycbcrRange, VK_SAMPLER_YCBCR_RANGE_ITU_NARROW);
    EXPECT_EQ(pdf.sType, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2);
    EXPECT_EQ(pdfsync2.sType, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES);
    EXPECT_EQ(pdfsync2.pNext, nullptr);
    EXPECT_EQ(pdfsync2.synchronization2, VK_TRUE);
    EXPECT_EQ(pdf.features.robustBufferAccess, VK_TRUE);
    EXPECT_EQ(pdf.features.fullDrawIndexUint32, VK_FALSE);
    EXPECT_EQ(pdf.features.imageCubeArray, VK_FALSE);
    EXPECT_EQ(pdf.features.independentBlend, VK_FALSE);
    EXPECT_EQ(pdf.features.geometryShader, VK_FALSE);
    EXPECT_EQ(pdf.features.tessellationShader, VK_FALSE);
    EXPECT_EQ(pdf.features.sampleRateShading, VK_FALSE);
    EXPECT_EQ(pdf.features.dualSrcBlend, VK_FALSE);
    EXPECT_EQ(pdf.features.logicOp, VK_FALSE);
    EXPECT_EQ(pdf.features.multiDrawIndirect, VK_FALSE);
    EXPECT_EQ(pdf.features.drawIndirectFirstInstance, VK_FALSE);
    EXPECT_EQ(pdf.features.depthClamp, VK_FALSE);
    EXPECT_EQ(pdf.features.depthBiasClamp, VK_FALSE);
    EXPECT_EQ(pdf.features.fillModeNonSolid, VK_FALSE);
    EXPECT_EQ(pdf.features.depthBounds, VK_FALSE);
    EXPECT_EQ(pdf.features.wideLines, VK_FALSE);
    EXPECT_EQ(pdf.features.largePoints, VK_FALSE);
    EXPECT_EQ(pdf.features.alphaToOne, VK_FALSE);
    EXPECT_EQ(pdf.features.multiViewport, VK_FALSE);
    EXPECT_EQ(pdf.features.samplerAnisotropy, VK_FALSE);
    EXPECT_EQ(pdf.features.textureCompressionETC2, VK_FALSE);
    EXPECT_EQ(pdf.features.textureCompressionASTC_LDR, VK_FALSE);
    EXPECT_EQ(pdf.features.textureCompressionBC, VK_FALSE);
    EXPECT_EQ(pdf.features.occlusionQueryPrecise, VK_FALSE);
    EXPECT_EQ(pdf.features.pipelineStatisticsQuery, VK_FALSE);
    EXPECT_EQ(pdf.features.vertexPipelineStoresAndAtomics, VK_FALSE);
    EXPECT_EQ(pdf.features.fragmentStoresAndAtomics, VK_FALSE);
    EXPECT_EQ(pdf.features.shaderTessellationAndGeometryPointSize, VK_FALSE);
    EXPECT_EQ(pdf.features.shaderImageGatherExtended, VK_FALSE);
    EXPECT_EQ(pdf.features.shaderStorageImageExtendedFormats, VK_FALSE);
    EXPECT_EQ(pdf.features.shaderStorageImageMultisample, VK_FALSE);
    EXPECT_EQ(pdf.features.shaderStorageImageReadWithoutFormat, VK_FALSE);
    EXPECT_EQ(pdf.features.shaderStorageImageWriteWithoutFormat, VK_FALSE);
    EXPECT_EQ(pdf.features.shaderUniformBufferArrayDynamicIndexing, VK_FALSE);
    EXPECT_EQ(pdf.features.shaderSampledImageArrayDynamicIndexing, VK_FALSE);
    EXPECT_EQ(pdf.features.shaderStorageBufferArrayDynamicIndexing, VK_FALSE);
    EXPECT_EQ(pdf.features.shaderStorageImageArrayDynamicIndexing, VK_FALSE);
    EXPECT_EQ(pdf.features.shaderClipDistance, VK_FALSE);
    EXPECT_EQ(pdf.features.shaderCullDistance, VK_FALSE);
    EXPECT_EQ(pdf.features.shaderFloat64, VK_FALSE);
    EXPECT_EQ(pdf.features.shaderInt64, VK_FALSE);
    EXPECT_EQ(pdf.features.shaderInt16, VK_FALSE);
    EXPECT_EQ(pdf.features.shaderResourceResidency, VK_FALSE);
    EXPECT_EQ(pdf.features.shaderResourceMinLod, VK_FALSE);
    EXPECT_EQ(pdf.features.sparseBinding, VK_FALSE);
    EXPECT_EQ(pdf.features.sparseResidencyBuffer, VK_FALSE);
    EXPECT_EQ(pdf.features.sparseResidencyImage2D, VK_FALSE);
    EXPECT_EQ(pdf.features.sparseResidencyImage3D, VK_FALSE);
    EXPECT_EQ(pdf.features.sparseResidency2Samples, VK_FALSE);
    EXPECT_EQ(pdf.features.sparseResidency4Samples, VK_FALSE);
    EXPECT_EQ(pdf.features.sparseResidency8Samples, VK_FALSE);
    EXPECT_EQ(pdf.features.sparseResidency16Samples, VK_FALSE);
    EXPECT_EQ(pdf.features.sparseResidencyAliased, VK_FALSE);
    EXPECT_EQ(pdf.features.variableMultisampleRate, VK_FALSE);
    EXPECT_EQ(pdf.features.inheritedQueries, VK_FALSE);
    EXPECT_EQ(data.enabledExtensionCount, 1);
    EXPECT_STREQ(data.ppEnabledExtensions[0], "VK_EXT_robustness2");
    uint8_t expected_uuid[VK_UUID_SIZE] = {85, 43, 255, 24, 155, 64, 62, 24, 0, 0, 0, 0, 0, 0, 0, 0};
    EXPECT_UUIDEQ(data.pipelineUUID, expected_uuid);
}

TEST_F(Parse, GraphicsPipelineJSON) {
    TEST_DESCRIPTION("Tests parsing of a reasonably simple graphics pipeline JSON");

#ifdef VK_USE_PLATFORM_SCREEN_QNX
    std::string ycbcr_ci_pnext = R"({
            "sType" : "VK_STRUCTURE_TYPE_EXTERNAL_FORMAT_QNX",
            "pNext": "NULL",
            "externalFormat": 10
    })";
#else
    std::string ycbcr_ci_pnext = R"("NULL")";
#endif  // VK_USE_PLATFORM_SCREEN_QNX

    const std::string json{R"({
	"EnabledExtensions" : 
	[
		"VK_EXT_robustness2"
	],
	"GraphicsPipelineState" : 
	{
		"DescriptorSetLayouts" : 
		[
			{
				"DescriptorSetLayout1" : 
				{
					"bindingCount" : 3,
					"flags" : "VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT",
					"pBindings" : 
					[
						{
							"binding" : 0,
							"descriptorCount" : 1,
							"descriptorType" : "VK_DESCRIPTOR_TYPE_STORAGE_BUFFER",
							"pImmutableSamplers" : "NULL",
							"stageFlags" : "VK_SHADER_STAGE_COMPUTE_BIT"
						},
						{
							"binding" : 1,
							"descriptorCount" : 1,
							"descriptorType" : "VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER",
							"pImmutableSamplers" : 
							[
								"ImmutableSampler1"
							],
							"stageFlags" : "VK_SHADER_STAGE_FRAGMENT_BIT"
						},
						{
							"binding" : 2,
							"descriptorCount" : 1,
							"descriptorType" : "VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER",
							"pImmutableSamplers" : 
							[
								"YcbcrSampler1"
							],
							"stageFlags" : "VK_SHADER_STAGE_FRAGMENT_BIT"
						}
					],
					"pNext" : 
					{
						"bindingCount" : 1,
						"pBindingFlags" : 
						[
							"VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT | VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT"
						],
						"pNext" : "NULL",
						"sType" : "VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO"
					},
					"sType" : "VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO"
				}
			}
		],
		"GraphicsPipeline" : 
		{
			"sType" : "VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO",
			"pNext": {
				"sType": "VK_STRUCTURE_TYPE_PIPELINE_DISCARD_RECTANGLE_STATE_CREATE_INFO_EXT",
				"pNext" : "NULL",
				"flags": "0",
				"discardRectangleMode": "VK_DISCARD_RECTANGLE_MODE_EXCLUSIVE_EXT",
				"discardRectangleCount": 1,
				"pDiscardRectangles": [
					{
						"offset":
						{
							"x" : 0,
							"y" : 0
						},
						"extent":
						{
							"width" : 51,
							"height" : 51
						}
					}
				]
			},
			"flags" : "0",
			"stageCount" : 5,
			"pStages": 
			[
			{
				"sType" : "VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO",
				"pNext": {
					"pNext" : "NULL",
					"requiredSubgroupSize" : 64,
					"sType" : "VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_REQUIRED_SUBGROUP_SIZE_CREATE_INFO"
				},
				"flags" : "0",
				"stage" : "VK_SHADER_STAGE_VERTEX_BIT",
				"module" : 35,
				"pName" : "main",
				"pSpecializationInfo": 
				"NULL"
			},
			{
				"sType" : "VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO",
				"pNext":"NULL",
				"flags" : "0",
				"stage" : "VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT",
				"module" : 36,
				"pName" : "main",
				"pSpecializationInfo": 
				"NULL"
			},
			{
				"sType" : "VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO",
				"pNext":"NULL",
				"flags" : "0",
				"stage" : "VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT",
				"module" : 37,
				"pName" : "main",
				"pSpecializationInfo": 
				"NULL"
			},
			{
				"sType" : "VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO",
				"pNext":"NULL",
				"flags" : "0",
				"stage" : "VK_SHADER_STAGE_GEOMETRY_BIT",
				"module" : 38,
				"pName" : "main",
				"pSpecializationInfo": 
				"NULL"
			},
			{
				"sType" : "VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO",
				"pNext":"NULL",
				"flags" : "0",
				"stage" : "VK_SHADER_STAGE_FRAGMENT_BIT",
				"module" : 39,
				"pName" : "main",
				"pSpecializationInfo": 
				"NULL"
			}
			],
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
					"stride" : 32,
					"inputRate" : "VK_VERTEX_INPUT_RATE_VERTEX"
				}
				],
				"vertexAttributeDescriptionCount" : 2,
				"pVertexAttributeDescriptions": 
				[
				{
					"location" : 0,
					"binding" : 0,
					"format" : "VK_FORMAT_R32G32B32A32_SFLOAT",
					"offset" : 0
				},
				{
					"location" : 1,
					"binding" : 0,
					"format" : "VK_FORMAT_R32G32B32A32_SFLOAT",
					"offset" : 16
				}
				]
			},
			"pInputAssemblyState": 
			{
				"sType" : "VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO",
				"pNext":"NULL",
				"flags" : 0,
				"topology" : "VK_PRIMITIVE_TOPOLOGY_PATCH_LIST",
				"primitiveRestartEnable" : "VK_FALSE"
			},
			"pTessellationState": 
			{
				"sType" : "VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO",
				"pNext":"NULL",
				"flags" : 0,
				"patchControlPoints" : 4
			},
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
					"width" : 51,
					"height" : 51,
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
						"width" : 51,
						"height" : 51
					}
				}
				]
			},
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
				"depthTestEnable" : "VK_TRUE",
				"depthWriteEnable" : "VK_TRUE",
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
			"pDynamicState": {
				"sType": "VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO",
				"pNext": "NULL",
				"flags": 0,
				"dynamicStateCount": 3,
				"pDynamicStates": [
					"VK_DYNAMIC_STATE_VIEWPORT",
					"VK_DYNAMIC_STATE_SCISSOR",
					"VK_DYNAMIC_STATE_DISCARD_RECTANGLE_EXT"
				]
			},
			"layout" : 8,
			"renderPass" : 6,
			"subpass" : 0,
			"basePipelineHandle" : "",
			"basePipelineIndex" : 0
		},
		"ImmutableSamplers" : 
		[
			{
				"ImmutableSampler1" : 
				{
					"addressModeU" : "VK_SAMPLER_ADDRESS_MODE_REPEAT",
					"addressModeV" : "VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT",
					"addressModeW" : "VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE",
					"anisotropyEnable" : "VK_FALSE",
					"borderColor" : "VK_BORDER_COLOR_FLOAT_CUSTOM_EXT",
					"compareEnable" : "VK_FALSE",
					"compareOp" : "VK_COMPARE_OP_NEVER",
					"flags" : 0,
					"magFilter" : "VK_FILTER_CUBIC_EXT",
					"maxAnisotropy" : 2.0,
					"maxLod" : 1000.0,
					"minFilter" : "VK_FILTER_NEAREST",
					"minLod" : 0.5,
					"mipLodBias" : 0.5,
					"mipmapMode" : "VK_SAMPLER_MIPMAP_MODE_LINEAR",
					"pNext" : 
					{
						"pNext" : "NULL",
						"reductionMode" : "VK_SAMPLER_REDUCTION_MODE_MAX",
						"sType" : "VK_STRUCTURE_TYPE_SAMPLER_REDUCTION_MODE_CREATE_INFO"
					},
					"sType" : "VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO",
					"unnormalizedCoordinates" : "VK_TRUE"
				}
			},
			{
				"YcbcrSampler1" : 
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
						"conversion" : "YcbcrConversion1",
						"pNext" : "NULL",
						"sType" : "VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_INFO"
					},
					"sType" : "VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO",
					"unnormalizedCoordinates" : "VK_FALSE"
				}
			}
		],
		"PhysicalDeviceFeatures" : 
		{
			"features" : 
			{
				"alphaToOne" : "VK_FALSE",
				"depthBiasClamp" : "VK_FALSE",
				"depthBounds" : "VK_FALSE",
				"depthClamp" : "VK_FALSE",
				"drawIndirectFirstInstance" : "VK_FALSE",
				"dualSrcBlend" : "VK_FALSE",
				"fillModeNonSolid" : "VK_FALSE",
				"fragmentStoresAndAtomics" : "VK_FALSE",
				"fullDrawIndexUint32" : "VK_FALSE",
				"geometryShader" : "VK_FALSE",
				"imageCubeArray" : "VK_FALSE",
				"independentBlend" : "VK_FALSE",
				"inheritedQueries" : "VK_FALSE",
				"largePoints" : "VK_FALSE",
				"logicOp" : "VK_FALSE",
				"multiDrawIndirect" : "VK_FALSE",
				"multiViewport" : "VK_FALSE",
				"occlusionQueryPrecise" : "VK_FALSE",
				"pipelineStatisticsQuery" : "VK_FALSE",
				"robustBufferAccess" : "VK_TRUE",
				"sampleRateShading" : "VK_FALSE",
				"samplerAnisotropy" : "VK_FALSE",
				"shaderClipDistance" : "VK_FALSE",
				"shaderCullDistance" : "VK_FALSE",
				"shaderFloat64" : "VK_FALSE",
				"shaderImageGatherExtended" : "VK_FALSE",
				"shaderInt16" : "VK_FALSE",
				"shaderInt64" : "VK_FALSE",
				"shaderResourceMinLod" : "VK_FALSE",
				"shaderResourceResidency" : "VK_FALSE",
				"shaderSampledImageArrayDynamicIndexing" : "VK_FALSE",
				"shaderStorageBufferArrayDynamicIndexing" : "VK_FALSE",
				"shaderStorageImageArrayDynamicIndexing" : "VK_FALSE",
				"shaderStorageImageExtendedFormats" : "VK_FALSE",
				"shaderStorageImageMultisample" : "VK_FALSE",
				"shaderStorageImageReadWithoutFormat" : "VK_FALSE",
				"shaderStorageImageWriteWithoutFormat" : "VK_FALSE",
				"shaderTessellationAndGeometryPointSize" : "VK_FALSE",
				"shaderUniformBufferArrayDynamicIndexing" : "VK_FALSE",
				"sparseBinding" : "VK_FALSE",
				"sparseResidency16Samples" : "VK_FALSE",
				"sparseResidency2Samples" : "VK_FALSE",
				"sparseResidency4Samples" : "VK_FALSE",
				"sparseResidency8Samples" : "VK_FALSE",
				"sparseResidencyAliased" : "VK_FALSE",
				"sparseResidencyBuffer" : "VK_FALSE",
				"sparseResidencyImage2D" : "VK_FALSE",
				"sparseResidencyImage3D" : "VK_FALSE",
				"tessellationShader" : "VK_FALSE",
				"textureCompressionASTC_LDR" : "VK_FALSE",
				"textureCompressionBC" : "VK_FALSE",
				"textureCompressionETC2" : "VK_FALSE",
				"variableMultisampleRate" : "VK_FALSE",
				"vertexPipelineStoresAndAtomics" : "VK_FALSE",
				"wideLines" : "VK_FALSE"
			},
			"pNext" : 
			{
				"pNext" : "NULL",
				"sType" : "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES",
				"synchronization2" : "VK_TRUE"
			},
			"sType" : "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2"
		},
		"PipelineLayout" : 
		{
			"flags" : 0,
			"pNext" : "NULL",
			"pPushConstantRanges" : 
			[
				{
					"offset" : 0,
					"size" : 4,
					"stageFlags" : "VK_SHADER_STAGE_COMPUTE_BIT"
				}
			],
			"pSetLayouts" : 
			[
				"DescriptorSetLayout1"
			],
			"pushConstantRangeCount" : 1,
			"sType" : "VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO",
			"setLayoutCount" : 1
		},
		"Renderpass" : 
		{
			"attachmentCount" : 2,
			"dependencyCount" : 2,
			"flags" : 0,
			"pAttachments" : 
			[
				{
					"finalLayout" : "VK_IMAGE_LAYOUT_PRESENT_SRC_KHR",
					"flags" : 0,
					"format" : "VK_FORMAT_R8G8B8A8_UNORM",
					"initialLayout" : "VK_IMAGE_LAYOUT_UNDEFINED",
					"loadOp" : "VK_ATTACHMENT_LOAD_OP_CLEAR",
					"samples" : "VK_SAMPLE_COUNT_1_BIT",
					"stencilLoadOp" : "VK_ATTACHMENT_LOAD_OP_DONT_CARE",
					"stencilStoreOp" : "VK_ATTACHMENT_STORE_OP_DONT_CARE",
					"storeOp" : "VK_ATTACHMENT_STORE_OP_STORE"
				},
				{
					"finalLayout" : "VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL",
					"flags" : 0,
					"format" : "VK_FORMAT_D16_UNORM",
					"initialLayout" : "VK_IMAGE_LAYOUT_UNDEFINED",
					"loadOp" : "VK_ATTACHMENT_LOAD_OP_CLEAR",
					"samples" : "VK_SAMPLE_COUNT_1_BIT",
					"stencilLoadOp" : "VK_ATTACHMENT_LOAD_OP_DONT_CARE",
					"stencilStoreOp" : "VK_ATTACHMENT_STORE_OP_DONT_CARE",
					"storeOp" : "VK_ATTACHMENT_STORE_OP_DONT_CARE"
				}
			],
			"pDependencies" : 
			[
				{
					"dependencyFlags" : 0,
					"dstAccessMask" : "VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT",
					"dstStageMask" : "VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT",
					"dstSubpass" : 0,
					"srcAccessMask" : "VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT",
					"srcStageMask" : "VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT",
					"srcSubpass" : 4294967295
				},
				{
					"dependencyFlags" : 0,
					"dstAccessMask" : "VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT",
					"dstStageMask" : "VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT",
					"dstSubpass" : 0,
					"srcAccessMask" : 0,
					"srcStageMask" : "VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT",
					"srcSubpass" : 4294967295
				}
			],
			"pNext" : "NULL",
			"pSubpasses" : 
			[
				{
					"colorAttachmentCount" : 1,
					"flags" : 0,
					"inputAttachmentCount" : 0,
					"pColorAttachments" : 
					[
						{
							"attachment" : 0,
							"layout" : "VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL"
						}
					],
					"pDepthStencilAttachment" : 
					{
						"attachment" : 1,
						"layout" : "VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL"
					},
					"pInputAttachments" : "NULL",
					"pPreserveAttachments" : "NULL",
					"pResolveAttachments" : "NULL",
					"pipelineBindPoint" : "VK_PIPELINE_BIND_POINT_GRAPHICS",
					"preserveAttachmentCount" : 0
				}
			],
			"sType" : "VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO",
			"subpassCount" : 1
		},
		"ShaderFileNames" : 
		[
			{
				"filename" : "shader.vert.spv",
				"stage" : "VK_SHADER_STAGE_VERTEX_BIT"
			},
			{
				"filename" : "shader.frag.spv",
				"stage" : "VK_SHADER_STAGE_FRAGMENT_BIT"
			}
		],
		"YcbcrSamplers" : 
		[
			{
				"YcbcrConversion1" : 
				{
					"sType" : "VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_CREATE_INFO",
					"pNext": )" +
                           ycbcr_ci_pnext + R"(,
					"format": "VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16",
					"ycbcrModel": "VK_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_2020",
					"ycbcrRange": "VK_SAMPLER_YCBCR_RANGE_ITU_NARROW",
					"components": {
						"r": "VK_COMPONENT_SWIZZLE_A",
						"g": "VK_COMPONENT_SWIZZLE_B",
						"b": "VK_COMPONENT_SWIZZLE_G",
						"a": "VK_COMPONENT_SWIZZLE_R"
					},
					"xChromaOffset": "VK_CHROMA_LOCATION_COSITED_EVEN",
					"yChromaOffset": "VK_CHROMA_LOCATION_MIDPOINT",
					"chromaFilter": "VK_FILTER_CUBIC_EXT",
					"forceExplicitReconstruction": "VK_TRUE"
				}
			}
		]
	},
	"PipelineUUID" : 
	[
		85,
		43,
		255,
		24,
		155,
		64,
		62,
		24,
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
    EXPECT_TRUE(ValidatePipelineJson(json));

    VpjData data;
    EXPECT_TRUE(vpjParsePipelineJson(this->parser_, json.c_str(), &data, &msg_));
    CHECK_PARSE(true);
    const auto& rp_ci = *reinterpret_cast<const VkRenderPassCreateInfo*>(data.graphicsPipelineState.pRenderPass);
    const auto& dsl_ci =
        reinterpret_cast<const VkDescriptorSetLayoutCreateInfo*>(data.graphicsPipelineState.pDescriptorSetLayouts)[0];
    const auto& dsl_names = data.graphicsPipelineState.ppDescriptorSetLayoutNames;
    const auto& bf_ci = *reinterpret_cast<const VkDescriptorSetLayoutBindingFlagsCreateInfo*>(dsl_ci.pNext);
    const auto& pl_ci = *reinterpret_cast<const VkPipelineLayoutCreateInfo*>(data.graphicsPipelineState.pPipelineLayout);
    const auto& gp_ci = *reinterpret_cast<const VkGraphicsPipelineCreateInfo*>(data.graphicsPipelineState.pGraphicsPipeline);
    const auto& pdrs_ci = *reinterpret_cast<const VkPipelineDiscardRectangleStateCreateInfoEXT*>(gp_ci.pNext);
    const auto& pvis_ci = *reinterpret_cast<const VkPipelineVertexInputStateCreateInfo*>(gp_ci.pVertexInputState);
    const auto& pias_ci = *reinterpret_cast<const VkPipelineInputAssemblyStateCreateInfo*>(gp_ci.pInputAssemblyState);
    const auto& pts_ci = *reinterpret_cast<const VkPipelineTessellationStateCreateInfo*>(gp_ci.pTessellationState);
    const auto& pvs_ci = *reinterpret_cast<const VkPipelineViewportStateCreateInfo*>(gp_ci.pViewportState);
    const auto& prs_ci = *reinterpret_cast<const VkPipelineRasterizationStateCreateInfo*>(gp_ci.pRasterizationState);
    const auto& pmss_ci = *reinterpret_cast<const VkPipelineMultisampleStateCreateInfo*>(gp_ci.pMultisampleState);
    const auto& pdss_ci = *reinterpret_cast<const VkPipelineDepthStencilStateCreateInfo*>(gp_ci.pDepthStencilState);
    const auto& pcbs_ci = *reinterpret_cast<const VkPipelineColorBlendStateCreateInfo*>(gp_ci.pColorBlendState);
    const auto& pds_ci = *reinterpret_cast<const VkPipelineDynamicStateCreateInfo*>(gp_ci.pDynamicState);
    const auto& pssrss_ci = *reinterpret_cast<const VkPipelineShaderStageRequiredSubgroupSizeCreateInfo*>(gp_ci.pStages[0].pNext);

    const auto& sampler_cis = reinterpret_cast<const VkSamplerCreateInfo*>(data.graphicsPipelineState.pImmutableSamplers);
    const auto& sampler_names = data.graphicsPipelineState.ppImmutableSamplerNames;
    const auto& srm_ci = *reinterpret_cast<const VkSamplerReductionModeCreateInfo*>(sampler_cis[0].pNext);
    const auto& ycbcrci = *reinterpret_cast<const VkSamplerYcbcrConversionInfo*>(sampler_cis[1].pNext);
    const auto& ycbcr_names = data.graphicsPipelineState.ppYcbcrSamplerNames;
    const auto& ycbcr_ci =
        reinterpret_cast<const VkSamplerYcbcrConversionCreateInfo*>(data.graphicsPipelineState.pYcbcrSamplers)[0];

    const auto& sfn = data.graphicsPipelineState.pShaderFileNames;

    EXPECT_EQ(gp_ci.sType, VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO);
    EXPECT_EQ(pdrs_ci.sType, VK_STRUCTURE_TYPE_PIPELINE_DISCARD_RECTANGLE_STATE_CREATE_INFO_EXT);
    EXPECT_EQ(pdrs_ci.pNext, nullptr);
    EXPECT_EQ(pdrs_ci.flags, 0);
    EXPECT_EQ(pdrs_ci.discardRectangleMode, VK_DISCARD_RECTANGLE_MODE_EXCLUSIVE_EXT);
    EXPECT_EQ(pdrs_ci.discardRectangleCount, 1);
    EXPECT_EQ(pdrs_ci.pDiscardRectangles[0].offset.x, 0);
    EXPECT_EQ(pdrs_ci.pDiscardRectangles[0].offset.y, 0);
    EXPECT_EQ(pdrs_ci.pDiscardRectangles[0].extent.width, 51);
    EXPECT_EQ(pdrs_ci.pDiscardRectangles[0].extent.height, 51);
    EXPECT_EQ(gp_ci.flags, 0);
    EXPECT_EQ(gp_ci.stageCount, 5);
    EXPECT_EQ(gp_ci.pStages[0].sType, VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO);
    EXPECT_EQ(pssrss_ci.sType, VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_REQUIRED_SUBGROUP_SIZE_CREATE_INFO);
    EXPECT_EQ(pssrss_ci.pNext, nullptr);
    EXPECT_EQ(pssrss_ci.requiredSubgroupSize, 64);
    EXPECT_EQ(gp_ci.pStages[0].flags, 0);
    EXPECT_EQ(gp_ci.pStages[0].stage, VK_SHADER_STAGE_VERTEX_BIT);
    EXPECT_EQ(gp_ci.pStages[0].module, reinterpret_cast<void*>(35));
    EXPECT_STREQ(gp_ci.pStages[0].pName, "main");
    EXPECT_EQ(gp_ci.pStages[0].pSpecializationInfo, nullptr);
    EXPECT_EQ(gp_ci.pStages[1].sType, VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO);
    EXPECT_EQ(gp_ci.pStages[1].pNext, nullptr);
    EXPECT_EQ(gp_ci.pStages[1].flags, 0);
    EXPECT_EQ(gp_ci.pStages[1].stage, VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT);
    EXPECT_EQ(gp_ci.pStages[1].module, reinterpret_cast<void*>(36));
    EXPECT_STREQ(gp_ci.pStages[1].pName, "main");
    EXPECT_EQ(gp_ci.pStages[1].pSpecializationInfo, nullptr);
    EXPECT_EQ(gp_ci.pStages[2].sType, VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO);
    EXPECT_EQ(gp_ci.pStages[2].pNext, nullptr);
    EXPECT_EQ(gp_ci.pStages[2].flags, 0);
    EXPECT_EQ(gp_ci.pStages[2].stage, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT);
    EXPECT_EQ(gp_ci.pStages[2].module, reinterpret_cast<void*>(37));
    EXPECT_STREQ(gp_ci.pStages[2].pName, "main");
    EXPECT_EQ(gp_ci.pStages[2].pSpecializationInfo, nullptr);
    EXPECT_EQ(gp_ci.pStages[3].sType, VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO);
    EXPECT_EQ(gp_ci.pStages[3].pNext, nullptr);
    EXPECT_EQ(gp_ci.pStages[3].flags, 0);
    EXPECT_EQ(gp_ci.pStages[3].stage, VK_SHADER_STAGE_GEOMETRY_BIT);
    EXPECT_EQ(gp_ci.pStages[3].module, reinterpret_cast<void*>(38));
    EXPECT_STREQ(gp_ci.pStages[3].pName, "main");
    EXPECT_EQ(gp_ci.pStages[3].pSpecializationInfo, nullptr);
    EXPECT_EQ(gp_ci.pStages[4].sType, VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO);
    EXPECT_EQ(gp_ci.pStages[4].pNext, nullptr);
    EXPECT_EQ(gp_ci.pStages[4].flags, 0);
    EXPECT_EQ(gp_ci.pStages[4].stage, VK_SHADER_STAGE_FRAGMENT_BIT);
    EXPECT_EQ(gp_ci.pStages[4].module, reinterpret_cast<void*>(39));
    EXPECT_STREQ(gp_ci.pStages[4].pName, "main");
    EXPECT_EQ(gp_ci.pStages[4].pSpecializationInfo, nullptr);
    EXPECT_EQ(pvis_ci.sType, VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO);
    EXPECT_EQ(pvis_ci.pNext, nullptr);
    EXPECT_EQ(pvis_ci.flags, 0);
    EXPECT_EQ(pvis_ci.vertexBindingDescriptionCount, 1);
    EXPECT_EQ(pvis_ci.pVertexBindingDescriptions[0].binding, 0);
    EXPECT_EQ(pvis_ci.pVertexBindingDescriptions[0].stride, 32);
    EXPECT_EQ(pvis_ci.pVertexBindingDescriptions[0].inputRate, VK_VERTEX_INPUT_RATE_VERTEX);
    EXPECT_EQ(pvis_ci.vertexAttributeDescriptionCount, 2);
    EXPECT_EQ(pvis_ci.pVertexAttributeDescriptions[0].location, 0);
    EXPECT_EQ(pvis_ci.pVertexAttributeDescriptions[0].binding, 0);
    EXPECT_EQ(pvis_ci.pVertexAttributeDescriptions[0].format, VK_FORMAT_R32G32B32A32_SFLOAT);
    EXPECT_EQ(pvis_ci.pVertexAttributeDescriptions[0].offset, 0);
    EXPECT_EQ(pvis_ci.pVertexAttributeDescriptions[1].location, 1);
    EXPECT_EQ(pvis_ci.pVertexAttributeDescriptions[1].binding, 0);
    EXPECT_EQ(pvis_ci.pVertexAttributeDescriptions[1].format, VK_FORMAT_R32G32B32A32_SFLOAT);
    EXPECT_EQ(pvis_ci.pVertexAttributeDescriptions[1].offset, 16);
    EXPECT_EQ(pias_ci.sType, VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO);
    EXPECT_EQ(pias_ci.pNext, nullptr);
    EXPECT_EQ(pias_ci.flags, 0);
    EXPECT_EQ(pias_ci.topology, VK_PRIMITIVE_TOPOLOGY_PATCH_LIST);
    EXPECT_EQ(pias_ci.primitiveRestartEnable, VK_FALSE);
    EXPECT_EQ(pts_ci.sType, VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO);
    EXPECT_EQ(pts_ci.pNext, nullptr);
    EXPECT_EQ(pts_ci.flags, 0);
    EXPECT_EQ(pts_ci.patchControlPoints, 4);
    EXPECT_EQ(pvs_ci.sType, VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO);
    EXPECT_EQ(pvs_ci.pNext, nullptr);
    EXPECT_EQ(pvs_ci.flags, 0);
    EXPECT_EQ(pvs_ci.viewportCount, 1);
    EXPECT_EQ(pvs_ci.pViewports[0].x, 0);
    EXPECT_EQ(pvs_ci.pViewports[0].y, 0);
    EXPECT_EQ(pvs_ci.pViewports[0].width, 51);
    EXPECT_EQ(pvs_ci.pViewports[0].height, 51);
    EXPECT_EQ(pvs_ci.pViewports[0].minDepth, 0);
    EXPECT_EQ(pvs_ci.pViewports[0].maxDepth, 1);
    EXPECT_EQ(pvs_ci.scissorCount, 1);
    EXPECT_EQ(pvs_ci.pScissors[0].offset.x, 0);
    EXPECT_EQ(pvs_ci.pScissors[0].offset.y, 0);
    EXPECT_EQ(pvs_ci.pScissors[0].extent.width, 51);
    EXPECT_EQ(pvs_ci.pScissors[0].extent.height, 51);
    EXPECT_EQ(prs_ci.sType, VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO);
    EXPECT_EQ(prs_ci.pNext, nullptr);
    EXPECT_EQ(prs_ci.flags, 0);
    EXPECT_EQ(prs_ci.depthClampEnable, VK_FALSE);
    EXPECT_EQ(prs_ci.rasterizerDiscardEnable, VK_FALSE);
    EXPECT_EQ(prs_ci.polygonMode, VK_POLYGON_MODE_FILL);
    EXPECT_EQ(prs_ci.cullMode, 0);
    EXPECT_EQ(prs_ci.frontFace, VK_FRONT_FACE_COUNTER_CLOCKWISE);
    EXPECT_EQ(prs_ci.depthBiasEnable, VK_FALSE);
    EXPECT_EQ(prs_ci.depthBiasConstantFactor, 0);
    EXPECT_EQ(prs_ci.depthBiasClamp, 0);
    EXPECT_EQ(prs_ci.depthBiasSlopeFactor, 0);
    EXPECT_EQ(prs_ci.lineWidth, 1);
    EXPECT_EQ(pmss_ci.sType, VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO);
    EXPECT_EQ(pmss_ci.pNext, nullptr);
    EXPECT_EQ(pmss_ci.flags, 0);
    EXPECT_EQ(pmss_ci.rasterizationSamples, VK_SAMPLE_COUNT_1_BIT);
    EXPECT_EQ(pmss_ci.sampleShadingEnable, VK_FALSE);
    EXPECT_EQ(pmss_ci.minSampleShading, 1);
    EXPECT_EQ(pmss_ci.pSampleMask, nullptr);
    EXPECT_EQ(pmss_ci.alphaToCoverageEnable, VK_FALSE);
    EXPECT_EQ(pmss_ci.alphaToOneEnable, VK_FALSE);
    EXPECT_EQ(pdss_ci.sType, VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO);
    EXPECT_EQ(pdss_ci.pNext, nullptr);
    EXPECT_EQ(pdss_ci.flags, 0);
    EXPECT_EQ(pdss_ci.depthTestEnable, VK_TRUE);
    EXPECT_EQ(pdss_ci.depthWriteEnable, VK_TRUE);
    EXPECT_EQ(pdss_ci.depthCompareOp, VK_COMPARE_OP_LESS_OR_EQUAL);
    EXPECT_EQ(pdss_ci.depthBoundsTestEnable, VK_FALSE);
    EXPECT_EQ(pdss_ci.stencilTestEnable, VK_FALSE);
    EXPECT_EQ(pdss_ci.front.failOp, VK_STENCIL_OP_KEEP);
    EXPECT_EQ(pdss_ci.front.passOp, VK_STENCIL_OP_KEEP);
    EXPECT_EQ(pdss_ci.front.depthFailOp, VK_STENCIL_OP_KEEP);
    EXPECT_EQ(pdss_ci.front.compareOp, VK_COMPARE_OP_NEVER);
    EXPECT_EQ(pdss_ci.front.compareMask, 0);
    EXPECT_EQ(pdss_ci.front.writeMask, 0);
    EXPECT_EQ(pdss_ci.front.reference, 0);
    EXPECT_EQ(pdss_ci.back.failOp, VK_STENCIL_OP_KEEP);
    EXPECT_EQ(pdss_ci.back.passOp, VK_STENCIL_OP_KEEP);
    EXPECT_EQ(pdss_ci.back.depthFailOp, VK_STENCIL_OP_KEEP);
    EXPECT_EQ(pdss_ci.back.compareOp, VK_COMPARE_OP_NEVER);
    EXPECT_EQ(pdss_ci.back.compareMask, 0);
    EXPECT_EQ(pdss_ci.back.writeMask, 0);
    EXPECT_EQ(pdss_ci.back.reference, 0);
    EXPECT_EQ(pdss_ci.minDepthBounds, 0);
    EXPECT_EQ(pdss_ci.maxDepthBounds, 1);
    EXPECT_EQ(pcbs_ci.sType, VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO);
    EXPECT_EQ(pcbs_ci.pNext, nullptr);
    EXPECT_EQ(pcbs_ci.flags, 0);
    EXPECT_EQ(pcbs_ci.logicOpEnable, VK_FALSE);
    EXPECT_EQ(pcbs_ci.logicOp, VK_LOGIC_OP_CLEAR);
    EXPECT_EQ(pcbs_ci.attachmentCount, 1);
    EXPECT_EQ(pcbs_ci.pAttachments[0].blendEnable, VK_FALSE);
    EXPECT_EQ(pcbs_ci.pAttachments[0].srcColorBlendFactor, VK_BLEND_FACTOR_ZERO);
    EXPECT_EQ(pcbs_ci.pAttachments[0].dstColorBlendFactor, VK_BLEND_FACTOR_ZERO);
    EXPECT_EQ(pcbs_ci.pAttachments[0].colorBlendOp, VK_BLEND_OP_ADD);
    EXPECT_EQ(pcbs_ci.pAttachments[0].srcAlphaBlendFactor, VK_BLEND_FACTOR_ZERO);
    EXPECT_EQ(pcbs_ci.pAttachments[0].dstAlphaBlendFactor, VK_BLEND_FACTOR_ZERO);
    EXPECT_EQ(pcbs_ci.pAttachments[0].alphaBlendOp, VK_BLEND_OP_ADD);
    EXPECT_EQ(pcbs_ci.pAttachments[0].colorWriteMask,
              VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);
    EXPECT_EQ(pcbs_ci.blendConstants[0], 0);
    EXPECT_EQ(pcbs_ci.blendConstants[1], 0);
    EXPECT_EQ(pcbs_ci.blendConstants[2], 0);
    EXPECT_EQ(pcbs_ci.blendConstants[3], 0);
    EXPECT_EQ(pds_ci.sType, VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO);
    EXPECT_EQ(pds_ci.pNext, nullptr);
    EXPECT_EQ(pds_ci.flags, 0);
    EXPECT_EQ(pds_ci.dynamicStateCount, 3);
    EXPECT_EQ(pds_ci.pDynamicStates[0], VK_DYNAMIC_STATE_VIEWPORT);
    EXPECT_EQ(pds_ci.pDynamicStates[1], VK_DYNAMIC_STATE_SCISSOR);
    EXPECT_EQ(pds_ci.pDynamicStates[2], VK_DYNAMIC_STATE_DISCARD_RECTANGLE_EXT);
    EXPECT_EQ(gp_ci.layout, reinterpret_cast<void*>(8));
    EXPECT_EQ(gp_ci.renderPass, reinterpret_cast<void*>(6));
    EXPECT_EQ(gp_ci.subpass, 0);
    EXPECT_EQ(gp_ci.basePipelineHandle, VK_NULL_HANDLE);
    EXPECT_EQ(gp_ci.basePipelineIndex, 0);

    EXPECT_EQ(rp_ci.sType, VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO);
    EXPECT_EQ(rp_ci.pNext, nullptr);
    EXPECT_EQ(rp_ci.flags, 0);
    EXPECT_EQ(rp_ci.attachmentCount, 2);
    EXPECT_EQ(rp_ci.pAttachments[0].flags, 0);
    EXPECT_EQ(rp_ci.pAttachments[0].format, VK_FORMAT_R8G8B8A8_UNORM);
    EXPECT_EQ(rp_ci.pAttachments[0].samples, VK_SAMPLE_COUNT_1_BIT);
    EXPECT_EQ(rp_ci.pAttachments[0].loadOp, VK_ATTACHMENT_LOAD_OP_CLEAR);
    EXPECT_EQ(rp_ci.pAttachments[0].storeOp, VK_ATTACHMENT_STORE_OP_STORE);
    EXPECT_EQ(rp_ci.pAttachments[0].stencilLoadOp, VK_ATTACHMENT_LOAD_OP_DONT_CARE);
    EXPECT_EQ(rp_ci.pAttachments[0].stencilStoreOp, VK_ATTACHMENT_STORE_OP_DONT_CARE);
    EXPECT_EQ(rp_ci.pAttachments[0].initialLayout, VK_IMAGE_LAYOUT_UNDEFINED);
    EXPECT_EQ(rp_ci.pAttachments[0].finalLayout, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    EXPECT_EQ(rp_ci.pAttachments[1].flags, 0);
    EXPECT_EQ(rp_ci.pAttachments[1].format, VK_FORMAT_D16_UNORM);
    EXPECT_EQ(rp_ci.pAttachments[1].samples, VK_SAMPLE_COUNT_1_BIT);
    EXPECT_EQ(rp_ci.pAttachments[1].loadOp, VK_ATTACHMENT_LOAD_OP_CLEAR);
    EXPECT_EQ(rp_ci.pAttachments[1].storeOp, VK_ATTACHMENT_STORE_OP_DONT_CARE);
    EXPECT_EQ(rp_ci.pAttachments[1].stencilLoadOp, VK_ATTACHMENT_LOAD_OP_DONT_CARE);
    EXPECT_EQ(rp_ci.pAttachments[1].stencilStoreOp, VK_ATTACHMENT_STORE_OP_DONT_CARE);
    EXPECT_EQ(rp_ci.pAttachments[1].initialLayout, VK_IMAGE_LAYOUT_UNDEFINED);
    EXPECT_EQ(rp_ci.pAttachments[1].finalLayout, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    EXPECT_EQ(rp_ci.subpassCount, 1);
    EXPECT_EQ(rp_ci.pSubpasses[0].flags, 0);
    EXPECT_EQ(rp_ci.pSubpasses[0].pipelineBindPoint, VK_PIPELINE_BIND_POINT_GRAPHICS);
    EXPECT_EQ(rp_ci.pSubpasses[0].inputAttachmentCount, 0);
    EXPECT_EQ(rp_ci.pSubpasses[0].pInputAttachments, nullptr);
    EXPECT_EQ(rp_ci.pSubpasses[0].colorAttachmentCount, 1);
    EXPECT_EQ(rp_ci.pSubpasses[0].pColorAttachments[0].attachment, 0);
    EXPECT_EQ(rp_ci.pSubpasses[0].pColorAttachments[0].layout, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    EXPECT_EQ(rp_ci.pSubpasses[0].pResolveAttachments, nullptr);
    EXPECT_EQ(rp_ci.pSubpasses[0].pDepthStencilAttachment[0].attachment, 1);
    EXPECT_EQ(rp_ci.pSubpasses[0].pDepthStencilAttachment[0].layout, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    EXPECT_EQ(rp_ci.pSubpasses[0].preserveAttachmentCount, 0);
    EXPECT_EQ(rp_ci.pSubpasses[0].pPreserveAttachments, nullptr);
    EXPECT_EQ(rp_ci.dependencyCount, 2);
    EXPECT_EQ(rp_ci.pDependencies[0].srcSubpass, VK_SUBPASS_EXTERNAL);
    EXPECT_EQ(rp_ci.pDependencies[0].dstSubpass, 0);
    EXPECT_EQ(rp_ci.pDependencies[0].srcStageMask,
              VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT);
    EXPECT_EQ(rp_ci.pDependencies[0].dstStageMask,
              VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT);
    EXPECT_EQ(rp_ci.pDependencies[0].srcAccessMask, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);
    EXPECT_EQ(rp_ci.pDependencies[0].dstAccessMask,
              VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);
    EXPECT_EQ(rp_ci.pDependencies[0].dependencyFlags, 0);
    EXPECT_EQ(rp_ci.pDependencies[1].srcSubpass, VK_SUBPASS_EXTERNAL);
    EXPECT_EQ(rp_ci.pDependencies[1].dstSubpass, 0);
    EXPECT_EQ(rp_ci.pDependencies[1].srcStageMask, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
    EXPECT_EQ(rp_ci.pDependencies[1].dstStageMask, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
    EXPECT_EQ(rp_ci.pDependencies[1].srcAccessMask, 0);
    EXPECT_EQ(rp_ci.pDependencies[1].dstAccessMask, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT);
    EXPECT_EQ(rp_ci.pDependencies[1].dependencyFlags, 0);

    EXPECT_EQ(pl_ci.sType, VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO);
    EXPECT_EQ(pl_ci.pNext, nullptr);
    EXPECT_EQ(pl_ci.flags, 0);
    EXPECT_EQ(pl_ci.setLayoutCount, 1);
    EXPECT_EQ(pl_ci.pSetLayouts[0], reinterpret_cast<void*>(0));
    EXPECT_EQ(pl_ci.pushConstantRangeCount, 1);
    EXPECT_EQ(pl_ci.pPushConstantRanges[0].stageFlags, VK_SHADER_STAGE_COMPUTE_BIT);
    EXPECT_EQ(pl_ci.pPushConstantRanges[0].offset, 0);
    EXPECT_EQ(pl_ci.pPushConstantRanges[0].size, 4);

    EXPECT_EQ(data.graphicsPipelineState.descriptorSetLayoutCount, 1);
    EXPECT_STREQ(dsl_names[0], "DescriptorSetLayout1");
    EXPECT_EQ(dsl_ci.sType, VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO);
    EXPECT_EQ(bf_ci.sType, VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO);
    EXPECT_EQ(bf_ci.pNext, nullptr);
    EXPECT_EQ(bf_ci.bindingCount, 1);
    EXPECT_EQ(bf_ci.pBindingFlags[0], VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT | VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT);
    EXPECT_EQ(dsl_ci.flags, VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT);
    EXPECT_EQ(dsl_ci.bindingCount, 3);
    EXPECT_EQ(dsl_ci.pBindings[0].binding, 0);
    EXPECT_EQ(dsl_ci.pBindings[0].descriptorType, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
    EXPECT_EQ(dsl_ci.pBindings[0].descriptorCount, 1);
    EXPECT_EQ(dsl_ci.pBindings[0].stageFlags, VK_SHADER_STAGE_COMPUTE_BIT);
    EXPECT_EQ(dsl_ci.pBindings[0].pImmutableSamplers, nullptr);
    EXPECT_EQ(dsl_ci.pBindings[1].binding, 1);
    EXPECT_EQ(dsl_ci.pBindings[1].descriptorType, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
    EXPECT_EQ((uintptr_t)dsl_ci.pBindings[1].pImmutableSamplers[0], 0);
    EXPECT_EQ(dsl_ci.pBindings[1].descriptorCount, 1);
    EXPECT_EQ(dsl_ci.pBindings[1].stageFlags, VK_SHADER_STAGE_FRAGMENT_BIT);
    EXPECT_EQ(dsl_ci.pBindings[2].binding, 2);
    EXPECT_EQ(dsl_ci.pBindings[2].descriptorType, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
    EXPECT_EQ((uintptr_t)dsl_ci.pBindings[2].pImmutableSamplers[0], 1);
    EXPECT_EQ(dsl_ci.pBindings[2].descriptorCount, 1);
    EXPECT_EQ(dsl_ci.pBindings[2].stageFlags, VK_SHADER_STAGE_FRAGMENT_BIT);

    EXPECT_EQ(data.graphicsPipelineState.immutableSamplerCount, 2);
    EXPECT_STREQ(sampler_names[0], "ImmutableSampler1");
    EXPECT_STREQ(sampler_names[1], "YcbcrSampler1");
    EXPECT_EQ(sampler_cis[0].sType, VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO);
    EXPECT_EQ(srm_ci.sType, VK_STRUCTURE_TYPE_SAMPLER_REDUCTION_MODE_CREATE_INFO);
    EXPECT_EQ(srm_ci.pNext, nullptr);
    EXPECT_EQ(srm_ci.reductionMode, VK_SAMPLER_REDUCTION_MODE_MAX);
    EXPECT_EQ(sampler_cis[0].addressModeU, VK_SAMPLER_ADDRESS_MODE_REPEAT);
    EXPECT_EQ(sampler_cis[0].addressModeV, VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT);
    EXPECT_EQ(sampler_cis[0].addressModeW, VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE);
    EXPECT_EQ(sampler_cis[0].anisotropyEnable, VK_FALSE);
    EXPECT_EQ(sampler_cis[0].borderColor, VK_BORDER_COLOR_FLOAT_CUSTOM_EXT);
    EXPECT_EQ(sampler_cis[0].compareEnable, VK_FALSE);
    EXPECT_EQ(sampler_cis[0].compareOp, VK_COMPARE_OP_NEVER);
    EXPECT_EQ(sampler_cis[0].flags, 0);
    EXPECT_EQ(sampler_cis[0].magFilter, VK_FILTER_CUBIC_EXT);
    EXPECT_EQ(sampler_cis[0].maxAnisotropy, 2.0);
    EXPECT_EQ(sampler_cis[0].maxLod, 1000.0);
    EXPECT_EQ(sampler_cis[0].minFilter, VK_FILTER_NEAREST);
    EXPECT_EQ(sampler_cis[0].minLod, 0.5);
    EXPECT_EQ(sampler_cis[0].mipLodBias, 0.5);
    EXPECT_EQ(sampler_cis[0].mipmapMode, VK_SAMPLER_MIPMAP_MODE_LINEAR);
    EXPECT_EQ(sampler_cis[0].unnormalizedCoordinates, VK_TRUE);
    EXPECT_EQ(sampler_cis[1].sType, VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO);
    EXPECT_EQ(ycbcrci.sType, VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_INFO);
    EXPECT_EQ(ycbcrci.pNext, nullptr);
    EXPECT_EQ((uintptr_t)ycbcrci.conversion, 0);
    EXPECT_EQ(sampler_cis[1].addressModeU, VK_SAMPLER_ADDRESS_MODE_REPEAT);
    EXPECT_EQ(sampler_cis[1].addressModeV, VK_SAMPLER_ADDRESS_MODE_REPEAT);
    EXPECT_EQ(sampler_cis[1].addressModeW, VK_SAMPLER_ADDRESS_MODE_REPEAT);
    EXPECT_EQ(sampler_cis[1].anisotropyEnable, VK_FALSE);
    EXPECT_EQ(sampler_cis[1].borderColor, VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK);
    EXPECT_EQ(sampler_cis[1].compareEnable, VK_FALSE);
    EXPECT_EQ(sampler_cis[1].compareOp, VK_COMPARE_OP_NEVER);
    EXPECT_EQ(sampler_cis[1].flags, 0);
    EXPECT_EQ(sampler_cis[1].magFilter, VK_FILTER_NEAREST);
    EXPECT_EQ(sampler_cis[1].maxAnisotropy, 0.0);
    EXPECT_EQ(sampler_cis[1].maxLod, 0.0);
    EXPECT_EQ(sampler_cis[1].minFilter, VK_FILTER_NEAREST);
    EXPECT_EQ(sampler_cis[1].minLod, 0.0);
    EXPECT_EQ(sampler_cis[1].mipLodBias, 0.0);
    EXPECT_EQ(sampler_cis[1].mipmapMode, VK_SAMPLER_MIPMAP_MODE_NEAREST);
    EXPECT_EQ(sampler_cis[1].unnormalizedCoordinates, VK_FALSE);

    EXPECT_EQ(data.graphicsPipelineState.ycbcrSamplerCount, 1);
    EXPECT_STREQ(ycbcr_names[0], "YcbcrConversion1");
    EXPECT_EQ(ycbcr_ci.sType, VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_CREATE_INFO);
#ifdef VK_USE_PLATFORM_SCREEN_QNX
    EXPECT_EQ(ef_qnx.sType, VK_STRUCTURE_TYPE_EXTERNAL_FORMAT_QNX);
    EXPECT_EQ(ef_qnx.pNext, nullptr);
    EXPECT_EQ(ef_qnx.externalFormat, 10);
#else
    EXPECT_EQ(ycbcr_ci.pNext, nullptr);
#endif  // VK_USE_PLATFORM_SCREEN_QNX
    EXPECT_EQ(ycbcr_ci.format, VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16);
    EXPECT_EQ(ycbcr_ci.ycbcrModel, VK_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_2020);
    EXPECT_EQ(ycbcr_ci.ycbcrRange, VK_SAMPLER_YCBCR_RANGE_ITU_NARROW);
    EXPECT_EQ(ycbcr_ci.components.r, VK_COMPONENT_SWIZZLE_A);
    EXPECT_EQ(ycbcr_ci.components.g, VK_COMPONENT_SWIZZLE_B);
    EXPECT_EQ(ycbcr_ci.components.b, VK_COMPONENT_SWIZZLE_G);
    EXPECT_EQ(ycbcr_ci.components.a, VK_COMPONENT_SWIZZLE_R);
    EXPECT_EQ(ycbcr_ci.xChromaOffset, VK_CHROMA_LOCATION_COSITED_EVEN);
    EXPECT_EQ(ycbcr_ci.yChromaOffset, VK_CHROMA_LOCATION_MIDPOINT);
    EXPECT_EQ(ycbcr_ci.chromaFilter, VK_FILTER_CUBIC_EXT);
    EXPECT_EQ(ycbcr_ci.forceExplicitReconstruction, VK_TRUE);

    EXPECT_EQ(data.graphicsPipelineState.shaderFileNameCount, 2);
    EXPECT_EQ(sfn[0].stage, VK_SHADER_STAGE_VERTEX_BIT);
    EXPECT_STREQ(sfn[0].pFilename, "shader.vert.spv");
    EXPECT_EQ(sfn[1].stage, VK_SHADER_STAGE_FRAGMENT_BIT);
    EXPECT_STREQ(sfn[1].pFilename, "shader.frag.spv");

    EXPECT_EQ(data.enabledExtensionCount, 1);
    EXPECT_STREQ(data.ppEnabledExtensions[0], "VK_EXT_robustness2");

    uint8_t expected_uuid[VK_UUID_SIZE] = {85, 43, 255, 24, 155, 64, 62, 24, 0, 0, 0, 0, 0, 0, 0, 0};
    EXPECT_UUIDEQ(data.pipelineUUID, expected_uuid);
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
