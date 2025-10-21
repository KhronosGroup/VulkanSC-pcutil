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
    std::string zero_out_uuid(std::string str) {
        return std::regex_replace(str, std::regex{R"=("PipelineUUID"[^:]*:[^[]*\[[^\]]*\])="},
                                  R"=("PipelineUUID" : [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0])=");
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
    vkDestroyPipelineLayout(device, pipeline_layout, nullptr);
    vkDestroyDescriptorSetLayout(device, ds_layout, nullptr);
    vkDestroyShaderModule(device, shader_module, nullptr);
    vkDestroyDevice(device, nullptr);
    vkDestroyInstance(instance, nullptr);

    size_t pipeline_id = 3;

    auto result_json = get_json(device_counter, pipeline_id);
    auto result_spirv = get_spirv(device_counter, pipeline_id, "compute");
    EXPECT_TRUE(ValidatePipelineJson(std::string(result_json)));

    std::string ref_json = {R"({
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
    write_ids(ref_json, device_counter, pipeline_id);

    auto spirv_words_match = std::equal(result_spirv.begin(), result_spirv.end(), ref_spirv.begin(), ref_spirv.end());
    EXPECT_TRUE(spirv_words_match);
    EXPECT_EQ(result_json, ref_json);
}

TEST_F(JSON, ComputeMultiPipeline) {
    TEST_DESCRIPTION("Tests whether generating multiple pipeline JSONs from one create call goes as expected");

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

    auto ds_layout_ci = std::make_unique<VkDescriptorSetLayoutCreateInfo>(vku::InitStructHelper());
    VkDescriptorSetLayout ds_layout;
    vkCreateDescriptorSetLayout(device, ds_layout_ci.get(), NULL, &ds_layout);

    auto pipeline_layout_ci = std::make_unique<VkPipelineLayoutCreateInfo>(vku::InitStructHelper());
    VkPipelineLayout pipeline_layout;
    vkCreatePipelineLayout(device, pipeline_layout_ci.get(), NULL, &pipeline_layout);

    std::array<VkComputePipelineCreateInfo, 2> pipeline_cis{{vku::InitStructHelper(), vku::InitStructHelper()}};
    std::array<VkPipelineShaderStageCreateInfo, 2> pipeline_stage_cis{{vku::InitStructHelper(), vku::InitStructHelper()}};
    pipeline_stage_cis[0].module = shader_module;
    pipeline_stage_cis[0].stage = VK_SHADER_STAGE_COMPUTE_BIT;
    pipeline_stage_cis[0].pName = "main1";
    pipeline_stage_cis[1].module = shader_module;
    pipeline_stage_cis[1].stage = VK_SHADER_STAGE_COMPUTE_BIT;
    pipeline_stage_cis[1].pName = "main2";
    pipeline_cis[0].layout = pipeline_layout;
    pipeline_cis[0].stage = pipeline_stage_cis[0];
    pipeline_cis[1].layout = pipeline_layout;
    pipeline_cis[1].stage = pipeline_stage_cis[1];
    std::array<VkPipeline, 2> pipelines;
    vkCreateComputePipelines(device, VK_NULL_HANDLE, (uint32_t)pipeline_cis.size(), pipeline_cis.data(), NULL, pipelines.data());

    for (auto& pipeline : pipelines) {
        vkDestroyPipeline(device, pipeline, nullptr);
    }
    vkDestroyPipelineLayout(device, pipeline_layout, nullptr);
    vkDestroyDescriptorSetLayout(device, ds_layout, nullptr);
    vkDestroyShaderModule(device, shader_module, nullptr);
    vkDestroyDevice(device, nullptr);
    vkDestroyInstance(instance, nullptr);

    size_t pipeline_id1 = 3;
    size_t pipeline_id2 = 4;

    auto result_json1 = zero_out_uuid(get_json(device_counter, pipeline_id1));
    auto result_json2 = zero_out_uuid(get_json(device_counter, pipeline_id2));
    auto result_spirv1 = get_spirv(device_counter, pipeline_id1, "compute");
    auto result_spirv2 = get_spirv(device_counter, pipeline_id2, "compute");
    EXPECT_TRUE(ValidatePipelineJson(std::string(result_json1)));
    EXPECT_TRUE(ValidatePipelineJson(std::string(result_json2)));

    std::string ref_json1 = {R"({
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
				"pName" : "main1",
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
	"PipelineUUID" : [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
})"};
    std::string ref_json2 = ref_json1;
    ref_json2.replace(ref_json2.find("main1"), strlen("main1"), "main2");
    write_ids(ref_json1, device_counter, pipeline_id1);
    write_ids(ref_json2, device_counter, pipeline_id2);

    auto spirv_words_match1 = std::equal(result_spirv1.begin(), result_spirv1.end(), ref_spirv.begin(), ref_spirv.end());
    auto spirv_words_match2 = std::equal(result_spirv2.begin(), result_spirv2.end(), ref_spirv.begin(), ref_spirv.end());
    EXPECT_TRUE(spirv_words_match1);
    EXPECT_TRUE(spirv_words_match2);
    EXPECT_EQ(result_json1, ref_json1);
    EXPECT_EQ(result_json2, ref_json2);
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

    vkDestroyPipelineLayout(device, pipeline_layout, nullptr);
    vkDestroyPipeline(device, pipeline, nullptr);
    vkDestroyShaderModule(device, shader_module, nullptr);
    vkDestroyDevice(device, nullptr);
    vkDestroyInstance(instance, nullptr);

    size_t pipeline1_id = 5;

    auto result_json = get_json(device_counter, pipeline1_id);
    auto result_spirv = get_spirv(device_counter, pipeline1_id, "compute");
    EXPECT_TRUE(ValidatePipelineJson(std::string(result_json)));

    std::string ref_json = {R"({
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
    write_ids(ref_json, device_counter, pipeline1_id);

    auto spirv_words_match = std::equal(result_spirv.begin(), result_spirv.end(), ref_spirv.begin(), ref_spirv.end());
    EXPECT_TRUE(spirv_words_match);
    EXPECT_EQ(result_json, ref_json);
}

TEST_F(JSON, ComputeReproducible) {
    TEST_DESCRIPTION("Tests whether identical API streams produce the same JSON");

    auto instance_ci = std::make_unique<VkInstanceCreateInfo>(vku::InitStructHelper());
    VkInstance instance;
    vkCreateInstance(instance_ci.get(), nullptr, &instance);

    uint32_t phys_dev_count = 1;
    std::vector<VkPhysicalDevice> phys_devs(phys_dev_count);
    vkEnumeratePhysicalDevices(instance, &phys_dev_count, phys_devs.data());

    auto device_ci = std::make_unique<VkDeviceCreateInfo>(vku::InitStructHelper());
    VkDevice device1, device2;
    vkCreateDevice(phys_devs[0], device_ci.get(), nullptr, &device1);
    vkCreateDevice(phys_devs[0], device_ci.get(), nullptr, &device2);

    auto create_pipeline = [](VkDevice device) {
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

        // Create Ycbcr sampler
        auto ycbcr_info = std::make_unique<VkSamplerYcbcrConversionInfo>(vku::InitStructHelper());
        ycbcr_info->conversion = ycbcr;
        auto sampler_ci = std::make_unique<VkSamplerCreateInfo>(vku::InitStructHelper());
        sampler_ci->pNext = ycbcr_info.get();
        VkSampler sampler;
        vkCreateSampler(device, sampler_ci.get(), nullptr, &sampler);

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

        // Create pipeline layout
        auto pipeline_layout_ci = std::make_unique<VkPipelineLayoutCreateInfo>(vku::InitStructHelper());
        pipeline_layout_ci->setLayoutCount = 1;
        pipeline_layout_ci->pSetLayouts = &ds_layout;
        VkPipelineLayout pipeline_layout;
        vkCreatePipelineLayout(device, pipeline_layout_ci.get(), nullptr, &pipeline_layout);

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

        vkDestroySamplerYcbcrConversion(device, ycbcr, nullptr);
        vkDestroySampler(device, sampler, nullptr);
        vkDestroyDescriptorSetLayout(device, ds_layout, nullptr);
        vkDestroyShaderModule(device, shader_module, nullptr);

        return pipeline;
    };

    auto pipeline1 = create_pipeline(device1);
    auto pipeline2 = create_pipeline(device2);
    auto pipeline3 = create_pipeline(device2);

    vkDestroyPipeline(device1, pipeline1, nullptr);
    vkDestroyPipeline(device2, pipeline2, nullptr);
    vkDestroyPipeline(device2, pipeline3, nullptr);

    size_t device1_id = device_counter++;
    size_t device2_id = device_counter++;
    size_t device3_id = device2_id;
    size_t pipeline1_id = 5;
    size_t pipeline2_id = 5;
    size_t pipeline3_id = 10;

    auto json1 = remove_filenames(get_json(device1_id, pipeline1_id));
    auto json2 = remove_filenames(get_json(device2_id, pipeline2_id));
    auto json3 = remove_filenames(get_json(device3_id, pipeline3_id));
    auto spirv1 = get_spirv(device1_id, pipeline1_id, "compute");
    auto spirv2 = get_spirv(device2_id, pipeline2_id, "compute");
    auto spirv3 = get_spirv(device3_id, pipeline3_id, "compute");

    std::ofstream result_stream{"result.json"};
    result_stream << json2;
    std::ofstream ref_stream{"ref.json"};
    ref_stream << json1;

    EXPECT_EQ(json1, json2);
    EXPECT_EQ(json2, json3);
    EXPECT_EQ(spirv1, spirv2);
    EXPECT_EQ(spirv2, spirv3);
}

TEST_F(JSON, ComputeComplex) {
    TEST_DESCRIPTION("Tests whether generated pipeline JSON for a complex compute pipeline is as expected");

    auto instance_ci = std::make_unique<VkInstanceCreateInfo>(vku::InitStructHelper());
    std::vector<const char*> enabled_instance_extensions = {"VK_EXT_headless_surface"};
    instance_ci->enabledExtensionCount = (uint32_t)enabled_instance_extensions.size();
    instance_ci->ppEnabledExtensionNames = enabled_instance_extensions.data();
    VkInstance instance;
    vkCreateInstance(instance_ci.get(), NULL, &instance);

    uint32_t phys_dev_count = 1;
    std::vector<VkPhysicalDevice> phys_devs(phys_dev_count);
    vkEnumeratePhysicalDevices(instance, &phys_dev_count, phys_devs.data());

    auto device_ci = std::make_unique<VkDeviceCreateInfo>(vku::InitStructHelper());
    std::vector<const char*> enabled_device_extensions = {"VK_KHR_shader_clock"};
    device_ci->enabledExtensionCount = (uint32_t)enabled_device_extensions.size();
    device_ci->ppEnabledExtensionNames = enabled_device_extensions.data();
    auto pdf2 = std::make_unique<VkPhysicalDeviceFeatures2>(vku::InitStructHelper());
    auto pdscf = std::make_unique<VkPhysicalDeviceShaderClockFeaturesKHR>(vku::InitStructHelper());
    pdf2->pNext = pdscf.get();
    pdscf->shaderSubgroupClock = VK_TRUE;
    VkDevice device;
    vkCreateDevice(phys_devs[0], device_ci.get(), NULL, &device);

    auto shader_module_ci = std::make_unique<VkShaderModuleCreateInfo>(vku::InitStructHelper());
    std::vector<uint32_t> ref_spirv{1, 2, 3, 4};
    shader_module_ci->codeSize = ref_spirv.size() * sizeof(uint32_t);
    shader_module_ci->pCode = ref_spirv.data();
    VkShaderModule shader_module;
    vkCreateShaderModule(device, shader_module_ci.get(), NULL, &shader_module);

    // Create Ycbcr sampler conversion
    auto ycbcr_ci = std::make_unique<VkSamplerYcbcrConversionCreateInfo>(vku::InitStructHelper());
#ifdef VK_USE_PLATFORM_SCREEN_QNX
    VkExternalFormatQNX ef_qnx{};
    ef_qnx.sType = VK_STRUCTURE_TYPE_EXTERNAL_FORMAT_QNX;
    ef_qnx.pNext = nullptr;
    ef_qnx.externalFormat = 10;
    ycbcr_ci->pNext = &ef_qnx;
#else
#endif  // VK_USE_PLATFORM_SCREEN_QNX
    ycbcr_ci->format = VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16;
    ycbcr_ci->ycbcrModel = VK_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_2020;
    ycbcr_ci->ycbcrRange = VK_SAMPLER_YCBCR_RANGE_ITU_NARROW;
    ycbcr_ci->components.r = VK_COMPONENT_SWIZZLE_A;
    ycbcr_ci->components.g = VK_COMPONENT_SWIZZLE_B;
    ycbcr_ci->components.b = VK_COMPONENT_SWIZZLE_G;
    ycbcr_ci->components.a = VK_COMPONENT_SWIZZLE_R;
    ycbcr_ci->xChromaOffset = VK_CHROMA_LOCATION_COSITED_EVEN;
    ycbcr_ci->yChromaOffset = VK_CHROMA_LOCATION_MIDPOINT;
    ycbcr_ci->chromaFilter = VK_FILTER_CUBIC_EXT;
    ycbcr_ci->forceExplicitReconstruction = VK_TRUE;
    VkSamplerYcbcrConversion ycbcr;
    vkCreateSamplerYcbcrConversion(device, ycbcr_ci.get(), nullptr, &ycbcr);

    // Create Ycbcr sampler
    std::array<VkSamplerCreateInfo, 3> sampler_cis{{vku::InitStructHelper(), vku::InitStructHelper(), vku::InitStructHelper()}};
    auto ycbcr_info = std::make_unique<VkSamplerYcbcrConversionInfo>(vku::InitStructHelper());
    auto srm_ci = std::make_unique<VkSamplerReductionModeCreateInfo>(vku::InitStructHelper());
    ycbcr_info->conversion = ycbcr;
    sampler_cis[0].pNext = ycbcr_info.get();
    sampler_cis[1].magFilter = VK_FILTER_CUBIC_EXT;
    sampler_cis[1].minFilter = VK_FILTER_NEAREST;
    sampler_cis[1].mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_cis[1].addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_cis[1].addressModeV = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    sampler_cis[1].addressModeW = VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
    sampler_cis[1].mipLodBias = 0.5f;
    sampler_cis[1].anisotropyEnable = VK_FALSE;
    sampler_cis[1].maxAnisotropy = 2.0f;
    sampler_cis[1].compareEnable = VK_FALSE;
    sampler_cis[1].compareOp = VK_COMPARE_OP_NOT_EQUAL;
    sampler_cis[1].minLod = 0.0f;
    sampler_cis[1].maxLod = VK_LOD_CLAMP_NONE;
    sampler_cis[1].borderColor = VK_BORDER_COLOR_FLOAT_CUSTOM_EXT;
    sampler_cis[1].unnormalizedCoordinates = VK_TRUE;
    srm_ci->reductionMode = VK_SAMPLER_REDUCTION_MODE_MAX;
    sampler_cis[2].pNext = srm_ci.get();
    std::array<VkSampler, 3> samplers;
    vkCreateSampler(device, &sampler_cis[0], nullptr, &samplers[0]);
    vkCreateSampler(device, &sampler_cis[1], nullptr, &samplers[1]);
    vkCreateSampler(device, &sampler_cis[2], nullptr, &samplers[2]);

    // Create descriptor set layouts
    std::array<VkDescriptorSetLayoutCreateInfo, 3> ds_layout_cis{
        {vku::InitStructHelper(), vku::InitStructHelper(), vku::InitStructHelper()}};
    auto binding = std::make_unique<VkDescriptorSetLayoutBinding>();
    binding->descriptorCount = 1;
    binding->descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    binding->pImmutableSamplers = &samplers[0];
    ds_layout_cis[0].bindingCount = 1;
    ds_layout_cis[0].pBindings = binding.get();
    std::array<VkDescriptorSetLayout, 3> ds_layouts;
    vkCreateDescriptorSetLayout(device, &ds_layout_cis[0], nullptr, &ds_layouts[0]);
    binding->descriptorCount = 1;
    binding->descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
    binding->pImmutableSamplers = &samplers[1];
    ds_layout_cis[1].bindingCount = 1;
    ds_layout_cis[1].pBindings = binding.get();
    vkCreateDescriptorSetLayout(device, &ds_layout_cis[1], nullptr, &ds_layouts[1]);
    auto ds_layout_binding_flags_ci = std::make_unique<VkDescriptorSetLayoutBindingFlagsCreateInfo>(vku::InitStructHelper());
    std::array<const VkDescriptorBindingFlags, 1> ds_layout_binding_flags_arr{{VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT}};
    ds_layout_binding_flags_ci->bindingCount = 1;
    ds_layout_binding_flags_ci->pBindingFlags = ds_layout_binding_flags_arr.data();
    binding->descriptorCount = 1;
    binding->descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    binding->pImmutableSamplers = &samplers[2];
    ds_layout_cis[2].bindingCount = 1;
    ds_layout_cis[2].pBindings = binding.get();
    ds_layout_cis[2].pNext = ds_layout_binding_flags_ci.get();
    vkCreateDescriptorSetLayout(device, &ds_layout_cis[2], nullptr, &ds_layouts[2]);

    // Create pipeline layout
    auto pipeline_layout_ci = std::make_unique<VkPipelineLayoutCreateInfo>(vku::InitStructHelper());
    std::array<VkPushConstantRange, 1> push_constants{{}};
    push_constants[0].offset = 4;
    push_constants[0].size = 8;
    push_constants[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    pipeline_layout_ci->setLayoutCount = (uint32_t)ds_layouts.size();
    pipeline_layout_ci->pSetLayouts = ds_layouts.data();
    pipeline_layout_ci->pushConstantRangeCount = (uint32_t)push_constants.size();
    pipeline_layout_ci->pPushConstantRanges = push_constants.data();
    VkPipelineLayout pipeline_layout;
    vkCreatePipelineLayout(device, pipeline_layout_ci.get(), nullptr, &pipeline_layout);

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

    vkDestroySamplerYcbcrConversion(device, ycbcr, nullptr);
    for (auto& sampler : samplers) {
        vkDestroySampler(device, sampler, nullptr);
    }
    for (auto& ds_layout : ds_layouts) {
        vkDestroyDescriptorSetLayout(device, ds_layout, nullptr);
    }
    vkDestroyPipelineLayout(device, pipeline_layout, nullptr);
    vkDestroyPipeline(device, pipeline, nullptr);
    vkDestroyShaderModule(device, shader_module, nullptr);
    vkDestroyDevice(device, nullptr);
    vkDestroyInstance(instance, nullptr);

    size_t pipeline_id = 9;

    auto result_json = get_json(device_counter, pipeline_id);
    auto result_spirv = get_spirv(device_counter, pipeline_id, "compute");
    EXPECT_TRUE(ValidatePipelineJson(std::string(result_json)));

    std::string ref_json = {R"({
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
			},
			{
				"DescriptorSetLayout2" : 
				{
					"bindingCount" : 1,
					"flags" : 0,
					"pBindings" : 
					[
						{
							"binding" : 0,
							"descriptorCount" : 1,
							"descriptorType" : "VK_DESCRIPTOR_TYPE_SAMPLER",
							"pImmutableSamplers" : 
							[
								"ImmutableSampler2"
							],
							"stageFlags" : 0
						}
					],
					"pNext" : "NULL",
					"sType" : "VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO"
				}
			},
			{
				"DescriptorSetLayout3" : 
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
								"ImmutableSampler3"
							],
							"stageFlags" : 0
						}
					],
					"pNext" : 
					{
						"bindingCount" : 1,
						"pBindingFlags" : 
						[
							"VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT"
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
			},
			{
				"ImmutableSampler2" : 
				{
					"addressModeU" : "VK_SAMPLER_ADDRESS_MODE_REPEAT",
					"addressModeV" : "VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT",
					"addressModeW" : "VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE",
					"anisotropyEnable" : "VK_FALSE",
					"borderColor" : "VK_BORDER_COLOR_FLOAT_CUSTOM_EXT",
					"compareEnable" : "VK_FALSE",
					"compareOp" : "VK_COMPARE_OP_NOT_EQUAL",
					"flags" : 0,
					"magFilter" : "VK_FILTER_CUBIC_EXT",
					"maxAnisotropy" : 2.0,
					"maxLod" : 1000.0,
					"minFilter" : "VK_FILTER_NEAREST",
					"minLod" : 0.0,
					"mipLodBias" : 0.5,
					"mipmapMode" : "VK_SAMPLER_MIPMAP_MODE_LINEAR",
					"pNext" : "NULL",
					"sType" : "VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO",
					"unnormalizedCoordinates" : "VK_TRUE"
				}
			},
			{
				"ImmutableSampler3" : 
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
						"pNext" : "NULL",
						"reductionMode" : "VK_SAMPLER_REDUCTION_MODE_MAX",
						"sType" : "VK_STRUCTURE_TYPE_SAMPLER_REDUCTION_MODE_CREATE_INFO"
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
			"pPushConstantRanges" : 
			[
				{
					"offset" : 4,
					"size" : 8,
					"stageFlags" : "VK_SHADER_STAGE_COMPUTE_BIT"
				}
			],
			"pSetLayouts" : 
			[
				"DescriptorSetLayout1",
				"DescriptorSetLayout2",
				"DescriptorSetLayout3"
			],
			"pushConstantRangeCount" : 1,
			"sType" : "VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO",
			"setLayoutCount" : 3
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
		"VK_KHR_shader_clock"
	],
	"PipelineUUID" : 
	[
		75,
		246,
		160,
		119,
		209,
		198,
		49,
		209,
		246,
		110,
		148,
		155,
		197,
		108,
		143,
		144
	]
})"};
    write_ids(ref_json, device_counter, pipeline_id);

    auto spirv_words_match = std::equal(result_spirv.begin(), result_spirv.end(), ref_spirv.begin(), ref_spirv.end());
    EXPECT_TRUE(spirv_words_match);
    EXPECT_EQ(result_json, ref_json);
}

