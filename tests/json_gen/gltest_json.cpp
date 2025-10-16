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

class GenLayerJSONTest : public testing::Test {
  public:
    GenLayerJSONTest() {
        std::for_each(std::filesystem::directory_iterator{"."}, std::filesystem::directory_iterator{},
                      [](const std::filesystem::directory_entry& entry) {
                          if (std::regex_search(entry.path().generic_string(), std::regex{R"(gltest_json_)"})) {
                              std::filesystem::remove(entry);
                          }
                      });
    }
    GenLayerJSONTest(const GenLayerJSONTest&) = delete;
    GenLayerJSONTest(GenLayerJSONTest&&) = delete;
    ~GenLayerJSONTest() = default;

    void TEST_DESCRIPTION(const char* desc) { RecordProperty("description", desc); }

    std::string get_json(size_t device_id, size_t pipeline_id) {
        std::filesystem::path json_path =
            std::string("./gltest_json_device_") + std::to_string(device_id) + "_pipeline_" + std::to_string(pipeline_id) + ".json";
        auto json_size = std::filesystem::file_size(json_path);
        std::string json_str(json_size, '\0');
        std::ifstream json_stream{json_path};
        json_stream.read(json_str.data(), json_size);
        return json_str;
    }
    std::vector<uint32_t> get_spirv(size_t device_id, size_t pipeline_id, const char *stage) {
        std::filesystem::path spirv_path = std::string("./gltest_json_device_") + std::to_string(device_id) + "_pipeline_" +
                                           std::to_string(pipeline_id) + "." + stage + ".spv";
        auto spirv_size = std::filesystem::file_size(spirv_path);
        std::vector<uint32_t> spirv_vec(spirv_size / 4, '\0');
        std::ifstream spirv_stream{spirv_path};
        spirv_stream.read(reinterpret_cast<char *>(spirv_vec.data()), spirv_size);
        return spirv_vec;
    }

  protected:
};

inline size_t device_counter = 0;

TEST_F(GenLayerJSONTest, ComputeSimple) {
    TEST_DESCRIPTION("Tests whether generated pipeline JSON for a compute pipeline is as expected");

    auto instance = SAXPY::create_instance();
    auto device = SAXPY::create_device(instance);
    size_t device_id = device_counter++;
    int32_t pipeline_counter = 0;
    {
        SAXPY saxpy{instance, device};
        saxpy.run();
        pipeline_counter += SAXPY::pipeline_increment;
    }
    vkDestroyDevice(device, nullptr);
    vkDestroyInstance(instance, nullptr);

    auto json = get_json(device_id, pipeline_counter);
    auto spirv = get_spirv(device_id, pipeline_counter, "compute");

    std::string json_ref = R"({
	"ComputePipelineState" : 
	{
		"ComputePipeline" : 
		{
			"basePipelineHandle" : "",
			"basePipelineIndex" : -1,
			"flags" : 0,
			"layout" : 2,
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
		"DescriptorSetLayouts" : 
		[
			{
				"DescriptorSetLayout1" : 
				{
					"bindingCount" : 2,
					"flags" : 0,
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
							"descriptorType" : "VK_DESCRIPTOR_TYPE_STORAGE_BUFFER",
							"pImmutableSamplers" : "NULL",
							"stageFlags" : "VK_SHADER_STAGE_COMPUTE_BIT"
						}
					],
					"pNext" : "NULL",
					"sType" : "VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO"
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
			"pNext" : "NULL",
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
				"filename" : "gltest_json_device_0_pipeline_3.compute.spv",
				"stage" : "VK_SHADER_STAGE_COMPUTE_BIT"
			}
		]
	},
	"EnabledExtensions" : 
	[
		"VK_KHR_synchronization2"
	],
	"PipelineUUID" : 
	[
		140,
		89,
		159,
		98,
		51,
		11,
		176,
		237,
		218,
		135,
		128,
		80,
		71,
		165,
		18,
		253
	]
})";
    const std::vector<uint32_t> spirv_ref{
#include "saxpy.comp.inc"
    };

    auto spirv_words_match = std::equal(spirv.begin(), spirv.end(), spirv_ref.begin(), spirv_ref.end());
    EXPECT_TRUE(spirv_words_match);
    EXPECT_EQ(json, json_ref);
}

