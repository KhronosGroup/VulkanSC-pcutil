/*
 * Copyright (c) 2025 The Khronos Group Inc.
 * Copyright (c) 2025 RasterGrid Kft.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <vulkan/pcjson/vksc_pipeline_json.h>
#include <vulkan/utility/vk_struct_helper.hpp>
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

    std::string ref_json = {R"({
        "sType": "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2",
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
)"
                            R"(
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

    auto sblf = vku::InitStruct<VkPhysicalDeviceScalarBlockLayoutFeatures>();
    sblf.scalarBlockLayout = VK_TRUE;

    auto vk11f = vku::InitStruct<VkPhysicalDeviceVulkan11Features>(&sblf);
    vk11f.samplerYcbcrConversion = VK_TRUE;

    auto pdf = vku::InitStruct<VkPhysicalDeviceFeatures2>(&vk11f);
    pdf.features.fullDrawIndexUint32 = VK_TRUE;
    pdf.features.independentBlend = VK_TRUE;
    pdf.features.tessellationShader = VK_TRUE;
    pdf.features.dualSrcBlend = VK_TRUE;
    pdf.features.multiDrawIndirect = VK_TRUE;
    pdf.features.depthBiasClamp = VK_TRUE;
    pdf.features.depthBounds = VK_TRUE;
    pdf.features.largePoints = VK_TRUE;
    pdf.features.multiViewport = VK_TRUE;
    pdf.features.textureCompressionETC2 = VK_TRUE;
    pdf.features.textureCompressionBC = VK_TRUE;
    pdf.features.pipelineStatisticsQuery = VK_TRUE;
    pdf.features.shaderTessellationAndGeometryPointSize = VK_TRUE;
    pdf.features.shaderStorageImageExtendedFormats = VK_TRUE;
    pdf.features.shaderStorageImageReadWithoutFormat = VK_TRUE;
    pdf.features.shaderUniformBufferArrayDynamicIndexing = VK_TRUE;
    pdf.features.shaderStorageBufferArrayDynamicIndexing = VK_TRUE;
    pdf.features.shaderClipDistance = VK_TRUE;
    pdf.features.shaderFloat64 = VK_TRUE;
    pdf.features.shaderInt16 = VK_TRUE;
    pdf.features.shaderResourceMinLod = VK_TRUE;
    pdf.features.sparseResidencyBuffer = VK_TRUE;
    pdf.features.sparseResidencyImage3D = VK_TRUE;
    pdf.features.sparseResidency4Samples = VK_TRUE;
    pdf.features.sparseResidency16Samples = VK_TRUE;
    pdf.features.variableMultisampleRate = VK_TRUE;

    const char* result_json = nullptr;

    EXPECT_TRUE(vpjGenerateSingleStructJson(generator_, &pdf, &result_json, &msg_));
    CHECK_GEN();
    EXPECT_TRUE(CompareJson(result_json, ref_json));
}

TEST_F(Gen, VkGraphicsPipelineCreateInfo) {
    TEST_DESCRIPTION("Tests generating of a reasonably complex graphics pipeline create info JSON");

    const char* ref_json = {R"({
        "sType" : "VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO",
        "pNext": {
            "sType": "VK_STRUCTURE_TYPE_PIPELINE_DISCARD_RECTANGLE_STATE_CREATE_INFO_EXT",
            "pNext" : "NULL",
            "flags": 0,
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
        "flags" : 0,
        "stageCount" : 5,
        "pStages": 
        [
        {
            "sType" : "VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO",
            "pNext":"NULL",
            "flags" : 0,
            "stage" : "VK_SHADER_STAGE_VERTEX_BIT",
            "module" : 35,
            "pName" : "main",
            "pSpecializationInfo": 
            "NULL"
        },
        {
            "sType" : "VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO",
            "pNext":"NULL",
            "flags" : 0,
            "stage" : "VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT",
            "module" : 36,
            "pName" : "main",
            "pSpecializationInfo": 
            "NULL"
        },
        {
            "sType" : "VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO",
            "pNext":"NULL",
            "flags" : 0,
            "stage" : "VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT",
            "module" : 37,
            "pName" : "main",
            "pSpecializationInfo": 
            "NULL"
        },
        {
            "sType" : "VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO",
            "pNext":"NULL",
            "flags" : 0,
            "stage" : "VK_SHADER_STAGE_GEOMETRY_BIT",
            "module" : 38,
            "pName" : "main",
            "pSpecializationInfo": 
            "NULL"
        },
        {
            "sType" : "VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO",
            "pNext":"NULL",
            "flags" : 0,
            "stage" : "VK_SHADER_STAGE_FRAGMENT_BIT",
            "module" : 39,
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
)"
                            R"(
        "pViewportState": 
        {
            "sType" : "VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO",
            "pNext":"NULL",
            "flags" : 0,
            "viewportCount" : 1,
            "pViewports": 
            [
            {
                "x" : 0.0,
                "y" : 0.0,
                "width" : 51.0,
                "height" : 51.0,
                "minDepth" : 0.0,
                "maxDepth" : 1.0
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
            "cullMode" : 0,
            "frontFace" : "VK_FRONT_FACE_COUNTER_CLOCKWISE",
            "depthBiasEnable" : "VK_FALSE",
            "depthBiasConstantFactor" : 0.0,
            "depthBiasClamp" : 0.0,
            "depthBiasSlopeFactor" : 0.0,
            "lineWidth" : 1.0
        },
        "pMultisampleState": 
        {
            "sType" : "VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO",
            "pNext":"NULL",
            "flags" : 0,
            "rasterizationSamples" : "VK_SAMPLE_COUNT_1_BIT",
            "sampleShadingEnable" : "VK_FALSE",
            "minSampleShading" : 1.0,
            "pSampleMask":
            "NULL",
            "alphaToCoverageEnable" : "VK_FALSE",
            "alphaToOneEnable" : "VK_FALSE"
        },
)"
                            R"(
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
            "minDepthBounds" : 0.0,
            "maxDepthBounds" : 1.0
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
            0.0,
            0.0,
            0.0,
            0.0
            ]
        },
        "pDynamicState": "NULL",
        "layout" : 8,
        "renderPass" : 6,
        "subpass" : 0,
        "basePipelineHandle" : "",
        "basePipelineIndex" : 0
    })"};

    VkGraphicsPipelineCreateInfo gp_ci{};

    VkPipelineDiscardRectangleStateCreateInfoEXT pdrs_ci{};
    VkPipelineVertexInputStateCreateInfo pvis_ci{};
    VkPipelineInputAssemblyStateCreateInfo pias_ci{};
    VkPipelineTessellationStateCreateInfo pts_ci{};
    VkPipelineViewportStateCreateInfo pvs_ci{};
    VkPipelineRasterizationStateCreateInfo prs_ci{};
    VkPipelineMultisampleStateCreateInfo pmss_ci{};
    VkPipelineDepthStencilStateCreateInfo pdss_ci{};
    VkPipelineColorBlendStateCreateInfo pcbs_ci{};

    gp_ci.pNext = &pdrs_ci;
    gp_ci.pVertexInputState = &pvis_ci;
    gp_ci.pInputAssemblyState = &pias_ci;
    gp_ci.pTessellationState = &pts_ci;
    gp_ci.pViewportState = &pvs_ci;
    gp_ci.pRasterizationState = &prs_ci;
    gp_ci.pMultisampleState = &pmss_ci;
    gp_ci.pDepthStencilState = &pdss_ci;
    gp_ci.pColorBlendState = &pcbs_ci;

    gp_ci.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pdrs_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_DISCARD_RECTANGLE_STATE_CREATE_INFO_EXT;
    pdrs_ci.pNext = nullptr;
    pdrs_ci.flags = 0;
    pdrs_ci.discardRectangleMode = VK_DISCARD_RECTANGLE_MODE_EXCLUSIVE_EXT;

    VkRect2D discardRectangles[1] = {};
    pdrs_ci.discardRectangleCount = 1;
    pdrs_ci.pDiscardRectangles = discardRectangles;
    discardRectangles[0].offset.y = 0;
    discardRectangles[0].extent.width = 51;
    discardRectangles[0].extent.height = 51;
    discardRectangles[0].offset.x = 0;

    gp_ci.flags = 0;

    VkPipelineShaderStageCreateInfo stages[5] = {};
    gp_ci.stageCount = 5;
    gp_ci.pStages = stages;

    stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stages[0].pNext = nullptr;
    stages[0].flags = 0;
    stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    stages[0].module = reinterpret_cast<VkShaderModule>(35);
    stages[0].pName = "main";
    stages[0].pSpecializationInfo = nullptr;
    stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stages[1].pNext = nullptr;
    stages[1].flags = 0;
    stages[1].stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
    stages[1].module = reinterpret_cast<VkShaderModule>(36);
    stages[1].pName = "main";
    stages[1].pSpecializationInfo = nullptr;
    stages[2].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stages[2].pNext = nullptr;
    stages[2].flags = 0;
    stages[2].stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
    stages[2].module = reinterpret_cast<VkShaderModule>(37);
    stages[2].pName = "main";
    stages[2].pSpecializationInfo = nullptr;
    stages[3].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stages[3].pNext = nullptr;
    stages[3].flags = 0;
    stages[3].stage = VK_SHADER_STAGE_GEOMETRY_BIT;
    stages[3].module = reinterpret_cast<VkShaderModule>(38);
    stages[3].pName = "main";
    stages[3].pSpecializationInfo = nullptr;
    stages[4].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stages[4].pNext = nullptr;
    stages[4].flags = 0;
    stages[4].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    stages[4].module = reinterpret_cast<VkShaderModule>(39);
    stages[4].pName = "main";
    stages[4].pSpecializationInfo = nullptr;

    pvis_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    pvis_ci.pNext = nullptr;
    pvis_ci.flags = 0;

    VkVertexInputBindingDescription vertex_binding_desc[1] = {};
    pvis_ci.vertexBindingDescriptionCount = 1;
    pvis_ci.pVertexBindingDescriptions = vertex_binding_desc;
    vertex_binding_desc[0].stride = 32;
    vertex_binding_desc[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    vertex_binding_desc[0].binding = 0;

    VkVertexInputAttributeDescription vertex_attrib_desc[2] = {};
    pvis_ci.vertexAttributeDescriptionCount = 2;
    pvis_ci.pVertexAttributeDescriptions = vertex_attrib_desc;
    vertex_attrib_desc[0].location = 0;
    vertex_attrib_desc[0].binding = 0;
    vertex_attrib_desc[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    vertex_attrib_desc[0].offset = 0;
    vertex_attrib_desc[1].location = 1;
    vertex_attrib_desc[1].binding = 0;
    vertex_attrib_desc[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    vertex_attrib_desc[1].offset = 16;

    pias_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    pias_ci.pNext = nullptr;
    pias_ci.flags = 0;
    pias_ci.topology = VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
    pias_ci.primitiveRestartEnable = VK_FALSE;
    pts_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
    pts_ci.pNext = nullptr;
    pts_ci.flags = 0;
    pts_ci.patchControlPoints = 4;
    pvs_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    pvs_ci.pNext = nullptr;
    pvs_ci.flags = 0;

    VkViewport viewports[1] = {};
    pvs_ci.viewportCount = 1;
    pvs_ci.pViewports = viewports;
    viewports[0].y = 0;
    viewports[0].width = 51;
    viewports[0].height = 51;
    viewports[0].x = 0;
    viewports[0].minDepth = 0;
    viewports[0].maxDepth = 1;

    VkRect2D scissors[1] = {};
    pvs_ci.scissorCount = 1;
    pvs_ci.pScissors = scissors;
    scissors[0].offset.x = 0;
    scissors[0].offset.y = 0;
    scissors[0].extent.width = 51;
    scissors[0].extent.height = 51;

    prs_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    prs_ci.pNext = nullptr;
    prs_ci.flags = 0;
    prs_ci.depthClampEnable = VK_FALSE;
    prs_ci.rasterizerDiscardEnable = VK_FALSE;
    prs_ci.polygonMode = VK_POLYGON_MODE_FILL;
    prs_ci.cullMode = 0;
    prs_ci.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    prs_ci.depthBiasEnable = VK_FALSE;
    prs_ci.depthBiasConstantFactor = 0;
    prs_ci.depthBiasClamp = 0;
    prs_ci.depthBiasSlopeFactor = 0;
    prs_ci.lineWidth = 1;
    pmss_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    pmss_ci.pNext = nullptr;
    pmss_ci.flags = 0;
    pmss_ci.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    pmss_ci.sampleShadingEnable = VK_FALSE;
    pmss_ci.minSampleShading = 1;
    pmss_ci.pSampleMask = nullptr;
    pmss_ci.alphaToCoverageEnable = VK_FALSE;
    pmss_ci.alphaToOneEnable = VK_FALSE;
    pdss_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    pdss_ci.pNext = nullptr;
    pdss_ci.flags = 0;
    pdss_ci.depthTestEnable = VK_TRUE;
    pdss_ci.depthWriteEnable = VK_TRUE;
    pdss_ci.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    pdss_ci.depthBoundsTestEnable = VK_FALSE;
    pdss_ci.stencilTestEnable = VK_FALSE;
    pdss_ci.front.failOp = VK_STENCIL_OP_KEEP;
    pdss_ci.front.passOp = VK_STENCIL_OP_KEEP;
    pdss_ci.front.depthFailOp = VK_STENCIL_OP_KEEP;
    pdss_ci.front.compareOp = VK_COMPARE_OP_NEVER;
    pdss_ci.front.compareMask = 0;
    pdss_ci.front.writeMask = 0;
    pdss_ci.front.reference = 0;
    pdss_ci.back.failOp = VK_STENCIL_OP_KEEP;
    pdss_ci.back.passOp = VK_STENCIL_OP_KEEP;
    pdss_ci.back.depthFailOp = VK_STENCIL_OP_KEEP;
    pdss_ci.back.compareOp = VK_COMPARE_OP_NEVER;
    pdss_ci.back.compareMask = 0;
    pdss_ci.back.writeMask = 0;
    pdss_ci.back.reference = 0;
    pdss_ci.minDepthBounds = 0;
    pdss_ci.maxDepthBounds = 1;
    pcbs_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    pcbs_ci.pNext = nullptr;
    pcbs_ci.flags = 0;
    pcbs_ci.logicOpEnable = VK_FALSE;
    pcbs_ci.logicOp = VK_LOGIC_OP_CLEAR;

    VkPipelineColorBlendAttachmentState attachments[1] = {};
    pcbs_ci.attachmentCount = 1;
    pcbs_ci.pAttachments = attachments;
    attachments[0].blendEnable = VK_FALSE;
    attachments[0].srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    attachments[0].dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    attachments[0].colorBlendOp = VK_BLEND_OP_ADD;
    attachments[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    attachments[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    attachments[0].alphaBlendOp = VK_BLEND_OP_ADD;
    attachments[0].colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    pcbs_ci.blendConstants[0] = 0;
    pcbs_ci.blendConstants[1] = 0;
    pcbs_ci.blendConstants[2] = 0;
    pcbs_ci.blendConstants[3] = 0;
    gp_ci.pDynamicState = nullptr;
    gp_ci.layout = reinterpret_cast<VkPipelineLayout>(8);
    gp_ci.renderPass = reinterpret_cast<VkRenderPass>(6);
    gp_ci.subpass = 0;
    gp_ci.basePipelineHandle = VK_NULL_HANDLE;
    gp_ci.basePipelineIndex = 0;

    const char* result_json = nullptr;

    EXPECT_TRUE(vpjGenerateSingleStructJson(generator_, &gp_ci, &result_json, &msg_));
    CHECK_GEN();
    EXPECT_TRUE(CompareJson(result_json, ref_json));
}

TEST_F(Gen, VkComputePipelineCreateInfo) {
    TEST_DESCRIPTION("Tests generating of a reasonably complex compute pipeline create info JSON");

    std::string ref_json = {R"({
        "sType" : "VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO",
        "pNext": "NULL",
        "flags" : 0,
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
                "pData": "AAECAw=="
            },
            "module": ""
        },
        "layout" : 9,
        "basePipelineHandle" : "",
        "basePipelineIndex" : 0
    })"};

    VkComputePipelineCreateInfo cp_ci{};
    cp_ci.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    cp_ci.pNext = nullptr;
    cp_ci.flags = 0;
    cp_ci.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;

    VkPipelineShaderStageRequiredSubgroupSizeCreateInfo pssrss_ci{};
    pssrss_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_REQUIRED_SUBGROUP_SIZE_CREATE_INFO;
    pssrss_ci.pNext = nullptr;
    pssrss_ci.requiredSubgroupSize = 64;
    cp_ci.stage.pNext = &pssrss_ci;

    cp_ci.stage.flags = VK_PIPELINE_SHADER_STAGE_CREATE_REQUIRE_FULL_SUBGROUPS_BIT;
    cp_ci.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    cp_ci.stage.pName = "main";

    VkSpecializationInfo specializationInfo{};

    VkSpecializationMapEntry specializationMapEntries[1] = {};
    specializationInfo.mapEntryCount = 1;

    specializationMapEntries[0].constantID = 0;
    specializationMapEntries[0].offset = 0;
    specializationMapEntries[0].size = 4;
    specializationInfo.pMapEntries = specializationMapEntries;

    uint8_t specializationData[4] = {0, 1, 2, 3};
    specializationInfo.dataSize = 4;

    specializationInfo.pData = specializationData;

    cp_ci.stage.pSpecializationInfo = &specializationInfo;
    cp_ci.stage.module = VK_NULL_HANDLE;
    cp_ci.layout = reinterpret_cast<VkPipelineLayout>(9);
    cp_ci.basePipelineHandle = VK_NULL_HANDLE;
    cp_ci.basePipelineIndex = 0;

    const char* result_json = nullptr;

    EXPECT_TRUE(vpjGenerateSingleStructJson(generator_, &cp_ci, &result_json, &msg_));
    CHECK_GEN();
    EXPECT_TRUE(CompareJson(result_json, ref_json));
}

TEST_F(Gen, VkSamplerYcbcrConversionCreateInfo) {
    TEST_DESCRIPTION("Tests generating of a reasonably complex ycbcr conversion create info JSON");

#ifdef VK_USE_PLATFORM_SCREEN_QNX
    std::string ycbcr_ci_pnext = R"({
            "sType" : "VK_STRUCTURE_TYPE_EXTERNAL_FORMAT_QNX",
            "pNext": "NULL",
            "externalFormat": 10
    })";
#else
    std::string ycbcr_ci_pnext = R"("NULL")";
#endif  // VK_USE_PLATFORM_SCREEN_QNX

    std::string ref_json = {R"({
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

    VkSamplerYcbcrConversionCreateInfo ycbcr_ci;
    ycbcr_ci.sType = VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_CREATE_INFO;
#ifdef VK_USE_PLATFORM_SCREEN_QNX
    VkExternalFormatQNX ef_qnx{};
    ef_qnx.sType = VK_STRUCTURE_TYPE_EXTERNAL_FORMAT_QNX;
    ef_qnx.pNext = nullptr;
    ef_qnx.externalFormat = 10;
    ycbcr_ci.pNext = &ef_qnx;
#else
    ycbcr_ci.pNext = nullptr;
#endif  // VK_USE_PLATFORM_SCREEN_QNX
    ycbcr_ci.format = VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16;
    ycbcr_ci.ycbcrModel = VK_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_2020;
    ycbcr_ci.ycbcrRange = VK_SAMPLER_YCBCR_RANGE_ITU_NARROW;
    ycbcr_ci.components.r = VK_COMPONENT_SWIZZLE_A;
    ycbcr_ci.components.g = VK_COMPONENT_SWIZZLE_B;
    ycbcr_ci.components.b = VK_COMPONENT_SWIZZLE_G;
    ycbcr_ci.components.a = VK_COMPONENT_SWIZZLE_R;
    ycbcr_ci.xChromaOffset = VK_CHROMA_LOCATION_COSITED_EVEN;
    ycbcr_ci.yChromaOffset = VK_CHROMA_LOCATION_MIDPOINT;
    ycbcr_ci.chromaFilter = VK_FILTER_CUBIC_EXT;
    ycbcr_ci.forceExplicitReconstruction = VK_TRUE;

    const char* result_json = nullptr;

    EXPECT_TRUE(vpjGenerateSingleStructJson(generator_, &ycbcr_ci, &result_json, &msg_));
    CHECK_GEN();
    EXPECT_TRUE(CompareJson(result_json, ref_json));
}

TEST_F(Gen, VkSamplerCreateInfo) {
    TEST_DESCRIPTION("Tests generating of a reasonably complex sampler create info JSON");

    std::string ref_json = {R"({
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
        "mipLodBias": 0.5,
        "anisotropyEnable": "VK_FALSE",
        "maxAnisotropy": 2.0,
        "compareEnable": "VK_FALSE",
        "compareOp": 137,
        "minLod": "NaN",
        "maxLod": 1000.0,
        "borderColor": "VK_BORDER_COLOR_FLOAT_CUSTOM_EXT",
        "unnormalizedCoordinates": "VK_TRUE"
    })"};

    VkSamplerCreateInfo sampler_ci{};
    VkSamplerReductionModeCreateInfo srm_ci{};

    sampler_ci.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

    srm_ci.sType = VK_STRUCTURE_TYPE_SAMPLER_REDUCTION_MODE_CREATE_INFO;
    srm_ci.pNext = nullptr;
    srm_ci.reductionMode = VK_SAMPLER_REDUCTION_MODE_MAX;

    sampler_ci.pNext = &srm_ci;
    sampler_ci.flags = 0;
    sampler_ci.magFilter = VK_FILTER_CUBIC_EXT;
    sampler_ci.minFilter = VK_FILTER_NEAREST;
    sampler_ci.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_ci.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_ci.addressModeV = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    sampler_ci.addressModeW = VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
    sampler_ci.mipLodBias = 0.5f;
    sampler_ci.anisotropyEnable = VK_FALSE;
    sampler_ci.maxAnisotropy = 2.0f;
    sampler_ci.compareEnable = VK_FALSE;
    sampler_ci.compareOp = static_cast<VkCompareOp>(137);
    sampler_ci.minLod = std::numeric_limits<float>::quiet_NaN();
    sampler_ci.maxLod = VK_LOD_CLAMP_NONE;  // NOTE: float constants can't be serialized as strings
    sampler_ci.borderColor = VK_BORDER_COLOR_FLOAT_CUSTOM_EXT;
    sampler_ci.unnormalizedCoordinates = VK_TRUE;

    const char* result_json = nullptr;

    EXPECT_TRUE(vpjGenerateSingleStructJson(generator_, &sampler_ci, &result_json, &msg_));
    CHECK_GEN(true);
    EXPECT_TRUE(CompareJson(result_json, ref_json));
}

TEST_F(Gen, VkDescriptorSetLayoutCreateInfo) {
    TEST_DESCRIPTION("Tests generating of a reasonably complex descriptor set layout create info JSON");

    std::string ref_json = {R"({
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

    VkDescriptorSetLayoutCreateInfo dsl_ci{};
    dsl_ci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;

    VkDescriptorSetLayoutBindingFlagsCreateInfo dslbf_ci{};
    dslbf_ci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
    dslbf_ci.pNext = nullptr;
    dslbf_ci.bindingCount = 1;
    dsl_ci.pNext = &dslbf_ci;

    VkDescriptorBindingFlags bindingFlags[1] = {};
    bindingFlags[0] = VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT | VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT;
    dslbf_ci.pBindingFlags = bindingFlags;

    dsl_ci.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;
    dsl_ci.bindingCount = 1;

    VkDescriptorSetLayoutBinding bindings[1] = {};
    bindings[0].binding = 12345;
    bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
    bindings[0].descriptorCount = 1;
    bindings[0].stageFlags = VK_SHADER_STAGE_ALL;
    bindings[0].pImmutableSamplers = nullptr;

    dsl_ci.pBindings = bindings;

    const char* result_json = nullptr;

    EXPECT_TRUE(vpjGenerateSingleStructJson(generator_, &dsl_ci, &result_json, &msg_));
    CHECK_GEN();
    EXPECT_TRUE(CompareJson(result_json, ref_json));
}

TEST_F(Gen, VkPipelineLayoutCreateInfo) {
    TEST_DESCRIPTION("Tests generating of a reasonably complex pipeline layout create info JSON");

    std::string ref_json = {R"({
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
                "stageFlags": "VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_COMPUTE_BIT",
                "offset": 0,
                "size": 8
            }
        ]
    })"};

    VkPipelineLayoutCreateInfo pl_ci{};
    pl_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pl_ci.pNext = nullptr;
    pl_ci.flags = 0;

    VkDescriptorSetLayout setLayouts[1] = {};
    pl_ci.setLayoutCount = 1;
    setLayouts[0] = VkDescriptorSetLayout(54321);
    pl_ci.pSetLayouts = setLayouts;

    VkPushConstantRange pushConstantRanges[1] = {};
    pl_ci.pushConstantRangeCount = 1;
    pushConstantRanges[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT | VK_SHADER_STAGE_GEOMETRY_BIT;
    pushConstantRanges[0].offset = 0;
    pushConstantRanges[0].size = 8;
    pl_ci.pPushConstantRanges = pushConstantRanges;

    const char* result_json = nullptr;

    EXPECT_TRUE(vpjGenerateSingleStructJson(generator_, &pl_ci, &result_json, &msg_));
    CHECK_GEN();
    EXPECT_TRUE(CompareJson(result_json, ref_json));
}

TEST_F(Gen, VkRenderPassCreateInfo) {
    TEST_DESCRIPTION("Tests generating of a reasonably complex render pass create info JSON");

    std::string ref_json = {R"({
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
                        "attachment": 4294967295,
                        "layout": "VK_IMAGE_LAYOUT_UNDEFINED"
                    }
                ],
                "pResolveAttachments": [
                    {
                        "attachment": 4294967295,
                        "layout": "VK_IMAGE_LAYOUT_UNDEFINED"
                    }
                ],
                "pDepthStencilAttachment": {
                    "attachment": 4294967295,
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
                        "layout": "VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL"
                    }
                ],
                "pResolveAttachments": [
                    {
                        "attachment": 4294967295,
                        "layout": "VK_IMAGE_LAYOUT_UNDEFINED"
                    }
                ],
                "pDepthStencilAttachment": {
                    "attachment": 4294967295,
                    "layout": "VK_IMAGE_LAYOUT_UNDEFINED"
                },
                "preserveAttachmentCount": 0,
                "pPreserveAttachments": "NULL"
            }
        ],
        "dependencyCount": 1,
        "pDependencies": [
            {
                "srcSubpass": 4294967295,
                "dstSubpass": 2345,
                "srcStageMask": 0,
                "dstStageMask": "VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT",
                "srcAccessMask": 0,
                "dstAccessMask": "VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT",
                "dependencyFlags": "VK_DEPENDENCY_DEVICE_GROUP_BIT"
            }
        ]
    })"};

    VkRenderPassCreateInfo rp_ci{};
    VkRenderPassInputAttachmentAspectCreateInfo rpiaa_ci{};
    VkRenderPassMultiviewCreateInfo rpmv_ci{};

    rp_ci.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

    rpiaa_ci.sType = VK_STRUCTURE_TYPE_RENDER_PASS_INPUT_ATTACHMENT_ASPECT_CREATE_INFO;
    rpiaa_ci.aspectReferenceCount = 1;

    VkInputAttachmentAspectReference aspectReferences[1] = {};
    aspectReferences[0].subpass = 1;
    aspectReferences[0].inputAttachmentIndex = 2;
    aspectReferences[0].aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    rpiaa_ci.pAspectReferences = aspectReferences;

    rpmv_ci.sType = VK_STRUCTURE_TYPE_RENDER_PASS_MULTIVIEW_CREATE_INFO;
    rpmv_ci.pNext = nullptr;
    rpmv_ci.subpassCount = 1;

    uint32_t viewMasks[1] = {1};
    rpmv_ci.pViewMasks = viewMasks;

    int32_t viewOffsets[2] = {0, 1};
    rpmv_ci.dependencyCount = 2;
    rpmv_ci.pViewOffsets = viewOffsets;

    uint32_t correlationMasks[1] = {8};
    rpmv_ci.correlationMaskCount = 1;
    rpmv_ci.pCorrelationMasks = correlationMasks;

    rpiaa_ci.pNext = &rpmv_ci;
    rp_ci.pNext = &rpiaa_ci;

    rp_ci.flags = 0;

    VkAttachmentDescription attachments[1] = {};
    rp_ci.attachmentCount = 1;
    attachments[0].flags = VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT;
    attachments[0].format = VK_FORMAT_R8G8_USCALED;
    attachments[0].samples = VK_SAMPLE_COUNT_8_BIT;
    attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[0].initialLayout = VK_IMAGE_LAYOUT_GENERAL;
    attachments[0].finalLayout = VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR;
    rp_ci.pAttachments = attachments;

    VkSubpassDescription subpasses[2] = {};
    rp_ci.subpassCount = 2;
    subpasses[0].flags = 0;
    subpasses[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

    VkAttachmentReference subpass_0_inputAttachments[1] = {{567, VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL}};
    subpasses[0].inputAttachmentCount = 1;
    subpasses[0].pInputAttachments = subpass_0_inputAttachments;

    VkAttachmentReference subpass_0_colorAttachments[1] = {{VK_ATTACHMENT_UNUSED, VK_IMAGE_LAYOUT_UNDEFINED}};
    VkAttachmentReference subpass_0_resolveAttachments[1] = {{VK_ATTACHMENT_UNUSED, VK_IMAGE_LAYOUT_UNDEFINED}};
    VkAttachmentReference subpass_0_deptStencilAttachments[1] = {{VK_ATTACHMENT_UNUSED, VK_IMAGE_LAYOUT_UNDEFINED}};
    subpasses[0].colorAttachmentCount = 1;
    subpasses[0].pColorAttachments = subpass_0_colorAttachments;
    subpasses[0].pResolveAttachments = subpass_0_resolveAttachments;
    subpasses[0].pDepthStencilAttachment = subpass_0_deptStencilAttachments;
    subpasses[0].preserveAttachmentCount = 0;
    subpasses[0].pPreserveAttachments = nullptr;

    subpasses[1].flags = 0;
    subpasses[1].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpasses[1].inputAttachmentCount = 0;
    subpasses[1].pInputAttachments = nullptr;

    VkAttachmentReference subpass_1_colorAttachments[1] = {{567, VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL_KHR}};
    VkAttachmentReference subpass_1_resolveAttachments[1] = {{VK_ATTACHMENT_UNUSED, VK_IMAGE_LAYOUT_UNDEFINED}};
    VkAttachmentReference subpass_1_deptStencilAttachments[1] = {{VK_ATTACHMENT_UNUSED, VK_IMAGE_LAYOUT_UNDEFINED}};
    subpasses[1].colorAttachmentCount = 1;
    subpasses[1].pColorAttachments = subpass_1_colorAttachments;
    subpasses[1].pResolveAttachments = subpass_1_resolveAttachments;
    subpasses[1].pDepthStencilAttachment = subpass_1_deptStencilAttachments;
    subpasses[1].preserveAttachmentCount = 0;
    subpasses[1].pPreserveAttachments = nullptr;
    rp_ci.pSubpasses = subpasses;

    VkSubpassDependency dependencies[1] = {};
    rp_ci.dependencyCount = 1;
    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 2345;
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_NONE_KHR;
    dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[0].srcAccessMask = VK_ACCESS_NONE_KHR;
    dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[0].dependencyFlags = VK_DEPENDENCY_DEVICE_GROUP_BIT;
    rp_ci.pDependencies = dependencies;

    const char* result_json = nullptr;

    EXPECT_TRUE(vpjGenerateSingleStructJson(generator_, &rp_ci, &result_json, &msg_));
    CHECK_GEN();
    EXPECT_TRUE(CompareJson(result_json, ref_json));
}

TEST_F(Gen, VkRenderPassCreateInfo2) {
    TEST_DESCRIPTION("Tests generating of a reasonably complex render pass 2 layout create info JSON");

    std::string ref_json = {R"({
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
                        "attachment": 4294967295,
                        "layout": "VK_IMAGE_LAYOUT_UNDEFINED",
                        "aspectMask": "VK_IMAGE_ASPECT_COLOR_BIT"
                    }
                ],
                "pResolveAttachments": [
                    {
                        "sType": "VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2",
                        "pNext": "NULL",
                        "attachment": 4294967295,
                        "layout": "VK_IMAGE_LAYOUT_UNDEFINED",
                        "aspectMask": "VK_IMAGE_ASPECT_COLOR_BIT"
                    }
                ],
                "pDepthStencilAttachment": {
                    "sType": "VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2",
                    "pNext": "NULL",
                    "attachment": 4294967295,
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
                        "layout": "VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL",
                        "aspectMask": "VK_IMAGE_ASPECT_COLOR_BIT"
                    }
                ],
                "pResolveAttachments": [
                    {
                        "sType": "VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2",
                        "pNext": "NULL",
                        "attachment": 4294967295,
                        "layout": "VK_IMAGE_LAYOUT_UNDEFINED",
                        "aspectMask": "VK_IMAGE_ASPECT_COLOR_BIT"
                    }
                ],
                "pDepthStencilAttachment": {
                    "sType": "VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2",
                    "pNext": "NULL",
                    "attachment": 4294967295,
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
                    "sType" : "VK_STRUCTURE_TYPE_MEMORY_BARRIER_2",
                    "pNext": "NULL",
                    "srcStageMask": "VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT",
                    "srcAccessMask": "VK_ACCESS_2_COLOR_ATTACHMENT_READ_NONCOHERENT_BIT_EXT",
                    "dstStageMask": "VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT",
                    "dstAccessMask": "VK_ACCESS_2_COMMAND_PREPROCESS_READ_BIT_EXT"
                },
                "srcSubpass": 4294967295,
                "dstSubpass": 2345,
                "srcStageMask": 0,
                "dstStageMask": "VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT",
                "srcAccessMask": 0,
                "dstAccessMask": "VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT",
                "dependencyFlags": "VK_DEPENDENCY_DEVICE_GROUP_BIT",
                "viewOffset": 0
            }
        ],
        "correlatedViewMaskCount": 1,
        "pCorrelatedViewMasks": [ 8 ]
    })"};

    VkRenderPassCreateInfo2 rp2_ci{};

    VkAttachmentDescriptionStencilLayout adsl{};
    VkFragmentShadingRateAttachmentInfoKHR fsrai{};

    VkMemoryBarrier2KHR mb2{};

    rp2_ci.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2;
    rp2_ci.pNext = nullptr;
    rp2_ci.flags = 0;

    adsl.sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_STENCIL_LAYOUT;
    adsl.pNext = nullptr;
    adsl.stencilInitialLayout = VK_IMAGE_LAYOUT_GENERAL;
    adsl.stencilFinalLayout = VK_IMAGE_LAYOUT_GENERAL;

    VkAttachmentDescription2 attachments[1] = {};
    rp2_ci.attachmentCount = 1;
    attachments[0].sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2;
    attachments[0].flags = VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT;
    attachments[0].format = VK_FORMAT_R8G8_USCALED;
    attachments[0].samples = VK_SAMPLE_COUNT_8_BIT;
    attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[0].initialLayout = VK_IMAGE_LAYOUT_GENERAL;
    attachments[0].finalLayout = VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR;
    attachments[0].pNext = &adsl;
    rp2_ci.pAttachments = attachments;

    VkAttachmentReference2 depthStencilResolveAttachment{};
    depthStencilResolveAttachment.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;
    depthStencilResolveAttachment.pNext = nullptr;
    depthStencilResolveAttachment.attachment = 8;
    depthStencilResolveAttachment.layout = VK_IMAGE_LAYOUT_GENERAL;
    depthStencilResolveAttachment.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

    VkSubpassDescriptionDepthStencilResolve sddsr{};
    sddsr.sType = VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_DEPTH_STENCIL_RESOLVE;
    sddsr.pNext = nullptr;
    sddsr.depthResolveMode = VK_RESOLVE_MODE_MIN_BIT;
    sddsr.stencilResolveMode = VK_RESOLVE_MODE_MAX_BIT;
    sddsr.pDepthStencilResolveAttachment = &depthStencilResolveAttachment;

    VkAttachmentReference2 fragmentShadingRateAttachment{};
    fsrai.sType = VK_STRUCTURE_TYPE_FRAGMENT_SHADING_RATE_ATTACHMENT_INFO_KHR;
    fragmentShadingRateAttachment.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;
    fragmentShadingRateAttachment.pNext = nullptr;
    fragmentShadingRateAttachment.attachment = 5;
    fragmentShadingRateAttachment.layout = VK_IMAGE_LAYOUT_GENERAL;
    fragmentShadingRateAttachment.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    fsrai.pFragmentShadingRateAttachment = &fragmentShadingRateAttachment;
    fsrai.pNext = &sddsr;

    fsrai.shadingRateAttachmentTexelSize.width = 4;
    fsrai.shadingRateAttachmentTexelSize.height = 4;

    VkSubpassDescription2 subpasses[2] = {};
    rp2_ci.subpassCount = 2;

    subpasses[0].sType = VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2;
    subpasses[0].flags = 0;
    subpasses[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpasses[0].inputAttachmentCount = 1;

    VkAttachmentReference2 subpass_0_inputAttachments[1] = {
        {VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2, nullptr, 567, VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT}};
    subpasses[0].pInputAttachments = subpass_0_inputAttachments;

    subpasses[0].colorAttachmentCount = 1;
    VkAttachmentReference2 subpass_0_colorAttachments[1] = {{VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2, nullptr,
                                                             VK_ATTACHMENT_UNUSED, VK_IMAGE_LAYOUT_UNDEFINED,
                                                             VK_IMAGE_ASPECT_COLOR_BIT}};
    VkAttachmentReference2 subpass_0_resolveAttachments[1] = {{VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2, nullptr,
                                                               VK_ATTACHMENT_UNUSED, VK_IMAGE_LAYOUT_UNDEFINED,
                                                               VK_IMAGE_ASPECT_COLOR_BIT}};
    VkAttachmentReference2 subpass_0_depthStencilAttachments[1] = {{VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2, nullptr,
                                                                    VK_ATTACHMENT_UNUSED, VK_IMAGE_LAYOUT_UNDEFINED,
                                                                    VK_IMAGE_ASPECT_DEPTH_BIT}};
    subpasses[0].pColorAttachments = subpass_0_colorAttachments;
    subpasses[0].pResolveAttachments = subpass_0_resolveAttachments;
    subpasses[0].pDepthStencilAttachment = subpass_0_depthStencilAttachments;
    subpasses[0].preserveAttachmentCount = 0;
    subpasses[0].pPreserveAttachments = nullptr;
    subpasses[0].pNext = &fsrai;

    subpasses[1].sType = VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2;
    subpasses[1].pNext = nullptr;
    subpasses[1].flags = 0;
    subpasses[1].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpasses[1].inputAttachmentCount = 0;
    subpasses[1].pInputAttachments = nullptr;

    subpasses[1].colorAttachmentCount = 1;
    VkAttachmentReference2 subpass_1_colorAttachments[1] = {
        {VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2, nullptr, 567, VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL_KHR, VK_IMAGE_ASPECT_COLOR_BIT}};
    VkAttachmentReference2 subpass_1_resolveAttachments[1] = {{VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2, nullptr,
                                                               VK_ATTACHMENT_UNUSED, VK_IMAGE_LAYOUT_UNDEFINED,
                                                               VK_IMAGE_ASPECT_COLOR_BIT}};
    VkAttachmentReference2 subpass_1_depthStencilAttachments[1] = {{VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2, nullptr,
                                                                    VK_ATTACHMENT_UNUSED, VK_IMAGE_LAYOUT_UNDEFINED,
                                                                    VK_IMAGE_ASPECT_DEPTH_BIT}};
    subpasses[1].pColorAttachments = subpass_1_colorAttachments;
    subpasses[1].pResolveAttachments = subpass_1_resolveAttachments;
    subpasses[1].pDepthStencilAttachment = subpass_1_depthStencilAttachments;
    subpasses[1].preserveAttachmentCount = 0;
    subpasses[1].pPreserveAttachments = nullptr;

    rp2_ci.pSubpasses = subpasses;

    mb2.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2_KHR;
    mb2.pNext = nullptr;
    mb2.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    mb2.srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_READ_NONCOHERENT_BIT_EXT;
    mb2.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    mb2.dstAccessMask = VK_ACCESS_2_COMMAND_PREPROCESS_READ_BIT_NV;

    rp2_ci.dependencyCount = 1;
    VkSubpassDependency2 dependencies[1] = {};
    dependencies[0].sType = VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2;
    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 2345;
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_NONE_KHR;
    dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[0].srcAccessMask = VK_ACCESS_NONE_KHR;
    dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[0].dependencyFlags = VK_DEPENDENCY_DEVICE_GROUP_BIT;
    dependencies[0].pNext = &mb2;
    rp2_ci.pDependencies = dependencies;

    rp2_ci.correlatedViewMaskCount = 1;
    uint32_t correlatedViewMasks[1] = {8};
    rp2_ci.pCorrelatedViewMasks = correlatedViewMasks;

    const char* result_json = nullptr;

    EXPECT_TRUE(vpjGenerateSingleStructJson(generator_, &rp2_ci, &result_json, &msg_));
    CHECK_GEN();
    EXPECT_TRUE(CompareJson(result_json, ref_json));
}

TEST_F(Gen, VkShaderModuleCreateInfo) {
    TEST_DESCRIPTION("Tests generating of a reasonably complex shader module create info JSON");

    std::string ref_json = {R"({
        "sType" : "VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO",
        "pNext": "NULL",
        "flags": 0,
        "codeSize": 4,
        "pCode": "GXsqCA=="
    })"};

    VkShaderModuleCreateInfo sm_ci;
    sm_ci.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    sm_ci.pNext = nullptr;
    sm_ci.flags = 0;
    sm_ci.codeSize = 4;

    uint32_t code[1] = {(25 << 0) + (123 << 8) + (42 << 16) + (8 << 24)};
    sm_ci.pCode = code;

    const char* result_json = nullptr;

    EXPECT_TRUE(vpjGenerateSingleStructJson(generator_, &sm_ci, &result_json, &msg_));
    CHECK_GEN();
    EXPECT_TRUE(CompareJson(result_json, ref_json));
}

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

    std::string ref_json = {R"({
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

    VkPipelineOfflineCreateInfo po_ci;

    po_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_OFFLINE_CREATE_INFO;
    po_ci.pNext = nullptr;
    po_ci.pipelineIdentifier[0] = 85;
    po_ci.pipelineIdentifier[1] = 43;
    po_ci.pipelineIdentifier[2] = 255;
    po_ci.pipelineIdentifier[3] = 24;
    po_ci.pipelineIdentifier[4] = 155;
    po_ci.pipelineIdentifier[5] = 64;
    po_ci.pipelineIdentifier[6] = 62;
    po_ci.pipelineIdentifier[7] = 24;
    po_ci.pipelineIdentifier[8] = 0;
    po_ci.pipelineIdentifier[9] = 0;
    po_ci.pipelineIdentifier[10] = 0;
    po_ci.pipelineIdentifier[11] = 0;
    po_ci.pipelineIdentifier[12] = 0;
    po_ci.pipelineIdentifier[13] = 0;
    po_ci.pipelineIdentifier[14] = 0;
    po_ci.pipelineIdentifier[15] = 0;
    po_ci.matchControl = VK_PIPELINE_MATCH_CONTROL_APPLICATION_UUID_EXACT_MATCH;
    po_ci.poolEntrySize = 1048576;

    const char* result_json = nullptr;

    EXPECT_TRUE(vpjGenerateSingleStructJson(generator_, &po_ci, &result_json, &msg_));
    CHECK_GEN();
    EXPECT_TRUE(CompareJson(result_json, ref_json));
}

TEST_F(Gen, ComputePipelineJSON) {
    TEST_DESCRIPTION("Tests generation of a reasonably complex compute pipeline JSON");

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
)"
                               R"(
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
                            "pImmutableSamplers" : [],
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
)"
                               R"(
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
)"
                               R"(
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
)"
                               R"(
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

    VpjData data{};

    VkSamplerYcbcrConversionCreateInfo ycbcr_ci;
    const char* ycbcr_names[1] = {"YcbcrConversion1"};
    ycbcr_ci.sType = VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_CREATE_INFO;
#ifdef VK_USE_PLATFORM_SCREEN_QNX
    VkExternalFormatQNX ef_qnx{};
    ef_qnx.sType = VK_STRUCTURE_TYPE_EXTERNAL_FORMAT_QNX;
    ef_qnx.pNext = nullptr;
    ef_qnx.externalFormat = 10;
    ycbcr_ci.pNext = &ef_qnx;
#else
    ycbcr_ci.pNext = nullptr;
#endif  // VK_USE_PLATFORM_SCREEN_QNX
    ycbcr_ci.format = VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16;
    ycbcr_ci.ycbcrModel = VK_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_2020;
    ycbcr_ci.ycbcrRange = VK_SAMPLER_YCBCR_RANGE_ITU_NARROW;
    ycbcr_ci.components.r = VK_COMPONENT_SWIZZLE_A;
    ycbcr_ci.components.g = VK_COMPONENT_SWIZZLE_B;
    ycbcr_ci.components.b = VK_COMPONENT_SWIZZLE_G;
    ycbcr_ci.components.a = VK_COMPONENT_SWIZZLE_R;
    ycbcr_ci.xChromaOffset = VK_CHROMA_LOCATION_COSITED_EVEN;
    ycbcr_ci.yChromaOffset = VK_CHROMA_LOCATION_MIDPOINT;
    ycbcr_ci.chromaFilter = VK_FILTER_CUBIC_EXT;
    ycbcr_ci.forceExplicitReconstruction = VK_TRUE;

    VkSamplerCreateInfo sampler_ci[2] = {};
    const char* sampler_names[2] = {"ImmutableSampler1", "YcbcrSampler1"};

    VkSamplerYcbcrConversionInfo ycbcrConversionInfo[1] = {};

    ycbcrConversionInfo[0].sType = VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_INFO;
    ycbcrConversionInfo[0].conversion = VkSamplerYcbcrConversion(0);

    VkSampler immutableSamplers[2] = {VkSampler(0), VkSampler(1)};

    sampler_ci[0].sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    VkSamplerReductionModeCreateInfo srm_ci{};
    srm_ci.sType = VK_STRUCTURE_TYPE_SAMPLER_REDUCTION_MODE_CREATE_INFO;
    srm_ci.pNext = nullptr;
    srm_ci.reductionMode = VK_SAMPLER_REDUCTION_MODE_MAX;
    sampler_ci[0].pNext = &srm_ci;
    sampler_ci[0].flags = 0;
    sampler_ci[0].magFilter = VK_FILTER_CUBIC_EXT;
    sampler_ci[0].minFilter = VK_FILTER_NEAREST;
    sampler_ci[0].mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_ci[0].addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_ci[0].addressModeV = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    sampler_ci[0].addressModeW = VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
    sampler_ci[0].mipLodBias = 0.5f;
    sampler_ci[0].anisotropyEnable = VK_FALSE;
    sampler_ci[0].maxAnisotropy = 2.0f;
    sampler_ci[0].compareEnable = VK_FALSE;
    sampler_ci[0].compareOp = VK_COMPARE_OP_NEVER;
    sampler_ci[0].minLod = 0.5f;
    sampler_ci[0].maxLod = VK_LOD_CLAMP_NONE;
    sampler_ci[0].borderColor = VK_BORDER_COLOR_FLOAT_CUSTOM_EXT;
    sampler_ci[0].unnormalizedCoordinates = VK_TRUE;

    sampler_ci[1].sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_ci[1].pNext = &ycbcrConversionInfo;

    VkComputePipelineCreateInfo cp_ci{};
    cp_ci.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    cp_ci.pNext = nullptr;
    cp_ci.flags = 0;
    cp_ci.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;

    VkPipelineLayoutCreateInfo pl_ci{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};

    VkDescriptorSetLayout setLayouts[3] = {VkDescriptorSetLayout(0)};
    pl_ci.setLayoutCount = 1;
    pl_ci.pSetLayouts = setLayouts;
    VkPushConstantRange pushConstantRanges[1] = {};
    pl_ci.pushConstantRangeCount = 1;
    pushConstantRanges[0].size = 4;
    pushConstantRanges[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    pl_ci.pPushConstantRanges = pushConstantRanges;

    VkPipelineShaderStageRequiredSubgroupSizeCreateInfo pssrss_ci{};
    pssrss_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_REQUIRED_SUBGROUP_SIZE_CREATE_INFO;
    pssrss_ci.pNext = nullptr;
    pssrss_ci.requiredSubgroupSize = 64;
    cp_ci.stage.pNext = &pssrss_ci;

    cp_ci.stage.flags = VK_PIPELINE_SHADER_STAGE_CREATE_REQUIRE_FULL_SUBGROUPS_BIT;
    cp_ci.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    cp_ci.stage.pName = "main";

    VkDescriptorSetLayoutCreateInfo dsl_ci{};
    const char* dsl_names[1] = {"DescriptorSetLayout1"};

    dsl_ci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    VkDescriptorSetLayoutBindingFlagsCreateInfo dslbf_ci{};
    dslbf_ci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
    dslbf_ci.pNext = nullptr;
    dslbf_ci.bindingCount = 1;
    VkDescriptorBindingFlags bindingFlags[1] = {};
    bindingFlags[0] = VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT | VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT;
    dslbf_ci.pBindingFlags = bindingFlags;
    dsl_ci.pNext = &dslbf_ci;
    dsl_ci.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;

    dsl_ci.bindingCount = 3;
    VkDescriptorSetLayoutBinding bindings[3] = {};

    bindings[0].binding = 0;
    bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    bindings[0].descriptorCount = 1;
    bindings[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    bindings[0].pImmutableSamplers = nullptr;

    bindings[1].binding = 1;
    bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    bindings[1].descriptorCount = 1;
    bindings[1].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    bindings[1].pImmutableSamplers = &immutableSamplers[0];

    bindings[2].binding = 2;
    bindings[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    bindings[2].descriptorCount = 1;
    bindings[2].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    bindings[2].pImmutableSamplers = &immutableSamplers[1];

    dsl_ci.pBindings = bindings;

    VkPhysicalDeviceFeatures2 pdf{};
    pdf.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    pdf.features.robustBufferAccess = VK_TRUE;
    VkPhysicalDeviceSynchronization2Features pdf_sync2{};
    pdf_sync2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
    pdf_sync2.synchronization2 = VK_TRUE;
    pdf.pNext = &pdf_sync2;

    VpjShaderFileName shaderFileNames[1] = {};
    shaderFileNames[0].pFilename = "shader.comp.spv";
    shaderFileNames[0].stage = VK_SHADER_STAGE_COMPUTE_BIT;

    const char* enabled_extensions[1] = {"VK_EXT_robustness2"};

    data.enabledExtensionCount = 1;
    data.ppEnabledExtensions = enabled_extensions;
    data.computePipelineState.pComputePipeline = &cp_ci;
    data.computePipelineState.pDescriptorSetLayouts = &dsl_ci;
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
    data.computePipelineState.shaderFileNameCount = 1;
    data.computePipelineState.pShaderFileNames = shaderFileNames;

    data.pipelineUUID[0] = 85;
    data.pipelineUUID[1] = 43;
    data.pipelineUUID[2] = 255;
    data.pipelineUUID[3] = 24;
    data.pipelineUUID[4] = 155;
    data.pipelineUUID[5] = 64;
    data.pipelineUUID[6] = 62;
    data.pipelineUUID[7] = 24;

    const char* result_json = nullptr;

    EXPECT_TRUE(vpjGeneratePipelineJson(generator_, &data, &result_json, &msg_));
    CHECK_GEN();
    EXPECT_TRUE(ValidatePipelineJson(std::string(result_json)));
    EXPECT_TRUE(CompareJson(result_json, ref_json));
}

TEST_F(Gen, GraphicsPipelineJSON) {
    TEST_DESCRIPTION("Tests generation of a reasonably complex graphics pipeline JSON");

    const std::string ref_json{R"({
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
                            "pImmutableSamplers" : [],
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
)"
                               R"(
        "GraphicsPipeline" : 
        {
            "basePipelineHandle" : "",
            "basePipelineIndex" : 0,
            "flags" : 0,
            "layout" : "",
            "pColorBlendState" : "NULL",
            "pDepthStencilState" : "NULL",
            "pDynamicState" : "NULL",
            "pInputAssemblyState" : "NULL",
            "pMultisampleState" : "NULL",
            "pNext" : "NULL",
            "pRasterizationState" : "NULL",
            "pStages" : 
            [
                {
                    "flags" : 0,
                    "module" : "",
                    "pName" : "main",
                    "pNext" : "NULL",
                    "pSpecializationInfo" : "NULL",
                    "sType" : "VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO",
                    "stage" : "VK_SHADER_STAGE_VERTEX_BIT"
                },
                {
                    "flags" : 0,
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
                    "stage" : "VK_SHADER_STAGE_FRAGMENT_BIT"
                }
            ],
            "pTessellationState" : "NULL",
            "pVertexInputState" : "NULL",
            "pViewportState" : "NULL",
            "renderPass" : "",
            "sType" : "VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO",
            "stageCount" : 2,
            "subpass" : 0
        },
)"
                               R"(
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
)"
                               R"(
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
)"
                               R"(
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
)"
                               R"(
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
)"
                               R"(
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

    VpjData data{};

    VkRenderPassCreateInfo renderPass = {};
    data.graphicsPipelineState.pRenderPass = &renderPass;
    renderPass.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPass.pNext = nullptr;
    renderPass.flags = 0;
    renderPass.attachmentCount = 2;
    VkAttachmentDescription attachments[2] = {{}, {}};
    renderPass.pAttachments = attachments;
    attachments[0].flags = 0;
    attachments[0].format = VK_FORMAT_R8G8B8A8_UNORM;
    attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    attachments[1].flags = 0;
    attachments[1].format = VK_FORMAT_D16_UNORM;
    attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    renderPass.subpassCount = 1;
    VkSubpassDescription subpasses[1] = {{}};
    renderPass.pSubpasses = subpasses;
    subpasses[0].flags = 0;
    subpasses[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpasses[0].inputAttachmentCount = 0;
    subpasses[0].pInputAttachments = nullptr;
    subpasses[0].colorAttachmentCount = 1;
    VkAttachmentReference colorAttachments[1] = {{}};
    colorAttachments[0].attachment = 0;
    colorAttachments[0].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    subpasses[0].pColorAttachments = colorAttachments;
    subpasses[0].pResolveAttachments = nullptr;
    VkAttachmentReference depthStencilAttachment[1] = {{}};
    depthStencilAttachment[0].attachment = 1;
    depthStencilAttachment[0].layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    subpasses[0].pDepthStencilAttachment = depthStencilAttachment;
    subpasses[0].preserveAttachmentCount = 0;
    subpasses[0].pPreserveAttachments = nullptr;
    renderPass.dependencyCount = 2;
    VkSubpassDependency dependencies[2] = {{}, {}};
    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 0;
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    dependencies[0].dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    dependencies[0].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    dependencies[0].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    dependencies[0].dependencyFlags = 0;
    dependencies[1].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[1].dstSubpass = 0;
    dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[1].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[1].srcAccessMask = 0;
    dependencies[1].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
    dependencies[1].dependencyFlags = 0;
    renderPass.pDependencies = dependencies;

    VkSamplerYcbcrConversionCreateInfo ycbcr_ci;
    const char* ycbcr_names[1] = {"YcbcrConversion1"};
    ycbcr_ci.sType = VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_CREATE_INFO;
#ifdef VK_USE_PLATFORM_SCREEN_QNX
    VkExternalFormatQNX ef_qnx{};
    ef_qnx.sType = VK_STRUCTURE_TYPE_EXTERNAL_FORMAT_QNX;
    ef_qnx.pNext = nullptr;
    ef_qnx.externalFormat = 10;
    ycbcr_ci.pNext = &ef_qnx;
#else
    ycbcr_ci.pNext = nullptr;
#endif  // VK_USE_PLATFORM_SCREEN_QNX
    ycbcr_ci.format = VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16;
    ycbcr_ci.ycbcrModel = VK_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_2020;
    ycbcr_ci.ycbcrRange = VK_SAMPLER_YCBCR_RANGE_ITU_NARROW;
    ycbcr_ci.components.r = VK_COMPONENT_SWIZZLE_A;
    ycbcr_ci.components.g = VK_COMPONENT_SWIZZLE_B;
    ycbcr_ci.components.b = VK_COMPONENT_SWIZZLE_G;
    ycbcr_ci.components.a = VK_COMPONENT_SWIZZLE_R;
    ycbcr_ci.xChromaOffset = VK_CHROMA_LOCATION_COSITED_EVEN;
    ycbcr_ci.yChromaOffset = VK_CHROMA_LOCATION_MIDPOINT;
    ycbcr_ci.chromaFilter = VK_FILTER_CUBIC_EXT;
    ycbcr_ci.forceExplicitReconstruction = VK_TRUE;

    VkSamplerCreateInfo sampler_ci[2] = {};
    const char* sampler_names[2] = {"ImmutableSampler1", "YcbcrSampler1"};

    VkSamplerYcbcrConversionInfo ycbcrConversionInfo[1] = {};

    ycbcrConversionInfo[0].sType = VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_INFO;
    ycbcrConversionInfo[0].conversion = VkSamplerYcbcrConversion(0);

    VkSampler immutableSamplers[2] = {VkSampler(0), VkSampler(1)};

    sampler_ci[0].sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    VkSamplerReductionModeCreateInfo srm_ci{};
    srm_ci.sType = VK_STRUCTURE_TYPE_SAMPLER_REDUCTION_MODE_CREATE_INFO;
    srm_ci.pNext = nullptr;
    srm_ci.reductionMode = VK_SAMPLER_REDUCTION_MODE_MAX;
    sampler_ci[0].pNext = &srm_ci;
    sampler_ci[0].flags = 0;
    sampler_ci[0].magFilter = VK_FILTER_CUBIC_EXT;
    sampler_ci[0].minFilter = VK_FILTER_NEAREST;
    sampler_ci[0].mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_ci[0].addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_ci[0].addressModeV = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    sampler_ci[0].addressModeW = VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
    sampler_ci[0].mipLodBias = 0.5f;
    sampler_ci[0].anisotropyEnable = VK_FALSE;
    sampler_ci[0].maxAnisotropy = 2.0f;
    sampler_ci[0].compareEnable = VK_FALSE;
    sampler_ci[0].compareOp = VK_COMPARE_OP_NEVER;
    sampler_ci[0].minLod = 0.5f;
    sampler_ci[0].maxLod = VK_LOD_CLAMP_NONE;
    sampler_ci[0].borderColor = VK_BORDER_COLOR_FLOAT_CUSTOM_EXT;
    sampler_ci[0].unnormalizedCoordinates = VK_TRUE;

    sampler_ci[1].sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_ci[1].pNext = &ycbcrConversionInfo;

    VkGraphicsPipelineCreateInfo gp_ci = {};
    data.graphicsPipelineState.pGraphicsPipeline = &gp_ci;
    gp_ci.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    gp_ci.pNext = nullptr;
    gp_ci.flags = 0;
    gp_ci.stageCount = 2;
    VkPipelineShaderStageCreateInfo stages[2] = {{}, {}};
    gp_ci.pStages = stages;
    stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stages[0].pNext = nullptr;
    stages[0].flags = 0;
    stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    stages[0].module = reinterpret_cast<VkShaderModule>(38);
    stages[0].pName = "main";
    stages[0].pSpecializationInfo = nullptr;
    stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    VkPipelineShaderStageRequiredSubgroupSizeCreateInfo pssrss_ci{};
    pssrss_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_REQUIRED_SUBGROUP_SIZE_CREATE_INFO;
    pssrss_ci.pNext = nullptr;
    pssrss_ci.requiredSubgroupSize = 64;
    stages[1].pNext = &pssrss_ci;
    stages[1].flags = 0;
    stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    stages[1].module = reinterpret_cast<VkShaderModule>(39);
    stages[1].pName = "main";
    stages[1].pSpecializationInfo = nullptr;

    VkPipelineLayoutCreateInfo pl_ci{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};

    VkDescriptorSetLayout setLayouts[3] = {VkDescriptorSetLayout(0)};
    pl_ci.setLayoutCount = 1;
    pl_ci.pSetLayouts = setLayouts;
    VkPushConstantRange pushConstantRanges[1] = {};
    pl_ci.pushConstantRangeCount = 1;
    pushConstantRanges[0].size = 4;
    pushConstantRanges[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    pl_ci.pPushConstantRanges = pushConstantRanges;

    VkDescriptorSetLayoutCreateInfo dsl_ci{};
    const char* dsl_names[1] = {"DescriptorSetLayout1"};

    dsl_ci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    VkDescriptorSetLayoutBindingFlagsCreateInfo dslbf_ci{};
    dslbf_ci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
    dslbf_ci.pNext = nullptr;
    dslbf_ci.bindingCount = 1;
    VkDescriptorBindingFlags bindingFlags[1] = {};
    bindingFlags[0] = VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT | VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT;
    dslbf_ci.pBindingFlags = bindingFlags;
    dsl_ci.pNext = &dslbf_ci;
    dsl_ci.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;

    dsl_ci.bindingCount = 3;
    VkDescriptorSetLayoutBinding bindings[3] = {};

    bindings[0].binding = 0;
    bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    bindings[0].descriptorCount = 1;
    bindings[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    bindings[0].pImmutableSamplers = nullptr;

    bindings[1].binding = 1;
    bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    bindings[1].descriptorCount = 1;
    bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    bindings[1].pImmutableSamplers = &immutableSamplers[0];

    bindings[2].binding = 2;
    bindings[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    bindings[2].descriptorCount = 1;
    bindings[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    bindings[2].pImmutableSamplers = &immutableSamplers[1];

    dsl_ci.pBindings = bindings;

    VkPhysicalDeviceFeatures2 pdf{};
    pdf.features.robustBufferAccess = VK_TRUE;
    pdf.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    VkPhysicalDeviceSynchronization2Features pdf_sync2{};
    pdf_sync2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
    pdf_sync2.synchronization2 = VK_TRUE;
    pdf.pNext = &pdf_sync2;

    VpjShaderFileName shaderFileNames[2] = {};
    shaderFileNames[0].pFilename = "shader.vert.spv";
    shaderFileNames[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shaderFileNames[1].pFilename = "shader.frag.spv";
    shaderFileNames[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;

    const char* enabled_extensions[1] = {"VK_EXT_robustness2"};

    data.enabledExtensionCount = 1;
    data.ppEnabledExtensions = enabled_extensions;
    data.graphicsPipelineState.pGraphicsPipeline = &gp_ci;
    data.graphicsPipelineState.pRenderPass = &renderPass;
    data.graphicsPipelineState.pDescriptorSetLayouts = &dsl_ci;
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
    data.graphicsPipelineState.shaderFileNameCount = 2;
    data.graphicsPipelineState.pShaderFileNames = shaderFileNames;

    data.pipelineUUID[0] = 85;
    data.pipelineUUID[1] = 43;
    data.pipelineUUID[2] = 255;
    data.pipelineUUID[3] = 24;
    data.pipelineUUID[4] = 155;
    data.pipelineUUID[5] = 64;
    data.pipelineUUID[6] = 62;
    data.pipelineUUID[7] = 24;

    const char* result_json = nullptr;

    EXPECT_TRUE(vpjGeneratePipelineJson(generator_, &data, &result_json, &msg_));
    CHECK_GEN();
    EXPECT_TRUE(ValidatePipelineJson(std::string(result_json)));
    EXPECT_TRUE(CompareJson(result_json, ref_json));
}

TEST_F(Gen, ComputePipelineJSONWithMD5) {
    TEST_DESCRIPTION("Tests generating pipeline JSON with MD5 UUID for a compute pipeline");

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
)"
                               R"(
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
                            "pImmutableSamplers" : [],
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
)"
                               R"(
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
)"
                               R"(
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
                "robustBufferAccess" : "VK_FALSE",
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
)"
                               R"(
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
        173,
        219,
        118,
        247,
        216,
        178,
        101,
        146,
        185,
        23,
        86,
        164,
        204,
        27,
        25,
        250
    ]
})"};

    VpjData data{};

    VkSamplerYcbcrConversionCreateInfo ycbcr_ci;
    const char* ycbcr_names[1] = {"YcbcrConversion1"};
    ycbcr_ci.sType = VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_CREATE_INFO;
#ifdef VK_USE_PLATFORM_SCREEN_QNX
    VkExternalFormatQNX ef_qnx{};
    ef_qnx.sType = VK_STRUCTURE_TYPE_EXTERNAL_FORMAT_QNX;
    ef_qnx.pNext = nullptr;
    ef_qnx.externalFormat = 10;
    ycbcr_ci.pNext = &ef_qnx;
#else
    ycbcr_ci.pNext = nullptr;
#endif  // VK_USE_PLATFORM_SCREEN_QNX
    ycbcr_ci.format = VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16;
    ycbcr_ci.ycbcrModel = VK_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_2020;
    ycbcr_ci.ycbcrRange = VK_SAMPLER_YCBCR_RANGE_ITU_NARROW;
    ycbcr_ci.components.r = VK_COMPONENT_SWIZZLE_A;
    ycbcr_ci.components.g = VK_COMPONENT_SWIZZLE_B;
    ycbcr_ci.components.b = VK_COMPONENT_SWIZZLE_G;
    ycbcr_ci.components.a = VK_COMPONENT_SWIZZLE_R;
    ycbcr_ci.xChromaOffset = VK_CHROMA_LOCATION_COSITED_EVEN;
    ycbcr_ci.yChromaOffset = VK_CHROMA_LOCATION_MIDPOINT;
    ycbcr_ci.chromaFilter = VK_FILTER_CUBIC_EXT;
    ycbcr_ci.forceExplicitReconstruction = VK_TRUE;

    VkSamplerCreateInfo sampler_ci[2] = {};
    const char* sampler_names[2] = {"ImmutableSampler1", "YcbcrSampler1"};

    VkSamplerYcbcrConversionInfo ycbcrConversionInfo[1] = {};

    ycbcrConversionInfo[0].sType = VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_INFO;
    ycbcrConversionInfo[0].conversion = VkSamplerYcbcrConversion(0);

    VkSampler immutableSamplers[2] = {VkSampler(0), VkSampler(1)};

    sampler_ci[0].sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    VkSamplerReductionModeCreateInfo srm_ci{};
    srm_ci.sType = VK_STRUCTURE_TYPE_SAMPLER_REDUCTION_MODE_CREATE_INFO;
    srm_ci.pNext = nullptr;
    srm_ci.reductionMode = VK_SAMPLER_REDUCTION_MODE_MAX;
    sampler_ci[0].pNext = &srm_ci;
    sampler_ci[0].flags = 0;
    sampler_ci[0].magFilter = VK_FILTER_CUBIC_EXT;
    sampler_ci[0].minFilter = VK_FILTER_NEAREST;
    sampler_ci[0].mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_ci[0].addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_ci[0].addressModeV = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    sampler_ci[0].addressModeW = VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
    sampler_ci[0].mipLodBias = 0.5f;
    sampler_ci[0].anisotropyEnable = VK_FALSE;
    sampler_ci[0].maxAnisotropy = 2.0f;
    sampler_ci[0].compareEnable = VK_FALSE;
    sampler_ci[0].compareOp = VK_COMPARE_OP_NEVER;
    sampler_ci[0].minLod = 0.5f;
    sampler_ci[0].maxLod = VK_LOD_CLAMP_NONE;
    sampler_ci[0].borderColor = VK_BORDER_COLOR_FLOAT_CUSTOM_EXT;
    sampler_ci[0].unnormalizedCoordinates = VK_TRUE;

    sampler_ci[1].sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_ci[1].pNext = &ycbcrConversionInfo;

    VkComputePipelineCreateInfo cp_ci{};
    cp_ci.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    cp_ci.pNext = nullptr;
    cp_ci.flags = 0;
    cp_ci.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;

    VkPipelineLayoutCreateInfo pl_ci{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};

    VkDescriptorSetLayout setLayouts[3] = {VkDescriptorSetLayout(0)};
    pl_ci.setLayoutCount = 1;
    pl_ci.pSetLayouts = setLayouts;
    VkPushConstantRange pushConstantRanges[1] = {};
    pl_ci.pushConstantRangeCount = 1;
    pushConstantRanges[0].size = 4;
    pushConstantRanges[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    pl_ci.pPushConstantRanges = pushConstantRanges;

    VkPipelineShaderStageRequiredSubgroupSizeCreateInfo pssrss_ci{};
    pssrss_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_REQUIRED_SUBGROUP_SIZE_CREATE_INFO;
    pssrss_ci.pNext = nullptr;
    pssrss_ci.requiredSubgroupSize = 64;
    cp_ci.stage.pNext = &pssrss_ci;

    cp_ci.stage.flags = VK_PIPELINE_SHADER_STAGE_CREATE_REQUIRE_FULL_SUBGROUPS_BIT;
    cp_ci.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    cp_ci.stage.pName = "main";

    VkDescriptorSetLayoutCreateInfo dsl_ci{};
    const char* dsl_names[1] = {"DescriptorSetLayout1"};

    dsl_ci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    VkDescriptorSetLayoutBindingFlagsCreateInfo dslbf_ci{};
    dslbf_ci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
    dslbf_ci.pNext = nullptr;
    dslbf_ci.bindingCount = 1;
    VkDescriptorBindingFlags bindingFlags[1] = {};
    bindingFlags[0] = VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT | VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT;
    dslbf_ci.pBindingFlags = bindingFlags;
    dsl_ci.pNext = &dslbf_ci;
    dsl_ci.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;

    dsl_ci.bindingCount = 3;
    VkDescriptorSetLayoutBinding bindings[3] = {};

    bindings[0].binding = 0;
    bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    bindings[0].descriptorCount = 1;
    bindings[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    bindings[0].pImmutableSamplers = nullptr;

    bindings[1].binding = 1;
    bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    bindings[1].descriptorCount = 1;
    bindings[1].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    bindings[1].pImmutableSamplers = &immutableSamplers[0];

    bindings[2].binding = 2;
    bindings[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    bindings[2].descriptorCount = 1;
    bindings[2].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    bindings[2].pImmutableSamplers = &immutableSamplers[1];

    dsl_ci.pBindings = bindings;

    VkPhysicalDeviceFeatures2 pdf{};
    pdf.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    VkPhysicalDeviceSynchronization2Features pdf_sync2{};
    pdf_sync2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
    pdf_sync2.synchronization2 = VK_TRUE;
    pdf.pNext = &pdf_sync2;

    VpjShaderFileName shaderFileNames[1] = {};
    shaderFileNames[0].pFilename = "shader.comp.spv";
    shaderFileNames[0].stage = VK_SHADER_STAGE_COMPUTE_BIT;

    std::vector<uint32_t> ref_spirv{1, 2, 3, 4};
    shaderFileNames[0].codeSize = ref_spirv.size() * sizeof(uint32_t);
    shaderFileNames[0].pCode = ref_spirv.data();

    const char* enabled_extensions[1] = {"VK_EXT_robustness2"};

    data.enabledExtensionCount = 1;
    data.ppEnabledExtensions = enabled_extensions;
    data.computePipelineState.pComputePipeline = &cp_ci;
    data.computePipelineState.pDescriptorSetLayouts = &dsl_ci;
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
    data.computePipelineState.shaderFileNameCount = 1;
    data.computePipelineState.pShaderFileNames = shaderFileNames;

    const char* result_json = nullptr;

    vpjSetMD5PipelineUUIDGeneration(generator_, true);
    EXPECT_TRUE(vpjGeneratePipelineJson(generator_, &data, &result_json, &msg_));
    CHECK_GEN();
    EXPECT_TRUE(ValidatePipelineJson(std::string(result_json)));
    EXPECT_TRUE(CompareJson(result_json, ref_json));
}

TEST_F(Gen, GraphicsPipelineJSONWithMD5) {
    TEST_DESCRIPTION("Tests generating pipeline JSON with MD5 UUID for a graphics pipeline");

    const std::string ref_json{R"({
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
                            "pImmutableSamplers" : [],
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
)"
                               R"(
        "GraphicsPipeline" : 
        {
            "basePipelineHandle" : "",
            "basePipelineIndex" : 0,
            "flags" : 0,
            "layout" : "",
            "pColorBlendState" : "NULL",
            "pDepthStencilState" : "NULL",
            "pDynamicState" : "NULL",
            "pInputAssemblyState" : "NULL",
            "pMultisampleState" : "NULL",
            "pNext" : "NULL",
            "pRasterizationState" : "NULL",
            "pStages" : 
            [
                {
                    "flags" : 0,
                    "module" : "",
                    "pName" : "main",
                    "pNext" : "NULL",
                    "pSpecializationInfo" : "NULL",
                    "sType" : "VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO",
                    "stage" : "VK_SHADER_STAGE_VERTEX_BIT"
                },
                {
                    "flags" : 0,
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
                    "stage" : "VK_SHADER_STAGE_FRAGMENT_BIT"
                }
            ],
            "pTessellationState" : "NULL",
            "pVertexInputState" : "NULL",
            "pViewportState" : "NULL",
            "renderPass" : "",
            "sType" : "VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO",
            "stageCount" : 2,
            "subpass" : 0
        },
)"
                               R"(
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
)"
                               R"(
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
)"
                               R"(
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
)"
                               R"(
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
)"
                               R"(
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
    "PipelineUUID" :
    [
        49,
        240,
        91,
        17,
        162,
        98,
        76,
        103,
        145,
        145,
        146,
        247,
        163,
        153,
        172,
        238
    ]
})"};

    VpjData data{};

    VkRenderPassCreateInfo renderPass = {};
    data.graphicsPipelineState.pRenderPass = &renderPass;
    renderPass.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPass.pNext = nullptr;
    renderPass.flags = 0;
    renderPass.attachmentCount = 2;
    VkAttachmentDescription attachments[2] = {{}, {}};
    renderPass.pAttachments = attachments;
    attachments[0].flags = 0;
    attachments[0].format = VK_FORMAT_R8G8B8A8_UNORM;
    attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    attachments[1].flags = 0;
    attachments[1].format = VK_FORMAT_D16_UNORM;
    attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    renderPass.subpassCount = 1;
    VkSubpassDescription subpasses[1] = {{}};
    renderPass.pSubpasses = subpasses;
    subpasses[0].flags = 0;
    subpasses[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpasses[0].inputAttachmentCount = 0;
    subpasses[0].pInputAttachments = nullptr;
    subpasses[0].colorAttachmentCount = 1;
    VkAttachmentReference colorAttachments[1] = {{}};
    colorAttachments[0].attachment = 0;
    colorAttachments[0].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    subpasses[0].pColorAttachments = colorAttachments;
    subpasses[0].pResolveAttachments = nullptr;
    VkAttachmentReference depthStencilAttachment[1] = {{}};
    depthStencilAttachment[0].attachment = 1;
    depthStencilAttachment[0].layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    subpasses[0].pDepthStencilAttachment = depthStencilAttachment;
    subpasses[0].preserveAttachmentCount = 0;
    subpasses[0].pPreserveAttachments = nullptr;
    renderPass.dependencyCount = 2;
    VkSubpassDependency dependencies[2] = {{}, {}};
    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 0;
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    dependencies[0].dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    dependencies[0].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    dependencies[0].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    dependencies[0].dependencyFlags = 0;
    dependencies[1].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[1].dstSubpass = 0;
    dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[1].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[1].srcAccessMask = 0;
    dependencies[1].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
    dependencies[1].dependencyFlags = 0;
    renderPass.pDependencies = dependencies;

    VkSamplerYcbcrConversionCreateInfo ycbcr_ci;
    const char* ycbcr_names[1] = {"YcbcrConversion1"};
    ycbcr_ci.sType = VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_CREATE_INFO;
#ifdef VK_USE_PLATFORM_SCREEN_QNX
    VkExternalFormatQNX ef_qnx{};
    ef_qnx.sType = VK_STRUCTURE_TYPE_EXTERNAL_FORMAT_QNX;
    ef_qnx.pNext = nullptr;
    ef_qnx.externalFormat = 10;
    ycbcr_ci.pNext = &ef_qnx;
#else
    ycbcr_ci.pNext = nullptr;
#endif  // VK_USE_PLATFORM_SCREEN_QNX
    ycbcr_ci.format = VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16;
    ycbcr_ci.ycbcrModel = VK_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_2020;
    ycbcr_ci.ycbcrRange = VK_SAMPLER_YCBCR_RANGE_ITU_NARROW;
    ycbcr_ci.components.r = VK_COMPONENT_SWIZZLE_A;
    ycbcr_ci.components.g = VK_COMPONENT_SWIZZLE_B;
    ycbcr_ci.components.b = VK_COMPONENT_SWIZZLE_G;
    ycbcr_ci.components.a = VK_COMPONENT_SWIZZLE_R;
    ycbcr_ci.xChromaOffset = VK_CHROMA_LOCATION_COSITED_EVEN;
    ycbcr_ci.yChromaOffset = VK_CHROMA_LOCATION_MIDPOINT;
    ycbcr_ci.chromaFilter = VK_FILTER_CUBIC_EXT;
    ycbcr_ci.forceExplicitReconstruction = VK_TRUE;

    VkSamplerCreateInfo sampler_ci[2] = {};
    const char* sampler_names[2] = {"ImmutableSampler1", "YcbcrSampler1"};

    VkSamplerYcbcrConversionInfo ycbcrConversionInfo[1] = {};

    ycbcrConversionInfo[0].sType = VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_INFO;
    ycbcrConversionInfo[0].conversion = VkSamplerYcbcrConversion(0);

    VkSampler immutableSamplers[2] = {VkSampler(0), VkSampler(1)};

    sampler_ci[0].sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    VkSamplerReductionModeCreateInfo srm_ci{};
    srm_ci.sType = VK_STRUCTURE_TYPE_SAMPLER_REDUCTION_MODE_CREATE_INFO;
    srm_ci.pNext = nullptr;
    srm_ci.reductionMode = VK_SAMPLER_REDUCTION_MODE_MAX;
    sampler_ci[0].pNext = &srm_ci;
    sampler_ci[0].flags = 0;
    sampler_ci[0].magFilter = VK_FILTER_CUBIC_EXT;
    sampler_ci[0].minFilter = VK_FILTER_NEAREST;
    sampler_ci[0].mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_ci[0].addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_ci[0].addressModeV = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    sampler_ci[0].addressModeW = VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
    sampler_ci[0].mipLodBias = 0.5f;
    sampler_ci[0].anisotropyEnable = VK_FALSE;
    sampler_ci[0].maxAnisotropy = 2.0f;
    sampler_ci[0].compareEnable = VK_FALSE;
    sampler_ci[0].compareOp = VK_COMPARE_OP_NEVER;
    sampler_ci[0].minLod = 0.5f;
    sampler_ci[0].maxLod = VK_LOD_CLAMP_NONE;
    sampler_ci[0].borderColor = VK_BORDER_COLOR_FLOAT_CUSTOM_EXT;
    sampler_ci[0].unnormalizedCoordinates = VK_TRUE;

    sampler_ci[1].sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_ci[1].pNext = &ycbcrConversionInfo;

    VkGraphicsPipelineCreateInfo gp_ci = {};
    data.graphicsPipelineState.pGraphicsPipeline = &gp_ci;
    gp_ci.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    gp_ci.pNext = nullptr;
    gp_ci.flags = 0;
    gp_ci.stageCount = 2;
    VkPipelineShaderStageCreateInfo stages[2] = {{}, {}};
    gp_ci.pStages = stages;
    stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stages[0].pNext = nullptr;
    stages[0].flags = 0;
    stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    stages[0].module = reinterpret_cast<VkShaderModule>(38);
    stages[0].pName = "main";
    stages[0].pSpecializationInfo = nullptr;
    stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    VkPipelineShaderStageRequiredSubgroupSizeCreateInfo pssrss_ci{};
    pssrss_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_REQUIRED_SUBGROUP_SIZE_CREATE_INFO;
    pssrss_ci.pNext = nullptr;
    pssrss_ci.requiredSubgroupSize = 64;
    stages[1].pNext = &pssrss_ci;
    stages[1].flags = 0;
    stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    stages[1].module = reinterpret_cast<VkShaderModule>(39);
    stages[1].pName = "main";
    stages[1].pSpecializationInfo = nullptr;

    VkPipelineLayoutCreateInfo pl_ci{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};

    VkDescriptorSetLayout setLayouts[3] = {VkDescriptorSetLayout(0)};
    pl_ci.setLayoutCount = 1;
    pl_ci.pSetLayouts = setLayouts;
    VkPushConstantRange pushConstantRanges[1] = {};
    pl_ci.pushConstantRangeCount = 1;
    pushConstantRanges[0].size = 4;
    pushConstantRanges[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    pl_ci.pPushConstantRanges = pushConstantRanges;

    VkDescriptorSetLayoutCreateInfo dsl_ci{};
    const char* dsl_names[1] = {"DescriptorSetLayout1"};

    dsl_ci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    VkDescriptorSetLayoutBindingFlagsCreateInfo dslbf_ci{};
    dslbf_ci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
    dslbf_ci.pNext = nullptr;
    dslbf_ci.bindingCount = 1;
    VkDescriptorBindingFlags bindingFlags[1] = {};
    bindingFlags[0] = VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT | VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT;
    dslbf_ci.pBindingFlags = bindingFlags;
    dsl_ci.pNext = &dslbf_ci;
    dsl_ci.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;

    dsl_ci.bindingCount = 3;
    VkDescriptorSetLayoutBinding bindings[3] = {};

    bindings[0].binding = 0;
    bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    bindings[0].descriptorCount = 1;
    bindings[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    bindings[0].pImmutableSamplers = nullptr;

    bindings[1].binding = 1;
    bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    bindings[1].descriptorCount = 1;
    bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    bindings[1].pImmutableSamplers = &immutableSamplers[0];

    bindings[2].binding = 2;
    bindings[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    bindings[2].descriptorCount = 1;
    bindings[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    bindings[2].pImmutableSamplers = &immutableSamplers[1];

    dsl_ci.pBindings = bindings;

    VkPhysicalDeviceFeatures2 pdf{};
    pdf.features.robustBufferAccess = VK_TRUE;
    pdf.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    VkPhysicalDeviceSynchronization2Features pdf_sync2{};
    pdf_sync2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
    pdf_sync2.synchronization2 = VK_TRUE;
    pdf.pNext = &pdf_sync2;

    VpjShaderFileName shaderFileNames[2] = {};
    shaderFileNames[0].pFilename = "shader.vert.spv";
    shaderFileNames[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shaderFileNames[1].pFilename = "shader.frag.spv";
    shaderFileNames[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;

    std::vector<uint32_t> ref_spirv{1, 2, 3, 4};
    shaderFileNames[0].codeSize = ref_spirv.size() * sizeof(uint32_t);
    shaderFileNames[0].pCode = ref_spirv.data();
    shaderFileNames[1].codeSize = ref_spirv.size() * sizeof(uint32_t);
    shaderFileNames[1].pCode = ref_spirv.data();

    const char* enabled_extensions[1] = {"VK_EXT_robustness2"};

    data.enabledExtensionCount = 1;
    data.ppEnabledExtensions = enabled_extensions;
    data.graphicsPipelineState.pGraphicsPipeline = &gp_ci;
    data.graphicsPipelineState.pRenderPass = &renderPass;
    data.graphicsPipelineState.pDescriptorSetLayouts = &dsl_ci;
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
    data.graphicsPipelineState.shaderFileNameCount = 2;
    data.graphicsPipelineState.pShaderFileNames = shaderFileNames;

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

    VkSamplerYcbcrConversionCreateInfo ycbcr_ci;
    const char* ycbcr_names[1] = {"YcbcrConversion1"};
    ycbcr_ci.sType = VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_CREATE_INFO;
#ifdef VK_USE_PLATFORM_SCREEN_QNX
    VkExternalFormatQNX ef_qnx{};
    ef_qnx.sType = VK_STRUCTURE_TYPE_EXTERNAL_FORMAT_QNX;
    ef_qnx.pNext = nullptr;
    ef_qnx.externalFormat = 10;
    ycbcr_ci.pNext = &ef_qnx;
#else
    ycbcr_ci.pNext = nullptr;
#endif  // VK_USE_PLATFORM_SCREEN_QNX
    ycbcr_ci.format = VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16;
    ycbcr_ci.ycbcrModel = VK_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_2020;
    ycbcr_ci.ycbcrRange = VK_SAMPLER_YCBCR_RANGE_ITU_NARROW;
    ycbcr_ci.components.r = VK_COMPONENT_SWIZZLE_A;
    ycbcr_ci.components.g = VK_COMPONENT_SWIZZLE_B;
    ycbcr_ci.components.b = VK_COMPONENT_SWIZZLE_G;
    ycbcr_ci.components.a = VK_COMPONENT_SWIZZLE_R;
    ycbcr_ci.xChromaOffset = VK_CHROMA_LOCATION_COSITED_EVEN;
    ycbcr_ci.yChromaOffset = VK_CHROMA_LOCATION_MIDPOINT;
    ycbcr_ci.chromaFilter = VK_FILTER_CUBIC_EXT;
    ycbcr_ci.forceExplicitReconstruction = VK_TRUE;

    VkSamplerCreateInfo sampler_ci[2] = {};
    const char* sampler_names[2] = {"ImmutableSampler1", "YcbcrSampler1"};

    VkSamplerYcbcrConversionInfo ycbcrConversionInfo[1] = {};

    ycbcrConversionInfo[0].sType = VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_INFO;
    ycbcrConversionInfo[0].conversion = VkSamplerYcbcrConversion(0);

    VkSampler immutableSamplers[2] = {VkSampler(0), VkSampler(1)};

    sampler_ci[0].sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    VkSamplerReductionModeCreateInfo srm_ci{};
    srm_ci.sType = VK_STRUCTURE_TYPE_SAMPLER_REDUCTION_MODE_CREATE_INFO;
    srm_ci.pNext = nullptr;
    srm_ci.reductionMode = VK_SAMPLER_REDUCTION_MODE_MAX;
    sampler_ci[0].pNext = &srm_ci;
    sampler_ci[0].flags = 0;
    sampler_ci[0].magFilter = VK_FILTER_CUBIC_EXT;
    sampler_ci[0].minFilter = VK_FILTER_NEAREST;
    sampler_ci[0].mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_ci[0].addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_ci[0].addressModeV = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    sampler_ci[0].addressModeW = VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
    sampler_ci[0].mipLodBias = 0.5f;
    sampler_ci[0].anisotropyEnable = VK_FALSE;
    sampler_ci[0].maxAnisotropy = 2.0f;
    sampler_ci[0].compareEnable = VK_FALSE;
    sampler_ci[0].compareOp = VK_COMPARE_OP_NEVER;
    sampler_ci[0].minLod = 0.5f;
    sampler_ci[0].maxLod = VK_LOD_CLAMP_NONE;
    sampler_ci[0].borderColor = VK_BORDER_COLOR_FLOAT_CUSTOM_EXT;
    sampler_ci[0].unnormalizedCoordinates = VK_TRUE;

    sampler_ci[1].sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_ci[1].pNext = &ycbcrConversionInfo;

    VkComputePipelineCreateInfo cp_ci{};
    cp_ci.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    cp_ci.pNext = nullptr;
    cp_ci.flags = 0;
    cp_ci.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;

    VkPipelineLayoutCreateInfo pl_ci{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};

    VkDescriptorSetLayout setLayouts[3] = {VkDescriptorSetLayout(0)};
    pl_ci.setLayoutCount = 1;
    pl_ci.pSetLayouts = setLayouts;
    VkPushConstantRange pushConstantRanges[1] = {};
    pl_ci.pushConstantRangeCount = 1;
    pushConstantRanges[0].size = 4;
    pushConstantRanges[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    pl_ci.pPushConstantRanges = pushConstantRanges;

    VkPipelineShaderStageRequiredSubgroupSizeCreateInfo pssrss_ci{};
    pssrss_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_REQUIRED_SUBGROUP_SIZE_CREATE_INFO;
    pssrss_ci.pNext = nullptr;
    pssrss_ci.requiredSubgroupSize = 64;
    cp_ci.stage.pNext = &pssrss_ci;

    cp_ci.stage.flags = VK_PIPELINE_SHADER_STAGE_CREATE_REQUIRE_FULL_SUBGROUPS_BIT;
    cp_ci.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    cp_ci.stage.pName = "main";

    VkDescriptorSetLayoutCreateInfo dsl_ci{};
    const char* dsl_names[1] = {"DescriptorSetLayout1"};

    dsl_ci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    VkDescriptorSetLayoutBindingFlagsCreateInfo dslbf_ci{};
    dslbf_ci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
    dslbf_ci.pNext = nullptr;
    dslbf_ci.bindingCount = 1;
    VkDescriptorBindingFlags bindingFlags[1] = {};
    bindingFlags[0] = VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT | VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT;
    dslbf_ci.pBindingFlags = bindingFlags;
    dsl_ci.pNext = &dslbf_ci;
    dsl_ci.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;

    dsl_ci.bindingCount = 3;
    VkDescriptorSetLayoutBinding bindings[3] = {};

    bindings[0].binding = 0;
    bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    bindings[0].descriptorCount = 1;
    bindings[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    bindings[0].pImmutableSamplers = nullptr;

    bindings[1].binding = 1;
    bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    bindings[1].descriptorCount = 1;
    bindings[1].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    bindings[1].pImmutableSamplers = &immutableSamplers[0];

    bindings[2].binding = 2;
    bindings[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    bindings[2].descriptorCount = 1;
    bindings[2].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    bindings[2].pImmutableSamplers = &immutableSamplers[1];

    dsl_ci.pBindings = bindings;

    VkPhysicalDeviceFeatures2 pdf{};
    pdf.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    pdf.features.robustBufferAccess = VK_TRUE;
    VkPhysicalDeviceSynchronization2Features pdf_sync2{};
    pdf_sync2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
    pdf_sync2.synchronization2 = VK_TRUE;
    pdf.pNext = &pdf_sync2;

    const char* enabled_extensions[1] = {"VK_EXT_robustness2"};

    data.enabledExtensionCount = 1;
    data.ppEnabledExtensions = enabled_extensions;
    data.computePipelineState.pComputePipeline = &cp_ci;
    data.computePipelineState.pDescriptorSetLayouts = &dsl_ci;
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

    data.pipelineUUID[0] = 85;
    data.pipelineUUID[1] = 43;
    data.pipelineUUID[2] = 255;
    data.pipelineUUID[3] = 24;
    data.pipelineUUID[4] = 155;
    data.pipelineUUID[5] = 64;
    data.pipelineUUID[6] = 62;
    data.pipelineUUID[7] = 24;

    const char* result_json = nullptr;

    EXPECT_FALSE(vpjGeneratePipelineJson(generator_, &data, &result_json, &msg_));
}

TEST_F(Gen, ZeroShaderFilenamesGraphics) {
    TEST_DESCRIPTION("Tests whether generation fails for graphics pipelines when no shader filenames are provided");

    VpjData data{};

    VkRenderPassCreateInfo renderPass = {};
    data.graphicsPipelineState.pRenderPass = &renderPass;
    renderPass.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPass.pNext = nullptr;
    renderPass.flags = 0;
    renderPass.attachmentCount = 2;
    VkAttachmentDescription attachments[2] = {{}, {}};
    renderPass.pAttachments = attachments;
    attachments[0].flags = 0;
    attachments[0].format = VK_FORMAT_R8G8B8A8_UNORM;
    attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    attachments[1].flags = 0;
    attachments[1].format = VK_FORMAT_D16_UNORM;
    attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    renderPass.subpassCount = 1;
    VkSubpassDescription subpasses[1] = {{}};
    renderPass.pSubpasses = subpasses;
    subpasses[0].flags = 0;
    subpasses[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpasses[0].inputAttachmentCount = 0;
    subpasses[0].pInputAttachments = nullptr;
    subpasses[0].colorAttachmentCount = 1;
    VkAttachmentReference colorAttachments[1] = {{}};
    colorAttachments[0].attachment = 0;
    colorAttachments[0].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    subpasses[0].pColorAttachments = colorAttachments;
    subpasses[0].pResolveAttachments = nullptr;
    VkAttachmentReference depthStencilAttachment[1] = {{}};
    depthStencilAttachment[0].attachment = 1;
    depthStencilAttachment[0].layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    subpasses[0].pDepthStencilAttachment = depthStencilAttachment;
    subpasses[0].preserveAttachmentCount = 0;
    subpasses[0].pPreserveAttachments = nullptr;
    renderPass.dependencyCount = 2;
    VkSubpassDependency dependencies[2] = {{}, {}};
    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 0;
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    dependencies[0].dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    dependencies[0].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    dependencies[0].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    dependencies[0].dependencyFlags = 0;
    dependencies[1].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[1].dstSubpass = 0;
    dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[1].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[1].srcAccessMask = 0;
    dependencies[1].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
    dependencies[1].dependencyFlags = 0;
    renderPass.pDependencies = dependencies;

    VkSamplerYcbcrConversionCreateInfo ycbcr_ci;
    const char* ycbcr_names[1] = {"YcbcrConversion1"};
    ycbcr_ci.sType = VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_CREATE_INFO;
#ifdef VK_USE_PLATFORM_SCREEN_QNX
    VkExternalFormatQNX ef_qnx{};
    ef_qnx.sType = VK_STRUCTURE_TYPE_EXTERNAL_FORMAT_QNX;
    ef_qnx.pNext = nullptr;
    ef_qnx.externalFormat = 10;
    ycbcr_ci.pNext = &ef_qnx;
#else
    ycbcr_ci.pNext = nullptr;
#endif  // VK_USE_PLATFORM_SCREEN_QNX
    ycbcr_ci.format = VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16;
    ycbcr_ci.ycbcrModel = VK_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_2020;
    ycbcr_ci.ycbcrRange = VK_SAMPLER_YCBCR_RANGE_ITU_NARROW;
    ycbcr_ci.components.r = VK_COMPONENT_SWIZZLE_A;
    ycbcr_ci.components.g = VK_COMPONENT_SWIZZLE_B;
    ycbcr_ci.components.b = VK_COMPONENT_SWIZZLE_G;
    ycbcr_ci.components.a = VK_COMPONENT_SWIZZLE_R;
    ycbcr_ci.xChromaOffset = VK_CHROMA_LOCATION_COSITED_EVEN;
    ycbcr_ci.yChromaOffset = VK_CHROMA_LOCATION_MIDPOINT;
    ycbcr_ci.chromaFilter = VK_FILTER_CUBIC_EXT;
    ycbcr_ci.forceExplicitReconstruction = VK_TRUE;

    VkSamplerCreateInfo sampler_ci[2] = {};
    const char* sampler_names[2] = {"ImmutableSampler1", "YcbcrSampler1"};

    VkSamplerYcbcrConversionInfo ycbcrConversionInfo[1] = {};

    ycbcrConversionInfo[0].sType = VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_INFO;
    ycbcrConversionInfo[0].conversion = VkSamplerYcbcrConversion(0);

    VkSampler immutableSamplers[2] = {VkSampler(0), VkSampler(1)};

    sampler_ci[0].sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    VkSamplerReductionModeCreateInfo srm_ci{};
    srm_ci.sType = VK_STRUCTURE_TYPE_SAMPLER_REDUCTION_MODE_CREATE_INFO;
    srm_ci.pNext = nullptr;
    srm_ci.reductionMode = VK_SAMPLER_REDUCTION_MODE_MAX;
    sampler_ci[0].pNext = &srm_ci;
    sampler_ci[0].flags = 0;
    sampler_ci[0].magFilter = VK_FILTER_CUBIC_EXT;
    sampler_ci[0].minFilter = VK_FILTER_NEAREST;
    sampler_ci[0].mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_ci[0].addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_ci[0].addressModeV = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    sampler_ci[0].addressModeW = VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
    sampler_ci[0].mipLodBias = 0.5f;
    sampler_ci[0].anisotropyEnable = VK_FALSE;
    sampler_ci[0].maxAnisotropy = 2.0f;
    sampler_ci[0].compareEnable = VK_FALSE;
    sampler_ci[0].compareOp = VK_COMPARE_OP_NEVER;
    sampler_ci[0].minLod = 0.5f;
    sampler_ci[0].maxLod = VK_LOD_CLAMP_NONE;
    sampler_ci[0].borderColor = VK_BORDER_COLOR_FLOAT_CUSTOM_EXT;
    sampler_ci[0].unnormalizedCoordinates = VK_TRUE;

    sampler_ci[1].sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_ci[1].pNext = &ycbcrConversionInfo;

    VkGraphicsPipelineCreateInfo gp_ci = {};
    data.graphicsPipelineState.pGraphicsPipeline = &gp_ci;
    gp_ci.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    gp_ci.pNext = nullptr;
    gp_ci.flags = 0;
    gp_ci.stageCount = 2;
    VkPipelineShaderStageCreateInfo stages[2] = {{}, {}};
    gp_ci.pStages = stages;
    stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stages[0].pNext = nullptr;
    stages[0].flags = 0;
    stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    stages[0].module = reinterpret_cast<VkShaderModule>(38);
    stages[0].pName = "main";
    stages[0].pSpecializationInfo = nullptr;
    stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    VkPipelineShaderStageRequiredSubgroupSizeCreateInfo pssrss_ci{};
    pssrss_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_REQUIRED_SUBGROUP_SIZE_CREATE_INFO;
    pssrss_ci.pNext = nullptr;
    pssrss_ci.requiredSubgroupSize = 64;
    stages[1].pNext = &pssrss_ci;
    stages[1].flags = 0;
    stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    stages[1].module = reinterpret_cast<VkShaderModule>(39);
    stages[1].pName = "main";
    stages[1].pSpecializationInfo = nullptr;

    VkPipelineLayoutCreateInfo pl_ci{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};

    VkDescriptorSetLayout setLayouts[3] = {VkDescriptorSetLayout(0)};
    pl_ci.setLayoutCount = 1;
    pl_ci.pSetLayouts = setLayouts;
    VkPushConstantRange pushConstantRanges[1] = {};
    pl_ci.pushConstantRangeCount = 1;
    pushConstantRanges[0].size = 4;
    pushConstantRanges[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    pl_ci.pPushConstantRanges = pushConstantRanges;

    VkDescriptorSetLayoutCreateInfo dsl_ci{};
    const char* dsl_names[1] = {"DescriptorSetLayout1"};

    dsl_ci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    VkDescriptorSetLayoutBindingFlagsCreateInfo dslbf_ci{};
    dslbf_ci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
    dslbf_ci.pNext = nullptr;
    dslbf_ci.bindingCount = 1;
    VkDescriptorBindingFlags bindingFlags[1] = {};
    bindingFlags[0] = VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT | VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT;
    dslbf_ci.pBindingFlags = bindingFlags;
    dsl_ci.pNext = &dslbf_ci;
    dsl_ci.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;

    dsl_ci.bindingCount = 3;
    VkDescriptorSetLayoutBinding bindings[3] = {};

    bindings[0].binding = 0;
    bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    bindings[0].descriptorCount = 1;
    bindings[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    bindings[0].pImmutableSamplers = nullptr;

    bindings[1].binding = 1;
    bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    bindings[1].descriptorCount = 1;
    bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    bindings[1].pImmutableSamplers = &immutableSamplers[0];

    bindings[2].binding = 2;
    bindings[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    bindings[2].descriptorCount = 1;
    bindings[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    bindings[2].pImmutableSamplers = &immutableSamplers[1];

    dsl_ci.pBindings = bindings;

    VkPhysicalDeviceFeatures2 pdf{};
    pdf.features.robustBufferAccess = VK_TRUE;
    pdf.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    VkPhysicalDeviceSynchronization2Features pdf_sync2{};
    pdf_sync2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
    pdf_sync2.synchronization2 = VK_TRUE;
    pdf.pNext = &pdf_sync2;

    const char* enabled_extensions[1] = {"VK_EXT_robustness2"};

    data.enabledExtensionCount = 1;
    data.ppEnabledExtensions = enabled_extensions;
    data.graphicsPipelineState.pGraphicsPipeline = &gp_ci;
    data.graphicsPipelineState.pRenderPass = &renderPass;
    data.graphicsPipelineState.pDescriptorSetLayouts = &dsl_ci;
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

    data.pipelineUUID[0] = 85;
    data.pipelineUUID[1] = 43;
    data.pipelineUUID[2] = 255;
    data.pipelineUUID[3] = 24;
    data.pipelineUUID[4] = 155;
    data.pipelineUUID[5] = 64;
    data.pipelineUUID[6] = 62;
    data.pipelineUUID[7] = 24;

    const char* result_json = nullptr;

    EXPECT_FALSE(vpjGeneratePipelineJson(generator_, &data, &result_json, &msg_));
}