TEST_F(JSON, GraphicsSimple) {
    TEST_DESCRIPTION("Tests whether generated pipeline JSON for a minimal graphics pipeline is as expected");

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

    auto ds_layout_ci = std::make_unique<VkDescriptorSetLayoutCreateInfo>(vku::InitStructHelper());
    VkDescriptorSetLayout ds_layout;
    vkCreateDescriptorSetLayout(device, ds_layout_ci.get(), NULL, &ds_layout);

    auto pipeline_layout_ci = std::make_unique<VkPipelineLayoutCreateInfo>(vku::InitStructHelper());
    VkPipelineLayout pipeline_layout;
    vkCreatePipelineLayout(device, pipeline_layout_ci.get(), NULL, &pipeline_layout);

    auto renderpass_ci = std::make_unique<VkRenderPassCreateInfo>(vku::InitStructHelper());
    VkRenderPass render_pass;
    vkCreateRenderPass(device, renderpass_ci.get(), nullptr, &render_pass);

    auto graphics_pipeline_ci = std::make_unique<VkGraphicsPipelineCreateInfo>(vku::InitStructHelper());
    auto pipeline_stage_ci = std::make_unique<VkPipelineShaderStageCreateInfo>(vku::InitStructHelper());
    pipeline_stage_ci->module = shader_module;
    pipeline_stage_ci->stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    pipeline_stage_ci->pName = "main";
    graphics_pipeline_ci->layout = pipeline_layout;
    graphics_pipeline_ci->stageCount = 1;
    graphics_pipeline_ci->pStages = pipeline_stage_ci.get();
    graphics_pipeline_ci->renderPass = render_pass;
    VkPipeline pipeline;
    vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, graphics_pipeline_ci.get(), NULL, &pipeline);

    vkDestroyPipeline(device, pipeline, nullptr);
    vkDestroyRenderPass(device, render_pass, nullptr);
    vkDestroyPipelineLayout(device, pipeline_layout, nullptr);
    vkDestroyDescriptorSetLayout(device, ds_layout, nullptr);
    vkDestroyShaderModule(device, shader_module, nullptr);
    vkDestroyDevice(device, nullptr);
    vkDestroyInstance(instance, nullptr);

    size_t pipeline_id = 3;

    auto result_json = get_json(device_counter, pipeline_id);
    auto result_spirv = get_spirv(device_counter, pipeline_id, "frag");
    EXPECT_TRUE(ValidatePipelineJson(std::string(result_json)));

    std::string ref_json = {R"({
	"EnabledExtensions" : [],
	"GraphicsPipelineState" : 
	{
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
					"stage" : "VK_SHADER_STAGE_FRAGMENT_BIT"
				}
			],
			"pTessellationState" : "NULL",
			"pVertexInputState" : "NULL",
			"pViewportState" : "NULL",
			"renderPass" : "",
			"sType" : "VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO",
			"stageCount" : 1,
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
			"pSetLayouts" : "NULL",
			"pushConstantRangeCount" : 0,
			"sType" : "VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO",
			"setLayoutCount" : 0
		},
		"Renderpass" : 
		{
			"attachmentCount" : 0,
			"dependencyCount" : 0,
			"flags" : 0,
			"pAttachments" : "NULL",
			"pDependencies" : "NULL",
			"pNext" : "NULL",
			"pSubpasses" : "NULL",
			"sType" : "VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO",
			"subpassCount" : 0
		},
		"ShaderFileNames" : 
		[
			{
				"filename" : "gltest_json_device_@_pipeline_#.frag.spv",
				"stage" : "VK_SHADER_STAGE_FRAGMENT_BIT"
			}
		]
	},
	"PipelineUUID" : 
	[
		172,
		140,
		31,
		159,
		143,
		235,
		178,
		73,
		188,
		141,
		213,
		160,
		169,
		41,
		214,
		207
	]
})"};
    write_ids(ref_json, device_counter, pipeline_id);

    auto spirv_words_match = std::equal(result_spirv.begin(), result_spirv.end(), ref_spirv.begin(), ref_spirv.end());
    EXPECT_TRUE(spirv_words_match);
    EXPECT_EQ(result_json, ref_json);
}