TEST_F(GenLayerJSONTest, ComputeReproducible) {
    TEST_DESCRIPTION("Tests whether generated pipeline JSON for a compute pipeline is as expected");

    auto instance = SAXPY::create_instance();
    auto device1 = SAXPY::create_device(instance);
    size_t device1_id = device_counter++;
    auto device2 = SAXPY::create_device(instance);
    size_t device2_id = device_counter++;

    int32_t pipeline_counter1 = 0, pipeline_counter2 = 0;
    {
        SAXPY saxpy1{instance, device1};
        saxpy1.run();
        pipeline_counter1 += SAXPY::pipeline_increment;

        SAXPY saxpy2{instance, device2};
        saxpy2.run();
        pipeline_counter2 = SAXPY::pipeline_increment;
    }
    vkDestroyDevice(device1, nullptr);
    vkDestroyDevice(device2, nullptr);
    vkDestroyInstance(instance, nullptr);

    auto json1 = get_json(device1_id, pipeline_counter1);
    auto json2 = get_json(device2_id, pipeline_counter2);
    GTEST_SKIP() << "Undecided if this should hold true or not.";
    EXPECT_EQ(json1, json2);
}

TEST_F(GenLayerJSONTest, GraphicsSimple) {
    TEST_DESCRIPTION("Tests whether generated pipeline JSON for a graphics pipeline is as expected");

    auto cube = Cube{};
    size_t device_id = device_counter++;
    cube.run();

    auto json = get_json(device_id, Cube::pipeline_increment);

    std::string json_ref = R"({
	"EnabledExtensions" : 
	[
		"VK_KHR_swapchain"
	],
	"GraphicsPipelineState" : 
	{
		"DescriptorSetLayouts" : 
		[
			{
				"DescriptorSetLayout2" : 
				{
					"bindingCount" : 2,
					"flags" : 0,
					"pBindings" : 
					[
						{
							"binding" : 0,
							"descriptorCount" : 1,
							"descriptorType" : "VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER",
							"pImmutableSamplers" : "NULL",
							"stageFlags" : "VK_SHADER_STAGE_VERTEX_BIT"
						},
						{
							"binding" : 1,
							"descriptorCount" : 1,
							"descriptorType" : "VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER",
							"pImmutableSamplers" : "NULL",
							"stageFlags" : "VK_SHADER_STAGE_FRAGMENT_BIT"
						}
					],
					"pNext" : "NULL",
					"sType" : "VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO"
				}
			}
		],
		"GraphicsPipeline" : 
		{
			"basePipelineHandle" : "",
			"basePipelineIndex" : 0,
			"flags" : 0,
			"layout" : 3,
			"pColorBlendState" : 
			{
				"attachmentCount" : 1,
				"blendConstants" : 
				[
					0.0,
					0.0,
					0.0,
					0.0
				],
				"flags" : 0,
				"logicOp" : "VK_LOGIC_OP_CLEAR",
				"logicOpEnable" : "VK_FALSE",
				"pAttachments" : 
				[
					{
						"alphaBlendOp" : "VK_BLEND_OP_ADD",
						"blendEnable" : "VK_FALSE",
						"colorBlendOp" : "VK_BLEND_OP_ADD",
						"colorWriteMask" : "VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT",
						"dstAlphaBlendFactor" : "VK_BLEND_FACTOR_ZERO",
						"dstColorBlendFactor" : "VK_BLEND_FACTOR_ZERO",
						"srcAlphaBlendFactor" : "VK_BLEND_FACTOR_ZERO",
						"srcColorBlendFactor" : "VK_BLEND_FACTOR_ZERO"
					}
				],
				"pNext" : "NULL",
				"sType" : "VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO"
			},
			"pDepthStencilState" : 
			{
				"back" : 
				{
					"compareMask" : 0,
					"compareOp" : "VK_COMPARE_OP_ALWAYS",
					"depthFailOp" : "VK_STENCIL_OP_KEEP",
					"failOp" : "VK_STENCIL_OP_KEEP",
					"passOp" : "VK_STENCIL_OP_KEEP",
					"reference" : 0,
					"writeMask" : 0
				},
				"depthBoundsTestEnable" : "VK_FALSE",
				"depthCompareOp" : "VK_COMPARE_OP_LESS_OR_EQUAL",
				"depthTestEnable" : "VK_TRUE",
				"depthWriteEnable" : "VK_TRUE",
				"flags" : 0,
				"front" : 
				{
					"compareMask" : 0,
					"compareOp" : "VK_COMPARE_OP_ALWAYS",
					"depthFailOp" : "VK_STENCIL_OP_KEEP",
					"failOp" : "VK_STENCIL_OP_KEEP",
					"passOp" : "VK_STENCIL_OP_KEEP",
					"reference" : 0,
					"writeMask" : 0
				},
				"maxDepthBounds" : 0.0,
				"minDepthBounds" : 0.0,
				"pNext" : "NULL",
				"sType" : "VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO",
				"stencilTestEnable" : "VK_FALSE"
			},
			"pDynamicState" : 
			{
				"dynamicStateCount" : 2,
				"flags" : 0,
				"pDynamicStates" : 
				[
					"VK_DYNAMIC_STATE_VIEWPORT",
					"VK_DYNAMIC_STATE_SCISSOR"
				],
				"pNext" : "NULL",
				"sType" : "VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO"
			},
			"pInputAssemblyState" : 
			{
				"flags" : 0,
				"pNext" : "NULL",
				"primitiveRestartEnable" : "VK_FALSE",
				"sType" : "VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO",
				"topology" : "VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST"
			},
			"pMultisampleState" : 
			{
				"alphaToCoverageEnable" : "VK_FALSE",
				"alphaToOneEnable" : "VK_FALSE",
				"flags" : 0,
				"minSampleShading" : 0.0,
				"pNext" : "NULL",
				"pSampleMask" : "NULL",
				"rasterizationSamples" : "VK_SAMPLE_COUNT_1_BIT",
				"sType" : "VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO",
				"sampleShadingEnable" : "VK_FALSE"
			},
			"pNext" : "NULL",
			"pRasterizationState" : 
			{
				"cullMode" : "VK_CULL_MODE_BACK_BIT",
				"depthBiasClamp" : 0.0,
				"depthBiasConstantFactor" : 0.0,
				"depthBiasEnable" : "VK_FALSE",
				"depthBiasSlopeFactor" : 0.0,
				"depthClampEnable" : "VK_FALSE",
				"flags" : 0,
				"frontFace" : "VK_FRONT_FACE_COUNTER_CLOCKWISE",
				"lineWidth" : 1.0,
				"pNext" : "NULL",
				"polygonMode" : "VK_POLYGON_MODE_FILL",
				"rasterizerDiscardEnable" : "VK_FALSE",
				"sType" : "VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO"
			},
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
					"pNext" : "NULL",
					"pSpecializationInfo" : "NULL",
					"sType" : "VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO",
					"stage" : "VK_SHADER_STAGE_FRAGMENT_BIT"
				}
			],
			"pTessellationState" : "NULL",
			"pVertexInputState" : 
			{
				"flags" : 0,
				"pNext" : "NULL",
				"pVertexAttributeDescriptions" : "NULL",
				"pVertexBindingDescriptions" : "NULL",
				"sType" : "VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO",
				"vertexAttributeDescriptionCount" : 0,
				"vertexBindingDescriptionCount" : 0
			},
			"pViewportState" : 
			{
				"flags" : 0,
				"pNext" : "NULL",
				"pScissors" : "NULL",
				"pViewports" : "NULL",
				"sType" : "VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO",
				"scissorCount" : 1,
				"viewportCount" : 1
			},
			"renderPass" : "",
			"sType" : "VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO",
			"stageCount" : 2,
			"subpass" : 0
		},
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
			"pNext" : "NULL",
			"sType" : "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2"
		},
		"PipelineLayout" : 
		{
			"flags" : 0,
			"pNext" : "NULL",
			"pPushConstantRanges" : "NULL",
			"pSetLayouts" : 
			[
				"DescriptorSetLayout2"
			],
			"pushConstantRangeCount" : 0,
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
					"finalLayout" : "VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL",
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
				"filename" : "gltest_json_device_3_pipeline_4.vert.spv",
				"stage" : "VK_SHADER_STAGE_VERTEX_BIT"
			},
			{
				"filename" : "gltest_json_device_3_pipeline_4.frag.spv",
				"stage" : "VK_SHADER_STAGE_FRAGMENT_BIT"
			}
		]
	},
	"PipelineUUID" : 
	[
		158,
		238,
		47,
		141,
		172,
		224,
		250,
		113,
		193,
		187,
		46,
		64,
		123,
		77,
		214,
		145
	]
})";
    EXPECT_EQ(json, json_ref);
}
