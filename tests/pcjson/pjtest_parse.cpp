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
    PJParseTest() : parser_{vpjCreateParser()} {}
    PJParseTest(const PJParseTest&) = delete;
    PJParseTest(PJParseTest&&) = default;
    ~PJParseTest() { vpjDestroyParser(parser_); }

    void TEST_DESCRIPTION(const char* desc) { RecordProperty("description", desc); }

  protected:
    VpjParser parser_;
};

TEST_F(PJParseTest, BasicTypesVkBool32) {
    TEST_DESCRIPTION("Tests value equivalence of written and parsed VkBool32 values");

    VkPhysicalDeviceFeatures2 data;
    const char* msg = nullptr;
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