TEST_F(JSON, GraphicsMultiPipeline) {
    TEST_DESCRIPTION("Tests whether generated pipeline JSON for a minimal graphics pipeline is as expected");

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

    auto ds_layout_ci = std::make_unique<VkDescriptorSetLayoutCreateInfo>(vku::InitStructHelper());
    VkDescriptorSetLayout ds_layout;
    vkCreateDescriptorSetLayout(device, ds_layout_ci.get(), NULL, &ds_layout);

    auto pipeline_layout_ci = std::make_unique<VkPipelineLayoutCreateInfo>(vku::InitStructHelper());
    VkPipelineLayout pipeline_layout;
    vkCreatePipelineLayout(device, pipeline_layout_ci.get(), NULL, &pipeline_layout);

    auto renderpass_ci = std::make_unique<VkRenderPassCreateInfo>(vku::InitStructHelper());
    VkRenderPass render_pass;
    vkCreateRenderPass(device, renderpass_ci.get(), nullptr, &render_pass);

    std::array<VkGraphicsPipelineCreateInfo, 2> graphics_pipeline_cis{{vku::InitStructHelper(), vku::InitStructHelper()}};
    std::array<VkPipelineShaderStageCreateInfo, 2> pipeline_stage_cis{{vku::InitStructHelper(), vku::InitStructHelper()}};
    pipeline_stage_cis[0].module = shader_module;
    pipeline_stage_cis[0].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    pipeline_stage_cis[0].pName = "main1";
    pipeline_stage_cis[1].module = shader_module;
    pipeline_stage_cis[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    pipeline_stage_cis[1].pName = "main2";
    graphics_pipeline_cis[0].layout = pipeline_layout;
    graphics_pipeline_cis[0].stageCount = 1;
    graphics_pipeline_cis[0].pStages = &pipeline_stage_cis[0];
    graphics_pipeline_cis[0].renderPass = render_pass;
    graphics_pipeline_cis[1].layout = pipeline_layout;
    graphics_pipeline_cis[1].stageCount = 1;
    graphics_pipeline_cis[1].pStages = &pipeline_stage_cis[1];
    graphics_pipeline_cis[1].renderPass = render_pass;
    std::array<VkPipeline, 2> pipelines;
    vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, (uint32_t)graphics_pipeline_cis.size(), graphics_pipeline_cis.data(), NULL,
                              pipelines.data());

    for (auto& pipeline : pipelines) {
        vkDestroyPipeline(device, pipeline, nullptr);
    }
    vkDestroyRenderPass(device, render_pass, nullptr);
    vkDestroyPipelineLayout(device, pipeline_layout, nullptr);
    vkDestroyDescriptorSetLayout(device, ds_layout, nullptr);
    vkDestroyShaderModule(device, shader_module, nullptr);
    vkDestroyDevice(device, nullptr);
    vkDestroyInstance(instance, nullptr);

    size_t pipeline_id1 = 3;
    size_t pipeline_id2 = 4;

    auto result_json1 = zero_out_uuid(get_json(device_counter, pipeline_id1));
    auto result_json2 = zero_out_uuid(get_json(device_counter, pipeline_id2));
    auto result_spirv1 = get_spirv(device_counter, pipeline_id1, "frag");
    auto result_spirv2 = get_spirv(device_counter, pipeline_id2, "frag");
    EXPECT_TRUE(ValidatePipelineJson(std::string(result_json1)));
    EXPECT_TRUE(ValidatePipelineJson(std::string(result_json2)));

    std::string ref_json1 = {R"({
	"EnabledExtensions" : [],
	"GraphicsPipelineState" : 
	{
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
					"pName" : "main1",
					"pNext" : "NULL",
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
			"stageCount" : 1,
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
			"pSetLayouts" : "NULL",
			"pushConstantRangeCount" : 0,
			"sType" : "VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO",
			"setLayoutCount" : 0
		},
		"Renderpass" : 
		{
			"attachmentCount" : 0,
			"dependencyCount" : 0,
			"flags" : 0,
			"pAttachments" : "NULL",
			"pDependencies" : "NULL",
			"pNext" : "NULL",
			"pSubpasses" : "NULL",
			"sType" : "VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO",
			"subpassCount" : 0
		},
		"ShaderFileNames" : 
		[
			{
				"filename" : "gltest_json_device_@_pipeline_#.frag.spv",
				"stage" : "VK_SHADER_STAGE_FRAGMENT_BIT"
			}
		]
	},
	"PipelineUUID" : [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
})"};
    std::string ref_json2 = ref_json1;
    ref_json2.replace(ref_json2.find("main1"), strlen("main1"), "main2");
    write_ids(ref_json1, device_counter, pipeline_id1);
    write_ids(ref_json2, device_counter, pipeline_id2);

    std::ofstream result_stream1{"result.json"};
    result_stream1 << result_json1;
    std::ofstream ref_stream1{"ref.json"};
    ref_stream1 << ref_json1;

    auto spirv_words_match1 = std::equal(result_spirv1.begin(), result_spirv1.end(), ref_spirv.begin(), ref_spirv.end());
    auto spirv_words_match2 = std::equal(result_spirv2.begin(), result_spirv2.end(), ref_spirv.begin(), ref_spirv.end());
    EXPECT_TRUE(spirv_words_match1);
    EXPECT_TRUE(spirv_words_match2);
    EXPECT_EQ(result_json1, ref_json1);
    EXPECT_EQ(result_json2, ref_json2);
}

