/*
 * Copyright (c) 2025 The Khronos Group Inc.
 * Copyright (c) 2025 RasterGrid Kft.
 * Copyright (c) 2021, NVIDIA CORPORATION
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <vulkan/vulkan.h>
#include <vulkan/utility/vk_safe_struct.hpp>
#include <vulkan/utility/vk_struct_helper.hpp>

#include <gtest/gtest.h>

#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <regex>

#include "json_validator.h"

class JSON : public testing::Test {
  public:
    JSON() {
        std::for_each(std::filesystem::directory_iterator{"."}, std::filesystem::directory_iterator{},
                      [](const std::filesystem::directory_entry& entry) {
                          if (std::regex_search(entry.path().generic_string(), std::regex{R"(gltest_json_)"})) {
                              std::filesystem::remove(entry);
                          }
                      });
    }
    JSON(const JSON&) = delete;
    JSON(JSON&&) = delete;
    ~JSON() = default;

    void TEST_DESCRIPTION(const char* desc) { RecordProperty("description", desc); }

    void write_ids(std::string& ref, size_t device_id, size_t pipeline_id) {
        for(auto pos = ref.find('@', 0); pos != std::string::npos; pos = ref.find('@', pos) ) {
            ref.replace(pos, 1, std::to_string(device_id));
        }
        for(auto pos = ref.find('#', 0); pos != std::string::npos; pos = ref.find('#', pos) ) {
            ref.replace(pos, 1, std::to_string(pipeline_id));
        }
    }
    std::string remove_filenames(std::string str) {
        return std::regex_replace(str, std::regex{R"=("filename" : "([^"]*)")="}, R"=("filename" : "")=");
    }
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

bool ValidatePipelineJson(const std::string& json_str) {
    JsonValidator json_validator;

    const std::string schema_path = std::string(SCHEMA_PATH) + "vksc_pipeline_schema.json";

    auto success = json_validator.LoadAndValidateSchema(schema_path);

    if (!success) {
        std::cout << json_validator.GetMessage() << std::endl;
        return success;
    }

    Json::String err;
    Json::Value json;
    Json::CharReaderBuilder builder;
    const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
    if (!reader->parse(json_str.c_str(), json_str.c_str() + json_str.size() + 1, &json, &err)) {
        std::cout << "error: " << err << std::endl;
        return EXIT_FAILURE;
    }
    success = json_validator.ValidateJson(json);

    if (!success) {
        std::cout << json_validator.GetMessage() << std::endl;
        return success;
    }

    return true;
}

inline size_t device_counter = 0;
inline size_t pipeline_counter = 0;

// TODO: create multiple pipelines from one invocation

TEST_F(JSON, ComputeSimple) {
    TEST_DESCRIPTION("Tests whether generated pipeline JSON for a minimal compute pipeline is as expected");

    vku::safe_VkInstanceCreateInfo instance_ci;
    VkInstance instance;
    vkCreateInstance(instance_ci.ptr(), NULL, &instance);

    uint32_t phys_dev_count = 1;
    std::vector<VkPhysicalDevice> phys_devs(phys_dev_count);
    vkEnumeratePhysicalDevices(instance, &phys_dev_count, phys_devs.data());

    vku::safe_VkDeviceCreateInfo device_ci;
    VkDevice device;
    vkCreateDevice(phys_devs[0], device_ci.ptr(), NULL, &device);

    vku::safe_VkShaderModuleCreateInfo shader_module_ci;
    std::vector<uint32_t> ref_spirv{1, 2, 3, 4};
    uint32_t* code = new uint32_t[ref_spirv.size()];
    std::copy(ref_spirv.begin(), ref_spirv.end(), code);
    shader_module_ci.codeSize = ref_spirv.size() * sizeof(uint32_t);
    shader_module_ci.pCode = code;
    VkShaderModule shader_module;
    vkCreateShaderModule(device, shader_module_ci.ptr(), NULL, &shader_module);

    vku::safe_VkDescriptorSetLayoutCreateInfo ds_layout_ci;
    VkDescriptorSetLayout ds_layout;
    vkCreateDescriptorSetLayout(device, ds_layout_ci.ptr(), NULL, &ds_layout);

    vku::safe_VkPipelineLayoutCreateInfo pipeline_layout_ci;
    VkPipelineLayout pipeline_layout;
    vkCreatePipelineLayout(device, pipeline_layout_ci.ptr(), NULL, &pipeline_layout);

    vku::safe_VkPipelineShaderStageCreateInfo pipeline_stage_ci{};
    char* name = new char[strlen("main") + 1];
    strcpy(name, "main");
    pipeline_stage_ci.module = shader_module;
    pipeline_stage_ci.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    pipeline_stage_ci.pName = name;
    vku::safe_VkComputePipelineCreateInfo compute_pipeline_ci;
    compute_pipeline_ci.layout = pipeline_layout;
    compute_pipeline_ci.stage = pipeline_stage_ci;
    VkPipeline pipeline;
    vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, compute_pipeline_ci.ptr(), NULL, &pipeline);

    vkDestroyPipeline(device, pipeline, nullptr);
    vkDestroyDescriptorSetLayout(device, ds_layout, nullptr);
    vkDestroyShaderModule(device, shader_module, nullptr);
    vkDestroyDevice(device, nullptr);
    vkDestroyInstance(instance, nullptr);

    auto result_json = get_json(device_counter, pipeline_counter += 3);
    auto result_spirv = get_spirv(device_counter, pipeline_counter, "compute");
    EXPECT_TRUE(ValidatePipelineJson(std::string(result_json)));

    std::string ref_json = {R"({
	"ComputePipelineState" : 
	{
		"ComputePipeline" : 
		{
			"basePipelineHandle" : "",
			"basePipelineIndex" : 0,
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
			"pSetLayouts" : "NULL",
			"pushConstantRangeCount" : 0,
			"sType" : "VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO",
			"setLayoutCount" : 0
		},
		"ShaderFileNames" : 
		[
			{
				"filename" : "gltest_json_device_@_pipeline_#.compute.spv",
				"stage" : "VK_SHADER_STAGE_COMPUTE_BIT"
			}
		]
	},
	"EnabledExtensions" : [],
	"PipelineUUID" : 
	[
		202,
		11,
		199,
		59,
		82,
		83,
		249,
		169,
		182,
		68,
		102,
		20,
		19,
		111,
		146,
		73
	]
})"};
    write_ids(ref_json, device_counter, pipeline_counter);

    auto spirv_words_match = std::equal(result_spirv.begin(), result_spirv.end(), ref_spirv.begin(), ref_spirv.end());
    EXPECT_TRUE(spirv_words_match);
    EXPECT_EQ(result_json, ref_json);
}

TEST_F(JSON, ComputeLifetime) {
    TEST_DESCRIPTION("Tests whether destroying objects as soon as allowed works as expected.");

    auto instance_ci = std::make_unique<VkInstanceCreateInfo>(vku::InitStructHelper());
    VkInstance instance;
    vkCreateInstance(instance_ci.get(), nullptr, &instance);

    uint32_t phys_dev_count = 1;
    std::vector<VkPhysicalDevice> phys_devs(phys_dev_count);
    vkEnumeratePhysicalDevices(instance, &phys_dev_count, phys_devs.data());

    auto device_ci = std::make_unique<VkDeviceCreateInfo>(vku::InitStructHelper());
    VkDevice device;
    vkCreateDevice(phys_devs[0], device_ci.get(), nullptr, &device);

    auto shader_module_ci = std::make_unique<VkShaderModuleCreateInfo>(vku::InitStructHelper());
    std::vector<uint32_t> ref_spirv{1, 2, 3, 4};
    shader_module_ci->codeSize = ref_spirv.size() * sizeof(uint32_t);
    shader_module_ci->pCode = ref_spirv.data();
    VkShaderModule shader_module;
    vkCreateShaderModule(device, shader_module_ci.get(), nullptr, &shader_module);

    // Create Ycbcr sampler conversion
    auto ycbcr_ci = std::make_unique<VkSamplerYcbcrConversionCreateInfo>(vku::InitStructHelper());
    VkSamplerYcbcrConversion ycbcr;
    vkCreateSamplerYcbcrConversion(device, ycbcr_ci.get(), nullptr, &ycbcr);

    // Destroy create info
    ycbcr_ci.reset();

    // Create Ycbcr sampler
    auto ycbcr_info = std::make_unique<VkSamplerYcbcrConversionInfo>(vku::InitStructHelper());
    ycbcr_info->conversion = ycbcr;
    auto sampler_ci = std::make_unique<VkSamplerCreateInfo>(vku::InitStructHelper());
    sampler_ci->pNext = ycbcr_info.get();
    VkSampler sampler;
    vkCreateSampler(device, sampler_ci.get(), nullptr, &sampler);

    // Destroy create info and Ycbcr conversion
    vkDestroySamplerYcbcrConversion(device, ycbcr, nullptr);
    sampler_ci.reset();

    // Create descriptor set layout
    auto ds_layout_ci = std::make_unique<VkDescriptorSetLayoutCreateInfo>(vku::InitStructHelper());
    auto binding = std::make_unique<VkDescriptorSetLayoutBinding>();
    ds_layout_ci->bindingCount = 1;
    ds_layout_ci->pBindings = binding.get();
    binding->descriptorCount = 1;
    binding->descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    binding->pImmutableSamplers = &sampler;
    VkDescriptorSetLayout ds_layout;
    vkCreateDescriptorSetLayout(device, ds_layout_ci.get(), nullptr, &ds_layout);

    // Destroy create info and sampler
    vkDestroySampler(device, sampler, nullptr);
    binding.release();
    ds_layout_ci.reset();

    // Create pipeline layout
    auto pipeline_layout_ci = std::make_unique<VkPipelineLayoutCreateInfo>(vku::InitStructHelper());
    pipeline_layout_ci->setLayoutCount = 1;
    pipeline_layout_ci->pSetLayouts = &ds_layout;
    VkPipelineLayout pipeline_layout;
    vkCreatePipelineLayout(device, pipeline_layout_ci.get(), nullptr, &pipeline_layout);

    // Destroy create info and descriptor set layout
    vkDestroyDescriptorSetLayout(device, ds_layout, nullptr);
    pipeline_layout_ci.release();

    // Create compute pipeline
    auto pipeline_stage_ci = std::make_unique<VkPipelineShaderStageCreateInfo>(vku::InitStructHelper());
    pipeline_stage_ci->module = shader_module;
    pipeline_stage_ci->stage = VK_SHADER_STAGE_COMPUTE_BIT;
    pipeline_stage_ci->pName = "main";
    auto compute_pipeline_ci = std::make_unique<VkComputePipelineCreateInfo>(vku::InitStructHelper());
    compute_pipeline_ci->layout = pipeline_layout;
    compute_pipeline_ci->stage = *pipeline_stage_ci;
    VkPipeline pipeline;
    vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, compute_pipeline_ci.get(), NULL, &pipeline);

    vkDestroyPipeline(device, pipeline, nullptr);
    vkDestroyShaderModule(device, shader_module, nullptr);
    vkDestroyDevice(device, nullptr);
    vkDestroyInstance(instance, nullptr);

    auto result_json = get_json(device_counter, pipeline_counter += 5);
    auto result_spirv = get_spirv(device_counter, pipeline_counter, "compute");
    EXPECT_TRUE(ValidatePipelineJson(std::string(result_json)));

    std::string ref_json = {R"({
	"ComputePipelineState" : 
	{
		"ComputePipeline" : 
		{
			"basePipelineHandle" : "",
			"basePipelineIndex" : 0,
			"flags" : 0,
			"layout" : 4,
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
					"bindingCount" : 1,
					"flags" : 0,
					"pBindings" : 
					[
						{
							"binding" : 0,
							"descriptorCount" : 1,
							"descriptorType" : "VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER",
							"pImmutableSamplers" : 
							[
								"ImmutableSampler1"
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
				"ImmutableSampler1" : 
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
						"conversion" : "YcbcrSampler1",
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
				"DescriptorSetLayout1"
			],
			"pushConstantRangeCount" : 0,
			"sType" : "VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO",
			"setLayoutCount" : 1
		},
		"ShaderFileNames" : 
		[
			{
				"filename" : "gltest_json_device_@_pipeline_#.compute.spv",
				"stage" : "VK_SHADER_STAGE_COMPUTE_BIT"
			}
		],
		"YcbcrSamplers" : 
		[
			{
				"YcbcrSampler1" : 
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
		17,
		162,
		188,
		10,
		38,
		74,
		65,
		15,
		175,
		182,
		244,
		158,
		157,
		4,
		113,
		57
	]
})"};
    write_ids(ref_json, device_counter, pipeline_counter);

    auto spirv_words_match = std::equal(result_spirv.begin(), result_spirv.end(), ref_spirv.begin(), ref_spirv.end());
    EXPECT_TRUE(spirv_words_match);
    EXPECT_EQ(result_json, ref_json);
}

/*
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
        pipeline_counter2 += SAXPY::pipeline_increment;
    }
    vkDestroyDevice(device1, nullptr);
    vkDestroyDevice(device2, nullptr);
    vkDestroyInstance(instance, nullptr);

    auto json1 = remove_filenames(get_json(device1_id, pipeline_counter1));
    auto json2 = remove_filenames(get_json(device2_id, pipeline_counter2));
    auto spirv1 = get_spirv(device1_id, pipeline_counter1, "compute");
    auto spirv2 = get_spirv(device2_id, pipeline_counter2, "compute");

    std::ofstream result_stream{"result.json"};
    result_stream << json2;
    std::ofstream ref_stream{"ref.json"};
    ref_stream << json1;

    EXPECT_EQ(json1, json2);
    EXPECT_EQ(spirv1, spirv2);
}

TEST_F(GenLayerJSONTest, GraphicsSimple) {
    TEST_DESCRIPTION("Tests whether generated pipeline JSON for a graphics pipeline is as expected");

    auto cube = Cube{};
    size_t device_id = device_counter++;
    cube.run();
    pipeline_counter += Cube::pipeline_increment;

    auto json = get_json(device_id, pipeline_counter);
    auto spirv_vert = get_spirv(device_id, pipeline_counter, "vert");
    auto spirv_frag = get_spirv(device_id, pipeline_counter, "frag");

    std::string json_ref = {R"({
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
                                                "colorWriteMask" : "VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT", "dstAlphaBlendFactor" : "VK_BLEND_FACTOR_ZERO", "dstColorBlendFactor" :
"VK_BLEND_FACTOR_ZERO", "srcAlphaBlendFactor" : "VK_BLEND_FACTOR_ZERO", "srcColorBlendFactor" : "VK_BLEND_FACTOR_ZERO"
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
                                        "dstAccessMask" : "VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT", "dstStageMask" : "VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT", "dstSubpass" : 0, "srcAccessMask" : "VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT",
                                        "srcStageMask" : "VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT", "srcSubpass" : 4294967295
                                },
                                {
                                        "dependencyFlags" : 0,
                                        "dstAccessMask" : "VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT", "dstStageMask" : "VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT", "dstSubpass" : 0,
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
                                "filename" : "gltest_json_device_@_pipeline_#.vert.spv",
                                "stage" : "VK_SHADER_STAGE_VERTEX_BIT"
                        },
                        {
                                "filename" : "gltest_json_device_@_pipeline_#.frag.spv",
                                "stage" : "VK_SHADER_STAGE_FRAGMENT_BIT"
                        }
                ]
        },
        "PipelineUUID" :
        [
                217,
                64,
                65,
                198,
                40,
                216,
                53,
                57,
                154,
                60,
                152,
                15,
                163,
                88,
                220,
                95
        ]
})"};
    write_ids(json_ref, device_id, pipeline_counter);
    const std::vector<uint32_t> spirv_vert_ref{
#include "cube.vert.inc"
    };
    const std::vector<uint32_t> spirv_frag_ref{
#include "cube.frag.inc"
    };

    EXPECT_EQ(json, json_ref);
    EXPECT_EQ(spirv_vert, spirv_vert_ref);
    EXPECT_EQ(spirv_frag, spirv_frag_ref);
}
*/