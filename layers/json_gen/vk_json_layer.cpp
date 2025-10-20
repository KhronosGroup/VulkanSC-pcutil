/*
 * Copyright (c) 2020-2025 The Khronos Group Inc.
 * Copyright (c) 2020-2025 LunarG, Inc.
 * Copyright (c) 2020-2025 Advanced Micro Devices, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "vk_json_layer.hpp"
#include <vulkan/pcjson/vksc_pipeline_json.h>

#include "atomic_max.hpp"
#include "log.h"
#include "vk_common.h"

#include <vulkan/vk_layer.h>

#include <vulkan/utility/vk_concurrent_unordered_map.hpp>
#include <vulkan/utility/vk_struct_helper.hpp>
#include <vulkan/utility/vk_safe_struct.hpp>

#include <new>            // std::bad_alloc
#include <memory>         // std::shared_ptr
#include <vector>         // std::vector
#include <string>         // std::string, std::size
#include <cstring>        // std::strncmp, std::memcpy
#include <unordered_map>  // std::unordered_map
#include <iostream>       // std::cout
#include <numeric>        // std::accumulate
#include <fstream>        // std::ofstream
#include <sstream>        // std::stringstream

namespace vk_json {

const char* stage_bit_to_string(const VkShaderStageFlagBits stage, VkGraphicsPipelineCreateInfo) {
    switch (stage) {
        case VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT:
            return "vert";
            break;
        case VkShaderStageFlagBits::VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
            return "tcs";
            break;
        case VkShaderStageFlagBits::VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
            return "tes";
            break;
        case VkShaderStageFlagBits::VK_SHADER_STAGE_GEOMETRY_BIT:
            return "geom";
            break;
        case VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT:
            return "frag";
            break;
        default:
            LOG("[%s] ERROR: Unrecognized shader stage: %d", VK_EXT_PIPELINE_PROPERTIES_EXTENSION_NAME, stage);
            exit(-1);
    }
};

const char* stage_bit_to_string(const VkShaderStageFlagBits stage, VkComputePipelineCreateInfo) {
    switch (stage) {
        case VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT:
            return "compute";
            break;
        default:
            LOG("[%s] ERROR: Unrecognized shader stage: %d", VK_EXT_PIPELINE_PROPERTIES_EXTENSION_NAME, stage);
            exit(-1);
    }
};

struct OwningVpjShaderFilenames {
    std::vector<std::string> storage;
    std::vector<VpjShaderFileName> filenames;
};

OwningVpjShaderFilenames get_shader_filenames(const VkGraphicsPipelineCreateInfo& ci, const std::string& prefix,
                                              const uint32_t device_index, const uint32_t pipeline_index) {
    const auto shader_filename = [&](const VkPipelineShaderStageCreateInfo& pss_ci) -> std::string {
        return prefix + "_device_" + std::to_string(device_index) + "_pipeline_" + std::to_string(pipeline_index) + '.' +
               stage_bit_to_string(pss_ci.stage, ci) + ".spv";
    };
    const auto filename_accumulator = [&](OwningVpjShaderFilenames& acc,
                                          const VkPipelineShaderStageCreateInfo& pss_ci) -> OwningVpjShaderFilenames& {
        acc.storage.emplace_back(shader_filename(pss_ci));
        acc.filenames.push_back(VpjShaderFileName{static_cast<int32_t>(pss_ci.stage), acc.storage.back().c_str()});
        return acc;
    };
    return std::accumulate(ci.pStages, ci.pStages + ci.stageCount, OwningVpjShaderFilenames{}, filename_accumulator);
};

OwningVpjShaderFilenames get_shader_filenames(const VkComputePipelineCreateInfo& ci, const std::string& prefix,
                                              const uint32_t device_index, const uint32_t pipeline_index) {
    const auto shader_filename = [&](const VkPipelineShaderStageCreateInfo& pss_ci) -> std::string {
        return prefix + "_device_" + std::to_string(device_index) + "_pipeline_" + std::to_string(pipeline_index) + '.' +
               stage_bit_to_string(pss_ci.stage, ci) + ".spv";
    };
    OwningVpjShaderFilenames res;
    res.storage.emplace_back(shader_filename(ci.stage));
    res.filenames.push_back(VpjShaderFileName{static_cast<int32_t>(ci.stage.stage), res.storage.back().c_str()});
    return res;
};

static constexpr VkLayerProperties kGlobalLayer = {"VK_LAYER_KHRONOS_json_gen",
                                                   VK_MAKE_VERSION(1, 2, VK_HEADER_VERSION),  // specVersion
                                                   VK_MAKE_VERSION(0, 2, 0),                  // implementationVersion
                                                   "layer: json_gen"};

// Instance extensions that this layer provides:
const VkExtensionProperties kInstanceExtensionProperties[] = {
    VkExtensionProperties{VK_EXT_PIPELINE_PROPERTIES_EXTENSION_NAME, VK_EXT_PIPELINE_PROPERTIES_SPEC_VERSION}};
const uint32_t kInstanceExtensionPropertiesCount = static_cast<uint32_t>(std::size(kInstanceExtensionProperties));

static const VkExtensionProperties kDeviceExtension = {VK_EXT_PIPELINE_PROPERTIES_EXTENSION_NAME,
                                                       VK_EXT_PIPELINE_PROPERTIES_SPEC_VERSION};

static vku::concurrent::unordered_map<uintptr_t, std::shared_ptr<InstanceData>> instance_data_map;
static vku::concurrent::unordered_map<uintptr_t, std::shared_ptr<DeviceData>> device_data_map;

uintptr_t DispatchKey(const void* object) {
    auto tmp = reinterpret_cast<const struct VkLayerDispatchTable_* const*>(object);
    return reinterpret_cast<uintptr_t>(*tmp);
}

// helper to pass vector sizes into vulkan without noisy casting everywhere
template <typename V>
static inline uint32_t VecSize(const V& v) {
    return static_cast<uint32_t>(v.size());
}

static std::shared_ptr<InstanceData> GetInstanceData(const void* object) {
    auto result = instance_data_map.find(DispatchKey(object));
    return result != instance_data_map.end() ? result->second : nullptr;
}

static std::shared_ptr<DeviceData> GetDeviceData(const void* object) {
    if (auto result = device_data_map.find(DispatchKey(object)); result->first) {
        return result->second;
    } else {
        LOG("[%s] ERROR: Failed to device data in accelerating structure.", VK_EXT_PIPELINE_PROPERTIES_EXTENSION_NAME);
        exit(-1);
    }
}

VKAPI_ATTR VkResult VKAPI_CALL EnumerateDeviceExtensionProperties(VkPhysicalDevice physicalDevice, const char* pLayerName,
                                                                  uint32_t* pPropertyCount, VkExtensionProperties* pProperties) {
    if (pLayerName && std::strncmp(pLayerName, kGlobalLayer.layerName, VK_MAX_EXTENSION_NAME_SIZE)) {
        auto instance_data = GetInstanceData(physicalDevice);
        return instance_data->vtable.EnumerateDeviceExtensionProperties(physicalDevice, pLayerName, pPropertyCount, pProperties);
    }

    if (!pLayerName) {
        uint32_t count = 0;
        auto instance_data = GetInstanceData(physicalDevice);
        instance_data->vtable.EnumerateDeviceExtensionProperties(physicalDevice, pLayerName, &count, nullptr);

        // We need to check if the extension is natively supported in order to return the correct count into `pPropertyCount`.
        std::vector<VkExtensionProperties> properties(count);
        instance_data->vtable.EnumerateDeviceExtensionProperties(physicalDevice, pLayerName, &count, properties.data());

        bool has_native_pipeline_props = false;
        for (uint32_t i = 0; i < count; ++i) {
            if (std::strncmp(properties[i].extensionName, VK_EXT_PIPELINE_PROPERTIES_EXTENSION_NAME, VK_MAX_EXTENSION_NAME_SIZE) ==
                0) {
                has_native_pipeline_props = true;
                break;
            }
        }

        uint32_t total_count = !has_native_pipeline_props ? count + 1 : count;

        if (!pProperties) {
            *pPropertyCount = total_count;
            return VK_SUCCESS;
        }

        if (*pPropertyCount < total_count) {
            // Spec for `vkEnumerateDeviceLayerProperties` says at most `pPropertyCount` structures will be written in this case,
            // which is the convention we follow for `vkEnumerate*` functions.
            std::memcpy(pProperties, properties.data(), *pPropertyCount * sizeof(VkExtensionProperties));
            return VK_INCOMPLETE;
        }

        std::memcpy(pProperties, properties.data(), count * sizeof(VkExtensionProperties));

        if (!has_native_pipeline_props) {
            pProperties[count] = kDeviceExtension;
        }

        *pPropertyCount = total_count;

        return VK_SUCCESS;
    }

    if (!pProperties) {
        *pPropertyCount = 1;
        return VK_SUCCESS;
    } else {
        if (*pPropertyCount >= 1) {
            *pPropertyCount = 1;
            pProperties[0] = kDeviceExtension;
            return VK_SUCCESS;
        } else {
            return VK_INCOMPLETE;
        }
    }
}

#define INIT_HOOK(_vt, _inst, fn) _vt.fn = reinterpret_cast<PFN_vk##fn>(vtable.GetInstanceProcAddr(_inst, "vk" #fn))
InstanceData::InstanceData(VkInstance inst, PFN_vkGetInstanceProcAddr gpa, const VkAllocationCallbacks* alloc)
    : instance{inst}, allocator{alloc}, physical_device_map{} {
    vtable.GetInstanceProcAddr = gpa;
    INIT_HOOK(vtable, instance, CreateInstance);
    INIT_HOOK(vtable, instance, DestroyInstance);
    INIT_HOOK(vtable, instance, CreateDevice);
    INIT_HOOK(vtable, instance, EnumeratePhysicalDevices);
    INIT_HOOK(vtable, instance, EnumerateDeviceExtensionProperties);
    INIT_HOOK(vtable, instance, EnumerateInstanceExtensionProperties);
    INIT_HOOK(vtable, instance, GetPhysicalDeviceFeatures2);
    INIT_HOOK(vtable, instance, GetPhysicalDeviceFeatures2KHR);
    INIT_HOOK(vtable, instance, GetPhysicalDeviceProperties);
}
#undef INIT_HOOK

std::shared_ptr<PhysicalDeviceData> InstanceData::GetPhysicalDeviceData(VkPhysicalDevice physical_device) const {
    if (const auto result = physical_device_map.find(physical_device); result->first) {
        return result->second;
    } else {
        LOG("[%s] ERROR: Failed to find physical device in accelerating structure referenced by instance.",
            VK_EXT_PIPELINE_PROPERTIES_EXTENSION_NAME);
        exit(-1);
    }
}

static VkLayerInstanceCreateInfo* GetChainInfo(const VkInstanceCreateInfo* pCreateInfo, VkLayerFunction func) {
    auto chain_info = reinterpret_cast<VkLayerInstanceCreateInfo*>(const_cast<void*>(pCreateInfo->pNext));
    while (chain_info && !(chain_info->sType == VK_STRUCTURE_TYPE_LOADER_INSTANCE_CREATE_INFO && chain_info->function == func)) {
        chain_info = reinterpret_cast<VkLayerInstanceCreateInfo*>(const_cast<void*>(chain_info->pNext));
    }
    ASSERT(chain_info != NULL);
    return chain_info;
}

static std::atomic<uint32_t> device_counter = 0;

#define INIT_HOOK(_vt, _dev, fn) _vt.fn = reinterpret_cast<PFN_vk##fn>(vtable.GetDeviceProcAddr(_dev, "vk" #fn))
#define INIT_HOOK_ALIAS(_vt, _dev, fn, fn_alias) \
    _vt.fn_alias = (_vt.fn_alias != nullptr) ? _vt.fn_alias : reinterpret_cast<PFN_vk##fn>(vtable.GetDeviceProcAddr(_dev, "vk" #fn))

DeviceData::DeviceData(VkDevice device, const VkDeviceCreateInfo* ci, PFN_vkGetDeviceProcAddr gpa, bool enable_ext,
                       const VkAllocationCallbacks* alloc)
    : device(device),
      create_info(ci),
      vtable{gpa},
      allocator(alloc),
      ycbcr_map{},
      sampler_map{},
      descriptor_set_layout_map{},
      pipeline_layout_map{},
      shader_module_map{},
      graphics_pipeline_map{},
      compute_pipeline_map{},
      renderpass_map{},
      renderpass2_map{},
      obj_res_info{},
      obj_counter{0},
      id{device_counter++} {
    INIT_HOOK(vtable, device, DestroyDevice);
    INIT_HOOK(vtable, device, CreateSemaphore);
    INIT_HOOK(vtable, device, DestroySemaphore);
    INIT_HOOK(vtable, device, CreateShaderModule);
    INIT_HOOK(vtable, device, DestroyShaderModule);
    INIT_HOOK(vtable, device, AllocateMemory);
    INIT_HOOK(vtable, device, CreatePipelineCache);
    INIT_HOOK(vtable, device, DestroyPipelineCache);
    INIT_HOOK(vtable, device, CreateGraphicsPipelines);
    INIT_HOOK(vtable, device, CreateComputePipelines);
    INIT_HOOK(vtable, device, DestroyPipeline);
    INIT_HOOK(vtable, device, CreateDescriptorSetLayout);
    INIT_HOOK(vtable, device, DestroyDescriptorSetLayout);
    INIT_HOOK(vtable, device, CreatePipelineLayout);
    INIT_HOOK(vtable, device, DestroyPipelineLayout);
    INIT_HOOK(vtable, device, CreateRenderPass);
    INIT_HOOK(vtable, device, CreateRenderPass2);
    INIT_HOOK(vtable, device, DestroyRenderPass);
    INIT_HOOK(vtable, device, CreateSampler);
    INIT_HOOK(vtable, device, DestroySampler);
    INIT_HOOK(vtable, device, CreateSamplerYcbcrConversion);
    INIT_HOOK(vtable, device, DestroySamplerYcbcrConversion);
    INIT_HOOK(vtable, device, CreateSwapchainKHR);
    INIT_HOOK(vtable, device, DestroySwapchainKHR);
    INIT_HOOK(vtable, device, CreateImage);
    INIT_HOOK(vtable, device, DestroyImage);
    INIT_HOOK(vtable, device, CreateImageView);
    INIT_HOOK(vtable, device, DestroyImageView);
    INIT_HOOK(vtable, device, AllocateCommandBuffers);
    INIT_HOOK(vtable, device, CreateFence);
    INIT_HOOK(vtable, device, DestroyFence);
    INIT_HOOK(vtable, device, CreateBuffer);
    INIT_HOOK(vtable, device, DestroyBuffer);
    INIT_HOOK(vtable, device, CreateBufferView);
    INIT_HOOK(vtable, device, DestroyBufferView);
    INIT_HOOK(vtable, device, CreateEvent);
    INIT_HOOK(vtable, device, DestroyEvent);
    INIT_HOOK(vtable, device, CreateQueryPool);
    INIT_HOOK(vtable, device, CreateDescriptorPool);
    INIT_HOOK(vtable, device, AllocateDescriptorSets);
    INIT_HOOK(vtable, device, FreeDescriptorSets);
    INIT_HOOK(vtable, device, CreateFramebuffer);
    INIT_HOOK(vtable, device, DestroyFramebuffer);
    INIT_HOOK(vtable, device, CreateCommandPool);
    if (enable_ext) {
        INIT_HOOK(vtable, device, GetPipelinePropertiesEXT);
    }
}
#undef INIT_HOOK
#undef INIT_HOOK_ALIAS

DeviceData::~DeviceData() { writeDeviceObjResHeader(); }

void DeviceData::writeDeviceObjResHeader() {
    std::stringstream result;
    auto header_path =
        std::string(getBaseDirectoryPath()) + std::string(getProcessName()) + "_objectResInfo_" + std::to_string(id) + ".hpp";
    // clang-format off
    result <<
        "#ifndef " << getProcessName() << "_objectResInfo_" << id << "_HPP\n" <<
        "#define " << getProcessName() << "_objectResInfo_" << id << "_HPP\n\n" <<
        "#include <vulkan/vulkan_sc_core.h>\n\n" <<
        "static VkDeviceObjectReservationCreateInfo g_objectResCreateInfo_" << id << " {};\n" <<
        "static void SetObjectResCreateInfo()\n" <<
        "{\n" <<
        "\tg_objectResCreateInfo_" << id << ".sType                                      = VK_STRUCTURE_TYPE_DEVICE_OBJECT_RESERVATION_CREATE_INFO;\n" <<
        "\tg_objectResCreateInfo_" << id << ".pNext                                      = nullptr;\n" <<
        "\tg_objectResCreateInfo_" << id << ".semaphoreRequestCount                      = " << obj_res_info.semaphoreHighWatermark << ";\n" <<
        "\tg_objectResCreateInfo_" << id << ".commandBufferRequestCount                  = " << obj_res_info.commandBufferHighWatermark << ";\n" <<
        "\tg_objectResCreateInfo_" << id << ".fenceRequestCount                          = " << obj_res_info.fenceHighWatermark << ";\n" <<
        "\tg_objectResCreateInfo_" << id << ".deviceMemoryRequestCount                   = " << obj_res_info.deviceMemoryHighWatermark << ";\n" <<
        "\tg_objectResCreateInfo_" << id << ".bufferRequestCount                         = " << obj_res_info.bufferHighWatermark << ";\n" <<
        "\tg_objectResCreateInfo_" << id << ".imageRequestCount                          = " << obj_res_info.imageHighWatermark << ";\n" <<
        "\tg_objectResCreateInfo_" << id << ".eventRequestCount                          = " << obj_res_info.eventHighWatermark << ";\n" <<
        "\tg_objectResCreateInfo_" << id << ".queryPoolRequestCount                      = " << obj_res_info.queryPoolHighWatermark << ";\n" <<
        "\tg_objectResCreateInfo_" << id << ".bufferViewRequestCount                     = " << obj_res_info.bufferViewHighWatermark << ";\n" <<
        "\tg_objectResCreateInfo_" << id << ".imageViewRequestCount                      = " << obj_res_info.imageViewHighWatermark << ";\n" <<
        "\tg_objectResCreateInfo_" << id << ".layeredImageViewRequestCount               = " << obj_res_info.layeredImageViewHighWatermark << ";\n" <<
        "\tg_objectResCreateInfo_" << id << ".pipelineCacheRequestCount                  = " << obj_res_info.pipelineCacheHighWatermark << ";\n" <<
        "\tg_objectResCreateInfo_" << id << ".pipelineLayoutRequestCount                 = " << obj_res_info.pipelineLayoutHighWatermark << ";\n" <<
        "\tg_objectResCreateInfo_" << id << ".renderPassRequestCount                     = " << obj_res_info.renderPassHighWatermark << ";\n" <<
        "\tg_objectResCreateInfo_" << id << ".graphicsPipelineRequestCount               = " << obj_res_info.graphicsPipelineHighWatermark << ";\n" <<
        "\tg_objectResCreateInfo_" << id << ".computePipelineRequestCount                = " << obj_res_info.computePipelineHighWatermark << ";\n" <<
        "\tg_objectResCreateInfo_" << id << ".descriptorSetLayoutRequestCount            = " << obj_res_info.descriptorSetLayoutHighWatermark << ";\n" <<
        "\tg_objectResCreateInfo_" << id << ".samplerRequestCount                        = " << obj_res_info.samplerHighWatermark << ";\n" <<
        "\tg_objectResCreateInfo_" << id << ".descriptorPoolRequestCount                 = " << obj_res_info.descriptorPoolHighWatermark << ";\n" <<
        "\tg_objectResCreateInfo_" << id << ".descriptorSetRequestCount                  = " << obj_res_info.descriptorSetHighWatermark << ";\n" <<
        "\tg_objectResCreateInfo_" << id << ".framebufferRequestCount                    = " << obj_res_info.framebufferHighWatermark << ";\n" <<
        "\tg_objectResCreateInfo_" << id << ".commandPoolRequestCount                    = " << obj_res_info.commandPoolHighWatermark << ";\n" <<
        "\tg_objectResCreateInfo_" << id << ".samplerYcbcrConversionRequestCount         = " << obj_res_info.samplerYcbcrConversionHighWatermark << ";\n" <<
        "\tg_objectResCreateInfo_" << id << ".swapchainRequestCount                      = " << obj_res_info.swapchainHighWatermark << ";\n" <<
        "\tg_objectResCreateInfo_" << id << ".subpassDescriptionRequestCount             = " << obj_res_info.subpassDescriptionHighWatermark << ";\n" <<
        "\tg_objectResCreateInfo_" << id << ".attachmentDescriptionRequestCount          = " << obj_res_info.attachmentDescriptionHighWatermark << ";\n" <<
        "\tg_objectResCreateInfo_" << id << ".descriptorSetLayoutBindingRequestCount     = " << obj_res_info.descriptorSetLayoutBindingHighWatermark << ";\n" <<
        "\tg_objectResCreateInfo_" << id << ".descriptorSetLayoutBindingLimit            = " << obj_res_info.descriptorSetLayoutBindingLimit << ";\n" <<
        "\tg_objectResCreateInfo_" << id << ".maxImageViewMipLevels                      = " << obj_res_info.maxImageViewMipLevels << ";\n" <<
        "\tg_objectResCreateInfo_" << id << ".maxImageViewArrayLayers                    = " << obj_res_info.maxImageViewArrayLayers << ";\n" <<
        "\tg_objectResCreateInfo_" << id << ".maxLayeredImageViewMipLevels               = " << obj_res_info.maxLayeredImageViewMipLevels << ";\n" <<
        "\tg_objectResCreateInfo_" << id << ".maxOcclusionQueriesPerPool                 = " << obj_res_info.maxOcclusionQueriesPerPool << ";\n" <<
        "\tg_objectResCreateInfo_" << id << ".maxPipelineStatisticsQueriesPerPool        = " << obj_res_info.maxPipelineStatisticsQueriesPerPool << ";\n" <<
        "\tg_objectResCreateInfo_" << id << ".maxTimestampQueriesPerPool                 = " << obj_res_info.maxTimestampQueriesPerPool << ";\n" <<
        "\tg_objectResCreateInfo_" << id << ".maxImmutableSamplersPerDescriptorSetLayout = " << obj_res_info.maxImmutableSamplersPerDescriptorSetLayout << ";\n" <<
        "}\n\n" <<
        "#endif\n";
    // clang-format on
    std::ofstream header_file(header_path);
    if (header_file) {
        header_file << result.rdbuf();
    } else {
        LOG("[%s] ERROR: Unable to open: %s", VK_EXT_PIPELINE_PROPERTIES_EXTENSION_NAME, header_path.c_str());
    }
}

static VkLayerDeviceCreateInfo* GetChainInfo(const VkDeviceCreateInfo* pCreateInfo, VkLayerFunction func) {
    auto chain_info = reinterpret_cast<VkLayerDeviceCreateInfo*>(const_cast<void*>(pCreateInfo->pNext));
    while (chain_info && !(chain_info->sType == VK_STRUCTURE_TYPE_LOADER_DEVICE_CREATE_INFO && chain_info->function == func)) {
        chain_info = reinterpret_cast<VkLayerDeviceCreateInfo*>(const_cast<void*>(chain_info->pNext));
    }
    ASSERT(chain_info != NULL);
    return chain_info;
}

DeviceFeatures::DeviceFeatures(const VkDeviceCreateInfo* create_info) : DeviceFeatures() {
    const VkBaseInStructure* chain = reinterpret_cast<const VkBaseInStructure*>(create_info->pNext);

    while (chain != nullptr) {
        switch (chain->sType) {
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_PROPERTIES_FEATURES_EXT: {
                pipeline_props = true;
            } break;

            default:
                break;
        }
        chain = chain->pNext;
    }
}

YcbcrData::YcbcrData(const VkSamplerYcbcrConversionCreateInfo* ci, DeviceData& device_data)
    : create_info{ci}, id{++device_data.obj_counter} {}

SamplerData::SamplerData(const VkSamplerCreateInfo* ci, DeviceData& device_data)
    : create_info{ci}, ycbcr_data{}, id{++device_data.obj_counter} {
    auto ycbcr = vku::FindStructInPNextChain<VkSamplerYcbcrConversionInfo>(create_info.ptr());
    if (ycbcr) {
        if (auto result = device_data.ycbcr_map.find(ycbcr->conversion); result->first) {
            // Rewrite handle(s) in create info to id(s)
            ycbcr->conversion = reinterpret_cast<VkSamplerYcbcrConversion>(result->second->id);
            // Store local copy of dependent create info
            ycbcr_data = *result->second;
        } else {
            LOG("[%s] ERROR: Failed to find ycbcr conversion in accelerating structure referenced by sampler.",
                VK_EXT_PIPELINE_PROPERTIES_EXTENSION_NAME);
        }
    }
}

DescriptorSetLayoutData::DescriptorSetLayoutData(const VkDescriptorSetLayoutCreateInfo* ci, DeviceData& device_data)
    : create_info{ci}, immutable_sampler_data{}, id{++device_data.obj_counter} {
    for (uint32_t i = 0; i < create_info.bindingCount; ++i) {
        if ((create_info.pBindings[i].descriptorType == VK_DESCRIPTOR_TYPE_SAMPLER ||
             create_info.pBindings[i].descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) &&
            create_info.pBindings[i].pImmutableSamplers != nullptr) {
            immutable_sampler_data.reserve(create_info.pBindings[i].descriptorCount);
            for (uint32_t j = 0; j < create_info.pBindings[i].descriptorCount; ++j) {
                if (auto result = device_data.sampler_map.find(create_info.pBindings[i].pImmutableSamplers[j]); result->first) {
                    // Rewrite handle(s) in create info to id(s)
                    create_info.pBindings[i].pImmutableSamplers[j] = reinterpret_cast<VkSampler>(result->second->id);
                    // Store local copy of dependent create info
                    immutable_sampler_data.emplace_back(*result->second);
                } else {
                    LOG("[%s] ERROR: Failed to find sampler in accelerating structure referenced by descriptor set layout.",
                        VK_EXT_PIPELINE_PROPERTIES_EXTENSION_NAME);
                }
            }
        }
    }
}

PipelineLayoutData::PipelineLayoutData(const VkPipelineLayoutCreateInfo* ci, DeviceData& device_data)
    : create_info{ci}, descriptor_set_layout_data{}, id{++device_data.obj_counter} {
    descriptor_set_layout_data.reserve(create_info.setLayoutCount);
    for (uint32_t i = 0; i < create_info.setLayoutCount; ++i) {
        if (auto result = device_data.descriptor_set_layout_map.find(create_info.pSetLayouts[i]); result->first) {
            // Rewrite handle(s) in create info to id(s)
            create_info.pSetLayouts[i] = reinterpret_cast<VkDescriptorSetLayout>(static_cast<std::uintptr_t>(result->second->id));
            // Store local copy of dependent create info
            descriptor_set_layout_data.emplace_back(*result->second);
        } else {
            LOG("[%s] ERROR: Failed to find descriptor set layout in accelerating structure referenced by pipeline layout.",
                VK_EXT_PIPELINE_PROPERTIES_EXTENSION_NAME);
        }
    }
}

ShaderModuleData::ShaderModuleData(const VkShaderModuleCreateInfo* ci) : create_info(ci) {}

RenderPassData::RenderPassData(const VkRenderPassCreateInfo* ci) : create_info(ci) {}

RenderPass2Data::RenderPass2Data(const VkRenderPassCreateInfo2* ci) : create_info(ci) {}

ImageViewData::ImageViewData(const VkImageViewCreateInfo* ci) : create_info(ci) {}

GraphicsPipelineData::GraphicsPipelineData(const VkGraphicsPipelineCreateInfo* ci, DeviceData& device_data)
    : create_info{ci, true, true}, pipeline_layout_data{}, renderpass_data{}, shader_module_data{}, id{++device_data.obj_counter} {
    if (auto result = device_data.pipeline_layout_map.find(create_info.layout); result->first) {
        // Save pipeline layout
        pipeline_layout_data = *result->second;
        // Rewrite handle of pipeline layout in graphics pipeline with autoinc id
        create_info.layout = reinterpret_cast<VkPipelineLayout>(result->second->id);
    } else {
        LOG("[%s] ERROR: Failed to find pipeline layout in accelerating structure referenced by graphics pipeline.",
            VK_EXT_PIPELINE_PROPERTIES_EXTENSION_NAME);
    }

    if (auto result = device_data.renderpass_map.find(create_info.renderPass); result->first) {
        renderpass_data.emplace<RenderPassData>(result->second->create_info.ptr());
    } else if (auto result2 = device_data.renderpass2_map.find(create_info.renderPass); result2->first) {
        renderpass_data.emplace<RenderPass2Data>(result2->second->create_info.ptr());
    } else {
        LOG("[%s] ERROR: Failed to find renderpass(2) in accelerating structure referenced by graphics pipeline.",
            VK_EXT_PIPELINE_PROPERTIES_EXTENSION_NAME);
    }

    for (uint32_t i = 0; i < create_info.stageCount; ++i) {
        if (auto result = device_data.shader_module_map.find(create_info.pStages[i].module); result->first) {
            shader_module_data.emplace_back(*result->second);
        } else {
            LOG("[%s] ERROR: Failed to find shader module in accelerating structure referenced by graphics pipeline.",
                VK_EXT_PIPELINE_PROPERTIES_EXTENSION_NAME);
        }
    }

    GenJsonUuidAndWriteToDisk(device_data.create_info, device_data.id);
}

ComputePipelineData::ComputePipelineData(const VkComputePipelineCreateInfo* ci, DeviceData& device_data)
    : create_info{ci}, pipeline_layout_data{}, shader_module_data{}, id{++device_data.obj_counter} {
    if (auto result = device_data.pipeline_layout_map.find(create_info.layout); result->first) {
        // Save pipeline layout
        pipeline_layout_data = *result->second;
        // Rewrite handle of pipeline layout in graphics pipeline with autoinc id
        create_info.layout = reinterpret_cast<VkPipelineLayout>(result->second->id);
    } else {
        LOG("[%s] ERROR: Failed to find pipeline layout in accelerating structure referenced by compute pipeline.",
            VK_EXT_PIPELINE_PROPERTIES_EXTENSION_NAME);
    }

    if (auto result = device_data.shader_module_map.find(create_info.stage.module); result->first) {
        shader_module_data = *result->second;
    } else {
        LOG("[%s] ERROR: Failed to find shader module in accelerating structure referenced by compute pipeline.",
            VK_EXT_PIPELINE_PROPERTIES_EXTENSION_NAME);
    }

    GenJsonUuidAndWriteToDisk(device_data.create_info, device_data.id);
}

// Begin layer logic

// Specifically implemented functions

VKAPI_ATTR VkResult VKAPI_CALL CreateInstance(const VkInstanceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator,
                                              VkInstance* pInstance) {
    VkLayerInstanceCreateInfo* chain_info = GetChainInfo(pCreateInfo, VK_LAYER_LINK_INFO);

    ASSERT(chain_info->u.pLayerInfo);
    auto gpa = chain_info->u.pLayerInfo->pfnNextGetInstanceProcAddr;
    auto create_instance = reinterpret_cast<PFN_vkCreateInstance>(gpa(NULL, "vkCreateInstance"));
    if (create_instance == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    // Advance the link info for the next element on the chain
    chain_info->u.pLayerInfo = chain_info->u.pLayerInfo->pNext;

    VkResult result = create_instance(pCreateInfo, pAllocator, pInstance);
    if (result != VK_SUCCESS) {
        return result;
    }
    try {
        auto instance_data =
            std::make_shared<InstanceData>(*pInstance, gpa, pAllocator ? pAllocator : &extension_layer::kDefaultAllocator);

        instance_data_map.insert(DispatchKey(*pInstance), instance_data);

        instance_data->api_version = pCreateInfo->pApplicationInfo ? pCreateInfo->pApplicationInfo->apiVersion : 0;

        // InitLayerSettings(pCreateInfo, pAllocator, &instance_data->layer_settings);
    } catch (const std::bad_alloc&) {
        auto destroy_instance = reinterpret_cast<PFN_vkDestroyInstance>(gpa(NULL, "vkDestroyInstance"));
        destroy_instance(*pInstance, pAllocator);
        result = VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL EnumeratePhysicalDevices(VkInstance instance, uint32_t* pPhysicalDeviceCount,
                                                        VkPhysicalDevice* pPhysicalDevices) {
    auto instance_data = GetInstanceData(instance);
    VkResult result =
        instance_data->vtable.EnumeratePhysicalDevices(instance_data->instance, pPhysicalDeviceCount, pPhysicalDevices);
    if ((result == VK_SUCCESS || result == VK_INCOMPLETE) && pPhysicalDevices != nullptr) {
        for (uint32_t i = 0; i < *pPhysicalDeviceCount; i++) {
            VkPhysicalDeviceProperties properties{};
            auto physical_device = pPhysicalDevices[i];

            if (instance_data->physical_device_map.find(physical_device) != instance_data->physical_device_map.end()) {
                continue;
            }
            auto pdd = std::make_shared<PhysicalDeviceData>();
            pdd->physical_device = physical_device;

            instance_data->vtable.GetPhysicalDeviceProperties(physical_device, &properties);
            pdd->api_version = properties.apiVersion;

            instance_data->physical_device_map.insert(physical_device, pdd);
        }
    }
    return result;
}

VKAPI_ATTR void VKAPI_ATTR VKAPI_CALL DestroyInstance(VkInstance instance, const VkAllocationCallbacks* pAllocator) {
    auto key = DispatchKey(instance);
    if (auto result = instance_data_map.find(key); result->first) {
        result->second->vtable.DestroyInstance(instance, pAllocator);
        instance_data_map.erase(key);
    } else {
        LOG("[%s] ERROR: Failed to find instance in accelerating structure.", VK_EXT_PIPELINE_PROPERTIES_EXTENSION_NAME);
    }
}

VKAPI_ATTR VkResult VKAPI_ATTR VKAPI_CALL CreateDevice(VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo* pCreateInfo,
                                                       const VkAllocationCallbacks* pAllocator, VkDevice* pDevice) {
    VkResult result;
    auto instance_data = GetInstanceData(physicalDevice);
    auto pdd = instance_data->GetPhysicalDeviceData(physicalDevice);

    VkLayerDeviceCreateInfo* chain_info = GetChainInfo(pCreateInfo, VK_LAYER_LINK_INFO);

    ASSERT(chain_info->u.pLayerInfo);
    PFN_vkGetInstanceProcAddr instance_proc_addr = chain_info->u.pLayerInfo->pfnNextGetInstanceProcAddr;
    PFN_vkCreateDevice create_device = (PFN_vkCreateDevice)instance_proc_addr(instance_data->instance, "vkCreateDevice");
    PFN_vkGetDeviceProcAddr gdpa = chain_info->u.pLayerInfo->pfnNextGetDeviceProcAddr;
    if (instance_data->vtable.CreateDevice == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    DeviceFeatures features(pCreateInfo);

    // Advance the link info for the next element on the chain
    chain_info->u.pLayerInfo = chain_info->u.pLayerInfo->pNext;

    try {
        bool enable_layer_ext = features.pipeline_props && !pdd->lower_has_pipeline_props;
        // Filter out our extension name and feature struct, in a copy of the create info.
        // Only enable device hooks if synchronization2 extension is enabled AND
        // the physical device doesn't support it already or we are force enabled.
        if (enable_layer_ext) {
            vku::safe_VkDeviceCreateInfo create_info(pCreateInfo);
            vku::RemoveExtension(create_info, VK_EXT_PIPELINE_PROPERTIES_EXTENSION_NAME);
            vku::RemoveFromPnext(create_info, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_PROPERTIES_FEATURES_EXT);

            result = create_device(physicalDevice, create_info.ptr(), pAllocator, pDevice);
        } else {
            result = create_device(physicalDevice, pCreateInfo, pAllocator, pDevice);
        }

        if (result != VK_SUCCESS) {
            return result;
        }
        auto alloccb = pAllocator ? pAllocator : instance_data->allocator;
        auto device_data = std::make_shared<DeviceData>(*pDevice, pCreateInfo, gdpa, enable_layer_ext, alloccb);

        device_data_map.insert(DispatchKey(*pDevice), device_data);
    } catch (const std::bad_alloc&) {
        auto destroy_device = reinterpret_cast<PFN_vkDestroyDevice>(gdpa(*pDevice, "vkDestroyDevice"));
        destroy_device(*pDevice, pAllocator);
        result = VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    return result;
}

VKAPI_ATTR void VKAPI_ATTR VKAPI_CALL DestroyDevice(VkDevice device, const VkAllocationCallbacks* pAllocator) {
    auto key = DispatchKey(device);
    if (auto result = device_data_map.find(key); result->first) {
        result->second->vtable.DestroyDevice(device, pAllocator);
        device_data_map.erase(key);
    } else {
        LOG("[%s] ERROR: Failed to find device in accelerating structure.", VK_EXT_PIPELINE_PROPERTIES_EXTENSION_NAME);
    }
}

// Autogen instance functions

VKAPI_ATTR void VKAPI_ATTR VKAPI_CALL GetPhysicalDeviceFeatures2KHR(VkPhysicalDevice physicalDevice,
                                                                    VkPhysicalDeviceFeatures2* pFeatures) {
    auto instance_data = GetInstanceData(physicalDevice);
    instance_data->vtable.GetPhysicalDeviceFeatures2KHR(physicalDevice, pFeatures);
    if (auto pdppf = vku::FindStructInPNextChain<VkPhysicalDevicePipelinePropertiesFeaturesEXT>(pFeatures->pNext);
        pdppf != nullptr) {
        pdppf->pipelinePropertiesIdentifier = true;
    }
}
VKAPI_ATTR void VKAPI_ATTR VKAPI_CALL GetPhysicalDeviceFeatures2(VkPhysicalDevice physicalDevice,
                                                                 VkPhysicalDeviceFeatures2* pFeatures) {
    auto instance_data = GetInstanceData(physicalDevice);
    instance_data->vtable.GetPhysicalDeviceFeatures2(physicalDevice, pFeatures);
    if (auto pdppf = vku::FindStructInPNextChain<VkPhysicalDevicePipelinePropertiesFeaturesEXT>(pFeatures->pNext);
        pdppf != nullptr) {
        pdppf->pipelinePropertiesIdentifier = true;
    }
}

// Autogen device functions

VKAPI_ATTR VkResult VKAPI_ATTR VKAPI_CALL CreateSemaphore(VkDevice device, const VkSemaphoreCreateInfo* pCreateInfo,
                                                          const VkAllocationCallbacks* pAllocator, VkSemaphore* pSemaphore) {
    auto device_data = GetDeviceData(device);
    VkResult result = device_data->vtable.CreateSemaphore(device, pCreateInfo, pAllocator, pSemaphore);
    if (result >= 0) {
        atomic_max(device_data->obj_res_info.semaphoreHighWatermark, ++device_data->obj_res_info.semaphoreRequestCount);
    }
    return result;
}
VKAPI_ATTR void VKAPI_ATTR VKAPI_CALL DestroySemaphore(VkDevice device, VkSemaphore fence,
                                                       const VkAllocationCallbacks* pAllocator) {
    auto device_data = GetDeviceData(device);
    device_data->vtable.DestroySemaphore(device, fence, pAllocator);
    if (fence != VK_NULL_HANDLE) {
        device_data->obj_res_info.semaphoreRequestCount--;
    }
}
VKAPI_ATTR VkResult VKAPI_ATTR VKAPI_CALL CreateShaderModule(VkDevice device, const VkShaderModuleCreateInfo* pCreateInfo,
                                                             const VkAllocationCallbacks* pAllocator,
                                                             VkShaderModule* pShaderModule) {
    auto device_data = GetDeviceData(device);
    VkResult result = device_data->vtable.CreateShaderModule(device, pCreateInfo, pAllocator, pShaderModule);
    if (result >= 0) device_data->shader_module_map.insert(*pShaderModule, std::make_shared<ShaderModuleData>(pCreateInfo));
    return result;
}
VKAPI_ATTR void VKAPI_ATTR VKAPI_CALL DestroyShaderModule(VkDevice device, VkShaderModule shaderModule,
                                                          const VkAllocationCallbacks* pAllocator) {
    auto device_data = GetDeviceData(device);
    device_data->vtable.DestroyShaderModule(device, shaderModule, pAllocator);
    if (shaderModule != VK_NULL_HANDLE) device_data->shader_module_map.erase(shaderModule);
}
VKAPI_ATTR VkResult VKAPI_ATTR VKAPI_CALL CreateGraphicsPipelines(VkDevice device, VkPipelineCache pipelineCache,
                                                                  uint32_t createInfoCount,
                                                                  const VkGraphicsPipelineCreateInfo* pCreateInfos,
                                                                  const VkAllocationCallbacks* pAllocator, VkPipeline* pPipelines) {
    auto device_data = GetDeviceData(device);
    VkResult result =
        device_data->vtable.CreateGraphicsPipelines(device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
    if (result >= 0)
        for (uint32_t i = 0; i < createInfoCount; ++i)
            if (pPipelines[i] != VK_NULL_HANDLE) {
                atomic_max(device_data->obj_res_info.graphicsPipelineHighWatermark,
                           ++device_data->obj_res_info.graphicsPipelineRequestCount);
                auto graphics_pipeline_data = std::make_shared<GraphicsPipelineData>(&pCreateInfos[i], *device_data);
                device_data->graphics_pipeline_map.insert(pPipelines[i], graphics_pipeline_data);
            }

    return result;
}
VKAPI_ATTR VkResult VKAPI_ATTR VKAPI_CALL CreateComputePipelines(VkDevice device, VkPipelineCache pipelineCache,
                                                                 uint32_t createInfoCount,
                                                                 const VkComputePipelineCreateInfo* pCreateInfos,
                                                                 const VkAllocationCallbacks* pAllocator, VkPipeline* pPipelines) {
    auto device_data = GetDeviceData(device);
    VkResult result =
        device_data->vtable.CreateComputePipelines(device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
    if (result >= 0)
        for (uint32_t i = 0; i < createInfoCount; ++i)
            if (pPipelines[i] != VK_NULL_HANDLE) {
                atomic_max(device_data->obj_res_info.computePipelineHighWatermark,
                           ++device_data->obj_res_info.computePipelineRequestCount);
                auto compute_pipeline_data = std::make_shared<ComputePipelineData>(&pCreateInfos[i], *device_data);
                device_data->compute_pipeline_map.insert(pPipelines[i], compute_pipeline_data);
            }

    return result;
}
VKAPI_ATTR void VKAPI_ATTR VKAPI_CALL DestroyPipeline(VkDevice device, VkPipeline pipeline,
                                                      const VkAllocationCallbacks* pAllocator) {
    auto device_data = GetDeviceData(device);
    device_data->vtable.DestroyPipeline(device, pipeline, pAllocator);
    if (pipeline != VK_NULL_HANDLE) {
        if (auto result = device_data->graphics_pipeline_map.find(pipeline); result->first) {
            device_data->obj_res_info.graphicsPipelineRequestCount--;
            device_data->graphics_pipeline_map.erase(pipeline);
        } else if (auto result2 = device_data->compute_pipeline_map.find(pipeline); result2->first) {
            device_data->obj_res_info.computePipelineRequestCount--;
            device_data->compute_pipeline_map.erase(pipeline);
        } else {
            LOG("[%s] ERROR: Failed to find pipeline in accelerating structure.", VK_EXT_PIPELINE_PROPERTIES_EXTENSION_NAME);
        }
    }
}
VKAPI_ATTR VkResult VKAPI_ATTR VKAPI_CALL CreateDescriptorSetLayout(VkDevice device,
                                                                    const VkDescriptorSetLayoutCreateInfo* pCreateInfo,
                                                                    const VkAllocationCallbacks* pAllocator,
                                                                    VkDescriptorSetLayout* pSetLayout) {
    auto device_data = GetDeviceData(device);
    VkResult result = device_data->vtable.CreateDescriptorSetLayout(device, pCreateInfo, pAllocator, pSetLayout);
    if (result >= 0) {
        atomic_max(device_data->obj_res_info.descriptorSetLayoutHighWatermark,
                   ++device_data->obj_res_info.descriptorSetLayoutRequestCount);
        atomic_max(device_data->obj_res_info.descriptorSetLayoutBindingHighWatermark,
                   device_data->obj_res_info.descriptorSetLayoutBindingRequestCount += pCreateInfo->bindingCount);
        atomic_max(device_data->obj_res_info.descriptorSetLayoutBindingLimit,
                   [=](auto first, auto last) {
                       auto it = std::max_element(
                           first, last, [](const VkDescriptorSetLayoutBinding& lhs, const VkDescriptorSetLayoutBinding& rhs) {
                               return lhs.binding < rhs.binding;
                           });
                       return it != last ? it->binding : 0;
                   }(pCreateInfo->pBindings, pCreateInfo->pBindings + pCreateInfo->bindingCount) +
                       1);
        atomic_max(device_data->obj_res_info.maxImmutableSamplersPerDescriptorSetLayout,
                   std::accumulate(pCreateInfo->pBindings, pCreateInfo->pBindings + pCreateInfo->bindingCount, 0u,
                                   [](const uint32_t& curr_max, const VkDescriptorSetLayoutBinding& dslb) {
                                       return std::max(curr_max, dslb.pImmutableSamplers != nullptr ? dslb.descriptorCount : 0);
                                   }));
        device_data->descriptor_set_layout_map.insert(*pSetLayout,
                                                      std::make_shared<DescriptorSetLayoutData>(pCreateInfo, *device_data));
    }
    return result;
}
VKAPI_ATTR void VKAPI_ATTR VKAPI_CALL DestroyDescriptorSetLayout(VkDevice device, VkDescriptorSetLayout descriptorSetLayout,
                                                                 const VkAllocationCallbacks* pAllocator) {
    auto device_data = GetDeviceData(device);
    device_data->vtable.DestroyDescriptorSetLayout(device, descriptorSetLayout, pAllocator);
    if (descriptorSetLayout != VK_NULL_HANDLE) {
        device_data->obj_res_info.descriptorSetLayoutRequestCount--;
        if (auto result = device_data->descriptor_set_layout_map.find(descriptorSetLayout); result->first) {
            device_data->obj_res_info.descriptorSetLayoutBindingRequestCount -= result->second->create_info.bindingCount;
        }
        device_data->descriptor_set_layout_map.erase(descriptorSetLayout);
    }
}
VKAPI_ATTR VkResult VKAPI_ATTR VKAPI_CALL CreatePipelineLayout(VkDevice device, const VkPipelineLayoutCreateInfo* pCreateInfo,
                                                               const VkAllocationCallbacks* pAllocator,
                                                               VkPipelineLayout* pPipelineLayout) {
    auto device_data = GetDeviceData(device);
    VkResult result = device_data->vtable.CreatePipelineLayout(device, pCreateInfo, pAllocator, pPipelineLayout);
    if (result >= 0) {
        atomic_max(device_data->obj_res_info.pipelineLayoutHighWatermark, ++device_data->obj_res_info.pipelineLayoutRequestCount);
        device_data->pipeline_layout_map.insert(*pPipelineLayout, std::make_shared<PipelineLayoutData>(pCreateInfo, *device_data));
    }
    return result;
}
VKAPI_ATTR void VKAPI_ATTR VKAPI_CALL DestroyPipelineLayout(VkDevice device, VkPipelineLayout pipelineLayout,
                                                            const VkAllocationCallbacks* pAllocator) {
    auto device_data = GetDeviceData(device);
    device_data->vtable.DestroyPipelineLayout(device, pipelineLayout, pAllocator);
    if (pipelineLayout != VK_NULL_HANDLE) {
        device_data->obj_res_info.pipelineLayoutRequestCount--;
        device_data->pipeline_layout_map.erase(pipelineLayout);
    }
}
VKAPI_ATTR VkResult VKAPI_ATTR VKAPI_CALL CreateRenderPass(VkDevice device, const VkRenderPassCreateInfo* pCreateInfo,
                                                           const VkAllocationCallbacks* pAllocator, VkRenderPass* pRenderPass) {
    auto device_data = GetDeviceData(device);
    VkResult result = device_data->vtable.CreateRenderPass(device, pCreateInfo, pAllocator, pRenderPass);
    if (result >= 0) {
        atomic_max(device_data->obj_res_info.renderPassHighWatermark, ++device_data->obj_res_info.renderPassRequestCount);
        atomic_max(device_data->obj_res_info.subpassDescriptionHighWatermark,
                   device_data->obj_res_info.subpassDescriptionRequestCount += pCreateInfo->subpassCount);
        atomic_max(device_data->obj_res_info.attachmentDescriptionHighWatermark,
                   device_data->obj_res_info.attachmentDescriptionRequestCount += pCreateInfo->attachmentCount);
        device_data->renderpass_map.insert(*pRenderPass, std::make_shared<RenderPassData>(pCreateInfo));
    }
    return result;
}
VKAPI_ATTR VkResult VKAPI_ATTR VKAPI_CALL CreateRenderPass2(VkDevice device, const VkRenderPassCreateInfo2* pCreateInfo,
                                                            const VkAllocationCallbacks* pAllocator, VkRenderPass* pRenderPass) {
    auto device_data = GetDeviceData(device);
    VkResult result = device_data->vtable.CreateRenderPass2(device, pCreateInfo, pAllocator, pRenderPass);
    if (result >= 0) {
        atomic_max(device_data->obj_res_info.renderPassHighWatermark, ++device_data->obj_res_info.renderPassRequestCount);
        atomic_max(device_data->obj_res_info.subpassDescriptionHighWatermark,
                   device_data->obj_res_info.subpassDescriptionRequestCount += pCreateInfo->subpassCount);
        atomic_max(device_data->obj_res_info.attachmentDescriptionHighWatermark,
                   device_data->obj_res_info.attachmentDescriptionRequestCount += pCreateInfo->attachmentCount);
        device_data->renderpass2_map.insert(*pRenderPass, std::make_shared<RenderPass2Data>(pCreateInfo));
    }
    return result;
}
VKAPI_ATTR void VKAPI_ATTR VKAPI_CALL DestroyRenderPass(VkDevice device, VkRenderPass renderPass,
                                                        const VkAllocationCallbacks* pAllocator) {
    auto device_data = GetDeviceData(device);
    device_data->vtable.DestroyRenderPass(device, renderPass, pAllocator);
    if (renderPass != VK_NULL_HANDLE) {
        device_data->obj_res_info.renderPassRequestCount--;
        if (auto result = device_data->renderpass_map.find(renderPass); result->first) {
            device_data->obj_res_info.subpassDescriptionRequestCount -= result->second->create_info.subpassCount;
            device_data->obj_res_info.attachmentDescriptionRequestCount -= result->second->create_info.attachmentCount;
            device_data->renderpass_map.erase(renderPass);
        } else if (auto result2 = device_data->renderpass2_map.find(renderPass); result2->first) {
            device_data->obj_res_info.subpassDescriptionRequestCount -= result2->second->create_info.subpassCount;
            device_data->obj_res_info.attachmentDescriptionRequestCount -= result2->second->create_info.attachmentCount;
            device_data->renderpass2_map.erase(renderPass);
        } else {
            LOG("[%s] ERROR: Failed to find renderpass in accelerating structure.", VK_EXT_PIPELINE_PROPERTIES_EXTENSION_NAME);
        }
    }
}
VKAPI_ATTR VkResult VKAPI_ATTR VKAPI_CALL CreateSampler(VkDevice device, const VkSamplerCreateInfo* pCreateInfo,
                                                        const VkAllocationCallbacks* pAllocator, VkSampler* pSampler) {
    auto device_data = GetDeviceData(device);
    VkResult result = device_data->vtable.CreateSampler(device, pCreateInfo, pAllocator, pSampler);
    if (result >= 0) {
        atomic_max(device_data->obj_res_info.samplerHighWatermark, ++device_data->obj_res_info.samplerRequestCount);
        device_data->sampler_map.insert(*pSampler, std::make_shared<SamplerData>(pCreateInfo, *device_data));
    }
    return result;
}
VKAPI_ATTR void VKAPI_ATTR VKAPI_CALL DestroySampler(VkDevice device, VkSampler sampler, const VkAllocationCallbacks* pAllocator) {
    auto device_data = GetDeviceData(device);
    device_data->vtable.DestroySampler(device, sampler, pAllocator);
    if (sampler != VK_NULL_HANDLE) {
        device_data->obj_res_info.samplerRequestCount--;
        device_data->sampler_map.erase(sampler);
    }
}
VKAPI_ATTR VkResult VKAPI_ATTR VKAPI_CALL CreateSamplerYcbcrConversion(VkDevice device,
                                                                       const VkSamplerYcbcrConversionCreateInfo* pCreateInfo,
                                                                       const VkAllocationCallbacks* pAllocator,
                                                                       VkSamplerYcbcrConversion* pYcbcrConversion) {
    auto device_data = GetDeviceData(device);
    VkResult result = device_data->vtable.CreateSamplerYcbcrConversion(device, pCreateInfo, pAllocator, pYcbcrConversion);
    if (result >= 0) {
        atomic_max(device_data->obj_res_info.samplerYcbcrConversionHighWatermark,
                   ++device_data->obj_res_info.samplerYcbcrConversionRequestCount);
        device_data->ycbcr_map.insert(*pYcbcrConversion, std::make_shared<YcbcrData>(pCreateInfo, *device_data));
    }
    return result;
}
VKAPI_ATTR void VKAPI_ATTR VKAPI_CALL DestroySamplerYcbcrConversion(VkDevice device, VkSamplerYcbcrConversion ycbcrConversion,
                                                                    const VkAllocationCallbacks* pAllocator) {
    auto device_data = GetDeviceData(device);
    device_data->vtable.DestroySamplerYcbcrConversion(device, ycbcrConversion, pAllocator);
    if (ycbcrConversion != VK_NULL_HANDLE) {
        device_data->obj_res_info.samplerYcbcrConversionRequestCount--;
        device_data->ycbcr_map.erase(ycbcrConversion);
    }
}
VKAPI_ATTR VkResult VKAPI_ATTR VKAPI_CALL CreateSwapchainKHR(VkDevice device, const VkSwapchainCreateInfoKHR* pCreateInfo,
                                                             const VkAllocationCallbacks* pAllocator, VkSwapchainKHR* pSwapchain) {
    auto device_data = GetDeviceData(device);
    VkResult result = device_data->vtable.CreateSwapchainKHR(device, pCreateInfo, pAllocator, pSwapchain);
    if (result >= 0) {
        atomic_max(device_data->obj_res_info.swapchainHighWatermark, ++device_data->obj_res_info.swapchainRequestCount);
    }
    return result;
}
VKAPI_ATTR void VKAPI_ATTR VKAPI_CALL DestroySwapchainKHR(VkDevice device, VkSwapchainKHR swapchain,
                                                          const VkAllocationCallbacks* pAllocator) {
    auto device_data = GetDeviceData(device);
    device_data->vtable.DestroySwapchainKHR(device, swapchain, pAllocator);
    if (swapchain != VK_NULL_HANDLE) {
        device_data->obj_res_info.swapchainRequestCount--;
    }
}
VKAPI_ATTR VkResult VKAPI_ATTR VKAPI_CALL CreateImage(VkDevice device, const VkImageCreateInfo* pCreateInfo,
                                                      const VkAllocationCallbacks* pAllocator, VkImage* pImage) {
    auto device_data = GetDeviceData(device);
    VkResult result = device_data->vtable.CreateImage(device, pCreateInfo, pAllocator, pImage);
    if (result >= 0) {
        atomic_max(device_data->obj_res_info.imageHighWatermark, ++device_data->obj_res_info.imageRequestCount);
    }
    return result;
}
VKAPI_ATTR void VKAPI_ATTR VKAPI_CALL DestroyImage(VkDevice device, VkImage image, const VkAllocationCallbacks* pAllocator) {
    auto device_data = GetDeviceData(device);
    device_data->vtable.DestroyImage(device, image, pAllocator);
    if (image != VK_NULL_HANDLE) {  /// TODO: Check all DestroyXyz predicates
        device_data->obj_res_info.imageRequestCount--;
    }
}
VKAPI_ATTR VkResult VKAPI_ATTR VKAPI_CALL CreateImageView(VkDevice device, const VkImageViewCreateInfo* pCreateInfo,
                                                          const VkAllocationCallbacks* pAllocator, VkImageView* pView) {
    auto device_data = GetDeviceData(device);
    VkResult result = device_data->vtable.CreateImageView(device, pCreateInfo, pAllocator, pView);
    if (result >= 0) {
        atomic_max(device_data->obj_res_info.imageViewHighWatermark, ++device_data->obj_res_info.imageViewRequestCount);
        if (pCreateInfo->subresourceRange.layerCount > 1) {
            atomic_max(device_data->obj_res_info.layeredImageViewHighWatermark,
                       ++device_data->obj_res_info.layeredImageViewRequestCount);
            atomic_max(device_data->obj_res_info.maxLayeredImageViewMipLevels, pCreateInfo->subresourceRange.levelCount);
        }
        atomic_max(device_data->obj_res_info.maxImageViewMipLevels, pCreateInfo->subresourceRange.levelCount);
        atomic_max(device_data->obj_res_info.maxImageViewArrayLayers, pCreateInfo->subresourceRange.layerCount);
        device_data->image_view_map.insert(*pView, std::make_shared<ImageViewData>(pCreateInfo));
    }
    return result;
}
VKAPI_ATTR void VKAPI_ATTR VKAPI_CALL DestroyImageView(VkDevice device, VkImageView imageView,
                                                       const VkAllocationCallbacks* pAllocator) {
    auto device_data = GetDeviceData(device);
    device_data->vtable.DestroyImageView(device, imageView, pAllocator);
    if (imageView != VK_NULL_HANDLE) {
        device_data->obj_res_info.imageViewRequestCount--;
        if (auto result = device_data->image_view_map.find(imageView); result->first) {
            if (result->second->create_info.subresourceRange.layerCount > 1) {
                device_data->obj_res_info.layeredImageViewRequestCount--;
            }
        }
        device_data->image_view_map.erase(imageView);
    }
}
VKAPI_ATTR VkResult VKAPI_ATTR VKAPI_CALL AllocateCommandBuffers(VkDevice device, const VkCommandBufferAllocateInfo* pAllocateInfo,
                                                                 VkCommandBuffer* pCommandBuffers) {
    auto device_data = GetDeviceData(device);
    VkResult result = device_data->vtable.AllocateCommandBuffers(device, pAllocateInfo, pCommandBuffers);
    if (result >= 0 && pAllocateInfo != nullptr) {
        atomic_max(device_data->obj_res_info.commandBufferHighWatermark,
                   device_data->obj_res_info.commandBufferRequestCount += pAllocateInfo->commandBufferCount);
    }
    return result;
}
VKAPI_ATTR VkResult VKAPI_ATTR VKAPI_CALL CreateFence(VkDevice device, const VkFenceCreateInfo* pCreateInfo,
                                                      const VkAllocationCallbacks* pAllocator, VkFence* pFence) {
    auto device_data = GetDeviceData(device);
    VkResult result = device_data->vtable.CreateFence(device, pCreateInfo, pAllocator, pFence);
    if (result >= 0) {
        atomic_max(device_data->obj_res_info.fenceHighWatermark, ++device_data->obj_res_info.fenceRequestCount);
    }
    return result;
}
VKAPI_ATTR void VKAPI_ATTR VKAPI_CALL DestroyFence(VkDevice device, VkFence fence, const VkAllocationCallbacks* pAllocator) {
    auto device_data = GetDeviceData(device);
    device_data->vtable.DestroyFence(device, fence, pAllocator);
    if (fence != VK_NULL_HANDLE) {
        device_data->obj_res_info.fenceRequestCount--;
    }
}
VKAPI_ATTR VkResult VKAPI_ATTR VKAPI_CALL AllocateMemory(VkDevice device, const VkMemoryAllocateInfo* pAllocateInfo,
                                                         const VkAllocationCallbacks* pAllocator, VkDeviceMemory* pMemory) {
    auto device_data = GetDeviceData(device);
    VkResult result = device_data->vtable.AllocateMemory(device, pAllocateInfo, pAllocator, pMemory);
    if (result >= 0) {
        atomic_max(device_data->obj_res_info.deviceMemoryHighWatermark, ++device_data->obj_res_info.deviceMemoryRequestCount);
    }
    return result;
}
VKAPI_ATTR VkResult VKAPI_ATTR VKAPI_CALL CreatePipelineCache(VkDevice device, const VkPipelineCacheCreateInfo* pCreateInfo,
                                                              const VkAllocationCallbacks* pAllocator,
                                                              VkPipelineCache* pPipelineCache) {
    auto device_data = GetDeviceData(device);
    VkResult result = device_data->vtable.CreatePipelineCache(device, pCreateInfo, pAllocator, pPipelineCache);
    if (result >= 0) {
        atomic_max(device_data->obj_res_info.pipelineCacheHighWatermark, ++device_data->obj_res_info.pipelineCacheRequestCount);
    }
    return result;
}
VKAPI_ATTR void VKAPI_ATTR VKAPI_CALL DestroyPipelineCache(VkDevice device, VkPipelineCache pipelineCache,
                                                           const VkAllocationCallbacks* pAllocator) {
    auto device_data = GetDeviceData(device);
    device_data->vtable.DestroyPipelineCache(device, pipelineCache, pAllocator);
    if (pipelineCache != VK_NULL_HANDLE) {
        device_data->obj_res_info.pipelineCacheRequestCount--;
    }
}
VKAPI_ATTR VkResult VKAPI_ATTR VKAPI_CALL CreateBuffer(VkDevice device, const VkBufferCreateInfo* pCreateInfo,
                                                       const VkAllocationCallbacks* pAllocator, VkBuffer* pBuffer) {
    auto device_data = GetDeviceData(device);
    VkResult result = device_data->vtable.CreateBuffer(device, pCreateInfo, pAllocator, pBuffer);
    if (result >= 0) {
        atomic_max(device_data->obj_res_info.bufferHighWatermark, ++device_data->obj_res_info.bufferRequestCount);
    }
    return result;
}
VKAPI_ATTR void VKAPI_ATTR VKAPI_CALL DestroyBuffer(VkDevice device, VkBuffer buffer, const VkAllocationCallbacks* pAllocator) {
    auto device_data = GetDeviceData(device);
    device_data->vtable.DestroyBuffer(device, buffer, pAllocator);
    if (buffer != VK_NULL_HANDLE) {
        device_data->obj_res_info.bufferRequestCount--;
    }
}
VKAPI_ATTR VkResult VKAPI_ATTR VKAPI_CALL CreateBufferView(VkDevice device, const VkBufferViewCreateInfo* pCreateInfo,
                                                           const VkAllocationCallbacks* pAllocator, VkBufferView* pView) {
    auto device_data = GetDeviceData(device);
    VkResult result = device_data->vtable.CreateBufferView(device, pCreateInfo, pAllocator, pView);
    if (result >= 0) {
        atomic_max(device_data->obj_res_info.bufferViewHighWatermark, ++device_data->obj_res_info.bufferViewRequestCount);
    }
    return result;
}
VKAPI_ATTR void VKAPI_ATTR VKAPI_CALL DestroyBufferView(VkDevice device, VkBufferView bufferView,
                                                        const VkAllocationCallbacks* pAllocator) {
    auto device_data = GetDeviceData(device);
    device_data->vtable.DestroyBufferView(device, bufferView, pAllocator);
    if (bufferView != VK_NULL_HANDLE) {
        device_data->obj_res_info.bufferViewRequestCount--;
    }
}
VKAPI_ATTR VkResult VKAPI_ATTR VKAPI_CALL CreateEvent(VkDevice device, const VkEventCreateInfo* pCreateInfo,
                                                      const VkAllocationCallbacks* pAllocator, VkEvent* pEvent) {
    auto device_data = GetDeviceData(device);
    VkResult result = device_data->vtable.CreateEvent(device, pCreateInfo, pAllocator, pEvent);
    if (result >= 0) {
        atomic_max(device_data->obj_res_info.eventHighWatermark, ++device_data->obj_res_info.eventRequestCount);
    }
    return result;
}
VKAPI_ATTR void VKAPI_ATTR VKAPI_CALL DestroyEvent(VkDevice device, VkEvent event, const VkAllocationCallbacks* pAllocator) {
    auto device_data = GetDeviceData(device);
    device_data->vtable.DestroyEvent(device, event, pAllocator);
    if (event != VK_NULL_HANDLE) {
        device_data->obj_res_info.eventRequestCount--;
    }
}
VKAPI_ATTR VkResult VKAPI_ATTR VKAPI_CALL CreateQueryPool(VkDevice device, const VkQueryPoolCreateInfo* pCreateInfo,
                                                          const VkAllocationCallbacks* pAllocator, VkQueryPool* pQueryPool) {
    auto device_data = GetDeviceData(device);
    VkResult result = device_data->vtable.CreateQueryPool(device, pCreateInfo, pAllocator, pQueryPool);
    if (result >= 0) {
        atomic_max(device_data->obj_res_info.queryPoolHighWatermark, ++device_data->obj_res_info.queryPoolRequestCount);
        if (pCreateInfo->queryType == VK_QUERY_TYPE_OCCLUSION) {
            atomic_max(device_data->obj_res_info.maxOcclusionQueriesPerPool, pCreateInfo->queryCount);
        } else if (pCreateInfo->queryType == VK_QUERY_TYPE_PIPELINE_STATISTICS) {
            atomic_max(device_data->obj_res_info.maxPipelineStatisticsQueriesPerPool, pCreateInfo->queryCount);
        } else if (pCreateInfo->queryType == VK_QUERY_TYPE_TIMESTAMP) {
            atomic_max(device_data->obj_res_info.maxTimestampQueriesPerPool, pCreateInfo->queryCount);
        }
    }
    return result;
}
VKAPI_ATTR VkResult VKAPI_ATTR VKAPI_CALL CreateDescriptorPool(VkDevice device, const VkDescriptorPoolCreateInfo* pCreateInfo,
                                                               const VkAllocationCallbacks* pAllocator,
                                                               VkDescriptorPool* pDescriptorPool) {
    auto device_data = GetDeviceData(device);
    VkResult result = device_data->vtable.CreateDescriptorPool(device, pCreateInfo, pAllocator, pDescriptorPool);
    if (pCreateInfo) {
        atomic_max(device_data->obj_res_info.descriptorPoolHighWatermark, ++device_data->obj_res_info.descriptorPoolRequestCount);
    }
    return result;
}
VKAPI_ATTR VkResult VKAPI_ATTR VKAPI_CALL AllocateDescriptorSets(VkDevice device, const VkDescriptorSetAllocateInfo* pAllocateInfo,
                                                                 VkDescriptorSet* pDescriptorSets) {
    auto device_data = GetDeviceData(device);
    VkResult result = device_data->vtable.AllocateDescriptorSets(device, pAllocateInfo, pDescriptorSets);
    if (pAllocateInfo && pAllocateInfo != nullptr) {
        atomic_max(device_data->obj_res_info.descriptorSetHighWatermark,
                   device_data->obj_res_info.descriptorSetRequestCount += pAllocateInfo->descriptorSetCount);
    }
    return result;
}
VKAPI_ATTR VkResult VKAPI_ATTR VKAPI_CALL FreeDescriptorSets(VkDevice device, VkDescriptorPool descriptorPool,
                                                             uint32_t descriptorSetCount, const VkDescriptorSet* pDescriptorSets) {
    auto device_data = GetDeviceData(device);
    VkResult result = device_data->vtable.FreeDescriptorSets(device, descriptorPool, descriptorSetCount, pDescriptorSets);
    uint32_t non_null_count = 0;
    for (uint32_t i = 0; i < descriptorSetCount; ++i) {
        if (pDescriptorSets[i] != VK_NULL_HANDLE) {
            ++non_null_count;
        }
    }
    if (result >= 0) {
        device_data->obj_res_info.descriptorSetRequestCount -= non_null_count;
    }
    return result;
}
VKAPI_ATTR VkResult VKAPI_ATTR VKAPI_CALL CreateFramebuffer(VkDevice device, const VkFramebufferCreateInfo* pCreateInfo,
                                                            const VkAllocationCallbacks* pAllocator, VkFramebuffer* pFramebuffer) {
    auto device_data = GetDeviceData(device);
    VkResult result = device_data->vtable.CreateFramebuffer(device, pCreateInfo, pAllocator, pFramebuffer);
    if (result >= 0) {
        atomic_max(device_data->obj_res_info.framebufferHighWatermark, ++device_data->obj_res_info.framebufferRequestCount);
    }
    return result;
}
VKAPI_ATTR void VKAPI_ATTR VKAPI_CALL DestroyFramebuffer(VkDevice device, VkFramebuffer framebuffer,
                                                         const VkAllocationCallbacks* pAllocator) {
    auto device_data = GetDeviceData(device);
    device_data->vtable.DestroyFramebuffer(device, framebuffer, pAllocator);
    if (framebuffer != VK_NULL_HANDLE) {
        device_data->obj_res_info.framebufferRequestCount--;
    }
}
VKAPI_ATTR VkResult VKAPI_ATTR VKAPI_CALL CreateCommandPool(VkDevice device, const VkCommandPoolCreateInfo* pCreateInfo,
                                                            const VkAllocationCallbacks* pAllocator, VkCommandPool* pCommandPool) {
    auto device_data = GetDeviceData(device);
    VkResult result = device_data->vtable.CreateCommandPool(device, pCreateInfo, pAllocator, pCommandPool);
    if (result >= 0) {
        atomic_max(device_data->obj_res_info.commandPoolHighWatermark, ++device_data->obj_res_info.commandPoolRequestCount);
    }
    return result;
}

// Gen logic

void GraphicsPipelineData::GenJsonUuidAndWriteToDisk(vku::safe_VkDeviceCreateInfo& device_create_info, std::uintptr_t device_id) {
    VpjGenerator generator = vpjCreateGenerator();
    vpjSetMD5PipelineUUIDGeneration(generator, true);
    VpjData data{};
    const char* msg_ = nullptr;
    const char* result_json = nullptr;

    // Graphics pipeline
    data.graphicsPipelineState.pGraphicsPipeline = create_info.ptr();

    // Renderpass
    if (std::holds_alternative<RenderPassData>(renderpass_data)) {
        data.graphicsPipelineState.pRenderPass = std::get<RenderPassData>(renderpass_data).create_info.ptr();
    } else {
        data.graphicsPipelineState.pRenderPass = std::get<RenderPass2Data>(renderpass_data).create_info.ptr();
    }

    // Descriptor set layouts, immutable samplers, ycbcr samplers
    std::vector<vku::safe_VkDescriptorSetLayoutCreateInfo> descriptor_set_layouts;
    std::vector<vku::safe_VkSamplerCreateInfo> immutable_samplers;
    std::vector<vku::safe_VkSamplerYcbcrConversionCreateInfo> ycbcr_samplers;
    std::vector<std::string> names_storage;
    std::vector<const char*> descriptor_set_layout_names;
    std::vector<uintptr_t> descriptor_set_layout_ids;
    std::vector<const char*> immutable_sampler_names;
    std::vector<uintptr_t> immutable_sampler_ids;
    std::vector<const char*> ycbcr_sampler_names;
    std::vector<uintptr_t> ycbcr_sampler_ids;
    // Serialize unique object create infos and generate names
    //
    // NOTE: names_storage persists object names until serialization. The xyz_names variables hold pointers to the
    //       persisted data. However, names_storage is built incrementally, and without pre-reserving storage, the
    //       xyz_names variables will hold pointers to possibly (and really) small-string-optimized storage, so
    //       they'll be referring to non-stable heap pointers, but to the inside reallocated std::vector heap. We
    //       allocate slightly pessimistically, assuming all objects are unique.
    uint32_t names_required = 0;
    for (size_t i = 0; i < pipeline_layout_data.descriptor_set_layout_data.size(); ++i) {
        ++names_required;
        for (size_t j = 0; j < pipeline_layout_data.descriptor_set_layout_data[i].immutable_sampler_data.size(); ++j) {
            ++names_required;
            if (pipeline_layout_data.descriptor_set_layout_data[i].immutable_sampler_data[j].ycbcr_data.has_value()) {
                ++names_required;
            }
        }
    }
    names_storage.reserve(names_required);
    auto not_contains = [](const auto& container, const uintptr_t id) {
        return std::find(std::cbegin(container), std::cend(container), id) == std::cend(container);
    };
    using namespace std::string_literals;
    for (size_t i = 0; i < pipeline_layout_data.descriptor_set_layout_data.size(); ++i) {
        const auto& descriptor_set_layout_data = pipeline_layout_data.descriptor_set_layout_data[i];
        if (not_contains(descriptor_set_layout_ids, descriptor_set_layout_data.id)) {
            names_storage.push_back("DescriptorSetLayout"s + std::to_string(descriptor_set_layout_ids.size() + 1));
            descriptor_set_layout_ids.push_back(descriptor_set_layout_data.id);
            descriptor_set_layout_names.push_back(names_storage.back().c_str());
            descriptor_set_layouts.push_back(descriptor_set_layout_data.create_info);
        }
        for (size_t j = 0; j < pipeline_layout_data.descriptor_set_layout_data[i].immutable_sampler_data.size(); ++j) {
            const auto& immutable_sampler_data = pipeline_layout_data.descriptor_set_layout_data[i].immutable_sampler_data[j];
            if (not_contains(immutable_sampler_ids, immutable_sampler_data.id)) {
                names_storage.push_back("ImmutableSampler"s + std::to_string(immutable_sampler_names.size() + 1));
                immutable_sampler_ids.push_back(immutable_sampler_data.id);
                immutable_sampler_names.push_back(names_storage.back().c_str());
                immutable_samplers.push_back(immutable_sampler_data.create_info);
            }
            if (pipeline_layout_data.descriptor_set_layout_data[i].immutable_sampler_data[j].ycbcr_data.has_value()) {
                const auto& ycbcr_sampler_data =
                    pipeline_layout_data.descriptor_set_layout_data[i].immutable_sampler_data[j].ycbcr_data.value();
                if (not_contains(ycbcr_sampler_ids, ycbcr_sampler_data.id)) {
                    names_storage.push_back("YcbcrSampler"s + std::to_string(ycbcr_sampler_names.size() + 1));
                    ycbcr_sampler_ids.push_back(ycbcr_sampler_data.id);
                    ycbcr_sampler_names.push_back(names_storage.back().c_str());
                    ycbcr_samplers.push_back(ycbcr_sampler_data.create_info);
                }
            }
        }
    }
    // Rewrite autoinc ids to indices
    auto find_id = [](const auto& container, const uintptr_t id) {
        return std::find(std::cbegin(container), std::cend(container), id);
    };
    for (size_t i = 0; i < pipeline_layout_data.create_info.setLayoutCount; ++i) {
        pipeline_layout_data.create_info.pSetLayouts[i] = reinterpret_cast<VkDescriptorSetLayout>(
            std::distance(std::cbegin(descriptor_set_layout_ids),
                          find_id(descriptor_set_layout_ids, (uintptr_t)pipeline_layout_data.create_info.pSetLayouts[i])));
    }
    for (auto& descriptor_set_layout : descriptor_set_layouts) {
        for (size_t i = 0; i < descriptor_set_layout.bindingCount; ++i) {
            if (descriptor_set_layout.pBindings[i].pImmutableSamplers) {
                for (size_t j = 0; j < descriptor_set_layout.pBindings[i].descriptorCount; ++j) {
                    descriptor_set_layout.pBindings[i].pImmutableSamplers[j] = reinterpret_cast<VkSampler>(std::distance(
                        std::cbegin(immutable_sampler_ids),
                        find_id(immutable_sampler_ids, (uintptr_t)descriptor_set_layout.pBindings[i].pImmutableSamplers[j])));
                }
            }
        }
    }
    for (auto& immutable_sampler : immutable_samplers) {
        auto ycbcr = vku::FindStructInPNextChain<VkSamplerYcbcrConversionInfo>(const_cast<void*>(immutable_sampler.pNext));
        if (ycbcr) {
            ycbcr->conversion = reinterpret_cast<VkSamplerYcbcrConversion>(
                std::distance(std::cbegin(ycbcr_sampler_ids), find_id(ycbcr_sampler_ids, (uintptr_t)ycbcr->conversion)));
        }
    }
    // Commit
    data.graphicsPipelineState.pPipelineLayout = pipeline_layout_data.create_info.ptr();
    data.graphicsPipelineState.descriptorSetLayoutCount = static_cast<uint32_t>(descriptor_set_layouts.size());
    data.graphicsPipelineState.pDescriptorSetLayouts = descriptor_set_layouts.size() ? descriptor_set_layouts.data() : nullptr;
    data.graphicsPipelineState.ppDescriptorSetLayoutNames =
        descriptor_set_layouts.size() ? descriptor_set_layout_names.data() : nullptr;
    data.graphicsPipelineState.immutableSamplerCount = static_cast<uint32_t>(immutable_samplers.size());
    data.graphicsPipelineState.pImmutableSamplers = immutable_samplers.size() ? immutable_samplers.data() : 0;
    data.graphicsPipelineState.ppImmutableSamplerNames = immutable_samplers.size() ? immutable_sampler_names.data() : nullptr;
    data.graphicsPipelineState.ycbcrSamplerCount = static_cast<uint32_t>(ycbcr_samplers.size());
    data.graphicsPipelineState.pYcbcrSamplers = ycbcr_samplers.size() ? ycbcr_samplers.data() : nullptr;
    data.graphicsPipelineState.ppYcbcrSamplerNames = ycbcr_samplers.size() ? ycbcr_sampler_names.data() : nullptr;

    // Shaders
    auto shader_filenames = get_shader_filenames(*create_info.ptr(), std::string(getProcessName()),
                                                 static_cast<uint32_t>(device_id), static_cast<uint32_t>(id));
    for (size_t i = 0; i < shader_module_data.size(); ++i) {
        auto& shader_ci = shader_module_data[i].create_info;
        auto shader_path = std::string(getBaseDirectoryPath()) + shader_filenames.filenames[i].pFilename;
        std::ofstream spv_file(shader_path, std::ios::binary);
        if (spv_file) {
            spv_file.write(reinterpret_cast<const char*>(shader_ci.pCode), shader_ci.codeSize);
        } else {
            LOG("[%s] ERROR: Unable to open: %s", VK_EXT_PIPELINE_PROPERTIES_EXTENSION_NAME, shader_path.c_str());
        }
    }
    data.graphicsPipelineState.shaderFileNameCount = static_cast<uint32_t>(shader_filenames.filenames.size());
    data.graphicsPipelineState.pShaderFileNames = shader_filenames.filenames.data();

    // Physical device features
    if (!vpjFilterDeviceFeatures(generator, create_info.ptr(), &data.graphicsPipelineState.pPhysicalDeviceFeatures, &msg_)) {
        LOG("[%s] ERROR: Unable to filter physical device features: %s", VK_EXT_PIPELINE_PROPERTIES_EXTENSION_NAME, msg_);
    }

    // Enabled extensions
    data.enabledExtensionCount = device_create_info.enabledExtensionCount;
    data.ppEnabledExtensions = const_cast<const char**>(device_create_info.ppEnabledExtensionNames);

    if (!vpjGeneratePipelineJson(generator, &data, &result_json, &msg_)) {
        LOG("[%s] ERROR: Unable to generate pipeline JSON: %s", VK_EXT_PIPELINE_PROPERTIES_EXTENSION_NAME, msg_);
    }
    if (!vpjGetGeneratedPipelineUUID(generator, uuid.data(), &msg_)) {
        LOG("[%s] ERROR: Unable to obtain generated pipeline UUID: %s", VK_EXT_PIPELINE_PROPERTIES_EXTENSION_NAME, msg_);
    }

    auto pipeline_path = std::string(getBaseDirectoryPath()) + std::string(getProcessName()) + "_device_" +
                         std::to_string(device_id) + "_pipeline_" + std::to_string(id) + ".json";
    std::ofstream pipeline_file(pipeline_path);
    if (pipeline_file) {
        pipeline_file << result_json;
    } else {
        LOG("[%s] ERROR: Unable to open: %s", VK_EXT_PIPELINE_PROPERTIES_EXTENSION_NAME, pipeline_path.c_str());
    }
}

void ComputePipelineData::GenJsonUuidAndWriteToDisk(vku::safe_VkDeviceCreateInfo& device_create_info, std::uintptr_t device_id) {
    VpjGenerator generator = vpjCreateGenerator();
    vpjSetMD5PipelineUUIDGeneration(generator, true);
    VpjData data{};
    const char* msg_ = nullptr;
    const char* result_json = nullptr;

    // Compute pipeline
    data.computePipelineState.pComputePipeline = create_info.ptr();

    // Descriptor set layouts, immutable samplers, ycbcr samplers
    std::vector<vku::safe_VkDescriptorSetLayoutCreateInfo> descriptor_set_layouts;
    std::vector<vku::safe_VkSamplerCreateInfo> immutable_samplers;
    std::vector<vku::safe_VkSamplerYcbcrConversionCreateInfo> ycbcr_samplers;
    std::vector<std::string> names_storage;
    std::vector<const char*> descriptor_set_layout_names;
    std::vector<uintptr_t> descriptor_set_layout_ids;
    std::vector<const char*> immutable_sampler_names;
    std::vector<uintptr_t> immutable_sampler_ids;
    std::vector<const char*> ycbcr_sampler_names;
    std::vector<uintptr_t> ycbcr_sampler_ids;
    // Serialize unique object create infos and generate names
    //
    // NOTE: names_storage persists object names until serialization. The xyz_names variables hold pointers to the
    //       persisted data. However, names_storage is built incrementally, and without pre-reserving storage, the
    //       xyz_names variables will hold pointers to possibly (and really) small-string-optimized storage, so
    //       they'll be referring to non-stable heap pointers, but to the inside reallocated std::vector heap. We
    //       allocate slightly pessimistically, assuming all objects are unique.
    uint32_t names_required = 0;
    for (size_t i = 0; i < pipeline_layout_data.descriptor_set_layout_data.size(); ++i) {
        ++names_required;
        for (size_t j = 0; j < pipeline_layout_data.descriptor_set_layout_data[i].immutable_sampler_data.size(); ++j) {
            ++names_required;
            if (pipeline_layout_data.descriptor_set_layout_data[i].immutable_sampler_data[j].ycbcr_data.has_value()) {
                ++names_required;
            }
        }
    }
    names_storage.reserve(names_required);
    auto not_contains = [](const auto& container, const uintptr_t id) {
        return std::find(std::cbegin(container), std::cend(container), id) == std::cend(container);
    };
    using namespace std::string_literals;
    for (size_t i = 0; i < pipeline_layout_data.descriptor_set_layout_data.size(); ++i) {
        const auto& descriptor_set_layout_data = pipeline_layout_data.descriptor_set_layout_data[i];
        if (not_contains(descriptor_set_layout_ids, descriptor_set_layout_data.id)) {
            names_storage.push_back("DescriptorSetLayout"s + std::to_string(descriptor_set_layout_ids.size() + 1));
            descriptor_set_layout_ids.push_back(descriptor_set_layout_data.id);
            descriptor_set_layout_names.push_back(names_storage.back().c_str());
            descriptor_set_layouts.push_back(descriptor_set_layout_data.create_info);
        }
        for (size_t j = 0; j < pipeline_layout_data.descriptor_set_layout_data[i].immutable_sampler_data.size(); ++j) {
            const auto& immutable_sampler_data = pipeline_layout_data.descriptor_set_layout_data[i].immutable_sampler_data[j];
            if (not_contains(immutable_sampler_ids, immutable_sampler_data.id)) {
                names_storage.push_back("ImmutableSampler"s + std::to_string(immutable_sampler_names.size() + 1));
                immutable_sampler_ids.push_back(immutable_sampler_data.id);
                immutable_sampler_names.push_back(names_storage.back().c_str());
                immutable_samplers.push_back(immutable_sampler_data.create_info);
            }
            if (pipeline_layout_data.descriptor_set_layout_data[i].immutable_sampler_data[j].ycbcr_data.has_value()) {
                const auto& ycbcr_sampler_data =
                    pipeline_layout_data.descriptor_set_layout_data[i].immutable_sampler_data[j].ycbcr_data.value();
                if (not_contains(ycbcr_sampler_ids, ycbcr_sampler_data.id)) {
                    names_storage.push_back("YcbcrSampler"s + std::to_string(ycbcr_sampler_names.size() + 1));
                    ycbcr_sampler_ids.push_back(ycbcr_sampler_data.id);
                    ycbcr_sampler_names.push_back(names_storage.back().c_str());
                    ycbcr_samplers.push_back(ycbcr_sampler_data.create_info);
                }
            }
        }
    }
    // Rewrite autoinc ids to indices
    auto find_id = [](const auto& container, const uintptr_t id) {
        return std::find(std::cbegin(container), std::cend(container), id);
    };
    for (size_t i = 0; i < pipeline_layout_data.create_info.setLayoutCount; ++i) {
        pipeline_layout_data.create_info.pSetLayouts[i] = reinterpret_cast<VkDescriptorSetLayout>(
            std::distance(std::cbegin(descriptor_set_layout_ids),
                          find_id(descriptor_set_layout_ids, (uintptr_t)pipeline_layout_data.create_info.pSetLayouts[i])));
    }
    for (auto& descriptor_set_layout : descriptor_set_layouts) {
        for (size_t i = 0; i < descriptor_set_layout.bindingCount; ++i) {
            if (descriptor_set_layout.pBindings[i].pImmutableSamplers) {
                for (size_t j = 0; j < descriptor_set_layout.pBindings[i].descriptorCount; ++j) {
                    descriptor_set_layout.pBindings[i].pImmutableSamplers[j] = reinterpret_cast<VkSampler>(std::distance(
                        std::cbegin(immutable_sampler_ids),
                        find_id(immutable_sampler_ids, (uintptr_t)descriptor_set_layout.pBindings[i].pImmutableSamplers[j])));
                }
            }
        }
    }
    for (auto& immutable_sampler : immutable_samplers) {
        auto ycbcr = vku::FindStructInPNextChain<VkSamplerYcbcrConversionInfo>(const_cast<void*>(immutable_sampler.pNext));
        if (ycbcr) {
            ycbcr->conversion = reinterpret_cast<VkSamplerYcbcrConversion>(
                std::distance(std::cbegin(ycbcr_sampler_ids), find_id(ycbcr_sampler_ids, (uintptr_t)ycbcr->conversion)));
        }
    }
    // Commit
    data.computePipelineState.pPipelineLayout = pipeline_layout_data.create_info.ptr();
    data.computePipelineState.descriptorSetLayoutCount = static_cast<uint32_t>(descriptor_set_layouts.size());
    data.computePipelineState.pDescriptorSetLayouts = descriptor_set_layouts.size() ? descriptor_set_layouts.data() : nullptr;
    data.computePipelineState.ppDescriptorSetLayoutNames =
        descriptor_set_layout_names.size() ? descriptor_set_layout_names.data() : nullptr;
    data.computePipelineState.immutableSamplerCount = static_cast<uint32_t>(immutable_samplers.size());
    data.computePipelineState.pImmutableSamplers = immutable_samplers.size() ? immutable_samplers.data() : 0;
    data.computePipelineState.ppImmutableSamplerNames = immutable_sampler_names.size() ? immutable_sampler_names.data() : nullptr;
    data.computePipelineState.ycbcrSamplerCount = static_cast<uint32_t>(ycbcr_samplers.size());
    data.computePipelineState.pYcbcrSamplers = ycbcr_samplers.size() ? ycbcr_samplers.data() : nullptr;
    data.computePipelineState.ppYcbcrSamplerNames = ycbcr_sampler_names.size() ? ycbcr_sampler_names.data() : nullptr;

    // Shaders
    auto shader_filename = get_shader_filenames(*create_info.ptr(), std::string(getProcessName()), static_cast<uint32_t>(device_id),
                                                static_cast<uint32_t>(id));
    auto shader_ci = shader_module_data.create_info;
    auto shader_path = std::string(getBaseDirectoryPath()) + shader_filename.filenames[0].pFilename;
    std::ofstream spv_file(shader_path, std::ios::binary);
    if (spv_file) {
        spv_file.write(reinterpret_cast<const char*>(shader_ci.pCode), shader_ci.codeSize);
    } else {
        LOG("[%s] ERROR: Unable to open: %s", VK_EXT_PIPELINE_PROPERTIES_EXTENSION_NAME, shader_path.c_str());
    }

    data.computePipelineState.shaderFileNameCount = static_cast<uint32_t>(shader_filename.filenames.size());
    data.computePipelineState.pShaderFileNames = shader_filename.filenames.data();

    // Physical device features
    if (!vpjFilterDeviceFeatures(generator, create_info.ptr(), &data.computePipelineState.pPhysicalDeviceFeatures, &msg_)) {
        LOG("[%s] ERROR: Unable to filter physical device features: %s", VK_EXT_PIPELINE_PROPERTIES_EXTENSION_NAME, msg_);
    }

    data.enabledExtensionCount = device_create_info.enabledExtensionCount;
    data.ppEnabledExtensions = const_cast<const char**>(device_create_info.ppEnabledExtensionNames);

    if (!vpjGeneratePipelineJson(generator, &data, &result_json, &msg_)) {
        LOG("[%s] ERROR: Unable to generate pipeline JSON: %s", VK_EXT_PIPELINE_PROPERTIES_EXTENSION_NAME, msg_);
    }
    if (!vpjGetGeneratedPipelineUUID(generator, uuid.data(), &msg_)) {
        LOG("[%s] ERROR: Unable to obtain generated pipeline UUID: %s", VK_EXT_PIPELINE_PROPERTIES_EXTENSION_NAME, msg_);
    }

    auto pipeline_path = std::string(getBaseDirectoryPath()) + std::string(getProcessName()) + "_device_" +
                         std::to_string(device_id) + "_pipeline_" + std::to_string(id) + ".json";
    std::ofstream pipeline_file(pipeline_path);
    if (pipeline_file) {
        pipeline_file << result_json;
    } else {
        LOG("[%s] ERROR: Unable to open: %s", VK_EXT_PIPELINE_PROPERTIES_EXTENSION_NAME, pipeline_path.c_str());
    }
}

// VK_EXT_pipeline_properties

VKAPI_ATTR VkResult VKAPI_ATTR VKAPI_CALL GetPipelinePropertiesEXT(VkDevice device, const VkPipelineInfoEXT* pPipelineInfo,
                                                                   VkBaseOutStructure* pPipelineProperties) {
    if (pPipelineProperties->sType != VK_STRUCTURE_TYPE_PIPELINE_PROPERTIES_IDENTIFIER_EXT) {
        return VK_ERROR_VALIDATION_FAILED;
    }
    VkPipelinePropertiesIdentifierEXT* props = reinterpret_cast<VkPipelinePropertiesIdentifierEXT*>(pPipelineProperties);

    auto device_data = GetDeviceData(device);
    if (auto result = device_data->graphics_pipeline_map.find(pPipelineInfo->pipeline); result->first) {
        std::copy(result->second->uuid.begin(), result->second->uuid.end(), props->pipelineIdentifier);
    } else if (auto result2 = device_data->compute_pipeline_map.find(pPipelineInfo->pipeline); result2->first) {
        std::copy(result2->second->uuid.begin(), result2->second->uuid.end(), props->pipelineIdentifier);
    } else {
        LOG("[%s] ERROR: Failed to find pipeline in accelerating structures.", VK_EXT_PIPELINE_PROPERTIES_EXTENSION_NAME);
        return VK_ERROR_UNKNOWN;
    }
    return VK_SUCCESS;
}

// End layer logic

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL GetDeviceProcAddr(VkDevice device, const char* pName);
VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL GetInstanceProcAddr(VkInstance instance, const char* pName);

#define ADD_HOOK(fn) \
    { "vk" #fn, (PFN_vkVoidFunction)fn }
#define ADD_HOOK_ALIAS(fn, fn_alias) \
    { "vk" #fn, (PFN_vkVoidFunction)fn_alias }

static const std::unordered_map<std::string, PFN_vkVoidFunction> kInstanceFunctions = {ADD_HOOK(CreateInstance),
                                                                                       ADD_HOOK(DestroyInstance),
                                                                                       ADD_HOOK(CreateDevice),
                                                                                       ADD_HOOK(EnumeratePhysicalDevices),
                                                                                       ADD_HOOK(EnumerateDeviceExtensionProperties),
                                                                                       ADD_HOOK(GetPhysicalDeviceFeatures2),
                                                                                       ADD_HOOK(GetPhysicalDeviceFeatures2KHR)};

static const std::unordered_map<std::string, PFN_vkVoidFunction> kDeviceFunctions = {
    // Needs to point to itself as Android loaders calls vkGet*ProcAddr to itself. Without these hooks, when the app calls
    // vkGetDeviceProcAddr to get layer functions it will fail on Android
    ADD_HOOK(GetInstanceProcAddr),
    ADD_HOOK(GetDeviceProcAddr),
    ADD_HOOK(DestroyDevice),
    ADD_HOOK(CreateSemaphore),
    ADD_HOOK(DestroySemaphore),
    ADD_HOOK(CreateShaderModule),
    ADD_HOOK(DestroyShaderModule),
    ADD_HOOK(AllocateMemory),
    ADD_HOOK(CreatePipelineCache),
    ADD_HOOK(DestroyPipelineCache),
    ADD_HOOK(CreateGraphicsPipelines),
    ADD_HOOK(CreateComputePipelines),
    ADD_HOOK(DestroyPipeline),
    ADD_HOOK(CreateDescriptorSetLayout),
    ADD_HOOK(DestroyDescriptorSetLayout),
    ADD_HOOK(CreatePipelineLayout),
    ADD_HOOK(DestroyPipelineLayout),
    ADD_HOOK(CreateRenderPass),
    ADD_HOOK(CreateRenderPass2),
    ADD_HOOK(DestroyRenderPass),
    ADD_HOOK(CreateSampler),
    ADD_HOOK(DestroySampler),
    ADD_HOOK(CreateSamplerYcbcrConversion),
    ADD_HOOK(DestroySamplerYcbcrConversion),
    ADD_HOOK(CreateSwapchainKHR),
    ADD_HOOK(DestroySwapchainKHR),
    ADD_HOOK(CreateImage),
    ADD_HOOK(DestroyImage),
    ADD_HOOK(CreateImageView),
    ADD_HOOK(DestroyImageView),
    ADD_HOOK(AllocateCommandBuffers),
    ADD_HOOK(CreateFence),
    ADD_HOOK(DestroyFence),
    ADD_HOOK(CreateBuffer),
    ADD_HOOK(DestroyBuffer),
    ADD_HOOK(CreateBufferView),
    ADD_HOOK(DestroyBufferView),
    ADD_HOOK(CreateEvent),
    ADD_HOOK(DestroyEvent),
    ADD_HOOK(CreateQueryPool),
    ADD_HOOK(CreateDescriptorPool),
    ADD_HOOK(AllocateDescriptorSets),
    ADD_HOOK(FreeDescriptorSets),
    ADD_HOOK(CreateFramebuffer),
    ADD_HOOK(DestroyFramebuffer),
    ADD_HOOK(CreateCommandPool),
    ADD_HOOK(GetPipelinePropertiesEXT)};
#undef ADD_HOOK
#undef ADD_HOOK_ALIAS

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL GetInstanceProcAddr(VkInstance instance, const char* pName) {
    auto instance_result = kInstanceFunctions.find(pName);
    if (instance_result != kInstanceFunctions.end()) {
        return instance_result->second;
    }
    auto dev_result = kDeviceFunctions.find(pName);
    if (dev_result != kDeviceFunctions.end()) {
        return dev_result->second;
    }
    auto instance_data = GetInstanceData(instance);
    if (instance_data != nullptr && instance_data->vtable.GetInstanceProcAddr) {
        PFN_vkVoidFunction result = instance_data->vtable.GetInstanceProcAddr(instance, pName);
        return result;
    }
    return nullptr;
}

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL GetDeviceProcAddr(VkDevice device, const char* pName) {
    auto device_data = GetDeviceData(device);
    if (auto result = kDeviceFunctions.find(pName); result != kDeviceFunctions.end()) {
        return result->second;
    }
    if (device_data && device_data->vtable.GetDeviceProcAddr) {
        PFN_vkVoidFunction result = device_data->vtable.GetDeviceProcAddr(device, pName);
        return result;
    }
    return nullptr;
}

}  // namespace vk_json

#if defined(__GNUC__) && __GNUC__ >= 4
#define VEL_EXPORT __attribute__((visibility("default")))
#else
#define VEL_EXPORT
#endif

extern "C" VEL_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vkGetInstanceProcAddr(VkInstance instance, const char* pName) {
    return vk_json::GetInstanceProcAddr(instance, pName);
}

extern "C" VEL_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vkGetDeviceProcAddr(VkDevice device, const char* pName) {
    return vk_json::GetDeviceProcAddr(device, pName);
}

extern "C" VEL_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkNegotiateLoaderLayerInterfaceVersion(VkNegotiateLayerInterface* pVersionStruct) {
    ASSERT(pVersionStruct != nullptr);
    ASSERT(pVersionStruct->sType == LAYER_NEGOTIATE_INTERFACE_STRUCT);

    // Fill in the function pointers if our version is at least capable of having the structure contain them.
    if (pVersionStruct->loaderLayerInterfaceVersion >= 2) {
        pVersionStruct->loaderLayerInterfaceVersion = 2;
        pVersionStruct->pfnGetInstanceProcAddr = vk_json::GetInstanceProcAddr;
        pVersionStruct->pfnGetDeviceProcAddr = vk_json::GetDeviceProcAddr;
        pVersionStruct->pfnGetPhysicalDeviceProcAddr = nullptr;
    }

    return VK_SUCCESS;
}

// loader-layer interface v0 - Needed for Android loader using explicit layers
extern "C" VEL_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceExtensionProperties(const char* pLayerName,
                                                                                            uint32_t* pPropertyCount,
                                                                                            VkExtensionProperties* pProperties) {
    if (pLayerName && std::strncmp(pLayerName, vk_json::kGlobalLayer.layerName, VK_MAX_EXTENSION_NAME_SIZE) == 0) {
        return EnumerateProperties(vk_json::kInstanceExtensionPropertiesCount, vk_json::kInstanceExtensionProperties,
                                   pPropertyCount, pProperties);
    }
    return VK_ERROR_LAYER_NOT_PRESENT;
}

// loader-layer interface v0 - Needed for Android loader using explicit layers
extern "C" VEL_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceLayerProperties(uint32_t* pPropertyCount,
                                                                                        VkLayerProperties* pProperties) {
    if (pProperties == NULL) {
        *pPropertyCount = 1;
        return VK_SUCCESS;
    }
    if (*pPropertyCount < 1) {
        return VK_INCOMPLETE;
    }
    *pPropertyCount = 1;
    pProperties[0] = vk_json::kGlobalLayer;
    return VK_SUCCESS;
}

// loader-layer interface v0 - Needed for Android loader using explicit layers
extern "C" VEL_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateDeviceExtensionProperties(VkPhysicalDevice physicalDevice,
                                                                                          const char* pLayerName,
                                                                                          uint32_t* pPropertyCount,
                                                                                          VkExtensionProperties* pProperties) {
    // Want to have this call down chain if multiple layers are enabling extenions
    return vk_json::EnumerateDeviceExtensionProperties(physicalDevice, pLayerName, pPropertyCount, pProperties);
}

// Deprecated, but needed or else Android loader will not call into the exported vkEnumerateDeviceExtensionProperties
extern "C" VEL_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateDeviceLayerProperties(VkPhysicalDevice, uint32_t* pPropertyCount,
                                                                                      VkLayerProperties* pProperties) {
    return vkEnumerateInstanceLayerProperties(pPropertyCount, pProperties);
}