TEST_F(JSON, GraphicsLifetime) {
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

    // Create renderass
    auto renderpass_ci = std::make_unique<VkRenderPassCreateInfo>(vku::InitStructHelper());
    VkRenderPass render_pass;
    vkCreateRenderPass(device, renderpass_ci.get(), nullptr, &render_pass);

    // Destroy create info
    renderpass_ci.release();

    // Create graphics pipeline
    auto graphics_pipeline_ci = std::make_unique<VkGraphicsPipelineCreateInfo>(vku::InitStructHelper());
    auto pipeline_stage_ci = std::make_unique<VkPipelineShaderStageCreateInfo>(vku::InitStructHelper());
    pipeline_stage_ci->module = shader_module;
    pipeline_stage_ci->stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    pipeline_stage_ci->pName = "main";
    graphics_pipeline_ci->layout = pipeline_layout;
    graphics_pipeline_ci->stageCount = 1;
    graphics_pipeline_ci->pStages = pipeline_stage_ci.get();
    graphics_pipeline_ci->renderPass = render_pass;
    VkPipeline pipeline;
    vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, graphics_pipeline_ci.get(), NULL, &pipeline);

    vkDestroyPipeline(device, pipeline, nullptr);
    vkDestroyRenderPass(device, render_pass, nullptr);
    vkDestroyPipelineLayout(device, pipeline_layout, nullptr);
    vkDestroyDescriptorSetLayout(device, ds_layout, nullptr);
    vkDestroyShaderModule(device, shader_module, nullptr);
    vkDestroyDevice(device, nullptr);
    vkDestroyInstance(instance, nullptr);

    size_t pipeline_id = 5;

    auto result_json = get_json(device_counter, pipeline_id);
    auto result_spirv = get_spirv(device_counter, pipeline_id, "frag");
    EXPECT_TRUE(ValidatePipelineJson(std::string(result_json)));

    std::string ref_json = {R"({
	"EnabledExtensions" : [],
	"GraphicsPipelineState" : 
	{
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
					"stage" : "VK_SHADER_STAGE_FRAGMENT_BIT"
				}
			],
			"pTessellationState" : "NULL",
			"pVertexInputState" : "NULL",
			"pViewportState" : "NULL",
			"renderPass" : "",
			"sType" : "VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO",
			"stageCount" : 1,
			"subpass" : 0
		},
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
		"Renderpass" : 
		{
			"attachmentCount" : 0,
			"dependencyCount" : 0,
			"flags" : 0,
			"pAttachments" : "NULL",
			"pDependencies" : "NULL",
			"pNext" : "NULL",
			"pSubpasses" : "NULL",
			"sType" : "VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO",
			"subpassCount" : 0
		},
		"ShaderFileNames" : 
		[
			{
				"filename" : "gltest_json_device_@_pipeline_#.frag.spv",
				"stage" : "VK_SHADER_STAGE_FRAGMENT_BIT"
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
	"PipelineUUID" : 
	[
		96,
		121,
		101,
		128,
		46,
		19,
		161,
		232,
		23,
		157,
		70,
		122,
		113,
		0,
		173,
		88
	]
})"};
    write_ids(ref_json, device_counter, pipeline_id);

    auto spirv_words_match = std::equal(result_spirv.begin(), result_spirv.end(), ref_spirv.begin(), ref_spirv.end());
    EXPECT_TRUE(spirv_words_match);
    EXPECT_EQ(result_json, ref_json);
}

