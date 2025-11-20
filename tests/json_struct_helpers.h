/*
 * Copyright (c) 2025 The Khronos Group Inc.
 * Copyright (c) 2025 RasterGrid Kft.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <vulkan/utility/vk_struct_helper.hpp>
#include <vulkan/utility/vk_safe_struct.hpp>
#include <vulkan/vulkan_sc.h>

#include <tuple>        // std::apply
#include <utility>      // std::forward, std::pair
#include <string>       // std::string
#include <string_view>  // std::string_view
#include <vector>       // std::vector
#include <optional>     // std::optional
#include <cmath>        // std::isnan

const char* shader_stage_flag_to_string(VkShaderStageFlags flags) {
    switch (flags) {
        case VK_SHADER_STAGE_VERTEX_BIT:
            return "VK_SHADER_STAGE_VERTEX_BIT";
            break;
        case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
            return "VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT";
            break;
        case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
            return "VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT";
            break;
        case VK_SHADER_STAGE_GEOMETRY_BIT:
            return "VK_SHADER_STAGE_GEOMETRY_BIT";
            break;
        case VK_SHADER_STAGE_FRAGMENT_BIT:
            return "VK_SHADER_STAGE_FRAGMENT_BIT";
            break;
        case VK_SHADER_STAGE_COMPUTE_BIT:
            return "VK_SHADER_STAGE_COMPUTE_BIT";
            break;
        case VK_SHADER_STAGE_ALL_GRAPHICS:
            return "VK_SHADER_STAGE_ALL_GRAPHICS";
            break;
        case VK_SHADER_STAGE_ALL:
            return "VK_SHADER_STAGE_ALL";
            break;
        case VK_SHADER_STAGE_RAYGEN_BIT_KHR:
            return "VK_SHADER_STAGE_RAYGEN_BIT_KHR";
            break;
        case VK_SHADER_STAGE_ANY_HIT_BIT_KHR:
            return "VK_SHADER_STAGE_ANY_HIT_BIT_KHR";
            break;
        case VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR:
            return "VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR";
            break;
        case VK_SHADER_STAGE_MISS_BIT_KHR:
            return "VK_SHADER_STAGE_MISS_BIT_KHR";
            break;
        case VK_SHADER_STAGE_INTERSECTION_BIT_KHR:
            return "VK_SHADER_STAGE_INTERSECTION_BIT_KHR";
            break;
        case VK_SHADER_STAGE_CALLABLE_BIT_KHR:
            return "VK_SHADER_STAGE_CALLABLE_BIT_KHR";
            break;
        case VK_SHADER_STAGE_TASK_BIT_EXT:
            return "VK_SHADER_STAGE_TASK_BIT_EXT";
            break;
        case VK_SHADER_STAGE_MESH_BIT_EXT:
            return "VK_SHADER_STAGE_MESH_BIT_EXT";
            break;
        case VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI:
            return "VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI";
            break;
        case VK_SHADER_STAGE_CLUSTER_CULLING_BIT_HUAWEI:
            return "VK_SHADER_STAGE_CLUSTER_CULLING_BIT_HUAWEI";
            break;
        case VK_SHADER_STAGE_ALL_GRAPHICS | VK_SHADER_STAGE_COMPUTE_BIT:
            return "VK_SHADER_STAGE_ALL_GRAPHICS | VK_SHADER_STAGE_COMPUTE_BIT";
            break;
        default:
            return "Unexpected flag combination";
            break;
    }
};

std::pair<vku::safe_VkSamplerYcbcrConversionCreateInfo, std::string> getVkSamplerYcbcrConversionCreateInfo(uint32_t seed = 0) {
    VkSamplerYcbcrConversionCreateInfo ci = vku::InitStructHelper();
    std::string json{};

    switch (seed % 2) {
        case 0:
            ci.format = VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16;
            ci.ycbcrModel = VK_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_2020;
            ci.ycbcrRange = VK_SAMPLER_YCBCR_RANGE_ITU_FULL;
            ci.components.r = VK_COMPONENT_SWIZZLE_A;
            ci.components.g = VK_COMPONENT_SWIZZLE_B;
            ci.components.b = VK_COMPONENT_SWIZZLE_G;
            ci.components.a = VK_COMPONENT_SWIZZLE_R;
            ci.xChromaOffset = VK_CHROMA_LOCATION_COSITED_EVEN;
            ci.yChromaOffset = VK_CHROMA_LOCATION_MIDPOINT;
            ci.chromaFilter = VK_FILTER_CUBIC_EXT;
            ci.forceExplicitReconstruction = VK_TRUE;
            json = R"({
                "sType": "VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_CREATE_INFO",
                "pNext": "NULL",
                "format": "VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16",
                "ycbcrModel": "VK_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_2020",
                "ycbcrRange": "VK_SAMPLER_YCBCR_RANGE_ITU_FULL",
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
            })";
            break;
        case 1:
            ci.format = VK_FORMAT_G16B16G16R16_422_UNORM;
            ci.ycbcrModel = VK_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_IDENTITY;
            ci.ycbcrRange = VK_SAMPLER_YCBCR_RANGE_ITU_NARROW;
            ci.components.r = VK_COMPONENT_SWIZZLE_R;
            ci.components.g = VK_COMPONENT_SWIZZLE_G;
            ci.components.b = VK_COMPONENT_SWIZZLE_B;
            ci.components.a = VK_COMPONENT_SWIZZLE_A;
            ci.xChromaOffset = VK_CHROMA_LOCATION_MIDPOINT;
            ci.yChromaOffset = VK_CHROMA_LOCATION_COSITED_EVEN;
            ci.chromaFilter = VK_FILTER_LINEAR;
            ci.forceExplicitReconstruction = VK_FALSE;
            json = R"({
                "sType": "VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_CREATE_INFO",
                "pNext": "NULL",
                "format": "VK_FORMAT_G16B16G16R16_422_UNORM",
                "ycbcrModel": "VK_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_IDENTITY",
                "ycbcrRange": "VK_SAMPLER_YCBCR_RANGE_ITU_NARROW",
                "components": {
                    "r": "VK_COMPONENT_SWIZZLE_R",
                    "g": "VK_COMPONENT_SWIZZLE_G",
                    "b": "VK_COMPONENT_SWIZZLE_B",
                    "a": "VK_COMPONENT_SWIZZLE_A",
                },
                "xChromaOffset": "VK_CHROMA_LOCATION_MIDPOINT",
                "yChromaOffset": "VK_CHROMA_LOCATION_COSITED_EVEN",
                "chromaFilter": "VK_FILTER_LINEAR",
                "forceExplicitReconstruction": "VK_FALSE"
            })";
            break;
    }

    return {&ci, json};
}

struct SamplerParams {
    const std::optional<VkSamplerYcbcrConversion> ycbcr_conversion = std::nullopt;
    const std::optional<std::string> ycbcr_name = std::nullopt;
};

std::pair<vku::safe_VkSamplerCreateInfo, std::string> getVkSamplerCreateInfo(uint32_t seed = 0, SamplerParams params = {}) {
    using namespace std::string_literals;

    VkSamplerCreateInfo ci = vku::InitStructHelper();
    [[maybe_unused]] VkSamplerYcbcrConversionInfo ycbcrci = vku::InitStructHelper();
    [[maybe_unused]] VkSamplerReductionModeCreateInfo rm_ci = vku::InitStructHelper();
    std::string conversion_str = R"("NULL")";
    std::string conversion_json = R"("NULL")";
    std::string json{};

    if (params.ycbcr_conversion) {
        ycbcrci.conversion = params.ycbcr_conversion.value();
        if (params.ycbcr_name) {
            conversion_str = R"(")"s + params.ycbcr_name.value() + R"(")";
        } else {
            conversion_str = std::to_string(reinterpret_cast<std::size_t>(params.ycbcr_conversion.value()));
        }
        conversion_json = R"({
            "sType": "VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_INFO",
            "pNext": "NULL",
                "conversion": )"s +
                          conversion_str + R"(
        })";
    }

    ci.flags = 0;
    switch (seed % 2) {
        case 0:
            if (params.ycbcr_conversion) {
                ci.pNext = &ycbcrci;
            }
            ci.magFilter = VK_FILTER_LINEAR;
            ci.minFilter = VK_FILTER_NEAREST;
            ci.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
            ci.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            ci.addressModeV = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
            ci.addressModeW = VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
            ci.mipLodBias = 0.5f;
            ci.anisotropyEnable = VK_TRUE;
            ci.maxAnisotropy = 2.0f;
            ci.compareEnable = VK_TRUE;
            ci.compareOp = VK_COMPARE_OP_ALWAYS;
            ci.minLod = 1.0f;
            ci.maxLod = 8.0f;
            ci.borderColor = VK_BORDER_COLOR_INT_TRANSPARENT_BLACK;
            ci.unnormalizedCoordinates = VK_TRUE;
            json = R"({
                "sType": "VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO",
                "pNext": )" +
                   conversion_json + R"(,
                "flags": 0,
                "magFilter": "VK_FILTER_LINEAR",
                "minFilter": "VK_FILTER_NEAREST",
                "mipmapMode": "VK_SAMPLER_MIPMAP_MODE_LINEAR",
                "addressModeU": "VK_SAMPLER_ADDRESS_MODE_REPEAT",
                "addressModeV": "VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT",
                "addressModeW": "VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE",
                "mipLodBias": 0.5,
                "anisotropyEnable": "VK_TRUE",
                "maxAnisotropy": 2.0,
                "compareEnable": "VK_TRUE",
                "compareOp": "VK_COMPARE_OP_ALWAYS",
                "minLod": 1.0,
                "maxLod": 8.0,
                "borderColor": "VK_BORDER_COLOR_INT_TRANSPARENT_BLACK",
                "unnormalizedCoordinates": "VK_TRUE"
            })";
            break;
        case 1:
            ci.pNext = &rm_ci;
            if (params.ycbcr_conversion) {
                rm_ci.pNext = &ycbcrci;
            }
            rm_ci.reductionMode = VK_SAMPLER_REDUCTION_MODE_MAX;
            ci.magFilter = VK_FILTER_LINEAR;
            ci.minFilter = VK_FILTER_NEAREST;
            ci.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
            ci.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            ci.addressModeV = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
            ci.addressModeW = VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
            ci.mipLodBias = 0.5f;
            ci.anisotropyEnable = VK_TRUE;
            ci.maxAnisotropy = 2.0f;
            ci.compareEnable = VK_TRUE;
            ci.compareOp = VK_COMPARE_OP_ALWAYS;
            ci.minLod = 1.0f;
            ci.maxLod = 8.0f;
            ci.borderColor = VK_BORDER_COLOR_INT_TRANSPARENT_BLACK;
            ci.unnormalizedCoordinates = VK_TRUE;
            json = R"({
                "sType": "VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO",
                "pNext": {
                    "sType": "VK_STRUCTURE_TYPE_SAMPLER_REDUCTION_MODE_CREATE_INFO",
                    "pNext": )" +
                   conversion_json + R"(,
                    "reductionMode": "VK_SAMPLER_REDUCTION_MODE_MAX"
                },
                "flags": 0,
                "magFilter": "VK_FILTER_LINEAR",
                "minFilter": "VK_FILTER_NEAREST",
                "mipmapMode": "VK_SAMPLER_MIPMAP_MODE_LINEAR",
                "addressModeU": "VK_SAMPLER_ADDRESS_MODE_REPEAT",
                "addressModeV": "VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT",
                "addressModeW": "VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE",
                "mipLodBias": 0.5,
                "anisotropyEnable": "VK_TRUE",
                "maxAnisotropy": 2.0,
                "compareEnable": "VK_TRUE",
                "compareOp": "VK_COMPARE_OP_ALWAYS",
                "minLod": 1.0,
                "maxLod": 8.0,
                "borderColor": "VK_BORDER_COLOR_INT_TRANSPARENT_BLACK",
                "unnormalizedCoordinates": "VK_TRUE"
            })";
            break;
        case 2:
            if (params.ycbcr_conversion) {
                ci.pNext = &ycbcrci;
            }
            ci.magFilter = VK_FILTER_CUBIC_EXT;
            ci.minFilter = VK_FILTER_NEAREST;
            ci.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
            ci.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
            ci.addressModeV = VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
            ci.addressModeW = VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
            ci.mipLodBias = 0.5f;
            ci.anisotropyEnable = VK_FALSE;
            ci.maxAnisotropy = 2.0f;
            ci.compareEnable = VK_FALSE;
            ci.compareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
            ci.minLod = std::numeric_limits<float>::quiet_NaN();
            ci.maxLod = VK_LOD_CLAMP_NONE;  // NOTE: float constants can't be serialized as strings
            ci.borderColor = VK_BORDER_COLOR_FLOAT_CUSTOM_EXT;
            ci.unnormalizedCoordinates = VK_TRUE;
            json = R"({
            "sType": "VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO",
            "pNext": )" +
                   conversion_json + R"(,
            "flags": 0,
            "magFilter": "VK_FILTER_CUBIC_EXT",
            "minFilter": "VK_FILTER_NEAREST",
            "mipmapMode": "VK_SAMPLER_MIPMAP_MODE_LINEAR",
            "addressModeU": "VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER",
            "addressModeV": "VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE",
            "addressModeW": "VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE",
            "mipLodBias": 0.5,
            "anisotropyEnable": "VK_FALSE",
            "maxAnisotropy": 2.0,
            "compareEnable": "VK_FALSE",
            "compareOp": "VK_COMPARE_OP_LESS_OR_EQUAL",
            "minLod": "NaN",
            "maxLod": 1000.0,
            "borderColor": "VK_BORDER_COLOR_FLOAT_CUSTOM_EXT",
            "unnormalizedCoordinates": "VK_TRUE"
        })";
            break;
    }

    return {&ci, json};
}

struct DescriptorSetLayoutParams {
    const std::optional<VkSampler> immutableSampler = std::nullopt;
    const std::optional<std::string> immutableSamplerName = std::nullopt;
};

std::pair<vku::safe_VkDescriptorSetLayoutCreateInfo, std::string> getVkDescriptorSetLayoutCreateInfo(
    uint32_t seed = 0, std::initializer_list<DescriptorSetLayoutParams> params = {}) {
    using namespace std::string_literals;

    VkDescriptorSetLayoutCreateInfo ci = vku::InitStructHelper();
    std::string json{};
    [[maybe_unused]] VkDescriptorSetLayoutBindingFlagsCreateInfo dslbf_ci = vku::InitStructHelper();
    [[maybe_unused]] std::vector<VkDescriptorBindingFlags> dslbf_bindingFlags{};
    [[maybe_unused]] std::string dslbf_pBindingFlags_str{};

    std::vector<VkDescriptorSetLayoutBinding> bindings{};
    std::string pBindingFlags_str{};
    VkShaderStageFlags stageFlags;
    switch (seed / 2) {
        case 0:
            stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS | VK_SHADER_STAGE_COMPUTE_BIT;
            break;
        default:
            stageFlags = VK_SHADER_STAGE_ALL;
    };

    ci.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;
    if (std::empty(params)) {
        pBindingFlags_str = R"("NULL")";
        dslbf_pBindingFlags_str = R"("NULL")";
    } else {
        pBindingFlags_str = "[";
        for (auto it = params.begin(); it != params.end(); ++it) {
            auto i = std::distance(params.begin(), it);
            bindings.push_back(VkDescriptorSetLayoutBinding{});
            bindings.back().binding = static_cast<uint32_t>(i);
            bindings.back().descriptorType =
                it->immutableSampler ? VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER : VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            bindings.back().descriptorCount = 1;
            bindings.back().stageFlags = stageFlags;
            bindings.back().pImmutableSamplers = it->immutableSampler ? &it->immutableSampler.value() : nullptr;
            pBindingFlags_str +=
                R"({
                "binding": )"s +
                std::to_string(i) + R"(,
                "descriptorType": )"s +
                (it->immutableSampler ? R"("VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER")"
                                      : R"("VK_DESCRIPTOR_TYPE_STORAGE_BUFFER")") +
                R"(,
                "descriptorCount": 1,
                "stageFlags": ")"s +
                shader_stage_flag_to_string(stageFlags) + R"(",
                "pImmutableSamplers": )"s +
                (it->immutableSampler
                     ? ("["s +
                        (it->immutableSamplerName ? (R"(")"s + it->immutableSamplerName.value() + R"(")")
                                                  : std::to_string(reinterpret_cast<std::size_t>(it->immutableSampler.value()))) +
                        "]")
                     : R"("NULL")"s) +
                R"(
        })";
            if (std::next(it) != params.end()) {
                pBindingFlags_str += ",";
            }
        }
        pBindingFlags_str += "]";

        dslbf_pBindingFlags_str = "[";
        for (uint32_t i = 0; i < params.size(); ++i) {
            dslbf_pBindingFlags_str +=
                R"("VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT | VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT")";
            if (i != params.size() - 1) {
                dslbf_pBindingFlags_str += ",";
            }
        }
        dslbf_pBindingFlags_str += "]";
    }
    ci.bindingCount = static_cast<uint32_t>(bindings.size());
    ci.pBindings = bindings.data();
    switch (seed % 2) {
        case 0:
            json = R"({
                "sType" : "VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO",
                "pNext": "NULL",
                "flags": "VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT",
                "bindingCount": )"s +
                   std::to_string(ci.bindingCount) + R"(,
                "pBindings": )"s +
                   pBindingFlags_str + R"(
            })";
            break;
        case 1:
            ci.pNext = &dslbf_ci;
            dslbf_bindingFlags.resize(ci.bindingCount,
                                      VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT | VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT);
            dslbf_ci.bindingCount = ci.bindingCount;
            dslbf_ci.pBindingFlags = dslbf_bindingFlags.data();

            json = R"({
                "sType" : "VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO",
                "pNext": {
                     "sType" : "VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO",
                     "pNext": "NULL",
                     "bindingCount": )"s +
                   std::to_string(dslbf_ci.bindingCount) + R"(,
                     "pBindingFlags": )"s +
                   dslbf_pBindingFlags_str + R"(
            },
                "flags": "VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT",
                "bindingCount": )"s +
                   std::to_string(ci.bindingCount) + R"(,
                "pBindings": )"s +
                   pBindingFlags_str + R"(
            })";
            break;
    }

    return {&ci, json};
}

struct PipelineLayoutParams {
    const std::optional<VkDescriptorSetLayout> descriptorSetLayout = std::nullopt;
    const std::optional<std::string> descriptorSetLayoutName = std::nullopt;
};

std::pair<vku::safe_VkPipelineLayoutCreateInfo, std::string> getVkPipelineLayoutCreateInfo(
    uint32_t seed = 0, std::initializer_list<PipelineLayoutParams> params = {}) {
    using namespace std::string_literals;

    VkPipelineLayoutCreateInfo ci = vku::InitStructHelper();
    std::string json{};

    std::vector<VkPushConstantRange> pushConstantRanges{};
    std::vector<VkDescriptorSetLayout> setLayouts{};
    std::string pPushConstantRanges_str = R"("NULL")";
    std::string pSetLayouts_str = R"("NULL")";

    switch (seed % 2) {
        default:
            pushConstantRanges.resize(1);
            pushConstantRanges.back().stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS | VK_SHADER_STAGE_COMPUTE_BIT;
            pushConstantRanges.back().offset = 0;
            pushConstantRanges.back().size = 4;
            ci.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size());
            ci.pPushConstantRanges = pushConstantRanges.data();
            pPushConstantRanges_str =
                R"([{"stageFlags": "VK_SHADER_STAGE_ALL_GRAPHICS | VK_SHADER_STAGE_COMPUTE_BIT", "offset": 0, "size": 4}])";
            break;
    }

    if (!std::empty(params)) {
        pSetLayouts_str = "[";
        for (auto it = params.begin(); it != params.end(); ++it) {
            setLayouts.push_back(it->descriptorSetLayout.value());
            pSetLayouts_str +=
                it->descriptorSetLayout
                    ? (it->descriptorSetLayoutName ? (R"(")"s + it->descriptorSetLayoutName.value() + R"(")")
                                                   : std::to_string(reinterpret_cast<std::size_t>(it->descriptorSetLayout.value())))
                    : R"("NULL")"s;
            if (std::next(it) != params.end()) {
                pSetLayouts_str += ",";
            }
        }
        pSetLayouts_str += "]";
    }
    ci.setLayoutCount = static_cast<uint32_t>(setLayouts.size());
    ci.pSetLayouts = setLayouts.data();

    json = R"({
                "sType" : "VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO",
                "pNext": "NULL",
                "flags": 0,
                "setLayoutCount": )"s +
           std::to_string(setLayouts.size()) + R"(,
                "pSetLayouts": )" +
           pSetLayouts_str + R"(,
                "pushConstantRangeCount": )" +
           std::to_string(pushConstantRanges.size()) + R"(,
                "pPushConstantRanges": )" +
           pPushConstantRanges_str + R"(,
            })";

    return {&ci, json};
}

std::pair<vku::safe_VkPipelineShaderStageRequiredSubgroupSizeCreateInfo, std::string>
getVkPipelineShaderStageRequiredSubgroupSizeCreateInfo(uint32_t seed = 0) {
    using namespace std::string_literals;

    VkPipelineShaderStageRequiredSubgroupSizeCreateInfo ci = vku::InitStructHelper();
    std::string json{};

    switch (seed % 3) {
        case 0:
            ci.requiredSubgroupSize = 32;
            break;
        case 1:
            ci.requiredSubgroupSize = 64;
            break;
        case 2:
            ci.requiredSubgroupSize = 256;
            break;
    }

    json = R"({
        "sType": "VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_REQUIRED_SUBGROUP_SIZE_CREATE_INFO",
            "pNext": "NULL",
            "requiredSubgroupSize": )"s +
           std::to_string(ci.requiredSubgroupSize) + R"(
        })";

    return {&ci, json};
}

template <typename... InStructure>
std::pair<vku::safe_VkComputePipelineCreateInfo, std::string> getVkComputePipelineCreateInfo(uint32_t seed = 0) {
    using namespace std::string_literals;

    VkComputePipelineCreateInfo ci = vku::InitStructHelper();
    std::string json{};

    VkPipelineShaderStageCreateInfo pss_ci = vku::InitStructHelper();
    std::string pss_json = R"("NULL")";

    VkSpecializationMapEntry mapEntry{};
    int32_t data{};
    VkSpecializationInfo specializationInfo{};

    pss_ci.flags = VK_PIPELINE_SHADER_STAGE_CREATE_REQUIRE_FULL_SUBGROUPS_BIT;
    pss_ci.pName = "main";
    pss_ci.stage = VK_SHADER_STAGE_COMPUTE_BIT;

    switch (seed % 2) {
        case 0:
            pss_json = R"({
            "sType": "VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO",
            "pNext": "NULL",
            "flags": "VK_PIPELINE_SHADER_STAGE_CREATE_REQUIRE_FULL_SUBGROUPS_BIT",
            "module": "",
            "pName": "main",
            "pSpecializationInfo" : "NULL",
            "stage" : ")"s +
                       shader_stage_flag_to_string(pss_ci.stage) + R"(",
        })";
            break;
        default:
            mapEntry = VkSpecializationMapEntry{0, 0, 4};
            data = 12345;
            specializationInfo = VkSpecializationInfo{1, &mapEntry, 4, &data};
            pss_ci.pSpecializationInfo = &specializationInfo;
            pss_json = R"({
            "sType": "VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO",
            "pNext": "NULL",
            "flags": "VK_PIPELINE_SHADER_STAGE_CREATE_REQUIRE_FULL_SUBGROUPS_BIT",
            "module": "",
            "pName": "main",
            "pSpecializationInfo" : {
                "mapEntryCount" : 1,
                "pMapEntries" : [
                    {
                        "constantID" : 0,
                        "offset" : 0,
                        "size" : 4
                    },
                ],
                "dataSize" : 4,
                "pData" : "OTAAAA==",
            },
            "stage" : ")"s +
                       shader_stage_flag_to_string(pss_ci.stage) + R"(",
        })";
            break;
    }

    ci.stage = pss_ci;
    json = R"({
            "basePipelineHandle" : "",
            "basePipelineIndex" : 0,
            "flags" : 0,
            "layout" : "",
            "pNext" : "NULL",
            "sType" : "VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO",
            "stage" : )" +
           pss_json + R"(
        })";

    return {&ci, json};
}

std::pair<vku::safe_VkRenderPassCreateInfo, std::string> getVkRenderPassCreateInfo(uint32_t seed = 0) {
    using namespace std::string_literals;

    VkRenderPassCreateInfo ci = vku::InitStructHelper();
    std::string json{};

    std::vector<VkAttachmentDescription> attachments{};
    std::string pAttachments_str = R"("NULL")";
    std::vector<VkSubpassDependency> dependencies{};
    std::string pDependencies_str = R"("NULL")";
    std::vector<VkSubpassDescription> subpasses{};
    std::string pSubpasses_str = R"("NULL")";
    std::vector<VkAttachmentReference> inputAttachments{}, colorAttachments{}, resolveAttachments{}, depthAttachments{};

    [[maybe_unused]] VkRenderPassMultiviewCreateInfo rpmv_ci = vku::InitStructHelper();
    [[maybe_unused]] std::string rpmv_json = R"("NULL")";

    [[maybe_unused]] std::vector<uint32_t> viewMasks;
    [[maybe_unused]] std::vector<int32_t> viewOffsets;
    [[maybe_unused]] std::vector<uint32_t> correlationMasks;

    attachments.push_back({});
    attachments.back().flags = VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT;
    attachments.back().format = VK_FORMAT_R8G8_USCALED;
    attachments.back().samples = VK_SAMPLE_COUNT_8_BIT;
    attachments.back().loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments.back().storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments.back().stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    attachments.back().stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments.back().initialLayout = VK_IMAGE_LAYOUT_GENERAL;
    attachments.back().finalLayout = VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR;
    pAttachments_str = R"([{
            "flags": "VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT",
            "format": "VK_FORMAT_R8G8_USCALED",
            "samples": "VK_SAMPLE_COUNT_8_BIT",
            "loadOp": "VK_ATTACHMENT_LOAD_OP_DONT_CARE",
            "storeOp": "VK_ATTACHMENT_STORE_OP_STORE",
            "stencilLoadOp": "VK_ATTACHMENT_LOAD_OP_LOAD",
            "stencilStoreOp": "VK_ATTACHMENT_STORE_OP_DONT_CARE",
            "initialLayout": "VK_IMAGE_LAYOUT_GENERAL",
            "finalLayout": "VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR"
    }])";
    dependencies.push_back({});
    dependencies.back().srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies.back().dstSubpass = 2345;
    dependencies.back().srcStageMask = VK_PIPELINE_STAGE_NONE_KHR;
    dependencies.back().dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies.back().srcAccessMask = VK_ACCESS_NONE_KHR;
    dependencies.back().dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies.back().dependencyFlags = VK_DEPENDENCY_DEVICE_GROUP_BIT;
    pDependencies_str = R"([{
        "srcSubpass": 4294967295,
        "dstSubpass": 2345,
        "srcStageMask": 0,
        "dstStageMask": "VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT",
        "srcAccessMask": 0,
        "dstAccessMask": "VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT",
        "dependencyFlags": "VK_DEPENDENCY_DEVICE_GROUP_BIT"
    }])";
    switch (seed % 2) {
        case 0:
            break;
        case 1:
            inputAttachments.push_back({567, VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL});
            colorAttachments.push_back({VK_ATTACHMENT_UNUSED, VK_IMAGE_LAYOUT_UNDEFINED});
            resolveAttachments.push_back({VK_ATTACHMENT_UNUSED, VK_IMAGE_LAYOUT_UNDEFINED});
            depthAttachments.push_back({VK_ATTACHMENT_UNUSED, VK_IMAGE_LAYOUT_UNDEFINED});
            inputAttachments.push_back({});
            colorAttachments.push_back({567, VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL});
            resolveAttachments.push_back({VK_ATTACHMENT_UNUSED, VK_IMAGE_LAYOUT_UNDEFINED});
            depthAttachments.push_back({VK_ATTACHMENT_UNUSED, VK_IMAGE_LAYOUT_UNDEFINED});
            subpasses.push_back({});
            subpasses.back().inputAttachmentCount = 1;
            subpasses.back().pInputAttachments = &inputAttachments[0];
            subpasses.back().colorAttachmentCount = 1;
            subpasses.back().pColorAttachments = &colorAttachments[0];
            subpasses.back().pResolveAttachments = &resolveAttachments[0];
            subpasses.back().pDepthStencilAttachment = &depthAttachments[0];
            subpasses.back().preserveAttachmentCount = 0;
            subpasses.back().pPreserveAttachments = nullptr;
            subpasses.push_back({});
            subpasses.back().inputAttachmentCount = 0;
            subpasses.back().pInputAttachments = nullptr;
            subpasses.back().colorAttachmentCount = 1;
            subpasses.back().pColorAttachments = &colorAttachments[1];
            subpasses.back().pResolveAttachments = &resolveAttachments[1];
            subpasses.back().pDepthStencilAttachment = &depthAttachments[1];
            subpasses.back().preserveAttachmentCount = 0;
            subpasses.back().pPreserveAttachments = nullptr;
            pSubpasses_str = R"([{
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
            },{
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
            }])";

            viewMasks = {1};
            viewOffsets = {0, 1};
            correlationMasks = {8};
            ci.pNext = &rpmv_ci;
            rpmv_ci.subpassCount = static_cast<uint32_t>(viewMasks.size());
            rpmv_ci.pViewMasks = viewMasks.data();
            rpmv_ci.dependencyCount = static_cast<uint32_t>(viewOffsets.size());
            rpmv_ci.pViewOffsets = viewOffsets.data();
            rpmv_ci.correlationMaskCount = static_cast<uint32_t>(correlationMasks.size());
            rpmv_ci.pCorrelationMasks = correlationMasks.data();
            rpmv_json = R"({
                "sType" : "VK_STRUCTURE_TYPE_RENDER_PASS_MULTIVIEW_CREATE_INFO",
                "pNext": "NULL",
                "subpassCount": 1,
                "pViewMasks": [ 1 ],
                "dependencyCount": 2,
                "pViewOffsets": [ 0, 1 ],
                "correlationMaskCount": 1,
                "pCorrelationMasks": [ 8 ]
            })";
            break;
    }
    ci.attachmentCount = static_cast<uint32_t>(attachments.size());
    ci.pAttachments = attachments.data();
    ci.subpassCount = static_cast<uint32_t>(subpasses.size());
    ci.pSubpasses = subpasses.data();
    ci.dependencyCount = static_cast<uint32_t>(dependencies.size());
    ci.pDependencies = dependencies.data();

    json = R"({
        "sType" : "VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO",
        "pNext": )"s +
           rpmv_json + R"(,
        "flags": 0,
        "attachmentCount": )" +
           std::to_string(ci.attachmentCount) + R"(,
        "pAttachments": )"s +
           pAttachments_str + R"(,
        "subpassCount": )" +
           std::to_string(ci.subpassCount) + R"(,
        "pSubpasses": )"s +
           pSubpasses_str + R"(,
        "dependencyCount": )" +
           std::to_string(ci.dependencyCount) + R"(,
        "pDependencies": )"s +
           pDependencies_str + R"(
    })";

    return {&ci, json};
}

std::pair<vku::safe_VkRenderPassCreateInfo2, std::string> getVkRenderPassCreateInfo2(uint32_t seed = 0) {
    using namespace std::string_literals;

    VkRenderPassCreateInfo2 ci = vku::InitStructHelper();
    std::string json{};

    std::vector<VkAttachmentDescription2> attachments{};
    std::string pAttachments_str = R"("NULL")";
    std::vector<uint32_t> correlatedViewMasks{};
    std::string pCorrelatedViewMasks_str = R"("NULL")";
    std::vector<VkSubpassDependency2> dependencies{};
    std::string pDependencies_str = R"("NULL")";
    std::vector<VkSubpassDescription2> subpasses{};
    std::string pSubpasses_str = R"("NULL")";

    std::vector<VkAttachmentReference2> inputAttachments{}, colorAttachments{}, resolveAttachments{}, depthAttachments{};

    VkMemoryBarrier2KHR mb2 = vku::InitStructHelper();
    std::string mb2_json = R"("NULL")";

    VkFragmentShadingRateAttachmentInfoKHR fsra_ci = vku::InitStructHelper();
    std::string fsra_json{};

    attachments.push_back(vku::InitStructHelper());
    attachments.back().flags = VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT;
    attachments.back().format = VK_FORMAT_R8G8_USCALED;
    attachments.back().samples = VK_SAMPLE_COUNT_8_BIT;
    attachments.back().loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments.back().storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments.back().stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    attachments.back().stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments.back().initialLayout = VK_IMAGE_LAYOUT_GENERAL;
    attachments.back().finalLayout = VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR;
    pAttachments_str = R"([{
            "pNext" : "NULL",
            "sType" : "VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2",
            "flags": "VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT",
            "format": "VK_FORMAT_R8G8_USCALED",
            "samples": "VK_SAMPLE_COUNT_8_BIT",
            "loadOp": "VK_ATTACHMENT_LOAD_OP_DONT_CARE",
            "storeOp": "VK_ATTACHMENT_STORE_OP_STORE",
            "stencilLoadOp": "VK_ATTACHMENT_LOAD_OP_LOAD",
            "stencilStoreOp": "VK_ATTACHMENT_STORE_OP_DONT_CARE",
            "initialLayout": "VK_IMAGE_LAYOUT_GENERAL",
            "finalLayout": "VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR"
    }])";

    mb2.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    mb2.srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_READ_NONCOHERENT_BIT_EXT;
    mb2.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    mb2.dstAccessMask = VK_ACCESS_2_COMMAND_PREPROCESS_READ_BIT_NV;
    mb2_json = R"({
        "sType" : "VK_STRUCTURE_TYPE_MEMORY_BARRIER_2",
        "pNext": "NULL",
        "srcStageMask": "VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT",
        "srcAccessMask": "VK_ACCESS_2_COLOR_ATTACHMENT_READ_NONCOHERENT_BIT_EXT",
        "dstStageMask": "VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT",
        "dstAccessMask": "VK_ACCESS_2_COMMAND_PREPROCESS_READ_BIT_EXT"
    })";
    dependencies.push_back(vku::InitStructHelper());
    dependencies.back().pNext = &mb2;
    dependencies.back().srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies.back().dstSubpass = 2345;
    dependencies.back().srcStageMask = VK_PIPELINE_STAGE_NONE_KHR;
    dependencies.back().dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies.back().srcAccessMask = VK_ACCESS_NONE_KHR;
    dependencies.back().dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies.back().dependencyFlags = VK_DEPENDENCY_DEVICE_GROUP_BIT;
    pDependencies_str = R"([{
        "sType" : "VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2",
        "pNext": )"s + mb2_json +
                        R"(,
        "srcSubpass": 4294967295,
        "dstSubpass": 2345,
        "srcStageMask": 0,
        "dstStageMask": "VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT",
        "srcAccessMask": 0,
        "dstAccessMask": "VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT",
        "dependencyFlags": "VK_DEPENDENCY_DEVICE_GROUP_BIT",
        "viewOffset": 0
    }])";
    switch (seed % 2) {
        case 0:
            break;
        case 1:
            inputAttachments.push_back(vku::InitStructHelper());
            inputAttachments.back().attachment = VK_ATTACHMENT_UNUSED;
            inputAttachments.back().layout = VK_IMAGE_LAYOUT_UNDEFINED;
            inputAttachments.back().aspectMask = VK_IMAGE_ASPECT_NONE_KHR;
            colorAttachments.push_back(vku::InitStructHelper());
            colorAttachments.back().attachment = 0;
            colorAttachments.back().layout = VK_IMAGE_LAYOUT_GENERAL;
            colorAttachments.back().aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            resolveAttachments.push_back(vku::InitStructHelper());
            resolveAttachments.back().attachment = VK_ATTACHMENT_UNUSED;
            resolveAttachments.back().layout = VK_IMAGE_LAYOUT_UNDEFINED;
            resolveAttachments.back().aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            depthAttachments.push_back(vku::InitStructHelper());
            depthAttachments.back().attachment = VK_ATTACHMENT_UNUSED;
            depthAttachments.back().layout = VK_IMAGE_LAYOUT_UNDEFINED;
            depthAttachments.back().aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            subpasses.push_back(vku::InitStructHelper());
            subpasses.back().pNext = &fsra_ci;
            subpasses.back().viewMask = (uint32_t)~0;
            subpasses.back().inputAttachmentCount = static_cast<uint32_t>(inputAttachments.size());
            subpasses.back().pInputAttachments = inputAttachments.data();
            subpasses.back().colorAttachmentCount = static_cast<uint32_t>(colorAttachments.size());
            subpasses.back().pColorAttachments = colorAttachments.data();
            subpasses.back().pResolveAttachments = resolveAttachments.data();
            subpasses.back().pDepthStencilAttachment = depthAttachments.data();
            subpasses.back().preserveAttachmentCount = 0;
            subpasses.back().pPreserveAttachments = nullptr;
            fsra_ci.pFragmentShadingRateAttachment = &colorAttachments.back();
            fsra_ci.shadingRateAttachmentTexelSize.width = 4;
            fsra_ci.shadingRateAttachmentTexelSize.height = 4;
            fsra_json = R"({
                "sType" : "VK_STRUCTURE_TYPE_FRAGMENT_SHADING_RATE_ATTACHMENT_INFO_KHR",
                "pNext": "NULL",
                "pFragmentShadingRateAttachment": {
                    "sType": "VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2",
                    "pNext": "NULL",
                    "attachment": 0,
                    "layout": "VK_IMAGE_LAYOUT_GENERAL",
                    "aspectMask": "VK_IMAGE_ASPECT_COLOR_BIT"
                },
                "shadingRateAttachmentTexelSize": {
                    "width": 4,
                    "height": 4
                }
            })";
            pSubpasses_str = R"([{
                "sType" : "VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2",
                "pNext": )"s +
                             fsra_json + R"(,
                "flags": 0,
                "pipelineBindPoint": "VK_PIPELINE_BIND_POINT_GRAPHICS",
                "viewMask": 4294967295,
                "inputAttachmentCount": 1,
                "pInputAttachments": [
                    {
                        "sType": "VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2",
                        "pNext": "NULL",
                        "attachment": 4294967295,
                        "layout": "VK_IMAGE_LAYOUT_UNDEFINED",
                        "aspectMask": 0
                    }
                ],
                "colorAttachmentCount": 1,
                "pColorAttachments": [
                    {
                        "sType": "VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2",
                        "pNext": "NULL",
                        "attachment": 0,
                        "layout": "VK_IMAGE_LAYOUT_GENERAL",
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
    }])";
            break;
    }

    ci.attachmentCount = static_cast<uint32_t>(attachments.size());
    ci.pAttachments = attachments.data();
    ci.subpassCount = static_cast<uint32_t>(subpasses.size());
    ci.pSubpasses = subpasses.data();
    ci.dependencyCount = static_cast<uint32_t>(dependencies.size());
    ci.pDependencies = dependencies.data();
    json = R"({
        "sType" : "VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2",
        "pNext": "NULL",
        "flags": 0,
        "attachmentCount": )" +
           std::to_string(ci.attachmentCount) + R"(,
        "pAttachments": )"s +
           pAttachments_str + R"(,
        "correlatedViewMaskCount": )" +
           std::to_string(ci.correlatedViewMaskCount) + R"(,
        "pCorrelatedViewMasks": )"s +
           pCorrelatedViewMasks_str + R"(,
        "subpassCount": )" +
           std::to_string(ci.subpassCount) + R"(,
        "pSubpasses": )"s +
           pSubpasses_str + R"(,
        "dependencyCount": )" +
           std::to_string(ci.dependencyCount) + R"(,
        "pDependencies": )"s +
           pDependencies_str + R"(
    })";

    return {&ci, json};
}

std::pair<vku::safe_VkShaderModuleCreateInfo, std::string> getVkShaderModuleCreateInfo(uint32_t seed = 0) {
    VkShaderModuleCreateInfo ci = vku::InitStructHelper();
    std::string json{};

    uint32_t code = (25 << 0) + (123 << 8) + (42 << 16) + (8 << 24);

    switch (seed % 2) {
        case 0:
            ci.flags = 0;
            ci.codeSize = 4;
            ci.pCode = &code;
            json = R"({
            "sType" : "VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO",
            "pNext": "NULL",
            "flags": 0,
            "codeSize": 4,
            "pCode": "GXsqCA=="
        })";
            break;
        case 1:
            ci.flags = 0;
            ci.codeSize = 4;
            ci.pCode = &code;
            json = R"({
            "sType" : "VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO",
            "pNext": "NULL",
            "flags": 0,
            "codeSize": 4,
            "pCode": [25, 123, 42, 8]
        })";
            break;
    }

    return {&ci, json};
}

std::pair<vku::safe_VkDeviceObjectReservationCreateInfo, std::string> getVkDeviceObjectReservationCreateInfo(uint32_t seed = 0) {
    VkDeviceObjectReservationCreateInfo ci = vku::InitStructHelper();
    std::string json{};

    std::vector<uint8_t> initialData{};
    std::vector<VkPipelineCacheCreateInfo> pipelineCacheCreateInfos{};
    std::vector<VkPipelinePoolSize> pipelinePoolSizes{};

    switch (seed % 2) {
        default:
            initialData = {0b01101001, 0b10110111, 0b00011101};
            pipelineCacheCreateInfos.push_back(vku::InitStructHelper());
            pipelineCacheCreateInfos.back().flags =
                VK_PIPELINE_CACHE_CREATE_READ_ONLY_BIT | VK_PIPELINE_CACHE_CREATE_USE_APPLICATION_STORAGE_BIT;
            pipelineCacheCreateInfos.back().initialDataSize = static_cast<uint32_t>(initialData.size());
            pipelineCacheCreateInfos.back().pInitialData = initialData.data();

            ci.pipelineCacheCreateInfoCount = static_cast<uint32_t>(pipelineCacheCreateInfos.size());
            ci.pPipelineCacheCreateInfos = pipelineCacheCreateInfos.data();

            pipelinePoolSizes.push_back(vku::InitStructHelper());
            pipelinePoolSizes.back().poolEntryCount = 1;
            pipelinePoolSizes.back().poolEntrySize = 1048576;
            ci.pipelinePoolSizeCount = static_cast<uint32_t>(pipelinePoolSizes.size());
            ci.pPipelinePoolSizes = pipelinePoolSizes.data();

            ci.semaphoreRequestCount = 0;
            ci.commandBufferRequestCount = 1;
            ci.fenceRequestCount = 1;
            ci.deviceMemoryRequestCount = 2;
            ci.bufferRequestCount = 2;
            ci.imageRequestCount = 0;
            ci.eventRequestCount = 0;
            ci.queryPoolRequestCount = 0;
            ci.bufferViewRequestCount = 0;
            ci.imageViewRequestCount = 0;
            ci.layeredImageViewRequestCount = 0;
            ci.pipelineCacheRequestCount = 1;
            ci.pipelineLayoutRequestCount = 1;
            ci.renderPassRequestCount = 1;
            ci.graphicsPipelineRequestCount = 0;
            ci.computePipelineRequestCount = 1;
            ci.descriptorSetLayoutRequestCount = 1;
            ci.samplerRequestCount = 0;
            ci.descriptorPoolRequestCount = 1;
            ci.descriptorSetRequestCount = 1;
            ci.framebufferRequestCount = 0;
            ci.commandPoolRequestCount = 2;
            ci.samplerYcbcrConversionRequestCount = 0;
            ci.surfaceRequestCount = 0;
            ci.swapchainRequestCount = 1;
            ci.displayModeRequestCount = 0;
            ci.subpassDescriptionRequestCount = 1;
            ci.attachmentDescriptionRequestCount = 2;
            ci.descriptorSetLayoutBindingRequestCount = 2;
            ci.descriptorSetLayoutBindingLimit = 2;
            ci.maxImageViewMipLevels = 1;
            ci.maxImageViewArrayLayers = 1;
            ci.maxLayeredImageViewMipLevels = 0;
            ci.maxOcclusionQueriesPerPool = 0;
            ci.maxPipelineStatisticsQueriesPerPool = 0;
            ci.maxTimestampQueriesPerPool = 0;
            ci.maxImmutableSamplersPerDescriptorSetLayout = 0;
            json = R"(json = {R"({
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
    })";
            break;
    }

    return {&ci, json};
}

std::pair<vku::safe_VkPipelineOfflineCreateInfo, std::string> getVkPipelineOfflineCreateInfo(uint32_t seed = 0) {
    VkPipelineOfflineCreateInfo ci = vku::InitStructHelper();
    std::string json{};

    switch (seed % 2) {
        default:
            std::array<uint8_t, VK_UUID_SIZE> pipelineIdentifier{85, 43, 255, 24, 155, 64, 62, 24, 0, 0, 0, 0, 0, 0, 0, 0};
            std::copy(pipelineIdentifier.begin(), pipelineIdentifier.end(), ci.pipelineIdentifier);
            ci.matchControl = VK_PIPELINE_MATCH_CONTROL_APPLICATION_UUID_EXACT_MATCH;
            ci.poolEntrySize = 1048576;
            json = R"({
            "sType" : "VK_STRUCTURE_TYPE_PIPELINE_OFFLINE_CREATE_INFO",
            "pNext": "NULL",
            "pipelineIdentifier": [85, 43, 255, 24, 155, 64, 62, 24, 0, 0, 0, 0, 0, 0, 0, 0],
            "matchControl": "VK_PIPELINE_MATCH_CONTROL_APPLICATION_UUID_EXACT_MATCH",
            "poolEntrySize": 1048576
        })";
            break;
    }

    return {&ci, json};
}

template <typename... InStructure>
std::pair<vku::safe_VkPipelineVertexInputStateCreateInfo, std::string> getVkPipelineVertexInputStateCreateInfo(
    uint32_t seed = 0, std::tuple<std::pair<InStructure, std::string>...> pNext = std::tuple<>{}) {
    using namespace std::string_literals;

    VkPipelineVertexInputStateCreateInfo ci = vku::InitStructHelper();
    std::string json{};

    switch (seed % 2) {
        default:
            VkVertexInputBindingDescription vertexBindingDescriptions[1] = {{0, 32, VK_VERTEX_INPUT_RATE_VERTEX}};
            VkVertexInputAttributeDescription vertexAttributeDescriptions[2] = {{0, 0, VK_FORMAT_R32G32B32A32_SFLOAT, 0},
                                                                                {1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, 16}};
            ci.vertexBindingDescriptionCount = 1;
            ci.pVertexBindingDescriptions = vertexBindingDescriptions;
            ci.vertexAttributeDescriptionCount = 2;
            ci.pVertexAttributeDescriptions = vertexAttributeDescriptions;
            json = R"({
            "sType" : "VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO",
            "pNext":"NULL",
            "flags" : 0,
            "vertexBindingDescriptionCount" : 1,
            "pVertexBindingDescriptions": [
                {
                    "binding" : 0,
                    "stride" : 32,
                    "inputRate" : "VK_VERTEX_INPUT_RATE_VERTEX"
                }
            ],
            "vertexAttributeDescriptionCount" : 2,
            "pVertexAttributeDescriptions": [
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
        })";
            break;
    }

    ChainPNext(ci, json, pNext);

    return {&ci, json};
}

template <typename... InStructure>
std::pair<vku::safe_VkPipelineInputAssemblyStateCreateInfo, std::string> getVkPipelineInputAssemblyStateCreateInfo(
    uint32_t seed = 0, std::tuple<std::pair<InStructure, std::string>...> pNext = std::tuple<>{}) {
    using namespace std::string_literals;

    VkPipelineInputAssemblyStateCreateInfo ci = vku::InitStructHelper();
    std::string json{};

    switch (seed % 2) {
        default:
            ci.topology = VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
            ci.primitiveRestartEnable = VK_FALSE;
            json = R"({
            "sType": "VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO",
            "pNext": "NULL",
            "flags": 0,
            "topology": "VK_PRIMITIVE_TOPOLOGY_PATCH_LIST",
            "primitiveRestartEnable": "VK_FALSE"
        })";
            break;
    }

    ChainPNext(ci, json, pNext);

    return {&ci, json};
}

template <typename... InStructure>
std::pair<vku::safe_VkPipelineTessellationStateCreateInfo, std::string> getVkPipelineTessellationStateCreateInfo(
    uint32_t seed = 0, std::tuple<std::pair<InStructure, std::string>...> pNext = std::tuple<>{}) {
    using namespace std::string_literals;

    VkPipelineTessellationStateCreateInfo ci = vku::InitStructHelper();
    std::string json{};

    switch (seed % 2) {
        default:
            ci.patchControlPoints = 4;
            json = R"({
            "sType": "VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO",
            "pNext": "NULL",
            "flags": 0,
            "patchControlPoints": 4
        })";
            break;
    }

    ChainPNext(ci, json, pNext);

    return {&ci, json};
}

template <typename... InStructure>
std::pair<vku::safe_VkPipelineViewportStateCreateInfo, std::string> getVkPipelineViewportStateCreateInfo(
    uint32_t seed = 0, std::tuple<std::pair<InStructure, std::string>...> pNext = std::tuple<>{}) {
    using namespace std::string_literals;

    VkPipelineViewportStateCreateInfo ci = vku::InitStructHelper();
    std::string json{};

    switch (seed % 2) {
        default:
            VkViewport viewports[1] = {{0.f, 0.f, 51.f, 51.f, 0.f, 1.f}};
            VkRect2D scissors[1] = {{VkOffset2D{0, 0}, VkExtent2D{51, 51}}};
            ci.viewportCount = 1;
            ci.pViewports = viewports;
            ci.scissorCount = 1;
            ci.pScissors = scissors;
            json = R"({
            "sType": "VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO",
            "pNext": "NULL",
            "flags": 0,
            "viewportCount": 1,
            "pViewports":
            [
                {
                    "x": 0.0,
                    "y": 0.0,
                    "width": 51.0,
                    "height": 51.0,
                    "minDepth": 0.0,
                    "maxDepth": 1.0
                }
            ],
            "scissorCount": 1,
            "pScissors":
            [
                {
                    "offset":
                    {
                        "x": 0,
                        "y": 0
                    },
                    "extent":
                    {
                        "width": 51,
                        "height": 51
                    }
                }
            ]
        })";
            break;
    }

    ChainPNext(ci, json, pNext);

    return {vku::safe_VkPipelineViewportStateCreateInfo{&ci, false, false}, json};
}

template <typename... InStructure>
std::pair<vku::safe_VkPipelineRasterizationStateCreateInfo, std::string> getVkPipelineRasterizationStateCreateInfo(
    uint32_t seed = 0, std::tuple<std::pair<InStructure, std::string>...> pNext = std::tuple<>{}) {
    using namespace std::string_literals;

    VkPipelineRasterizationStateCreateInfo ci = vku::InitStructHelper();
    std::string json{};

    switch (seed % 2) {
        default:
            ci.depthClampEnable = VK_FALSE;
            ci.rasterizerDiscardEnable = VK_FALSE;
            ci.polygonMode = VK_POLYGON_MODE_FILL;
            ci.cullMode = 0;
            ci.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
            ci.depthBiasEnable = VK_FALSE;
            ci.depthBiasConstantFactor = 0.f;
            ci.depthBiasClamp = 0.f;
            ci.depthBiasSlopeFactor = 0.f;
            ci.lineWidth = 1.f;
            json = R"({
            "sType": "VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO",
            "pNext": "NULL",
            "flags": 0,
            "depthClampEnable": "VK_FALSE",
            "rasterizerDiscardEnable": "VK_FALSE",
            "polygonMode": "VK_POLYGON_MODE_FILL",
            "cullMode": 0,
            "frontFace": "VK_FRONT_FACE_COUNTER_CLOCKWISE",
            "depthBiasEnable": "VK_FALSE",
            "depthBiasConstantFactor": 0.0,
            "depthBiasClamp": 0.0,
            "depthBiasSlopeFactor": 0.0,
            "lineWidth": 1.0
        })";
            break;
    }

    ChainPNext(ci, json, pNext);

    return {&ci, json};
}

template <typename... InStructure>
std::pair<vku::safe_VkPipelineMultisampleStateCreateInfo, std::string> getVkPipelineMultisampleStateCreateInfo(
    uint32_t seed = 0, std::tuple<std::pair<InStructure, std::string>...> pNext = std::tuple<>{}) {
    using namespace std::string_literals;

    VkPipelineMultisampleStateCreateInfo ci = vku::InitStructHelper();
    std::string json{};

    switch (seed % 2) {
        default:
            ci.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
            ci.sampleShadingEnable = VK_FALSE;
            ci.minSampleShading = 1.f;
            ci.pSampleMask = nullptr;
            ci.alphaToCoverageEnable = VK_FALSE;
            ci.alphaToOneEnable = VK_FALSE;
            json = R"({
            "sType": "VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO",
            "pNext": "NULL",
            "flags": 0,
            "rasterizationSamples": "VK_SAMPLE_COUNT_1_BIT",
            "sampleShadingEnable": "VK_FALSE",
            "minSampleShading": 1.0,
            "pSampleMask": "NULL",
            "alphaToCoverageEnable": "VK_FALSE",
            "alphaToOneEnable": "VK_FALSE"
        })";
            break;
    }

    ChainPNext(ci, json, pNext);

    return {&ci, json};
}

template <typename... InStructure>
std::pair<vku::safe_VkPipelineDepthStencilStateCreateInfo, std::string> getVkPipelineDepthStencilStateCreateInfo(
    uint32_t seed = 0, std::tuple<std::pair<InStructure, std::string>...> pNext = std::tuple<>{}) {
    using namespace std::string_literals;

    VkPipelineDepthStencilStateCreateInfo ci = vku::InitStructHelper();
    std::string json{};

    switch (seed % 2) {
        default:
            ci.depthTestEnable = VK_TRUE;
            ci.depthWriteEnable = VK_TRUE;
            ci.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
            ci.depthBoundsTestEnable = VK_FALSE;
            ci.stencilTestEnable = VK_FALSE;
            ci.front.failOp = VK_STENCIL_OP_INVERT;
            ci.front.passOp = VK_STENCIL_OP_KEEP;
            ci.front.depthFailOp = VK_STENCIL_OP_ZERO;
            ci.front.compareOp = VK_COMPARE_OP_NEVER;
            ci.front.compareMask = 0;
            ci.front.writeMask = 0;
            ci.front.reference = 0;
            ci.back.failOp = VK_STENCIL_OP_INVERT;
            ci.back.passOp = VK_STENCIL_OP_KEEP;
            ci.back.depthFailOp = VK_STENCIL_OP_ZERO;
            ci.back.compareOp = VK_COMPARE_OP_NEVER;
            ci.back.compareMask = 0;
            ci.back.writeMask = 0;
            ci.back.reference = 0;
            ci.minDepthBounds = 0.f;
            ci.maxDepthBounds = 1.f;
            json = R"({
            "sType": "VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO",
            "pNext": "NULL",
            "flags": 0,
            "depthTestEnable": "VK_TRUE",
            "depthWriteEnable": "VK_TRUE",
            "depthCompareOp": "VK_COMPARE_OP_LESS_OR_EQUAL",
            "depthBoundsTestEnable": "VK_FALSE",
            "stencilTestEnable": "VK_FALSE",
            "front":
            {
                "failOp": "VK_STENCIL_OP_INVERT",
                "passOp": "VK_STENCIL_OP_KEEP",
                "depthFailOp": "VK_STENCIL_OP_ZERO",
                "compareOp": "VK_COMPARE_OP_NEVER",
                "compareMask": 0,
                "writeMask": 0,
                "reference": 0
            },
            "back":
            {
                "failOp": "VK_STENCIL_OP_INVERT",
                "passOp": "VK_STENCIL_OP_KEEP",
                "depthFailOp": "VK_STENCIL_OP_ZERO",
                "compareOp": "VK_COMPARE_OP_NEVER",
                "compareMask": 0,
                "writeMask": 0,
                "reference": 0
            },
            "minDepthBounds": 0.0,
            "maxDepthBounds": 1.0
        })";
            break;
    }

    ChainPNext(ci, json, pNext);

    return {&ci, json};
}

template <typename... InStructure>
std::pair<vku::safe_VkPipelineColorBlendStateCreateInfo, std::string> getVkPipelineColorBlendStateCreateInfo(
    uint32_t seed = 0, std::tuple<std::pair<InStructure, std::string>...> pNext = std::tuple<>{}) {
    using namespace std::string_literals;

    VkPipelineColorBlendStateCreateInfo ci = vku::InitStructHelper();
    std::string json{};

    switch (seed % 2) {
        default:
            VkPipelineColorBlendAttachmentState attachments[1] = {
                {VK_FALSE, VK_BLEND_FACTOR_ZERO, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD, VK_BLEND_FACTOR_ZERO, VK_BLEND_FACTOR_ZERO,
                 VK_BLEND_OP_ADD,
                 VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT}};
            ci.logicOpEnable = VK_FALSE;
            ci.logicOp = VK_LOGIC_OP_CLEAR;
            ci.attachmentCount = 1;
            ci.pAttachments = attachments;
            ci.blendConstants[0] = 0.f;
            ci.blendConstants[1] = 0.f;
            ci.blendConstants[2] = 0.f;
            ci.blendConstants[3] = 0.f;
            json = R"({
            "sType": "VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO",
            "pNext": "NULL",
            "flags": 0,
            "logicOpEnable": "VK_FALSE",
            "logicOp": "VK_LOGIC_OP_CLEAR",
            "attachmentCount": 1,
            "pAttachments":
            [
                {
                    "blendEnable": "VK_FALSE",
                    "srcColorBlendFactor": "VK_BLEND_FACTOR_ZERO",
                    "dstColorBlendFactor": "VK_BLEND_FACTOR_ZERO",
                    "colorBlendOp": "VK_BLEND_OP_ADD",
                    "srcAlphaBlendFactor": "VK_BLEND_FACTOR_ZERO",
                    "dstAlphaBlendFactor": "VK_BLEND_FACTOR_ZERO",
                    "alphaBlendOp": "VK_BLEND_OP_ADD",
                    "colorWriteMask": "VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT"
                }
            ],
            "blendConstants": [0.0, 0.0, 0.0, 0.0]
        })";
            break;
    }

    ChainPNext(ci, json, pNext);

    return {&ci, json};
}

template <typename... InStructure>
std::pair<vku::safe_VkPipelineDynamicStateCreateInfo, std::string> getVkPipelineDynamicStateCreateInfo(
    uint32_t seed = 0, std::tuple<std::pair<InStructure, std::string>...> pNext = std::tuple<>{}) {
    using namespace std::string_literals;

    VkPipelineDynamicStateCreateInfo ci = vku::InitStructHelper();
    std::string json{};

    switch (seed % 2) {
        default:
            VkDynamicState dynamicStates[1] = {VK_DYNAMIC_STATE_LINE_WIDTH};
            ci.dynamicStateCount = 1;
            ci.pDynamicStates = dynamicStates;
            json = R"({
            "sType": "VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO",
            "pNext": "NULL",
            "flags": 0,
            "dynamicStateCount": 1,
            "pDynamicStates": ["VK_DYNAMIC_STATE_LINE_WIDTH"]
        })";
            break;
    }

    ChainPNext(ci, json, pNext);

    return {&ci, json};
}

std::pair<vku::safe_VkPipelineDiscardRectangleStateCreateInfoEXT, std::string> getVkPipelineDiscardRectangleStateCreateInfoEXT(
    uint32_t seed = 0) {
    using namespace std::string_literals;

    VkPipelineDiscardRectangleStateCreateInfoEXT ci = vku::InitStructHelper();
    std::string json{};

    ci.sType = VK_STRUCTURE_TYPE_PIPELINE_DISCARD_RECTANGLE_STATE_CREATE_INFO_EXT;
    ci.pNext = nullptr;
    ci.flags = 0;
    VkRect2D discardRectangles{};
    switch (seed % 2) {
        default:
            discardRectangles = {VkOffset2D{0, 0}, VkExtent2D{51, 51}};
            ci.discardRectangleMode = VK_DISCARD_RECTANGLE_MODE_EXCLUSIVE_EXT;
            ci.discardRectangleCount = 1;
            ci.pDiscardRectangles = &discardRectangles;
            json = R"({
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
        })";
            break;
    }

    return {&ci, json};
}

std::pair<vku::safe_VkGraphicsPipelineCreateInfo, std::string> getVkGraphicsPipelineCreateInfo(uint32_t seed = 0) {
    using namespace std::string_literals;

    VkGraphicsPipelineCreateInfo ci = vku::InitStructHelper();
    std::string json{};

    VkPipelineDiscardRectangleStateCreateInfoEXT pdrs_ci = vku::InitStructHelper();
    std::string pdrs_json = R"("NULL")";

    std::vector<VkPipelineShaderStageCreateInfo> pStages{};
    std::string pStages_str{};
    VkPipelineShaderStageRequiredSubgroupSizeCreateInfo pssrss_ci = vku::InitStructHelper();

    VkPipelineVertexInputStateCreateInfo pvis_ci = vku::InitStructHelper();
    std::string pvis_json = R"("NULL")";
    VkPipelineInputAssemblyStateCreateInfo pias_ci = vku::InitStructHelper();
    std::string pias_json = R"("NULL")";
    VkPipelineTessellationStateCreateInfo pts_ci = vku::InitStructHelper();
    std::string pts_json = R"("NULL")";
    VkPipelineViewportStateCreateInfo pvs_ci = vku::InitStructHelper();
    std::string pvs_json = R"("NULL")";
    VkPipelineRasterizationStateCreateInfo prs_ci = vku::InitStructHelper();
    std::string prs_json = R"("NULL")";
    VkPipelineMultisampleStateCreateInfo pms_ci = vku::InitStructHelper();
    std::string pms_json = R"("NULL")";
    VkPipelineDepthStencilStateCreateInfo pdss_ci = vku::InitStructHelper();
    std::string pdss_json = R"("NULL")";
    VkPipelineColorBlendStateCreateInfo pcbs_ci = vku::InitStructHelper();
    std::string pcbs_json = R"("NULL")";
    VkPipelineDynamicStateCreateInfo pds_ci = vku::InitStructHelper();
    std::string pds_json = R"("NULL")";

    VkVertexInputBindingDescription vertexBindingDescriptions[1] = {{0, 32, VK_VERTEX_INPUT_RATE_VERTEX}};
    VkVertexInputAttributeDescription vertexAttributeDescriptions[2] = {{0, 0, VK_FORMAT_R32G32B32A32_SFLOAT, 0},
                                                                        {1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, 16}};

    VkViewport viewports[1] = {{0.f, 0.f, 51.f, 51.f, 0.f, 1.f}};
    VkRect2D scissors[1] = {{VkOffset2D{0, 0}, VkExtent2D{51, 51}}};

    VkPipelineColorBlendAttachmentState attachments[1] = {
        {VK_FALSE, VK_BLEND_FACTOR_ZERO, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD, VK_BLEND_FACTOR_ZERO, VK_BLEND_FACTOR_ZERO,
         VK_BLEND_OP_ADD,
         VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT}};

    VkDynamicState dynamicStates[1] = {VK_DYNAMIC_STATE_LINE_WIDTH};

    VkRect2D discardRectangles = {VkOffset2D{0, 0}, VkExtent2D{51, 51}};

    switch (seed % 2) {
        default:
            ci.pNext = &pdrs_ci;
            pssrss_ci.requiredSubgroupSize = 64;
            pStages.push_back(vku::InitStructHelper());
            pStages.back().pName = "main";
            pStages.back().stage = VK_SHADER_STAGE_VERTEX_BIT;
            pStages.push_back(vku::InitStructHelper());
            pStages.back().pNext = &pssrss_ci;
            pStages.back().flags = VK_PIPELINE_SHADER_STAGE_CREATE_REQUIRE_FULL_SUBGROUPS_BIT;
            pStages.back().pName = "main";
            pStages.back().stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
            pStages.push_back(vku::InitStructHelper());
            pStages.back().pName = "main";
            pStages.back().stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
            pStages.push_back(vku::InitStructHelper());
            pStages.back().pName = "main";
            pStages.back().stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            pStages_str = R"([
        {
            "sType": "VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO",
            "pNext": "NULL",
            "flags": 0,
            "module": "",
            "pName": "main",
            "pSpecializationInfo" : "NULL",
            "stage" : "VK_SHADER_STAGE_VERTEX_BIT",
        },{
            "sType": "VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO",
            "pNext": {
                "sType": "VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_REQUIRED_SUBGROUP_SIZE_CREATE_INFO",
                "pNext": "NULL",
                "requiredSubgroupSize": 64
            },
            "flags": "VK_PIPELINE_SHADER_STAGE_CREATE_REQUIRE_FULL_SUBGROUPS_BIT",
            "module": "",
            "pName": "main",
            "pSpecializationInfo" : "NULL",
            "stage" : "VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT",
        },
        {
            "sType": "VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO",
            "pNext": "NULL",
            "flags": 0,
            "module": "",
            "pName": "main",
            "pSpecializationInfo" : "NULL",
            "stage" : "VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT",
        },{
            "sType": "VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO",
            "pNext": "NULL",
            "flags": 0,
            "module": "",
            "pName": "main",
            "pSpecializationInfo" : "NULL",
            "stage" : "VK_SHADER_STAGE_FRAGMENT_BIT",
        }])";

            pvis_ci.vertexBindingDescriptionCount = 1;
            pvis_ci.pVertexBindingDescriptions = vertexBindingDescriptions;
            pvis_ci.vertexAttributeDescriptionCount = 2;
            pvis_ci.pVertexAttributeDescriptions = vertexAttributeDescriptions;
            pvis_json = R"({
            "sType" : "VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO",
            "pNext":"NULL",
            "flags" : 0,
            "vertexBindingDescriptionCount" : 1,
            "pVertexBindingDescriptions": [
                {
                    "binding" : 0,
                    "stride" : 32,
                    "inputRate" : "VK_VERTEX_INPUT_RATE_VERTEX"
                }
            ],
            "vertexAttributeDescriptionCount" : 2,
            "pVertexAttributeDescriptions": [
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
        })";

            pias_ci.topology = VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
            pias_ci.primitiveRestartEnable = VK_FALSE;
            pias_json = R"({
            "sType": "VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO",
            "pNext": "NULL",
            "flags": 0,
            "topology": "VK_PRIMITIVE_TOPOLOGY_PATCH_LIST",
            "primitiveRestartEnable": "VK_FALSE"
        })";

            pts_ci.patchControlPoints = 4;
            pts_json = R"({
            "sType": "VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO",
            "pNext": "NULL",
            "flags": 0,
            "patchControlPoints": 4
        })";

            pvs_ci.viewportCount = 1;
            pvs_ci.pViewports = viewports;
            pvs_ci.scissorCount = 1;
            pvs_ci.pScissors = scissors;
            pvs_json = R"({
            "sType": "VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO",
            "pNext": "NULL",
            "flags": 0,
            "viewportCount": 1,
            "pViewports":
            [
                {
                    "x": 0.0,
                    "y": 0.0,
                    "width": 51.0,
                    "height": 51.0,
                    "minDepth": 0.0,
                    "maxDepth": 1.0
                }
            ],
            "scissorCount": 1,
            "pScissors":
            [
                {
                    "offset":
                    {
                        "x": 0,
                        "y": 0
                    },
                    "extent":
                    {
                        "width": 51,
                        "height": 51
                    }
                }
            ]
        })";

            prs_ci.depthClampEnable = VK_FALSE;
            prs_ci.rasterizerDiscardEnable = VK_FALSE;
            prs_ci.polygonMode = VK_POLYGON_MODE_FILL;
            prs_ci.cullMode = 0;
            prs_ci.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
            prs_ci.depthBiasEnable = VK_FALSE;
            prs_ci.depthBiasConstantFactor = 0.f;
            prs_ci.depthBiasClamp = 0.f;
            prs_ci.depthBiasSlopeFactor = 0.f;
            prs_ci.lineWidth = 1.f;
            prs_json = R"({
            "sType": "VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO",
            "pNext": "NULL",
            "flags": 0,
            "depthClampEnable": "VK_FALSE",
            "rasterizerDiscardEnable": "VK_FALSE",
            "polygonMode": "VK_POLYGON_MODE_FILL",
            "cullMode": 0,
            "frontFace": "VK_FRONT_FACE_COUNTER_CLOCKWISE",
            "depthBiasEnable": "VK_FALSE",
            "depthBiasConstantFactor": 0.0,
            "depthBiasClamp": 0.0,
            "depthBiasSlopeFactor": 0.0,
            "lineWidth": 1.0
        })";

            pms_ci.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
            pms_ci.sampleShadingEnable = VK_FALSE;
            pms_ci.minSampleShading = 1.f;
            pms_ci.pSampleMask = nullptr;
            pms_ci.alphaToCoverageEnable = VK_FALSE;
            pms_ci.alphaToOneEnable = VK_FALSE;
            pms_json = R"({
            "sType": "VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO",
            "pNext": "NULL",
            "flags": 0,
            "rasterizationSamples": "VK_SAMPLE_COUNT_1_BIT",
            "sampleShadingEnable": "VK_FALSE",
            "minSampleShading": 1.0,
            "pSampleMask": "NULL",
            "alphaToCoverageEnable": "VK_FALSE",
            "alphaToOneEnable": "VK_FALSE"
        })";

            pdss_ci.depthTestEnable = VK_TRUE;
            pdss_ci.depthWriteEnable = VK_TRUE;
            pdss_ci.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
            pdss_ci.depthBoundsTestEnable = VK_FALSE;
            pdss_ci.stencilTestEnable = VK_FALSE;
            pdss_ci.front.failOp = VK_STENCIL_OP_INVERT;
            pdss_ci.front.passOp = VK_STENCIL_OP_KEEP;
            pdss_ci.front.depthFailOp = VK_STENCIL_OP_ZERO;
            pdss_ci.front.compareOp = VK_COMPARE_OP_NEVER;
            pdss_ci.front.compareMask = 0;
            pdss_ci.front.writeMask = 0;
            pdss_ci.front.reference = 0;
            pdss_ci.back.failOp = VK_STENCIL_OP_INVERT;
            pdss_ci.back.passOp = VK_STENCIL_OP_KEEP;
            pdss_ci.back.depthFailOp = VK_STENCIL_OP_ZERO;
            pdss_ci.back.compareOp = VK_COMPARE_OP_NEVER;
            pdss_ci.back.compareMask = 0;
            pdss_ci.back.writeMask = 0;
            pdss_ci.back.reference = 0;
            pdss_ci.minDepthBounds = 0.f;
            pdss_ci.maxDepthBounds = 1.f;
            pdss_json = R"({
            "sType": "VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO",
            "pNext": "NULL",
            "flags": 0,
            "depthTestEnable": "VK_TRUE",
            "depthWriteEnable": "VK_TRUE",
            "depthCompareOp": "VK_COMPARE_OP_LESS_OR_EQUAL",
            "depthBoundsTestEnable": "VK_FALSE",
            "stencilTestEnable": "VK_FALSE",
            "front":
            {
                "failOp": "VK_STENCIL_OP_INVERT",
                "passOp": "VK_STENCIL_OP_KEEP",
                "depthFailOp": "VK_STENCIL_OP_ZERO",
                "compareOp": "VK_COMPARE_OP_NEVER",
                "compareMask": 0,
                "writeMask": 0,
                "reference": 0
            },
            "back":
            {
                "failOp": "VK_STENCIL_OP_INVERT",
                "passOp": "VK_STENCIL_OP_KEEP",
                "depthFailOp": "VK_STENCIL_OP_ZERO",
                "compareOp": "VK_COMPARE_OP_NEVER",
                "compareMask": 0,
                "writeMask": 0,
                "reference": 0
            },
            "minDepthBounds": 0.0,
            "maxDepthBounds": 1.0
        })";

            pcbs_ci.logicOpEnable = VK_FALSE;
            pcbs_ci.logicOp = VK_LOGIC_OP_CLEAR;
            pcbs_ci.attachmentCount = 1;
            pcbs_ci.pAttachments = attachments;
            pcbs_ci.blendConstants[0] = 0.f;
            pcbs_ci.blendConstants[1] = 0.f;
            pcbs_ci.blendConstants[2] = 0.f;
            pcbs_ci.blendConstants[3] = 0.f;
            pcbs_json = R"({
            "sType": "VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO",
            "pNext": "NULL",
            "flags": 0,
            "logicOpEnable": "VK_FALSE",
            "logicOp": "VK_LOGIC_OP_CLEAR",
            "attachmentCount": 1,
            "pAttachments":
            [
                {
                    "blendEnable": "VK_FALSE",
                    "srcColorBlendFactor": "VK_BLEND_FACTOR_ZERO",
                    "dstColorBlendFactor": "VK_BLEND_FACTOR_ZERO",
                    "colorBlendOp": "VK_BLEND_OP_ADD",
                    "srcAlphaBlendFactor": "VK_BLEND_FACTOR_ZERO",
                    "dstAlphaBlendFactor": "VK_BLEND_FACTOR_ZERO",
                    "alphaBlendOp": "VK_BLEND_OP_ADD",
                    "colorWriteMask": "VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT"
                }
            ],
            "blendConstants": [0.0, 0.0, 0.0, 0.0]
        })";

            pds_ci.dynamicStateCount = 1;
            pds_ci.pDynamicStates = dynamicStates;
            pds_json = R"({
            "sType": "VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO",
            "pNext": "NULL",
            "flags": 0,
            "dynamicStateCount": 1,
            "pDynamicStates": ["VK_DYNAMIC_STATE_LINE_WIDTH"]
        })";

            pdrs_ci.discardRectangleMode = VK_DISCARD_RECTANGLE_MODE_EXCLUSIVE_EXT;
            pdrs_ci.discardRectangleCount = 1;
            pdrs_ci.pDiscardRectangles = &discardRectangles;
            pdrs_json = R"({
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
        })";
            break;
    }

    ci.stageCount = (uint32_t)pStages.size();
    ci.pStages = pStages.data();
    ci.pVertexInputState = &pvis_ci;
    ci.pInputAssemblyState = &pias_ci;
    ci.pTessellationState = &pts_ci;
    ci.pViewportState = &pvs_ci;
    ci.pRasterizationState = &prs_ci;
    ci.pMultisampleState = &pms_ci;
    ci.pDepthStencilState = &pdss_ci;
    ci.pColorBlendState = &pcbs_ci;
    ci.pDynamicState = &pds_ci;
    json = R"({
        "sType": "VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO",
        "pNext": )" +
           pdrs_json + R"(,
        "flags": 0,
        "stageCount": )" +
           std::to_string(ci.stageCount) + R"(,
        "pStages": )" +
           pStages_str + R"(,
        "pVertexInputState": )" +
           pvis_json + R"(,
        "pInputAssemblyState": )" +
           pias_json + R"(,
        "pTessellationState": )" +
           pts_json + R"(,
        "pViewportState": )" +
           pvs_json + R"(,
        "pRasterizationState": )" +
           prs_json + R"(,
        "pMultisampleState": )" +
           pms_json + R"(,
        "pDepthStencilState": )" +
           pdss_json + R"(,
        "pColorBlendState": )" +
           pcbs_json + R"(,
        "pDynamicState": )" +
           pds_json + R"(,
        "layout": "",
        "renderPass": "",
        "subpass": 0,
        "basePipelineHandle" : "",
        "basePipelineIndex" : 0,
    })";

    return {vku::safe_VkGraphicsPipelineCreateInfo{&ci, true, true}, json};
}

std::pair<vku::safe_VkPhysicalDeviceFeatures2, std::string> getVkPhysicalDeviceFeatures2(uint32_t seed = 0) {
    using namespace std::string_literals;

    VkPhysicalDeviceFeatures2 pdf = vku::InitStructHelper();
    [[maybe_unused]] VkPhysicalDeviceVulkan11Features pdv11f = vku::InitStructHelper();
    [[maybe_unused]] VkPhysicalDeviceScalarBlockLayoutFeatures pdsblf = vku::InitStructHelper();
    [[maybe_unused]] VkPhysicalDeviceSynchronization2Features pds2f = vku::InitStructHelper();
    std::string json{};

    std::string features_str = R"({
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
    })";
    std::string features11_str = R"(
        "storageBuffer16BitAccess" : "VK_FALSE",
        "uniformAndStorageBuffer16BitAccess" : "VK_FALSE",
        "storagePushConstant16" : "VK_FALSE",
        "storageInputOutput16" : "VK_FALSE",
        "multiview" : "VK_FALSE",
        "multiviewGeometryShader" : "VK_FALSE",
        "multiviewTessellationShader" : "VK_FALSE",
        "variablePointersStorageBuffer" : "VK_FALSE",
        "variablePointers" : "VK_FALSE",
        "protectedMemory" : "VK_FALSE",
        "samplerYcbcrConversion" : "VK_FALSE",
        "shaderDrawParameters" : "VK_FALSE"
    )";

    auto change_to_true = [](std::string& features_str, std::string_view feature) {
        std::string false_snippet = R"(")"s + feature.data() + R"(" : "VK_FALSE")";
        std::string true_snippet = R"(")"s + feature.data() + R"(" : "VK_TRUE")";
        features_str.replace(features_str.find(false_snippet), false_snippet.length(), true_snippet);
    };
    switch (seed % 3) {
        case 0:
            pdf.pNext = &pdv11f;
            pdf.features.robustBufferAccess = VK_TRUE;
            pdv11f.samplerYcbcrConversion = VK_TRUE;
            change_to_true(features_str, "robustBufferAccess");
            change_to_true(features11_str, "samplerYcbcrConversion");
            json = R"({
                "sType": "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2",
                "pNext": {
                    "sType": "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES",
                    "pNext": "NULL",
                    )"s +
                   features11_str + R"(
                },
                "features": )"s +
                   features_str + R"(
            })";
            break;
        case 1:
            pdf.pNext = &pdv11f;
            pdf.features.sparseBinding = VK_TRUE;
            pdv11f.multiview = VK_TRUE;
            change_to_true(features_str, "sparseBinding");
            change_to_true(features11_str, "multiview");
            json = R"({
                "sType": "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2",
                "pNext": {
                    "sType": "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES",
                    "pNext": "NULL",
                    )"s +
                   features11_str + R"(
                },
                "features": )"s +
                   features_str + R"(
            })";
            break;
        case 2:
            pdf.pNext = &pdsblf;
            pdsblf.pNext = &pds2f;
            pdf.features.wideLines = VK_TRUE;
            pdsblf.scalarBlockLayout = VK_TRUE;
            pds2f.synchronization2 = VK_TRUE;
            change_to_true(features_str, "wideLines");
            json = R"({
                "sType": "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2",
                "pNext": {
                    "sType": "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SCALAR_BLOCK_LAYOUT_FEATURES",
                    "pNext": {
                        "sType": "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES",
                        "pNext": "NULL",
                        "synchronization2": "VK_TRUE"
                    },
                    "scalarBlockLayout": "VK_TRUE"
                },
                "features": )"s +
                   features_str + R"(
            })";
            break;
    }

    return {&pdf, json};
}

// NOTE: It is known that VpjShaderFileName holds it's members in a non-owning manner. In the absence of
// safe_VpjShaderFilename This utility only works with pFilename and pCode kept alive by the caller
// (literal or otherwise).
std::pair<std::vector<VpjShaderFileName>, std::string> getShaderFileNames(
    std::initializer_list<VpjShaderFileName> shader_filenames = {}) {
    using namespace std::string_literals;

    std::vector<VpjShaderFileName> shaderFilenames(shader_filenames);
    std::string json{};

    json = "[";
    for (auto it = shaderFilenames.begin(); it != shaderFilenames.end(); ++it) {
        json += R"({
            "filename" : ")"s +
                it->pFilename + R"(",
            "stage" : ")" +
                shader_stage_flag_to_string(it->stage) + R"("})";
        if (std::next(it) != shaderFilenames.end()) {
            json += ",";
        }
    }
    json += "]";

    return {shaderFilenames, json};
}

void CompareStruct(const vku::safe_VkPhysicalDeviceSynchronization2Features& ref,
                   const VkPhysicalDeviceSynchronization2Features& res) {
    EXPECT_EQ(ref.sType, res.sType);
    EXPECT_EQ(ref.synchronization2, res.synchronization2);
}

void CompareStruct(const vku::safe_VkPhysicalDeviceVulkan11Features& ref, const VkPhysicalDeviceVulkan11Features& res) {
    EXPECT_EQ(ref.sType, res.sType);
    EXPECT_EQ(ref.storageBuffer16BitAccess, res.storageBuffer16BitAccess);
    EXPECT_EQ(ref.uniformAndStorageBuffer16BitAccess, res.uniformAndStorageBuffer16BitAccess);
    EXPECT_EQ(ref.storagePushConstant16, res.storagePushConstant16);
    EXPECT_EQ(ref.storageInputOutput16, res.storageInputOutput16);
    EXPECT_EQ(ref.multiview, res.multiview);
    EXPECT_EQ(ref.multiviewGeometryShader, res.multiviewGeometryShader);
    EXPECT_EQ(ref.multiviewTessellationShader, res.multiviewTessellationShader);
    EXPECT_EQ(ref.variablePointersStorageBuffer, res.variablePointersStorageBuffer);
    EXPECT_EQ(ref.variablePointers, res.variablePointers);
    EXPECT_EQ(ref.protectedMemory, res.protectedMemory);
    EXPECT_EQ(ref.samplerYcbcrConversion, res.samplerYcbcrConversion);
    EXPECT_EQ(ref.shaderDrawParameters, res.shaderDrawParameters);
}

void CompareStruct(const vku::safe_VkPhysicalDeviceScalarBlockLayoutFeatures& ref,
                   const VkPhysicalDeviceScalarBlockLayoutFeatures& res) {
    EXPECT_EQ(ref.sType, res.sType);
    EXPECT_EQ(ref.scalarBlockLayout, res.scalarBlockLayout);
}

void CompareStruct(const vku::safe_VkPhysicalDeviceFeatures2& ref, const VkPhysicalDeviceFeatures2& res) {
    EXPECT_EQ(ref.sType, res.sType);
    for (const VkBaseInStructure *ref_pNext = reinterpret_cast<VkBaseInStructure*>(ref.pNext),
                                 *res_pNext = reinterpret_cast<VkBaseInStructure*>(res.pNext);
         ref_pNext != nullptr; ref_pNext = ref_pNext->pNext, res_pNext = res_pNext->pNext) {
        switch (ref_pNext->sType) {
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES:
                CompareStruct(*reinterpret_cast<const vku::safe_VkPhysicalDeviceSynchronization2Features*>(ref_pNext),
                              *reinterpret_cast<const VkPhysicalDeviceSynchronization2Features*>(res_pNext));
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES:
                CompareStruct(*reinterpret_cast<const vku::safe_VkPhysicalDeviceVulkan11Features*>(ref_pNext),
                              *reinterpret_cast<const VkPhysicalDeviceVulkan11Features*>(res_pNext));
                break;
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SCALAR_BLOCK_LAYOUT_FEATURES:
                CompareStruct(*reinterpret_cast<const vku::safe_VkPhysicalDeviceScalarBlockLayoutFeatures*>(ref_pNext),
                              *reinterpret_cast<const VkPhysicalDeviceScalarBlockLayoutFeatures*>(res_pNext));
                break;
            default:
                FAIL() << "Unkown sType found in reference struct";
                break;
        }
    }
    EXPECT_EQ(ref.features.robustBufferAccess, res.features.robustBufferAccess);
    EXPECT_EQ(ref.features.fullDrawIndexUint32, res.features.fullDrawIndexUint32);
    EXPECT_EQ(ref.features.imageCubeArray, res.features.imageCubeArray);
    EXPECT_EQ(ref.features.independentBlend, res.features.independentBlend);
    EXPECT_EQ(ref.features.geometryShader, res.features.geometryShader);
    EXPECT_EQ(ref.features.tessellationShader, res.features.tessellationShader);
    EXPECT_EQ(ref.features.sampleRateShading, res.features.sampleRateShading);
    EXPECT_EQ(ref.features.dualSrcBlend, res.features.dualSrcBlend);
    EXPECT_EQ(ref.features.logicOp, res.features.logicOp);
    EXPECT_EQ(ref.features.multiDrawIndirect, res.features.multiDrawIndirect);
    EXPECT_EQ(ref.features.drawIndirectFirstInstance, res.features.drawIndirectFirstInstance);
    EXPECT_EQ(ref.features.depthClamp, res.features.depthClamp);
    EXPECT_EQ(ref.features.depthBiasClamp, res.features.depthBiasClamp);
    EXPECT_EQ(ref.features.fillModeNonSolid, res.features.fillModeNonSolid);
    EXPECT_EQ(ref.features.depthBounds, res.features.depthBounds);
    EXPECT_EQ(ref.features.wideLines, res.features.wideLines);
    EXPECT_EQ(ref.features.largePoints, res.features.largePoints);
    EXPECT_EQ(ref.features.alphaToOne, res.features.alphaToOne);
    EXPECT_EQ(ref.features.multiViewport, res.features.multiViewport);
    EXPECT_EQ(ref.features.samplerAnisotropy, res.features.samplerAnisotropy);
    EXPECT_EQ(ref.features.textureCompressionETC2, res.features.textureCompressionETC2);
    EXPECT_EQ(ref.features.textureCompressionASTC_LDR, res.features.textureCompressionASTC_LDR);
    EXPECT_EQ(ref.features.textureCompressionBC, res.features.textureCompressionBC);
    EXPECT_EQ(ref.features.occlusionQueryPrecise, res.features.occlusionQueryPrecise);
    EXPECT_EQ(ref.features.pipelineStatisticsQuery, res.features.pipelineStatisticsQuery);
    EXPECT_EQ(ref.features.vertexPipelineStoresAndAtomics, res.features.vertexPipelineStoresAndAtomics);
    EXPECT_EQ(ref.features.fragmentStoresAndAtomics, res.features.fragmentStoresAndAtomics);
    EXPECT_EQ(ref.features.shaderTessellationAndGeometryPointSize, res.features.shaderTessellationAndGeometryPointSize);
    EXPECT_EQ(ref.features.shaderImageGatherExtended, res.features.shaderImageGatherExtended);
    EXPECT_EQ(ref.features.shaderStorageImageExtendedFormats, res.features.shaderStorageImageExtendedFormats);
    EXPECT_EQ(ref.features.shaderStorageImageMultisample, res.features.shaderStorageImageMultisample);
    EXPECT_EQ(ref.features.shaderStorageImageReadWithoutFormat, res.features.shaderStorageImageReadWithoutFormat);
    EXPECT_EQ(ref.features.shaderStorageImageWriteWithoutFormat, res.features.shaderStorageImageWriteWithoutFormat);
    EXPECT_EQ(ref.features.shaderUniformBufferArrayDynamicIndexing, res.features.shaderUniformBufferArrayDynamicIndexing);
    EXPECT_EQ(ref.features.shaderSampledImageArrayDynamicIndexing, res.features.shaderSampledImageArrayDynamicIndexing);
    EXPECT_EQ(ref.features.shaderStorageBufferArrayDynamicIndexing, res.features.shaderStorageBufferArrayDynamicIndexing);
    EXPECT_EQ(ref.features.shaderStorageImageArrayDynamicIndexing, res.features.shaderStorageImageArrayDynamicIndexing);
    EXPECT_EQ(ref.features.shaderClipDistance, res.features.shaderClipDistance);
    EXPECT_EQ(ref.features.shaderCullDistance, res.features.shaderCullDistance);
    EXPECT_EQ(ref.features.shaderFloat64, res.features.shaderFloat64);
    EXPECT_EQ(ref.features.shaderInt64, res.features.shaderInt64);
    EXPECT_EQ(ref.features.shaderInt16, res.features.shaderInt16);
    EXPECT_EQ(ref.features.shaderResourceResidency, res.features.shaderResourceResidency);
    EXPECT_EQ(ref.features.shaderResourceMinLod, res.features.shaderResourceMinLod);
    EXPECT_EQ(ref.features.sparseBinding, res.features.sparseBinding);
    EXPECT_EQ(ref.features.sparseResidencyBuffer, res.features.sparseResidencyBuffer);
    EXPECT_EQ(ref.features.sparseResidencyImage2D, res.features.sparseResidencyImage2D);
    EXPECT_EQ(ref.features.sparseResidencyImage3D, res.features.sparseResidencyImage3D);
    EXPECT_EQ(ref.features.sparseResidency2Samples, res.features.sparseResidency2Samples);
    EXPECT_EQ(ref.features.sparseResidency4Samples, res.features.sparseResidency4Samples);
    EXPECT_EQ(ref.features.sparseResidency8Samples, res.features.sparseResidency8Samples);
    EXPECT_EQ(ref.features.sparseResidency16Samples, res.features.sparseResidency16Samples);
    EXPECT_EQ(ref.features.sparseResidencyAliased, res.features.sparseResidencyAliased);
    EXPECT_EQ(ref.features.variableMultisampleRate, res.features.variableMultisampleRate);
    EXPECT_EQ(ref.features.inheritedQueries, res.features.inheritedQueries);
}

void CompareStruct(const vku::safe_VkSamplerYcbcrConversionCreateInfo& ref, const VkSamplerYcbcrConversionCreateInfo& res) {
    EXPECT_EQ(ref.sType, res.sType);
    for (const VkBaseInStructure *ref_pNext = reinterpret_cast<const VkBaseInStructure*>(ref.pNext),
                                 *res_pNext = reinterpret_cast<const VkBaseInStructure*>(res.pNext);
         ref_pNext != nullptr; ref_pNext = ref_pNext->pNext, res_pNext = res_pNext->pNext) {
        switch (ref_pNext->sType) {
            default:
                FAIL() << "Unkown sType found in reference struct";
                break;
        }
    }
    EXPECT_EQ(ref.format, res.format);
    EXPECT_EQ(ref.ycbcrModel, res.ycbcrModel);
    EXPECT_EQ(ref.ycbcrRange, res.ycbcrRange);
    EXPECT_EQ(ref.components.r, res.components.r);
    EXPECT_EQ(ref.components.g, res.components.g);
    EXPECT_EQ(ref.components.b, res.components.b);
    EXPECT_EQ(ref.components.a, res.components.a);
    EXPECT_EQ(ref.xChromaOffset, res.xChromaOffset);
    EXPECT_EQ(ref.yChromaOffset, res.yChromaOffset);
    EXPECT_EQ(ref.chromaFilter, res.chromaFilter);
}

void CompareStruct(const vku::safe_VkSamplerYcbcrConversionInfo& ref, const VkSamplerYcbcrConversionInfo& res,
                   bool has_names = false) {
    EXPECT_EQ(ref.sType, res.sType);
    if (!has_names) {
        EXPECT_EQ(ref.conversion, res.conversion);
    }
}

void CompareStruct(const vku::safe_VkSamplerReductionModeCreateInfo& ref, const VkSamplerReductionModeCreateInfo& res) {
    EXPECT_EQ(ref.sType, res.sType);
    EXPECT_EQ(ref.reductionMode, res.reductionMode);
}

void CompareStruct(const vku::safe_VkSamplerCreateInfo& ref, const VkSamplerCreateInfo& res, bool has_names = false) {
    EXPECT_EQ(ref.sType, res.sType);
    for (const VkBaseInStructure *ref_pNext = reinterpret_cast<const VkBaseInStructure*>(ref.pNext),
                                 *res_pNext = reinterpret_cast<const VkBaseInStructure*>(res.pNext);
         ref_pNext != nullptr; ref_pNext = ref_pNext->pNext, res_pNext = res_pNext->pNext) {
        switch (ref_pNext->sType) {
            case VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_INFO:
                CompareStruct(*reinterpret_cast<const vku::safe_VkSamplerYcbcrConversionInfo*>(ref_pNext),
                              *reinterpret_cast<const VkSamplerYcbcrConversionInfo*>(res_pNext), has_names);
                break;
            case VK_STRUCTURE_TYPE_SAMPLER_REDUCTION_MODE_CREATE_INFO:
                CompareStruct(*reinterpret_cast<const vku::safe_VkSamplerReductionModeCreateInfo*>(ref_pNext),
                              *reinterpret_cast<const VkSamplerReductionModeCreateInfo*>(res_pNext));
                break;
            default:
                FAIL() << "Unkown sType found in reference struct";
                break;
        }
    }
    EXPECT_EQ(ref.flags, res.flags);
    EXPECT_EQ(ref.magFilter, res.magFilter);
    EXPECT_EQ(ref.minFilter, res.minFilter);
    EXPECT_EQ(ref.mipmapMode, res.mipmapMode);
    EXPECT_EQ(ref.addressModeU, res.addressModeU);
    EXPECT_EQ(ref.addressModeV, res.addressModeV);
    EXPECT_EQ(ref.addressModeW, res.addressModeW);
    EXPECT_EQ(ref.mipLodBias, res.mipLodBias);
    EXPECT_EQ(ref.anisotropyEnable, res.anisotropyEnable);
    EXPECT_EQ(ref.maxAnisotropy, res.maxAnisotropy);
    EXPECT_EQ(ref.compareEnable, res.compareEnable);
    EXPECT_EQ(ref.compareOp, res.compareOp);
    if (std::isnan(ref.minLod)) {
        EXPECT_TRUE(std::isnan(res.minLod));
    } else {
        EXPECT_EQ(ref.compareOp, res.compareOp);
    };
    EXPECT_EQ(ref.maxLod, res.maxLod);
    EXPECT_EQ(ref.borderColor, res.borderColor);
    EXPECT_EQ(ref.unnormalizedCoordinates, res.unnormalizedCoordinates);
}

void CompareStruct(const vku::safe_VkDescriptorSetLayoutBindingFlagsCreateInfo& ref,
                   const VkDescriptorSetLayoutBindingFlagsCreateInfo& res) {
    EXPECT_EQ(ref.sType, res.sType);
    EXPECT_EQ(ref.bindingCount, res.bindingCount);
    for (uint32_t i = 0; i < ref.bindingCount; ++i) {
        EXPECT_EQ(ref.pBindingFlags[i], res.pBindingFlags[i]);
    }
}

void CompareStruct(const vku::safe_VkDescriptorSetLayoutCreateInfo& ref, const VkDescriptorSetLayoutCreateInfo& res,
                   bool has_names = false) {
    EXPECT_EQ(ref.sType, res.sType);
    for (const VkBaseInStructure *ref_pNext = reinterpret_cast<const VkBaseInStructure*>(ref.pNext),
                                 *res_pNext = reinterpret_cast<const VkBaseInStructure*>(res.pNext);
         ref_pNext != nullptr; ref_pNext = ref_pNext->pNext, res_pNext = res_pNext->pNext) {
        switch (ref_pNext->sType) {
            case VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO:
                CompareStruct(*reinterpret_cast<const vku::safe_VkDescriptorSetLayoutBindingFlagsCreateInfo*>(ref_pNext),
                              *reinterpret_cast<const VkDescriptorSetLayoutBindingFlagsCreateInfo*>(res_pNext));
                break;
            default:
                FAIL() << "Unkown sType found in reference struct";
                break;
        }
    }
    EXPECT_EQ(ref.flags, res.flags);
    EXPECT_EQ(ref.bindingCount, res.bindingCount);
    for (uint32_t i = 0; i < ref.bindingCount; ++i) {
        EXPECT_EQ(ref.pBindings[i].binding, res.pBindings[i].binding);
        EXPECT_EQ(ref.pBindings[i].descriptorType, res.pBindings[i].descriptorType);
        EXPECT_EQ(ref.pBindings[i].descriptorCount, res.pBindings[i].descriptorCount);
        EXPECT_EQ(ref.pBindings[i].stageFlags, res.pBindings[i].stageFlags);
        if (!has_names) {
            if (ref.pBindings[i].pImmutableSamplers) {
                EXPECT_EQ(*ref.pBindings[i].pImmutableSamplers, *res.pBindings[i].pImmutableSamplers);
            } else {
                EXPECT_EQ(ref.pBindings[i].pImmutableSamplers, res.pBindings[i].pImmutableSamplers);
            }
        }
    }
}

void CompareStruct(const vku::safe_VkPipelineLayoutCreateInfo& ref, const VkPipelineLayoutCreateInfo& res, bool has_names = false) {
    EXPECT_EQ(ref.sType, res.sType);
    for (const VkBaseInStructure *ref_pNext = reinterpret_cast<const VkBaseInStructure*>(ref.pNext),
                                 *res_pNext = reinterpret_cast<const VkBaseInStructure*>(res.pNext);
         ref_pNext != nullptr; ref_pNext = ref_pNext->pNext, res_pNext = res_pNext->pNext) {
        switch (ref_pNext->sType) {
            default:
                FAIL() << "Unkown sType found in reference struct";
                break;
        }
    }
    EXPECT_EQ(ref.flags, res.flags);
    EXPECT_EQ(ref.setLayoutCount, res.setLayoutCount);
    if (ref.setLayoutCount) {
        for (uint32_t i = 0; i < ref.setLayoutCount; ++i) {
            if (!has_names) {
                EXPECT_EQ(ref.pSetLayouts[i], res.pSetLayouts[i]);
            }
        }
    } else {
        EXPECT_EQ(nullptr, res.pSetLayouts);
    }
    EXPECT_EQ(ref.pushConstantRangeCount, res.pushConstantRangeCount);
    if (ref.pushConstantRangeCount) {
        for (uint32_t i = 0; i < ref.pushConstantRangeCount; ++i) {
            EXPECT_EQ(ref.pPushConstantRanges[i].stageFlags, res.pPushConstantRanges[i].stageFlags);
            EXPECT_EQ(ref.pPushConstantRanges[i].offset, res.pPushConstantRanges[i].offset);
            EXPECT_EQ(ref.pPushConstantRanges[i].size, res.pPushConstantRanges[i].size);
        }
    } else {
        EXPECT_EQ(nullptr, res.pPushConstantRanges);
    }
}

void CompareStruct(const vku::safe_VkRenderPassMultiviewCreateInfo& ref, const VkRenderPassMultiviewCreateInfo& res) {
    EXPECT_EQ(ref.sType, res.sType);
    EXPECT_EQ(ref.subpassCount, res.subpassCount);
    if (ref.subpassCount) {
        for (uint32_t i = 0; i < ref.subpassCount; ++i) {
            EXPECT_EQ(ref.pViewMasks[i], res.pViewMasks[i]);
        }
    } else {
        EXPECT_EQ(nullptr, res.pViewMasks);
    }
    EXPECT_EQ(ref.dependencyCount, res.dependencyCount);
    if (ref.dependencyCount) {
        for (uint32_t i = 0; i < ref.dependencyCount; ++i) {
            EXPECT_EQ(ref.pViewOffsets[i], res.pViewOffsets[i]);
        }
    } else {
        EXPECT_EQ(nullptr, res.pViewOffsets);
    }
    EXPECT_EQ(ref.correlationMaskCount, res.correlationMaskCount);
    if (ref.correlationMaskCount) {
        for (uint32_t i = 0; i < ref.correlationMaskCount; ++i) {
            EXPECT_EQ(ref.pCorrelationMasks[i], res.pCorrelationMasks[i]);
        }
    } else {
        EXPECT_EQ(nullptr, res.pCorrelationMasks);
    }
}

void CompareStruct(const vku::safe_VkRenderPassCreateInfo& ref, const VkRenderPassCreateInfo& res) {
    EXPECT_EQ(ref.sType, res.sType);
    for (const VkBaseInStructure *ref_pNext = reinterpret_cast<const VkBaseInStructure*>(ref.pNext),
                                 *res_pNext = reinterpret_cast<const VkBaseInStructure*>(res.pNext);
         ref_pNext != nullptr; ref_pNext = ref_pNext->pNext, res_pNext = res_pNext->pNext) {
        switch (ref_pNext->sType) {
            case VK_STRUCTURE_TYPE_RENDER_PASS_MULTIVIEW_CREATE_INFO:
                CompareStruct(*reinterpret_cast<const vku::safe_VkRenderPassMultiviewCreateInfo*>(ref_pNext),
                              *reinterpret_cast<const VkRenderPassMultiviewCreateInfo*>(res_pNext));
                break;
            default:
                FAIL() << "Unkown sType found in reference struct";
                break;
        }
    }
    EXPECT_EQ(ref.flags, res.flags);
    EXPECT_EQ(ref.attachmentCount, res.attachmentCount);
    if (ref.attachmentCount) {
        for (uint32_t i = 0; i < ref.attachmentCount; ++i) {
            EXPECT_EQ(ref.pAttachments[i].flags, res.pAttachments[i].flags);
            EXPECT_EQ(ref.pAttachments[i].format, res.pAttachments[i].format);
            EXPECT_EQ(ref.pAttachments[i].samples, res.pAttachments[i].samples);
            EXPECT_EQ(ref.pAttachments[i].loadOp, res.pAttachments[i].loadOp);
            EXPECT_EQ(ref.pAttachments[i].storeOp, res.pAttachments[i].storeOp);
            EXPECT_EQ(ref.pAttachments[i].stencilLoadOp, res.pAttachments[i].stencilLoadOp);
            EXPECT_EQ(ref.pAttachments[i].stencilStoreOp, res.pAttachments[i].stencilStoreOp);
            EXPECT_EQ(ref.pAttachments[i].initialLayout, res.pAttachments[i].initialLayout);
            EXPECT_EQ(ref.pAttachments[i].finalLayout, res.pAttachments[i].finalLayout);
        }
    } else {
        EXPECT_EQ(nullptr, res.pAttachments);
    }
    EXPECT_EQ(ref.subpassCount, res.subpassCount);
    if (ref.subpassCount) {
        for (uint32_t i = 0; i < ref.subpassCount; ++i) {
            EXPECT_EQ(ref.pSubpasses[i].flags, res.pSubpasses[i].flags);
            EXPECT_EQ(ref.pSubpasses[i].pipelineBindPoint, res.pSubpasses[i].pipelineBindPoint);
            EXPECT_EQ(ref.pSubpasses[i].inputAttachmentCount, res.pSubpasses[i].inputAttachmentCount);
            if (ref.pSubpasses[i].inputAttachmentCount) {
                for (uint32_t j = 0; j < ref.pSubpasses[i].inputAttachmentCount; ++j) {
                    EXPECT_EQ(ref.pSubpasses[i].pInputAttachments[j].attachment, res.pSubpasses[i].pInputAttachments[j].attachment);
                    EXPECT_EQ(ref.pSubpasses[i].pInputAttachments[j].layout, res.pSubpasses[i].pInputAttachments[j].layout);
                }
            } else {
                EXPECT_EQ(nullptr, res.pSubpasses[i].pInputAttachments);
            }
            EXPECT_EQ(ref.pSubpasses[i].colorAttachmentCount, res.pSubpasses[i].colorAttachmentCount);
            if (ref.pSubpasses[i].colorAttachmentCount) {
                for (uint32_t j = 0; j < ref.pSubpasses[i].colorAttachmentCount; ++j) {
                    EXPECT_EQ(ref.pSubpasses[i].pColorAttachments[j].attachment, res.pSubpasses[i].pColorAttachments[j].attachment);
                    EXPECT_EQ(ref.pSubpasses[i].pColorAttachments[j].layout, res.pSubpasses[i].pColorAttachments[j].layout);
                }
            } else {
                EXPECT_EQ(nullptr, res.pSubpasses[i].pColorAttachments);
            }
            if (ref.pSubpasses[i].pResolveAttachments) {
                for (uint32_t j = 0; j < ref.pSubpasses[i].colorAttachmentCount; ++j) {
                    EXPECT_EQ(ref.pSubpasses[i].pResolveAttachments[j].attachment,
                              res.pSubpasses[i].pResolveAttachments[j].attachment);
                    EXPECT_EQ(ref.pSubpasses[i].pResolveAttachments[j].layout, res.pSubpasses[i].pResolveAttachments[j].layout);
                }
            } else {
                EXPECT_EQ(nullptr, res.pSubpasses[i].pResolveAttachments);
            }
            if (ref.pSubpasses[i].pDepthStencilAttachment) {
                for (uint32_t j = 0; j < ref.pSubpasses[i].colorAttachmentCount; ++j) {
                    EXPECT_EQ(ref.pSubpasses[i].pDepthStencilAttachment[j].attachment,
                              res.pSubpasses[i].pDepthStencilAttachment[j].attachment);
                    EXPECT_EQ(ref.pSubpasses[i].pDepthStencilAttachment[j].layout,
                              res.pSubpasses[i].pDepthStencilAttachment[j].layout);
                }
            } else {
                EXPECT_EQ(nullptr, res.pSubpasses[i].pDepthStencilAttachment);
            }
            if (ref.pSubpasses[i].pPreserveAttachments) {
                for (uint32_t j = 0; j < ref.pSubpasses[i].colorAttachmentCount; ++j) {
                    EXPECT_EQ(ref.pSubpasses[i].pPreserveAttachments[j], res.pSubpasses[i].pPreserveAttachments[j]);
                }
            } else {
                EXPECT_EQ(nullptr, res.pSubpasses[i].pPreserveAttachments);
            }
        }
    } else {
        EXPECT_EQ(nullptr, res.pSubpasses);
    }
    EXPECT_EQ(ref.dependencyCount, res.dependencyCount);
    if (ref.dependencyCount) {
        for (uint32_t i = 0; i < ref.dependencyCount; ++i) {
            EXPECT_EQ(ref.pDependencies[i].srcSubpass, res.pDependencies[i].srcSubpass);
            EXPECT_EQ(ref.pDependencies[i].dstSubpass, res.pDependencies[i].dstSubpass);
            EXPECT_EQ(ref.pDependencies[i].srcStageMask, res.pDependencies[i].srcStageMask);
            EXPECT_EQ(ref.pDependencies[i].dstStageMask, res.pDependencies[i].dstStageMask);
            EXPECT_EQ(ref.pDependencies[i].srcAccessMask, res.pDependencies[i].srcAccessMask);
            EXPECT_EQ(ref.pDependencies[i].dstAccessMask, res.pDependencies[i].dstAccessMask);
            EXPECT_EQ(ref.pDependencies[i].dependencyFlags, res.pDependencies[i].dependencyFlags);
        }
    } else {
        EXPECT_EQ(nullptr, res.pDependencies);
    }
}

void CompareStruct(const vku::safe_VkAttachmentDescription2& ref, const VkAttachmentDescription2& res) {
    EXPECT_EQ(ref.sType, res.sType);
    for (const VkBaseInStructure *ref_pNext = reinterpret_cast<const VkBaseInStructure*>(ref.pNext),
                                 *res_pNext = reinterpret_cast<const VkBaseInStructure*>(res.pNext);
         ref_pNext != nullptr; ref_pNext = ref_pNext->pNext, res_pNext = res_pNext->pNext) {
        switch (ref_pNext->sType) {
            default:
                FAIL() << "Unkown sType found in reference struct";
                break;
        }
    }
    EXPECT_EQ(ref.flags, res.flags);
    EXPECT_EQ(ref.format, res.format);
    EXPECT_EQ(ref.samples, res.samples);
    EXPECT_EQ(ref.loadOp, res.loadOp);
    EXPECT_EQ(ref.storeOp, res.storeOp);
    EXPECT_EQ(ref.stencilLoadOp, res.stencilLoadOp);
    EXPECT_EQ(ref.stencilStoreOp, res.stencilStoreOp);
    EXPECT_EQ(ref.initialLayout, res.initialLayout);
    EXPECT_EQ(ref.finalLayout, res.finalLayout);
}

void CompareStruct(const vku::safe_VkAttachmentReference2& ref, const VkAttachmentReference2& res) {
    EXPECT_EQ(ref.sType, res.sType);
    for (const VkBaseInStructure *ref_pNext = reinterpret_cast<const VkBaseInStructure*>(ref.pNext),
                                 *res_pNext = reinterpret_cast<const VkBaseInStructure*>(res.pNext);
         ref_pNext != nullptr; ref_pNext = ref_pNext->pNext, res_pNext = res_pNext->pNext) {
        switch (ref_pNext->sType) {
            default:
                FAIL() << "Unkown sType found in reference struct";
                break;
        }
    }
    EXPECT_EQ(ref.attachment, res.attachment);
    EXPECT_EQ(ref.layout, res.layout);
    EXPECT_EQ(ref.aspectMask, res.aspectMask);
}

void CompareStruct(const vku::safe_VkFragmentShadingRateAttachmentInfoKHR& ref, const VkFragmentShadingRateAttachmentInfoKHR& res) {
    EXPECT_EQ(ref.sType, res.sType);
    if (ref.pFragmentShadingRateAttachment) {
        CompareStruct(*ref.pFragmentShadingRateAttachment, *res.pFragmentShadingRateAttachment);
    } else {
        EXPECT_EQ(nullptr, res.pFragmentShadingRateAttachment);
    }
    EXPECT_EQ(ref.shadingRateAttachmentTexelSize.width, res.shadingRateAttachmentTexelSize.width);
    EXPECT_EQ(ref.shadingRateAttachmentTexelSize.height, res.shadingRateAttachmentTexelSize.height);
}

void CompareStruct(const vku::safe_VkSubpassDescriptionDepthStencilResolve& ref,
                   const VkSubpassDescriptionDepthStencilResolve& res) {
    EXPECT_EQ(ref.sType, res.sType);
    if (ref.pDepthStencilResolveAttachment) {
        CompareStruct(*ref.pDepthStencilResolveAttachment, *res.pDepthStencilResolveAttachment);
    } else {
        EXPECT_EQ(nullptr, res.pDepthStencilResolveAttachment);
    }
}

void CompareStruct(const vku::safe_VkSubpassDescription2& ref, const VkSubpassDescription2& res) {
    EXPECT_EQ(ref.sType, res.sType);
    for (const VkBaseInStructure *ref_pNext = reinterpret_cast<const VkBaseInStructure*>(ref.pNext),
                                 *res_pNext = reinterpret_cast<const VkBaseInStructure*>(res.pNext);
         ref_pNext != nullptr; ref_pNext = ref_pNext->pNext, res_pNext = res_pNext->pNext) {
        switch (ref_pNext->sType) {
            case VK_STRUCTURE_TYPE_FRAGMENT_SHADING_RATE_ATTACHMENT_INFO_KHR:
                CompareStruct(*reinterpret_cast<const vku::safe_VkFragmentShadingRateAttachmentInfoKHR*>(ref_pNext),
                              *reinterpret_cast<const VkFragmentShadingRateAttachmentInfoKHR*>(res_pNext));
                break;
            case VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_DEPTH_STENCIL_RESOLVE:
                CompareStruct(*reinterpret_cast<const vku::safe_VkSubpassDescriptionDepthStencilResolve*>(ref_pNext),
                              *reinterpret_cast<const VkSubpassDescriptionDepthStencilResolve*>(res_pNext));
                break;
            default:
                FAIL() << "Unkown sType found in reference struct";
                break;
        }
    }
    EXPECT_EQ(ref.flags, res.flags);
    EXPECT_EQ(ref.pipelineBindPoint, res.pipelineBindPoint);
    EXPECT_EQ(ref.inputAttachmentCount, res.inputAttachmentCount);
    if (ref.inputAttachmentCount) {
        for (uint32_t j = 0; j < ref.inputAttachmentCount; ++j) {
            EXPECT_EQ(ref.pInputAttachments[j].attachment, res.pInputAttachments[j].attachment);
            EXPECT_EQ(ref.pInputAttachments[j].layout, res.pInputAttachments[j].layout);
        }
    } else {
        EXPECT_EQ(nullptr, res.pInputAttachments);
    }
    EXPECT_EQ(ref.colorAttachmentCount, res.colorAttachmentCount);
    if (ref.colorAttachmentCount) {
        for (uint32_t j = 0; j < ref.colorAttachmentCount; ++j) {
            EXPECT_EQ(ref.pColorAttachments[j].attachment, res.pColorAttachments[j].attachment);
            EXPECT_EQ(ref.pColorAttachments[j].layout, res.pColorAttachments[j].layout);
        }
    } else {
        EXPECT_EQ(nullptr, res.pColorAttachments);
    }
    if (ref.pResolveAttachments) {
        for (uint32_t j = 0; j < ref.colorAttachmentCount; ++j) {
            EXPECT_EQ(ref.pResolveAttachments[j].attachment, res.pResolveAttachments[j].attachment);
            EXPECT_EQ(ref.pResolveAttachments[j].layout, res.pResolveAttachments[j].layout);
        }
    } else {
        EXPECT_EQ(nullptr, res.pResolveAttachments);
    }
    if (ref.pDepthStencilAttachment) {
        for (uint32_t j = 0; j < ref.colorAttachmentCount; ++j) {
            EXPECT_EQ(ref.pDepthStencilAttachment[j].attachment, res.pDepthStencilAttachment[j].attachment);
            EXPECT_EQ(ref.pDepthStencilAttachment[j].layout, res.pDepthStencilAttachment[j].layout);
        }
    } else {
        EXPECT_EQ(nullptr, res.pDepthStencilAttachment);
    }
    if (ref.pPreserveAttachments) {
        for (uint32_t j = 0; j < ref.colorAttachmentCount; ++j) {
            EXPECT_EQ(ref.pPreserveAttachments[j], res.pPreserveAttachments[j]);
        }
    } else {
        EXPECT_EQ(nullptr, res.pPreserveAttachments);
    }
}

void CompareStruct(const vku::safe_VkMemoryBarrier2& ref, const VkMemoryBarrier2KHR& res) {
    EXPECT_EQ(ref.sType, res.sType);
    EXPECT_EQ(ref.srcStageMask, res.srcStageMask);
    EXPECT_EQ(ref.srcAccessMask, res.srcAccessMask);
    EXPECT_EQ(ref.dstStageMask, res.dstStageMask);
    EXPECT_EQ(ref.dstAccessMask, res.dstAccessMask);
}

void CompareStruct(const vku::safe_VkSubpassDependency2& ref, const VkSubpassDependency2& res) {
    EXPECT_EQ(ref.sType, res.sType);
    for (const VkBaseInStructure *ref_pNext = reinterpret_cast<const VkBaseInStructure*>(ref.pNext),
                                 *res_pNext = reinterpret_cast<const VkBaseInStructure*>(res.pNext);
         ref_pNext != nullptr; ref_pNext = ref_pNext->pNext, res_pNext = res_pNext->pNext) {
        switch (ref_pNext->sType) {
            case VK_STRUCTURE_TYPE_MEMORY_BARRIER_2_KHR:
                CompareStruct(*reinterpret_cast<const vku::safe_VkMemoryBarrier2*>(ref_pNext),
                              *reinterpret_cast<const VkMemoryBarrier2KHR*>(res_pNext));
                break;
            default:
                FAIL() << "Unkown sType found in reference struct";
                break;
        }
    }
    EXPECT_EQ(ref.srcSubpass, res.srcSubpass);
    EXPECT_EQ(ref.dstSubpass, res.dstSubpass);
    EXPECT_EQ(ref.srcStageMask, res.srcStageMask);
    EXPECT_EQ(ref.dstStageMask, res.dstStageMask);
    EXPECT_EQ(ref.srcAccessMask, res.srcAccessMask);
    EXPECT_EQ(ref.dstAccessMask, res.dstAccessMask);
    EXPECT_EQ(ref.dependencyFlags, res.dependencyFlags);
}

void CompareStruct(const vku::safe_VkRenderPassCreateInfo2& ref, const VkRenderPassCreateInfo2& res) {
    EXPECT_EQ(ref.sType, res.sType);
    for (const VkBaseInStructure *ref_pNext = reinterpret_cast<const VkBaseInStructure*>(ref.pNext),
                                 *res_pNext = reinterpret_cast<const VkBaseInStructure*>(res.pNext);
         ref_pNext != nullptr; ref_pNext = ref_pNext->pNext, res_pNext = res_pNext->pNext) {
        switch (ref_pNext->sType) {
            case VK_STRUCTURE_TYPE_RENDER_PASS_MULTIVIEW_CREATE_INFO:
                CompareStruct(*reinterpret_cast<const vku::safe_VkRenderPassMultiviewCreateInfo*>(ref_pNext),
                              *reinterpret_cast<const VkRenderPassMultiviewCreateInfo*>(res_pNext));
                break;
            default:
                FAIL() << "Unkown sType found in reference struct";
                break;
        }
    }
    EXPECT_EQ(ref.flags, res.flags);
    EXPECT_EQ(ref.attachmentCount, res.attachmentCount);
    if (ref.attachmentCount) {
        for (uint32_t i = 0; i < ref.attachmentCount; ++i) {
            CompareStruct(ref.pAttachments[i], res.pAttachments[i]);
        }
    } else {
        EXPECT_EQ(nullptr, res.pAttachments);
    }
    EXPECT_EQ(ref.subpassCount, res.subpassCount);
    if (ref.subpassCount) {
        for (uint32_t i = 0; i < ref.subpassCount; ++i) {
            CompareStruct(ref.pSubpasses[i], res.pSubpasses[i]);
        }
    } else {
        EXPECT_EQ(nullptr, res.pSubpasses);
    }
    EXPECT_EQ(ref.dependencyCount, res.dependencyCount);
    if (ref.dependencyCount) {
        for (uint32_t i = 0; i < ref.dependencyCount; ++i) {
            CompareStruct(ref.pDependencies[i], res.pDependencies[i]);
        }
    } else {
        EXPECT_EQ(nullptr, res.pDependencies);
    }
    if (ref.correlatedViewMaskCount) {
        for (uint32_t i = 0; i < ref.correlatedViewMaskCount; ++i) {
            EXPECT_EQ(ref.pCorrelatedViewMasks[i], res.pCorrelatedViewMasks[i]);
        }
    } else {
        EXPECT_EQ(nullptr, res.pCorrelatedViewMasks);
    }
}

void CompareStruct(const vku::safe_VkShaderModuleCreateInfo& ref, const VkShaderModuleCreateInfo& res) {
    EXPECT_EQ(ref.sType, res.sType);
    for (const VkBaseInStructure *ref_pNext = reinterpret_cast<const VkBaseInStructure*>(ref.pNext),
                                 *res_pNext = reinterpret_cast<const VkBaseInStructure*>(res.pNext);
         ref_pNext != nullptr; ref_pNext = ref_pNext->pNext, res_pNext = res_pNext->pNext) {
        switch (ref_pNext->sType) {
            default:
                FAIL() << "Unkown sType found in reference struct";
                break;
        }
    }
    EXPECT_EQ(ref.flags, res.flags);
    EXPECT_EQ(ref.codeSize, res.codeSize);
    if (ref.codeSize) {
        for (uint32_t i = 0; i < ref.codeSize; ++i) {
            EXPECT_EQ(reinterpret_cast<const uint8_t*>(ref.pCode)[i], reinterpret_cast<const uint8_t*>(res.pCode)[i]);
        }
    } else {
        EXPECT_EQ(nullptr, res.pCode);
    }
}

void CompareStruct(const vku::safe_VkPipelineCacheCreateInfo& ref, const VkPipelineCacheCreateInfo& res) {
    EXPECT_EQ(ref.sType, res.sType);
    for (const VkBaseInStructure *ref_pNext = reinterpret_cast<const VkBaseInStructure*>(ref.pNext),
                                 *res_pNext = reinterpret_cast<const VkBaseInStructure*>(res.pNext);
         ref_pNext != nullptr; ref_pNext = ref_pNext->pNext, res_pNext = res_pNext->pNext) {
        switch (ref_pNext->sType) {
            default:
                FAIL() << "Unkown sType found in reference struct";
                break;
        }
    }
    EXPECT_EQ(ref.flags, res.flags);
    EXPECT_EQ(ref.initialDataSize, res.initialDataSize);
    if (ref.initialDataSize) {
        for (uint32_t i = 0; i < ref.initialDataSize; ++i) {
            EXPECT_EQ(reinterpret_cast<const uint8_t*>(ref.pInitialData)[i], reinterpret_cast<const uint8_t*>(res.pInitialData)[i]);
        }
    } else {
        EXPECT_EQ(nullptr, res.pInitialData);
    }
}

void CompareStruct(const vku::safe_VkPipelinePoolSize& ref, const VkPipelinePoolSize& res) {
    EXPECT_EQ(ref.sType, res.sType);
    for (const VkBaseInStructure *ref_pNext = reinterpret_cast<const VkBaseInStructure*>(ref.pNext),
                                 *res_pNext = reinterpret_cast<const VkBaseInStructure*>(res.pNext);
         ref_pNext != nullptr; ref_pNext = ref_pNext->pNext, res_pNext = res_pNext->pNext) {
        switch (ref_pNext->sType) {
            default:
                FAIL() << "Unkown sType found in reference struct";
                break;
        }
    }
    EXPECT_EQ(ref.poolEntryCount, res.poolEntryCount);
    EXPECT_EQ(ref.poolEntrySize, res.poolEntrySize);
}

void CompareStruct(const vku::safe_VkDeviceObjectReservationCreateInfo& ref, const VkDeviceObjectReservationCreateInfo& res) {
    EXPECT_EQ(ref.sType, res.sType);
    for (const VkBaseInStructure *ref_pNext = reinterpret_cast<const VkBaseInStructure*>(ref.pNext),
                                 *res_pNext = reinterpret_cast<const VkBaseInStructure*>(res.pNext);
         ref_pNext != nullptr; ref_pNext = ref_pNext->pNext, res_pNext = res_pNext->pNext) {
        switch (ref_pNext->sType) {
            default:
                FAIL() << "Unkown sType found in reference struct";
                break;
        }
    }
    EXPECT_EQ(ref.pipelineCacheCreateInfoCount, res.pipelineCacheCreateInfoCount);
    if (ref.pipelineCacheCreateInfoCount) {
        for (uint32_t i = 0; i < ref.pipelineCacheCreateInfoCount; ++i) {
            CompareStruct(ref.pPipelineCacheCreateInfos[i], res.pPipelineCacheCreateInfos[i]);
        }
    } else {
        EXPECT_EQ(nullptr, res.pPipelineCacheCreateInfos);
    }
    EXPECT_EQ(ref.pipelinePoolSizeCount, res.pipelinePoolSizeCount);
    if (ref.pipelinePoolSizeCount) {
        for (uint32_t i = 0; i < ref.pipelinePoolSizeCount; ++i) {
            CompareStruct(ref.pPipelinePoolSizes[i], res.pPipelinePoolSizes[i]);
        }
    } else {
        EXPECT_EQ(nullptr, res.pPipelinePoolSizes);
    }
    EXPECT_EQ(ref.semaphoreRequestCount, res.semaphoreRequestCount);
    EXPECT_EQ(ref.commandBufferRequestCount, res.commandBufferRequestCount);
    EXPECT_EQ(ref.fenceRequestCount, res.fenceRequestCount);
    EXPECT_EQ(ref.deviceMemoryRequestCount, res.deviceMemoryRequestCount);
    EXPECT_EQ(ref.bufferRequestCount, res.bufferRequestCount);
    EXPECT_EQ(ref.imageRequestCount, res.imageRequestCount);
    EXPECT_EQ(ref.eventRequestCount, res.eventRequestCount);
    EXPECT_EQ(ref.queryPoolRequestCount, res.queryPoolRequestCount);
    EXPECT_EQ(ref.bufferViewRequestCount, res.bufferViewRequestCount);
    EXPECT_EQ(ref.imageViewRequestCount, res.imageViewRequestCount);
    EXPECT_EQ(ref.layeredImageViewRequestCount, res.layeredImageViewRequestCount);
    EXPECT_EQ(ref.pipelineCacheRequestCount, res.pipelineCacheRequestCount);
    EXPECT_EQ(ref.pipelineLayoutRequestCount, res.pipelineLayoutRequestCount);
    EXPECT_EQ(ref.renderPassRequestCount, res.renderPassRequestCount);
    EXPECT_EQ(ref.graphicsPipelineRequestCount, res.graphicsPipelineRequestCount);
    EXPECT_EQ(ref.computePipelineRequestCount, res.computePipelineRequestCount);
    EXPECT_EQ(ref.descriptorSetLayoutRequestCount, res.descriptorSetLayoutRequestCount);
    EXPECT_EQ(ref.samplerRequestCount, res.samplerRequestCount);
    EXPECT_EQ(ref.descriptorPoolRequestCount, res.descriptorPoolRequestCount);
    EXPECT_EQ(ref.descriptorSetRequestCount, res.descriptorSetRequestCount);
    EXPECT_EQ(ref.framebufferRequestCount, res.framebufferRequestCount);
    EXPECT_EQ(ref.commandPoolRequestCount, res.commandPoolRequestCount);
    EXPECT_EQ(ref.samplerYcbcrConversionRequestCount, res.samplerYcbcrConversionRequestCount);
    EXPECT_EQ(ref.surfaceRequestCount, res.surfaceRequestCount);
    EXPECT_EQ(ref.swapchainRequestCount, res.swapchainRequestCount);
    EXPECT_EQ(ref.displayModeRequestCount, res.displayModeRequestCount);
    EXPECT_EQ(ref.subpassDescriptionRequestCount, res.subpassDescriptionRequestCount);
    EXPECT_EQ(ref.attachmentDescriptionRequestCount, res.attachmentDescriptionRequestCount);
    EXPECT_EQ(ref.descriptorSetLayoutBindingRequestCount, res.descriptorSetLayoutBindingRequestCount);
    EXPECT_EQ(ref.descriptorSetLayoutBindingLimit, res.descriptorSetLayoutBindingLimit);
    EXPECT_EQ(ref.maxImageViewMipLevels, res.maxImageViewMipLevels);
    EXPECT_EQ(ref.maxImageViewArrayLayers, res.maxImageViewArrayLayers);
    EXPECT_EQ(ref.maxLayeredImageViewMipLevels, res.maxLayeredImageViewMipLevels);
    EXPECT_EQ(ref.maxOcclusionQueriesPerPool, res.maxOcclusionQueriesPerPool);
    EXPECT_EQ(ref.maxPipelineStatisticsQueriesPerPool, res.maxPipelineStatisticsQueriesPerPool);
    EXPECT_EQ(ref.maxTimestampQueriesPerPool, res.maxTimestampQueriesPerPool);
    EXPECT_EQ(ref.maxImmutableSamplersPerDescriptorSetLayout, res.maxImmutableSamplersPerDescriptorSetLayout);
}

void CompareStruct(const vku::safe_VkPipelineOfflineCreateInfo& ref, const VkPipelineOfflineCreateInfo& res) {
    EXPECT_EQ(ref.sType, res.sType);
    for (const VkBaseInStructure *ref_pNext = reinterpret_cast<const VkBaseInStructure*>(ref.pNext),
                                 *res_pNext = reinterpret_cast<const VkBaseInStructure*>(res.pNext);
         ref_pNext != nullptr; ref_pNext = ref_pNext->pNext, res_pNext = res_pNext->pNext) {
        switch (ref_pNext->sType) {
            default:
                FAIL() << "Unkown sType found in reference struct";
                break;
        }
    }
    for (uint32_t i = 0; i < VK_UUID_SIZE; ++i) {
        EXPECT_EQ(ref.pipelineIdentifier[i], res.pipelineIdentifier[i]);
    }
    EXPECT_EQ(ref.matchControl, res.matchControl);
    EXPECT_EQ(ref.poolEntrySize, res.poolEntrySize);
}

void CompareStruct(const vku::safe_VkPipelineShaderStageRequiredSubgroupSizeCreateInfo& ref,
                   const VkPipelineShaderStageRequiredSubgroupSizeCreateInfo& res) {
    EXPECT_EQ(ref.sType, res.sType);
    EXPECT_EQ(ref.requiredSubgroupSize, res.requiredSubgroupSize);
}

void CompareStruct(const vku::safe_VkPipelineShaderStageCreateInfo& ref, const VkPipelineShaderStageCreateInfo& res) {
    EXPECT_EQ(ref.sType, res.sType);
    for (const VkBaseInStructure *ref_pNext = reinterpret_cast<const VkBaseInStructure*>(ref.pNext),
                                 *res_pNext = reinterpret_cast<const VkBaseInStructure*>(res.pNext);
         ref_pNext != nullptr; ref_pNext = ref_pNext->pNext, res_pNext = res_pNext->pNext) {
        switch (ref_pNext->sType) {
            case VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_REQUIRED_SUBGROUP_SIZE_CREATE_INFO:
                CompareStruct(*reinterpret_cast<const vku::safe_VkPipelineShaderStageRequiredSubgroupSizeCreateInfo*>(ref_pNext),
                              *reinterpret_cast<const VkPipelineShaderStageRequiredSubgroupSizeCreateInfo*>(res_pNext));
                break;
            default:
                FAIL() << "Unkown sType found in reference struct";
                break;
        }
    }
    EXPECT_EQ(ref.flags, res.flags);
    EXPECT_EQ(ref.stage, res.stage);
    EXPECT_STREQ(ref.pName, res.pName);
    if (ref.pSpecializationInfo) {
        if (ref.pSpecializationInfo->mapEntryCount) {
            for (uint32_t i = 0; i < ref.pSpecializationInfo->mapEntryCount; ++i) {
                EXPECT_EQ(ref.pSpecializationInfo->pMapEntries[i].constantID, res.pSpecializationInfo->pMapEntries[i].constantID);
                EXPECT_EQ(ref.pSpecializationInfo->pMapEntries[i].constantID, res.pSpecializationInfo->pMapEntries[i].constantID);
                EXPECT_EQ(ref.pSpecializationInfo->pMapEntries[i].constantID, res.pSpecializationInfo->pMapEntries[i].constantID);
            }
        } else {
            EXPECT_EQ(nullptr, res.pSpecializationInfo->pMapEntries);
        }
        for (uint32_t i = 0; i < ref.pSpecializationInfo->dataSize; ++i) {
            EXPECT_EQ(reinterpret_cast<const uint8_t*>(ref.pSpecializationInfo->pData)[i],
                      reinterpret_cast<const uint8_t*>(res.pSpecializationInfo->pData)[i]);
        }
    } else {
        EXPECT_EQ(nullptr, res.pSpecializationInfo);
    }
}

void CompareStruct(const vku::safe_VkComputePipelineCreateInfo& ref, const VkComputePipelineCreateInfo& res) {
    EXPECT_EQ(ref.sType, res.sType);
    for (const VkBaseInStructure *ref_pNext = reinterpret_cast<const VkBaseInStructure*>(ref.pNext),
                                 *res_pNext = reinterpret_cast<const VkBaseInStructure*>(res.pNext);
         ref_pNext != nullptr; ref_pNext = ref_pNext->pNext, res_pNext = res_pNext->pNext) {
        switch (ref_pNext->sType) {
            default:
                FAIL() << "Unkown sType found in reference struct";
                break;
        }
    }
    EXPECT_EQ(ref.flags, res.flags);
    CompareStruct(ref.stage, res.stage);
    EXPECT_EQ(ref.layout, res.layout);
    EXPECT_EQ(ref.basePipelineHandle, res.basePipelineHandle);
    EXPECT_EQ(ref.basePipelineIndex, res.basePipelineIndex);
}

void CompareStruct(const vku::safe_VkPipelineDiscardRectangleStateCreateInfoEXT& ref,
                   const VkPipelineDiscardRectangleStateCreateInfoEXT& res) {
    EXPECT_EQ(ref.sType, res.sType);
    EXPECT_EQ(ref.flags, res.flags);
    EXPECT_EQ(ref.discardRectangleMode, res.discardRectangleMode);
    if (ref.discardRectangleCount) {
        for (uint32_t i = 0; i < ref.discardRectangleCount; ++i) {
            EXPECT_EQ(ref.pDiscardRectangles[i].offset.x, res.pDiscardRectangles[i].offset.x);
            EXPECT_EQ(ref.pDiscardRectangles[i].offset.y, res.pDiscardRectangles[i].offset.y);
            EXPECT_EQ(ref.pDiscardRectangles[i].extent.width, res.pDiscardRectangles[i].extent.width);
            EXPECT_EQ(ref.pDiscardRectangles[i].extent.height, res.pDiscardRectangles[i].extent.height);
        }
    } else {
        EXPECT_EQ(nullptr, res.pDiscardRectangles);
    }
}

void CompareStruct(const vku::safe_VkPipelineVertexInputStateCreateInfo& ref, const VkPipelineVertexInputStateCreateInfo& res) {
    EXPECT_EQ(ref.sType, res.sType);
    for (const VkBaseInStructure *ref_pNext = reinterpret_cast<const VkBaseInStructure*>(ref.pNext),
                                 *res_pNext = reinterpret_cast<const VkBaseInStructure*>(res.pNext);
         ref_pNext != nullptr; ref_pNext = ref_pNext->pNext, res_pNext = res_pNext->pNext) {
        switch (ref_pNext->sType) {
            default:
                FAIL() << "Unkown sType found in reference struct";
                break;
        }
    }
    EXPECT_EQ(ref.flags, res.flags);
    if (ref.vertexBindingDescriptionCount) {
        for (uint32_t i = 0; i < ref.vertexBindingDescriptionCount; ++i) {
            EXPECT_EQ(ref.pVertexBindingDescriptions[i].binding, res.pVertexBindingDescriptions[i].binding);
            EXPECT_EQ(ref.pVertexBindingDescriptions[i].stride, res.pVertexBindingDescriptions[i].stride);
            EXPECT_EQ(ref.pVertexBindingDescriptions[i].inputRate, res.pVertexBindingDescriptions[i].inputRate);
        }
    } else {
        EXPECT_EQ(nullptr, res.pVertexBindingDescriptions);
    }
    if (ref.vertexAttributeDescriptionCount) {
        for (uint32_t i = 0; i < ref.vertexAttributeDescriptionCount; ++i) {
            EXPECT_EQ(ref.pVertexAttributeDescriptions[i].location, res.pVertexAttributeDescriptions[i].location);
            EXPECT_EQ(ref.pVertexAttributeDescriptions[i].binding, res.pVertexAttributeDescriptions[i].binding);
            EXPECT_EQ(ref.pVertexAttributeDescriptions[i].format, res.pVertexAttributeDescriptions[i].format);
            EXPECT_EQ(ref.pVertexAttributeDescriptions[i].offset, res.pVertexAttributeDescriptions[i].offset);
        }
    } else {
        EXPECT_EQ(nullptr, res.pVertexAttributeDescriptions);
    }
}

void CompareStruct(const vku::safe_VkPipelineInputAssemblyStateCreateInfo& ref, const VkPipelineInputAssemblyStateCreateInfo& res) {
    EXPECT_EQ(ref.sType, res.sType);
    for (const VkBaseInStructure *ref_pNext = reinterpret_cast<const VkBaseInStructure*>(ref.pNext),
                                 *res_pNext = reinterpret_cast<const VkBaseInStructure*>(res.pNext);
         ref_pNext != nullptr; ref_pNext = ref_pNext->pNext, res_pNext = res_pNext->pNext) {
        switch (ref_pNext->sType) {
            default:
                FAIL() << "Unkown sType found in reference struct";
                break;
        }
    }
    EXPECT_EQ(ref.flags, res.flags);
    EXPECT_EQ(ref.topology, res.topology);
    EXPECT_EQ(ref.primitiveRestartEnable, res.primitiveRestartEnable);
}

void CompareStruct(const vku::safe_VkPipelineTessellationStateCreateInfo& ref, const VkPipelineTessellationStateCreateInfo& res) {
    EXPECT_EQ(ref.sType, res.sType);
    for (const VkBaseInStructure *ref_pNext = reinterpret_cast<const VkBaseInStructure*>(ref.pNext),
                                 *res_pNext = reinterpret_cast<const VkBaseInStructure*>(res.pNext);
         ref_pNext != nullptr; ref_pNext = ref_pNext->pNext, res_pNext = res_pNext->pNext) {
        switch (ref_pNext->sType) {
            default:
                FAIL() << "Unkown sType found in reference struct";
                break;
        }
    }
    EXPECT_EQ(ref.flags, res.flags);
    EXPECT_EQ(ref.patchControlPoints, res.patchControlPoints);
}

void CompareStruct(const vku::safe_VkPipelineViewportStateCreateInfo& ref, const VkPipelineViewportStateCreateInfo& res) {
    EXPECT_EQ(ref.sType, res.sType);
    for (const VkBaseInStructure *ref_pNext = reinterpret_cast<const VkBaseInStructure*>(ref.pNext),
                                 *res_pNext = reinterpret_cast<const VkBaseInStructure*>(res.pNext);
         ref_pNext != nullptr; ref_pNext = ref_pNext->pNext, res_pNext = res_pNext->pNext) {
        switch (ref_pNext->sType) {
            default:
                FAIL() << "Unkown sType found in reference struct";
                break;
        }
    }
    EXPECT_EQ(ref.flags, res.flags);
    if (ref.viewportCount) {
        for (uint32_t i = 0; i < ref.viewportCount; ++i) {
            EXPECT_EQ(ref.pViewports[i].x, res.pViewports[i].x);
            EXPECT_EQ(ref.pViewports[i].y, res.pViewports[i].y);
            EXPECT_EQ(ref.pViewports[i].width, res.pViewports[i].width);
            EXPECT_EQ(ref.pViewports[i].height, res.pViewports[i].height);
            EXPECT_EQ(ref.pViewports[i].minDepth, res.pViewports[i].minDepth);
            EXPECT_EQ(ref.pViewports[i].maxDepth, res.pViewports[i].maxDepth);
        }
    } else {
        EXPECT_EQ(nullptr, res.pViewports);
    }
    if (ref.scissorCount) {
        for (uint32_t i = 0; i < ref.scissorCount; ++i) {
            EXPECT_EQ(ref.pScissors[i].offset.x, res.pScissors[i].offset.x);
            EXPECT_EQ(ref.pScissors[i].offset.y, res.pScissors[i].offset.y);
            EXPECT_EQ(ref.pScissors[i].extent.width, res.pScissors[i].extent.width);
            EXPECT_EQ(ref.pScissors[i].extent.height, res.pScissors[i].extent.height);
        }
    } else {
        EXPECT_EQ(nullptr, res.pViewports);
    }
}

void CompareStruct(const vku::safe_VkPipelineRasterizationStateCreateInfo& ref, const VkPipelineRasterizationStateCreateInfo& res) {
    EXPECT_EQ(ref.sType, res.sType);
    for (const VkBaseInStructure *ref_pNext = reinterpret_cast<const VkBaseInStructure*>(ref.pNext),
                                 *res_pNext = reinterpret_cast<const VkBaseInStructure*>(res.pNext);
         ref_pNext != nullptr; ref_pNext = ref_pNext->pNext, res_pNext = res_pNext->pNext) {
        switch (ref_pNext->sType) {
            default:
                FAIL() << "Unkown sType found in reference struct";
                break;
        }
    }
    EXPECT_EQ(ref.flags, res.flags);
    EXPECT_EQ(ref.depthClampEnable, res.depthClampEnable);
    EXPECT_EQ(ref.rasterizerDiscardEnable, res.rasterizerDiscardEnable);
    EXPECT_EQ(ref.polygonMode, res.polygonMode);
    EXPECT_EQ(ref.cullMode, res.cullMode);
    EXPECT_EQ(ref.frontFace, res.frontFace);
    EXPECT_EQ(ref.depthBiasEnable, res.depthBiasEnable);
    EXPECT_EQ(ref.depthBiasConstantFactor, res.depthBiasConstantFactor);
    EXPECT_EQ(ref.depthBiasClamp, res.depthBiasClamp);
    EXPECT_EQ(ref.depthBiasSlopeFactor, res.depthBiasSlopeFactor);
    EXPECT_EQ(ref.lineWidth, res.lineWidth);
}

void CompareStruct(const vku::safe_VkPipelineMultisampleStateCreateInfo& ref, const VkPipelineMultisampleStateCreateInfo& res) {
    EXPECT_EQ(ref.sType, res.sType);
    for (const VkBaseInStructure *ref_pNext = reinterpret_cast<const VkBaseInStructure*>(ref.pNext),
                                 *res_pNext = reinterpret_cast<const VkBaseInStructure*>(res.pNext);
         ref_pNext != nullptr; ref_pNext = ref_pNext->pNext, res_pNext = res_pNext->pNext) {
        switch (ref_pNext->sType) {
            default:
                FAIL() << "Unkown sType found in reference struct";
                break;
        }
    }
    EXPECT_EQ(ref.flags, res.flags);
    EXPECT_EQ(ref.rasterizationSamples, res.rasterizationSamples);
    EXPECT_EQ(ref.sampleShadingEnable, res.sampleShadingEnable);
    EXPECT_EQ(ref.minSampleShading, res.minSampleShading);
    if (ref.pSampleMask) {
        EXPECT_EQ(*ref.pSampleMask, *res.pSampleMask);
    } else {
        EXPECT_EQ(nullptr, res.pSampleMask);
    }
    EXPECT_EQ(ref.alphaToCoverageEnable, res.alphaToCoverageEnable);
    EXPECT_EQ(ref.alphaToOneEnable, res.alphaToOneEnable);
}

void CompareStruct(const vku::safe_VkPipelineDepthStencilStateCreateInfo& ref, const VkPipelineDepthStencilStateCreateInfo& res) {
    EXPECT_EQ(ref.sType, res.sType);
    for (const VkBaseInStructure *ref_pNext = reinterpret_cast<const VkBaseInStructure*>(ref.pNext),
                                 *res_pNext = reinterpret_cast<const VkBaseInStructure*>(res.pNext);
         ref_pNext != nullptr; ref_pNext = ref_pNext->pNext, res_pNext = res_pNext->pNext) {
        switch (ref_pNext->sType) {
            default:
                FAIL() << "Unkown sType found in reference struct";
                break;
        }
    }
    EXPECT_EQ(ref.flags, res.flags);
    EXPECT_EQ(ref.depthTestEnable, res.depthTestEnable);
    EXPECT_EQ(ref.depthWriteEnable, res.depthWriteEnable);
    EXPECT_EQ(ref.depthCompareOp, res.depthCompareOp);
    EXPECT_EQ(ref.depthBoundsTestEnable, res.depthBoundsTestEnable);
    EXPECT_EQ(ref.stencilTestEnable, res.stencilTestEnable);
    EXPECT_EQ(ref.front.failOp, res.front.failOp);
    EXPECT_EQ(ref.front.passOp, res.front.passOp);
    EXPECT_EQ(ref.front.depthFailOp, res.front.depthFailOp);
    EXPECT_EQ(ref.front.compareOp, res.front.compareOp);
    EXPECT_EQ(ref.front.compareMask, res.front.compareMask);
    EXPECT_EQ(ref.front.writeMask, res.front.writeMask);
    EXPECT_EQ(ref.front.reference, res.front.reference);
    EXPECT_EQ(ref.back.failOp, res.back.failOp);
    EXPECT_EQ(ref.back.passOp, res.back.passOp);
    EXPECT_EQ(ref.back.depthFailOp, res.back.depthFailOp);
    EXPECT_EQ(ref.back.compareOp, res.back.compareOp);
    EXPECT_EQ(ref.back.compareMask, res.back.compareMask);
    EXPECT_EQ(ref.back.writeMask, res.back.writeMask);
    EXPECT_EQ(ref.back.reference, res.back.reference);
    EXPECT_EQ(ref.minDepthBounds, res.minDepthBounds);
    EXPECT_EQ(ref.maxDepthBounds, res.maxDepthBounds);
}

void CompareStruct(const vku::safe_VkPipelineColorBlendStateCreateInfo& ref, const VkPipelineColorBlendStateCreateInfo& res) {
    EXPECT_EQ(ref.sType, res.sType);
    for (const VkBaseInStructure *ref_pNext = reinterpret_cast<const VkBaseInStructure*>(ref.pNext),
                                 *res_pNext = reinterpret_cast<const VkBaseInStructure*>(res.pNext);
         ref_pNext != nullptr; ref_pNext = ref_pNext->pNext, res_pNext = res_pNext->pNext) {
        switch (ref_pNext->sType) {
            default:
                FAIL() << "Unkown sType found in reference struct";
                break;
        }
    }
    EXPECT_EQ(ref.flags, res.flags);
    EXPECT_EQ(ref.logicOpEnable, res.logicOpEnable);
    EXPECT_EQ(ref.logicOp, res.logicOp);
    if (ref.attachmentCount) {
        for (uint32_t i = 0; i < ref.attachmentCount; ++i) {
            EXPECT_EQ(ref.pAttachments[i].blendEnable, res.pAttachments[i].blendEnable);
            EXPECT_EQ(ref.pAttachments[i].srcColorBlendFactor, res.pAttachments[i].srcColorBlendFactor);
            EXPECT_EQ(ref.pAttachments[i].dstColorBlendFactor, res.pAttachments[i].dstColorBlendFactor);
            EXPECT_EQ(ref.pAttachments[i].colorBlendOp, res.pAttachments[i].colorBlendOp);
            EXPECT_EQ(ref.pAttachments[i].srcAlphaBlendFactor, res.pAttachments[i].srcAlphaBlendFactor);
            EXPECT_EQ(ref.pAttachments[i].dstAlphaBlendFactor, res.pAttachments[i].dstAlphaBlendFactor);
            EXPECT_EQ(ref.pAttachments[i].alphaBlendOp, res.pAttachments[i].alphaBlendOp);
            EXPECT_EQ(ref.pAttachments[i].colorWriteMask, res.pAttachments[i].colorWriteMask);
        }
    } else {
        EXPECT_EQ(nullptr, res.pAttachments);
    }
    for (uint32_t i = 0; i < 4; ++i) {
        EXPECT_EQ(ref.blendConstants[i], res.blendConstants[i]);
    }
}

void CompareStruct(const vku::safe_VkPipelineDynamicStateCreateInfo& ref, const VkPipelineDynamicStateCreateInfo& res) {
    EXPECT_EQ(ref.sType, res.sType);
    for (const VkBaseInStructure *ref_pNext = reinterpret_cast<const VkBaseInStructure*>(ref.pNext),
                                 *res_pNext = reinterpret_cast<const VkBaseInStructure*>(res.pNext);
         ref_pNext != nullptr; ref_pNext = ref_pNext->pNext, res_pNext = res_pNext->pNext) {
        switch (ref_pNext->sType) {
            default:
                FAIL() << "Unkown sType found in reference struct";
                break;
        }
    }
    EXPECT_EQ(ref.flags, res.flags);
    if (ref.dynamicStateCount) {
        for (uint32_t i = 0; i < ref.dynamicStateCount; ++i) {
            EXPECT_EQ(ref.pDynamicStates[i], res.pDynamicStates[i]);
        }
    } else {
        EXPECT_EQ(nullptr, res.pDynamicStates);
    }
}

void CompareStruct(const vku::safe_VkGraphicsPipelineCreateInfo& ref, const VkGraphicsPipelineCreateInfo& res) {
    EXPECT_EQ(ref.sType, res.sType);
    for (const VkBaseInStructure *ref_pNext = reinterpret_cast<const VkBaseInStructure*>(ref.pNext),
                                 *res_pNext = reinterpret_cast<const VkBaseInStructure*>(res.pNext);
         ref_pNext != nullptr; ref_pNext = ref_pNext->pNext, res_pNext = res_pNext->pNext) {
        switch (ref_pNext->sType) {
            case VK_STRUCTURE_TYPE_PIPELINE_DISCARD_RECTANGLE_STATE_CREATE_INFO_EXT:
                CompareStruct(*reinterpret_cast<const vku::safe_VkPipelineDiscardRectangleStateCreateInfoEXT*>(ref_pNext),
                              *reinterpret_cast<const VkPipelineDiscardRectangleStateCreateInfoEXT*>(res_pNext));
                break;
            default:
                FAIL() << "Unkown sType found in reference struct";
                break;
        }
    }
    EXPECT_EQ(ref.flags, res.flags);
    for (uint32_t i = 0; i < ref.stageCount; ++i) {
        CompareStruct(ref.pStages[i], res.pStages[i]);
    }
    if (ref.pVertexInputState) {
        CompareStruct(*ref.pVertexInputState, *res.pVertexInputState);
    } else {
        EXPECT_EQ(nullptr, res.pVertexInputState);
    }
    if (ref.pInputAssemblyState) {
        CompareStruct(*ref.pInputAssemblyState, *res.pInputAssemblyState);
    } else {
        EXPECT_EQ(nullptr, res.pInputAssemblyState);
    }
    if (ref.pTessellationState) {
        CompareStruct(*ref.pTessellationState, *res.pTessellationState);
    } else {
        EXPECT_EQ(nullptr, res.pTessellationState);
    }
    if (ref.pViewportState) {
        CompareStruct(*ref.pViewportState, *res.pViewportState);
    } else {
        EXPECT_EQ(nullptr, res.pViewportState);
    }
    if (ref.pRasterizationState) {
        CompareStruct(*ref.pRasterizationState, *res.pRasterizationState);
    } else {
        EXPECT_EQ(nullptr, res.pRasterizationState);
    }
    if (ref.pMultisampleState) {
        CompareStruct(*ref.pMultisampleState, *res.pMultisampleState);
    } else {
        EXPECT_EQ(nullptr, res.pMultisampleState);
    }
    if (ref.pDepthStencilState) {
        CompareStruct(*ref.pDepthStencilState, *res.pDepthStencilState);
    } else {
        EXPECT_EQ(nullptr, res.pDepthStencilState);
    }
    if (ref.pColorBlendState) {
        CompareStruct(*ref.pColorBlendState, *res.pColorBlendState);
    } else {
        EXPECT_EQ(nullptr, res.pColorBlendState);
    }
    if (ref.pDynamicState) {
        CompareStruct(*ref.pDynamicState, *res.pDynamicState);
    } else {
        EXPECT_EQ(nullptr, res.pDynamicState);
    }
    EXPECT_EQ(ref.basePipelineHandle, res.basePipelineHandle);
    EXPECT_EQ(ref.basePipelineIndex, res.basePipelineIndex);
}

void EXPECT_UUIDEQ(uint8_t* it1, uint8_t* it2) {
    for (unsigned int i = 0; i < VK_UUID_SIZE; ++i) {
        EXPECT_EQ(it1[i], it2[i]);
    }
}

template <typename UnionType>
void CompareData(VpjData ref, VpjData res) {
    if (ref.enabledExtensionCount) {
        for (uint32_t i = 0; i < ref.enabledExtensionCount; ++i) {
            EXPECT_STREQ(ref.ppEnabledExtensions[i], res.ppEnabledExtensions[i]);
        }
    } else {
        EXPECT_EQ(nullptr, res.ppEnabledExtensions);
    }
    if constexpr (std::is_same_v<UnionType, VpjComputePipelineState>) {
        CompareStruct(reinterpret_cast<const VkComputePipelineCreateInfo*>(ref.computePipelineState.pComputePipeline),
                      *reinterpret_cast<const VkComputePipelineCreateInfo*>(res.computePipelineState.pComputePipeline));
        if (ref.computePipelineState.immutableSamplerCount) {
            for (uint32_t i = 0; i < ref.computePipelineState.immutableSamplerCount; ++i) {
                CompareStruct(&reinterpret_cast<const VkSamplerCreateInfo*>(ref.computePipelineState.pImmutableSamplers)[i],
                              reinterpret_cast<const VkSamplerCreateInfo*>(res.computePipelineState.pImmutableSamplers)[i],
                              ref.computePipelineState.ppYcbcrSamplerNames);
                EXPECT_STREQ(ref.computePipelineState.ppImmutableSamplerNames[i],
                             res.computePipelineState.ppImmutableSamplerNames[i]);
            }
        } else {
            EXPECT_EQ(nullptr, res.computePipelineState.pImmutableSamplers);
        }
        if (ref.computePipelineState.ycbcrSamplerCount) {
            for (uint32_t i = 0; i < ref.computePipelineState.ycbcrSamplerCount; ++i) {
                CompareStruct(
                    &reinterpret_cast<const VkSamplerYcbcrConversionCreateInfo*>(ref.computePipelineState.pYcbcrSamplers)[i],
                    reinterpret_cast<const VkSamplerYcbcrConversionCreateInfo*>(res.computePipelineState.pYcbcrSamplers)[i]);
                EXPECT_STREQ(ref.computePipelineState.ppYcbcrSamplerNames[i], res.computePipelineState.ppYcbcrSamplerNames[i]);
            }
        } else {
            EXPECT_EQ(nullptr, res.computePipelineState.pYcbcrSamplers);
        }
        if (ref.computePipelineState.descriptorSetLayoutCount) {
            for (uint32_t i = 0; i < ref.computePipelineState.descriptorSetLayoutCount; ++i) {
                CompareStruct(
                    &reinterpret_cast<const VkDescriptorSetLayoutCreateInfo*>(ref.computePipelineState.pDescriptorSetLayouts)[i],
                    reinterpret_cast<const VkDescriptorSetLayoutCreateInfo*>(res.computePipelineState.pDescriptorSetLayouts)[i],
                    ref.computePipelineState.ppImmutableSamplerNames);
                EXPECT_STREQ(ref.computePipelineState.ppDescriptorSetLayoutNames[i],
                             res.computePipelineState.ppDescriptorSetLayoutNames[i]);
            }
        } else {
            EXPECT_EQ(nullptr, res.computePipelineState.pDescriptorSetLayouts);
        }
        for (uint32_t i = 0; i < ref.computePipelineState.descriptorSetLayoutCount; ++i) {
            EXPECT_EQ(ref.computePipelineState.pShaderFileNames[i].stage, res.computePipelineState.pShaderFileNames[i].stage);
            EXPECT_STREQ(ref.computePipelineState.pShaderFileNames[i].pFilename,
                         res.computePipelineState.pShaderFileNames[i].pFilename);
        }
        CompareStruct(reinterpret_cast<const VkPipelineLayoutCreateInfo*>(ref.computePipelineState.pPipelineLayout),
                      *reinterpret_cast<const VkPipelineLayoutCreateInfo*>(res.computePipelineState.pPipelineLayout),
                      ref.computePipelineState.ppDescriptorSetLayoutNames);
        if (ref.computePipelineState.pPhysicalDeviceFeatures) {
            CompareStruct(reinterpret_cast<const VkPhysicalDeviceFeatures2*>(ref.computePipelineState.pPhysicalDeviceFeatures),
                          *reinterpret_cast<const VkPhysicalDeviceFeatures2*>(res.computePipelineState.pPhysicalDeviceFeatures));
        } else {
            EXPECT_EQ(nullptr, res.computePipelineState.pPhysicalDeviceFeatures);
        }
    } else if constexpr (std::is_same_v<UnionType, VpjGraphicsPipelineState>) {
        CompareStruct(
            vku::safe_VkGraphicsPipelineCreateInfo{
                reinterpret_cast<const VkGraphicsPipelineCreateInfo*>(ref.graphicsPipelineState.pGraphicsPipeline), true, true},
            *reinterpret_cast<const VkGraphicsPipelineCreateInfo*>(res.graphicsPipelineState.pGraphicsPipeline));
        if (ref.graphicsPipelineState.immutableSamplerCount) {
            for (uint32_t i = 0; i < ref.graphicsPipelineState.immutableSamplerCount; ++i) {
                CompareStruct(&reinterpret_cast<const VkSamplerCreateInfo*>(ref.graphicsPipelineState.pImmutableSamplers)[i],
                              reinterpret_cast<const VkSamplerCreateInfo*>(res.graphicsPipelineState.pImmutableSamplers)[i],
                              ref.graphicsPipelineState.ppYcbcrSamplerNames);
                EXPECT_STREQ(ref.graphicsPipelineState.ppImmutableSamplerNames[i],
                             res.graphicsPipelineState.ppImmutableSamplerNames[i]);
            }
        } else {
            EXPECT_EQ(nullptr, res.graphicsPipelineState.pImmutableSamplers);
        }
        if (ref.graphicsPipelineState.ycbcrSamplerCount) {
            for (uint32_t i = 0; i < ref.graphicsPipelineState.ycbcrSamplerCount; ++i) {
                CompareStruct(
                    &reinterpret_cast<const VkSamplerYcbcrConversionCreateInfo*>(ref.graphicsPipelineState.pYcbcrSamplers)[i],
                    reinterpret_cast<const VkSamplerYcbcrConversionCreateInfo*>(res.graphicsPipelineState.pYcbcrSamplers)[i]);
                EXPECT_STREQ(ref.graphicsPipelineState.ppYcbcrSamplerNames[i], res.graphicsPipelineState.ppYcbcrSamplerNames[i]);
            }
        } else {
            EXPECT_EQ(nullptr, res.graphicsPipelineState.pYcbcrSamplers);
        }
        if (ref.graphicsPipelineState.descriptorSetLayoutCount) {
            for (uint32_t i = 0; i < ref.graphicsPipelineState.descriptorSetLayoutCount; ++i) {
                CompareStruct(
                    &reinterpret_cast<const VkDescriptorSetLayoutCreateInfo*>(ref.graphicsPipelineState.pDescriptorSetLayouts)[i],
                    reinterpret_cast<const VkDescriptorSetLayoutCreateInfo*>(res.graphicsPipelineState.pDescriptorSetLayouts)[i],
                    ref.graphicsPipelineState.ppImmutableSamplerNames);
                EXPECT_STREQ(ref.graphicsPipelineState.ppDescriptorSetLayoutNames[i],
                             res.graphicsPipelineState.ppDescriptorSetLayoutNames[i]);
            }
        } else {
            EXPECT_EQ(nullptr, res.graphicsPipelineState.pDescriptorSetLayouts);
        }
        CompareStruct(reinterpret_cast<const VkPipelineLayoutCreateInfo*>(ref.graphicsPipelineState.pPipelineLayout),
                      *reinterpret_cast<const VkPipelineLayoutCreateInfo*>(res.graphicsPipelineState.pPipelineLayout),
                      ref.graphicsPipelineState.ppDescriptorSetLayoutNames);
        for (uint32_t i = 0; i < ref.graphicsPipelineState.descriptorSetLayoutCount; ++i) {
            EXPECT_EQ(ref.graphicsPipelineState.pShaderFileNames[i].stage, res.graphicsPipelineState.pShaderFileNames[i].stage);
            EXPECT_STREQ(ref.graphicsPipelineState.pShaderFileNames[i].pFilename,
                         res.graphicsPipelineState.pShaderFileNames[i].pFilename);
        }
        if (ref.graphicsPipelineState.pPhysicalDeviceFeatures) {
            CompareStruct(reinterpret_cast<const VkPhysicalDeviceFeatures2*>(ref.graphicsPipelineState.pPhysicalDeviceFeatures),
                          *reinterpret_cast<const VkPhysicalDeviceFeatures2*>(res.graphicsPipelineState.pPhysicalDeviceFeatures));
        } else {
            EXPECT_EQ(nullptr, res.graphicsPipelineState.pPhysicalDeviceFeatures);
        }
        if (reinterpret_cast<const VkBaseInStructure*>(ref.graphicsPipelineState.pRenderPass)->sType ==
            VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO) {
            CompareStruct(reinterpret_cast<const VkRenderPassCreateInfo*>(ref.graphicsPipelineState.pRenderPass),
                          *reinterpret_cast<const VkRenderPassCreateInfo*>(res.graphicsPipelineState.pRenderPass));
        } else {
            CompareStruct(reinterpret_cast<const VkRenderPassCreateInfo2*>(ref.graphicsPipelineState.pRenderPass),
                          *reinterpret_cast<const VkRenderPassCreateInfo2*>(res.graphicsPipelineState.pRenderPass));
        }
    } else {
        FAIL() << "Invalid VpjData union type provided.";
    }
    EXPECT_UUIDEQ(ref.pipelineUUID, res.pipelineUUID);
}
