/*
 * Copyright (c) 2025 The Khronos Group Inc.
 * Copyright (c) 2025 RasterGrid Kft.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <vulkan/vulkan.h>
#include <vulkan/utility/vk_struct_helper.hpp>

#include <gtest/gtest.h>

#include <filesystem>
#include <regex>
#include <string>
#include <vector>
#include <array>

// Shorthand to throw GTest exception, causing the test to fail with user-provided message stream fragment
#define FAIL_TEST_IF(pred, msg_stream)                                                                         \
    do {                                                                                                       \
        if (pred) {                                                                                            \
            GTEST_MESSAGE_AT_(__FILE__, __LINE__, "", ::testing::TestPartResult::kFatalFailure) << msg_stream; \
            throw testing::AssertionException(                                                                 \
                testing::TestPartResult(testing::TestPartResult::kFatalFailure, __FILE__, __LINE__, ""));      \
        }                                                                                                      \
    } while (0)

class UUID : public testing::Test {
  public:
    UUID() {
        CleanDataFiles();
        CreateDataFolder();
    }
    UUID(const UUID&) = delete;
    UUID(UUID&&) = delete;
    ~UUID() { CleanDataFiles(); }

    void TEST_DESCRIPTION(const char* desc) { RecordProperty("description", desc); }

    std::array<uint8_t, VK_UUID_SIZE> GetUUID(VkDevice device, VkPipeline pipeline) {
        PFN_vkGetPipelinePropertiesEXT vkGetPipelinePropertiesEXT =
            (PFN_vkGetPipelinePropertiesEXT)vkGetDeviceProcAddr(device, "vkGetPipelinePropertiesEXT");

        VkPipelineInfoEXT pipeline_info_ext;
        pipeline_info_ext.sType = VK_STRUCTURE_TYPE_PIPELINE_INFO_EXT;
        pipeline_info_ext.pNext = nullptr;
        pipeline_info_ext.pipeline = pipeline;
        VkPipelinePropertiesIdentifierEXT pipeline_props{};
        pipeline_props.sType = VK_STRUCTURE_TYPE_PIPELINE_PROPERTIES_IDENTIFIER_EXT;
        pipeline_props.pNext = nullptr;
        vkGetPipelinePropertiesEXT(device, &pipeline_info_ext, reinterpret_cast<VkBaseOutStructure*>(&pipeline_props));

        std::array<uint8_t, VK_UUID_SIZE> result;
        std::copy(pipeline_props.pipelineIdentifier, pipeline_props.pipelineIdentifier + VK_UUID_SIZE, result.begin());
        return result;
    }

  private:
    void CleanDataFiles() {
        FAIL_TEST_IF(!getenv("VK_JSON_FILE_PATH"), "Environment variable VK_JSON_FILE_PATH not set.");
        std::filesystem::path VK_JSON_FILE_PATH{getenv("VK_JSON_FILE_PATH")};
        if (std::filesystem::exists(VK_JSON_FILE_PATH)) {
            FAIL_TEST_IF(!std::filesystem::is_directory(VK_JSON_FILE_PATH),
                         "Path set by VK_JSON_FILE_PATH exists but is not a directory.");
            try {
                std::filesystem::remove_all(VK_JSON_FILE_PATH);
            } catch (std::exception& e) {
                FAIL_TEST_IF(true, "Failed to remove directory pointed to by VK_JSON_FILE_PATH: " << VK_JSON_FILE_PATH << "("
                                                                                                  << e.what() << ")");
            }
        }
    }
    void CreateDataFolder() {
        FAIL_TEST_IF(!getenv("VK_JSON_FILE_PATH"), "Environment variable VK_JSON_FILE_PATH not set.");
        std::filesystem::path VK_JSON_FILE_PATH{getenv("VK_JSON_FILE_PATH")};
        try {
            std::filesystem::create_directories(VK_JSON_FILE_PATH);
        } catch (std::exception& e) {
            FAIL_TEST_IF(true, "Failed to create directory pointed to by VK_JSON_FILE_PATH: " << VK_JSON_FILE_PATH << "("
                                                                                              << e.what() << ")");
        }
    }
};

TEST_F(UUID, ComputeSimple) {
    TEST_DESCRIPTION("Tests whether generated UUID for a compute pipeline is as expected");

    auto instance_ci = std::make_unique<VkInstanceCreateInfo>(vku::InitStructHelper());
    VkInstance instance;
    vkCreateInstance(instance_ci.get(), nullptr, &instance);

    uint32_t phys_dev_count = 1;
    std::vector<VkPhysicalDevice> phys_devs(phys_dev_count);
    vkEnumeratePhysicalDevices(instance, &phys_dev_count, phys_devs.data());

    const char* pipeline_props_name = "VK_EXT_pipeline_properties";
    auto device_ci = std::make_unique<VkDeviceCreateInfo>(vku::InitStructHelper());
    auto pipeline_props_feature = std::make_unique<VkPhysicalDevicePipelinePropertiesFeaturesEXT>(vku::InitStructHelper());
    device_ci->enabledExtensionCount = 1;
    device_ci->ppEnabledExtensionNames = &pipeline_props_name;
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

    auto pipeline_stage_ci = std::make_unique<VkPipelineShaderStageCreateInfo>(vku::InitStructHelper());
    pipeline_stage_ci->module = shader_module;
    pipeline_stage_ci->stage = VK_SHADER_STAGE_COMPUTE_BIT;
    pipeline_stage_ci->pName = "main";
    auto compute_pipeline_ci = std::make_unique<VkComputePipelineCreateInfo>(vku::InitStructHelper());
    compute_pipeline_ci->layout = pipeline_layout;
    compute_pipeline_ci->stage = *pipeline_stage_ci;
    VkPipeline pipeline;
    vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, compute_pipeline_ci.get(), NULL, &pipeline);

    auto uuid = GetUUID(device, pipeline);

    vkDestroyPipeline(device, pipeline, nullptr);
    vkDestroyPipelineLayout(device, pipeline_layout, nullptr);
    vkDestroyDescriptorSetLayout(device, ds_layout, nullptr);
    vkDestroyShaderModule(device, shader_module, nullptr);
    vkDestroyDevice(device, nullptr);
    vkDestroyInstance(instance, nullptr);

    std::array<uint8_t, VK_UUID_SIZE> ref{{60, 107, 242, 102, 76, 42, 138, 202, 223, 212, 110, 146, 235, 193, 36, 196}};
    EXPECT_EQ(uuid, ref);
}

TEST_F(UUID, ComputeReproducible) {
    TEST_DESCRIPTION("Tests whether generated UUIDs for identical compute pipelines match");

    auto simple_compute = [this](VkDevice device) {
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

        auto pipeline_stage_ci = std::make_unique<VkPipelineShaderStageCreateInfo>(vku::InitStructHelper());
        pipeline_stage_ci->module = shader_module;
        pipeline_stage_ci->stage = VK_SHADER_STAGE_COMPUTE_BIT;
        pipeline_stage_ci->pName = "main";
        auto compute_pipeline_ci = std::make_unique<VkComputePipelineCreateInfo>(vku::InitStructHelper());
        compute_pipeline_ci->layout = pipeline_layout;
        compute_pipeline_ci->stage = *pipeline_stage_ci;
        VkPipeline pipeline;
        vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, compute_pipeline_ci.get(), NULL, &pipeline);

        auto uuid = GetUUID(device, pipeline);

        vkDestroyPipeline(device, pipeline, nullptr);
        vkDestroyPipelineLayout(device, pipeline_layout, nullptr);
        vkDestroyDescriptorSetLayout(device, ds_layout, nullptr);
        vkDestroyShaderModule(device, shader_module, nullptr);

        return uuid;
    };

    auto instance_ci = std::make_unique<VkInstanceCreateInfo>(vku::InitStructHelper());
    VkInstance instance;
    vkCreateInstance(instance_ci.get(), nullptr, &instance);

    uint32_t phys_dev_count = 1;
    std::vector<VkPhysicalDevice> phys_devs(phys_dev_count);
    vkEnumeratePhysicalDevices(instance, &phys_dev_count, phys_devs.data());

    const char* pipeline_props_name = "VK_EXT_pipeline_properties";
    auto device_ci = std::make_unique<VkDeviceCreateInfo>(vku::InitStructHelper());
    auto pipeline_props_feature = std::make_unique<VkPhysicalDevicePipelinePropertiesFeaturesEXT>(vku::InitStructHelper());
    device_ci->enabledExtensionCount = 1;
    device_ci->ppEnabledExtensionNames = &pipeline_props_name;
    VkDevice device1, device2;
    vkCreateDevice(phys_devs[0], device_ci.get(), nullptr, &device1);
    vkCreateDevice(phys_devs[0], device_ci.get(), nullptr, &device2);

    auto uuid1 = simple_compute(device1);
    auto uuid2 = simple_compute(device2);
    auto uuid3 = simple_compute(device2);

    vkDestroyDevice(device1, nullptr);
    vkDestroyDevice(device2, nullptr);
    vkDestroyInstance(instance, nullptr);

    EXPECT_EQ(uuid1, uuid2);
    EXPECT_EQ(uuid1, uuid3);
}

TEST_F(UUID, ComputeDifferent) {
    TEST_DESCRIPTION("Tests whether generated UUIDs for different compute pipelines differ");

    auto simple_compute = [this](VkDevice device, const char* name) {
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

        auto pipeline_stage_ci = std::make_unique<VkPipelineShaderStageCreateInfo>(vku::InitStructHelper());
        pipeline_stage_ci->module = shader_module;
        pipeline_stage_ci->stage = VK_SHADER_STAGE_COMPUTE_BIT;
        pipeline_stage_ci->pName = name;
        auto compute_pipeline_ci = std::make_unique<VkComputePipelineCreateInfo>(vku::InitStructHelper());
        compute_pipeline_ci->layout = pipeline_layout;
        compute_pipeline_ci->stage = *pipeline_stage_ci;
        VkPipeline pipeline;
        vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, compute_pipeline_ci.get(), NULL, &pipeline);

        auto uuid = GetUUID(device, pipeline);

        vkDestroyPipeline(device, pipeline, nullptr);
        vkDestroyPipelineLayout(device, pipeline_layout, nullptr);
        vkDestroyDescriptorSetLayout(device, ds_layout, nullptr);
        vkDestroyShaderModule(device, shader_module, nullptr);

        return uuid;
    };

    auto instance_ci = std::make_unique<VkInstanceCreateInfo>(vku::InitStructHelper());
    VkInstance instance;
    vkCreateInstance(instance_ci.get(), nullptr, &instance);

    uint32_t phys_dev_count = 1;
    std::vector<VkPhysicalDevice> phys_devs(phys_dev_count);
    vkEnumeratePhysicalDevices(instance, &phys_dev_count, phys_devs.data());

    const char* pipeline_props_name = "VK_EXT_pipeline_properties";
    auto device_ci = std::make_unique<VkDeviceCreateInfo>(vku::InitStructHelper());
    auto pipeline_props_feature = std::make_unique<VkPhysicalDevicePipelinePropertiesFeaturesEXT>(vku::InitStructHelper());
    device_ci->enabledExtensionCount = 1;
    device_ci->ppEnabledExtensionNames = &pipeline_props_name;
    VkDevice device1, device2;
    vkCreateDevice(phys_devs[0], device_ci.get(), nullptr, &device1);
    vkCreateDevice(phys_devs[0], device_ci.get(), nullptr, &device2);

    auto uuid1 = simple_compute(device1, "main1");
    auto uuid2 = simple_compute(device2, "main2");

    vkDestroyDevice(device1, nullptr);
    vkDestroyDevice(device2, nullptr);
    vkDestroyInstance(instance, nullptr);

    EXPECT_NE(uuid1, uuid2);
}

TEST_F(UUID, GraphicsSimple) {
    TEST_DESCRIPTION("Tests whether generated UUID for a graphics pipeline is as expected");

    auto instance_ci = std::make_unique<VkInstanceCreateInfo>(vku::InitStructHelper());
    VkInstance instance;
    vkCreateInstance(instance_ci.get(), nullptr, &instance);

    uint32_t phys_dev_count = 1;
    std::vector<VkPhysicalDevice> phys_devs(phys_dev_count);
    vkEnumeratePhysicalDevices(instance, &phys_dev_count, phys_devs.data());

    const char* pipeline_props_name = "VK_EXT_pipeline_properties";
    auto device_ci = std::make_unique<VkDeviceCreateInfo>(vku::InitStructHelper());
    auto pipeline_props_feature = std::make_unique<VkPhysicalDevicePipelinePropertiesFeaturesEXT>(vku::InitStructHelper());
    device_ci->enabledExtensionCount = 1;
    device_ci->ppEnabledExtensionNames = &pipeline_props_name;
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

    auto uuid = GetUUID(device, pipeline);

    vkDestroyPipeline(device, pipeline, nullptr);
    vkDestroyRenderPass(device, render_pass, nullptr);
    vkDestroyPipelineLayout(device, pipeline_layout, nullptr);
    vkDestroyDescriptorSetLayout(device, ds_layout, nullptr);
    vkDestroyShaderModule(device, shader_module, nullptr);
    vkDestroyDevice(device, nullptr);
    vkDestroyInstance(instance, nullptr);

    std::array<uint8_t, VK_UUID_SIZE> ref{{172, 140, 31, 159, 143, 235, 178, 73, 188, 141, 213, 160, 169, 41, 214, 207}};
    EXPECT_EQ(uuid, ref);
}

TEST_F(UUID, GraphicsReproducible) {
    TEST_DESCRIPTION("Tests whether generated UUIDs for identical graphics pipelines match");

    auto simple_graphics = [this](VkDevice device) {
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

        auto uuid = GetUUID(device, pipeline);

        vkDestroyPipeline(device, pipeline, nullptr);
        vkDestroyRenderPass(device, render_pass, nullptr);
        vkDestroyPipelineLayout(device, pipeline_layout, nullptr);
        vkDestroyDescriptorSetLayout(device, ds_layout, nullptr);
        vkDestroyShaderModule(device, shader_module, nullptr);

        return uuid;
    };

    auto instance_ci = std::make_unique<VkInstanceCreateInfo>(vku::InitStructHelper());
    VkInstance instance;
    vkCreateInstance(instance_ci.get(), nullptr, &instance);

    uint32_t phys_dev_count = 1;
    std::vector<VkPhysicalDevice> phys_devs(phys_dev_count);
    vkEnumeratePhysicalDevices(instance, &phys_dev_count, phys_devs.data());

    const char* pipeline_props_name = "VK_EXT_pipeline_properties";
    auto device_ci = std::make_unique<VkDeviceCreateInfo>(vku::InitStructHelper());
    auto pipeline_props_feature = std::make_unique<VkPhysicalDevicePipelinePropertiesFeaturesEXT>(vku::InitStructHelper());
    device_ci->enabledExtensionCount = 1;
    device_ci->ppEnabledExtensionNames = &pipeline_props_name;
    VkDevice device1, device2;
    vkCreateDevice(phys_devs[0], device_ci.get(), nullptr, &device1);
    vkCreateDevice(phys_devs[0], device_ci.get(), nullptr, &device2);

    auto uuid1 = simple_graphics(device1);
    auto uuid2 = simple_graphics(device2);
    auto uuid3 = simple_graphics(device2);

    vkDestroyDevice(device1, nullptr);
    vkDestroyDevice(device2, nullptr);
    vkDestroyInstance(instance, nullptr);

    EXPECT_EQ(uuid1, uuid2);
    EXPECT_EQ(uuid1, uuid3);
}

TEST_F(UUID, GraphicsDifferent) {
    TEST_DESCRIPTION("Tests whether generated UUIDs for different graphics pipelines differ");

    auto simple_graphics = [this](VkDevice device, const char* name) {
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
        pipeline_stage_ci->pName = name;
        graphics_pipeline_ci->layout = pipeline_layout;
        graphics_pipeline_ci->stageCount = 1;
        graphics_pipeline_ci->pStages = pipeline_stage_ci.get();
        graphics_pipeline_ci->renderPass = render_pass;
        VkPipeline pipeline;
        vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, graphics_pipeline_ci.get(), NULL, &pipeline);

        auto uuid = GetUUID(device, pipeline);

        vkDestroyPipeline(device, pipeline, nullptr);
        vkDestroyRenderPass(device, render_pass, nullptr);
        vkDestroyPipelineLayout(device, pipeline_layout, nullptr);
        vkDestroyDescriptorSetLayout(device, ds_layout, nullptr);
        vkDestroyShaderModule(device, shader_module, nullptr);

        return uuid;
    };

    auto instance_ci = std::make_unique<VkInstanceCreateInfo>(vku::InitStructHelper());
    VkInstance instance;
    vkCreateInstance(instance_ci.get(), nullptr, &instance);

    uint32_t phys_dev_count = 1;
    std::vector<VkPhysicalDevice> phys_devs(phys_dev_count);
    vkEnumeratePhysicalDevices(instance, &phys_dev_count, phys_devs.data());

    const char* pipeline_props_name = "VK_EXT_pipeline_properties";
    auto device_ci = std::make_unique<VkDeviceCreateInfo>(vku::InitStructHelper());
    auto pipeline_props_feature = std::make_unique<VkPhysicalDevicePipelinePropertiesFeaturesEXT>(vku::InitStructHelper());
    device_ci->enabledExtensionCount = 1;
    device_ci->ppEnabledExtensionNames = &pipeline_props_name;
    VkDevice device1, device2;
    vkCreateDevice(phys_devs[0], device_ci.get(), nullptr, &device1);
    vkCreateDevice(phys_devs[0], device_ci.get(), nullptr, &device2);

    auto uuid1 = simple_graphics(device1, "main1");
    auto uuid2 = simple_graphics(device2, "main2");

    vkDestroyDevice(device1, nullptr);
    vkDestroyDevice(device2, nullptr);
    vkDestroyInstance(instance, nullptr);

    EXPECT_NE(uuid1, uuid2);
}