TEST_F(JSON, GraphicsComplex) {
    TEST_DESCRIPTION("Tests whether generated pipeline JSON for a complex graphics pipeline is as expected");

    auto instance_ci = std::make_unique<VkInstanceCreateInfo>(vku::InitStructHelper());
    std::vector<const char*> enabled_instance_extensions = {"VK_EXT_headless_surface"};
    instance_ci->enabledExtensionCount = (uint32_t)enabled_instance_extensions.size();
    instance_ci->ppEnabledExtensionNames = enabled_instance_extensions.data();
    VkInstance instance;
    vkCreateInstance(instance_ci.get(), NULL, &instance);

    uint32_t phys_dev_count = 1;
    std::vector<VkPhysicalDevice> phys_devs(phys_dev_count);
    vkEnumeratePhysicalDevices(instance, &phys_dev_count, phys_devs.data());

    auto device_ci = std::make_unique<VkDeviceCreateInfo>(vku::InitStructHelper());
    std::vector<const char*> enabled_device_extensions = {"VK_KHR_shader_clock"};
    device_ci->enabledExtensionCount = (uint32_t)enabled_device_extensions.size();
    device_ci->ppEnabledExtensionNames = enabled_device_extensions.data();
    auto pdf2 = std::make_unique<VkPhysicalDeviceFeatures2>(vku::InitStructHelper());
    auto pdscf = std::make_unique<VkPhysicalDeviceShaderClockFeaturesKHR>(vku::InitStructHelper());
    pdf2->pNext = pdscf.get();
    pdscf->shaderSubgroupClock = VK_TRUE;
    VkDevice device;
    vkCreateDevice(phys_devs[0], device_ci.get(), NULL, &device);

    auto shader_module_ci = std::make_unique<VkShaderModuleCreateInfo>(vku::InitStructHelper());
    std::vector<uint32_t> ref_spirv{1, 2, 3, 4};
    shader_module_ci->codeSize = ref_spirv.size() * sizeof(uint32_t);
    shader_module_ci->pCode = ref_spirv.data();
    VkShaderModule shader_module;
    vkCreateShaderModule(device, shader_module_ci.get(), NULL, &shader_module);

    // Create Ycbcr sampler conversion
    auto ycbcr_ci = std::make_unique<VkSamplerYcbcrConversionCreateInfo>(vku::InitStructHelper());
#ifdef VK_USE_PLATFORM_SCREEN_QNX
    VkExternalFormatQNX ef_qnx{};
    ef_qnx.sType = VK_STRUCTURE_TYPE_EXTERNAL_FORMAT_QNX;
    ef_qnx.pNext = nullptr;
    ef_qnx.externalFormat = 10;
    ycbcr_ci->pNext = &ef_qnx;
#else
#endif  // VK_USE_PLATFORM_SCREEN_QNX
    ycbcr_ci->format = VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16;
    ycbcr_ci->ycbcrModel = VK_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_2020;
    ycbcr_ci->ycbcrRange = VK_SAMPLER_YCBCR_RANGE_ITU_NARROW;
    ycbcr_ci->components.r = VK_COMPONENT_SWIZZLE_A;
    ycbcr_ci->components.g = VK_COMPONENT_SWIZZLE_B;
    ycbcr_ci->components.b = VK_COMPONENT_SWIZZLE_G;
    ycbcr_ci->components.a = VK_COMPONENT_SWIZZLE_R;
    ycbcr_ci->xChromaOffset = VK_CHROMA_LOCATION_COSITED_EVEN;
    ycbcr_ci->yChromaOffset = VK_CHROMA_LOCATION_MIDPOINT;
    ycbcr_ci->chromaFilter = VK_FILTER_CUBIC_EXT;
    ycbcr_ci->forceExplicitReconstruction = VK_TRUE;
    VkSamplerYcbcrConversion ycbcr;
    vkCreateSamplerYcbcrConversion(device, ycbcr_ci.get(), nullptr, &ycbcr);

    // Create Ycbcr sampler
    std::array<VkSamplerCreateInfo, 3> sampler_cis{{vku::InitStructHelper(), vku::InitStructHelper(), vku::InitStructHelper()}};
    auto ycbcr_info = std::make_unique<VkSamplerYcbcrConversionInfo>(vku::InitStructHelper());
    auto srm_ci = std::make_unique<VkSamplerReductionModeCreateInfo>(vku::InitStructHelper());
    ycbcr_info->conversion = ycbcr;
    sampler_cis[0].pNext = ycbcr_info.get();
    sampler_cis[1].magFilter = VK_FILTER_CUBIC_EXT;
    sampler_cis[1].minFilter = VK_FILTER_NEAREST;
    sampler_cis[1].mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_cis[1].addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_cis[1].addressModeV = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    sampler_cis[1].addressModeW = VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
    sampler_cis[1].mipLodBias = 0.5f;
    sampler_cis[1].anisotropyEnable = VK_FALSE;
    sampler_cis[1].maxAnisotropy = 2.0f;
    sampler_cis[1].compareEnable = VK_FALSE;
    sampler_cis[1].compareOp = VK_COMPARE_OP_NOT_EQUAL;
    sampler_cis[1].minLod = 0.0f;
    sampler_cis[1].maxLod = VK_LOD_CLAMP_NONE;
    sampler_cis[1].borderColor = VK_BORDER_COLOR_FLOAT_CUSTOM_EXT;
    sampler_cis[1].unnormalizedCoordinates = VK_TRUE;
    srm_ci->reductionMode = VK_SAMPLER_REDUCTION_MODE_MAX;
    sampler_cis[2].pNext = srm_ci.get();
    std::array<VkSampler, 3> samplers;
    vkCreateSampler(device, &sampler_cis[0], nullptr, &samplers[0]);
    vkCreateSampler(device, &sampler_cis[1], nullptr, &samplers[1]);
    vkCreateSampler(device, &sampler_cis[2], nullptr, &samplers[2]);

    // Create descriptor set layouts
    std::array<VkDescriptorSetLayoutCreateInfo, 3> ds_layout_cis{
        {vku::InitStructHelper(), vku::InitStructHelper(), vku::InitStructHelper()}};
    auto binding = std::make_unique<VkDescriptorSetLayoutBinding>();
    binding->descriptorCount = 1;
    binding->descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    binding->pImmutableSamplers = &samplers[0];
    ds_layout_cis[0].bindingCount = 1;
    ds_layout_cis[0].pBindings = binding.get();
    std::array<VkDescriptorSetLayout, 3> ds_layouts;
    vkCreateDescriptorSetLayout(device, &ds_layout_cis[0], nullptr, &ds_layouts[0]);
    binding->descriptorCount = 1;
    binding->descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
    binding->pImmutableSamplers = &samplers[1];
    ds_layout_cis[1].bindingCount = 1;
    ds_layout_cis[1].pBindings = binding.get();
    vkCreateDescriptorSetLayout(device, &ds_layout_cis[1], nullptr, &ds_layouts[1]);
    auto ds_layout_binding_flags_ci = std::make_unique<VkDescriptorSetLayoutBindingFlagsCreateInfo>(vku::InitStructHelper());
    std::array<const VkDescriptorBindingFlags, 1> ds_layout_binding_flags_arr{{VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT}};
    ds_layout_binding_flags_ci->bindingCount = 1;
    ds_layout_binding_flags_ci->pBindingFlags = ds_layout_binding_flags_arr.data();
    binding->descriptorCount = 1;
    binding->descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    binding->pImmutableSamplers = &samplers[2];
    ds_layout_cis[2].bindingCount = 1;
    ds_layout_cis[2].pBindings = binding.get();
    ds_layout_cis[2].pNext = ds_layout_binding_flags_ci.get();
    vkCreateDescriptorSetLayout(device, &ds_layout_cis[2], nullptr, &ds_layouts[2]);

    // Create pipeline layout
    auto pipeline_layout_ci = std::make_unique<VkPipelineLayoutCreateInfo>(vku::InitStructHelper());
    std::array<VkPushConstantRange, 1> push_constants{{}};
    push_constants[0].offset = 4;
    push_constants[0].size = 8;
    push_constants[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    pipeline_layout_ci->setLayoutCount = (uint32_t)ds_layouts.size();
    pipeline_layout_ci->pSetLayouts = ds_layouts.data();
    pipeline_layout_ci->pushConstantRangeCount = (uint32_t)push_constants.size();
    pipeline_layout_ci->pPushConstantRanges = push_constants.data();
    VkPipelineLayout pipeline_layout;
    vkCreatePipelineLayout(device, pipeline_layout_ci.get(), nullptr, &pipeline_layout);

    // Create renderpass
    auto renderpass_ci = std::make_unique<VkRenderPassCreateInfo>(vku::InitStructHelper());

    auto rpiaa_ci = std::make_unique<VkRenderPassInputAttachmentAspectCreateInfo>(vku::InitStructHelper());
    VkInputAttachmentAspectReference aspectReferences[1] = {};
    aspectReferences[0].subpass = 1;
    aspectReferences[0].inputAttachmentIndex = 2;
    aspectReferences[0].aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    rpiaa_ci->aspectReferenceCount = 1;
    rpiaa_ci->pAspectReferences = aspectReferences;

    auto rpmv_ci = std::make_unique<VkRenderPassMultiviewCreateInfo>(vku::InitStructHelper());
    rpmv_ci->subpassCount = 1;

    uint32_t viewMasks[1] = {1};
    rpmv_ci->pViewMasks = viewMasks;

    int32_t viewOffsets[2] = {0, 1};
    rpmv_ci->dependencyCount = 2;
    rpmv_ci->pViewOffsets = viewOffsets;

    uint32_t correlationMasks[1] = {8};
    rpmv_ci->correlationMaskCount = 1;
    rpmv_ci->pCorrelationMasks = correlationMasks;

    rpiaa_ci->pNext = &rpmv_ci;
    renderpass_ci->pNext = &rpiaa_ci;
    renderpass_ci->flags = 0;

    VkAttachmentDescription attachments[1] = {};
    renderpass_ci->attachmentCount = 1;
    attachments[0].flags = VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT;
    attachments[0].format = VK_FORMAT_R8G8_USCALED;
    attachments[0].samples = VK_SAMPLE_COUNT_8_BIT;
    attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[0].initialLayout = VK_IMAGE_LAYOUT_GENERAL;
    attachments[0].finalLayout = VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR;
    renderpass_ci->pAttachments = attachments;

    VkSubpassDescription subpasses[2] = {};
    renderpass_ci->subpassCount = 2;
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
    renderpass_ci->pSubpasses = subpasses;

    VkSubpassDependency dependencies[1] = {};
    renderpass_ci->dependencyCount = 1;
    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 2345;
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_NONE_KHR;
    dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[0].srcAccessMask = VK_ACCESS_NONE_KHR;
    dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[0].dependencyFlags = VK_DEPENDENCY_DEVICE_GROUP_BIT;
    renderpass_ci->pDependencies = dependencies;

    VkRenderPass render_pass;
    vkCreateRenderPass(device, renderpass_ci.get(), nullptr, &render_pass);

    // Create graphics pipeline
    std::array<VkPipelineShaderStageCreateInfo, 2> pipeline_stage_cis{{vku::InitStructHelper(), vku::InitStructHelper()}};
    pipeline_stage_cis[0].module = shader_module;
    pipeline_stage_cis[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    pipeline_stage_cis[0].pName = "main";
    pipeline_stage_cis[1].module = shader_module;
    pipeline_stage_cis[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    pipeline_stage_cis[1].pName = "main";
    auto pipeline_ci = std::make_unique<VkGraphicsPipelineCreateInfo>(vku::InitStructHelper());
    pipeline_ci->layout = pipeline_layout;
    pipeline_ci->stageCount = (uint32_t)pipeline_stage_cis.size();
    pipeline_ci->pStages = pipeline_stage_cis.data();
    pipeline_ci->renderPass = render_pass;
    VkPipeline pipeline;
    vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, pipeline_ci.get(), NULL, &pipeline);

    vkDestroySamplerYcbcrConversion(device, ycbcr, nullptr);
    for (auto& sampler : samplers) {
        vkDestroySampler(device, sampler, nullptr);
    }
    for (auto& ds_layout : ds_layouts) {
        vkDestroyDescriptorSetLayout(device, ds_layout, nullptr);
    }
    vkDestroyPipelineLayout(device, pipeline_layout, nullptr);
    vkDestroyPipeline(device, pipeline, nullptr);
    vkDestroyShaderModule(device, shader_module, nullptr);
    vkDestroyDevice(device, nullptr);
    vkDestroyInstance(instance, nullptr);

    size_t pipeline_id = 9;

    auto result_json = get_json(device_counter, pipeline_id);
    auto result_vert_spirv = get_spirv(device_counter, pipeline_id, "vert");
    auto result_frag_spirv = get_spirv(device_counter, pipeline_id, "frag");
    EXPECT_TRUE(ValidatePipelineJson(std::string(result_json)));

    std::string ref_json = {R"({
	"EnabledExtensions" : 
	[
		"VK_KHR_shader_clock"
	],
	"GraphicsPipelineState" : 
	{
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
			},
			{
				"DescriptorSetLayout2" : 
				{
					"bindingCount" : 1,
					"flags" : 0,
					"pBindings" : 
					[
						{
							"binding" : 0,
							"descriptorCount" : 1,
							"descriptorType" : "VK_DESCRIPTOR_TYPE_SAMPLER",
							"pImmutableSamplers" : 
							[
								"ImmutableSampler2"
							],
							"stageFlags" : 0
						}
					],
					"pNext" : "NULL",
					"sType" : "VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO"
				}
			},
			{
				"DescriptorSetLayout3" : 
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
								"ImmutableSampler3"
							],
							"stageFlags" : 0
						}
					],
					"pNext" : 
					{
						"bindingCount" : 1,
						"pBindingFlags" : 
						[
							"VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT"
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
					"pNext" : "NULL",
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
			},
			{
				"ImmutableSampler2" : 
				{
					"addressModeU" : "VK_SAMPLER_ADDRESS_MODE_REPEAT",
					"addressModeV" : "VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT",
					"addressModeW" : "VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE",
					"anisotropyEnable" : "VK_FALSE",
					"borderColor" : "VK_BORDER_COLOR_FLOAT_CUSTOM_EXT",
					"compareEnable" : "VK_FALSE",
					"compareOp" : "VK_COMPARE_OP_NOT_EQUAL",
					"flags" : 0,
					"magFilter" : "VK_FILTER_CUBIC_EXT",
					"maxAnisotropy" : 2.0,
					"maxLod" : 1000.0,
					"minFilter" : "VK_FILTER_NEAREST",
					"minLod" : 0.0,
					"mipLodBias" : 0.5,
					"mipmapMode" : "VK_SAMPLER_MIPMAP_MODE_LINEAR",
					"pNext" : "NULL",
					"sType" : "VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO",
					"unnormalizedCoordinates" : "VK_TRUE"
				}
			},
			{
				"ImmutableSampler3" : 
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
						"pNext" : "NULL",
						"reductionMode" : "VK_SAMPLER_REDUCTION_MODE_MAX",
						"sType" : "VK_STRUCTURE_TYPE_SAMPLER_REDUCTION_MODE_CREATE_INFO"
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
			"pPushConstantRanges" : 
			[
				{
					"offset" : 4,
					"size" : 8,
					"stageFlags" : "VK_SHADER_STAGE_COMPUTE_BIT"
				}
			],
			"pSetLayouts" : 
			[
				"DescriptorSetLayout1",
				"DescriptorSetLayout2",
				"DescriptorSetLayout3"
			],
			"pushConstantRangeCount" : 1,
			"sType" : "VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO",
			"setLayoutCount" : 3
		},
		"Renderpass" : 
		{
			"attachmentCount" : 1,
			"dependencyCount" : 1,
			"flags" : 0,
			"pAttachments" : 
			[
				{
					"finalLayout" : "VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR",
					"flags" : "VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT",
					"format" : "VK_FORMAT_R8G8_USCALED",
					"initialLayout" : "VK_IMAGE_LAYOUT_GENERAL",
					"loadOp" : "VK_ATTACHMENT_LOAD_OP_DONT_CARE",
					"samples" : "VK_SAMPLE_COUNT_8_BIT",
					"stencilLoadOp" : "VK_ATTACHMENT_LOAD_OP_LOAD",
					"stencilStoreOp" : "VK_ATTACHMENT_STORE_OP_DONT_CARE",
					"storeOp" : "VK_ATTACHMENT_STORE_OP_STORE"
				}
			],
			"pDependencies" : 
			[
				{
					"dependencyFlags" : "VK_DEPENDENCY_DEVICE_GROUP_BIT",
					"dstAccessMask" : "VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT",
					"dstStageMask" : "VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT",
					"dstSubpass" : 2345,
					"srcAccessMask" : 0,
					"srcStageMask" : 0,
					"srcSubpass" : 4294967295
				}
			],
			"pNext" : 
			{
				"correlationMaskCount" : 1,
				"dependencyCount" : 2,
				"pCorrelationMasks" : 
				[
					8
				],
				"pNext" : "NULL",
				"pViewMasks" : 
				[
					1
				],
				"pViewOffsets" : 
				[
					0,
					1
				],
				"sType" : "VK_STRUCTURE_TYPE_RENDER_PASS_MULTIVIEW_CREATE_INFO",
				"subpassCount" : 1
			},
			"pSubpasses" : 
			[
				{
					"colorAttachmentCount" : 1,
					"flags" : 0,
					"inputAttachmentCount" : 1,
					"pColorAttachments" : 
					[
						{
							"attachment" : 4294967295,
							"layout" : "VK_IMAGE_LAYOUT_UNDEFINED"
						}
					],
					"pDepthStencilAttachment" : 
					{
						"attachment" : 4294967295,
						"layout" : "VK_IMAGE_LAYOUT_UNDEFINED"
					},
					"pInputAttachments" : 
					[
						{
							"attachment" : 567,
							"layout" : "VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL"
						}
					],
					"pPreserveAttachments" : "NULL",
					"pResolveAttachments" : 
					[
						{
							"attachment" : 4294967295,
							"layout" : "VK_IMAGE_LAYOUT_UNDEFINED"
						}
					],
					"pipelineBindPoint" : "VK_PIPELINE_BIND_POINT_GRAPHICS",
					"preserveAttachmentCount" : 0
				},
				{
					"colorAttachmentCount" : 1,
					"flags" : 0,
					"inputAttachmentCount" : 0,
					"pColorAttachments" : 
					[
						{
							"attachment" : 567,
							"layout" : "VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL"
						}
					],
					"pDepthStencilAttachment" : 
					{
						"attachment" : 4294967295,
						"layout" : "VK_IMAGE_LAYOUT_UNDEFINED"
					},
					"pInputAttachments" : "NULL",
					"pPreserveAttachments" : "NULL",
					"pResolveAttachments" : 
					[
						{
							"attachment" : 4294967295,
							"layout" : "VK_IMAGE_LAYOUT_UNDEFINED"
						}
					],
					"pipelineBindPoint" : "VK_PIPELINE_BIND_POINT_GRAPHICS",
					"preserveAttachmentCount" : 0
				}
			],
			"sType" : "VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO",
			"subpassCount" : 2
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
		],
		"YcbcrSamplers" : 
		[
			{
				"YcbcrSampler1" : 
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
		184,
		103,
		149,
		174,
		185,
		166,
		77,
		33,
		213,
		190,
		252,
		18,
		122,
		120,
		123,
		145
	]
})"};
    write_ids(ref_json, device_counter, pipeline_id);

    auto vert_spirv_words_match =
        std::equal(result_vert_spirv.begin(), result_vert_spirv.end(), ref_spirv.begin(), ref_spirv.end());
    auto frag_spirv_words_match =
        std::equal(result_frag_spirv.begin(), result_frag_spirv.end(), ref_spirv.begin(), ref_spirv.end());
    EXPECT_TRUE(vert_spirv_words_match);
    EXPECT_TRUE(frag_spirv_words_match);
    EXPECT_EQ(result_json, ref_json);
}
