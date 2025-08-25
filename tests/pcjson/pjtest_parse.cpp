/*
 * Copyright (c) 2025 The Khronos Group Inc.
 * Copyright (c) 2025 RasterGrid Kft.
 * Copyright (c) 2021, NVIDIA CORPORATION
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <vulkan/pcjson/vksc_pipeline_json.h>
#include <vulkan/vulkan_sc.h>

#include <gtest/gtest.h>

#include <cassert>
#include <cstddef>
#include <iostream>
#include <cstring>
#include <vector>
#include <string>

class PJParseTest : public testing::Test {
  public:
    PJParseTest() : parser_{vpjCreateParser()}, msg_{nullptr} {}
    PJParseTest(const PJParseTest&) = delete;
    PJParseTest(PJParseTest&&) = default;
    ~PJParseTest() { vpjDestroyParser(parser_); }

    void TEST_DESCRIPTION(const char* desc) { RecordProperty("description", desc); }
    void CHECK_PARSE(bool success) {
        EXPECT_TRUE(success);
        if (msg_ && std::strlen(msg_)) {
            FAIL() << msg_;
        }
    }

  protected:
    VpjParser parser_;
    const char* msg_;
};

TEST_F(PJParseTest, BasicTypesVkBool32) {
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
        if (msg && std::strlen(msg)) {
            std::cerr << msg << std::endl;
        }
        EXPECT_EQ(data.features.robustBufferAccess, expect);
    };

    test_eq(R"("VK_TRUE")", VK_TRUE);
    test_eq(R"(0)", 0);
    test_eq(R"("VK_FALSE")", VK_FALSE);
    test_eq(R"(4294967295)", 4294967295);
}

TEST_F(PJParseTest, VkGraphicsPipelineCreateInfo) {
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
                "pNext":"NULL",
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
            "pDynamicState": 
            "NULL",
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
    EXPECT_EQ(gp_ci.pStages[0].pNext, nullptr);
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
    EXPECT_EQ(gp_ci.pDynamicState, nullptr);
    EXPECT_EQ(gp_ci.layout, reinterpret_cast<void*>(8));
    EXPECT_EQ(gp_ci.renderPass, reinterpret_cast<void*>(6));
    EXPECT_EQ(gp_ci.subpass, 0);
    EXPECT_EQ(gp_ci.basePipelineHandle, VK_NULL_HANDLE);
    EXPECT_EQ(gp_ci.basePipelineIndex, 0);
}

TEST_F(PJParseTest, VkComputePipelineCreateInfo) {
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

TEST_F(PJParseTest, SAXPY) {
    TEST_DESCRIPTION("Tests parsing of a reasonably simple compute pipeline JSON");

    const std::string json{R"({
        "ComputePipelineState" :
        {
            "DescriptorSetLayouts" :
            [
                {
                    "5":
                    {
                        "sType" : "VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO",
                        "pNext":"NULL",
                        "flags" : "0",
                        "bindingCount" : 2,
                        "pBindings":
                        [
                        {
                            "binding" : 0,
                            "descriptorType" : "VK_DESCRIPTOR_TYPE_STORAGE_BUFFER",
                            "descriptorCount" : 1,
                            "stageFlags" : "VK_SHADER_STAGE_COMPUTE_BIT",
                            "pImmutableSamplers": "NULL"
                        },
                        {
                            "binding" : 1,
                            "descriptorType" : "VK_DESCRIPTOR_TYPE_STORAGE_BUFFER",
                            "descriptorCount" : 1,
                            "stageFlags" : "VK_SHADER_STAGE_COMPUTE_BIT",
                            "pImmutableSamplers": "NULL"
                        }
                        ]
                    }
                }
            ],
            "PipelineLayout" :
            {
                "sType" : "VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO",
                "pNext":"NULL",
                "flags" : 0,
                "setLayoutCount" : 1,
                "pSetLayouts":
                [
                    5
                ],
                "pushConstantRangeCount" : 1,
                "pPushConstantRanges": [
                    {
                        "stageFlags": "VK_SHADER_STAGE_COMPUTE_BIT",
                        "offset": 0,
                        "size": 4
                    }
                ]
            },
            "ComputePipeline" :
            {
                "sType" : "VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO",
                "pNext": "NULL",
                "flags" : "0",
                "stage":
                {
                    "sType" : "VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO",
                    "pNext": "NULL",
                    "flags" : "0",
                    "stage" : "VK_SHADER_STAGE_COMPUTE_BIT",
                    "pName" : "main",
                    "pSpecializationInfo": "NULL",
                    "module": "NULL"
                },
                "layout" : 9,
                "basePipelineHandle" : "NULL",
                "basePipelineIndex" : 0
            },
            "ShaderFileNames" :
            [
                {
                    "stage" : "VK_SHADER_STAGE_COMPUTE_BIT",
                    "filename" : "saxpy.comp.spv"
                }
            ],
            "PhysicalDeviceFeatures" :
            {
                "sType" : "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2",
                "pNext":
                {
                    "sType" : "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES",
                    "pNext": "NULL",
                    "synchronization2" : "VK_TRUE"
                },
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
        "EnabledExtensions" :
        [
            "VK_KHR_synchronization2"
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

    VpjData data;
    const char* msg = nullptr;
    EXPECT_TRUE(vpjParsePipelineJson(this->parser_, json.c_str(), &data, &msg));
    if (msg && std::strlen(msg) != 0) {
        std::cerr << msg << std::endl;
    }
    // Check sufficiently deep, non-trivial entry
    EXPECT_EQ(
        reinterpret_cast<const VkDescriptorSetLayoutCreateInfo*>(data.computePipelineState.pDescriptorSetLayouts)[0].bindingCount,
        2);
    // Check ds layout name rewritten to index
    EXPECT_EQ(
        (size_t) reinterpret_cast<const VkPipelineLayoutCreateInfo*>(data.computePipelineState.pPipelineLayout)->pSetLayouts[0], 0);
    // Check original name of ds layout is preserved
    EXPECT_EQ(std::string_view{data.computePipelineState.ppDescriptorSetLayoutNames[0]}, "5");
}
