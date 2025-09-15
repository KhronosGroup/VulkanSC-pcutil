// *** THIS FILE IS GENERATED - DO NOT EDIT ***
// See json_gen_generator.py for modifications

/*
 * Copyright (c) 2024-2025 The Khronos Group Inc.
 * Copyright (c) 2024-2025 RasterGrid Kft.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
// NOLINTBEGIN
#include <json/json.h>
#include <vulkan/vulkan.h>

#include <string>
#include <sstream>
#include <algorithm>
#include <cmath>

#include "vksc_pipeline_json_base.hpp"

namespace pcjson {

class GeneratorBase : protected Base {
  private:
    Json::Value gen_VkShaderModule(const VkShaderModule v, const LocationScope&) {
        if (v == VK_NULL_HANDLE) {
            return "";
        } else {
            return uint64_t(v);
        }
    }

    Json::Value gen_VkPipelineLayout(const VkPipelineLayout v, const LocationScope&) {
        if (v == VK_NULL_HANDLE) {
            return "";
        } else {
            return uint64_t(v);
        }
    }

    Json::Value gen_VkRenderPass(const VkRenderPass v, const LocationScope&) {
        if (v == VK_NULL_HANDLE) {
            return "";
        } else {
            return uint64_t(v);
        }
    }

    Json::Value gen_VkPipeline(const VkPipeline v, const LocationScope&) {
        if (v == VK_NULL_HANDLE) {
            return "";
        } else {
            return uint64_t(v);
        }
    }

    Json::Value gen_VkSamplerYcbcrConversion(const VkSamplerYcbcrConversion v, const LocationScope&) {
        if (v == VK_NULL_HANDLE) {
            return "";
        } else {
            return uint64_t(v);
        }
    }

    Json::Value gen_VkSampler(const VkSampler v, const LocationScope&) {
        if (v == VK_NULL_HANDLE) {
            return "";
        } else {
            return uint64_t(v);
        }
    }

    Json::Value gen_VkDescriptorSetLayout(const VkDescriptorSetLayout v, const LocationScope&) {
        if (v == VK_NULL_HANDLE) {
            return "";
        } else {
            return uint64_t(v);
        }
    }

    const char* gen_VkPipelineCreateFlagBits_c_str(const VkPipelineCreateFlagBits v) {
        switch (v) {
            case VK_PIPELINE_CREATE_DISABLE_OPTIMIZATION_BIT:
                return "VK_PIPELINE_CREATE_DISABLE_OPTIMIZATION_BIT";
            case VK_PIPELINE_CREATE_VIEW_INDEX_FROM_DEVICE_INDEX_BIT:
                return "VK_PIPELINE_CREATE_VIEW_INDEX_FROM_DEVICE_INDEX_BIT";
            case VK_PIPELINE_CREATE_DISPATCH_BASE_BIT:
                return "VK_PIPELINE_CREATE_DISPATCH_BASE_BIT";
            case VK_PIPELINE_CREATE_FAIL_ON_PIPELINE_COMPILE_REQUIRED_BIT:
                return "VK_PIPELINE_CREATE_FAIL_ON_PIPELINE_COMPILE_REQUIRED_BIT";
            case VK_PIPELINE_CREATE_EARLY_RETURN_ON_FAILURE_BIT:
                return "VK_PIPELINE_CREATE_EARLY_RETURN_ON_FAILURE_BIT";
            case VK_PIPELINE_CREATE_NO_PROTECTED_ACCESS_BIT:
                return "VK_PIPELINE_CREATE_NO_PROTECTED_ACCESS_BIT";
            case VK_PIPELINE_CREATE_PROTECTED_ACCESS_ONLY_BIT:
                return "VK_PIPELINE_CREATE_PROTECTED_ACCESS_ONLY_BIT";
            default:
                break;
        }
        Error() << "Invalid bitmask value";
        return "Invalid bitmask value";
    }

    const char* gen_VkPipelineShaderStageCreateFlagBits_c_str(const VkPipelineShaderStageCreateFlagBits v) {
        switch (v) {
            case VK_PIPELINE_SHADER_STAGE_CREATE_ALLOW_VARYING_SUBGROUP_SIZE_BIT:
                return "VK_PIPELINE_SHADER_STAGE_CREATE_ALLOW_VARYING_SUBGROUP_SIZE_BIT";
            case VK_PIPELINE_SHADER_STAGE_CREATE_REQUIRE_FULL_SUBGROUPS_BIT:
                return "VK_PIPELINE_SHADER_STAGE_CREATE_REQUIRE_FULL_SUBGROUPS_BIT";
            default:
                break;
        }
        Error() << "Invalid bitmask value";
        return "Invalid bitmask value";
    }

    const char* gen_VkShaderStageFlagBits_c_str(const VkShaderStageFlagBits v) {
        switch (v) {
            case VK_SHADER_STAGE_VERTEX_BIT:
                return "VK_SHADER_STAGE_VERTEX_BIT";
            case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
                return "VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT";
            case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
                return "VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT";
            case VK_SHADER_STAGE_GEOMETRY_BIT:
                return "VK_SHADER_STAGE_GEOMETRY_BIT";
            case VK_SHADER_STAGE_FRAGMENT_BIT:
                return "VK_SHADER_STAGE_FRAGMENT_BIT";
            case VK_SHADER_STAGE_COMPUTE_BIT:
                return "VK_SHADER_STAGE_COMPUTE_BIT";
            case VK_SHADER_STAGE_ALL_GRAPHICS:
                return "VK_SHADER_STAGE_ALL_GRAPHICS";
            case VK_SHADER_STAGE_ALL:
                return "VK_SHADER_STAGE_ALL";
            default:
                break;
        }
        Error() << "Invalid bitmask value";
        return "Invalid bitmask value";
    }

    const char* gen_VkObjectType_c_str(const VkObjectType v) {
        switch (v) {
            case VK_OBJECT_TYPE_UNKNOWN:
                return "VK_OBJECT_TYPE_UNKNOWN";
            case VK_OBJECT_TYPE_INSTANCE:
                return "VK_OBJECT_TYPE_INSTANCE";
            case VK_OBJECT_TYPE_PHYSICAL_DEVICE:
                return "VK_OBJECT_TYPE_PHYSICAL_DEVICE";
            case VK_OBJECT_TYPE_DEVICE:
                return "VK_OBJECT_TYPE_DEVICE";
            case VK_OBJECT_TYPE_QUEUE:
                return "VK_OBJECT_TYPE_QUEUE";
            case VK_OBJECT_TYPE_SEMAPHORE:
                return "VK_OBJECT_TYPE_SEMAPHORE";
            case VK_OBJECT_TYPE_COMMAND_BUFFER:
                return "VK_OBJECT_TYPE_COMMAND_BUFFER";
            case VK_OBJECT_TYPE_FENCE:
                return "VK_OBJECT_TYPE_FENCE";
            case VK_OBJECT_TYPE_DEVICE_MEMORY:
                return "VK_OBJECT_TYPE_DEVICE_MEMORY";
            case VK_OBJECT_TYPE_BUFFER:
                return "VK_OBJECT_TYPE_BUFFER";
            case VK_OBJECT_TYPE_IMAGE:
                return "VK_OBJECT_TYPE_IMAGE";
            case VK_OBJECT_TYPE_EVENT:
                return "VK_OBJECT_TYPE_EVENT";
            case VK_OBJECT_TYPE_QUERY_POOL:
                return "VK_OBJECT_TYPE_QUERY_POOL";
            case VK_OBJECT_TYPE_BUFFER_VIEW:
                return "VK_OBJECT_TYPE_BUFFER_VIEW";
            case VK_OBJECT_TYPE_IMAGE_VIEW:
                return "VK_OBJECT_TYPE_IMAGE_VIEW";
            case VK_OBJECT_TYPE_SHADER_MODULE:
                return "VK_OBJECT_TYPE_SHADER_MODULE";
            case VK_OBJECT_TYPE_PIPELINE_CACHE:
                return "VK_OBJECT_TYPE_PIPELINE_CACHE";
            case VK_OBJECT_TYPE_PIPELINE_LAYOUT:
                return "VK_OBJECT_TYPE_PIPELINE_LAYOUT";
            case VK_OBJECT_TYPE_RENDER_PASS:
                return "VK_OBJECT_TYPE_RENDER_PASS";
            case VK_OBJECT_TYPE_PIPELINE:
                return "VK_OBJECT_TYPE_PIPELINE";
            case VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT:
                return "VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT";
            case VK_OBJECT_TYPE_SAMPLER:
                return "VK_OBJECT_TYPE_SAMPLER";
            case VK_OBJECT_TYPE_DESCRIPTOR_POOL:
                return "VK_OBJECT_TYPE_DESCRIPTOR_POOL";
            case VK_OBJECT_TYPE_DESCRIPTOR_SET:
                return "VK_OBJECT_TYPE_DESCRIPTOR_SET";
            case VK_OBJECT_TYPE_FRAMEBUFFER:
                return "VK_OBJECT_TYPE_FRAMEBUFFER";
            case VK_OBJECT_TYPE_COMMAND_POOL:
                return "VK_OBJECT_TYPE_COMMAND_POOL";
            case VK_OBJECT_TYPE_SAMPLER_YCBCR_CONVERSION:
                return "VK_OBJECT_TYPE_SAMPLER_YCBCR_CONVERSION";
            case VK_OBJECT_TYPE_PRIVATE_DATA_SLOT:
                return "VK_OBJECT_TYPE_PRIVATE_DATA_SLOT";
            case VK_OBJECT_TYPE_SURFACE_KHR:
                return "VK_OBJECT_TYPE_SURFACE_KHR";
            case VK_OBJECT_TYPE_SWAPCHAIN_KHR:
                return "VK_OBJECT_TYPE_SWAPCHAIN_KHR";
            case VK_OBJECT_TYPE_DISPLAY_KHR:
                return "VK_OBJECT_TYPE_DISPLAY_KHR";
            case VK_OBJECT_TYPE_DISPLAY_MODE_KHR:
                return "VK_OBJECT_TYPE_DISPLAY_MODE_KHR";
            case VK_OBJECT_TYPE_DEBUG_UTILS_MESSENGER_EXT:
                return "VK_OBJECT_TYPE_DEBUG_UTILS_MESSENGER_EXT";
            case VK_OBJECT_TYPE_SEMAPHORE_SCI_SYNC_POOL_NV:
                return "VK_OBJECT_TYPE_SEMAPHORE_SCI_SYNC_POOL_NV";
            default:
                break;
        }
        Warn() << "Invalid VkObjectType enum value";
        return nullptr;
    }

    const char* gen_VkPipelineRobustnessBufferBehavior_c_str(const VkPipelineRobustnessBufferBehavior v) {
        switch (v) {
            case VK_PIPELINE_ROBUSTNESS_BUFFER_BEHAVIOR_DEVICE_DEFAULT:
                return "VK_PIPELINE_ROBUSTNESS_BUFFER_BEHAVIOR_DEVICE_DEFAULT";
            case VK_PIPELINE_ROBUSTNESS_BUFFER_BEHAVIOR_DISABLED:
                return "VK_PIPELINE_ROBUSTNESS_BUFFER_BEHAVIOR_DISABLED";
            case VK_PIPELINE_ROBUSTNESS_BUFFER_BEHAVIOR_ROBUST_BUFFER_ACCESS:
                return "VK_PIPELINE_ROBUSTNESS_BUFFER_BEHAVIOR_ROBUST_BUFFER_ACCESS";
            case VK_PIPELINE_ROBUSTNESS_BUFFER_BEHAVIOR_ROBUST_BUFFER_ACCESS_2:
                return "VK_PIPELINE_ROBUSTNESS_BUFFER_BEHAVIOR_ROBUST_BUFFER_ACCESS_2";
            default:
                break;
        }
        Warn() << "Invalid VkPipelineRobustnessBufferBehavior enum value";
        return nullptr;
    }

    const char* gen_VkPipelineRobustnessImageBehavior_c_str(const VkPipelineRobustnessImageBehavior v) {
        switch (v) {
            case VK_PIPELINE_ROBUSTNESS_IMAGE_BEHAVIOR_DEVICE_DEFAULT:
                return "VK_PIPELINE_ROBUSTNESS_IMAGE_BEHAVIOR_DEVICE_DEFAULT";
            case VK_PIPELINE_ROBUSTNESS_IMAGE_BEHAVIOR_DISABLED:
                return "VK_PIPELINE_ROBUSTNESS_IMAGE_BEHAVIOR_DISABLED";
            case VK_PIPELINE_ROBUSTNESS_IMAGE_BEHAVIOR_ROBUST_IMAGE_ACCESS:
                return "VK_PIPELINE_ROBUSTNESS_IMAGE_BEHAVIOR_ROBUST_IMAGE_ACCESS";
            case VK_PIPELINE_ROBUSTNESS_IMAGE_BEHAVIOR_ROBUST_IMAGE_ACCESS_2:
                return "VK_PIPELINE_ROBUSTNESS_IMAGE_BEHAVIOR_ROBUST_IMAGE_ACCESS_2";
            default:
                break;
        }
        Warn() << "Invalid VkPipelineRobustnessImageBehavior enum value";
        return nullptr;
    }

    const char* gen_VkVertexInputRate_c_str(const VkVertexInputRate v) {
        switch (v) {
            case VK_VERTEX_INPUT_RATE_VERTEX:
                return "VK_VERTEX_INPUT_RATE_VERTEX";
            case VK_VERTEX_INPUT_RATE_INSTANCE:
                return "VK_VERTEX_INPUT_RATE_INSTANCE";
            default:
                break;
        }
        Warn() << "Invalid VkVertexInputRate enum value";
        return nullptr;
    }

    const char* gen_VkFormat_c_str(const VkFormat v) {
        switch (v) {
            case VK_FORMAT_UNDEFINED:
                return "VK_FORMAT_UNDEFINED";
            case VK_FORMAT_R4G4_UNORM_PACK8:
                return "VK_FORMAT_R4G4_UNORM_PACK8";
            case VK_FORMAT_R4G4B4A4_UNORM_PACK16:
                return "VK_FORMAT_R4G4B4A4_UNORM_PACK16";
            case VK_FORMAT_B4G4R4A4_UNORM_PACK16:
                return "VK_FORMAT_B4G4R4A4_UNORM_PACK16";
            case VK_FORMAT_R5G6B5_UNORM_PACK16:
                return "VK_FORMAT_R5G6B5_UNORM_PACK16";
            case VK_FORMAT_B5G6R5_UNORM_PACK16:
                return "VK_FORMAT_B5G6R5_UNORM_PACK16";
            case VK_FORMAT_R5G5B5A1_UNORM_PACK16:
                return "VK_FORMAT_R5G5B5A1_UNORM_PACK16";
            case VK_FORMAT_B5G5R5A1_UNORM_PACK16:
                return "VK_FORMAT_B5G5R5A1_UNORM_PACK16";
            case VK_FORMAT_A1R5G5B5_UNORM_PACK16:
                return "VK_FORMAT_A1R5G5B5_UNORM_PACK16";
            case VK_FORMAT_R8_UNORM:
                return "VK_FORMAT_R8_UNORM";
            case VK_FORMAT_R8_SNORM:
                return "VK_FORMAT_R8_SNORM";
            case VK_FORMAT_R8_USCALED:
                return "VK_FORMAT_R8_USCALED";
            case VK_FORMAT_R8_SSCALED:
                return "VK_FORMAT_R8_SSCALED";
            case VK_FORMAT_R8_UINT:
                return "VK_FORMAT_R8_UINT";
            case VK_FORMAT_R8_SINT:
                return "VK_FORMAT_R8_SINT";
            case VK_FORMAT_R8_SRGB:
                return "VK_FORMAT_R8_SRGB";
            case VK_FORMAT_R8G8_UNORM:
                return "VK_FORMAT_R8G8_UNORM";
            case VK_FORMAT_R8G8_SNORM:
                return "VK_FORMAT_R8G8_SNORM";
            case VK_FORMAT_R8G8_USCALED:
                return "VK_FORMAT_R8G8_USCALED";
            case VK_FORMAT_R8G8_SSCALED:
                return "VK_FORMAT_R8G8_SSCALED";
            case VK_FORMAT_R8G8_UINT:
                return "VK_FORMAT_R8G8_UINT";
            case VK_FORMAT_R8G8_SINT:
                return "VK_FORMAT_R8G8_SINT";
            case VK_FORMAT_R8G8_SRGB:
                return "VK_FORMAT_R8G8_SRGB";
            case VK_FORMAT_R8G8B8_UNORM:
                return "VK_FORMAT_R8G8B8_UNORM";
            case VK_FORMAT_R8G8B8_SNORM:
                return "VK_FORMAT_R8G8B8_SNORM";
            case VK_FORMAT_R8G8B8_USCALED:
                return "VK_FORMAT_R8G8B8_USCALED";
            case VK_FORMAT_R8G8B8_SSCALED:
                return "VK_FORMAT_R8G8B8_SSCALED";
            case VK_FORMAT_R8G8B8_UINT:
                return "VK_FORMAT_R8G8B8_UINT";
            case VK_FORMAT_R8G8B8_SINT:
                return "VK_FORMAT_R8G8B8_SINT";
            case VK_FORMAT_R8G8B8_SRGB:
                return "VK_FORMAT_R8G8B8_SRGB";
            case VK_FORMAT_B8G8R8_UNORM:
                return "VK_FORMAT_B8G8R8_UNORM";
            case VK_FORMAT_B8G8R8_SNORM:
                return "VK_FORMAT_B8G8R8_SNORM";
            case VK_FORMAT_B8G8R8_USCALED:
                return "VK_FORMAT_B8G8R8_USCALED";
            case VK_FORMAT_B8G8R8_SSCALED:
                return "VK_FORMAT_B8G8R8_SSCALED";
            case VK_FORMAT_B8G8R8_UINT:
                return "VK_FORMAT_B8G8R8_UINT";
            case VK_FORMAT_B8G8R8_SINT:
                return "VK_FORMAT_B8G8R8_SINT";
            case VK_FORMAT_B8G8R8_SRGB:
                return "VK_FORMAT_B8G8R8_SRGB";
            case VK_FORMAT_R8G8B8A8_UNORM:
                return "VK_FORMAT_R8G8B8A8_UNORM";
            case VK_FORMAT_R8G8B8A8_SNORM:
                return "VK_FORMAT_R8G8B8A8_SNORM";
            case VK_FORMAT_R8G8B8A8_USCALED:
                return "VK_FORMAT_R8G8B8A8_USCALED";
            case VK_FORMAT_R8G8B8A8_SSCALED:
                return "VK_FORMAT_R8G8B8A8_SSCALED";
            case VK_FORMAT_R8G8B8A8_UINT:
                return "VK_FORMAT_R8G8B8A8_UINT";
            case VK_FORMAT_R8G8B8A8_SINT:
                return "VK_FORMAT_R8G8B8A8_SINT";
            case VK_FORMAT_R8G8B8A8_SRGB:
                return "VK_FORMAT_R8G8B8A8_SRGB";
            case VK_FORMAT_B8G8R8A8_UNORM:
                return "VK_FORMAT_B8G8R8A8_UNORM";
            case VK_FORMAT_B8G8R8A8_SNORM:
                return "VK_FORMAT_B8G8R8A8_SNORM";
            case VK_FORMAT_B8G8R8A8_USCALED:
                return "VK_FORMAT_B8G8R8A8_USCALED";
            case VK_FORMAT_B8G8R8A8_SSCALED:
                return "VK_FORMAT_B8G8R8A8_SSCALED";
            case VK_FORMAT_B8G8R8A8_UINT:
                return "VK_FORMAT_B8G8R8A8_UINT";
            case VK_FORMAT_B8G8R8A8_SINT:
                return "VK_FORMAT_B8G8R8A8_SINT";
            case VK_FORMAT_B8G8R8A8_SRGB:
                return "VK_FORMAT_B8G8R8A8_SRGB";
            case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
                return "VK_FORMAT_A8B8G8R8_UNORM_PACK32";
            case VK_FORMAT_A8B8G8R8_SNORM_PACK32:
                return "VK_FORMAT_A8B8G8R8_SNORM_PACK32";
            case VK_FORMAT_A8B8G8R8_USCALED_PACK32:
                return "VK_FORMAT_A8B8G8R8_USCALED_PACK32";
            case VK_FORMAT_A8B8G8R8_SSCALED_PACK32:
                return "VK_FORMAT_A8B8G8R8_SSCALED_PACK32";
            case VK_FORMAT_A8B8G8R8_UINT_PACK32:
                return "VK_FORMAT_A8B8G8R8_UINT_PACK32";
            case VK_FORMAT_A8B8G8R8_SINT_PACK32:
                return "VK_FORMAT_A8B8G8R8_SINT_PACK32";
            case VK_FORMAT_A8B8G8R8_SRGB_PACK32:
                return "VK_FORMAT_A8B8G8R8_SRGB_PACK32";
            case VK_FORMAT_A2R10G10B10_UNORM_PACK32:
                return "VK_FORMAT_A2R10G10B10_UNORM_PACK32";
            case VK_FORMAT_A2R10G10B10_SNORM_PACK32:
                return "VK_FORMAT_A2R10G10B10_SNORM_PACK32";
            case VK_FORMAT_A2R10G10B10_USCALED_PACK32:
                return "VK_FORMAT_A2R10G10B10_USCALED_PACK32";
            case VK_FORMAT_A2R10G10B10_SSCALED_PACK32:
                return "VK_FORMAT_A2R10G10B10_SSCALED_PACK32";
            case VK_FORMAT_A2R10G10B10_UINT_PACK32:
                return "VK_FORMAT_A2R10G10B10_UINT_PACK32";
            case VK_FORMAT_A2R10G10B10_SINT_PACK32:
                return "VK_FORMAT_A2R10G10B10_SINT_PACK32";
            case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
                return "VK_FORMAT_A2B10G10R10_UNORM_PACK32";
            case VK_FORMAT_A2B10G10R10_SNORM_PACK32:
                return "VK_FORMAT_A2B10G10R10_SNORM_PACK32";
            case VK_FORMAT_A2B10G10R10_USCALED_PACK32:
                return "VK_FORMAT_A2B10G10R10_USCALED_PACK32";
            case VK_FORMAT_A2B10G10R10_SSCALED_PACK32:
                return "VK_FORMAT_A2B10G10R10_SSCALED_PACK32";
            case VK_FORMAT_A2B10G10R10_UINT_PACK32:
                return "VK_FORMAT_A2B10G10R10_UINT_PACK32";
            case VK_FORMAT_A2B10G10R10_SINT_PACK32:
                return "VK_FORMAT_A2B10G10R10_SINT_PACK32";
            case VK_FORMAT_R16_UNORM:
                return "VK_FORMAT_R16_UNORM";
            case VK_FORMAT_R16_SNORM:
                return "VK_FORMAT_R16_SNORM";
            case VK_FORMAT_R16_USCALED:
                return "VK_FORMAT_R16_USCALED";
            case VK_FORMAT_R16_SSCALED:
                return "VK_FORMAT_R16_SSCALED";
            case VK_FORMAT_R16_UINT:
                return "VK_FORMAT_R16_UINT";
            case VK_FORMAT_R16_SINT:
                return "VK_FORMAT_R16_SINT";
            case VK_FORMAT_R16_SFLOAT:
                return "VK_FORMAT_R16_SFLOAT";
            case VK_FORMAT_R16G16_UNORM:
                return "VK_FORMAT_R16G16_UNORM";
            case VK_FORMAT_R16G16_SNORM:
                return "VK_FORMAT_R16G16_SNORM";
            case VK_FORMAT_R16G16_USCALED:
                return "VK_FORMAT_R16G16_USCALED";
            case VK_FORMAT_R16G16_SSCALED:
                return "VK_FORMAT_R16G16_SSCALED";
            case VK_FORMAT_R16G16_UINT:
                return "VK_FORMAT_R16G16_UINT";
            case VK_FORMAT_R16G16_SINT:
                return "VK_FORMAT_R16G16_SINT";
            case VK_FORMAT_R16G16_SFLOAT:
                return "VK_FORMAT_R16G16_SFLOAT";
            case VK_FORMAT_R16G16B16_UNORM:
                return "VK_FORMAT_R16G16B16_UNORM";
            case VK_FORMAT_R16G16B16_SNORM:
                return "VK_FORMAT_R16G16B16_SNORM";
            case VK_FORMAT_R16G16B16_USCALED:
                return "VK_FORMAT_R16G16B16_USCALED";
            case VK_FORMAT_R16G16B16_SSCALED:
                return "VK_FORMAT_R16G16B16_SSCALED";
            case VK_FORMAT_R16G16B16_UINT:
                return "VK_FORMAT_R16G16B16_UINT";
            case VK_FORMAT_R16G16B16_SINT:
                return "VK_FORMAT_R16G16B16_SINT";
            case VK_FORMAT_R16G16B16_SFLOAT:
                return "VK_FORMAT_R16G16B16_SFLOAT";
            case VK_FORMAT_R16G16B16A16_UNORM:
                return "VK_FORMAT_R16G16B16A16_UNORM";
            case VK_FORMAT_R16G16B16A16_SNORM:
                return "VK_FORMAT_R16G16B16A16_SNORM";
            case VK_FORMAT_R16G16B16A16_USCALED:
                return "VK_FORMAT_R16G16B16A16_USCALED";
            case VK_FORMAT_R16G16B16A16_SSCALED:
                return "VK_FORMAT_R16G16B16A16_SSCALED";
            case VK_FORMAT_R16G16B16A16_UINT:
                return "VK_FORMAT_R16G16B16A16_UINT";
            case VK_FORMAT_R16G16B16A16_SINT:
                return "VK_FORMAT_R16G16B16A16_SINT";
            case VK_FORMAT_R16G16B16A16_SFLOAT:
                return "VK_FORMAT_R16G16B16A16_SFLOAT";
            case VK_FORMAT_R32_UINT:
                return "VK_FORMAT_R32_UINT";
            case VK_FORMAT_R32_SINT:
                return "VK_FORMAT_R32_SINT";
            case VK_FORMAT_R32_SFLOAT:
                return "VK_FORMAT_R32_SFLOAT";
            case VK_FORMAT_R32G32_UINT:
                return "VK_FORMAT_R32G32_UINT";
            case VK_FORMAT_R32G32_SINT:
                return "VK_FORMAT_R32G32_SINT";
            case VK_FORMAT_R32G32_SFLOAT:
                return "VK_FORMAT_R32G32_SFLOAT";
            case VK_FORMAT_R32G32B32_UINT:
                return "VK_FORMAT_R32G32B32_UINT";
            case VK_FORMAT_R32G32B32_SINT:
                return "VK_FORMAT_R32G32B32_SINT";
            case VK_FORMAT_R32G32B32_SFLOAT:
                return "VK_FORMAT_R32G32B32_SFLOAT";
            case VK_FORMAT_R32G32B32A32_UINT:
                return "VK_FORMAT_R32G32B32A32_UINT";
            case VK_FORMAT_R32G32B32A32_SINT:
                return "VK_FORMAT_R32G32B32A32_SINT";
            case VK_FORMAT_R32G32B32A32_SFLOAT:
                return "VK_FORMAT_R32G32B32A32_SFLOAT";
            case VK_FORMAT_R64_UINT:
                return "VK_FORMAT_R64_UINT";
            case VK_FORMAT_R64_SINT:
                return "VK_FORMAT_R64_SINT";
            case VK_FORMAT_R64_SFLOAT:
                return "VK_FORMAT_R64_SFLOAT";
            case VK_FORMAT_R64G64_UINT:
                return "VK_FORMAT_R64G64_UINT";
            case VK_FORMAT_R64G64_SINT:
                return "VK_FORMAT_R64G64_SINT";
            case VK_FORMAT_R64G64_SFLOAT:
                return "VK_FORMAT_R64G64_SFLOAT";
            case VK_FORMAT_R64G64B64_UINT:
                return "VK_FORMAT_R64G64B64_UINT";
            case VK_FORMAT_R64G64B64_SINT:
                return "VK_FORMAT_R64G64B64_SINT";
            case VK_FORMAT_R64G64B64_SFLOAT:
                return "VK_FORMAT_R64G64B64_SFLOAT";
            case VK_FORMAT_R64G64B64A64_UINT:
                return "VK_FORMAT_R64G64B64A64_UINT";
            case VK_FORMAT_R64G64B64A64_SINT:
                return "VK_FORMAT_R64G64B64A64_SINT";
            case VK_FORMAT_R64G64B64A64_SFLOAT:
                return "VK_FORMAT_R64G64B64A64_SFLOAT";
            case VK_FORMAT_B10G11R11_UFLOAT_PACK32:
                return "VK_FORMAT_B10G11R11_UFLOAT_PACK32";
            case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32:
                return "VK_FORMAT_E5B9G9R9_UFLOAT_PACK32";
            case VK_FORMAT_D16_UNORM:
                return "VK_FORMAT_D16_UNORM";
            case VK_FORMAT_X8_D24_UNORM_PACK32:
                return "VK_FORMAT_X8_D24_UNORM_PACK32";
            case VK_FORMAT_D32_SFLOAT:
                return "VK_FORMAT_D32_SFLOAT";
            case VK_FORMAT_S8_UINT:
                return "VK_FORMAT_S8_UINT";
            case VK_FORMAT_D16_UNORM_S8_UINT:
                return "VK_FORMAT_D16_UNORM_S8_UINT";
            case VK_FORMAT_D24_UNORM_S8_UINT:
                return "VK_FORMAT_D24_UNORM_S8_UINT";
            case VK_FORMAT_D32_SFLOAT_S8_UINT:
                return "VK_FORMAT_D32_SFLOAT_S8_UINT";
            case VK_FORMAT_BC1_RGB_UNORM_BLOCK:
                return "VK_FORMAT_BC1_RGB_UNORM_BLOCK";
            case VK_FORMAT_BC1_RGB_SRGB_BLOCK:
                return "VK_FORMAT_BC1_RGB_SRGB_BLOCK";
            case VK_FORMAT_BC1_RGBA_UNORM_BLOCK:
                return "VK_FORMAT_BC1_RGBA_UNORM_BLOCK";
            case VK_FORMAT_BC1_RGBA_SRGB_BLOCK:
                return "VK_FORMAT_BC1_RGBA_SRGB_BLOCK";
            case VK_FORMAT_BC2_UNORM_BLOCK:
                return "VK_FORMAT_BC2_UNORM_BLOCK";
            case VK_FORMAT_BC2_SRGB_BLOCK:
                return "VK_FORMAT_BC2_SRGB_BLOCK";
            case VK_FORMAT_BC3_UNORM_BLOCK:
                return "VK_FORMAT_BC3_UNORM_BLOCK";
            case VK_FORMAT_BC3_SRGB_BLOCK:
                return "VK_FORMAT_BC3_SRGB_BLOCK";
            case VK_FORMAT_BC4_UNORM_BLOCK:
                return "VK_FORMAT_BC4_UNORM_BLOCK";
            case VK_FORMAT_BC4_SNORM_BLOCK:
                return "VK_FORMAT_BC4_SNORM_BLOCK";
            case VK_FORMAT_BC5_UNORM_BLOCK:
                return "VK_FORMAT_BC5_UNORM_BLOCK";
            case VK_FORMAT_BC5_SNORM_BLOCK:
                return "VK_FORMAT_BC5_SNORM_BLOCK";
            case VK_FORMAT_BC6H_UFLOAT_BLOCK:
                return "VK_FORMAT_BC6H_UFLOAT_BLOCK";
            case VK_FORMAT_BC6H_SFLOAT_BLOCK:
                return "VK_FORMAT_BC6H_SFLOAT_BLOCK";
            case VK_FORMAT_BC7_UNORM_BLOCK:
                return "VK_FORMAT_BC7_UNORM_BLOCK";
            case VK_FORMAT_BC7_SRGB_BLOCK:
                return "VK_FORMAT_BC7_SRGB_BLOCK";
            case VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK:
                return "VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK";
            case VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK:
                return "VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK";
            case VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK:
                return "VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK";
            case VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK:
                return "VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK";
            case VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK:
                return "VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK";
            case VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK:
                return "VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK";
            case VK_FORMAT_EAC_R11_UNORM_BLOCK:
                return "VK_FORMAT_EAC_R11_UNORM_BLOCK";
            case VK_FORMAT_EAC_R11_SNORM_BLOCK:
                return "VK_FORMAT_EAC_R11_SNORM_BLOCK";
            case VK_FORMAT_EAC_R11G11_UNORM_BLOCK:
                return "VK_FORMAT_EAC_R11G11_UNORM_BLOCK";
            case VK_FORMAT_EAC_R11G11_SNORM_BLOCK:
                return "VK_FORMAT_EAC_R11G11_SNORM_BLOCK";
            case VK_FORMAT_ASTC_4x4_UNORM_BLOCK:
                return "VK_FORMAT_ASTC_4x4_UNORM_BLOCK";
            case VK_FORMAT_ASTC_4x4_SRGB_BLOCK:
                return "VK_FORMAT_ASTC_4x4_SRGB_BLOCK";
            case VK_FORMAT_ASTC_5x4_UNORM_BLOCK:
                return "VK_FORMAT_ASTC_5x4_UNORM_BLOCK";
            case VK_FORMAT_ASTC_5x4_SRGB_BLOCK:
                return "VK_FORMAT_ASTC_5x4_SRGB_BLOCK";
            case VK_FORMAT_ASTC_5x5_UNORM_BLOCK:
                return "VK_FORMAT_ASTC_5x5_UNORM_BLOCK";
            case VK_FORMAT_ASTC_5x5_SRGB_BLOCK:
                return "VK_FORMAT_ASTC_5x5_SRGB_BLOCK";
            case VK_FORMAT_ASTC_6x5_UNORM_BLOCK:
                return "VK_FORMAT_ASTC_6x5_UNORM_BLOCK";
            case VK_FORMAT_ASTC_6x5_SRGB_BLOCK:
                return "VK_FORMAT_ASTC_6x5_SRGB_BLOCK";
            case VK_FORMAT_ASTC_6x6_UNORM_BLOCK:
                return "VK_FORMAT_ASTC_6x6_UNORM_BLOCK";
            case VK_FORMAT_ASTC_6x6_SRGB_BLOCK:
                return "VK_FORMAT_ASTC_6x6_SRGB_BLOCK";
            case VK_FORMAT_ASTC_8x5_UNORM_BLOCK:
                return "VK_FORMAT_ASTC_8x5_UNORM_BLOCK";
            case VK_FORMAT_ASTC_8x5_SRGB_BLOCK:
                return "VK_FORMAT_ASTC_8x5_SRGB_BLOCK";
            case VK_FORMAT_ASTC_8x6_UNORM_BLOCK:
                return "VK_FORMAT_ASTC_8x6_UNORM_BLOCK";
            case VK_FORMAT_ASTC_8x6_SRGB_BLOCK:
                return "VK_FORMAT_ASTC_8x6_SRGB_BLOCK";
            case VK_FORMAT_ASTC_8x8_UNORM_BLOCK:
                return "VK_FORMAT_ASTC_8x8_UNORM_BLOCK";
            case VK_FORMAT_ASTC_8x8_SRGB_BLOCK:
                return "VK_FORMAT_ASTC_8x8_SRGB_BLOCK";
            case VK_FORMAT_ASTC_10x5_UNORM_BLOCK:
                return "VK_FORMAT_ASTC_10x5_UNORM_BLOCK";
            case VK_FORMAT_ASTC_10x5_SRGB_BLOCK:
                return "VK_FORMAT_ASTC_10x5_SRGB_BLOCK";
            case VK_FORMAT_ASTC_10x6_UNORM_BLOCK:
                return "VK_FORMAT_ASTC_10x6_UNORM_BLOCK";
            case VK_FORMAT_ASTC_10x6_SRGB_BLOCK:
                return "VK_FORMAT_ASTC_10x6_SRGB_BLOCK";
            case VK_FORMAT_ASTC_10x8_UNORM_BLOCK:
                return "VK_FORMAT_ASTC_10x8_UNORM_BLOCK";
            case VK_FORMAT_ASTC_10x8_SRGB_BLOCK:
                return "VK_FORMAT_ASTC_10x8_SRGB_BLOCK";
            case VK_FORMAT_ASTC_10x10_UNORM_BLOCK:
                return "VK_FORMAT_ASTC_10x10_UNORM_BLOCK";
            case VK_FORMAT_ASTC_10x10_SRGB_BLOCK:
                return "VK_FORMAT_ASTC_10x10_SRGB_BLOCK";
            case VK_FORMAT_ASTC_12x10_UNORM_BLOCK:
                return "VK_FORMAT_ASTC_12x10_UNORM_BLOCK";
            case VK_FORMAT_ASTC_12x10_SRGB_BLOCK:
                return "VK_FORMAT_ASTC_12x10_SRGB_BLOCK";
            case VK_FORMAT_ASTC_12x12_UNORM_BLOCK:
                return "VK_FORMAT_ASTC_12x12_UNORM_BLOCK";
            case VK_FORMAT_ASTC_12x12_SRGB_BLOCK:
                return "VK_FORMAT_ASTC_12x12_SRGB_BLOCK";
            case VK_FORMAT_G8B8G8R8_422_UNORM:
                return "VK_FORMAT_G8B8G8R8_422_UNORM";
            case VK_FORMAT_B8G8R8G8_422_UNORM:
                return "VK_FORMAT_B8G8R8G8_422_UNORM";
            case VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM:
                return "VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM";
            case VK_FORMAT_G8_B8R8_2PLANE_420_UNORM:
                return "VK_FORMAT_G8_B8R8_2PLANE_420_UNORM";
            case VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM:
                return "VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM";
            case VK_FORMAT_G8_B8R8_2PLANE_422_UNORM:
                return "VK_FORMAT_G8_B8R8_2PLANE_422_UNORM";
            case VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM:
                return "VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM";
            case VK_FORMAT_R10X6_UNORM_PACK16:
                return "VK_FORMAT_R10X6_UNORM_PACK16";
            case VK_FORMAT_R10X6G10X6_UNORM_2PACK16:
                return "VK_FORMAT_R10X6G10X6_UNORM_2PACK16";
            case VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16:
                return "VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16";
            case VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16:
                return "VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16";
            case VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16:
                return "VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16";
            case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16:
                return "VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16";
            case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16:
                return "VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16";
            case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16:
                return "VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16";
            case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16:
                return "VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16";
            case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16:
                return "VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16";
            case VK_FORMAT_R12X4_UNORM_PACK16:
                return "VK_FORMAT_R12X4_UNORM_PACK16";
            case VK_FORMAT_R12X4G12X4_UNORM_2PACK16:
                return "VK_FORMAT_R12X4G12X4_UNORM_2PACK16";
            case VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16:
                return "VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16";
            case VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16:
                return "VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16";
            case VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16:
                return "VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16";
            case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16:
                return "VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16";
            case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16:
                return "VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16";
            case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16:
                return "VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16";
            case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16:
                return "VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16";
            case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16:
                return "VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16";
            case VK_FORMAT_G16B16G16R16_422_UNORM:
                return "VK_FORMAT_G16B16G16R16_422_UNORM";
            case VK_FORMAT_B16G16R16G16_422_UNORM:
                return "VK_FORMAT_B16G16R16G16_422_UNORM";
            case VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM:
                return "VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM";
            case VK_FORMAT_G16_B16R16_2PLANE_420_UNORM:
                return "VK_FORMAT_G16_B16R16_2PLANE_420_UNORM";
            case VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM:
                return "VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM";
            case VK_FORMAT_G16_B16R16_2PLANE_422_UNORM:
                return "VK_FORMAT_G16_B16R16_2PLANE_422_UNORM";
            case VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM:
                return "VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM";
            case VK_FORMAT_G8_B8R8_2PLANE_444_UNORM:
                return "VK_FORMAT_G8_B8R8_2PLANE_444_UNORM";
            case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16:
                return "VK_FORMAT_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16";
            case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16:
                return "VK_FORMAT_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16";
            case VK_FORMAT_G16_B16R16_2PLANE_444_UNORM:
                return "VK_FORMAT_G16_B16R16_2PLANE_444_UNORM";
            case VK_FORMAT_A4R4G4B4_UNORM_PACK16:
                return "VK_FORMAT_A4R4G4B4_UNORM_PACK16";
            case VK_FORMAT_A4B4G4R4_UNORM_PACK16:
                return "VK_FORMAT_A4B4G4R4_UNORM_PACK16";
            case VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK:
                return "VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK";
            case VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK:
                return "VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK";
            case VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK:
                return "VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK";
            case VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK:
                return "VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK";
            case VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK:
                return "VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK";
            case VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK:
                return "VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK";
            case VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK:
                return "VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK";
            case VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK:
                return "VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK";
            case VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK:
                return "VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK";
            case VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK:
                return "VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK";
            case VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK:
                return "VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK";
            case VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK:
                return "VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK";
            case VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK:
                return "VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK";
            case VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK:
                return "VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK";
            case VK_FORMAT_A1B5G5R5_UNORM_PACK16:
                return "VK_FORMAT_A1B5G5R5_UNORM_PACK16";
            case VK_FORMAT_A8_UNORM:
                return "VK_FORMAT_A8_UNORM";
            default:
                break;
        }
        Warn() << "Invalid VkFormat enum value";
        return nullptr;
    }

    const char* gen_VkPrimitiveTopology_c_str(const VkPrimitiveTopology v) {
        switch (v) {
            case VK_PRIMITIVE_TOPOLOGY_POINT_LIST:
                return "VK_PRIMITIVE_TOPOLOGY_POINT_LIST";
            case VK_PRIMITIVE_TOPOLOGY_LINE_LIST:
                return "VK_PRIMITIVE_TOPOLOGY_LINE_LIST";
            case VK_PRIMITIVE_TOPOLOGY_LINE_STRIP:
                return "VK_PRIMITIVE_TOPOLOGY_LINE_STRIP";
            case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST:
                return "VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST";
            case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP:
                return "VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP";
            case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN:
                return "VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN";
            case VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY:
                return "VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY";
            case VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY:
                return "VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY";
            case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY:
                return "VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY";
            case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY:
                return "VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY";
            case VK_PRIMITIVE_TOPOLOGY_PATCH_LIST:
                return "VK_PRIMITIVE_TOPOLOGY_PATCH_LIST";
            default:
                break;
        }
        Warn() << "Invalid VkPrimitiveTopology enum value";
        return nullptr;
    }

    const char* gen_VkTessellationDomainOrigin_c_str(const VkTessellationDomainOrigin v) {
        switch (v) {
            case VK_TESSELLATION_DOMAIN_ORIGIN_UPPER_LEFT:
                return "VK_TESSELLATION_DOMAIN_ORIGIN_UPPER_LEFT";
            case VK_TESSELLATION_DOMAIN_ORIGIN_LOWER_LEFT:
                return "VK_TESSELLATION_DOMAIN_ORIGIN_LOWER_LEFT";
            default:
                break;
        }
        Warn() << "Invalid VkTessellationDomainOrigin enum value";
        return nullptr;
    }

    const char* gen_VkPolygonMode_c_str(const VkPolygonMode v) {
        switch (v) {
            case VK_POLYGON_MODE_FILL:
                return "VK_POLYGON_MODE_FILL";
            case VK_POLYGON_MODE_LINE:
                return "VK_POLYGON_MODE_LINE";
            case VK_POLYGON_MODE_POINT:
                return "VK_POLYGON_MODE_POINT";
            default:
                break;
        }
        Warn() << "Invalid VkPolygonMode enum value";
        return nullptr;
    }

    const char* gen_VkCullModeFlagBits_c_str(const VkCullModeFlagBits v) {
        switch (v) {
            case VK_CULL_MODE_NONE:
                return "VK_CULL_MODE_NONE";
            case VK_CULL_MODE_FRONT_BIT:
                return "VK_CULL_MODE_FRONT_BIT";
            case VK_CULL_MODE_BACK_BIT:
                return "VK_CULL_MODE_BACK_BIT";
            case VK_CULL_MODE_FRONT_AND_BACK:
                return "VK_CULL_MODE_FRONT_AND_BACK";
            default:
                break;
        }
        Error() << "Invalid bitmask value";
        return "Invalid bitmask value";
    }

    const char* gen_VkFrontFace_c_str(const VkFrontFace v) {
        switch (v) {
            case VK_FRONT_FACE_COUNTER_CLOCKWISE:
                return "VK_FRONT_FACE_COUNTER_CLOCKWISE";
            case VK_FRONT_FACE_CLOCKWISE:
                return "VK_FRONT_FACE_CLOCKWISE";
            default:
                break;
        }
        Warn() << "Invalid VkFrontFace enum value";
        return nullptr;
    }

    const char* gen_VkConservativeRasterizationModeEXT_c_str(const VkConservativeRasterizationModeEXT v) {
        switch (v) {
            case VK_CONSERVATIVE_RASTERIZATION_MODE_DISABLED_EXT:
                return "VK_CONSERVATIVE_RASTERIZATION_MODE_DISABLED_EXT";
            case VK_CONSERVATIVE_RASTERIZATION_MODE_OVERESTIMATE_EXT:
                return "VK_CONSERVATIVE_RASTERIZATION_MODE_OVERESTIMATE_EXT";
            case VK_CONSERVATIVE_RASTERIZATION_MODE_UNDERESTIMATE_EXT:
                return "VK_CONSERVATIVE_RASTERIZATION_MODE_UNDERESTIMATE_EXT";
            default:
                break;
        }
        Warn() << "Invalid VkConservativeRasterizationModeEXT enum value";
        return nullptr;
    }

    const char* gen_VkLineRasterizationMode_c_str(const VkLineRasterizationMode v) {
        switch (v) {
            case VK_LINE_RASTERIZATION_MODE_DEFAULT:
                return "VK_LINE_RASTERIZATION_MODE_DEFAULT";
            case VK_LINE_RASTERIZATION_MODE_RECTANGULAR:
                return "VK_LINE_RASTERIZATION_MODE_RECTANGULAR";
            case VK_LINE_RASTERIZATION_MODE_BRESENHAM:
                return "VK_LINE_RASTERIZATION_MODE_BRESENHAM";
            case VK_LINE_RASTERIZATION_MODE_RECTANGULAR_SMOOTH:
                return "VK_LINE_RASTERIZATION_MODE_RECTANGULAR_SMOOTH";
            default:
                break;
        }
        Warn() << "Invalid VkLineRasterizationMode enum value";
        return nullptr;
    }

    const char* gen_VkSampleCountFlagBits_c_str(const VkSampleCountFlagBits v) {
        switch (v) {
            case VK_SAMPLE_COUNT_1_BIT:
                return "VK_SAMPLE_COUNT_1_BIT";
            case VK_SAMPLE_COUNT_2_BIT:
                return "VK_SAMPLE_COUNT_2_BIT";
            case VK_SAMPLE_COUNT_4_BIT:
                return "VK_SAMPLE_COUNT_4_BIT";
            case VK_SAMPLE_COUNT_8_BIT:
                return "VK_SAMPLE_COUNT_8_BIT";
            case VK_SAMPLE_COUNT_16_BIT:
                return "VK_SAMPLE_COUNT_16_BIT";
            case VK_SAMPLE_COUNT_32_BIT:
                return "VK_SAMPLE_COUNT_32_BIT";
            case VK_SAMPLE_COUNT_64_BIT:
                return "VK_SAMPLE_COUNT_64_BIT";
            default:
                break;
        }
        Error() << "Invalid bitmask value";
        return "Invalid bitmask value";
    }

    const char* gen_VkCompareOp_c_str(const VkCompareOp v) {
        switch (v) {
            case VK_COMPARE_OP_NEVER:
                return "VK_COMPARE_OP_NEVER";
            case VK_COMPARE_OP_LESS:
                return "VK_COMPARE_OP_LESS";
            case VK_COMPARE_OP_EQUAL:
                return "VK_COMPARE_OP_EQUAL";
            case VK_COMPARE_OP_LESS_OR_EQUAL:
                return "VK_COMPARE_OP_LESS_OR_EQUAL";
            case VK_COMPARE_OP_GREATER:
                return "VK_COMPARE_OP_GREATER";
            case VK_COMPARE_OP_NOT_EQUAL:
                return "VK_COMPARE_OP_NOT_EQUAL";
            case VK_COMPARE_OP_GREATER_OR_EQUAL:
                return "VK_COMPARE_OP_GREATER_OR_EQUAL";
            case VK_COMPARE_OP_ALWAYS:
                return "VK_COMPARE_OP_ALWAYS";
            default:
                break;
        }
        Warn() << "Invalid VkCompareOp enum value";
        return nullptr;
    }

    const char* gen_VkStencilOp_c_str(const VkStencilOp v) {
        switch (v) {
            case VK_STENCIL_OP_KEEP:
                return "VK_STENCIL_OP_KEEP";
            case VK_STENCIL_OP_ZERO:
                return "VK_STENCIL_OP_ZERO";
            case VK_STENCIL_OP_REPLACE:
                return "VK_STENCIL_OP_REPLACE";
            case VK_STENCIL_OP_INCREMENT_AND_CLAMP:
                return "VK_STENCIL_OP_INCREMENT_AND_CLAMP";
            case VK_STENCIL_OP_DECREMENT_AND_CLAMP:
                return "VK_STENCIL_OP_DECREMENT_AND_CLAMP";
            case VK_STENCIL_OP_INVERT:
                return "VK_STENCIL_OP_INVERT";
            case VK_STENCIL_OP_INCREMENT_AND_WRAP:
                return "VK_STENCIL_OP_INCREMENT_AND_WRAP";
            case VK_STENCIL_OP_DECREMENT_AND_WRAP:
                return "VK_STENCIL_OP_DECREMENT_AND_WRAP";
            default:
                break;
        }
        Warn() << "Invalid VkStencilOp enum value";
        return nullptr;
    }

    const char* gen_VkLogicOp_c_str(const VkLogicOp v) {
        switch (v) {
            case VK_LOGIC_OP_CLEAR:
                return "VK_LOGIC_OP_CLEAR";
            case VK_LOGIC_OP_AND:
                return "VK_LOGIC_OP_AND";
            case VK_LOGIC_OP_AND_REVERSE:
                return "VK_LOGIC_OP_AND_REVERSE";
            case VK_LOGIC_OP_COPY:
                return "VK_LOGIC_OP_COPY";
            case VK_LOGIC_OP_AND_INVERTED:
                return "VK_LOGIC_OP_AND_INVERTED";
            case VK_LOGIC_OP_NO_OP:
                return "VK_LOGIC_OP_NO_OP";
            case VK_LOGIC_OP_XOR:
                return "VK_LOGIC_OP_XOR";
            case VK_LOGIC_OP_OR:
                return "VK_LOGIC_OP_OR";
            case VK_LOGIC_OP_NOR:
                return "VK_LOGIC_OP_NOR";
            case VK_LOGIC_OP_EQUIVALENT:
                return "VK_LOGIC_OP_EQUIVALENT";
            case VK_LOGIC_OP_INVERT:
                return "VK_LOGIC_OP_INVERT";
            case VK_LOGIC_OP_OR_REVERSE:
                return "VK_LOGIC_OP_OR_REVERSE";
            case VK_LOGIC_OP_COPY_INVERTED:
                return "VK_LOGIC_OP_COPY_INVERTED";
            case VK_LOGIC_OP_OR_INVERTED:
                return "VK_LOGIC_OP_OR_INVERTED";
            case VK_LOGIC_OP_NAND:
                return "VK_LOGIC_OP_NAND";
            case VK_LOGIC_OP_SET:
                return "VK_LOGIC_OP_SET";
            default:
                break;
        }
        Warn() << "Invalid VkLogicOp enum value";
        return nullptr;
    }

    const char* gen_VkBlendFactor_c_str(const VkBlendFactor v) {
        switch (v) {
            case VK_BLEND_FACTOR_ZERO:
                return "VK_BLEND_FACTOR_ZERO";
            case VK_BLEND_FACTOR_ONE:
                return "VK_BLEND_FACTOR_ONE";
            case VK_BLEND_FACTOR_SRC_COLOR:
                return "VK_BLEND_FACTOR_SRC_COLOR";
            case VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR:
                return "VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR";
            case VK_BLEND_FACTOR_DST_COLOR:
                return "VK_BLEND_FACTOR_DST_COLOR";
            case VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR:
                return "VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR";
            case VK_BLEND_FACTOR_SRC_ALPHA:
                return "VK_BLEND_FACTOR_SRC_ALPHA";
            case VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA:
                return "VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA";
            case VK_BLEND_FACTOR_DST_ALPHA:
                return "VK_BLEND_FACTOR_DST_ALPHA";
            case VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA:
                return "VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA";
            case VK_BLEND_FACTOR_CONSTANT_COLOR:
                return "VK_BLEND_FACTOR_CONSTANT_COLOR";
            case VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR:
                return "VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR";
            case VK_BLEND_FACTOR_CONSTANT_ALPHA:
                return "VK_BLEND_FACTOR_CONSTANT_ALPHA";
            case VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA:
                return "VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA";
            case VK_BLEND_FACTOR_SRC_ALPHA_SATURATE:
                return "VK_BLEND_FACTOR_SRC_ALPHA_SATURATE";
            case VK_BLEND_FACTOR_SRC1_COLOR:
                return "VK_BLEND_FACTOR_SRC1_COLOR";
            case VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR:
                return "VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR";
            case VK_BLEND_FACTOR_SRC1_ALPHA:
                return "VK_BLEND_FACTOR_SRC1_ALPHA";
            case VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA:
                return "VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA";
            default:
                break;
        }
        Warn() << "Invalid VkBlendFactor enum value";
        return nullptr;
    }

    const char* gen_VkBlendOp_c_str(const VkBlendOp v) {
        switch (v) {
            case VK_BLEND_OP_ADD:
                return "VK_BLEND_OP_ADD";
            case VK_BLEND_OP_SUBTRACT:
                return "VK_BLEND_OP_SUBTRACT";
            case VK_BLEND_OP_REVERSE_SUBTRACT:
                return "VK_BLEND_OP_REVERSE_SUBTRACT";
            case VK_BLEND_OP_MIN:
                return "VK_BLEND_OP_MIN";
            case VK_BLEND_OP_MAX:
                return "VK_BLEND_OP_MAX";
            case VK_BLEND_OP_ZERO_EXT:
                return "VK_BLEND_OP_ZERO_EXT";
            case VK_BLEND_OP_SRC_EXT:
                return "VK_BLEND_OP_SRC_EXT";
            case VK_BLEND_OP_DST_EXT:
                return "VK_BLEND_OP_DST_EXT";
            case VK_BLEND_OP_SRC_OVER_EXT:
                return "VK_BLEND_OP_SRC_OVER_EXT";
            case VK_BLEND_OP_DST_OVER_EXT:
                return "VK_BLEND_OP_DST_OVER_EXT";
            case VK_BLEND_OP_SRC_IN_EXT:
                return "VK_BLEND_OP_SRC_IN_EXT";
            case VK_BLEND_OP_DST_IN_EXT:
                return "VK_BLEND_OP_DST_IN_EXT";
            case VK_BLEND_OP_SRC_OUT_EXT:
                return "VK_BLEND_OP_SRC_OUT_EXT";
            case VK_BLEND_OP_DST_OUT_EXT:
                return "VK_BLEND_OP_DST_OUT_EXT";
            case VK_BLEND_OP_SRC_ATOP_EXT:
                return "VK_BLEND_OP_SRC_ATOP_EXT";
            case VK_BLEND_OP_DST_ATOP_EXT:
                return "VK_BLEND_OP_DST_ATOP_EXT";
            case VK_BLEND_OP_XOR_EXT:
                return "VK_BLEND_OP_XOR_EXT";
            case VK_BLEND_OP_MULTIPLY_EXT:
                return "VK_BLEND_OP_MULTIPLY_EXT";
            case VK_BLEND_OP_SCREEN_EXT:
                return "VK_BLEND_OP_SCREEN_EXT";
            case VK_BLEND_OP_OVERLAY_EXT:
                return "VK_BLEND_OP_OVERLAY_EXT";
            case VK_BLEND_OP_DARKEN_EXT:
                return "VK_BLEND_OP_DARKEN_EXT";
            case VK_BLEND_OP_LIGHTEN_EXT:
                return "VK_BLEND_OP_LIGHTEN_EXT";
            case VK_BLEND_OP_COLORDODGE_EXT:
                return "VK_BLEND_OP_COLORDODGE_EXT";
            case VK_BLEND_OP_COLORBURN_EXT:
                return "VK_BLEND_OP_COLORBURN_EXT";
            case VK_BLEND_OP_HARDLIGHT_EXT:
                return "VK_BLEND_OP_HARDLIGHT_EXT";
            case VK_BLEND_OP_SOFTLIGHT_EXT:
                return "VK_BLEND_OP_SOFTLIGHT_EXT";
            case VK_BLEND_OP_DIFFERENCE_EXT:
                return "VK_BLEND_OP_DIFFERENCE_EXT";
            case VK_BLEND_OP_EXCLUSION_EXT:
                return "VK_BLEND_OP_EXCLUSION_EXT";
            case VK_BLEND_OP_INVERT_EXT:
                return "VK_BLEND_OP_INVERT_EXT";
            case VK_BLEND_OP_INVERT_RGB_EXT:
                return "VK_BLEND_OP_INVERT_RGB_EXT";
            case VK_BLEND_OP_LINEARDODGE_EXT:
                return "VK_BLEND_OP_LINEARDODGE_EXT";
            case VK_BLEND_OP_LINEARBURN_EXT:
                return "VK_BLEND_OP_LINEARBURN_EXT";
            case VK_BLEND_OP_VIVIDLIGHT_EXT:
                return "VK_BLEND_OP_VIVIDLIGHT_EXT";
            case VK_BLEND_OP_LINEARLIGHT_EXT:
                return "VK_BLEND_OP_LINEARLIGHT_EXT";
            case VK_BLEND_OP_PINLIGHT_EXT:
                return "VK_BLEND_OP_PINLIGHT_EXT";
            case VK_BLEND_OP_HARDMIX_EXT:
                return "VK_BLEND_OP_HARDMIX_EXT";
            case VK_BLEND_OP_HSL_HUE_EXT:
                return "VK_BLEND_OP_HSL_HUE_EXT";
            case VK_BLEND_OP_HSL_SATURATION_EXT:
                return "VK_BLEND_OP_HSL_SATURATION_EXT";
            case VK_BLEND_OP_HSL_COLOR_EXT:
                return "VK_BLEND_OP_HSL_COLOR_EXT";
            case VK_BLEND_OP_HSL_LUMINOSITY_EXT:
                return "VK_BLEND_OP_HSL_LUMINOSITY_EXT";
            case VK_BLEND_OP_PLUS_EXT:
                return "VK_BLEND_OP_PLUS_EXT";
            case VK_BLEND_OP_PLUS_CLAMPED_EXT:
                return "VK_BLEND_OP_PLUS_CLAMPED_EXT";
            case VK_BLEND_OP_PLUS_CLAMPED_ALPHA_EXT:
                return "VK_BLEND_OP_PLUS_CLAMPED_ALPHA_EXT";
            case VK_BLEND_OP_PLUS_DARKER_EXT:
                return "VK_BLEND_OP_PLUS_DARKER_EXT";
            case VK_BLEND_OP_MINUS_EXT:
                return "VK_BLEND_OP_MINUS_EXT";
            case VK_BLEND_OP_MINUS_CLAMPED_EXT:
                return "VK_BLEND_OP_MINUS_CLAMPED_EXT";
            case VK_BLEND_OP_CONTRAST_EXT:
                return "VK_BLEND_OP_CONTRAST_EXT";
            case VK_BLEND_OP_INVERT_OVG_EXT:
                return "VK_BLEND_OP_INVERT_OVG_EXT";
            case VK_BLEND_OP_RED_EXT:
                return "VK_BLEND_OP_RED_EXT";
            case VK_BLEND_OP_GREEN_EXT:
                return "VK_BLEND_OP_GREEN_EXT";
            case VK_BLEND_OP_BLUE_EXT:
                return "VK_BLEND_OP_BLUE_EXT";
            default:
                break;
        }
        Warn() << "Invalid VkBlendOp enum value";
        return nullptr;
    }

    const char* gen_VkColorComponentFlagBits_c_str(const VkColorComponentFlagBits v) {
        switch (v) {
            case VK_COLOR_COMPONENT_R_BIT:
                return "VK_COLOR_COMPONENT_R_BIT";
            case VK_COLOR_COMPONENT_G_BIT:
                return "VK_COLOR_COMPONENT_G_BIT";
            case VK_COLOR_COMPONENT_B_BIT:
                return "VK_COLOR_COMPONENT_B_BIT";
            case VK_COLOR_COMPONENT_A_BIT:
                return "VK_COLOR_COMPONENT_A_BIT";
            default:
                break;
        }
        Error() << "Invalid bitmask value";
        return "Invalid bitmask value";
    }

    const char* gen_VkBlendOverlapEXT_c_str(const VkBlendOverlapEXT v) {
        switch (v) {
            case VK_BLEND_OVERLAP_UNCORRELATED_EXT:
                return "VK_BLEND_OVERLAP_UNCORRELATED_EXT";
            case VK_BLEND_OVERLAP_DISJOINT_EXT:
                return "VK_BLEND_OVERLAP_DISJOINT_EXT";
            case VK_BLEND_OVERLAP_CONJOINT_EXT:
                return "VK_BLEND_OVERLAP_CONJOINT_EXT";
            default:
                break;
        }
        Warn() << "Invalid VkBlendOverlapEXT enum value";
        return nullptr;
    }

    const char* gen_VkDynamicState_c_str(const VkDynamicState v) {
        switch (v) {
            case VK_DYNAMIC_STATE_VIEWPORT:
                return "VK_DYNAMIC_STATE_VIEWPORT";
            case VK_DYNAMIC_STATE_SCISSOR:
                return "VK_DYNAMIC_STATE_SCISSOR";
            case VK_DYNAMIC_STATE_LINE_WIDTH:
                return "VK_DYNAMIC_STATE_LINE_WIDTH";
            case VK_DYNAMIC_STATE_DEPTH_BIAS:
                return "VK_DYNAMIC_STATE_DEPTH_BIAS";
            case VK_DYNAMIC_STATE_BLEND_CONSTANTS:
                return "VK_DYNAMIC_STATE_BLEND_CONSTANTS";
            case VK_DYNAMIC_STATE_DEPTH_BOUNDS:
                return "VK_DYNAMIC_STATE_DEPTH_BOUNDS";
            case VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK:
                return "VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK";
            case VK_DYNAMIC_STATE_STENCIL_WRITE_MASK:
                return "VK_DYNAMIC_STATE_STENCIL_WRITE_MASK";
            case VK_DYNAMIC_STATE_STENCIL_REFERENCE:
                return "VK_DYNAMIC_STATE_STENCIL_REFERENCE";
            case VK_DYNAMIC_STATE_CULL_MODE:
                return "VK_DYNAMIC_STATE_CULL_MODE";
            case VK_DYNAMIC_STATE_FRONT_FACE:
                return "VK_DYNAMIC_STATE_FRONT_FACE";
            case VK_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY:
                return "VK_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY";
            case VK_DYNAMIC_STATE_VIEWPORT_WITH_COUNT:
                return "VK_DYNAMIC_STATE_VIEWPORT_WITH_COUNT";
            case VK_DYNAMIC_STATE_SCISSOR_WITH_COUNT:
                return "VK_DYNAMIC_STATE_SCISSOR_WITH_COUNT";
            case VK_DYNAMIC_STATE_VERTEX_INPUT_BINDING_STRIDE:
                return "VK_DYNAMIC_STATE_VERTEX_INPUT_BINDING_STRIDE";
            case VK_DYNAMIC_STATE_DEPTH_TEST_ENABLE:
                return "VK_DYNAMIC_STATE_DEPTH_TEST_ENABLE";
            case VK_DYNAMIC_STATE_DEPTH_WRITE_ENABLE:
                return "VK_DYNAMIC_STATE_DEPTH_WRITE_ENABLE";
            case VK_DYNAMIC_STATE_DEPTH_COMPARE_OP:
                return "VK_DYNAMIC_STATE_DEPTH_COMPARE_OP";
            case VK_DYNAMIC_STATE_DEPTH_BOUNDS_TEST_ENABLE:
                return "VK_DYNAMIC_STATE_DEPTH_BOUNDS_TEST_ENABLE";
            case VK_DYNAMIC_STATE_STENCIL_TEST_ENABLE:
                return "VK_DYNAMIC_STATE_STENCIL_TEST_ENABLE";
            case VK_DYNAMIC_STATE_STENCIL_OP:
                return "VK_DYNAMIC_STATE_STENCIL_OP";
            case VK_DYNAMIC_STATE_RASTERIZER_DISCARD_ENABLE:
                return "VK_DYNAMIC_STATE_RASTERIZER_DISCARD_ENABLE";
            case VK_DYNAMIC_STATE_DEPTH_BIAS_ENABLE:
                return "VK_DYNAMIC_STATE_DEPTH_BIAS_ENABLE";
            case VK_DYNAMIC_STATE_PRIMITIVE_RESTART_ENABLE:
                return "VK_DYNAMIC_STATE_PRIMITIVE_RESTART_ENABLE";
            case VK_DYNAMIC_STATE_LINE_STIPPLE:
                return "VK_DYNAMIC_STATE_LINE_STIPPLE";
            case VK_DYNAMIC_STATE_DISCARD_RECTANGLE_EXT:
                return "VK_DYNAMIC_STATE_DISCARD_RECTANGLE_EXT";
            case VK_DYNAMIC_STATE_DISCARD_RECTANGLE_ENABLE_EXT:
                return "VK_DYNAMIC_STATE_DISCARD_RECTANGLE_ENABLE_EXT";
            case VK_DYNAMIC_STATE_DISCARD_RECTANGLE_MODE_EXT:
                return "VK_DYNAMIC_STATE_DISCARD_RECTANGLE_MODE_EXT";
            case VK_DYNAMIC_STATE_SAMPLE_LOCATIONS_EXT:
                return "VK_DYNAMIC_STATE_SAMPLE_LOCATIONS_EXT";
            case VK_DYNAMIC_STATE_FRAGMENT_SHADING_RATE_KHR:
                return "VK_DYNAMIC_STATE_FRAGMENT_SHADING_RATE_KHR";
            case VK_DYNAMIC_STATE_VERTEX_INPUT_EXT:
                return "VK_DYNAMIC_STATE_VERTEX_INPUT_EXT";
            case VK_DYNAMIC_STATE_PATCH_CONTROL_POINTS_EXT:
                return "VK_DYNAMIC_STATE_PATCH_CONTROL_POINTS_EXT";
            case VK_DYNAMIC_STATE_LOGIC_OP_EXT:
                return "VK_DYNAMIC_STATE_LOGIC_OP_EXT";
            case VK_DYNAMIC_STATE_COLOR_WRITE_ENABLE_EXT:
                return "VK_DYNAMIC_STATE_COLOR_WRITE_ENABLE_EXT";
            default:
                break;
        }
        Warn() << "Invalid VkDynamicState enum value";
        return nullptr;
    }

    const char* gen_VkPipelineCreateFlagBits2_c_str(const VkPipelineCreateFlagBits2 v) {
        switch (v) {
            case VK_PIPELINE_CREATE_2_DISABLE_OPTIMIZATION_BIT:
                return "VK_PIPELINE_CREATE_2_DISABLE_OPTIMIZATION_BIT";
            case VK_PIPELINE_CREATE_2_ALLOW_DERIVATIVES_BIT:
                return "VK_PIPELINE_CREATE_2_ALLOW_DERIVATIVES_BIT";
            case VK_PIPELINE_CREATE_2_DERIVATIVE_BIT:
                return "VK_PIPELINE_CREATE_2_DERIVATIVE_BIT";
            case VK_PIPELINE_CREATE_2_VIEW_INDEX_FROM_DEVICE_INDEX_BIT:
                return "VK_PIPELINE_CREATE_2_VIEW_INDEX_FROM_DEVICE_INDEX_BIT";
            case VK_PIPELINE_CREATE_2_DISPATCH_BASE_BIT:
                return "VK_PIPELINE_CREATE_2_DISPATCH_BASE_BIT";
            case VK_PIPELINE_CREATE_2_FAIL_ON_PIPELINE_COMPILE_REQUIRED_BIT:
                return "VK_PIPELINE_CREATE_2_FAIL_ON_PIPELINE_COMPILE_REQUIRED_BIT";
            case VK_PIPELINE_CREATE_2_EARLY_RETURN_ON_FAILURE_BIT:
                return "VK_PIPELINE_CREATE_2_EARLY_RETURN_ON_FAILURE_BIT";
            case VK_PIPELINE_CREATE_2_NO_PROTECTED_ACCESS_BIT:
                return "VK_PIPELINE_CREATE_2_NO_PROTECTED_ACCESS_BIT";
            case VK_PIPELINE_CREATE_2_PROTECTED_ACCESS_ONLY_BIT:
                return "VK_PIPELINE_CREATE_2_PROTECTED_ACCESS_ONLY_BIT";
            default:
                break;
        }
        Error() << "Invalid bitmask value";
        return "Invalid bitmask value";
    }

    const char* gen_VkPipelineCreationFeedbackFlagBits_c_str(const VkPipelineCreationFeedbackFlagBits v) {
        switch (v) {
            case VK_PIPELINE_CREATION_FEEDBACK_VALID_BIT:
                return "VK_PIPELINE_CREATION_FEEDBACK_VALID_BIT";
            case VK_PIPELINE_CREATION_FEEDBACK_APPLICATION_PIPELINE_CACHE_HIT_BIT:
                return "VK_PIPELINE_CREATION_FEEDBACK_APPLICATION_PIPELINE_CACHE_HIT_BIT";
            case VK_PIPELINE_CREATION_FEEDBACK_BASE_PIPELINE_ACCELERATION_BIT:
                return "VK_PIPELINE_CREATION_FEEDBACK_BASE_PIPELINE_ACCELERATION_BIT";
            default:
                break;
        }
        Error() << "Invalid bitmask value";
        return "Invalid bitmask value";
    }

    const char* gen_VkDiscardRectangleModeEXT_c_str(const VkDiscardRectangleModeEXT v) {
        switch (v) {
            case VK_DISCARD_RECTANGLE_MODE_INCLUSIVE_EXT:
                return "VK_DISCARD_RECTANGLE_MODE_INCLUSIVE_EXT";
            case VK_DISCARD_RECTANGLE_MODE_EXCLUSIVE_EXT:
                return "VK_DISCARD_RECTANGLE_MODE_EXCLUSIVE_EXT";
            default:
                break;
        }
        Warn() << "Invalid VkDiscardRectangleModeEXT enum value";
        return nullptr;
    }

    const char* gen_VkFragmentShadingRateCombinerOpKHR_c_str(const VkFragmentShadingRateCombinerOpKHR v) {
        switch (v) {
            case VK_FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP_KHR:
                return "VK_FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP_KHR";
            case VK_FRAGMENT_SHADING_RATE_COMBINER_OP_REPLACE_KHR:
                return "VK_FRAGMENT_SHADING_RATE_COMBINER_OP_REPLACE_KHR";
            case VK_FRAGMENT_SHADING_RATE_COMBINER_OP_MIN_KHR:
                return "VK_FRAGMENT_SHADING_RATE_COMBINER_OP_MIN_KHR";
            case VK_FRAGMENT_SHADING_RATE_COMBINER_OP_MAX_KHR:
                return "VK_FRAGMENT_SHADING_RATE_COMBINER_OP_MAX_KHR";
            case VK_FRAGMENT_SHADING_RATE_COMBINER_OP_MUL_KHR:
                return "VK_FRAGMENT_SHADING_RATE_COMBINER_OP_MUL_KHR";
            default:
                break;
        }
        Warn() << "Invalid VkFragmentShadingRateCombinerOpKHR enum value";
        return nullptr;
    }

    const char* gen_VkPipelineMatchControl_c_str(const VkPipelineMatchControl v) {
        switch (v) {
            case VK_PIPELINE_MATCH_CONTROL_APPLICATION_UUID_EXACT_MATCH:
                return "VK_PIPELINE_MATCH_CONTROL_APPLICATION_UUID_EXACT_MATCH";
            default:
                break;
        }
        Warn() << "Invalid VkPipelineMatchControl enum value";
        return nullptr;
    }

    const char* gen_VkSamplerYcbcrModelConversion_c_str(const VkSamplerYcbcrModelConversion v) {
        switch (v) {
            case VK_SAMPLER_YCBCR_MODEL_CONVERSION_RGB_IDENTITY:
                return "VK_SAMPLER_YCBCR_MODEL_CONVERSION_RGB_IDENTITY";
            case VK_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_IDENTITY:
                return "VK_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_IDENTITY";
            case VK_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_709:
                return "VK_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_709";
            case VK_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_601:
                return "VK_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_601";
            case VK_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_2020:
                return "VK_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_2020";
            default:
                break;
        }
        Warn() << "Invalid VkSamplerYcbcrModelConversion enum value";
        return nullptr;
    }

    const char* gen_VkSamplerYcbcrRange_c_str(const VkSamplerYcbcrRange v) {
        switch (v) {
            case VK_SAMPLER_YCBCR_RANGE_ITU_FULL:
                return "VK_SAMPLER_YCBCR_RANGE_ITU_FULL";
            case VK_SAMPLER_YCBCR_RANGE_ITU_NARROW:
                return "VK_SAMPLER_YCBCR_RANGE_ITU_NARROW";
            default:
                break;
        }
        Warn() << "Invalid VkSamplerYcbcrRange enum value";
        return nullptr;
    }

    const char* gen_VkComponentSwizzle_c_str(const VkComponentSwizzle v) {
        switch (v) {
            case VK_COMPONENT_SWIZZLE_IDENTITY:
                return "VK_COMPONENT_SWIZZLE_IDENTITY";
            case VK_COMPONENT_SWIZZLE_ZERO:
                return "VK_COMPONENT_SWIZZLE_ZERO";
            case VK_COMPONENT_SWIZZLE_ONE:
                return "VK_COMPONENT_SWIZZLE_ONE";
            case VK_COMPONENT_SWIZZLE_R:
                return "VK_COMPONENT_SWIZZLE_R";
            case VK_COMPONENT_SWIZZLE_G:
                return "VK_COMPONENT_SWIZZLE_G";
            case VK_COMPONENT_SWIZZLE_B:
                return "VK_COMPONENT_SWIZZLE_B";
            case VK_COMPONENT_SWIZZLE_A:
                return "VK_COMPONENT_SWIZZLE_A";
            default:
                break;
        }
        Warn() << "Invalid VkComponentSwizzle enum value";
        return nullptr;
    }

    const char* gen_VkChromaLocation_c_str(const VkChromaLocation v) {
        switch (v) {
            case VK_CHROMA_LOCATION_COSITED_EVEN:
                return "VK_CHROMA_LOCATION_COSITED_EVEN";
            case VK_CHROMA_LOCATION_MIDPOINT:
                return "VK_CHROMA_LOCATION_MIDPOINT";
            default:
                break;
        }
        Warn() << "Invalid VkChromaLocation enum value";
        return nullptr;
    }

    const char* gen_VkFilter_c_str(const VkFilter v) {
        switch (v) {
            case VK_FILTER_NEAREST:
                return "VK_FILTER_NEAREST";
            case VK_FILTER_LINEAR:
                return "VK_FILTER_LINEAR";
            case VK_FILTER_CUBIC_EXT:
                return "VK_FILTER_CUBIC_EXT";
            default:
                break;
        }
        Warn() << "Invalid VkFilter enum value";
        return nullptr;
    }

    const char* gen_VkSamplerCreateFlagBits_c_str(const VkSamplerCreateFlagBits v) {
        switch (v) {
            default:
                break;
        }
        Error() << "Invalid bitmask value";
        return "Invalid bitmask value";
    }

    const char* gen_VkSamplerMipmapMode_c_str(const VkSamplerMipmapMode v) {
        switch (v) {
            case VK_SAMPLER_MIPMAP_MODE_NEAREST:
                return "VK_SAMPLER_MIPMAP_MODE_NEAREST";
            case VK_SAMPLER_MIPMAP_MODE_LINEAR:
                return "VK_SAMPLER_MIPMAP_MODE_LINEAR";
            default:
                break;
        }
        Warn() << "Invalid VkSamplerMipmapMode enum value";
        return nullptr;
    }

    const char* gen_VkSamplerAddressMode_c_str(const VkSamplerAddressMode v) {
        switch (v) {
            case VK_SAMPLER_ADDRESS_MODE_REPEAT:
                return "VK_SAMPLER_ADDRESS_MODE_REPEAT";
            case VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT:
                return "VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT";
            case VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE:
                return "VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE";
            case VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER:
                return "VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER";
            case VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE:
                return "VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE";
            default:
                break;
        }
        Warn() << "Invalid VkSamplerAddressMode enum value";
        return nullptr;
    }

    const char* gen_VkBorderColor_c_str(const VkBorderColor v) {
        switch (v) {
            case VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK:
                return "VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK";
            case VK_BORDER_COLOR_INT_TRANSPARENT_BLACK:
                return "VK_BORDER_COLOR_INT_TRANSPARENT_BLACK";
            case VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK:
                return "VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK";
            case VK_BORDER_COLOR_INT_OPAQUE_BLACK:
                return "VK_BORDER_COLOR_INT_OPAQUE_BLACK";
            case VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE:
                return "VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE";
            case VK_BORDER_COLOR_INT_OPAQUE_WHITE:
                return "VK_BORDER_COLOR_INT_OPAQUE_WHITE";
            case VK_BORDER_COLOR_FLOAT_CUSTOM_EXT:
                return "VK_BORDER_COLOR_FLOAT_CUSTOM_EXT";
            case VK_BORDER_COLOR_INT_CUSTOM_EXT:
                return "VK_BORDER_COLOR_INT_CUSTOM_EXT";
            default:
                break;
        }
        Warn() << "Invalid VkBorderColor enum value";
        return nullptr;
    }

    const char* gen_VkSamplerReductionMode_c_str(const VkSamplerReductionMode v) {
        switch (v) {
            case VK_SAMPLER_REDUCTION_MODE_WEIGHTED_AVERAGE:
                return "VK_SAMPLER_REDUCTION_MODE_WEIGHTED_AVERAGE";
            case VK_SAMPLER_REDUCTION_MODE_MIN:
                return "VK_SAMPLER_REDUCTION_MODE_MIN";
            case VK_SAMPLER_REDUCTION_MODE_MAX:
                return "VK_SAMPLER_REDUCTION_MODE_MAX";
            default:
                break;
        }
        Warn() << "Invalid VkSamplerReductionMode enum value";
        return nullptr;
    }

    const char* gen_VkDescriptorSetLayoutCreateFlagBits_c_str(const VkDescriptorSetLayoutCreateFlagBits v) {
        switch (v) {
            case VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT:
                return "VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT";
            case VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT:
                return "VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT";
            default:
                break;
        }
        Error() << "Invalid bitmask value";
        return "Invalid bitmask value";
    }

    const char* gen_VkDescriptorType_c_str(const VkDescriptorType v) {
        switch (v) {
            case VK_DESCRIPTOR_TYPE_SAMPLER:
                return "VK_DESCRIPTOR_TYPE_SAMPLER";
            case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
                return "VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER";
            case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
                return "VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE";
            case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
                return "VK_DESCRIPTOR_TYPE_STORAGE_IMAGE";
            case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
                return "VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER";
            case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
                return "VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER";
            case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
                return "VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER";
            case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
                return "VK_DESCRIPTOR_TYPE_STORAGE_BUFFER";
            case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
                return "VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC";
            case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
                return "VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC";
            case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
                return "VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT";
            case VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK:
                return "VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK";
            default:
                break;
        }
        Warn() << "Invalid VkDescriptorType enum value";
        return nullptr;
    }

    const char* gen_VkDescriptorBindingFlagBits_c_str(const VkDescriptorBindingFlagBits v) {
        switch (v) {
            case VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT:
                return "VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT";
            case VK_DESCRIPTOR_BINDING_UPDATE_UNUSED_WHILE_PENDING_BIT:
                return "VK_DESCRIPTOR_BINDING_UPDATE_UNUSED_WHILE_PENDING_BIT";
            case VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT:
                return "VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT";
            case VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT:
                return "VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT";
            default:
                break;
        }
        Error() << "Invalid bitmask value";
        return "Invalid bitmask value";
    }

    const char* gen_VkPipelineLayoutCreateFlagBits_c_str(const VkPipelineLayoutCreateFlagBits v) {
        switch (v) {
            default:
                break;
        }
        Error() << "Invalid bitmask value";
        return "Invalid bitmask value";
    }

    const char* gen_VkRenderPassCreateFlagBits_c_str(const VkRenderPassCreateFlagBits v) {
        switch (v) {
            default:
                break;
        }
        Error() << "Invalid bitmask value";
        return "Invalid bitmask value";
    }

    const char* gen_VkAttachmentDescriptionFlagBits_c_str(const VkAttachmentDescriptionFlagBits v) {
        switch (v) {
            case VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT:
                return "VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT";
            default:
                break;
        }
        Error() << "Invalid bitmask value";
        return "Invalid bitmask value";
    }

    const char* gen_VkAttachmentLoadOp_c_str(const VkAttachmentLoadOp v) {
        switch (v) {
            case VK_ATTACHMENT_LOAD_OP_LOAD:
                return "VK_ATTACHMENT_LOAD_OP_LOAD";
            case VK_ATTACHMENT_LOAD_OP_CLEAR:
                return "VK_ATTACHMENT_LOAD_OP_CLEAR";
            case VK_ATTACHMENT_LOAD_OP_DONT_CARE:
                return "VK_ATTACHMENT_LOAD_OP_DONT_CARE";
            case VK_ATTACHMENT_LOAD_OP_NONE:
                return "VK_ATTACHMENT_LOAD_OP_NONE";
            default:
                break;
        }
        Warn() << "Invalid VkAttachmentLoadOp enum value";
        return nullptr;
    }

    const char* gen_VkAttachmentStoreOp_c_str(const VkAttachmentStoreOp v) {
        switch (v) {
            case VK_ATTACHMENT_STORE_OP_STORE:
                return "VK_ATTACHMENT_STORE_OP_STORE";
            case VK_ATTACHMENT_STORE_OP_DONT_CARE:
                return "VK_ATTACHMENT_STORE_OP_DONT_CARE";
            case VK_ATTACHMENT_STORE_OP_NONE:
                return "VK_ATTACHMENT_STORE_OP_NONE";
            default:
                break;
        }
        Warn() << "Invalid VkAttachmentStoreOp enum value";
        return nullptr;
    }

    const char* gen_VkImageLayout_c_str(const VkImageLayout v) {
        switch (v) {
            case VK_IMAGE_LAYOUT_UNDEFINED:
                return "VK_IMAGE_LAYOUT_UNDEFINED";
            case VK_IMAGE_LAYOUT_GENERAL:
                return "VK_IMAGE_LAYOUT_GENERAL";
            case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
                return "VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL";
            case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
                return "VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL";
            case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
                return "VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL";
            case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
                return "VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL";
            case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
                return "VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL";
            case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
                return "VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL";
            case VK_IMAGE_LAYOUT_PREINITIALIZED:
                return "VK_IMAGE_LAYOUT_PREINITIALIZED";
            case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL:
                return "VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL";
            case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL:
                return "VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL";
            case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL:
                return "VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL";
            case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL:
                return "VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL";
            case VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL:
                return "VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL";
            case VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL:
                return "VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL";
            case VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL:
                return "VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL";
            case VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL:
                return "VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL";
            case VK_IMAGE_LAYOUT_RENDERING_LOCAL_READ:
                return "VK_IMAGE_LAYOUT_RENDERING_LOCAL_READ";
            case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
                return "VK_IMAGE_LAYOUT_PRESENT_SRC_KHR";
            case VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR:
                return "VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR";
            case VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR:
                return "VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR";
            default:
                break;
        }
        Warn() << "Invalid VkImageLayout enum value";
        return nullptr;
    }

    const char* gen_VkSubpassDescriptionFlagBits_c_str(const VkSubpassDescriptionFlagBits v) {
        switch (v) {
            default:
                break;
        }
        Error() << "Invalid bitmask value";
        return "Invalid bitmask value";
    }

    const char* gen_VkPipelineBindPoint_c_str(const VkPipelineBindPoint v) {
        switch (v) {
            case VK_PIPELINE_BIND_POINT_GRAPHICS:
                return "VK_PIPELINE_BIND_POINT_GRAPHICS";
            case VK_PIPELINE_BIND_POINT_COMPUTE:
                return "VK_PIPELINE_BIND_POINT_COMPUTE";
            default:
                break;
        }
        Warn() << "Invalid VkPipelineBindPoint enum value";
        return nullptr;
    }

    const char* gen_VkPipelineStageFlagBits_c_str(const VkPipelineStageFlagBits v) {
        switch (v) {
            case VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT:
                return "VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT";
            case VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT:
                return "VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT";
            case VK_PIPELINE_STAGE_VERTEX_INPUT_BIT:
                return "VK_PIPELINE_STAGE_VERTEX_INPUT_BIT";
            case VK_PIPELINE_STAGE_VERTEX_SHADER_BIT:
                return "VK_PIPELINE_STAGE_VERTEX_SHADER_BIT";
            case VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT:
                return "VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT";
            case VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT:
                return "VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT";
            case VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT:
                return "VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT";
            case VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT:
                return "VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT";
            case VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT:
                return "VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT";
            case VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT:
                return "VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT";
            case VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT:
                return "VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT";
            case VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT:
                return "VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT";
            case VK_PIPELINE_STAGE_TRANSFER_BIT:
                return "VK_PIPELINE_STAGE_TRANSFER_BIT";
            case VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT:
                return "VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT";
            case VK_PIPELINE_STAGE_HOST_BIT:
                return "VK_PIPELINE_STAGE_HOST_BIT";
            case VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT:
                return "VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT";
            case VK_PIPELINE_STAGE_ALL_COMMANDS_BIT:
                return "VK_PIPELINE_STAGE_ALL_COMMANDS_BIT";
            case VK_PIPELINE_STAGE_NONE:
                return "VK_PIPELINE_STAGE_NONE";
            case VK_PIPELINE_STAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR:
                return "VK_PIPELINE_STAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR";
            default:
                break;
        }
        Error() << "Invalid bitmask value";
        return "Invalid bitmask value";
    }

    const char* gen_VkAccessFlagBits_c_str(const VkAccessFlagBits v) {
        switch (v) {
            case VK_ACCESS_INDIRECT_COMMAND_READ_BIT:
                return "VK_ACCESS_INDIRECT_COMMAND_READ_BIT";
            case VK_ACCESS_INDEX_READ_BIT:
                return "VK_ACCESS_INDEX_READ_BIT";
            case VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT:
                return "VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT";
            case VK_ACCESS_UNIFORM_READ_BIT:
                return "VK_ACCESS_UNIFORM_READ_BIT";
            case VK_ACCESS_INPUT_ATTACHMENT_READ_BIT:
                return "VK_ACCESS_INPUT_ATTACHMENT_READ_BIT";
            case VK_ACCESS_SHADER_READ_BIT:
                return "VK_ACCESS_SHADER_READ_BIT";
            case VK_ACCESS_SHADER_WRITE_BIT:
                return "VK_ACCESS_SHADER_WRITE_BIT";
            case VK_ACCESS_COLOR_ATTACHMENT_READ_BIT:
                return "VK_ACCESS_COLOR_ATTACHMENT_READ_BIT";
            case VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT:
                return "VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT";
            case VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT:
                return "VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT";
            case VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT:
                return "VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT";
            case VK_ACCESS_TRANSFER_READ_BIT:
                return "VK_ACCESS_TRANSFER_READ_BIT";
            case VK_ACCESS_TRANSFER_WRITE_BIT:
                return "VK_ACCESS_TRANSFER_WRITE_BIT";
            case VK_ACCESS_HOST_READ_BIT:
                return "VK_ACCESS_HOST_READ_BIT";
            case VK_ACCESS_HOST_WRITE_BIT:
                return "VK_ACCESS_HOST_WRITE_BIT";
            case VK_ACCESS_MEMORY_READ_BIT:
                return "VK_ACCESS_MEMORY_READ_BIT";
            case VK_ACCESS_MEMORY_WRITE_BIT:
                return "VK_ACCESS_MEMORY_WRITE_BIT";
            case VK_ACCESS_NONE:
                return "VK_ACCESS_NONE";
            case VK_ACCESS_COLOR_ATTACHMENT_READ_NONCOHERENT_BIT_EXT:
                return "VK_ACCESS_COLOR_ATTACHMENT_READ_NONCOHERENT_BIT_EXT";
            case VK_ACCESS_FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR:
                return "VK_ACCESS_FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR";
            default:
                break;
        }
        Error() << "Invalid bitmask value";
        return "Invalid bitmask value";
    }

    const char* gen_VkDependencyFlagBits_c_str(const VkDependencyFlagBits v) {
        switch (v) {
            case VK_DEPENDENCY_BY_REGION_BIT:
                return "VK_DEPENDENCY_BY_REGION_BIT";
            case VK_DEPENDENCY_DEVICE_GROUP_BIT:
                return "VK_DEPENDENCY_DEVICE_GROUP_BIT";
            case VK_DEPENDENCY_VIEW_LOCAL_BIT:
                return "VK_DEPENDENCY_VIEW_LOCAL_BIT";
            default:
                break;
        }
        Error() << "Invalid bitmask value";
        return "Invalid bitmask value";
    }

    const char* gen_VkImageAspectFlagBits_c_str(const VkImageAspectFlagBits v) {
        switch (v) {
            case VK_IMAGE_ASPECT_COLOR_BIT:
                return "VK_IMAGE_ASPECT_COLOR_BIT";
            case VK_IMAGE_ASPECT_DEPTH_BIT:
                return "VK_IMAGE_ASPECT_DEPTH_BIT";
            case VK_IMAGE_ASPECT_STENCIL_BIT:
                return "VK_IMAGE_ASPECT_STENCIL_BIT";
            case VK_IMAGE_ASPECT_METADATA_BIT:
                return "VK_IMAGE_ASPECT_METADATA_BIT";
            case VK_IMAGE_ASPECT_PLANE_0_BIT:
                return "VK_IMAGE_ASPECT_PLANE_0_BIT";
            case VK_IMAGE_ASPECT_PLANE_1_BIT:
                return "VK_IMAGE_ASPECT_PLANE_1_BIT";
            case VK_IMAGE_ASPECT_PLANE_2_BIT:
                return "VK_IMAGE_ASPECT_PLANE_2_BIT";
            case VK_IMAGE_ASPECT_NONE:
                return "VK_IMAGE_ASPECT_NONE";
            case VK_IMAGE_ASPECT_MEMORY_PLANE_0_BIT_EXT:
                return "VK_IMAGE_ASPECT_MEMORY_PLANE_0_BIT_EXT";
            case VK_IMAGE_ASPECT_MEMORY_PLANE_1_BIT_EXT:
                return "VK_IMAGE_ASPECT_MEMORY_PLANE_1_BIT_EXT";
            case VK_IMAGE_ASPECT_MEMORY_PLANE_2_BIT_EXT:
                return "VK_IMAGE_ASPECT_MEMORY_PLANE_2_BIT_EXT";
            case VK_IMAGE_ASPECT_MEMORY_PLANE_3_BIT_EXT:
                return "VK_IMAGE_ASPECT_MEMORY_PLANE_3_BIT_EXT";
            default:
                break;
        }
        Error() << "Invalid bitmask value";
        return "Invalid bitmask value";
    }

    const char* gen_VkResolveModeFlagBits_c_str(const VkResolveModeFlagBits v) {
        switch (v) {
            case VK_RESOLVE_MODE_NONE:
                return "VK_RESOLVE_MODE_NONE";
            case VK_RESOLVE_MODE_SAMPLE_ZERO_BIT:
                return "VK_RESOLVE_MODE_SAMPLE_ZERO_BIT";
            case VK_RESOLVE_MODE_AVERAGE_BIT:
                return "VK_RESOLVE_MODE_AVERAGE_BIT";
            case VK_RESOLVE_MODE_MIN_BIT:
                return "VK_RESOLVE_MODE_MIN_BIT";
            case VK_RESOLVE_MODE_MAX_BIT:
                return "VK_RESOLVE_MODE_MAX_BIT";
            default:
                break;
        }
        Error() << "Invalid bitmask value";
        return "Invalid bitmask value";
    }

    const char* gen_VkPipelineStageFlagBits2_c_str(const VkPipelineStageFlagBits2 v) {
        switch (v) {
            case VK_PIPELINE_STAGE_2_NONE:
                return "VK_PIPELINE_STAGE_2_NONE";
            case VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT:
                return "VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT";
            case VK_PIPELINE_STAGE_2_DRAW_INDIRECT_BIT:
                return "VK_PIPELINE_STAGE_2_DRAW_INDIRECT_BIT";
            case VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT:
                return "VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT";
            case VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT:
                return "VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT";
            case VK_PIPELINE_STAGE_2_TESSELLATION_CONTROL_SHADER_BIT:
                return "VK_PIPELINE_STAGE_2_TESSELLATION_CONTROL_SHADER_BIT";
            case VK_PIPELINE_STAGE_2_TESSELLATION_EVALUATION_SHADER_BIT:
                return "VK_PIPELINE_STAGE_2_TESSELLATION_EVALUATION_SHADER_BIT";
            case VK_PIPELINE_STAGE_2_GEOMETRY_SHADER_BIT:
                return "VK_PIPELINE_STAGE_2_GEOMETRY_SHADER_BIT";
            case VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT:
                return "VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT";
            case VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT:
                return "VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT";
            case VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT:
                return "VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT";
            case VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT:
                return "VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT";
            case VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT:
                return "VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT";
            case VK_PIPELINE_STAGE_2_ALL_TRANSFER_BIT:
                return "VK_PIPELINE_STAGE_2_ALL_TRANSFER_BIT";
            case VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT:
                return "VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT";
            case VK_PIPELINE_STAGE_2_HOST_BIT:
                return "VK_PIPELINE_STAGE_2_HOST_BIT";
            case VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT:
                return "VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT";
            case VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT:
                return "VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT";
            case VK_PIPELINE_STAGE_2_COPY_BIT:
                return "VK_PIPELINE_STAGE_2_COPY_BIT";
            case VK_PIPELINE_STAGE_2_RESOLVE_BIT:
                return "VK_PIPELINE_STAGE_2_RESOLVE_BIT";
            case VK_PIPELINE_STAGE_2_BLIT_BIT:
                return "VK_PIPELINE_STAGE_2_BLIT_BIT";
            case VK_PIPELINE_STAGE_2_CLEAR_BIT:
                return "VK_PIPELINE_STAGE_2_CLEAR_BIT";
            case VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT:
                return "VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT";
            case VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT:
                return "VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT";
            case VK_PIPELINE_STAGE_2_PRE_RASTERIZATION_SHADERS_BIT:
                return "VK_PIPELINE_STAGE_2_PRE_RASTERIZATION_SHADERS_BIT";
            case VK_PIPELINE_STAGE_2_TRANSFORM_FEEDBACK_BIT_EXT:
                return "VK_PIPELINE_STAGE_2_TRANSFORM_FEEDBACK_BIT_EXT";
            case VK_PIPELINE_STAGE_2_CONDITIONAL_RENDERING_BIT_EXT:
                return "VK_PIPELINE_STAGE_2_CONDITIONAL_RENDERING_BIT_EXT";
            case VK_PIPELINE_STAGE_2_COMMAND_PREPROCESS_BIT_EXT:
                return "VK_PIPELINE_STAGE_2_COMMAND_PREPROCESS_BIT_EXT";
            case VK_PIPELINE_STAGE_2_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR:
                return "VK_PIPELINE_STAGE_2_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR";
            case VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_KHR:
                return "VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_KHR";
            case VK_PIPELINE_STAGE_2_RAY_TRACING_SHADER_BIT_KHR:
                return "VK_PIPELINE_STAGE_2_RAY_TRACING_SHADER_BIT_KHR";
            case VK_PIPELINE_STAGE_2_FRAGMENT_DENSITY_PROCESS_BIT_EXT:
                return "VK_PIPELINE_STAGE_2_FRAGMENT_DENSITY_PROCESS_BIT_EXT";
            case VK_PIPELINE_STAGE_2_TASK_SHADER_BIT_EXT:
                return "VK_PIPELINE_STAGE_2_TASK_SHADER_BIT_EXT";
            case VK_PIPELINE_STAGE_2_MESH_SHADER_BIT_EXT:
                return "VK_PIPELINE_STAGE_2_MESH_SHADER_BIT_EXT";
            default:
                break;
        }
        Error() << "Invalid bitmask value";
        return "Invalid bitmask value";
    }

    const char* gen_VkAccessFlagBits2_c_str(const VkAccessFlagBits2 v) {
        switch (v) {
            case VK_ACCESS_2_NONE:
                return "VK_ACCESS_2_NONE";
            case VK_ACCESS_2_INDIRECT_COMMAND_READ_BIT:
                return "VK_ACCESS_2_INDIRECT_COMMAND_READ_BIT";
            case VK_ACCESS_2_INDEX_READ_BIT:
                return "VK_ACCESS_2_INDEX_READ_BIT";
            case VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT:
                return "VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT";
            case VK_ACCESS_2_UNIFORM_READ_BIT:
                return "VK_ACCESS_2_UNIFORM_READ_BIT";
            case VK_ACCESS_2_INPUT_ATTACHMENT_READ_BIT:
                return "VK_ACCESS_2_INPUT_ATTACHMENT_READ_BIT";
            case VK_ACCESS_2_SHADER_READ_BIT:
                return "VK_ACCESS_2_SHADER_READ_BIT";
            case VK_ACCESS_2_SHADER_WRITE_BIT:
                return "VK_ACCESS_2_SHADER_WRITE_BIT";
            case VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT:
                return "VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT";
            case VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT:
                return "VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT";
            case VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT:
                return "VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT";
            case VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT:
                return "VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT";
            case VK_ACCESS_2_TRANSFER_READ_BIT:
                return "VK_ACCESS_2_TRANSFER_READ_BIT";
            case VK_ACCESS_2_TRANSFER_WRITE_BIT:
                return "VK_ACCESS_2_TRANSFER_WRITE_BIT";
            case VK_ACCESS_2_HOST_READ_BIT:
                return "VK_ACCESS_2_HOST_READ_BIT";
            case VK_ACCESS_2_HOST_WRITE_BIT:
                return "VK_ACCESS_2_HOST_WRITE_BIT";
            case VK_ACCESS_2_MEMORY_READ_BIT:
                return "VK_ACCESS_2_MEMORY_READ_BIT";
            case VK_ACCESS_2_MEMORY_WRITE_BIT:
                return "VK_ACCESS_2_MEMORY_WRITE_BIT";
            case VK_ACCESS_2_SHADER_SAMPLED_READ_BIT:
                return "VK_ACCESS_2_SHADER_SAMPLED_READ_BIT";
            case VK_ACCESS_2_SHADER_STORAGE_READ_BIT:
                return "VK_ACCESS_2_SHADER_STORAGE_READ_BIT";
            case VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT:
                return "VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT";
            case VK_ACCESS_2_TRANSFORM_FEEDBACK_WRITE_BIT_EXT:
                return "VK_ACCESS_2_TRANSFORM_FEEDBACK_WRITE_BIT_EXT";
            case VK_ACCESS_2_TRANSFORM_FEEDBACK_COUNTER_READ_BIT_EXT:
                return "VK_ACCESS_2_TRANSFORM_FEEDBACK_COUNTER_READ_BIT_EXT";
            case VK_ACCESS_2_TRANSFORM_FEEDBACK_COUNTER_WRITE_BIT_EXT:
                return "VK_ACCESS_2_TRANSFORM_FEEDBACK_COUNTER_WRITE_BIT_EXT";
            case VK_ACCESS_2_CONDITIONAL_RENDERING_READ_BIT_EXT:
                return "VK_ACCESS_2_CONDITIONAL_RENDERING_READ_BIT_EXT";
            case VK_ACCESS_2_COMMAND_PREPROCESS_READ_BIT_EXT:
                return "VK_ACCESS_2_COMMAND_PREPROCESS_READ_BIT_EXT";
            case VK_ACCESS_2_COMMAND_PREPROCESS_WRITE_BIT_EXT:
                return "VK_ACCESS_2_COMMAND_PREPROCESS_WRITE_BIT_EXT";
            case VK_ACCESS_2_FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR:
                return "VK_ACCESS_2_FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR";
            case VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR:
                return "VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR";
            case VK_ACCESS_2_ACCELERATION_STRUCTURE_WRITE_BIT_KHR:
                return "VK_ACCESS_2_ACCELERATION_STRUCTURE_WRITE_BIT_KHR";
            case VK_ACCESS_2_FRAGMENT_DENSITY_MAP_READ_BIT_EXT:
                return "VK_ACCESS_2_FRAGMENT_DENSITY_MAP_READ_BIT_EXT";
            case VK_ACCESS_2_COLOR_ATTACHMENT_READ_NONCOHERENT_BIT_EXT:
                return "VK_ACCESS_2_COLOR_ATTACHMENT_READ_NONCOHERENT_BIT_EXT";
            default:
                break;
        }
        Error() << "Invalid bitmask value";
        return "Invalid bitmask value";
    }

    const char* gen_VkPipelineCacheCreateFlagBits_c_str(const VkPipelineCacheCreateFlagBits v) {
        switch (v) {
            case VK_PIPELINE_CACHE_CREATE_EXTERNALLY_SYNCHRONIZED_BIT:
                return "VK_PIPELINE_CACHE_CREATE_EXTERNALLY_SYNCHRONIZED_BIT";
            case VK_PIPELINE_CACHE_CREATE_READ_ONLY_BIT:
                return "VK_PIPELINE_CACHE_CREATE_READ_ONLY_BIT";
            case VK_PIPELINE_CACHE_CREATE_USE_APPLICATION_STORAGE_BIT:
                return "VK_PIPELINE_CACHE_CREATE_USE_APPLICATION_STORAGE_BIT";
            default:
                break;
        }
        Error() << "Invalid bitmask value";
        return "Invalid bitmask value";
    }

  protected:
    void* filter_VkPhysicalDeviceFeatures2(const void* pDeviceCreateInfoPNext, const LocationScope& l) {
        auto base = AllocMem<VkPhysicalDeviceFeatures2>();
        void* pnext = nullptr;
        auto p = reinterpret_cast<const VkBaseInStructure*>(pDeviceCreateInfoPNext);
        while (p != nullptr) {
            switch (p->sType) {
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2: {
                    *base = *reinterpret_cast<const VkPhysicalDeviceFeatures2*>(p);
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES: {
                    auto s = AllocMem<VkPhysicalDevice16BitStorageFeatures>();
                    *s = *reinterpret_cast<const VkPhysicalDevice16BitStorageFeatures*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_4444_FORMATS_FEATURES_EXT: {
                    auto s = AllocMem<VkPhysicalDevice4444FormatsFeaturesEXT>();
                    *s = *reinterpret_cast<const VkPhysicalDevice4444FormatsFeaturesEXT*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES: {
                    auto s = AllocMem<VkPhysicalDevice8BitStorageFeatures>();
                    *s = *reinterpret_cast<const VkPhysicalDevice8BitStorageFeatures*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ASTC_DECODE_FEATURES_EXT: {
                    auto s = AllocMem<VkPhysicalDeviceASTCDecodeFeaturesEXT>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceASTCDecodeFeaturesEXT*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BLEND_OPERATION_ADVANCED_FEATURES_EXT: {
                    auto s = AllocMem<VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES: {
                    auto s = AllocMem<VkPhysicalDeviceBufferDeviceAddressFeatures>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceBufferDeviceAddressFeatures*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COLOR_WRITE_ENABLE_FEATURES_EXT: {
                    auto s = AllocMem<VkPhysicalDeviceColorWriteEnableFeaturesEXT>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceColorWriteEnableFeaturesEXT*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CUSTOM_BORDER_COLOR_FEATURES_EXT: {
                    auto s = AllocMem<VkPhysicalDeviceCustomBorderColorFeaturesEXT>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceCustomBorderColorFeaturesEXT*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLIP_ENABLE_FEATURES_EXT: {
                    auto s = AllocMem<VkPhysicalDeviceDepthClipEnableFeaturesEXT>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceDepthClipEnableFeaturesEXT*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES: {
                    auto s = AllocMem<VkPhysicalDeviceDescriptorIndexingFeatures>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceDescriptorIndexingFeatures*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES: {
                    auto s = AllocMem<VkPhysicalDeviceDynamicRenderingFeatures>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceDynamicRenderingFeatures*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_LOCAL_READ_FEATURES: {
                    auto s = AllocMem<VkPhysicalDeviceDynamicRenderingLocalReadFeatures>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceDynamicRenderingLocalReadFeatures*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_2_FEATURES_EXT: {
                    auto s = AllocMem<VkPhysicalDeviceExtendedDynamicState2FeaturesEXT>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceExtendedDynamicState2FeaturesEXT*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT: {
                    auto s = AllocMem<VkPhysicalDeviceExtendedDynamicStateFeaturesEXT>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceExtendedDynamicStateFeaturesEXT*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }
#ifdef VK_USE_PLATFORM_SCI

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_MEMORY_SCI_BUF_FEATURES_NV: {
                    auto s = AllocMem<VkPhysicalDeviceExternalMemorySciBufFeaturesNV>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceExternalMemorySciBufFeaturesNV*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }
#endif  // VK_USE_PLATFORM_SCI
#ifdef VK_USE_PLATFORM_SCREEN_QNX

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_MEMORY_SCREEN_BUFFER_FEATURES_QNX: {
                    auto s = AllocMem<VkPhysicalDeviceExternalMemoryScreenBufferFeaturesQNX>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceExternalMemoryScreenBufferFeaturesQNX*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }
#endif  // VK_USE_PLATFORM_SCREEN_QNX
#ifdef VK_USE_PLATFORM_SCI

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_SCI_SYNC_2_FEATURES_NV: {
                    auto s = AllocMem<VkPhysicalDeviceExternalSciSync2FeaturesNV>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceExternalSciSync2FeaturesNV*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_SCI_SYNC_FEATURES_NV: {
                    auto s = AllocMem<VkPhysicalDeviceExternalSciSyncFeaturesNV>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceExternalSciSyncFeaturesNV*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }
#endif  // VK_USE_PLATFORM_SCI

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_INTERLOCK_FEATURES_EXT: {
                    auto s = AllocMem<VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR: {
                    auto s = AllocMem<VkPhysicalDeviceFragmentShadingRateFeaturesKHR>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceFragmentShadingRateFeaturesKHR*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GLOBAL_PRIORITY_QUERY_FEATURES: {
                    auto s = AllocMem<VkPhysicalDeviceGlobalPriorityQueryFeatures>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceGlobalPriorityQueryFeatures*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_IMAGE_COPY_FEATURES: {
                    auto s = AllocMem<VkPhysicalDeviceHostImageCopyFeatures>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceHostImageCopyFeatures*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES: {
                    auto s = AllocMem<VkPhysicalDeviceHostQueryResetFeatures>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceHostQueryResetFeatures*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_ROBUSTNESS_FEATURES: {
                    auto s = AllocMem<VkPhysicalDeviceImageRobustnessFeatures>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceImageRobustnessFeatures*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES: {
                    auto s = AllocMem<VkPhysicalDeviceImagelessFramebufferFeatures>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceImagelessFramebufferFeatures*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INDEX_TYPE_UINT8_FEATURES: {
                    auto s = AllocMem<VkPhysicalDeviceIndexTypeUint8Features>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceIndexTypeUint8Features*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_FEATURES: {
                    auto s = AllocMem<VkPhysicalDeviceInlineUniformBlockFeatures>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceInlineUniformBlockFeatures*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINE_RASTERIZATION_FEATURES: {
                    auto s = AllocMem<VkPhysicalDeviceLineRasterizationFeatures>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceLineRasterizationFeatures*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_4_FEATURES: {
                    auto s = AllocMem<VkPhysicalDeviceMaintenance4Features>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceMaintenance4Features*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_5_FEATURES: {
                    auto s = AllocMem<VkPhysicalDeviceMaintenance5Features>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceMaintenance5Features*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_6_FEATURES: {
                    auto s = AllocMem<VkPhysicalDeviceMaintenance6Features>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceMaintenance6Features*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES: {
                    auto s = AllocMem<VkPhysicalDeviceMultiviewFeatures>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceMultiviewFeatures*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PERFORMANCE_QUERY_FEATURES_KHR: {
                    auto s = AllocMem<VkPhysicalDevicePerformanceQueryFeaturesKHR>();
                    *s = *reinterpret_cast<const VkPhysicalDevicePerformanceQueryFeaturesKHR*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_CREATION_CACHE_CONTROL_FEATURES: {
                    auto s = AllocMem<VkPhysicalDevicePipelineCreationCacheControlFeatures>();
                    *s = *reinterpret_cast<const VkPhysicalDevicePipelineCreationCacheControlFeatures*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_PROTECTED_ACCESS_FEATURES: {
                    auto s = AllocMem<VkPhysicalDevicePipelineProtectedAccessFeatures>();
                    *s = *reinterpret_cast<const VkPhysicalDevicePipelineProtectedAccessFeatures*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_ROBUSTNESS_FEATURES: {
                    auto s = AllocMem<VkPhysicalDevicePipelineRobustnessFeatures>();
                    *s = *reinterpret_cast<const VkPhysicalDevicePipelineRobustnessFeatures*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRIVATE_DATA_FEATURES: {
                    auto s = AllocMem<VkPhysicalDevicePrivateDataFeatures>();
                    *s = *reinterpret_cast<const VkPhysicalDevicePrivateDataFeatures*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROTECTED_MEMORY_FEATURES: {
                    auto s = AllocMem<VkPhysicalDeviceProtectedMemoryFeatures>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceProtectedMemoryFeatures*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_KHR: {
                    auto s = AllocMem<VkPhysicalDeviceRobustness2FeaturesKHR>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceRobustness2FeaturesKHR*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES: {
                    auto s = AllocMem<VkPhysicalDeviceSamplerYcbcrConversionFeatures>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceSamplerYcbcrConversionFeatures*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SCALAR_BLOCK_LAYOUT_FEATURES: {
                    auto s = AllocMem<VkPhysicalDeviceScalarBlockLayoutFeatures>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceScalarBlockLayoutFeatures*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SEPARATE_DEPTH_STENCIL_LAYOUTS_FEATURES: {
                    auto s = AllocMem<VkPhysicalDeviceSeparateDepthStencilLayoutsFeatures>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceSeparateDepthStencilLayoutsFeatures*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_FEATURES_EXT: {
                    auto s = AllocMem<VkPhysicalDeviceShaderAtomicFloatFeaturesEXT>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceShaderAtomicFloatFeaturesEXT*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_INT64_FEATURES: {
                    auto s = AllocMem<VkPhysicalDeviceShaderAtomicInt64Features>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceShaderAtomicInt64Features*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CLOCK_FEATURES_KHR: {
                    auto s = AllocMem<VkPhysicalDeviceShaderClockFeaturesKHR>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceShaderClockFeaturesKHR*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DEMOTE_TO_HELPER_INVOCATION_FEATURES: {
                    auto s = AllocMem<VkPhysicalDeviceShaderDemoteToHelperInvocationFeatures>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceShaderDemoteToHelperInvocationFeatures*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES: {
                    auto s = AllocMem<VkPhysicalDeviceShaderDrawParametersFeatures>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceShaderDrawParametersFeatures*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_EXPECT_ASSUME_FEATURES: {
                    auto s = AllocMem<VkPhysicalDeviceShaderExpectAssumeFeatures>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceShaderExpectAssumeFeatures*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT16_INT8_FEATURES: {
                    auto s = AllocMem<VkPhysicalDeviceShaderFloat16Int8Features>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceShaderFloat16Int8Features*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT_CONTROLS_2_FEATURES: {
                    auto s = AllocMem<VkPhysicalDeviceShaderFloatControls2Features>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceShaderFloatControls2Features*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_IMAGE_ATOMIC_INT64_FEATURES_EXT: {
                    auto s = AllocMem<VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_INTEGER_DOT_PRODUCT_FEATURES: {
                    auto s = AllocMem<VkPhysicalDeviceShaderIntegerDotProductFeatures>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceShaderIntegerDotProductFeatures*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_EXTENDED_TYPES_FEATURES: {
                    auto s = AllocMem<VkPhysicalDeviceShaderSubgroupExtendedTypesFeatures>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceShaderSubgroupExtendedTypesFeatures*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_ROTATE_FEATURES: {
                    auto s = AllocMem<VkPhysicalDeviceShaderSubgroupRotateFeatures>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceShaderSubgroupRotateFeatures*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_TERMINATE_INVOCATION_FEATURES: {
                    auto s = AllocMem<VkPhysicalDeviceShaderTerminateInvocationFeatures>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceShaderTerminateInvocationFeatures*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_FEATURES: {
                    auto s = AllocMem<VkPhysicalDeviceSubgroupSizeControlFeatures>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceSubgroupSizeControlFeatures*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES: {
                    auto s = AllocMem<VkPhysicalDeviceSynchronization2Features>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceSynchronization2Features*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXEL_BUFFER_ALIGNMENT_FEATURES_EXT: {
                    auto s = AllocMem<VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXTURE_COMPRESSION_ASTC_HDR_FEATURES: {
                    auto s = AllocMem<VkPhysicalDeviceTextureCompressionASTCHDRFeatures>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceTextureCompressionASTCHDRFeatures*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES: {
                    auto s = AllocMem<VkPhysicalDeviceTimelineSemaphoreFeatures>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceTimelineSemaphoreFeatures*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFORM_BUFFER_STANDARD_LAYOUT_FEATURES: {
                    auto s = AllocMem<VkPhysicalDeviceUniformBufferStandardLayoutFeatures>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceUniformBufferStandardLayoutFeatures*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTERS_FEATURES: {
                    auto s = AllocMem<VkPhysicalDeviceVariablePointersFeatures>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceVariablePointersFeatures*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_FEATURES: {
                    auto s = AllocMem<VkPhysicalDeviceVertexAttributeDivisorFeatures>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceVertexAttributeDivisorFeatures*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_INPUT_DYNAMIC_STATE_FEATURES_EXT: {
                    auto s = AllocMem<VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES: {
                    auto s = AllocMem<VkPhysicalDeviceVulkan11Features>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceVulkan11Features*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES: {
                    auto s = AllocMem<VkPhysicalDeviceVulkan12Features>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceVulkan12Features*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES: {
                    auto s = AllocMem<VkPhysicalDeviceVulkan13Features>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceVulkan13Features*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES: {
                    auto s = AllocMem<VkPhysicalDeviceVulkan14Features>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceVulkan14Features*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_MEMORY_MODEL_FEATURES: {
                    auto s = AllocMem<VkPhysicalDeviceVulkanMemoryModelFeatures>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceVulkanMemoryModelFeatures*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_SC_1_0_FEATURES: {
                    auto s = AllocMem<VkPhysicalDeviceVulkanSC10Features>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceVulkanSC10Features*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_YCBCR_2_PLANE_444_FORMATS_FEATURES_EXT: {
                    auto s = AllocMem<VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_YCBCR_IMAGE_ARRAYS_FEATURES_EXT: {
                    auto s = AllocMem<VkPhysicalDeviceYcbcrImageArraysFeaturesEXT>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceYcbcrImageArraysFeaturesEXT*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ZERO_INITIALIZE_WORKGROUP_MEMORY_FEATURES: {
                    auto s = AllocMem<VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeatures>();
                    *s = *reinterpret_cast<const VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeatures*>(p);
                    s->pNext = pnext;
                    pnext = s;
                    break;
                }

                default:
                    break;
            }
            p = p->pNext;
        }

        base->sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        base->pNext = pnext;

        return base;
    }
    Json::Value gen_int8_t(const int8_t v, const LocationScope&) { return v; }
    Json::Value gen_uint8_t(const uint8_t v, const LocationScope&) { return v; }
    Json::Value gen_int16_t(const int16_t v, const LocationScope&) { return v; }
    Json::Value gen_uint16_t(const uint16_t v, const LocationScope&) { return v; }
    Json::Value gen_int32_t(const int32_t v, const LocationScope&) { return v; }
    Json::Value gen_uint32_t(const uint32_t v, const LocationScope&) { return v; }
    Json::Value gen_int64_t(const int64_t v, const LocationScope&) { return v; }
    Json::Value gen_uint64_t(const uint64_t v, const LocationScope&) { return v; }
    Json::Value gen_float(const float v, const LocationScope&) {
        if (std::isnan(v))
            return "NaN";
        else
            return v;
    }
    Json::Value gen_size_t(const size_t v, const LocationScope&) { return v; }
    Json::Value gen_VkDeviceSize(const VkDeviceSize v, const LocationScope&) { return v; }
    Json::Value gen_VkSampleMask(const VkSampleMask v, const LocationScope&) { return v; }

    Json::Value gen_VkBool32(const VkBool32 v, const LocationScope&) { return Json::Value(v ? "VK_TRUE" : "VK_FALSE"); }

    std::string gen_string(const char* str) { return str; }

    Json::Value gen_binary(const void* ptr, const size_t size) {
        static const char base64_table[64] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                              'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                              'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                              'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};

        const uint8_t* data = reinterpret_cast<const uint8_t*>(ptr);
        size_t src_idx = 0;
        std::string result;
        result.reserve(size * 4);

        while (src_idx < size) {
            size_t num_read = std::min(size_t(3), size - src_idx);

            uint8_t s0 = data[src_idx];
            uint8_t s1 = (num_read >= 2) ? data[src_idx + 1] : 0;
            uint8_t s2 = (num_read >= 3) ? data[src_idx + 2] : 0;

            src_idx += num_read;

            result.push_back(base64_table[s0 >> 2]);
            result.push_back(base64_table[((s0 & 0x3) << 4) | (s1 >> 4)]);
            result.push_back(base64_table[((s1 & 0xF) << 2) | (s2 >> 6)]);
            result.push_back(base64_table[s2 & 0x3F]);

            if (num_read < 3) result[result.size() - 1] = '=';
            if (num_read < 2) result[result.size() - 2] = '=';
        }

        if (result != "NULL") {
            return result;
        }

        Json::Value result_array = Json::arrayValue;
        result_array.resize(Json::Value::ArrayIndex(size));
        for (size_t i = 0; i < size; i++) {
            result[i] = data[i];
        }
        return result_array;
    }

    Json::Value gen_VkPipelineCreateFlagBits(const VkPipelineCreateFlagBits v, const LocationScope&) {
        return gen_VkPipelineCreateFlagBits_c_str(v);
    }

    Json::Value gen_VkPipelineShaderStageCreateFlagBits(const VkPipelineShaderStageCreateFlagBits v, const LocationScope&) {
        return gen_VkPipelineShaderStageCreateFlagBits_c_str(v);
    }

    Json::Value gen_VkShaderStageFlagBits(const VkShaderStageFlagBits v, const LocationScope&) {
        return gen_VkShaderStageFlagBits_c_str(v);
    }

    Json::Value gen_VkObjectType(const VkObjectType v, const LocationScope&) {
        const char* enum_str = gen_VkObjectType_c_str(v);
        return enum_str ? Json::Value(enum_str) : Json::Value(v);
    }

    Json::Value gen_VkPipelineRobustnessBufferBehavior(const VkPipelineRobustnessBufferBehavior v, const LocationScope&) {
        const char* enum_str = gen_VkPipelineRobustnessBufferBehavior_c_str(v);
        return enum_str ? Json::Value(enum_str) : Json::Value(v);
    }

    Json::Value gen_VkPipelineRobustnessImageBehavior(const VkPipelineRobustnessImageBehavior v, const LocationScope&) {
        const char* enum_str = gen_VkPipelineRobustnessImageBehavior_c_str(v);
        return enum_str ? Json::Value(enum_str) : Json::Value(v);
    }

    Json::Value gen_VkVertexInputRate(const VkVertexInputRate v, const LocationScope&) {
        const char* enum_str = gen_VkVertexInputRate_c_str(v);
        return enum_str ? Json::Value(enum_str) : Json::Value(v);
    }

    Json::Value gen_VkFormat(const VkFormat v, const LocationScope&) {
        const char* enum_str = gen_VkFormat_c_str(v);
        return enum_str ? Json::Value(enum_str) : Json::Value(v);
    }

    Json::Value gen_VkPrimitiveTopology(const VkPrimitiveTopology v, const LocationScope&) {
        const char* enum_str = gen_VkPrimitiveTopology_c_str(v);
        return enum_str ? Json::Value(enum_str) : Json::Value(v);
    }

    Json::Value gen_VkTessellationDomainOrigin(const VkTessellationDomainOrigin v, const LocationScope&) {
        const char* enum_str = gen_VkTessellationDomainOrigin_c_str(v);
        return enum_str ? Json::Value(enum_str) : Json::Value(v);
    }

    Json::Value gen_VkPolygonMode(const VkPolygonMode v, const LocationScope&) {
        const char* enum_str = gen_VkPolygonMode_c_str(v);
        return enum_str ? Json::Value(enum_str) : Json::Value(v);
    }

    Json::Value gen_VkCullModeFlagBits(const VkCullModeFlagBits v, const LocationScope&) { return gen_VkCullModeFlagBits_c_str(v); }

    Json::Value gen_VkFrontFace(const VkFrontFace v, const LocationScope&) {
        const char* enum_str = gen_VkFrontFace_c_str(v);
        return enum_str ? Json::Value(enum_str) : Json::Value(v);
    }

    Json::Value gen_VkConservativeRasterizationModeEXT(const VkConservativeRasterizationModeEXT v, const LocationScope&) {
        const char* enum_str = gen_VkConservativeRasterizationModeEXT_c_str(v);
        return enum_str ? Json::Value(enum_str) : Json::Value(v);
    }

    Json::Value gen_VkLineRasterizationMode(const VkLineRasterizationMode v, const LocationScope&) {
        const char* enum_str = gen_VkLineRasterizationMode_c_str(v);
        return enum_str ? Json::Value(enum_str) : Json::Value(v);
    }

    Json::Value gen_VkSampleCountFlagBits(const VkSampleCountFlagBits v, const LocationScope&) {
        return gen_VkSampleCountFlagBits_c_str(v);
    }

    Json::Value gen_VkCompareOp(const VkCompareOp v, const LocationScope&) {
        const char* enum_str = gen_VkCompareOp_c_str(v);
        return enum_str ? Json::Value(enum_str) : Json::Value(v);
    }

    Json::Value gen_VkStencilOp(const VkStencilOp v, const LocationScope&) {
        const char* enum_str = gen_VkStencilOp_c_str(v);
        return enum_str ? Json::Value(enum_str) : Json::Value(v);
    }

    Json::Value gen_VkLogicOp(const VkLogicOp v, const LocationScope&) {
        const char* enum_str = gen_VkLogicOp_c_str(v);
        return enum_str ? Json::Value(enum_str) : Json::Value(v);
    }

    Json::Value gen_VkBlendFactor(const VkBlendFactor v, const LocationScope&) {
        const char* enum_str = gen_VkBlendFactor_c_str(v);
        return enum_str ? Json::Value(enum_str) : Json::Value(v);
    }

    Json::Value gen_VkBlendOp(const VkBlendOp v, const LocationScope&) {
        const char* enum_str = gen_VkBlendOp_c_str(v);
        return enum_str ? Json::Value(enum_str) : Json::Value(v);
    }

    Json::Value gen_VkColorComponentFlagBits(const VkColorComponentFlagBits v, const LocationScope&) {
        return gen_VkColorComponentFlagBits_c_str(v);
    }

    Json::Value gen_VkBlendOverlapEXT(const VkBlendOverlapEXT v, const LocationScope&) {
        const char* enum_str = gen_VkBlendOverlapEXT_c_str(v);
        return enum_str ? Json::Value(enum_str) : Json::Value(v);
    }

    Json::Value gen_VkDynamicState(const VkDynamicState v, const LocationScope&) {
        const char* enum_str = gen_VkDynamicState_c_str(v);
        return enum_str ? Json::Value(enum_str) : Json::Value(v);
    }

    Json::Value gen_VkPipelineCreateFlagBits2(const VkPipelineCreateFlagBits2 v, const LocationScope&) {
        return gen_VkPipelineCreateFlagBits2_c_str(v);
    }

    Json::Value gen_VkPipelineCreationFeedbackFlagBits(const VkPipelineCreationFeedbackFlagBits v, const LocationScope&) {
        return gen_VkPipelineCreationFeedbackFlagBits_c_str(v);
    }

    Json::Value gen_VkDiscardRectangleModeEXT(const VkDiscardRectangleModeEXT v, const LocationScope&) {
        const char* enum_str = gen_VkDiscardRectangleModeEXT_c_str(v);
        return enum_str ? Json::Value(enum_str) : Json::Value(v);
    }

    Json::Value gen_VkFragmentShadingRateCombinerOpKHR(const VkFragmentShadingRateCombinerOpKHR v, const LocationScope&) {
        const char* enum_str = gen_VkFragmentShadingRateCombinerOpKHR_c_str(v);
        return enum_str ? Json::Value(enum_str) : Json::Value(v);
    }

    Json::Value gen_VkPipelineMatchControl(const VkPipelineMatchControl v, const LocationScope&) {
        const char* enum_str = gen_VkPipelineMatchControl_c_str(v);
        return enum_str ? Json::Value(enum_str) : Json::Value(v);
    }

    Json::Value gen_VkSamplerYcbcrModelConversion(const VkSamplerYcbcrModelConversion v, const LocationScope&) {
        const char* enum_str = gen_VkSamplerYcbcrModelConversion_c_str(v);
        return enum_str ? Json::Value(enum_str) : Json::Value(v);
    }

    Json::Value gen_VkSamplerYcbcrRange(const VkSamplerYcbcrRange v, const LocationScope&) {
        const char* enum_str = gen_VkSamplerYcbcrRange_c_str(v);
        return enum_str ? Json::Value(enum_str) : Json::Value(v);
    }

    Json::Value gen_VkComponentSwizzle(const VkComponentSwizzle v, const LocationScope&) {
        const char* enum_str = gen_VkComponentSwizzle_c_str(v);
        return enum_str ? Json::Value(enum_str) : Json::Value(v);
    }

    Json::Value gen_VkChromaLocation(const VkChromaLocation v, const LocationScope&) {
        const char* enum_str = gen_VkChromaLocation_c_str(v);
        return enum_str ? Json::Value(enum_str) : Json::Value(v);
    }

    Json::Value gen_VkFilter(const VkFilter v, const LocationScope&) {
        const char* enum_str = gen_VkFilter_c_str(v);
        return enum_str ? Json::Value(enum_str) : Json::Value(v);
    }

    Json::Value gen_VkSamplerCreateFlagBits(const VkSamplerCreateFlagBits v, const LocationScope&) {
        return gen_VkSamplerCreateFlagBits_c_str(v);
    }

    Json::Value gen_VkSamplerMipmapMode(const VkSamplerMipmapMode v, const LocationScope&) {
        const char* enum_str = gen_VkSamplerMipmapMode_c_str(v);
        return enum_str ? Json::Value(enum_str) : Json::Value(v);
    }

    Json::Value gen_VkSamplerAddressMode(const VkSamplerAddressMode v, const LocationScope&) {
        const char* enum_str = gen_VkSamplerAddressMode_c_str(v);
        return enum_str ? Json::Value(enum_str) : Json::Value(v);
    }

    Json::Value gen_VkBorderColor(const VkBorderColor v, const LocationScope&) {
        const char* enum_str = gen_VkBorderColor_c_str(v);
        return enum_str ? Json::Value(enum_str) : Json::Value(v);
    }

    Json::Value gen_VkSamplerReductionMode(const VkSamplerReductionMode v, const LocationScope&) {
        const char* enum_str = gen_VkSamplerReductionMode_c_str(v);
        return enum_str ? Json::Value(enum_str) : Json::Value(v);
    }

    Json::Value gen_VkDescriptorSetLayoutCreateFlagBits(const VkDescriptorSetLayoutCreateFlagBits v, const LocationScope&) {
        return gen_VkDescriptorSetLayoutCreateFlagBits_c_str(v);
    }

    Json::Value gen_VkDescriptorType(const VkDescriptorType v, const LocationScope&) {
        const char* enum_str = gen_VkDescriptorType_c_str(v);
        return enum_str ? Json::Value(enum_str) : Json::Value(v);
    }

    Json::Value gen_VkDescriptorBindingFlagBits(const VkDescriptorBindingFlagBits v, const LocationScope&) {
        return gen_VkDescriptorBindingFlagBits_c_str(v);
    }

    Json::Value gen_VkPipelineLayoutCreateFlagBits(const VkPipelineLayoutCreateFlagBits v, const LocationScope&) {
        return gen_VkPipelineLayoutCreateFlagBits_c_str(v);
    }

    Json::Value gen_VkRenderPassCreateFlagBits(const VkRenderPassCreateFlagBits v, const LocationScope&) {
        return gen_VkRenderPassCreateFlagBits_c_str(v);
    }

    Json::Value gen_VkAttachmentDescriptionFlagBits(const VkAttachmentDescriptionFlagBits v, const LocationScope&) {
        return gen_VkAttachmentDescriptionFlagBits_c_str(v);
    }

    Json::Value gen_VkAttachmentLoadOp(const VkAttachmentLoadOp v, const LocationScope&) {
        const char* enum_str = gen_VkAttachmentLoadOp_c_str(v);
        return enum_str ? Json::Value(enum_str) : Json::Value(v);
    }

    Json::Value gen_VkAttachmentStoreOp(const VkAttachmentStoreOp v, const LocationScope&) {
        const char* enum_str = gen_VkAttachmentStoreOp_c_str(v);
        return enum_str ? Json::Value(enum_str) : Json::Value(v);
    }

    Json::Value gen_VkImageLayout(const VkImageLayout v, const LocationScope&) {
        const char* enum_str = gen_VkImageLayout_c_str(v);
        return enum_str ? Json::Value(enum_str) : Json::Value(v);
    }

    Json::Value gen_VkSubpassDescriptionFlagBits(const VkSubpassDescriptionFlagBits v, const LocationScope&) {
        return gen_VkSubpassDescriptionFlagBits_c_str(v);
    }

    Json::Value gen_VkPipelineBindPoint(const VkPipelineBindPoint v, const LocationScope&) {
        const char* enum_str = gen_VkPipelineBindPoint_c_str(v);
        return enum_str ? Json::Value(enum_str) : Json::Value(v);
    }

    Json::Value gen_VkPipelineStageFlagBits(const VkPipelineStageFlagBits v, const LocationScope&) {
        return gen_VkPipelineStageFlagBits_c_str(v);
    }

    Json::Value gen_VkAccessFlagBits(const VkAccessFlagBits v, const LocationScope&) { return gen_VkAccessFlagBits_c_str(v); }

    Json::Value gen_VkDependencyFlagBits(const VkDependencyFlagBits v, const LocationScope&) {
        return gen_VkDependencyFlagBits_c_str(v);
    }

    Json::Value gen_VkImageAspectFlagBits(const VkImageAspectFlagBits v, const LocationScope&) {
        return gen_VkImageAspectFlagBits_c_str(v);
    }

    Json::Value gen_VkResolveModeFlagBits(const VkResolveModeFlagBits v, const LocationScope&) {
        return gen_VkResolveModeFlagBits_c_str(v);
    }

    Json::Value gen_VkPipelineStageFlagBits2(const VkPipelineStageFlagBits2 v, const LocationScope&) {
        return gen_VkPipelineStageFlagBits2_c_str(v);
    }

    Json::Value gen_VkAccessFlagBits2(const VkAccessFlagBits2 v, const LocationScope&) { return gen_VkAccessFlagBits2_c_str(v); }

    Json::Value gen_VkPipelineCacheCreateFlagBits(const VkPipelineCacheCreateFlagBits v, const LocationScope&) {
        return gen_VkPipelineCacheCreateFlagBits_c_str(v);
    }

    Json::Value gen_VkPipelineCreateFlags(const VkPipelineCreateFlags v, const LocationScope&) {
        if (!v) {
            return 0;
        }
        std::stringstream strm;
        for (int i = 0; i < 31; ++i) {
            auto bit = uint32_t(1) << i;
            if ((v & bit) != 0) {
                if (strm.rdbuf()->in_avail() > 0) {
                    strm << " | ";
                }
                strm << gen_VkPipelineCreateFlagBits_c_str(static_cast<VkPipelineCreateFlagBits>(bit));
            }
        }

        return strm.str();
    }

    Json::Value gen_VkPipelineShaderStageCreateFlags(const VkPipelineShaderStageCreateFlags v, const LocationScope&) {
        if (!v) {
            return 0;
        }
        std::stringstream strm;
        for (int i = 0; i < 31; ++i) {
            auto bit = uint32_t(1) << i;
            if ((v & bit) != 0) {
                if (strm.rdbuf()->in_avail() > 0) {
                    strm << " | ";
                }
                strm << gen_VkPipelineShaderStageCreateFlagBits_c_str(static_cast<VkPipelineShaderStageCreateFlagBits>(bit));
            }
        }

        return strm.str();
    }

    Json::Value gen_VkPipelineVertexInputStateCreateFlags(const VkPipelineVertexInputStateCreateFlags v, const LocationScope&) {
        if (!v) {
            return 0;
        }

        Error() << "Invalid flags value";
        return "Invalid flags value";
    }

    Json::Value gen_VkPipelineInputAssemblyStateCreateFlags(const VkPipelineInputAssemblyStateCreateFlags v, const LocationScope&) {
        if (!v) {
            return 0;
        }

        Error() << "Invalid flags value";
        return "Invalid flags value";
    }

    Json::Value gen_VkPipelineTessellationStateCreateFlags(const VkPipelineTessellationStateCreateFlags v, const LocationScope&) {
        if (!v) {
            return 0;
        }

        Error() << "Invalid flags value";
        return "Invalid flags value";
    }

    Json::Value gen_VkPipelineViewportStateCreateFlags(const VkPipelineViewportStateCreateFlags v, const LocationScope&) {
        if (!v) {
            return 0;
        }

        Error() << "Invalid flags value";
        return "Invalid flags value";
    }

    Json::Value gen_VkPipelineRasterizationStateCreateFlags(const VkPipelineRasterizationStateCreateFlags v, const LocationScope&) {
        if (!v) {
            return 0;
        }

        Error() << "Invalid flags value";
        return "Invalid flags value";
    }

    Json::Value gen_VkCullModeFlags(const VkCullModeFlags v, const LocationScope&) {
        if (!v) {
            return 0;
        }
        std::stringstream strm;
        std::array<VkCullModeFlagBits, 1> multi_bit_flags{{VkCullModeFlagBits::VK_CULL_MODE_FRONT_AND_BACK}};
        std::vector<VkCullModeFlagBits> matched_multi_bit_flags;
        for (auto multi_bit_flag : multi_bit_flags) {
            if (v == multi_bit_flag) {
                matched_multi_bit_flags.push_back(multi_bit_flag);
                if (strm.rdbuf()->in_avail() > 0) {
                    strm << " | ";
                }
                strm << gen_VkCullModeFlagBits_c_str(static_cast<VkCullModeFlagBits>(multi_bit_flag));
            }
        }
        auto isnt_part_of_any_matched_multi_bit_flags = [&](const auto bit) {
            return std::none_of(matched_multi_bit_flags.begin(), matched_multi_bit_flags.end(),
                                [bit](const auto multi_bit_flag) { return multi_bit_flag & bit; });
        };
        for (int i = 0; i < 31; ++i) {
            auto bit = uint32_t(1) << i;
            if ((v & bit) != 0 && isnt_part_of_any_matched_multi_bit_flags(bit)) {
                if (strm.rdbuf()->in_avail() > 0) {
                    strm << " | ";
                }
                strm << gen_VkCullModeFlagBits_c_str(static_cast<VkCullModeFlagBits>(bit));
            }
        }

        return strm.str();
    }

    Json::Value gen_VkPipelineRasterizationConservativeStateCreateFlagsEXT(
        const VkPipelineRasterizationConservativeStateCreateFlagsEXT v, const LocationScope&) {
        if (!v) {
            return 0;
        }

        Error() << "Invalid flags value";
        return "Invalid flags value";
    }

    Json::Value gen_VkPipelineRasterizationDepthClipStateCreateFlagsEXT(const VkPipelineRasterizationDepthClipStateCreateFlagsEXT v,
                                                                        const LocationScope&) {
        if (!v) {
            return 0;
        }

        Error() << "Invalid flags value";
        return "Invalid flags value";
    }

    Json::Value gen_VkPipelineMultisampleStateCreateFlags(const VkPipelineMultisampleStateCreateFlags v, const LocationScope&) {
        if (!v) {
            return 0;
        }

        Error() << "Invalid flags value";
        return "Invalid flags value";
    }

    Json::Value gen_VkPipelineDepthStencilStateCreateFlags(const VkPipelineDepthStencilStateCreateFlags v, const LocationScope&) {
        if (!v) {
            return 0;
        }

        Error() << "Invalid flags value";
        return "Invalid flags value";
    }

    Json::Value gen_VkPipelineColorBlendStateCreateFlags(const VkPipelineColorBlendStateCreateFlags v, const LocationScope&) {
        if (!v) {
            return 0;
        }

        Error() << "Invalid flags value";
        return "Invalid flags value";
    }

    Json::Value gen_VkColorComponentFlags(const VkColorComponentFlags v, const LocationScope&) {
        if (!v) {
            return 0;
        }
        std::stringstream strm;
        for (int i = 0; i < 31; ++i) {
            auto bit = uint32_t(1) << i;
            if ((v & bit) != 0) {
                if (strm.rdbuf()->in_avail() > 0) {
                    strm << " | ";
                }
                strm << gen_VkColorComponentFlagBits_c_str(static_cast<VkColorComponentFlagBits>(bit));
            }
        }

        return strm.str();
    }

    Json::Value gen_VkPipelineDynamicStateCreateFlags(const VkPipelineDynamicStateCreateFlags v, const LocationScope&) {
        if (!v) {
            return 0;
        }

        Error() << "Invalid flags value";
        return "Invalid flags value";
    }

    Json::Value gen_VkPipelineCreateFlags2(const VkPipelineCreateFlags2 v, const LocationScope&) {
        if (!v) {
            return 0;
        }
        std::stringstream strm;
        for (int i = 0; i < 63; ++i) {
            auto bit = uint64_t(1) << i;
            if ((v & bit) != 0) {
                if (strm.rdbuf()->in_avail() > 0) {
                    strm << " | ";
                }
                strm << gen_VkPipelineCreateFlagBits2_c_str(static_cast<VkPipelineCreateFlagBits2>(bit));
            }
        }

        return strm.str();
    }

    Json::Value gen_VkPipelineCreationFeedbackFlags(const VkPipelineCreationFeedbackFlags v, const LocationScope&) {
        if (!v) {
            return 0;
        }
        std::stringstream strm;
        for (int i = 0; i < 31; ++i) {
            auto bit = uint32_t(1) << i;
            if ((v & bit) != 0) {
                if (strm.rdbuf()->in_avail() > 0) {
                    strm << " | ";
                }
                strm << gen_VkPipelineCreationFeedbackFlagBits_c_str(static_cast<VkPipelineCreationFeedbackFlagBits>(bit));
            }
        }

        return strm.str();
    }

    Json::Value gen_VkPipelineDiscardRectangleStateCreateFlagsEXT(const VkPipelineDiscardRectangleStateCreateFlagsEXT v,
                                                                  const LocationScope&) {
        if (!v) {
            return 0;
        }

        Error() << "Invalid flags value";
        return "Invalid flags value";
    }

    Json::Value gen_VkSamplerCreateFlags(const VkSamplerCreateFlags v, const LocationScope&) {
        if (!v) {
            return 0;
        }
        std::stringstream strm;
        for (int i = 0; i < 31; ++i) {
            auto bit = uint32_t(1) << i;
            if ((v & bit) != 0) {
                if (strm.rdbuf()->in_avail() > 0) {
                    strm << " | ";
                }
                strm << gen_VkSamplerCreateFlagBits_c_str(static_cast<VkSamplerCreateFlagBits>(bit));
            }
        }

        return strm.str();
    }

    Json::Value gen_VkDescriptorSetLayoutCreateFlags(const VkDescriptorSetLayoutCreateFlags v, const LocationScope&) {
        if (!v) {
            return 0;
        }
        std::stringstream strm;
        for (int i = 0; i < 31; ++i) {
            auto bit = uint32_t(1) << i;
            if ((v & bit) != 0) {
                if (strm.rdbuf()->in_avail() > 0) {
                    strm << " | ";
                }
                strm << gen_VkDescriptorSetLayoutCreateFlagBits_c_str(static_cast<VkDescriptorSetLayoutCreateFlagBits>(bit));
            }
        }

        return strm.str();
    }

    Json::Value gen_VkShaderStageFlags(const VkShaderStageFlags v, const LocationScope&) {
        if (!v) {
            return 0;
        }
        std::stringstream strm;
        std::array<VkShaderStageFlagBits, 2> multi_bit_flags{
            {VkShaderStageFlagBits::VK_SHADER_STAGE_ALL_GRAPHICS, VkShaderStageFlagBits::VK_SHADER_STAGE_ALL}};
        std::vector<VkShaderStageFlagBits> matched_multi_bit_flags;
        for (auto multi_bit_flag : multi_bit_flags) {
            if (v == multi_bit_flag) {
                matched_multi_bit_flags.push_back(multi_bit_flag);
                if (strm.rdbuf()->in_avail() > 0) {
                    strm << " | ";
                }
                strm << gen_VkShaderStageFlagBits_c_str(static_cast<VkShaderStageFlagBits>(multi_bit_flag));
            }
        }
        auto isnt_part_of_any_matched_multi_bit_flags = [&](const auto bit) {
            return std::none_of(matched_multi_bit_flags.begin(), matched_multi_bit_flags.end(),
                                [bit](const auto multi_bit_flag) { return multi_bit_flag & bit; });
        };
        for (int i = 0; i < 31; ++i) {
            auto bit = uint32_t(1) << i;
            if ((v & bit) != 0 && isnt_part_of_any_matched_multi_bit_flags(bit)) {
                if (strm.rdbuf()->in_avail() > 0) {
                    strm << " | ";
                }
                strm << gen_VkShaderStageFlagBits_c_str(static_cast<VkShaderStageFlagBits>(bit));
            }
        }

        return strm.str();
    }

    Json::Value gen_VkDescriptorBindingFlags(const VkDescriptorBindingFlags v, const LocationScope&) {
        if (!v) {
            return 0;
        }
        std::stringstream strm;
        for (int i = 0; i < 31; ++i) {
            auto bit = uint32_t(1) << i;
            if ((v & bit) != 0) {
                if (strm.rdbuf()->in_avail() > 0) {
                    strm << " | ";
                }
                strm << gen_VkDescriptorBindingFlagBits_c_str(static_cast<VkDescriptorBindingFlagBits>(bit));
            }
        }

        return strm.str();
    }

    Json::Value gen_VkPipelineLayoutCreateFlags(const VkPipelineLayoutCreateFlags v, const LocationScope&) {
        if (!v) {
            return 0;
        }
        std::stringstream strm;
        for (int i = 0; i < 31; ++i) {
            auto bit = uint32_t(1) << i;
            if ((v & bit) != 0) {
                if (strm.rdbuf()->in_avail() > 0) {
                    strm << " | ";
                }
                strm << gen_VkPipelineLayoutCreateFlagBits_c_str(static_cast<VkPipelineLayoutCreateFlagBits>(bit));
            }
        }

        return strm.str();
    }

    Json::Value gen_VkRenderPassCreateFlags(const VkRenderPassCreateFlags v, const LocationScope&) {
        if (!v) {
            return 0;
        }
        std::stringstream strm;
        for (int i = 0; i < 31; ++i) {
            auto bit = uint32_t(1) << i;
            if ((v & bit) != 0) {
                if (strm.rdbuf()->in_avail() > 0) {
                    strm << " | ";
                }
                strm << gen_VkRenderPassCreateFlagBits_c_str(static_cast<VkRenderPassCreateFlagBits>(bit));
            }
        }

        return strm.str();
    }

    Json::Value gen_VkAttachmentDescriptionFlags(const VkAttachmentDescriptionFlags v, const LocationScope&) {
        if (!v) {
            return 0;
        }
        std::stringstream strm;
        for (int i = 0; i < 31; ++i) {
            auto bit = uint32_t(1) << i;
            if ((v & bit) != 0) {
                if (strm.rdbuf()->in_avail() > 0) {
                    strm << " | ";
                }
                strm << gen_VkAttachmentDescriptionFlagBits_c_str(static_cast<VkAttachmentDescriptionFlagBits>(bit));
            }
        }

        return strm.str();
    }

    Json::Value gen_VkSubpassDescriptionFlags(const VkSubpassDescriptionFlags v, const LocationScope&) {
        if (!v) {
            return 0;
        }
        std::stringstream strm;
        for (int i = 0; i < 31; ++i) {
            auto bit = uint32_t(1) << i;
            if ((v & bit) != 0) {
                if (strm.rdbuf()->in_avail() > 0) {
                    strm << " | ";
                }
                strm << gen_VkSubpassDescriptionFlagBits_c_str(static_cast<VkSubpassDescriptionFlagBits>(bit));
            }
        }

        return strm.str();
    }

    Json::Value gen_VkPipelineStageFlags(const VkPipelineStageFlags v, const LocationScope&) {
        if (!v) {
            return 0;
        }
        std::stringstream strm;
        for (int i = 0; i < 31; ++i) {
            auto bit = uint32_t(1) << i;
            if ((v & bit) != 0) {
                if (strm.rdbuf()->in_avail() > 0) {
                    strm << " | ";
                }
                strm << gen_VkPipelineStageFlagBits_c_str(static_cast<VkPipelineStageFlagBits>(bit));
            }
        }

        return strm.str();
    }

    Json::Value gen_VkAccessFlags(const VkAccessFlags v, const LocationScope&) {
        if (!v) {
            return 0;
        }
        std::stringstream strm;
        for (int i = 0; i < 31; ++i) {
            auto bit = uint32_t(1) << i;
            if ((v & bit) != 0) {
                if (strm.rdbuf()->in_avail() > 0) {
                    strm << " | ";
                }
                strm << gen_VkAccessFlagBits_c_str(static_cast<VkAccessFlagBits>(bit));
            }
        }

        return strm.str();
    }

    Json::Value gen_VkDependencyFlags(const VkDependencyFlags v, const LocationScope&) {
        if (!v) {
            return 0;
        }
        std::stringstream strm;
        for (int i = 0; i < 31; ++i) {
            auto bit = uint32_t(1) << i;
            if ((v & bit) != 0) {
                if (strm.rdbuf()->in_avail() > 0) {
                    strm << " | ";
                }
                strm << gen_VkDependencyFlagBits_c_str(static_cast<VkDependencyFlagBits>(bit));
            }
        }

        return strm.str();
    }

    Json::Value gen_VkImageAspectFlags(const VkImageAspectFlags v, const LocationScope&) {
        if (!v) {
            return 0;
        }
        std::stringstream strm;
        for (int i = 0; i < 31; ++i) {
            auto bit = uint32_t(1) << i;
            if ((v & bit) != 0) {
                if (strm.rdbuf()->in_avail() > 0) {
                    strm << " | ";
                }
                strm << gen_VkImageAspectFlagBits_c_str(static_cast<VkImageAspectFlagBits>(bit));
            }
        }

        return strm.str();
    }

    Json::Value gen_VkPipelineStageFlags2(const VkPipelineStageFlags2 v, const LocationScope&) {
        if (!v) {
            return 0;
        }
        std::stringstream strm;
        for (int i = 0; i < 63; ++i) {
            auto bit = uint64_t(1) << i;
            if ((v & bit) != 0) {
                if (strm.rdbuf()->in_avail() > 0) {
                    strm << " | ";
                }
                strm << gen_VkPipelineStageFlagBits2_c_str(static_cast<VkPipelineStageFlagBits2>(bit));
            }
        }

        return strm.str();
    }

    Json::Value gen_VkAccessFlags2(const VkAccessFlags2 v, const LocationScope&) {
        if (!v) {
            return 0;
        }
        std::stringstream strm;
        for (int i = 0; i < 63; ++i) {
            auto bit = uint64_t(1) << i;
            if ((v & bit) != 0) {
                if (strm.rdbuf()->in_avail() > 0) {
                    strm << " | ";
                }
                strm << gen_VkAccessFlagBits2_c_str(static_cast<VkAccessFlagBits2>(bit));
            }
        }

        return strm.str();
    }

    Json::Value gen_VkPipelineCacheCreateFlags(const VkPipelineCacheCreateFlags v, const LocationScope&) {
        if (!v) {
            return 0;
        }
        std::stringstream strm;
        for (int i = 0; i < 31; ++i) {
            auto bit = uint32_t(1) << i;
            if ((v & bit) != 0) {
                if (strm.rdbuf()->in_avail() > 0) {
                    strm << " | ";
                }
                strm << gen_VkPipelineCacheCreateFlagBits_c_str(static_cast<VkPipelineCacheCreateFlagBits>(bit));
            }
        }

        return strm.str();
    }

    Json::Value gen_VkShaderModuleCreateInfo(const VkShaderModuleCreateInfo& s, const LocationScope& l) {
        Json::Value json = Json::objectValue;
        json["sType"] = "VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO";
        json["pNext"] = "NULL";
        json["flags"] = 0;
        json["codeSize"] = s.codeSize;
        json["pCode"] = gen_binary(s.pCode, s.codeSize);
        return json;
    }

    Json::Value gen_VkPipelineShaderStageCreateInfo(const VkPipelineShaderStageCreateInfo& s, const LocationScope& l) {
        Json::Value json = gen_VkPipelineShaderStageCreateInfo_contents(s, l);

        json["sType"] = "VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO";

        auto next = reinterpret_cast<const VkBaseInStructure*>(s.pNext);
        Json::Value* json_next = &json["pNext"];

        while (next != nullptr) {
            switch (next->sType) {
                case VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT: {
                    *json_next =
                        gen_VkDebugUtilsObjectNameInfoEXT_contents(*reinterpret_cast<const VkDebugUtilsObjectNameInfoEXT*>(next),
                                                                   CreateScope("pNext<VkDebugUtilsObjectNameInfoEXT>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT";
                    break;
                }

                case VK_STRUCTURE_TYPE_PIPELINE_ROBUSTNESS_CREATE_INFO: {
                    *json_next =
                        gen_VkPipelineRobustnessCreateInfo_contents(*reinterpret_cast<const VkPipelineRobustnessCreateInfo*>(next),
                                                                    CreateScope("pNext<VkPipelineRobustnessCreateInfo>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PIPELINE_ROBUSTNESS_CREATE_INFO";
                    break;
                }

                case VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_REQUIRED_SUBGROUP_SIZE_CREATE_INFO: {
                    *json_next = gen_VkPipelineShaderStageRequiredSubgroupSizeCreateInfo_contents(
                        *reinterpret_cast<const VkPipelineShaderStageRequiredSubgroupSizeCreateInfo*>(next),
                        CreateScope("pNext<VkPipelineShaderStageRequiredSubgroupSizeCreateInfo>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_REQUIRED_SUBGROUP_SIZE_CREATE_INFO";
                    break;
                }

                default:
                    Error() << "Invalid structure type extending VkPipelineShaderStageCreateInfo: " << next->sType;
                    break;
            }
            next = next->pNext;
            if (!json_next->isNull()) {
                json_next = &(*json_next)["pNext"];
            }
        }

        *json_next = "NULL";

        return json;
    };

    Json::Value gen_VkPipelineVertexInputStateCreateInfo(const VkPipelineVertexInputStateCreateInfo& s, const LocationScope& l) {
        Json::Value json = gen_VkPipelineVertexInputStateCreateInfo_contents(s, l);

        json["sType"] = "VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO";

        auto next = reinterpret_cast<const VkBaseInStructure*>(s.pNext);
        Json::Value* json_next = &json["pNext"];

        while (next != nullptr) {
            switch (next->sType) {
                case VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_DIVISOR_STATE_CREATE_INFO: {
                    *json_next = gen_VkPipelineVertexInputDivisorStateCreateInfo_contents(
                        *reinterpret_cast<const VkPipelineVertexInputDivisorStateCreateInfo*>(next),
                        CreateScope("pNext<VkPipelineVertexInputDivisorStateCreateInfo>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_DIVISOR_STATE_CREATE_INFO";
                    break;
                }

                default:
                    Error() << "Invalid structure type extending VkPipelineVertexInputStateCreateInfo: " << next->sType;
                    break;
            }
            next = next->pNext;
            if (!json_next->isNull()) {
                json_next = &(*json_next)["pNext"];
            }
        }

        *json_next = "NULL";

        return json;
    };

    Json::Value gen_VkPipelineInputAssemblyStateCreateInfo(const VkPipelineInputAssemblyStateCreateInfo& s,
                                                           const LocationScope& l) {
        Json::Value json = gen_VkPipelineInputAssemblyStateCreateInfo_contents(s, l);

        json["sType"] = "VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO";

        auto next = reinterpret_cast<const VkBaseInStructure*>(s.pNext);
        Json::Value* json_next = &json["pNext"];

        while (next != nullptr) {
            switch (next->sType) {
                default:
                    Error() << "Invalid structure type extending VkPipelineInputAssemblyStateCreateInfo: " << next->sType;
                    break;
            }
            next = next->pNext;
            if (!json_next->isNull()) {
                json_next = &(*json_next)["pNext"];
            }
        }

        *json_next = "NULL";

        return json;
    };

    Json::Value gen_VkPipelineTessellationStateCreateInfo(const VkPipelineTessellationStateCreateInfo& s, const LocationScope& l) {
        Json::Value json = gen_VkPipelineTessellationStateCreateInfo_contents(s, l);

        json["sType"] = "VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO";

        auto next = reinterpret_cast<const VkBaseInStructure*>(s.pNext);
        Json::Value* json_next = &json["pNext"];

        while (next != nullptr) {
            switch (next->sType) {
                case VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_DOMAIN_ORIGIN_STATE_CREATE_INFO: {
                    *json_next = gen_VkPipelineTessellationDomainOriginStateCreateInfo_contents(
                        *reinterpret_cast<const VkPipelineTessellationDomainOriginStateCreateInfo*>(next),
                        CreateScope("pNext<VkPipelineTessellationDomainOriginStateCreateInfo>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_DOMAIN_ORIGIN_STATE_CREATE_INFO";
                    break;
                }

                default:
                    Error() << "Invalid structure type extending VkPipelineTessellationStateCreateInfo: " << next->sType;
                    break;
            }
            next = next->pNext;
            if (!json_next->isNull()) {
                json_next = &(*json_next)["pNext"];
            }
        }

        *json_next = "NULL";

        return json;
    };

    Json::Value gen_VkPipelineViewportStateCreateInfo(const VkPipelineViewportStateCreateInfo& s, const LocationScope& l) {
        Json::Value json = gen_VkPipelineViewportStateCreateInfo_contents(s, l);

        json["sType"] = "VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO";

        auto next = reinterpret_cast<const VkBaseInStructure*>(s.pNext);
        Json::Value* json_next = &json["pNext"];

        while (next != nullptr) {
            switch (next->sType) {
                default:
                    Error() << "Invalid structure type extending VkPipelineViewportStateCreateInfo: " << next->sType;
                    break;
            }
            next = next->pNext;
            if (!json_next->isNull()) {
                json_next = &(*json_next)["pNext"];
            }
        }

        *json_next = "NULL";

        return json;
    };

    Json::Value gen_VkPipelineRasterizationStateCreateInfo(const VkPipelineRasterizationStateCreateInfo& s,
                                                           const LocationScope& l) {
        Json::Value json = gen_VkPipelineRasterizationStateCreateInfo_contents(s, l);

        json["sType"] = "VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO";

        auto next = reinterpret_cast<const VkBaseInStructure*>(s.pNext);
        Json::Value* json_next = &json["pNext"];

        while (next != nullptr) {
            switch (next->sType) {
                case VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_CONSERVATIVE_STATE_CREATE_INFO_EXT: {
                    *json_next = gen_VkPipelineRasterizationConservativeStateCreateInfoEXT_contents(
                        *reinterpret_cast<const VkPipelineRasterizationConservativeStateCreateInfoEXT*>(next),
                        CreateScope("pNext<VkPipelineRasterizationConservativeStateCreateInfoEXT>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_CONSERVATIVE_STATE_CREATE_INFO_EXT";
                    break;
                }

                case VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_DEPTH_CLIP_STATE_CREATE_INFO_EXT: {
                    *json_next = gen_VkPipelineRasterizationDepthClipStateCreateInfoEXT_contents(
                        *reinterpret_cast<const VkPipelineRasterizationDepthClipStateCreateInfoEXT*>(next),
                        CreateScope("pNext<VkPipelineRasterizationDepthClipStateCreateInfoEXT>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_DEPTH_CLIP_STATE_CREATE_INFO_EXT";
                    break;
                }

                case VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_LINE_STATE_CREATE_INFO: {
                    *json_next = gen_VkPipelineRasterizationLineStateCreateInfo_contents(
                        *reinterpret_cast<const VkPipelineRasterizationLineStateCreateInfo*>(next),
                        CreateScope("pNext<VkPipelineRasterizationLineStateCreateInfo>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_LINE_STATE_CREATE_INFO";
                    break;
                }

                default:
                    Error() << "Invalid structure type extending VkPipelineRasterizationStateCreateInfo: " << next->sType;
                    break;
            }
            next = next->pNext;
            if (!json_next->isNull()) {
                json_next = &(*json_next)["pNext"];
            }
        }

        *json_next = "NULL";

        return json;
    };

    Json::Value gen_VkSampleLocationsInfoEXT(const VkSampleLocationsInfoEXT& s, const LocationScope& l) {
        Json::Value json = gen_VkSampleLocationsInfoEXT_contents(s, l);

        json["sType"] = "VK_STRUCTURE_TYPE_SAMPLE_LOCATIONS_INFO_EXT";

        auto next = reinterpret_cast<const VkBaseInStructure*>(s.pNext);
        Json::Value* json_next = &json["pNext"];

        while (next != nullptr) {
            switch (next->sType) {
                default:
                    Error() << "Invalid structure type extending VkSampleLocationsInfoEXT: " << next->sType;
                    break;
            }
            next = next->pNext;
            if (!json_next->isNull()) {
                json_next = &(*json_next)["pNext"];
            }
        }

        *json_next = "NULL";

        return json;
    };

    Json::Value gen_VkPipelineMultisampleStateCreateInfo(const VkPipelineMultisampleStateCreateInfo& s, const LocationScope& l) {
        Json::Value json = gen_VkPipelineMultisampleStateCreateInfo_contents(s, l);

        json["sType"] = "VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO";

        auto next = reinterpret_cast<const VkBaseInStructure*>(s.pNext);
        Json::Value* json_next = &json["pNext"];

        while (next != nullptr) {
            switch (next->sType) {
                case VK_STRUCTURE_TYPE_PIPELINE_SAMPLE_LOCATIONS_STATE_CREATE_INFO_EXT: {
                    *json_next = gen_VkPipelineSampleLocationsStateCreateInfoEXT_contents(
                        *reinterpret_cast<const VkPipelineSampleLocationsStateCreateInfoEXT*>(next),
                        CreateScope("pNext<VkPipelineSampleLocationsStateCreateInfoEXT>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PIPELINE_SAMPLE_LOCATIONS_STATE_CREATE_INFO_EXT";
                    break;
                }

                default:
                    Error() << "Invalid structure type extending VkPipelineMultisampleStateCreateInfo: " << next->sType;
                    break;
            }
            next = next->pNext;
            if (!json_next->isNull()) {
                json_next = &(*json_next)["pNext"];
            }
        }

        *json_next = "NULL";

        return json;
    };

    Json::Value gen_VkPipelineDepthStencilStateCreateInfo(const VkPipelineDepthStencilStateCreateInfo& s, const LocationScope& l) {
        Json::Value json = gen_VkPipelineDepthStencilStateCreateInfo_contents(s, l);

        json["sType"] = "VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO";

        auto next = reinterpret_cast<const VkBaseInStructure*>(s.pNext);
        Json::Value* json_next = &json["pNext"];

        while (next != nullptr) {
            switch (next->sType) {
                default:
                    Error() << "Invalid structure type extending VkPipelineDepthStencilStateCreateInfo: " << next->sType;
                    break;
            }
            next = next->pNext;
            if (!json_next->isNull()) {
                json_next = &(*json_next)["pNext"];
            }
        }

        *json_next = "NULL";

        return json;
    };

    Json::Value gen_VkPipelineColorBlendStateCreateInfo(const VkPipelineColorBlendStateCreateInfo& s, const LocationScope& l) {
        Json::Value json = gen_VkPipelineColorBlendStateCreateInfo_contents(s, l);

        json["sType"] = "VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO";

        auto next = reinterpret_cast<const VkBaseInStructure*>(s.pNext);
        Json::Value* json_next = &json["pNext"];

        while (next != nullptr) {
            switch (next->sType) {
                case VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_ADVANCED_STATE_CREATE_INFO_EXT: {
                    *json_next = gen_VkPipelineColorBlendAdvancedStateCreateInfoEXT_contents(
                        *reinterpret_cast<const VkPipelineColorBlendAdvancedStateCreateInfoEXT*>(next),
                        CreateScope("pNext<VkPipelineColorBlendAdvancedStateCreateInfoEXT>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_ADVANCED_STATE_CREATE_INFO_EXT";
                    break;
                }

                case VK_STRUCTURE_TYPE_PIPELINE_COLOR_WRITE_CREATE_INFO_EXT: {
                    *json_next = gen_VkPipelineColorWriteCreateInfoEXT_contents(
                        *reinterpret_cast<const VkPipelineColorWriteCreateInfoEXT*>(next),
                        CreateScope("pNext<VkPipelineColorWriteCreateInfoEXT>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PIPELINE_COLOR_WRITE_CREATE_INFO_EXT";
                    break;
                }

                default:
                    Error() << "Invalid structure type extending VkPipelineColorBlendStateCreateInfo: " << next->sType;
                    break;
            }
            next = next->pNext;
            if (!json_next->isNull()) {
                json_next = &(*json_next)["pNext"];
            }
        }

        *json_next = "NULL";

        return json;
    };

    Json::Value gen_VkPipelineDynamicStateCreateInfo(const VkPipelineDynamicStateCreateInfo& s, const LocationScope& l) {
        Json::Value json = gen_VkPipelineDynamicStateCreateInfo_contents(s, l);

        json["sType"] = "VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO";

        auto next = reinterpret_cast<const VkBaseInStructure*>(s.pNext);
        Json::Value* json_next = &json["pNext"];

        while (next != nullptr) {
            switch (next->sType) {
                default:
                    Error() << "Invalid structure type extending VkPipelineDynamicStateCreateInfo: " << next->sType;
                    break;
            }
            next = next->pNext;
            if (!json_next->isNull()) {
                json_next = &(*json_next)["pNext"];
            }
        }

        *json_next = "NULL";

        return json;
    };

    Json::Value gen_VkGraphicsPipelineCreateInfo(const VkGraphicsPipelineCreateInfo& s, const LocationScope& l) {
        Json::Value json = gen_VkGraphicsPipelineCreateInfo_contents(s, l);

        json["sType"] = "VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO";

        auto next = reinterpret_cast<const VkBaseInStructure*>(s.pNext);
        Json::Value* json_next = &json["pNext"];

        while (next != nullptr) {
            switch (next->sType) {
                case VK_STRUCTURE_TYPE_PIPELINE_CREATE_FLAGS_2_CREATE_INFO: {
                    *json_next = gen_VkPipelineCreateFlags2CreateInfo_contents(
                        *reinterpret_cast<const VkPipelineCreateFlags2CreateInfo*>(next),
                        CreateScope("pNext<VkPipelineCreateFlags2CreateInfo>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PIPELINE_CREATE_FLAGS_2_CREATE_INFO";
                    break;
                }

                case VK_STRUCTURE_TYPE_PIPELINE_CREATION_FEEDBACK_CREATE_INFO: {
                    *json_next = gen_VkPipelineCreationFeedbackCreateInfo_contents(
                        *reinterpret_cast<const VkPipelineCreationFeedbackCreateInfo*>(next),
                        CreateScope("pNext<VkPipelineCreationFeedbackCreateInfo>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PIPELINE_CREATION_FEEDBACK_CREATE_INFO";
                    break;
                }

                case VK_STRUCTURE_TYPE_PIPELINE_DISCARD_RECTANGLE_STATE_CREATE_INFO_EXT: {
                    *json_next = gen_VkPipelineDiscardRectangleStateCreateInfoEXT_contents(
                        *reinterpret_cast<const VkPipelineDiscardRectangleStateCreateInfoEXT*>(next),
                        CreateScope("pNext<VkPipelineDiscardRectangleStateCreateInfoEXT>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PIPELINE_DISCARD_RECTANGLE_STATE_CREATE_INFO_EXT";
                    break;
                }

                case VK_STRUCTURE_TYPE_PIPELINE_FRAGMENT_SHADING_RATE_STATE_CREATE_INFO_KHR: {
                    *json_next = gen_VkPipelineFragmentShadingRateStateCreateInfoKHR_contents(
                        *reinterpret_cast<const VkPipelineFragmentShadingRateStateCreateInfoKHR*>(next),
                        CreateScope("pNext<VkPipelineFragmentShadingRateStateCreateInfoKHR>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PIPELINE_FRAGMENT_SHADING_RATE_STATE_CREATE_INFO_KHR";
                    break;
                }

                case VK_STRUCTURE_TYPE_PIPELINE_OFFLINE_CREATE_INFO: {
                    *json_next =
                        gen_VkPipelineOfflineCreateInfo_contents(*reinterpret_cast<const VkPipelineOfflineCreateInfo*>(next),
                                                                 CreateScope("pNext<VkPipelineOfflineCreateInfo>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PIPELINE_OFFLINE_CREATE_INFO";
                    break;
                }

                case VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO: {
                    *json_next =
                        gen_VkPipelineRenderingCreateInfo_contents(*reinterpret_cast<const VkPipelineRenderingCreateInfo*>(next),
                                                                   CreateScope("pNext<VkPipelineRenderingCreateInfo>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO";
                    break;
                }

                case VK_STRUCTURE_TYPE_PIPELINE_ROBUSTNESS_CREATE_INFO: {
                    *json_next =
                        gen_VkPipelineRobustnessCreateInfo_contents(*reinterpret_cast<const VkPipelineRobustnessCreateInfo*>(next),
                                                                    CreateScope("pNext<VkPipelineRobustnessCreateInfo>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PIPELINE_ROBUSTNESS_CREATE_INFO";
                    break;
                }

                case VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_LOCATION_INFO: {
                    *json_next = gen_VkRenderingAttachmentLocationInfo_contents(
                        *reinterpret_cast<const VkRenderingAttachmentLocationInfo*>(next),
                        CreateScope("pNext<VkRenderingAttachmentLocationInfo>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_LOCATION_INFO";
                    break;
                }

                case VK_STRUCTURE_TYPE_RENDERING_INPUT_ATTACHMENT_INDEX_INFO: {
                    *json_next = gen_VkRenderingInputAttachmentIndexInfo_contents(
                        *reinterpret_cast<const VkRenderingInputAttachmentIndexInfo*>(next),
                        CreateScope("pNext<VkRenderingInputAttachmentIndexInfo>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_RENDERING_INPUT_ATTACHMENT_INDEX_INFO";
                    break;
                }

                default:
                    Error() << "Invalid structure type extending VkGraphicsPipelineCreateInfo: " << next->sType;
                    break;
            }
            next = next->pNext;
            if (!json_next->isNull()) {
                json_next = &(*json_next)["pNext"];
            }
        }

        *json_next = "NULL";

        return json;
    };

    Json::Value gen_VkComputePipelineCreateInfo(const VkComputePipelineCreateInfo& s, const LocationScope& l) {
        Json::Value json = gen_VkComputePipelineCreateInfo_contents(s, l);

        json["sType"] = "VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO";

        auto next = reinterpret_cast<const VkBaseInStructure*>(s.pNext);
        Json::Value* json_next = &json["pNext"];

        while (next != nullptr) {
            switch (next->sType) {
                case VK_STRUCTURE_TYPE_PIPELINE_CREATE_FLAGS_2_CREATE_INFO: {
                    *json_next = gen_VkPipelineCreateFlags2CreateInfo_contents(
                        *reinterpret_cast<const VkPipelineCreateFlags2CreateInfo*>(next),
                        CreateScope("pNext<VkPipelineCreateFlags2CreateInfo>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PIPELINE_CREATE_FLAGS_2_CREATE_INFO";
                    break;
                }

                case VK_STRUCTURE_TYPE_PIPELINE_CREATION_FEEDBACK_CREATE_INFO: {
                    *json_next = gen_VkPipelineCreationFeedbackCreateInfo_contents(
                        *reinterpret_cast<const VkPipelineCreationFeedbackCreateInfo*>(next),
                        CreateScope("pNext<VkPipelineCreationFeedbackCreateInfo>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PIPELINE_CREATION_FEEDBACK_CREATE_INFO";
                    break;
                }

                case VK_STRUCTURE_TYPE_PIPELINE_OFFLINE_CREATE_INFO: {
                    *json_next =
                        gen_VkPipelineOfflineCreateInfo_contents(*reinterpret_cast<const VkPipelineOfflineCreateInfo*>(next),
                                                                 CreateScope("pNext<VkPipelineOfflineCreateInfo>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PIPELINE_OFFLINE_CREATE_INFO";
                    break;
                }

                case VK_STRUCTURE_TYPE_PIPELINE_ROBUSTNESS_CREATE_INFO: {
                    *json_next =
                        gen_VkPipelineRobustnessCreateInfo_contents(*reinterpret_cast<const VkPipelineRobustnessCreateInfo*>(next),
                                                                    CreateScope("pNext<VkPipelineRobustnessCreateInfo>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PIPELINE_ROBUSTNESS_CREATE_INFO";
                    break;
                }

                default:
                    Error() << "Invalid structure type extending VkComputePipelineCreateInfo: " << next->sType;
                    break;
            }
            next = next->pNext;
            if (!json_next->isNull()) {
                json_next = &(*json_next)["pNext"];
            }
        }

        *json_next = "NULL";

        return json;
    };

    Json::Value gen_VkSamplerYcbcrConversionCreateInfo(const VkSamplerYcbcrConversionCreateInfo& s, const LocationScope& l) {
        Json::Value json = gen_VkSamplerYcbcrConversionCreateInfo_contents(s, l);

        json["sType"] = "VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_CREATE_INFO";

        auto next = reinterpret_cast<const VkBaseInStructure*>(s.pNext);
        Json::Value* json_next = &json["pNext"];

        while (next != nullptr) {
            switch (next->sType) {
#ifdef VK_USE_PLATFORM_SCREEN_QNX

                case VK_STRUCTURE_TYPE_EXTERNAL_FORMAT_QNX: {
                    *json_next = gen_VkExternalFormatQNX_contents(*reinterpret_cast<const VkExternalFormatQNX*>(next),
                                                                  CreateScope("pNext<VkExternalFormatQNX>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_EXTERNAL_FORMAT_QNX";
                    break;
                }
#endif  // VK_USE_PLATFORM_SCREEN_QNX

                default:
                    Error() << "Invalid structure type extending VkSamplerYcbcrConversionCreateInfo: " << next->sType;
                    break;
            }
            next = next->pNext;
            if (!json_next->isNull()) {
                json_next = &(*json_next)["pNext"];
            }
        }

        *json_next = "NULL";

        return json;
    };

    Json::Value gen_VkSamplerCreateInfo(const VkSamplerCreateInfo& s, const LocationScope& l) {
        Json::Value json = gen_VkSamplerCreateInfo_contents(s, l);

        json["sType"] = "VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO";

        auto next = reinterpret_cast<const VkBaseInStructure*>(s.pNext);
        Json::Value* json_next = &json["pNext"];

        while (next != nullptr) {
            switch (next->sType) {
                case VK_STRUCTURE_TYPE_SAMPLER_CUSTOM_BORDER_COLOR_CREATE_INFO_EXT: {
                    *json_next = gen_VkSamplerCustomBorderColorCreateInfoEXT_contents(
                        *reinterpret_cast<const VkSamplerCustomBorderColorCreateInfoEXT*>(next),
                        CreateScope("pNext<VkSamplerCustomBorderColorCreateInfoEXT>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_SAMPLER_CUSTOM_BORDER_COLOR_CREATE_INFO_EXT";
                    break;
                }

                case VK_STRUCTURE_TYPE_SAMPLER_REDUCTION_MODE_CREATE_INFO: {
                    *json_next = gen_VkSamplerReductionModeCreateInfo_contents(
                        *reinterpret_cast<const VkSamplerReductionModeCreateInfo*>(next),
                        CreateScope("pNext<VkSamplerReductionModeCreateInfo>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_SAMPLER_REDUCTION_MODE_CREATE_INFO";
                    break;
                }

                case VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_INFO: {
                    *json_next =
                        gen_VkSamplerYcbcrConversionInfo_contents(*reinterpret_cast<const VkSamplerYcbcrConversionInfo*>(next),
                                                                  CreateScope("pNext<VkSamplerYcbcrConversionInfo>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_INFO";
                    break;
                }

                default:
                    Error() << "Invalid structure type extending VkSamplerCreateInfo: " << next->sType;
                    break;
            }
            next = next->pNext;
            if (!json_next->isNull()) {
                json_next = &(*json_next)["pNext"];
            }
        }

        *json_next = "NULL";

        return json;
    };

    Json::Value gen_VkDescriptorSetLayoutCreateInfo(const VkDescriptorSetLayoutCreateInfo& s, const LocationScope& l) {
        Json::Value json = gen_VkDescriptorSetLayoutCreateInfo_contents(s, l);

        json["sType"] = "VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO";

        auto next = reinterpret_cast<const VkBaseInStructure*>(s.pNext);
        Json::Value* json_next = &json["pNext"];

        while (next != nullptr) {
            switch (next->sType) {
                case VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO: {
                    *json_next = gen_VkDescriptorSetLayoutBindingFlagsCreateInfo_contents(
                        *reinterpret_cast<const VkDescriptorSetLayoutBindingFlagsCreateInfo*>(next),
                        CreateScope("pNext<VkDescriptorSetLayoutBindingFlagsCreateInfo>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO";
                    break;
                }

                default:
                    Error() << "Invalid structure type extending VkDescriptorSetLayoutCreateInfo: " << next->sType;
                    break;
            }
            next = next->pNext;
            if (!json_next->isNull()) {
                json_next = &(*json_next)["pNext"];
            }
        }

        *json_next = "NULL";

        return json;
    };

    Json::Value gen_VkPipelineLayoutCreateInfo(const VkPipelineLayoutCreateInfo& s, const LocationScope& l) {
        Json::Value json = gen_VkPipelineLayoutCreateInfo_contents(s, l);

        json["sType"] = "VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO";

        auto next = reinterpret_cast<const VkBaseInStructure*>(s.pNext);
        Json::Value* json_next = &json["pNext"];

        while (next != nullptr) {
            switch (next->sType) {
                default:
                    Error() << "Invalid structure type extending VkPipelineLayoutCreateInfo: " << next->sType;
                    break;
            }
            next = next->pNext;
            if (!json_next->isNull()) {
                json_next = &(*json_next)["pNext"];
            }
        }

        *json_next = "NULL";

        return json;
    };

    Json::Value gen_VkPhysicalDeviceFeatures2(const VkPhysicalDeviceFeatures2& s, const LocationScope& l) {
        Json::Value json = gen_VkPhysicalDeviceFeatures2_contents(s, l);

        json["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2";

        auto next = reinterpret_cast<const VkBaseInStructure*>(s.pNext);
        Json::Value* json_next = &json["pNext"];

        while (next != nullptr) {
            switch (next->sType) {
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES: {
                    *json_next = gen_VkPhysicalDevice16BitStorageFeatures_contents(
                        *reinterpret_cast<const VkPhysicalDevice16BitStorageFeatures*>(next),
                        CreateScope("pNext<VkPhysicalDevice16BitStorageFeatures>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_4444_FORMATS_FEATURES_EXT: {
                    *json_next = gen_VkPhysicalDevice4444FormatsFeaturesEXT_contents(
                        *reinterpret_cast<const VkPhysicalDevice4444FormatsFeaturesEXT*>(next),
                        CreateScope("pNext<VkPhysicalDevice4444FormatsFeaturesEXT>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_4444_FORMATS_FEATURES_EXT";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES: {
                    *json_next = gen_VkPhysicalDevice8BitStorageFeatures_contents(
                        *reinterpret_cast<const VkPhysicalDevice8BitStorageFeatures*>(next),
                        CreateScope("pNext<VkPhysicalDevice8BitStorageFeatures>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ASTC_DECODE_FEATURES_EXT: {
                    *json_next = gen_VkPhysicalDeviceASTCDecodeFeaturesEXT_contents(
                        *reinterpret_cast<const VkPhysicalDeviceASTCDecodeFeaturesEXT*>(next),
                        CreateScope("pNext<VkPhysicalDeviceASTCDecodeFeaturesEXT>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ASTC_DECODE_FEATURES_EXT";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BLEND_OPERATION_ADVANCED_FEATURES_EXT: {
                    *json_next = gen_VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT_contents(
                        *reinterpret_cast<const VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT*>(next),
                        CreateScope("pNext<VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BLEND_OPERATION_ADVANCED_FEATURES_EXT";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES: {
                    *json_next = gen_VkPhysicalDeviceBufferDeviceAddressFeatures_contents(
                        *reinterpret_cast<const VkPhysicalDeviceBufferDeviceAddressFeatures*>(next),
                        CreateScope("pNext<VkPhysicalDeviceBufferDeviceAddressFeatures>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COLOR_WRITE_ENABLE_FEATURES_EXT: {
                    *json_next = gen_VkPhysicalDeviceColorWriteEnableFeaturesEXT_contents(
                        *reinterpret_cast<const VkPhysicalDeviceColorWriteEnableFeaturesEXT*>(next),
                        CreateScope("pNext<VkPhysicalDeviceColorWriteEnableFeaturesEXT>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COLOR_WRITE_ENABLE_FEATURES_EXT";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CUSTOM_BORDER_COLOR_FEATURES_EXT: {
                    *json_next = gen_VkPhysicalDeviceCustomBorderColorFeaturesEXT_contents(
                        *reinterpret_cast<const VkPhysicalDeviceCustomBorderColorFeaturesEXT*>(next),
                        CreateScope("pNext<VkPhysicalDeviceCustomBorderColorFeaturesEXT>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_CUSTOM_BORDER_COLOR_FEATURES_EXT";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLIP_ENABLE_FEATURES_EXT: {
                    *json_next = gen_VkPhysicalDeviceDepthClipEnableFeaturesEXT_contents(
                        *reinterpret_cast<const VkPhysicalDeviceDepthClipEnableFeaturesEXT*>(next),
                        CreateScope("pNext<VkPhysicalDeviceDepthClipEnableFeaturesEXT>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLIP_ENABLE_FEATURES_EXT";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES: {
                    *json_next = gen_VkPhysicalDeviceDescriptorIndexingFeatures_contents(
                        *reinterpret_cast<const VkPhysicalDeviceDescriptorIndexingFeatures*>(next),
                        CreateScope("pNext<VkPhysicalDeviceDescriptorIndexingFeatures>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES: {
                    *json_next = gen_VkPhysicalDeviceDynamicRenderingFeatures_contents(
                        *reinterpret_cast<const VkPhysicalDeviceDynamicRenderingFeatures*>(next),
                        CreateScope("pNext<VkPhysicalDeviceDynamicRenderingFeatures>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_LOCAL_READ_FEATURES: {
                    *json_next = gen_VkPhysicalDeviceDynamicRenderingLocalReadFeatures_contents(
                        *reinterpret_cast<const VkPhysicalDeviceDynamicRenderingLocalReadFeatures*>(next),
                        CreateScope("pNext<VkPhysicalDeviceDynamicRenderingLocalReadFeatures>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_LOCAL_READ_FEATURES";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_2_FEATURES_EXT: {
                    *json_next = gen_VkPhysicalDeviceExtendedDynamicState2FeaturesEXT_contents(
                        *reinterpret_cast<const VkPhysicalDeviceExtendedDynamicState2FeaturesEXT*>(next),
                        CreateScope("pNext<VkPhysicalDeviceExtendedDynamicState2FeaturesEXT>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_2_FEATURES_EXT";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT: {
                    *json_next = gen_VkPhysicalDeviceExtendedDynamicStateFeaturesEXT_contents(
                        *reinterpret_cast<const VkPhysicalDeviceExtendedDynamicStateFeaturesEXT*>(next),
                        CreateScope("pNext<VkPhysicalDeviceExtendedDynamicStateFeaturesEXT>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT";
                    break;
                }
#ifdef VK_USE_PLATFORM_SCI

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_MEMORY_SCI_BUF_FEATURES_NV: {
                    *json_next = gen_VkPhysicalDeviceExternalMemorySciBufFeaturesNV_contents(
                        *reinterpret_cast<const VkPhysicalDeviceExternalMemorySciBufFeaturesNV*>(next),
                        CreateScope("pNext<VkPhysicalDeviceExternalMemorySciBufFeaturesNV>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_MEMORY_SCI_BUF_FEATURES_NV";
                    break;
                }
#endif  // VK_USE_PLATFORM_SCI
#ifdef VK_USE_PLATFORM_SCREEN_QNX

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_MEMORY_SCREEN_BUFFER_FEATURES_QNX: {
                    *json_next = gen_VkPhysicalDeviceExternalMemoryScreenBufferFeaturesQNX_contents(
                        *reinterpret_cast<const VkPhysicalDeviceExternalMemoryScreenBufferFeaturesQNX*>(next),
                        CreateScope("pNext<VkPhysicalDeviceExternalMemoryScreenBufferFeaturesQNX>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_MEMORY_SCREEN_BUFFER_FEATURES_QNX";
                    break;
                }
#endif  // VK_USE_PLATFORM_SCREEN_QNX
#ifdef VK_USE_PLATFORM_SCI

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_SCI_SYNC_2_FEATURES_NV: {
                    *json_next = gen_VkPhysicalDeviceExternalSciSync2FeaturesNV_contents(
                        *reinterpret_cast<const VkPhysicalDeviceExternalSciSync2FeaturesNV*>(next),
                        CreateScope("pNext<VkPhysicalDeviceExternalSciSync2FeaturesNV>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_SCI_SYNC_2_FEATURES_NV";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_SCI_SYNC_FEATURES_NV: {
                    *json_next = gen_VkPhysicalDeviceExternalSciSyncFeaturesNV_contents(
                        *reinterpret_cast<const VkPhysicalDeviceExternalSciSyncFeaturesNV*>(next),
                        CreateScope("pNext<VkPhysicalDeviceExternalSciSyncFeaturesNV>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_SCI_SYNC_FEATURES_NV";
                    break;
                }
#endif  // VK_USE_PLATFORM_SCI

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_INTERLOCK_FEATURES_EXT: {
                    *json_next = gen_VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT_contents(
                        *reinterpret_cast<const VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT*>(next),
                        CreateScope("pNext<VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_INTERLOCK_FEATURES_EXT";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR: {
                    *json_next = gen_VkPhysicalDeviceFragmentShadingRateFeaturesKHR_contents(
                        *reinterpret_cast<const VkPhysicalDeviceFragmentShadingRateFeaturesKHR*>(next),
                        CreateScope("pNext<VkPhysicalDeviceFragmentShadingRateFeaturesKHR>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GLOBAL_PRIORITY_QUERY_FEATURES: {
                    *json_next = gen_VkPhysicalDeviceGlobalPriorityQueryFeatures_contents(
                        *reinterpret_cast<const VkPhysicalDeviceGlobalPriorityQueryFeatures*>(next),
                        CreateScope("pNext<VkPhysicalDeviceGlobalPriorityQueryFeatures>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GLOBAL_PRIORITY_QUERY_FEATURES";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_IMAGE_COPY_FEATURES: {
                    *json_next = gen_VkPhysicalDeviceHostImageCopyFeatures_contents(
                        *reinterpret_cast<const VkPhysicalDeviceHostImageCopyFeatures*>(next),
                        CreateScope("pNext<VkPhysicalDeviceHostImageCopyFeatures>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_IMAGE_COPY_FEATURES";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES: {
                    *json_next = gen_VkPhysicalDeviceHostQueryResetFeatures_contents(
                        *reinterpret_cast<const VkPhysicalDeviceHostQueryResetFeatures*>(next),
                        CreateScope("pNext<VkPhysicalDeviceHostQueryResetFeatures>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_ROBUSTNESS_FEATURES: {
                    *json_next = gen_VkPhysicalDeviceImageRobustnessFeatures_contents(
                        *reinterpret_cast<const VkPhysicalDeviceImageRobustnessFeatures*>(next),
                        CreateScope("pNext<VkPhysicalDeviceImageRobustnessFeatures>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_ROBUSTNESS_FEATURES";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES: {
                    *json_next = gen_VkPhysicalDeviceImagelessFramebufferFeatures_contents(
                        *reinterpret_cast<const VkPhysicalDeviceImagelessFramebufferFeatures*>(next),
                        CreateScope("pNext<VkPhysicalDeviceImagelessFramebufferFeatures>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INDEX_TYPE_UINT8_FEATURES: {
                    *json_next = gen_VkPhysicalDeviceIndexTypeUint8Features_contents(
                        *reinterpret_cast<const VkPhysicalDeviceIndexTypeUint8Features*>(next),
                        CreateScope("pNext<VkPhysicalDeviceIndexTypeUint8Features>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INDEX_TYPE_UINT8_FEATURES";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_FEATURES: {
                    *json_next = gen_VkPhysicalDeviceInlineUniformBlockFeatures_contents(
                        *reinterpret_cast<const VkPhysicalDeviceInlineUniformBlockFeatures*>(next),
                        CreateScope("pNext<VkPhysicalDeviceInlineUniformBlockFeatures>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INLINE_UNIFORM_BLOCK_FEATURES";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINE_RASTERIZATION_FEATURES: {
                    *json_next = gen_VkPhysicalDeviceLineRasterizationFeatures_contents(
                        *reinterpret_cast<const VkPhysicalDeviceLineRasterizationFeatures*>(next),
                        CreateScope("pNext<VkPhysicalDeviceLineRasterizationFeatures>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINE_RASTERIZATION_FEATURES";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_4_FEATURES: {
                    *json_next = gen_VkPhysicalDeviceMaintenance4Features_contents(
                        *reinterpret_cast<const VkPhysicalDeviceMaintenance4Features*>(next),
                        CreateScope("pNext<VkPhysicalDeviceMaintenance4Features>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_4_FEATURES";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_5_FEATURES: {
                    *json_next = gen_VkPhysicalDeviceMaintenance5Features_contents(
                        *reinterpret_cast<const VkPhysicalDeviceMaintenance5Features*>(next),
                        CreateScope("pNext<VkPhysicalDeviceMaintenance5Features>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_5_FEATURES";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_6_FEATURES: {
                    *json_next = gen_VkPhysicalDeviceMaintenance6Features_contents(
                        *reinterpret_cast<const VkPhysicalDeviceMaintenance6Features*>(next),
                        CreateScope("pNext<VkPhysicalDeviceMaintenance6Features>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_6_FEATURES";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES: {
                    *json_next = gen_VkPhysicalDeviceMultiviewFeatures_contents(
                        *reinterpret_cast<const VkPhysicalDeviceMultiviewFeatures*>(next),
                        CreateScope("pNext<VkPhysicalDeviceMultiviewFeatures>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PERFORMANCE_QUERY_FEATURES_KHR: {
                    *json_next = gen_VkPhysicalDevicePerformanceQueryFeaturesKHR_contents(
                        *reinterpret_cast<const VkPhysicalDevicePerformanceQueryFeaturesKHR*>(next),
                        CreateScope("pNext<VkPhysicalDevicePerformanceQueryFeaturesKHR>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PERFORMANCE_QUERY_FEATURES_KHR";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_CREATION_CACHE_CONTROL_FEATURES: {
                    *json_next = gen_VkPhysicalDevicePipelineCreationCacheControlFeatures_contents(
                        *reinterpret_cast<const VkPhysicalDevicePipelineCreationCacheControlFeatures*>(next),
                        CreateScope("pNext<VkPhysicalDevicePipelineCreationCacheControlFeatures>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_CREATION_CACHE_CONTROL_FEATURES";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_PROTECTED_ACCESS_FEATURES: {
                    *json_next = gen_VkPhysicalDevicePipelineProtectedAccessFeatures_contents(
                        *reinterpret_cast<const VkPhysicalDevicePipelineProtectedAccessFeatures*>(next),
                        CreateScope("pNext<VkPhysicalDevicePipelineProtectedAccessFeatures>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_PROTECTED_ACCESS_FEATURES";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_ROBUSTNESS_FEATURES: {
                    *json_next = gen_VkPhysicalDevicePipelineRobustnessFeatures_contents(
                        *reinterpret_cast<const VkPhysicalDevicePipelineRobustnessFeatures*>(next),
                        CreateScope("pNext<VkPhysicalDevicePipelineRobustnessFeatures>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PIPELINE_ROBUSTNESS_FEATURES";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRIVATE_DATA_FEATURES: {
                    *json_next = gen_VkPhysicalDevicePrivateDataFeatures_contents(
                        *reinterpret_cast<const VkPhysicalDevicePrivateDataFeatures*>(next),
                        CreateScope("pNext<VkPhysicalDevicePrivateDataFeatures>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRIVATE_DATA_FEATURES";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROTECTED_MEMORY_FEATURES: {
                    *json_next = gen_VkPhysicalDeviceProtectedMemoryFeatures_contents(
                        *reinterpret_cast<const VkPhysicalDeviceProtectedMemoryFeatures*>(next),
                        CreateScope("pNext<VkPhysicalDeviceProtectedMemoryFeatures>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROTECTED_MEMORY_FEATURES";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_KHR: {
                    *json_next = gen_VkPhysicalDeviceRobustness2FeaturesKHR_contents(
                        *reinterpret_cast<const VkPhysicalDeviceRobustness2FeaturesKHR*>(next),
                        CreateScope("pNext<VkPhysicalDeviceRobustness2FeaturesKHR>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_KHR";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES: {
                    *json_next = gen_VkPhysicalDeviceSamplerYcbcrConversionFeatures_contents(
                        *reinterpret_cast<const VkPhysicalDeviceSamplerYcbcrConversionFeatures*>(next),
                        CreateScope("pNext<VkPhysicalDeviceSamplerYcbcrConversionFeatures>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SCALAR_BLOCK_LAYOUT_FEATURES: {
                    *json_next = gen_VkPhysicalDeviceScalarBlockLayoutFeatures_contents(
                        *reinterpret_cast<const VkPhysicalDeviceScalarBlockLayoutFeatures*>(next),
                        CreateScope("pNext<VkPhysicalDeviceScalarBlockLayoutFeatures>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SCALAR_BLOCK_LAYOUT_FEATURES";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SEPARATE_DEPTH_STENCIL_LAYOUTS_FEATURES: {
                    *json_next = gen_VkPhysicalDeviceSeparateDepthStencilLayoutsFeatures_contents(
                        *reinterpret_cast<const VkPhysicalDeviceSeparateDepthStencilLayoutsFeatures*>(next),
                        CreateScope("pNext<VkPhysicalDeviceSeparateDepthStencilLayoutsFeatures>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SEPARATE_DEPTH_STENCIL_LAYOUTS_FEATURES";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_FEATURES_EXT: {
                    *json_next = gen_VkPhysicalDeviceShaderAtomicFloatFeaturesEXT_contents(
                        *reinterpret_cast<const VkPhysicalDeviceShaderAtomicFloatFeaturesEXT*>(next),
                        CreateScope("pNext<VkPhysicalDeviceShaderAtomicFloatFeaturesEXT>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_FEATURES_EXT";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_INT64_FEATURES: {
                    *json_next = gen_VkPhysicalDeviceShaderAtomicInt64Features_contents(
                        *reinterpret_cast<const VkPhysicalDeviceShaderAtomicInt64Features*>(next),
                        CreateScope("pNext<VkPhysicalDeviceShaderAtomicInt64Features>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_INT64_FEATURES";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CLOCK_FEATURES_KHR: {
                    *json_next = gen_VkPhysicalDeviceShaderClockFeaturesKHR_contents(
                        *reinterpret_cast<const VkPhysicalDeviceShaderClockFeaturesKHR*>(next),
                        CreateScope("pNext<VkPhysicalDeviceShaderClockFeaturesKHR>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CLOCK_FEATURES_KHR";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DEMOTE_TO_HELPER_INVOCATION_FEATURES: {
                    *json_next = gen_VkPhysicalDeviceShaderDemoteToHelperInvocationFeatures_contents(
                        *reinterpret_cast<const VkPhysicalDeviceShaderDemoteToHelperInvocationFeatures*>(next),
                        CreateScope("pNext<VkPhysicalDeviceShaderDemoteToHelperInvocationFeatures>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DEMOTE_TO_HELPER_INVOCATION_FEATURES";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES: {
                    *json_next = gen_VkPhysicalDeviceShaderDrawParametersFeatures_contents(
                        *reinterpret_cast<const VkPhysicalDeviceShaderDrawParametersFeatures*>(next),
                        CreateScope("pNext<VkPhysicalDeviceShaderDrawParametersFeatures>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_EXPECT_ASSUME_FEATURES: {
                    *json_next = gen_VkPhysicalDeviceShaderExpectAssumeFeatures_contents(
                        *reinterpret_cast<const VkPhysicalDeviceShaderExpectAssumeFeatures*>(next),
                        CreateScope("pNext<VkPhysicalDeviceShaderExpectAssumeFeatures>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_EXPECT_ASSUME_FEATURES";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT16_INT8_FEATURES: {
                    *json_next = gen_VkPhysicalDeviceShaderFloat16Int8Features_contents(
                        *reinterpret_cast<const VkPhysicalDeviceShaderFloat16Int8Features*>(next),
                        CreateScope("pNext<VkPhysicalDeviceShaderFloat16Int8Features>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT16_INT8_FEATURES";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT_CONTROLS_2_FEATURES: {
                    *json_next = gen_VkPhysicalDeviceShaderFloatControls2Features_contents(
                        *reinterpret_cast<const VkPhysicalDeviceShaderFloatControls2Features*>(next),
                        CreateScope("pNext<VkPhysicalDeviceShaderFloatControls2Features>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT_CONTROLS_2_FEATURES";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_IMAGE_ATOMIC_INT64_FEATURES_EXT: {
                    *json_next = gen_VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT_contents(
                        *reinterpret_cast<const VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT*>(next),
                        CreateScope("pNext<VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_IMAGE_ATOMIC_INT64_FEATURES_EXT";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_INTEGER_DOT_PRODUCT_FEATURES: {
                    *json_next = gen_VkPhysicalDeviceShaderIntegerDotProductFeatures_contents(
                        *reinterpret_cast<const VkPhysicalDeviceShaderIntegerDotProductFeatures*>(next),
                        CreateScope("pNext<VkPhysicalDeviceShaderIntegerDotProductFeatures>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_INTEGER_DOT_PRODUCT_FEATURES";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_EXTENDED_TYPES_FEATURES: {
                    *json_next = gen_VkPhysicalDeviceShaderSubgroupExtendedTypesFeatures_contents(
                        *reinterpret_cast<const VkPhysicalDeviceShaderSubgroupExtendedTypesFeatures*>(next),
                        CreateScope("pNext<VkPhysicalDeviceShaderSubgroupExtendedTypesFeatures>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_EXTENDED_TYPES_FEATURES";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_ROTATE_FEATURES: {
                    *json_next = gen_VkPhysicalDeviceShaderSubgroupRotateFeatures_contents(
                        *reinterpret_cast<const VkPhysicalDeviceShaderSubgroupRotateFeatures*>(next),
                        CreateScope("pNext<VkPhysicalDeviceShaderSubgroupRotateFeatures>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_ROTATE_FEATURES";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_TERMINATE_INVOCATION_FEATURES: {
                    *json_next = gen_VkPhysicalDeviceShaderTerminateInvocationFeatures_contents(
                        *reinterpret_cast<const VkPhysicalDeviceShaderTerminateInvocationFeatures*>(next),
                        CreateScope("pNext<VkPhysicalDeviceShaderTerminateInvocationFeatures>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_TERMINATE_INVOCATION_FEATURES";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_FEATURES: {
                    *json_next = gen_VkPhysicalDeviceSubgroupSizeControlFeatures_contents(
                        *reinterpret_cast<const VkPhysicalDeviceSubgroupSizeControlFeatures*>(next),
                        CreateScope("pNext<VkPhysicalDeviceSubgroupSizeControlFeatures>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_FEATURES";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES: {
                    *json_next = gen_VkPhysicalDeviceSynchronization2Features_contents(
                        *reinterpret_cast<const VkPhysicalDeviceSynchronization2Features*>(next),
                        CreateScope("pNext<VkPhysicalDeviceSynchronization2Features>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXEL_BUFFER_ALIGNMENT_FEATURES_EXT: {
                    *json_next = gen_VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT_contents(
                        *reinterpret_cast<const VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT*>(next),
                        CreateScope("pNext<VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXEL_BUFFER_ALIGNMENT_FEATURES_EXT";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXTURE_COMPRESSION_ASTC_HDR_FEATURES: {
                    *json_next = gen_VkPhysicalDeviceTextureCompressionASTCHDRFeatures_contents(
                        *reinterpret_cast<const VkPhysicalDeviceTextureCompressionASTCHDRFeatures*>(next),
                        CreateScope("pNext<VkPhysicalDeviceTextureCompressionASTCHDRFeatures>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXTURE_COMPRESSION_ASTC_HDR_FEATURES";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES: {
                    *json_next = gen_VkPhysicalDeviceTimelineSemaphoreFeatures_contents(
                        *reinterpret_cast<const VkPhysicalDeviceTimelineSemaphoreFeatures*>(next),
                        CreateScope("pNext<VkPhysicalDeviceTimelineSemaphoreFeatures>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFORM_BUFFER_STANDARD_LAYOUT_FEATURES: {
                    *json_next = gen_VkPhysicalDeviceUniformBufferStandardLayoutFeatures_contents(
                        *reinterpret_cast<const VkPhysicalDeviceUniformBufferStandardLayoutFeatures*>(next),
                        CreateScope("pNext<VkPhysicalDeviceUniformBufferStandardLayoutFeatures>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFORM_BUFFER_STANDARD_LAYOUT_FEATURES";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTERS_FEATURES: {
                    *json_next = gen_VkPhysicalDeviceVariablePointersFeatures_contents(
                        *reinterpret_cast<const VkPhysicalDeviceVariablePointersFeatures*>(next),
                        CreateScope("pNext<VkPhysicalDeviceVariablePointersFeatures>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTERS_FEATURES";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_FEATURES: {
                    *json_next = gen_VkPhysicalDeviceVertexAttributeDivisorFeatures_contents(
                        *reinterpret_cast<const VkPhysicalDeviceVertexAttributeDivisorFeatures*>(next),
                        CreateScope("pNext<VkPhysicalDeviceVertexAttributeDivisorFeatures>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_FEATURES";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_INPUT_DYNAMIC_STATE_FEATURES_EXT: {
                    *json_next = gen_VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT_contents(
                        *reinterpret_cast<const VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT*>(next),
                        CreateScope("pNext<VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_INPUT_DYNAMIC_STATE_FEATURES_EXT";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES: {
                    *json_next = gen_VkPhysicalDeviceVulkan11Features_contents(
                        *reinterpret_cast<const VkPhysicalDeviceVulkan11Features*>(next),
                        CreateScope("pNext<VkPhysicalDeviceVulkan11Features>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES: {
                    *json_next = gen_VkPhysicalDeviceVulkan12Features_contents(
                        *reinterpret_cast<const VkPhysicalDeviceVulkan12Features*>(next),
                        CreateScope("pNext<VkPhysicalDeviceVulkan12Features>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES: {
                    *json_next = gen_VkPhysicalDeviceVulkan13Features_contents(
                        *reinterpret_cast<const VkPhysicalDeviceVulkan13Features*>(next),
                        CreateScope("pNext<VkPhysicalDeviceVulkan13Features>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES: {
                    *json_next = gen_VkPhysicalDeviceVulkan14Features_contents(
                        *reinterpret_cast<const VkPhysicalDeviceVulkan14Features*>(next),
                        CreateScope("pNext<VkPhysicalDeviceVulkan14Features>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_MEMORY_MODEL_FEATURES: {
                    *json_next = gen_VkPhysicalDeviceVulkanMemoryModelFeatures_contents(
                        *reinterpret_cast<const VkPhysicalDeviceVulkanMemoryModelFeatures*>(next),
                        CreateScope("pNext<VkPhysicalDeviceVulkanMemoryModelFeatures>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_MEMORY_MODEL_FEATURES";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_SC_1_0_FEATURES: {
                    *json_next = gen_VkPhysicalDeviceVulkanSC10Features_contents(
                        *reinterpret_cast<const VkPhysicalDeviceVulkanSC10Features*>(next),
                        CreateScope("pNext<VkPhysicalDeviceVulkanSC10Features>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_SC_1_0_FEATURES";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_YCBCR_2_PLANE_444_FORMATS_FEATURES_EXT: {
                    *json_next = gen_VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT_contents(
                        *reinterpret_cast<const VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT*>(next),
                        CreateScope("pNext<VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_YCBCR_2_PLANE_444_FORMATS_FEATURES_EXT";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_YCBCR_IMAGE_ARRAYS_FEATURES_EXT: {
                    *json_next = gen_VkPhysicalDeviceYcbcrImageArraysFeaturesEXT_contents(
                        *reinterpret_cast<const VkPhysicalDeviceYcbcrImageArraysFeaturesEXT*>(next),
                        CreateScope("pNext<VkPhysicalDeviceYcbcrImageArraysFeaturesEXT>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_YCBCR_IMAGE_ARRAYS_FEATURES_EXT";
                    break;
                }

                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ZERO_INITIALIZE_WORKGROUP_MEMORY_FEATURES: {
                    *json_next = gen_VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeatures_contents(
                        *reinterpret_cast<const VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeatures*>(next),
                        CreateScope("pNext<VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeatures>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ZERO_INITIALIZE_WORKGROUP_MEMORY_FEATURES";
                    break;
                }

                default:
                    Error() << "Invalid structure type extending VkPhysicalDeviceFeatures2: " << next->sType;
                    break;
            }
            next = next->pNext;
            if (!json_next->isNull()) {
                json_next = &(*json_next)["pNext"];
            }
        }

        *json_next = "NULL";

        return json;
    };

    Json::Value gen_VkRenderPassCreateInfo(const VkRenderPassCreateInfo& s, const LocationScope& l) {
        Json::Value json = gen_VkRenderPassCreateInfo_contents(s, l);

        json["sType"] = "VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO";

        auto next = reinterpret_cast<const VkBaseInStructure*>(s.pNext);
        Json::Value* json_next = &json["pNext"];

        while (next != nullptr) {
            switch (next->sType) {
                case VK_STRUCTURE_TYPE_RENDER_PASS_INPUT_ATTACHMENT_ASPECT_CREATE_INFO: {
                    *json_next = gen_VkRenderPassInputAttachmentAspectCreateInfo_contents(
                        *reinterpret_cast<const VkRenderPassInputAttachmentAspectCreateInfo*>(next),
                        CreateScope("pNext<VkRenderPassInputAttachmentAspectCreateInfo>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_RENDER_PASS_INPUT_ATTACHMENT_ASPECT_CREATE_INFO";
                    break;
                }

                case VK_STRUCTURE_TYPE_RENDER_PASS_MULTIVIEW_CREATE_INFO: {
                    *json_next = gen_VkRenderPassMultiviewCreateInfo_contents(
                        *reinterpret_cast<const VkRenderPassMultiviewCreateInfo*>(next),
                        CreateScope("pNext<VkRenderPassMultiviewCreateInfo>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_RENDER_PASS_MULTIVIEW_CREATE_INFO";
                    break;
                }

                default:
                    Error() << "Invalid structure type extending VkRenderPassCreateInfo: " << next->sType;
                    break;
            }
            next = next->pNext;
            if (!json_next->isNull()) {
                json_next = &(*json_next)["pNext"];
            }
        }

        *json_next = "NULL";

        return json;
    };

    Json::Value gen_VkAttachmentDescription2(const VkAttachmentDescription2& s, const LocationScope& l) {
        Json::Value json = gen_VkAttachmentDescription2_contents(s, l);

        json["sType"] = "VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2";

        auto next = reinterpret_cast<const VkBaseInStructure*>(s.pNext);
        Json::Value* json_next = &json["pNext"];

        while (next != nullptr) {
            switch (next->sType) {
                case VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_STENCIL_LAYOUT: {
                    *json_next = gen_VkAttachmentDescriptionStencilLayout_contents(
                        *reinterpret_cast<const VkAttachmentDescriptionStencilLayout*>(next),
                        CreateScope("pNext<VkAttachmentDescriptionStencilLayout>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_STENCIL_LAYOUT";
                    break;
                }

                default:
                    Error() << "Invalid structure type extending VkAttachmentDescription2: " << next->sType;
                    break;
            }
            next = next->pNext;
            if (!json_next->isNull()) {
                json_next = &(*json_next)["pNext"];
            }
        }

        *json_next = "NULL";

        return json;
    };

    Json::Value gen_VkAttachmentReference2(const VkAttachmentReference2& s, const LocationScope& l) {
        Json::Value json = gen_VkAttachmentReference2_contents(s, l);

        json["sType"] = "VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2";

        auto next = reinterpret_cast<const VkBaseInStructure*>(s.pNext);
        Json::Value* json_next = &json["pNext"];

        while (next != nullptr) {
            switch (next->sType) {
                case VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_STENCIL_LAYOUT: {
                    *json_next = gen_VkAttachmentReferenceStencilLayout_contents(
                        *reinterpret_cast<const VkAttachmentReferenceStencilLayout*>(next),
                        CreateScope("pNext<VkAttachmentReferenceStencilLayout>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_STENCIL_LAYOUT";
                    break;
                }

                default:
                    Error() << "Invalid structure type extending VkAttachmentReference2: " << next->sType;
                    break;
            }
            next = next->pNext;
            if (!json_next->isNull()) {
                json_next = &(*json_next)["pNext"];
            }
        }

        *json_next = "NULL";

        return json;
    };

    Json::Value gen_VkSubpassDescription2(const VkSubpassDescription2& s, const LocationScope& l) {
        Json::Value json = gen_VkSubpassDescription2_contents(s, l);

        json["sType"] = "VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2";

        auto next = reinterpret_cast<const VkBaseInStructure*>(s.pNext);
        Json::Value* json_next = &json["pNext"];

        while (next != nullptr) {
            switch (next->sType) {
                case VK_STRUCTURE_TYPE_FRAGMENT_SHADING_RATE_ATTACHMENT_INFO_KHR: {
                    *json_next = gen_VkFragmentShadingRateAttachmentInfoKHR_contents(
                        *reinterpret_cast<const VkFragmentShadingRateAttachmentInfoKHR*>(next),
                        CreateScope("pNext<VkFragmentShadingRateAttachmentInfoKHR>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_FRAGMENT_SHADING_RATE_ATTACHMENT_INFO_KHR";
                    break;
                }

                case VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_DEPTH_STENCIL_RESOLVE: {
                    *json_next = gen_VkSubpassDescriptionDepthStencilResolve_contents(
                        *reinterpret_cast<const VkSubpassDescriptionDepthStencilResolve*>(next),
                        CreateScope("pNext<VkSubpassDescriptionDepthStencilResolve>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_DEPTH_STENCIL_RESOLVE";
                    break;
                }

                default:
                    Error() << "Invalid structure type extending VkSubpassDescription2: " << next->sType;
                    break;
            }
            next = next->pNext;
            if (!json_next->isNull()) {
                json_next = &(*json_next)["pNext"];
            }
        }

        *json_next = "NULL";

        return json;
    };

    Json::Value gen_VkSubpassDependency2(const VkSubpassDependency2& s, const LocationScope& l) {
        Json::Value json = gen_VkSubpassDependency2_contents(s, l);

        json["sType"] = "VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2";

        auto next = reinterpret_cast<const VkBaseInStructure*>(s.pNext);
        Json::Value* json_next = &json["pNext"];

        while (next != nullptr) {
            switch (next->sType) {
                case VK_STRUCTURE_TYPE_MEMORY_BARRIER_2: {
                    *json_next = gen_VkMemoryBarrier2_contents(*reinterpret_cast<const VkMemoryBarrier2*>(next),
                                                               CreateScope("pNext<VkMemoryBarrier2>", true));
                    (*json_next)["sType"] = "VK_STRUCTURE_TYPE_MEMORY_BARRIER_2";
                    break;
                }

                default:
                    Error() << "Invalid structure type extending VkSubpassDependency2: " << next->sType;
                    break;
            }
            next = next->pNext;
            if (!json_next->isNull()) {
                json_next = &(*json_next)["pNext"];
            }
        }

        *json_next = "NULL";

        return json;
    };

    Json::Value gen_VkRenderPassCreateInfo2(const VkRenderPassCreateInfo2& s, const LocationScope& l) {
        Json::Value json = gen_VkRenderPassCreateInfo2_contents(s, l);

        json["sType"] = "VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2";

        auto next = reinterpret_cast<const VkBaseInStructure*>(s.pNext);
        Json::Value* json_next = &json["pNext"];

        while (next != nullptr) {
            switch (next->sType) {
                default:
                    Error() << "Invalid structure type extending VkRenderPassCreateInfo2: " << next->sType;
                    break;
            }
            next = next->pNext;
            if (!json_next->isNull()) {
                json_next = &(*json_next)["pNext"];
            }
        }

        *json_next = "NULL";

        return json;
    };

    Json::Value gen_VkPipelineCacheCreateInfo(const VkPipelineCacheCreateInfo& s, const LocationScope& l) {
        Json::Value json = gen_VkPipelineCacheCreateInfo_contents(s, l);

        json["sType"] = "VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO";

        auto next = reinterpret_cast<const VkBaseInStructure*>(s.pNext);
        Json::Value* json_next = &json["pNext"];

        while (next != nullptr) {
            switch (next->sType) {
                default:
                    Error() << "Invalid structure type extending VkPipelineCacheCreateInfo: " << next->sType;
                    break;
            }
            next = next->pNext;
            if (!json_next->isNull()) {
                json_next = &(*json_next)["pNext"];
            }
        }

        *json_next = "NULL";

        return json;
    };

    Json::Value gen_VkPipelinePoolSize(const VkPipelinePoolSize& s, const LocationScope& l) {
        Json::Value json = gen_VkPipelinePoolSize_contents(s, l);

        json["sType"] = "VK_STRUCTURE_TYPE_PIPELINE_POOL_SIZE";

        auto next = reinterpret_cast<const VkBaseInStructure*>(s.pNext);
        Json::Value* json_next = &json["pNext"];

        while (next != nullptr) {
            switch (next->sType) {
                default:
                    Error() << "Invalid structure type extending VkPipelinePoolSize: " << next->sType;
                    break;
            }
            next = next->pNext;
            if (!json_next->isNull()) {
                json_next = &(*json_next)["pNext"];
            }
        }

        *json_next = "NULL";

        return json;
    };

    Json::Value gen_VkDeviceObjectReservationCreateInfo(const VkDeviceObjectReservationCreateInfo& s, const LocationScope& l) {
        Json::Value json = gen_VkDeviceObjectReservationCreateInfo_contents(s, l);

        json["sType"] = "VK_STRUCTURE_TYPE_DEVICE_OBJECT_RESERVATION_CREATE_INFO";

        auto next = reinterpret_cast<const VkBaseInStructure*>(s.pNext);
        Json::Value* json_next = &json["pNext"];

        while (next != nullptr) {
            switch (next->sType) {
                default:
                    Error() << "Invalid structure type extending VkDeviceObjectReservationCreateInfo: " << next->sType;
                    break;
            }
            next = next->pNext;
            if (!json_next->isNull()) {
                json_next = &(*json_next)["pNext"];
            }
        }

        *json_next = "NULL";

        return json;
    };

    Json::Value gen_VkPipelineOfflineCreateInfo(const VkPipelineOfflineCreateInfo& s, const LocationScope& l) {
        Json::Value json = gen_VkPipelineOfflineCreateInfo_contents(s, l);

        json["sType"] = "VK_STRUCTURE_TYPE_PIPELINE_OFFLINE_CREATE_INFO";

        auto next = reinterpret_cast<const VkBaseInStructure*>(s.pNext);
        Json::Value* json_next = &json["pNext"];

        while (next != nullptr) {
            switch (next->sType) {
                default:
                    Error() << "Invalid structure type extending VkPipelineOfflineCreateInfo: " << next->sType;
                    break;
            }
            next = next->pNext;
            if (!json_next->isNull()) {
                json_next = &(*json_next)["pNext"];
            }
        }

        *json_next = "NULL";

        return json;
    };

  private:
    Json::Value gen_VkSpecializationMapEntry_contents(const VkSpecializationMapEntry& s, const LocationScope& l) {
        Json::Value json;

        json["constantID"] = gen_uint32_t(s.constantID, CreateScope("constantID"));
        json["offset"] = gen_uint32_t(s.offset, CreateScope("offset"));
        json["size"] = gen_size_t(s.size, CreateScope("size"));

        return json;
    }

    Json::Value gen_VkSpecializationInfo_contents(const VkSpecializationInfo& s, const LocationScope& l) {
        Json::Value json;

        json["mapEntryCount"] = gen_uint32_t(s.mapEntryCount, CreateScope("mapEntryCount"));

        if (s.mapEntryCount != 0) {
            Json::Value json_array_pMapEntries;
            if (s.pMapEntries != nullptr) {
                for (Json::Value::ArrayIndex i = 0; i < s.mapEntryCount; i++) {
                    json_array_pMapEntries[i] =
                        gen_VkSpecializationMapEntry_contents(s.pMapEntries[i], CreateScope("pMapEntries", i));
                }
                json["pMapEntries"] = json_array_pMapEntries;
            } else {
                Error() << "pMapEntries is NULL but its length is " << s.mapEntryCount;
            }
        } else {
            json["pMapEntries"] = "NULL";
        }
        json["dataSize"] = gen_size_t(s.dataSize, CreateScope("dataSize"));

        if (s.dataSize != 0) {
            if (s.pData != nullptr) {
                json["pData"] = gen_binary(s.pData, s.dataSize);
            } else {
                Error() << "pData is NULL but its length is " << s.dataSize;
            }
        } else {
            json["pData"] = "NULL";
        }

        return json;
    }

    Json::Value gen_VkPipelineShaderStageCreateInfo_contents(const VkPipelineShaderStageCreateInfo& s, const LocationScope& l) {
        Json::Value json;

        json["flags"] = gen_VkPipelineShaderStageCreateFlags(s.flags, CreateScope("flags"));
        json["stage"] = gen_VkShaderStageFlagBits(s.stage, CreateScope("stage"));
        json["module"] = gen_VkShaderModule(s.module, CreateScope("module"));
        json["pName"] = gen_string(s.pName);
        json["pSpecializationInfo"] = s.pSpecializationInfo ? gen_VkSpecializationInfo_contents(
                                                                  *s.pSpecializationInfo, CreateScope("pSpecializationInfo", true))
                                                            : "NULL";
        return json;
    }

    Json::Value gen_VkDebugUtilsObjectNameInfoEXT_contents(const VkDebugUtilsObjectNameInfoEXT& s, const LocationScope& l) {
        Json::Value json;

        json["objectType"] = gen_VkObjectType(s.objectType, CreateScope("objectType"));
        json["objectHandle"] = gen_uint64_t(s.objectHandle, CreateScope("objectHandle"));
        json["pObjectName"] = gen_string(s.pObjectName);
        return json;
    }

    Json::Value gen_VkPipelineRobustnessCreateInfo_contents(const VkPipelineRobustnessCreateInfo& s, const LocationScope& l) {
        Json::Value json;

        json["storageBuffers"] = gen_VkPipelineRobustnessBufferBehavior(s.storageBuffers, CreateScope("storageBuffers"));
        json["uniformBuffers"] = gen_VkPipelineRobustnessBufferBehavior(s.uniformBuffers, CreateScope("uniformBuffers"));
        json["vertexInputs"] = gen_VkPipelineRobustnessBufferBehavior(s.vertexInputs, CreateScope("vertexInputs"));
        json["images"] = gen_VkPipelineRobustnessImageBehavior(s.images, CreateScope("images"));

        return json;
    }

    Json::Value gen_VkPipelineShaderStageRequiredSubgroupSizeCreateInfo_contents(
        const VkPipelineShaderStageRequiredSubgroupSizeCreateInfo& s, const LocationScope& l) {
        Json::Value json;

        json["requiredSubgroupSize"] = gen_uint32_t(s.requiredSubgroupSize, CreateScope("requiredSubgroupSize"));

        return json;
    }

    Json::Value gen_VkVertexInputBindingDescription_contents(const VkVertexInputBindingDescription& s, const LocationScope& l) {
        Json::Value json;

        json["binding"] = gen_uint32_t(s.binding, CreateScope("binding"));
        json["stride"] = gen_uint32_t(s.stride, CreateScope("stride"));
        json["inputRate"] = gen_VkVertexInputRate(s.inputRate, CreateScope("inputRate"));

        return json;
    }

    Json::Value gen_VkVertexInputAttributeDescription_contents(const VkVertexInputAttributeDescription& s, const LocationScope& l) {
        Json::Value json;

        json["location"] = gen_uint32_t(s.location, CreateScope("location"));
        json["binding"] = gen_uint32_t(s.binding, CreateScope("binding"));
        json["format"] = gen_VkFormat(s.format, CreateScope("format"));
        json["offset"] = gen_uint32_t(s.offset, CreateScope("offset"));

        return json;
    }

    Json::Value gen_VkPipelineVertexInputStateCreateInfo_contents(const VkPipelineVertexInputStateCreateInfo& s,
                                                                  const LocationScope& l) {
        Json::Value json;

        json["flags"] = gen_VkPipelineVertexInputStateCreateFlags(s.flags, CreateScope("flags"));
        json["vertexBindingDescriptionCount"] =
            gen_uint32_t(s.vertexBindingDescriptionCount, CreateScope("vertexBindingDescriptionCount"));

        if (s.vertexBindingDescriptionCount != 0) {
            Json::Value json_array_pVertexBindingDescriptions;
            if (s.pVertexBindingDescriptions != nullptr) {
                for (Json::Value::ArrayIndex i = 0; i < s.vertexBindingDescriptionCount; i++) {
                    json_array_pVertexBindingDescriptions[i] = gen_VkVertexInputBindingDescription_contents(
                        s.pVertexBindingDescriptions[i], CreateScope("pVertexBindingDescriptions", i));
                }
                json["pVertexBindingDescriptions"] = json_array_pVertexBindingDescriptions;
            } else {
                Error() << "pVertexBindingDescriptions is NULL but its length is " << s.vertexBindingDescriptionCount;
            }
        } else {
            json["pVertexBindingDescriptions"] = "NULL";
        }
        json["vertexAttributeDescriptionCount"] =
            gen_uint32_t(s.vertexAttributeDescriptionCount, CreateScope("vertexAttributeDescriptionCount"));

        if (s.vertexAttributeDescriptionCount != 0) {
            Json::Value json_array_pVertexAttributeDescriptions;
            if (s.pVertexAttributeDescriptions != nullptr) {
                for (Json::Value::ArrayIndex i = 0; i < s.vertexAttributeDescriptionCount; i++) {
                    json_array_pVertexAttributeDescriptions[i] = gen_VkVertexInputAttributeDescription_contents(
                        s.pVertexAttributeDescriptions[i], CreateScope("pVertexAttributeDescriptions", i));
                }
                json["pVertexAttributeDescriptions"] = json_array_pVertexAttributeDescriptions;
            } else {
                Error() << "pVertexAttributeDescriptions is NULL but its length is " << s.vertexAttributeDescriptionCount;
            }
        } else {
            json["pVertexAttributeDescriptions"] = "NULL";
        }

        return json;
    }

    Json::Value gen_VkVertexInputBindingDivisorDescription_contents(const VkVertexInputBindingDivisorDescription& s,
                                                                    const LocationScope& l) {
        Json::Value json;

        json["binding"] = gen_uint32_t(s.binding, CreateScope("binding"));
        json["divisor"] = gen_uint32_t(s.divisor, CreateScope("divisor"));

        return json;
    }

    Json::Value gen_VkPipelineVertexInputDivisorStateCreateInfo_contents(const VkPipelineVertexInputDivisorStateCreateInfo& s,
                                                                         const LocationScope& l) {
        Json::Value json;

        json["vertexBindingDivisorCount"] = gen_uint32_t(s.vertexBindingDivisorCount, CreateScope("vertexBindingDivisorCount"));

        if (s.vertexBindingDivisorCount != 0) {
            Json::Value json_array_pVertexBindingDivisors;
            if (s.pVertexBindingDivisors != nullptr) {
                for (Json::Value::ArrayIndex i = 0; i < s.vertexBindingDivisorCount; i++) {
                    json_array_pVertexBindingDivisors[i] = gen_VkVertexInputBindingDivisorDescription_contents(
                        s.pVertexBindingDivisors[i], CreateScope("pVertexBindingDivisors", i));
                }
                json["pVertexBindingDivisors"] = json_array_pVertexBindingDivisors;
            } else {
                Error() << "pVertexBindingDivisors is NULL but its length is " << s.vertexBindingDivisorCount;
            }
        } else {
            json["pVertexBindingDivisors"] = "NULL";
        }

        return json;
    }

    Json::Value gen_VkPipelineInputAssemblyStateCreateInfo_contents(const VkPipelineInputAssemblyStateCreateInfo& s,
                                                                    const LocationScope& l) {
        Json::Value json;

        json["flags"] = gen_VkPipelineInputAssemblyStateCreateFlags(s.flags, CreateScope("flags"));
        json["topology"] = gen_VkPrimitiveTopology(s.topology, CreateScope("topology"));
        json["primitiveRestartEnable"] = gen_VkBool32(s.primitiveRestartEnable, CreateScope("primitiveRestartEnable"));

        return json;
    }

    Json::Value gen_VkPipelineTessellationStateCreateInfo_contents(const VkPipelineTessellationStateCreateInfo& s,
                                                                   const LocationScope& l) {
        Json::Value json;

        json["flags"] = gen_VkPipelineTessellationStateCreateFlags(s.flags, CreateScope("flags"));
        json["patchControlPoints"] = gen_uint32_t(s.patchControlPoints, CreateScope("patchControlPoints"));

        return json;
    }

    Json::Value gen_VkPipelineTessellationDomainOriginStateCreateInfo_contents(
        const VkPipelineTessellationDomainOriginStateCreateInfo& s, const LocationScope& l) {
        Json::Value json;

        json["domainOrigin"] = gen_VkTessellationDomainOrigin(s.domainOrigin, CreateScope("domainOrigin"));

        return json;
    }

    Json::Value gen_VkViewport_contents(const VkViewport& s, const LocationScope& l) {
        Json::Value json;

        json["x"] = gen_float(s.x, CreateScope("x"));
        json["y"] = gen_float(s.y, CreateScope("y"));
        json["width"] = gen_float(s.width, CreateScope("width"));
        json["height"] = gen_float(s.height, CreateScope("height"));
        json["minDepth"] = gen_float(s.minDepth, CreateScope("minDepth"));
        json["maxDepth"] = gen_float(s.maxDepth, CreateScope("maxDepth"));

        return json;
    }

    Json::Value gen_VkOffset2D_contents(const VkOffset2D& s, const LocationScope& l) {
        Json::Value json;

        json["x"] = gen_int32_t(s.x, CreateScope("x"));
        json["y"] = gen_int32_t(s.y, CreateScope("y"));

        return json;
    }

    Json::Value gen_VkExtent2D_contents(const VkExtent2D& s, const LocationScope& l) {
        Json::Value json;

        json["width"] = gen_uint32_t(s.width, CreateScope("width"));
        json["height"] = gen_uint32_t(s.height, CreateScope("height"));

        return json;
    }

    Json::Value gen_VkRect2D_contents(const VkRect2D& s, const LocationScope& l) {
        Json::Value json;

        json["offset"] = gen_VkOffset2D_contents(s.offset, CreateScope("offset"));
        json["extent"] = gen_VkExtent2D_contents(s.extent, CreateScope("extent"));

        return json;
    }

    Json::Value gen_VkPipelineViewportStateCreateInfo_contents(const VkPipelineViewportStateCreateInfo& s, const LocationScope& l) {
        Json::Value json;

        json["flags"] = gen_VkPipelineViewportStateCreateFlags(s.flags, CreateScope("flags"));
        json["viewportCount"] = gen_uint32_t(s.viewportCount, CreateScope("viewportCount"));

        if (s.viewportCount != 0) {
            Json::Value json_array_pViewports;
            if (s.pViewports != nullptr) {
                for (Json::Value::ArrayIndex i = 0; i < s.viewportCount; i++) {
                    json_array_pViewports[i] = gen_VkViewport_contents(s.pViewports[i], CreateScope("pViewports", i));
                }
                json["pViewports"] = json_array_pViewports;
            } else {
                json["pViewports"] = "NULL";
            }
        } else {
            json["pViewports"] = "NULL";
        }
        json["scissorCount"] = gen_uint32_t(s.scissorCount, CreateScope("scissorCount"));

        if (s.scissorCount != 0) {
            Json::Value json_array_pScissors;
            if (s.pScissors != nullptr) {
                for (Json::Value::ArrayIndex i = 0; i < s.scissorCount; i++) {
                    json_array_pScissors[i] = gen_VkRect2D_contents(s.pScissors[i], CreateScope("pScissors", i));
                }
                json["pScissors"] = json_array_pScissors;
            } else {
                json["pScissors"] = "NULL";
            }
        } else {
            json["pScissors"] = "NULL";
        }

        return json;
    }

    Json::Value gen_VkPipelineRasterizationStateCreateInfo_contents(const VkPipelineRasterizationStateCreateInfo& s,
                                                                    const LocationScope& l) {
        Json::Value json;

        json["flags"] = gen_VkPipelineRasterizationStateCreateFlags(s.flags, CreateScope("flags"));
        json["depthClampEnable"] = gen_VkBool32(s.depthClampEnable, CreateScope("depthClampEnable"));
        json["rasterizerDiscardEnable"] = gen_VkBool32(s.rasterizerDiscardEnable, CreateScope("rasterizerDiscardEnable"));
        json["polygonMode"] = gen_VkPolygonMode(s.polygonMode, CreateScope("polygonMode"));
        json["cullMode"] = gen_VkCullModeFlags(s.cullMode, CreateScope("cullMode"));
        json["frontFace"] = gen_VkFrontFace(s.frontFace, CreateScope("frontFace"));
        json["depthBiasEnable"] = gen_VkBool32(s.depthBiasEnable, CreateScope("depthBiasEnable"));
        json["depthBiasConstantFactor"] = gen_float(s.depthBiasConstantFactor, CreateScope("depthBiasConstantFactor"));
        json["depthBiasClamp"] = gen_float(s.depthBiasClamp, CreateScope("depthBiasClamp"));
        json["depthBiasSlopeFactor"] = gen_float(s.depthBiasSlopeFactor, CreateScope("depthBiasSlopeFactor"));
        json["lineWidth"] = gen_float(s.lineWidth, CreateScope("lineWidth"));

        return json;
    }

    Json::Value gen_VkPipelineRasterizationConservativeStateCreateInfoEXT_contents(
        const VkPipelineRasterizationConservativeStateCreateInfoEXT& s, const LocationScope& l) {
        Json::Value json;

        json["flags"] = gen_VkPipelineRasterizationConservativeStateCreateFlagsEXT(s.flags, CreateScope("flags"));
        json["conservativeRasterizationMode"] =
            gen_VkConservativeRasterizationModeEXT(s.conservativeRasterizationMode, CreateScope("conservativeRasterizationMode"));
        json["extraPrimitiveOverestimationSize"] =
            gen_float(s.extraPrimitiveOverestimationSize, CreateScope("extraPrimitiveOverestimationSize"));

        return json;
    }

    Json::Value gen_VkPipelineRasterizationDepthClipStateCreateInfoEXT_contents(
        const VkPipelineRasterizationDepthClipStateCreateInfoEXT& s, const LocationScope& l) {
        Json::Value json;

        json["flags"] = gen_VkPipelineRasterizationDepthClipStateCreateFlagsEXT(s.flags, CreateScope("flags"));
        json["depthClipEnable"] = gen_VkBool32(s.depthClipEnable, CreateScope("depthClipEnable"));

        return json;
    }

    Json::Value gen_VkPipelineRasterizationLineStateCreateInfo_contents(const VkPipelineRasterizationLineStateCreateInfo& s,
                                                                        const LocationScope& l) {
        Json::Value json;

        json["lineRasterizationMode"] = gen_VkLineRasterizationMode(s.lineRasterizationMode, CreateScope("lineRasterizationMode"));
        json["stippledLineEnable"] = gen_VkBool32(s.stippledLineEnable, CreateScope("stippledLineEnable"));
        json["lineStippleFactor"] = gen_uint32_t(s.lineStippleFactor, CreateScope("lineStippleFactor"));
        json["lineStipplePattern"] = gen_uint16_t(s.lineStipplePattern, CreateScope("lineStipplePattern"));

        return json;
    }

    Json::Value gen_VkPipelineMultisampleStateCreateInfo_contents(const VkPipelineMultisampleStateCreateInfo& s,
                                                                  const LocationScope& l) {
        Json::Value json;

        json["flags"] = gen_VkPipelineMultisampleStateCreateFlags(s.flags, CreateScope("flags"));
        json["rasterizationSamples"] = gen_VkSampleCountFlagBits(s.rasterizationSamples, CreateScope("rasterizationSamples"));
        json["sampleShadingEnable"] = gen_VkBool32(s.sampleShadingEnable, CreateScope("sampleShadingEnable"));
        json["minSampleShading"] = gen_float(s.minSampleShading, CreateScope("minSampleShading"));

        if (size_t((s.rasterizationSamples + 31) / 32) != 0) {
            Json::Value json_array_pSampleMask;
            if (s.pSampleMask != nullptr) {
                for (Json::Value::ArrayIndex i = 0; i < size_t((s.rasterizationSamples + 31) / 32); i++) {
                    json_array_pSampleMask[i] = gen_VkSampleMask(s.pSampleMask[i], CreateScope("pSampleMask", i));
                }
                json["pSampleMask"] = json_array_pSampleMask;
            } else {
                json["pSampleMask"] = "NULL";
            }
        } else {
            json["pSampleMask"] = "NULL";
        }
        json["alphaToCoverageEnable"] = gen_VkBool32(s.alphaToCoverageEnable, CreateScope("alphaToCoverageEnable"));
        json["alphaToOneEnable"] = gen_VkBool32(s.alphaToOneEnable, CreateScope("alphaToOneEnable"));

        return json;
    }

    Json::Value gen_VkSampleLocationEXT_contents(const VkSampleLocationEXT& s, const LocationScope& l) {
        Json::Value json;

        json["x"] = gen_float(s.x, CreateScope("x"));
        json["y"] = gen_float(s.y, CreateScope("y"));

        return json;
    }

    Json::Value gen_VkSampleLocationsInfoEXT_contents(const VkSampleLocationsInfoEXT& s, const LocationScope& l) {
        Json::Value json;

        json["sampleLocationsPerPixel"] =
            gen_VkSampleCountFlagBits(s.sampleLocationsPerPixel, CreateScope("sampleLocationsPerPixel"));
        json["sampleLocationGridSize"] = gen_VkExtent2D_contents(s.sampleLocationGridSize, CreateScope("sampleLocationGridSize"));
        json["sampleLocationsCount"] = gen_uint32_t(s.sampleLocationsCount, CreateScope("sampleLocationsCount"));

        if (s.sampleLocationsCount != 0) {
            Json::Value json_array_pSampleLocations;
            if (s.pSampleLocations != nullptr) {
                for (Json::Value::ArrayIndex i = 0; i < s.sampleLocationsCount; i++) {
                    json_array_pSampleLocations[i] =
                        gen_VkSampleLocationEXT_contents(s.pSampleLocations[i], CreateScope("pSampleLocations", i));
                }
                json["pSampleLocations"] = json_array_pSampleLocations;
            } else {
                Error() << "pSampleLocations is NULL but its length is " << s.sampleLocationsCount;
            }
        } else {
            json["pSampleLocations"] = "NULL";
        }

        return json;
    }

    Json::Value gen_VkPipelineSampleLocationsStateCreateInfoEXT_contents(const VkPipelineSampleLocationsStateCreateInfoEXT& s,
                                                                         const LocationScope& l) {
        Json::Value json;

        json["sampleLocationsEnable"] = gen_VkBool32(s.sampleLocationsEnable, CreateScope("sampleLocationsEnable"));
        json["sampleLocationsInfo"] = gen_VkSampleLocationsInfoEXT(s.sampleLocationsInfo, CreateScope("sampleLocationsInfo"));

        return json;
    }

    Json::Value gen_VkStencilOpState_contents(const VkStencilOpState& s, const LocationScope& l) {
        Json::Value json;

        json["failOp"] = gen_VkStencilOp(s.failOp, CreateScope("failOp"));
        json["passOp"] = gen_VkStencilOp(s.passOp, CreateScope("passOp"));
        json["depthFailOp"] = gen_VkStencilOp(s.depthFailOp, CreateScope("depthFailOp"));
        json["compareOp"] = gen_VkCompareOp(s.compareOp, CreateScope("compareOp"));
        json["compareMask"] = gen_uint32_t(s.compareMask, CreateScope("compareMask"));
        json["writeMask"] = gen_uint32_t(s.writeMask, CreateScope("writeMask"));
        json["reference"] = gen_uint32_t(s.reference, CreateScope("reference"));

        return json;
    }

    Json::Value gen_VkPipelineDepthStencilStateCreateInfo_contents(const VkPipelineDepthStencilStateCreateInfo& s,
                                                                   const LocationScope& l) {
        Json::Value json;

        json["flags"] = gen_VkPipelineDepthStencilStateCreateFlags(s.flags, CreateScope("flags"));
        json["depthTestEnable"] = gen_VkBool32(s.depthTestEnable, CreateScope("depthTestEnable"));
        json["depthWriteEnable"] = gen_VkBool32(s.depthWriteEnable, CreateScope("depthWriteEnable"));
        json["depthCompareOp"] = gen_VkCompareOp(s.depthCompareOp, CreateScope("depthCompareOp"));
        json["depthBoundsTestEnable"] = gen_VkBool32(s.depthBoundsTestEnable, CreateScope("depthBoundsTestEnable"));
        json["stencilTestEnable"] = gen_VkBool32(s.stencilTestEnable, CreateScope("stencilTestEnable"));
        json["front"] = gen_VkStencilOpState_contents(s.front, CreateScope("front"));
        json["back"] = gen_VkStencilOpState_contents(s.back, CreateScope("back"));
        json["minDepthBounds"] = gen_float(s.minDepthBounds, CreateScope("minDepthBounds"));
        json["maxDepthBounds"] = gen_float(s.maxDepthBounds, CreateScope("maxDepthBounds"));

        return json;
    }

    Json::Value gen_VkPipelineColorBlendAttachmentState_contents(const VkPipelineColorBlendAttachmentState& s,
                                                                 const LocationScope& l) {
        Json::Value json;

        json["blendEnable"] = gen_VkBool32(s.blendEnable, CreateScope("blendEnable"));
        json["srcColorBlendFactor"] = gen_VkBlendFactor(s.srcColorBlendFactor, CreateScope("srcColorBlendFactor"));
        json["dstColorBlendFactor"] = gen_VkBlendFactor(s.dstColorBlendFactor, CreateScope("dstColorBlendFactor"));
        json["colorBlendOp"] = gen_VkBlendOp(s.colorBlendOp, CreateScope("colorBlendOp"));
        json["srcAlphaBlendFactor"] = gen_VkBlendFactor(s.srcAlphaBlendFactor, CreateScope("srcAlphaBlendFactor"));
        json["dstAlphaBlendFactor"] = gen_VkBlendFactor(s.dstAlphaBlendFactor, CreateScope("dstAlphaBlendFactor"));
        json["alphaBlendOp"] = gen_VkBlendOp(s.alphaBlendOp, CreateScope("alphaBlendOp"));
        json["colorWriteMask"] = gen_VkColorComponentFlags(s.colorWriteMask, CreateScope("colorWriteMask"));

        return json;
    }

    Json::Value gen_VkPipelineColorBlendStateCreateInfo_contents(const VkPipelineColorBlendStateCreateInfo& s,
                                                                 const LocationScope& l) {
        Json::Value json;

        json["flags"] = gen_VkPipelineColorBlendStateCreateFlags(s.flags, CreateScope("flags"));
        json["logicOpEnable"] = gen_VkBool32(s.logicOpEnable, CreateScope("logicOpEnable"));
        json["logicOp"] = gen_VkLogicOp(s.logicOp, CreateScope("logicOp"));
        json["attachmentCount"] = gen_uint32_t(s.attachmentCount, CreateScope("attachmentCount"));

        if (s.attachmentCount != 0) {
            Json::Value json_array_pAttachments;
            if (s.pAttachments != nullptr) {
                for (Json::Value::ArrayIndex i = 0; i < s.attachmentCount; i++) {
                    json_array_pAttachments[i] =
                        gen_VkPipelineColorBlendAttachmentState_contents(s.pAttachments[i], CreateScope("pAttachments", i));
                }
                json["pAttachments"] = json_array_pAttachments;
            } else {
                json["pAttachments"] = "NULL";
            }
        } else {
            json["pAttachments"] = "NULL";
        }

        if (4 != 0) {
            Json::Value json_array_blendConstants;
            {
                for (Json::Value::ArrayIndex i = 0; i < 4; i++) {
                    json_array_blendConstants[i] = gen_float(s.blendConstants[i], CreateScope("blendConstants", i));
                }
                json["blendConstants"] = json_array_blendConstants;
            }
        } else {
            json["blendConstants"] = "NULL";
        }

        return json;
    }

    Json::Value gen_VkPipelineColorBlendAdvancedStateCreateInfoEXT_contents(const VkPipelineColorBlendAdvancedStateCreateInfoEXT& s,
                                                                            const LocationScope& l) {
        Json::Value json;

        json["srcPremultiplied"] = gen_VkBool32(s.srcPremultiplied, CreateScope("srcPremultiplied"));
        json["dstPremultiplied"] = gen_VkBool32(s.dstPremultiplied, CreateScope("dstPremultiplied"));
        json["blendOverlap"] = gen_VkBlendOverlapEXT(s.blendOverlap, CreateScope("blendOverlap"));

        return json;
    }

    Json::Value gen_VkPipelineColorWriteCreateInfoEXT_contents(const VkPipelineColorWriteCreateInfoEXT& s, const LocationScope& l) {
        Json::Value json;

        json["attachmentCount"] = gen_uint32_t(s.attachmentCount, CreateScope("attachmentCount"));

        if (s.attachmentCount != 0) {
            Json::Value json_array_pColorWriteEnables;
            if (s.pColorWriteEnables != nullptr) {
                for (Json::Value::ArrayIndex i = 0; i < s.attachmentCount; i++) {
                    json_array_pColorWriteEnables[i] = gen_VkBool32(s.pColorWriteEnables[i], CreateScope("pColorWriteEnables", i));
                }
                json["pColorWriteEnables"] = json_array_pColorWriteEnables;
            } else {
                Error() << "pColorWriteEnables is NULL but its length is " << s.attachmentCount;
            }
        } else {
            json["pColorWriteEnables"] = "NULL";
        }

        return json;
    }

    Json::Value gen_VkPipelineDynamicStateCreateInfo_contents(const VkPipelineDynamicStateCreateInfo& s, const LocationScope& l) {
        Json::Value json;

        json["flags"] = gen_VkPipelineDynamicStateCreateFlags(s.flags, CreateScope("flags"));
        json["dynamicStateCount"] = gen_uint32_t(s.dynamicStateCount, CreateScope("dynamicStateCount"));

        if (s.dynamicStateCount != 0) {
            Json::Value json_array_pDynamicStates;
            if (s.pDynamicStates != nullptr) {
                for (Json::Value::ArrayIndex i = 0; i < s.dynamicStateCount; i++) {
                    json_array_pDynamicStates[i] = gen_VkDynamicState(s.pDynamicStates[i], CreateScope("pDynamicStates", i));
                }
                json["pDynamicStates"] = json_array_pDynamicStates;
            } else {
                Error() << "pDynamicStates is NULL but its length is " << s.dynamicStateCount;
            }
        } else {
            json["pDynamicStates"] = "NULL";
        }

        return json;
    }

    Json::Value gen_VkGraphicsPipelineCreateInfo_contents(const VkGraphicsPipelineCreateInfo& s, const LocationScope& l) {
        Json::Value json;

        json["flags"] = gen_VkPipelineCreateFlags(s.flags, CreateScope("flags"));
        json["stageCount"] = gen_uint32_t(s.stageCount, CreateScope("stageCount"));

        if (s.stageCount != 0) {
            Json::Value json_array_pStages;
            if (s.pStages != nullptr) {
                for (Json::Value::ArrayIndex i = 0; i < s.stageCount; i++) {
                    json_array_pStages[i] = gen_VkPipelineShaderStageCreateInfo(s.pStages[i], CreateScope("pStages", i));
                }
                json["pStages"] = json_array_pStages;
            } else {
                json["pStages"] = "NULL";
            }
        } else {
            json["pStages"] = "NULL";
        }
        json["pVertexInputState"] = s.pVertexInputState ? gen_VkPipelineVertexInputStateCreateInfo(
                                                              *s.pVertexInputState, CreateScope("pVertexInputState", true))
                                                        : "NULL";
        json["pInputAssemblyState"] = s.pInputAssemblyState ? gen_VkPipelineInputAssemblyStateCreateInfo(
                                                                  *s.pInputAssemblyState, CreateScope("pInputAssemblyState", true))
                                                            : "NULL";
        json["pTessellationState"] = s.pTessellationState ? gen_VkPipelineTessellationStateCreateInfo(
                                                                *s.pTessellationState, CreateScope("pTessellationState", true))
                                                          : "NULL";
        json["pViewportState"] = s.pViewportState
                                     ? gen_VkPipelineViewportStateCreateInfo(*s.pViewportState, CreateScope("pViewportState", true))
                                     : "NULL";
        json["pRasterizationState"] = s.pRasterizationState ? gen_VkPipelineRasterizationStateCreateInfo(
                                                                  *s.pRasterizationState, CreateScope("pRasterizationState", true))
                                                            : "NULL";
        json["pMultisampleState"] = s.pMultisampleState ? gen_VkPipelineMultisampleStateCreateInfo(
                                                              *s.pMultisampleState, CreateScope("pMultisampleState", true))
                                                        : "NULL";
        json["pDepthStencilState"] = s.pDepthStencilState ? gen_VkPipelineDepthStencilStateCreateInfo(
                                                                *s.pDepthStencilState, CreateScope("pDepthStencilState", true))
                                                          : "NULL";
        json["pColorBlendState"] =
            s.pColorBlendState ? gen_VkPipelineColorBlendStateCreateInfo(*s.pColorBlendState, CreateScope("pColorBlendState", true))
                               : "NULL";
        json["pDynamicState"] =
            s.pDynamicState ? gen_VkPipelineDynamicStateCreateInfo(*s.pDynamicState, CreateScope("pDynamicState", true)) : "NULL";
        json["layout"] = gen_VkPipelineLayout(s.layout, CreateScope("layout"));
        json["renderPass"] = gen_VkRenderPass(s.renderPass, CreateScope("renderPass"));
        json["subpass"] = gen_uint32_t(s.subpass, CreateScope("subpass"));
        json["basePipelineHandle"] = gen_VkPipeline(s.basePipelineHandle, CreateScope("basePipelineHandle"));
        json["basePipelineIndex"] = gen_int32_t(s.basePipelineIndex, CreateScope("basePipelineIndex"));

        return json;
    }

    Json::Value gen_VkPipelineCreateFlags2CreateInfo_contents(const VkPipelineCreateFlags2CreateInfo& s, const LocationScope& l) {
        Json::Value json;

        json["flags"] = gen_VkPipelineCreateFlags2(s.flags, CreateScope("flags"));

        return json;
    }

    Json::Value gen_VkPipelineCreationFeedback_contents(const VkPipelineCreationFeedback& s, const LocationScope& l) {
        Json::Value json;

        json["flags"] = gen_VkPipelineCreationFeedbackFlags(s.flags, CreateScope("flags"));
        json["duration"] = gen_uint64_t(s.duration, CreateScope("duration"));

        return json;
    }

    Json::Value gen_VkPipelineCreationFeedbackCreateInfo_contents(const VkPipelineCreationFeedbackCreateInfo& s,
                                                                  const LocationScope& l) {
        Json::Value json;

        json["pPipelineCreationFeedback"] = s.pPipelineCreationFeedback
                                                ? gen_VkPipelineCreationFeedback_contents(
                                                      *s.pPipelineCreationFeedback, CreateScope("pPipelineCreationFeedback", true))
                                                : "NULL";
        json["pipelineStageCreationFeedbackCount"] =
            gen_uint32_t(s.pipelineStageCreationFeedbackCount, CreateScope("pipelineStageCreationFeedbackCount"));

        if (s.pipelineStageCreationFeedbackCount != 0) {
            Json::Value json_array_pPipelineStageCreationFeedbacks;
            if (s.pPipelineStageCreationFeedbacks != nullptr) {
                for (Json::Value::ArrayIndex i = 0; i < s.pipelineStageCreationFeedbackCount; i++) {
                    json_array_pPipelineStageCreationFeedbacks[i] = gen_VkPipelineCreationFeedback_contents(
                        s.pPipelineStageCreationFeedbacks[i], CreateScope("pPipelineStageCreationFeedbacks", i));
                }
                json["pPipelineStageCreationFeedbacks"] = json_array_pPipelineStageCreationFeedbacks;
            } else {
                Error() << "pPipelineStageCreationFeedbacks is NULL but its length is " << s.pipelineStageCreationFeedbackCount;
            }
        } else {
            json["pPipelineStageCreationFeedbacks"] = "NULL";
        }

        return json;
    }

    Json::Value gen_VkPipelineDiscardRectangleStateCreateInfoEXT_contents(const VkPipelineDiscardRectangleStateCreateInfoEXT& s,
                                                                          const LocationScope& l) {
        Json::Value json;

        json["flags"] = gen_VkPipelineDiscardRectangleStateCreateFlagsEXT(s.flags, CreateScope("flags"));
        json["discardRectangleMode"] = gen_VkDiscardRectangleModeEXT(s.discardRectangleMode, CreateScope("discardRectangleMode"));
        json["discardRectangleCount"] = gen_uint32_t(s.discardRectangleCount, CreateScope("discardRectangleCount"));

        if (s.discardRectangleCount != 0) {
            Json::Value json_array_pDiscardRectangles;
            if (s.pDiscardRectangles != nullptr) {
                for (Json::Value::ArrayIndex i = 0; i < s.discardRectangleCount; i++) {
                    json_array_pDiscardRectangles[i] =
                        gen_VkRect2D_contents(s.pDiscardRectangles[i], CreateScope("pDiscardRectangles", i));
                }
                json["pDiscardRectangles"] = json_array_pDiscardRectangles;
            } else {
                json["pDiscardRectangles"] = "NULL";
            }
        } else {
            json["pDiscardRectangles"] = "NULL";
        }

        return json;
    }

    Json::Value gen_VkPipelineFragmentShadingRateStateCreateInfoKHR_contents(
        const VkPipelineFragmentShadingRateStateCreateInfoKHR& s, const LocationScope& l) {
        Json::Value json;

        json["fragmentSize"] = gen_VkExtent2D_contents(s.fragmentSize, CreateScope("fragmentSize"));

        if (2 != 0) {
            Json::Value json_array_combinerOps;
            {
                for (Json::Value::ArrayIndex i = 0; i < 2; i++) {
                    json_array_combinerOps[i] =
                        gen_VkFragmentShadingRateCombinerOpKHR(s.combinerOps[i], CreateScope("combinerOps", i));
                }
                json["combinerOps"] = json_array_combinerOps;
            }
        } else {
            json["combinerOps"] = "NULL";
        }

        return json;
    }

    Json::Value gen_VkPipelineOfflineCreateInfo_contents(const VkPipelineOfflineCreateInfo& s, const LocationScope& l) {
        Json::Value json;

        if (VK_UUID_SIZE != 0) {
            Json::Value json_array_pipelineIdentifier;
            {
                for (Json::Value::ArrayIndex i = 0; i < VK_UUID_SIZE; i++) {
                    json_array_pipelineIdentifier[i] = gen_uint8_t(s.pipelineIdentifier[i], CreateScope("pipelineIdentifier", i));
                }
                json["pipelineIdentifier"] = json_array_pipelineIdentifier;
            }
        } else {
            json["pipelineIdentifier"] = "NULL";
        }
        json["matchControl"] = gen_VkPipelineMatchControl(s.matchControl, CreateScope("matchControl"));
        json["poolEntrySize"] = gen_VkDeviceSize(s.poolEntrySize, CreateScope("poolEntrySize"));

        return json;
    }

    Json::Value gen_VkPipelineRenderingCreateInfo_contents(const VkPipelineRenderingCreateInfo& s, const LocationScope& l) {
        Json::Value json;

        json["viewMask"] = gen_uint32_t(s.viewMask, CreateScope("viewMask"));
        json["colorAttachmentCount"] = gen_uint32_t(s.colorAttachmentCount, CreateScope("colorAttachmentCount"));

        if (s.colorAttachmentCount != 0) {
            Json::Value json_array_pColorAttachmentFormats;
            if (s.pColorAttachmentFormats != nullptr) {
                for (Json::Value::ArrayIndex i = 0; i < s.colorAttachmentCount; i++) {
                    json_array_pColorAttachmentFormats[i] =
                        gen_VkFormat(s.pColorAttachmentFormats[i], CreateScope("pColorAttachmentFormats", i));
                }
                json["pColorAttachmentFormats"] = json_array_pColorAttachmentFormats;
            } else {
                json["pColorAttachmentFormats"] = "NULL";
            }
        } else {
            json["pColorAttachmentFormats"] = "NULL";
        }
        json["depthAttachmentFormat"] = gen_VkFormat(s.depthAttachmentFormat, CreateScope("depthAttachmentFormat"));
        json["stencilAttachmentFormat"] = gen_VkFormat(s.stencilAttachmentFormat, CreateScope("stencilAttachmentFormat"));

        return json;
    }

    Json::Value gen_VkRenderingAttachmentLocationInfo_contents(const VkRenderingAttachmentLocationInfo& s, const LocationScope& l) {
        Json::Value json;

        json["colorAttachmentCount"] = gen_uint32_t(s.colorAttachmentCount, CreateScope("colorAttachmentCount"));

        if (s.colorAttachmentCount != 0) {
            Json::Value json_array_pColorAttachmentLocations;
            if (s.pColorAttachmentLocations != nullptr) {
                for (Json::Value::ArrayIndex i = 0; i < s.colorAttachmentCount; i++) {
                    json_array_pColorAttachmentLocations[i] =
                        gen_uint32_t(s.pColorAttachmentLocations[i], CreateScope("pColorAttachmentLocations", i));
                }
                json["pColorAttachmentLocations"] = json_array_pColorAttachmentLocations;
            } else {
                json["pColorAttachmentLocations"] = "NULL";
            }
        } else {
            json["pColorAttachmentLocations"] = "NULL";
        }

        return json;
    }

    Json::Value gen_VkRenderingInputAttachmentIndexInfo_contents(const VkRenderingInputAttachmentIndexInfo& s,
                                                                 const LocationScope& l) {
        Json::Value json;

        json["colorAttachmentCount"] = gen_uint32_t(s.colorAttachmentCount, CreateScope("colorAttachmentCount"));

        if (s.colorAttachmentCount != 0) {
            Json::Value json_array_pColorAttachmentInputIndices;
            if (s.pColorAttachmentInputIndices != nullptr) {
                for (Json::Value::ArrayIndex i = 0; i < s.colorAttachmentCount; i++) {
                    json_array_pColorAttachmentInputIndices[i] =
                        gen_uint32_t(s.pColorAttachmentInputIndices[i], CreateScope("pColorAttachmentInputIndices", i));
                }
                json["pColorAttachmentInputIndices"] = json_array_pColorAttachmentInputIndices;
            } else {
                json["pColorAttachmentInputIndices"] = "NULL";
            }
        } else {
            json["pColorAttachmentInputIndices"] = "NULL";
        }
        json["pDepthInputAttachmentIndex"] =
            s.pDepthInputAttachmentIndex
                ? gen_uint32_t(*s.pDepthInputAttachmentIndex, CreateScope("pDepthInputAttachmentIndex", true))
                : "NULL";
        json["pStencilInputAttachmentIndex"] =
            s.pStencilInputAttachmentIndex
                ? gen_uint32_t(*s.pStencilInputAttachmentIndex, CreateScope("pStencilInputAttachmentIndex", true))
                : "NULL";
        return json;
    }

    Json::Value gen_VkComputePipelineCreateInfo_contents(const VkComputePipelineCreateInfo& s, const LocationScope& l) {
        Json::Value json;

        json["flags"] = gen_VkPipelineCreateFlags(s.flags, CreateScope("flags"));
        json["stage"] = gen_VkPipelineShaderStageCreateInfo(s.stage, CreateScope("stage"));
        json["layout"] = gen_VkPipelineLayout(s.layout, CreateScope("layout"));
        json["basePipelineHandle"] = gen_VkPipeline(s.basePipelineHandle, CreateScope("basePipelineHandle"));
        json["basePipelineIndex"] = gen_int32_t(s.basePipelineIndex, CreateScope("basePipelineIndex"));

        return json;
    }

    Json::Value gen_VkComponentMapping_contents(const VkComponentMapping& s, const LocationScope& l) {
        Json::Value json;

        json["r"] = gen_VkComponentSwizzle(s.r, CreateScope("r"));
        json["g"] = gen_VkComponentSwizzle(s.g, CreateScope("g"));
        json["b"] = gen_VkComponentSwizzle(s.b, CreateScope("b"));
        json["a"] = gen_VkComponentSwizzle(s.a, CreateScope("a"));

        return json;
    }

    Json::Value gen_VkSamplerYcbcrConversionCreateInfo_contents(const VkSamplerYcbcrConversionCreateInfo& s,
                                                                const LocationScope& l) {
        Json::Value json;

        json["format"] = gen_VkFormat(s.format, CreateScope("format"));
        json["ycbcrModel"] = gen_VkSamplerYcbcrModelConversion(s.ycbcrModel, CreateScope("ycbcrModel"));
        json["ycbcrRange"] = gen_VkSamplerYcbcrRange(s.ycbcrRange, CreateScope("ycbcrRange"));
        json["components"] = gen_VkComponentMapping_contents(s.components, CreateScope("components"));
        json["xChromaOffset"] = gen_VkChromaLocation(s.xChromaOffset, CreateScope("xChromaOffset"));
        json["yChromaOffset"] = gen_VkChromaLocation(s.yChromaOffset, CreateScope("yChromaOffset"));
        json["chromaFilter"] = gen_VkFilter(s.chromaFilter, CreateScope("chromaFilter"));
        json["forceExplicitReconstruction"] =
            gen_VkBool32(s.forceExplicitReconstruction, CreateScope("forceExplicitReconstruction"));

        return json;
    }
#ifdef VK_USE_PLATFORM_SCREEN_QNX

    Json::Value gen_VkExternalFormatQNX_contents(const VkExternalFormatQNX& s, const LocationScope& l) {
        Json::Value json;

        json["externalFormat"] = gen_uint64_t(s.externalFormat, CreateScope("externalFormat"));

        return json;
    }
#endif  // VK_USE_PLATFORM_SCREEN_QNX

    Json::Value gen_VkSamplerCreateInfo_contents(const VkSamplerCreateInfo& s, const LocationScope& l) {
        Json::Value json;

        json["flags"] = gen_VkSamplerCreateFlags(s.flags, CreateScope("flags"));
        json["magFilter"] = gen_VkFilter(s.magFilter, CreateScope("magFilter"));
        json["minFilter"] = gen_VkFilter(s.minFilter, CreateScope("minFilter"));
        json["mipmapMode"] = gen_VkSamplerMipmapMode(s.mipmapMode, CreateScope("mipmapMode"));
        json["addressModeU"] = gen_VkSamplerAddressMode(s.addressModeU, CreateScope("addressModeU"));
        json["addressModeV"] = gen_VkSamplerAddressMode(s.addressModeV, CreateScope("addressModeV"));
        json["addressModeW"] = gen_VkSamplerAddressMode(s.addressModeW, CreateScope("addressModeW"));
        json["mipLodBias"] = gen_float(s.mipLodBias, CreateScope("mipLodBias"));
        json["anisotropyEnable"] = gen_VkBool32(s.anisotropyEnable, CreateScope("anisotropyEnable"));
        json["maxAnisotropy"] = gen_float(s.maxAnisotropy, CreateScope("maxAnisotropy"));
        json["compareEnable"] = gen_VkBool32(s.compareEnable, CreateScope("compareEnable"));
        json["compareOp"] = gen_VkCompareOp(s.compareOp, CreateScope("compareOp"));
        json["minLod"] = gen_float(s.minLod, CreateScope("minLod"));
        json["maxLod"] = gen_float(s.maxLod, CreateScope("maxLod"));
        json["borderColor"] = gen_VkBorderColor(s.borderColor, CreateScope("borderColor"));
        json["unnormalizedCoordinates"] = gen_VkBool32(s.unnormalizedCoordinates, CreateScope("unnormalizedCoordinates"));

        return json;
    }

    Json::Value gen_VkClearColorValue_contents(const VkClearColorValue& s, const LocationScope& l) {
        Json::Value json;

        if (4 != 0) {
            Json::Value json_array_float32;
            {
                for (Json::Value::ArrayIndex i = 0; i < 4; i++) {
                    json_array_float32[i] = gen_float(s.float32[i], CreateScope("float32", i));
                }
                json["float32"] = json_array_float32;
            }
        } else {
            json["float32"] = "NULL";
        }

        if (4 != 0) {
            Json::Value json_array_int32;
            {
                for (Json::Value::ArrayIndex i = 0; i < 4; i++) {
                    json_array_int32[i] = gen_int32_t(s.int32[i], CreateScope("int32", i));
                }
                json["int32"] = json_array_int32;
            }
        } else {
            json["int32"] = "NULL";
        }

        if (4 != 0) {
            Json::Value json_array_uint32;
            {
                for (Json::Value::ArrayIndex i = 0; i < 4; i++) {
                    json_array_uint32[i] = gen_uint32_t(s.uint32[i], CreateScope("uint32", i));
                }
                json["uint32"] = json_array_uint32;
            }
        } else {
            json["uint32"] = "NULL";
        }

        return json;
    }

    Json::Value gen_VkSamplerCustomBorderColorCreateInfoEXT_contents(const VkSamplerCustomBorderColorCreateInfoEXT& s,
                                                                     const LocationScope& l) {
        Json::Value json;

        json["customBorderColor"] = gen_VkClearColorValue_contents(s.customBorderColor, CreateScope("customBorderColor"));
        json["format"] = gen_VkFormat(s.format, CreateScope("format"));

        return json;
    }

    Json::Value gen_VkSamplerReductionModeCreateInfo_contents(const VkSamplerReductionModeCreateInfo& s, const LocationScope& l) {
        Json::Value json;

        json["reductionMode"] = gen_VkSamplerReductionMode(s.reductionMode, CreateScope("reductionMode"));

        return json;
    }

    Json::Value gen_VkSamplerYcbcrConversionInfo_contents(const VkSamplerYcbcrConversionInfo& s, const LocationScope& l) {
        Json::Value json;

        json["conversion"] = gen_VkSamplerYcbcrConversion(s.conversion, CreateScope("conversion"));

        return json;
    }

    Json::Value gen_VkDescriptorSetLayoutBinding_contents(const VkDescriptorSetLayoutBinding& s, const LocationScope& l) {
        Json::Value json;

        json["binding"] = gen_uint32_t(s.binding, CreateScope("binding"));
        json["descriptorType"] = gen_VkDescriptorType(s.descriptorType, CreateScope("descriptorType"));
        json["descriptorCount"] = gen_uint32_t(s.descriptorCount, CreateScope("descriptorCount"));
        json["stageFlags"] = gen_VkShaderStageFlags(s.stageFlags, CreateScope("stageFlags"));

        if (s.descriptorCount != 0) {
            Json::Value json_array_pImmutableSamplers;
            if (s.pImmutableSamplers != nullptr) {
                for (Json::Value::ArrayIndex i = 0; i < s.descriptorCount; i++) {
                    json_array_pImmutableSamplers[i] = gen_VkSampler(s.pImmutableSamplers[i], CreateScope("pImmutableSamplers", i));
                }
                json["pImmutableSamplers"] = json_array_pImmutableSamplers;
            } else {
                json["pImmutableSamplers"] = "NULL";
            }
        } else {
            json["pImmutableSamplers"] = "NULL";
        }

        return json;
    }

    Json::Value gen_VkDescriptorSetLayoutCreateInfo_contents(const VkDescriptorSetLayoutCreateInfo& s, const LocationScope& l) {
        Json::Value json;

        json["flags"] = gen_VkDescriptorSetLayoutCreateFlags(s.flags, CreateScope("flags"));
        json["bindingCount"] = gen_uint32_t(s.bindingCount, CreateScope("bindingCount"));

        if (s.bindingCount != 0) {
            Json::Value json_array_pBindings;
            if (s.pBindings != nullptr) {
                for (Json::Value::ArrayIndex i = 0; i < s.bindingCount; i++) {
                    json_array_pBindings[i] =
                        gen_VkDescriptorSetLayoutBinding_contents(s.pBindings[i], CreateScope("pBindings", i));
                }
                json["pBindings"] = json_array_pBindings;
            } else {
                Error() << "pBindings is NULL but its length is " << s.bindingCount;
            }
        } else {
            json["pBindings"] = "NULL";
        }

        return json;
    }

    Json::Value gen_VkDescriptorSetLayoutBindingFlagsCreateInfo_contents(const VkDescriptorSetLayoutBindingFlagsCreateInfo& s,
                                                                         const LocationScope& l) {
        Json::Value json;

        json["bindingCount"] = gen_uint32_t(s.bindingCount, CreateScope("bindingCount"));

        if (s.bindingCount != 0) {
            Json::Value json_array_pBindingFlags;
            if (s.pBindingFlags != nullptr) {
                for (Json::Value::ArrayIndex i = 0; i < s.bindingCount; i++) {
                    json_array_pBindingFlags[i] = gen_VkDescriptorBindingFlags(s.pBindingFlags[i], CreateScope("pBindingFlags", i));
                }
                json["pBindingFlags"] = json_array_pBindingFlags;
            } else {
                Error() << "pBindingFlags is NULL but its length is " << s.bindingCount;
            }
        } else {
            json["pBindingFlags"] = "NULL";
        }

        return json;
    }

    Json::Value gen_VkPushConstantRange_contents(const VkPushConstantRange& s, const LocationScope& l) {
        Json::Value json;

        json["stageFlags"] = gen_VkShaderStageFlags(s.stageFlags, CreateScope("stageFlags"));
        json["offset"] = gen_uint32_t(s.offset, CreateScope("offset"));
        json["size"] = gen_uint32_t(s.size, CreateScope("size"));

        return json;
    }

    Json::Value gen_VkPipelineLayoutCreateInfo_contents(const VkPipelineLayoutCreateInfo& s, const LocationScope& l) {
        Json::Value json;

        json["flags"] = gen_VkPipelineLayoutCreateFlags(s.flags, CreateScope("flags"));
        json["setLayoutCount"] = gen_uint32_t(s.setLayoutCount, CreateScope("setLayoutCount"));

        if (s.setLayoutCount != 0) {
            Json::Value json_array_pSetLayouts;
            if (s.pSetLayouts != nullptr) {
                for (Json::Value::ArrayIndex i = 0; i < s.setLayoutCount; i++) {
                    json_array_pSetLayouts[i] = gen_VkDescriptorSetLayout(s.pSetLayouts[i], CreateScope("pSetLayouts", i));
                }
                json["pSetLayouts"] = json_array_pSetLayouts;
            } else {
                Error() << "pSetLayouts is NULL but its length is " << s.setLayoutCount;
            }
        } else {
            json["pSetLayouts"] = "NULL";
        }
        json["pushConstantRangeCount"] = gen_uint32_t(s.pushConstantRangeCount, CreateScope("pushConstantRangeCount"));

        if (s.pushConstantRangeCount != 0) {
            Json::Value json_array_pPushConstantRanges;
            if (s.pPushConstantRanges != nullptr) {
                for (Json::Value::ArrayIndex i = 0; i < s.pushConstantRangeCount; i++) {
                    json_array_pPushConstantRanges[i] =
                        gen_VkPushConstantRange_contents(s.pPushConstantRanges[i], CreateScope("pPushConstantRanges", i));
                }
                json["pPushConstantRanges"] = json_array_pPushConstantRanges;
            } else {
                Error() << "pPushConstantRanges is NULL but its length is " << s.pushConstantRangeCount;
            }
        } else {
            json["pPushConstantRanges"] = "NULL";
        }

        return json;
    }

    Json::Value gen_VkPhysicalDeviceFeatures_contents(const VkPhysicalDeviceFeatures& s, const LocationScope& l) {
        Json::Value json;

        json["robustBufferAccess"] = gen_VkBool32(s.robustBufferAccess, CreateScope("robustBufferAccess"));
        json["fullDrawIndexUint32"] = gen_VkBool32(s.fullDrawIndexUint32, CreateScope("fullDrawIndexUint32"));
        json["imageCubeArray"] = gen_VkBool32(s.imageCubeArray, CreateScope("imageCubeArray"));
        json["independentBlend"] = gen_VkBool32(s.independentBlend, CreateScope("independentBlend"));
        json["geometryShader"] = gen_VkBool32(s.geometryShader, CreateScope("geometryShader"));
        json["tessellationShader"] = gen_VkBool32(s.tessellationShader, CreateScope("tessellationShader"));
        json["sampleRateShading"] = gen_VkBool32(s.sampleRateShading, CreateScope("sampleRateShading"));
        json["dualSrcBlend"] = gen_VkBool32(s.dualSrcBlend, CreateScope("dualSrcBlend"));
        json["logicOp"] = gen_VkBool32(s.logicOp, CreateScope("logicOp"));
        json["multiDrawIndirect"] = gen_VkBool32(s.multiDrawIndirect, CreateScope("multiDrawIndirect"));
        json["drawIndirectFirstInstance"] = gen_VkBool32(s.drawIndirectFirstInstance, CreateScope("drawIndirectFirstInstance"));
        json["depthClamp"] = gen_VkBool32(s.depthClamp, CreateScope("depthClamp"));
        json["depthBiasClamp"] = gen_VkBool32(s.depthBiasClamp, CreateScope("depthBiasClamp"));
        json["fillModeNonSolid"] = gen_VkBool32(s.fillModeNonSolid, CreateScope("fillModeNonSolid"));
        json["depthBounds"] = gen_VkBool32(s.depthBounds, CreateScope("depthBounds"));
        json["wideLines"] = gen_VkBool32(s.wideLines, CreateScope("wideLines"));
        json["largePoints"] = gen_VkBool32(s.largePoints, CreateScope("largePoints"));
        json["alphaToOne"] = gen_VkBool32(s.alphaToOne, CreateScope("alphaToOne"));
        json["multiViewport"] = gen_VkBool32(s.multiViewport, CreateScope("multiViewport"));
        json["samplerAnisotropy"] = gen_VkBool32(s.samplerAnisotropy, CreateScope("samplerAnisotropy"));
        json["textureCompressionETC2"] = gen_VkBool32(s.textureCompressionETC2, CreateScope("textureCompressionETC2"));
        json["textureCompressionASTC_LDR"] = gen_VkBool32(s.textureCompressionASTC_LDR, CreateScope("textureCompressionASTC_LDR"));
        json["textureCompressionBC"] = gen_VkBool32(s.textureCompressionBC, CreateScope("textureCompressionBC"));
        json["occlusionQueryPrecise"] = gen_VkBool32(s.occlusionQueryPrecise, CreateScope("occlusionQueryPrecise"));
        json["pipelineStatisticsQuery"] = gen_VkBool32(s.pipelineStatisticsQuery, CreateScope("pipelineStatisticsQuery"));
        json["vertexPipelineStoresAndAtomics"] =
            gen_VkBool32(s.vertexPipelineStoresAndAtomics, CreateScope("vertexPipelineStoresAndAtomics"));
        json["fragmentStoresAndAtomics"] = gen_VkBool32(s.fragmentStoresAndAtomics, CreateScope("fragmentStoresAndAtomics"));
        json["shaderTessellationAndGeometryPointSize"] =
            gen_VkBool32(s.shaderTessellationAndGeometryPointSize, CreateScope("shaderTessellationAndGeometryPointSize"));
        json["shaderImageGatherExtended"] = gen_VkBool32(s.shaderImageGatherExtended, CreateScope("shaderImageGatherExtended"));
        json["shaderStorageImageExtendedFormats"] =
            gen_VkBool32(s.shaderStorageImageExtendedFormats, CreateScope("shaderStorageImageExtendedFormats"));
        json["shaderStorageImageMultisample"] =
            gen_VkBool32(s.shaderStorageImageMultisample, CreateScope("shaderStorageImageMultisample"));
        json["shaderStorageImageReadWithoutFormat"] =
            gen_VkBool32(s.shaderStorageImageReadWithoutFormat, CreateScope("shaderStorageImageReadWithoutFormat"));
        json["shaderStorageImageWriteWithoutFormat"] =
            gen_VkBool32(s.shaderStorageImageWriteWithoutFormat, CreateScope("shaderStorageImageWriteWithoutFormat"));
        json["shaderUniformBufferArrayDynamicIndexing"] =
            gen_VkBool32(s.shaderUniformBufferArrayDynamicIndexing, CreateScope("shaderUniformBufferArrayDynamicIndexing"));
        json["shaderSampledImageArrayDynamicIndexing"] =
            gen_VkBool32(s.shaderSampledImageArrayDynamicIndexing, CreateScope("shaderSampledImageArrayDynamicIndexing"));
        json["shaderStorageBufferArrayDynamicIndexing"] =
            gen_VkBool32(s.shaderStorageBufferArrayDynamicIndexing, CreateScope("shaderStorageBufferArrayDynamicIndexing"));
        json["shaderStorageImageArrayDynamicIndexing"] =
            gen_VkBool32(s.shaderStorageImageArrayDynamicIndexing, CreateScope("shaderStorageImageArrayDynamicIndexing"));
        json["shaderClipDistance"] = gen_VkBool32(s.shaderClipDistance, CreateScope("shaderClipDistance"));
        json["shaderCullDistance"] = gen_VkBool32(s.shaderCullDistance, CreateScope("shaderCullDistance"));
        json["shaderFloat64"] = gen_VkBool32(s.shaderFloat64, CreateScope("shaderFloat64"));
        json["shaderInt64"] = gen_VkBool32(s.shaderInt64, CreateScope("shaderInt64"));
        json["shaderInt16"] = gen_VkBool32(s.shaderInt16, CreateScope("shaderInt16"));
        json["shaderResourceResidency"] = gen_VkBool32(s.shaderResourceResidency, CreateScope("shaderResourceResidency"));
        json["shaderResourceMinLod"] = gen_VkBool32(s.shaderResourceMinLod, CreateScope("shaderResourceMinLod"));
        json["sparseBinding"] = gen_VkBool32(s.sparseBinding, CreateScope("sparseBinding"));
        json["sparseResidencyBuffer"] = gen_VkBool32(s.sparseResidencyBuffer, CreateScope("sparseResidencyBuffer"));
        json["sparseResidencyImage2D"] = gen_VkBool32(s.sparseResidencyImage2D, CreateScope("sparseResidencyImage2D"));
        json["sparseResidencyImage3D"] = gen_VkBool32(s.sparseResidencyImage3D, CreateScope("sparseResidencyImage3D"));
        json["sparseResidency2Samples"] = gen_VkBool32(s.sparseResidency2Samples, CreateScope("sparseResidency2Samples"));
        json["sparseResidency4Samples"] = gen_VkBool32(s.sparseResidency4Samples, CreateScope("sparseResidency4Samples"));
        json["sparseResidency8Samples"] = gen_VkBool32(s.sparseResidency8Samples, CreateScope("sparseResidency8Samples"));
        json["sparseResidency16Samples"] = gen_VkBool32(s.sparseResidency16Samples, CreateScope("sparseResidency16Samples"));
        json["sparseResidencyAliased"] = gen_VkBool32(s.sparseResidencyAliased, CreateScope("sparseResidencyAliased"));
        json["variableMultisampleRate"] = gen_VkBool32(s.variableMultisampleRate, CreateScope("variableMultisampleRate"));
        json["inheritedQueries"] = gen_VkBool32(s.inheritedQueries, CreateScope("inheritedQueries"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceFeatures2_contents(const VkPhysicalDeviceFeatures2& s, const LocationScope& l) {
        Json::Value json;

        json["features"] = gen_VkPhysicalDeviceFeatures_contents(s.features, CreateScope("features"));

        return json;
    }

    Json::Value gen_VkPhysicalDevice16BitStorageFeatures_contents(const VkPhysicalDevice16BitStorageFeatures& s,
                                                                  const LocationScope& l) {
        Json::Value json;

        json["storageBuffer16BitAccess"] = gen_VkBool32(s.storageBuffer16BitAccess, CreateScope("storageBuffer16BitAccess"));
        json["uniformAndStorageBuffer16BitAccess"] =
            gen_VkBool32(s.uniformAndStorageBuffer16BitAccess, CreateScope("uniformAndStorageBuffer16BitAccess"));
        json["storagePushConstant16"] = gen_VkBool32(s.storagePushConstant16, CreateScope("storagePushConstant16"));
        json["storageInputOutput16"] = gen_VkBool32(s.storageInputOutput16, CreateScope("storageInputOutput16"));

        return json;
    }

    Json::Value gen_VkPhysicalDevice4444FormatsFeaturesEXT_contents(const VkPhysicalDevice4444FormatsFeaturesEXT& s,
                                                                    const LocationScope& l) {
        Json::Value json;

        json["formatA4R4G4B4"] = gen_VkBool32(s.formatA4R4G4B4, CreateScope("formatA4R4G4B4"));
        json["formatA4B4G4R4"] = gen_VkBool32(s.formatA4B4G4R4, CreateScope("formatA4B4G4R4"));

        return json;
    }

    Json::Value gen_VkPhysicalDevice8BitStorageFeatures_contents(const VkPhysicalDevice8BitStorageFeatures& s,
                                                                 const LocationScope& l) {
        Json::Value json;

        json["storageBuffer8BitAccess"] = gen_VkBool32(s.storageBuffer8BitAccess, CreateScope("storageBuffer8BitAccess"));
        json["uniformAndStorageBuffer8BitAccess"] =
            gen_VkBool32(s.uniformAndStorageBuffer8BitAccess, CreateScope("uniformAndStorageBuffer8BitAccess"));
        json["storagePushConstant8"] = gen_VkBool32(s.storagePushConstant8, CreateScope("storagePushConstant8"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceASTCDecodeFeaturesEXT_contents(const VkPhysicalDeviceASTCDecodeFeaturesEXT& s,
                                                                   const LocationScope& l) {
        Json::Value json;

        json["decodeModeSharedExponent"] = gen_VkBool32(s.decodeModeSharedExponent, CreateScope("decodeModeSharedExponent"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT_contents(
        const VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT& s, const LocationScope& l) {
        Json::Value json;

        json["advancedBlendCoherentOperations"] =
            gen_VkBool32(s.advancedBlendCoherentOperations, CreateScope("advancedBlendCoherentOperations"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceBufferDeviceAddressFeatures_contents(const VkPhysicalDeviceBufferDeviceAddressFeatures& s,
                                                                         const LocationScope& l) {
        Json::Value json;

        json["bufferDeviceAddress"] = gen_VkBool32(s.bufferDeviceAddress, CreateScope("bufferDeviceAddress"));
        json["bufferDeviceAddressCaptureReplay"] =
            gen_VkBool32(s.bufferDeviceAddressCaptureReplay, CreateScope("bufferDeviceAddressCaptureReplay"));
        json["bufferDeviceAddressMultiDevice"] =
            gen_VkBool32(s.bufferDeviceAddressMultiDevice, CreateScope("bufferDeviceAddressMultiDevice"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceColorWriteEnableFeaturesEXT_contents(const VkPhysicalDeviceColorWriteEnableFeaturesEXT& s,
                                                                         const LocationScope& l) {
        Json::Value json;

        json["colorWriteEnable"] = gen_VkBool32(s.colorWriteEnable, CreateScope("colorWriteEnable"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceCustomBorderColorFeaturesEXT_contents(const VkPhysicalDeviceCustomBorderColorFeaturesEXT& s,
                                                                          const LocationScope& l) {
        Json::Value json;

        json["customBorderColors"] = gen_VkBool32(s.customBorderColors, CreateScope("customBorderColors"));
        json["customBorderColorWithoutFormat"] =
            gen_VkBool32(s.customBorderColorWithoutFormat, CreateScope("customBorderColorWithoutFormat"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceDepthClipEnableFeaturesEXT_contents(const VkPhysicalDeviceDepthClipEnableFeaturesEXT& s,
                                                                        const LocationScope& l) {
        Json::Value json;

        json["depthClipEnable"] = gen_VkBool32(s.depthClipEnable, CreateScope("depthClipEnable"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceDescriptorIndexingFeatures_contents(const VkPhysicalDeviceDescriptorIndexingFeatures& s,
                                                                        const LocationScope& l) {
        Json::Value json;

        json["shaderInputAttachmentArrayDynamicIndexing"] =
            gen_VkBool32(s.shaderInputAttachmentArrayDynamicIndexing, CreateScope("shaderInputAttachmentArrayDynamicIndexing"));
        json["shaderUniformTexelBufferArrayDynamicIndexing"] = gen_VkBool32(
            s.shaderUniformTexelBufferArrayDynamicIndexing, CreateScope("shaderUniformTexelBufferArrayDynamicIndexing"));
        json["shaderStorageTexelBufferArrayDynamicIndexing"] = gen_VkBool32(
            s.shaderStorageTexelBufferArrayDynamicIndexing, CreateScope("shaderStorageTexelBufferArrayDynamicIndexing"));
        json["shaderUniformBufferArrayNonUniformIndexing"] =
            gen_VkBool32(s.shaderUniformBufferArrayNonUniformIndexing, CreateScope("shaderUniformBufferArrayNonUniformIndexing"));
        json["shaderSampledImageArrayNonUniformIndexing"] =
            gen_VkBool32(s.shaderSampledImageArrayNonUniformIndexing, CreateScope("shaderSampledImageArrayNonUniformIndexing"));
        json["shaderStorageBufferArrayNonUniformIndexing"] =
            gen_VkBool32(s.shaderStorageBufferArrayNonUniformIndexing, CreateScope("shaderStorageBufferArrayNonUniformIndexing"));
        json["shaderStorageImageArrayNonUniformIndexing"] =
            gen_VkBool32(s.shaderStorageImageArrayNonUniformIndexing, CreateScope("shaderStorageImageArrayNonUniformIndexing"));
        json["shaderInputAttachmentArrayNonUniformIndexing"] = gen_VkBool32(
            s.shaderInputAttachmentArrayNonUniformIndexing, CreateScope("shaderInputAttachmentArrayNonUniformIndexing"));
        json["shaderUniformTexelBufferArrayNonUniformIndexing"] = gen_VkBool32(
            s.shaderUniformTexelBufferArrayNonUniformIndexing, CreateScope("shaderUniformTexelBufferArrayNonUniformIndexing"));
        json["shaderStorageTexelBufferArrayNonUniformIndexing"] = gen_VkBool32(
            s.shaderStorageTexelBufferArrayNonUniformIndexing, CreateScope("shaderStorageTexelBufferArrayNonUniformIndexing"));
        json["descriptorBindingUniformBufferUpdateAfterBind"] = gen_VkBool32(
            s.descriptorBindingUniformBufferUpdateAfterBind, CreateScope("descriptorBindingUniformBufferUpdateAfterBind"));
        json["descriptorBindingSampledImageUpdateAfterBind"] = gen_VkBool32(
            s.descriptorBindingSampledImageUpdateAfterBind, CreateScope("descriptorBindingSampledImageUpdateAfterBind"));
        json["descriptorBindingStorageImageUpdateAfterBind"] = gen_VkBool32(
            s.descriptorBindingStorageImageUpdateAfterBind, CreateScope("descriptorBindingStorageImageUpdateAfterBind"));
        json["descriptorBindingStorageBufferUpdateAfterBind"] = gen_VkBool32(
            s.descriptorBindingStorageBufferUpdateAfterBind, CreateScope("descriptorBindingStorageBufferUpdateAfterBind"));
        json["descriptorBindingUniformTexelBufferUpdateAfterBind"] =
            gen_VkBool32(s.descriptorBindingUniformTexelBufferUpdateAfterBind,
                         CreateScope("descriptorBindingUniformTexelBufferUpdateAfterBind"));
        json["descriptorBindingStorageTexelBufferUpdateAfterBind"] =
            gen_VkBool32(s.descriptorBindingStorageTexelBufferUpdateAfterBind,
                         CreateScope("descriptorBindingStorageTexelBufferUpdateAfterBind"));
        json["descriptorBindingUpdateUnusedWhilePending"] =
            gen_VkBool32(s.descriptorBindingUpdateUnusedWhilePending, CreateScope("descriptorBindingUpdateUnusedWhilePending"));
        json["descriptorBindingPartiallyBound"] =
            gen_VkBool32(s.descriptorBindingPartiallyBound, CreateScope("descriptorBindingPartiallyBound"));
        json["descriptorBindingVariableDescriptorCount"] =
            gen_VkBool32(s.descriptorBindingVariableDescriptorCount, CreateScope("descriptorBindingVariableDescriptorCount"));
        json["runtimeDescriptorArray"] = gen_VkBool32(s.runtimeDescriptorArray, CreateScope("runtimeDescriptorArray"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceDynamicRenderingFeatures_contents(const VkPhysicalDeviceDynamicRenderingFeatures& s,
                                                                      const LocationScope& l) {
        Json::Value json;

        json["dynamicRendering"] = gen_VkBool32(s.dynamicRendering, CreateScope("dynamicRendering"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceDynamicRenderingLocalReadFeatures_contents(
        const VkPhysicalDeviceDynamicRenderingLocalReadFeatures& s, const LocationScope& l) {
        Json::Value json;

        json["dynamicRenderingLocalRead"] = gen_VkBool32(s.dynamicRenderingLocalRead, CreateScope("dynamicRenderingLocalRead"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceExtendedDynamicState2FeaturesEXT_contents(
        const VkPhysicalDeviceExtendedDynamicState2FeaturesEXT& s, const LocationScope& l) {
        Json::Value json;

        json["extendedDynamicState2"] = gen_VkBool32(s.extendedDynamicState2, CreateScope("extendedDynamicState2"));
        json["extendedDynamicState2LogicOp"] =
            gen_VkBool32(s.extendedDynamicState2LogicOp, CreateScope("extendedDynamicState2LogicOp"));
        json["extendedDynamicState2PatchControlPoints"] =
            gen_VkBool32(s.extendedDynamicState2PatchControlPoints, CreateScope("extendedDynamicState2PatchControlPoints"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceExtendedDynamicStateFeaturesEXT_contents(
        const VkPhysicalDeviceExtendedDynamicStateFeaturesEXT& s, const LocationScope& l) {
        Json::Value json;

        json["extendedDynamicState"] = gen_VkBool32(s.extendedDynamicState, CreateScope("extendedDynamicState"));

        return json;
    }
#ifdef VK_USE_PLATFORM_SCI

    Json::Value gen_VkPhysicalDeviceExternalMemorySciBufFeaturesNV_contents(const VkPhysicalDeviceExternalMemorySciBufFeaturesNV& s,
                                                                            const LocationScope& l) {
        Json::Value json;

        json["sciBufImport"] = gen_VkBool32(s.sciBufImport, CreateScope("sciBufImport"));
        json["sciBufExport"] = gen_VkBool32(s.sciBufExport, CreateScope("sciBufExport"));

        return json;
    }
#endif  // VK_USE_PLATFORM_SCI
#ifdef VK_USE_PLATFORM_SCREEN_QNX

    Json::Value gen_VkPhysicalDeviceExternalMemoryScreenBufferFeaturesQNX_contents(
        const VkPhysicalDeviceExternalMemoryScreenBufferFeaturesQNX& s, const LocationScope& l) {
        Json::Value json;

        json["screenBufferImport"] = gen_VkBool32(s.screenBufferImport, CreateScope("screenBufferImport"));

        return json;
    }
#endif  // VK_USE_PLATFORM_SCREEN_QNX
#ifdef VK_USE_PLATFORM_SCI

    Json::Value gen_VkPhysicalDeviceExternalSciSync2FeaturesNV_contents(const VkPhysicalDeviceExternalSciSync2FeaturesNV& s,
                                                                        const LocationScope& l) {
        Json::Value json;

        json["sciSyncFence"] = gen_VkBool32(s.sciSyncFence, CreateScope("sciSyncFence"));
        json["sciSyncSemaphore2"] = gen_VkBool32(s.sciSyncSemaphore2, CreateScope("sciSyncSemaphore2"));
        json["sciSyncImport"] = gen_VkBool32(s.sciSyncImport, CreateScope("sciSyncImport"));
        json["sciSyncExport"] = gen_VkBool32(s.sciSyncExport, CreateScope("sciSyncExport"));

        return json;
    }
#endif  // VK_USE_PLATFORM_SCI
#ifdef VK_USE_PLATFORM_SCI

    Json::Value gen_VkPhysicalDeviceExternalSciSyncFeaturesNV_contents(const VkPhysicalDeviceExternalSciSyncFeaturesNV& s,
                                                                       const LocationScope& l) {
        Json::Value json;

        json["sciSyncFence"] = gen_VkBool32(s.sciSyncFence, CreateScope("sciSyncFence"));
        json["sciSyncSemaphore"] = gen_VkBool32(s.sciSyncSemaphore, CreateScope("sciSyncSemaphore"));
        json["sciSyncImport"] = gen_VkBool32(s.sciSyncImport, CreateScope("sciSyncImport"));
        json["sciSyncExport"] = gen_VkBool32(s.sciSyncExport, CreateScope("sciSyncExport"));

        return json;
    }
#endif  // VK_USE_PLATFORM_SCI

    Json::Value gen_VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT_contents(
        const VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT& s, const LocationScope& l) {
        Json::Value json;

        json["fragmentShaderSampleInterlock"] =
            gen_VkBool32(s.fragmentShaderSampleInterlock, CreateScope("fragmentShaderSampleInterlock"));
        json["fragmentShaderPixelInterlock"] =
            gen_VkBool32(s.fragmentShaderPixelInterlock, CreateScope("fragmentShaderPixelInterlock"));
        json["fragmentShaderShadingRateInterlock"] =
            gen_VkBool32(s.fragmentShaderShadingRateInterlock, CreateScope("fragmentShaderShadingRateInterlock"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceFragmentShadingRateFeaturesKHR_contents(const VkPhysicalDeviceFragmentShadingRateFeaturesKHR& s,
                                                                            const LocationScope& l) {
        Json::Value json;

        json["pipelineFragmentShadingRate"] =
            gen_VkBool32(s.pipelineFragmentShadingRate, CreateScope("pipelineFragmentShadingRate"));
        json["primitiveFragmentShadingRate"] =
            gen_VkBool32(s.primitiveFragmentShadingRate, CreateScope("primitiveFragmentShadingRate"));
        json["attachmentFragmentShadingRate"] =
            gen_VkBool32(s.attachmentFragmentShadingRate, CreateScope("attachmentFragmentShadingRate"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceGlobalPriorityQueryFeatures_contents(const VkPhysicalDeviceGlobalPriorityQueryFeatures& s,
                                                                         const LocationScope& l) {
        Json::Value json;

        json["globalPriorityQuery"] = gen_VkBool32(s.globalPriorityQuery, CreateScope("globalPriorityQuery"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceHostImageCopyFeatures_contents(const VkPhysicalDeviceHostImageCopyFeatures& s,
                                                                   const LocationScope& l) {
        Json::Value json;

        json["hostImageCopy"] = gen_VkBool32(s.hostImageCopy, CreateScope("hostImageCopy"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceHostQueryResetFeatures_contents(const VkPhysicalDeviceHostQueryResetFeatures& s,
                                                                    const LocationScope& l) {
        Json::Value json;

        json["hostQueryReset"] = gen_VkBool32(s.hostQueryReset, CreateScope("hostQueryReset"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceImageRobustnessFeatures_contents(const VkPhysicalDeviceImageRobustnessFeatures& s,
                                                                     const LocationScope& l) {
        Json::Value json;

        json["robustImageAccess"] = gen_VkBool32(s.robustImageAccess, CreateScope("robustImageAccess"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceImagelessFramebufferFeatures_contents(const VkPhysicalDeviceImagelessFramebufferFeatures& s,
                                                                          const LocationScope& l) {
        Json::Value json;

        json["imagelessFramebuffer"] = gen_VkBool32(s.imagelessFramebuffer, CreateScope("imagelessFramebuffer"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceIndexTypeUint8Features_contents(const VkPhysicalDeviceIndexTypeUint8Features& s,
                                                                    const LocationScope& l) {
        Json::Value json;

        json["indexTypeUint8"] = gen_VkBool32(s.indexTypeUint8, CreateScope("indexTypeUint8"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceInlineUniformBlockFeatures_contents(const VkPhysicalDeviceInlineUniformBlockFeatures& s,
                                                                        const LocationScope& l) {
        Json::Value json;

        json["inlineUniformBlock"] = gen_VkBool32(s.inlineUniformBlock, CreateScope("inlineUniformBlock"));
        json["descriptorBindingInlineUniformBlockUpdateAfterBind"] =
            gen_VkBool32(s.descriptorBindingInlineUniformBlockUpdateAfterBind,
                         CreateScope("descriptorBindingInlineUniformBlockUpdateAfterBind"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceLineRasterizationFeatures_contents(const VkPhysicalDeviceLineRasterizationFeatures& s,
                                                                       const LocationScope& l) {
        Json::Value json;

        json["rectangularLines"] = gen_VkBool32(s.rectangularLines, CreateScope("rectangularLines"));
        json["bresenhamLines"] = gen_VkBool32(s.bresenhamLines, CreateScope("bresenhamLines"));
        json["smoothLines"] = gen_VkBool32(s.smoothLines, CreateScope("smoothLines"));
        json["stippledRectangularLines"] = gen_VkBool32(s.stippledRectangularLines, CreateScope("stippledRectangularLines"));
        json["stippledBresenhamLines"] = gen_VkBool32(s.stippledBresenhamLines, CreateScope("stippledBresenhamLines"));
        json["stippledSmoothLines"] = gen_VkBool32(s.stippledSmoothLines, CreateScope("stippledSmoothLines"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceMaintenance4Features_contents(const VkPhysicalDeviceMaintenance4Features& s,
                                                                  const LocationScope& l) {
        Json::Value json;

        json["maintenance4"] = gen_VkBool32(s.maintenance4, CreateScope("maintenance4"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceMaintenance5Features_contents(const VkPhysicalDeviceMaintenance5Features& s,
                                                                  const LocationScope& l) {
        Json::Value json;

        json["maintenance5"] = gen_VkBool32(s.maintenance5, CreateScope("maintenance5"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceMaintenance6Features_contents(const VkPhysicalDeviceMaintenance6Features& s,
                                                                  const LocationScope& l) {
        Json::Value json;

        json["maintenance6"] = gen_VkBool32(s.maintenance6, CreateScope("maintenance6"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceMultiviewFeatures_contents(const VkPhysicalDeviceMultiviewFeatures& s, const LocationScope& l) {
        Json::Value json;

        json["multiview"] = gen_VkBool32(s.multiview, CreateScope("multiview"));
        json["multiviewGeometryShader"] = gen_VkBool32(s.multiviewGeometryShader, CreateScope("multiviewGeometryShader"));
        json["multiviewTessellationShader"] =
            gen_VkBool32(s.multiviewTessellationShader, CreateScope("multiviewTessellationShader"));

        return json;
    }

    Json::Value gen_VkPhysicalDevicePerformanceQueryFeaturesKHR_contents(const VkPhysicalDevicePerformanceQueryFeaturesKHR& s,
                                                                         const LocationScope& l) {
        Json::Value json;

        json["performanceCounterQueryPools"] =
            gen_VkBool32(s.performanceCounterQueryPools, CreateScope("performanceCounterQueryPools"));
        json["performanceCounterMultipleQueryPools"] =
            gen_VkBool32(s.performanceCounterMultipleQueryPools, CreateScope("performanceCounterMultipleQueryPools"));

        return json;
    }

    Json::Value gen_VkPhysicalDevicePipelineCreationCacheControlFeatures_contents(
        const VkPhysicalDevicePipelineCreationCacheControlFeatures& s, const LocationScope& l) {
        Json::Value json;

        json["pipelineCreationCacheControl"] =
            gen_VkBool32(s.pipelineCreationCacheControl, CreateScope("pipelineCreationCacheControl"));

        return json;
    }

    Json::Value gen_VkPhysicalDevicePipelineProtectedAccessFeatures_contents(
        const VkPhysicalDevicePipelineProtectedAccessFeatures& s, const LocationScope& l) {
        Json::Value json;

        json["pipelineProtectedAccess"] = gen_VkBool32(s.pipelineProtectedAccess, CreateScope("pipelineProtectedAccess"));

        return json;
    }

    Json::Value gen_VkPhysicalDevicePipelineRobustnessFeatures_contents(const VkPhysicalDevicePipelineRobustnessFeatures& s,
                                                                        const LocationScope& l) {
        Json::Value json;

        json["pipelineRobustness"] = gen_VkBool32(s.pipelineRobustness, CreateScope("pipelineRobustness"));

        return json;
    }

    Json::Value gen_VkPhysicalDevicePrivateDataFeatures_contents(const VkPhysicalDevicePrivateDataFeatures& s,
                                                                 const LocationScope& l) {
        Json::Value json;

        json["privateData"] = gen_VkBool32(s.privateData, CreateScope("privateData"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceProtectedMemoryFeatures_contents(const VkPhysicalDeviceProtectedMemoryFeatures& s,
                                                                     const LocationScope& l) {
        Json::Value json;

        json["protectedMemory"] = gen_VkBool32(s.protectedMemory, CreateScope("protectedMemory"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceRobustness2FeaturesKHR_contents(const VkPhysicalDeviceRobustness2FeaturesKHR& s,
                                                                    const LocationScope& l) {
        Json::Value json;

        json["robustBufferAccess2"] = gen_VkBool32(s.robustBufferAccess2, CreateScope("robustBufferAccess2"));
        json["robustImageAccess2"] = gen_VkBool32(s.robustImageAccess2, CreateScope("robustImageAccess2"));
        json["nullDescriptor"] = gen_VkBool32(s.nullDescriptor, CreateScope("nullDescriptor"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceSamplerYcbcrConversionFeatures_contents(const VkPhysicalDeviceSamplerYcbcrConversionFeatures& s,
                                                                            const LocationScope& l) {
        Json::Value json;

        json["samplerYcbcrConversion"] = gen_VkBool32(s.samplerYcbcrConversion, CreateScope("samplerYcbcrConversion"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceScalarBlockLayoutFeatures_contents(const VkPhysicalDeviceScalarBlockLayoutFeatures& s,
                                                                       const LocationScope& l) {
        Json::Value json;

        json["scalarBlockLayout"] = gen_VkBool32(s.scalarBlockLayout, CreateScope("scalarBlockLayout"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceSeparateDepthStencilLayoutsFeatures_contents(
        const VkPhysicalDeviceSeparateDepthStencilLayoutsFeatures& s, const LocationScope& l) {
        Json::Value json;

        json["separateDepthStencilLayouts"] =
            gen_VkBool32(s.separateDepthStencilLayouts, CreateScope("separateDepthStencilLayouts"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceShaderAtomicFloatFeaturesEXT_contents(const VkPhysicalDeviceShaderAtomicFloatFeaturesEXT& s,
                                                                          const LocationScope& l) {
        Json::Value json;

        json["shaderBufferFloat32Atomics"] = gen_VkBool32(s.shaderBufferFloat32Atomics, CreateScope("shaderBufferFloat32Atomics"));
        json["shaderBufferFloat32AtomicAdd"] =
            gen_VkBool32(s.shaderBufferFloat32AtomicAdd, CreateScope("shaderBufferFloat32AtomicAdd"));
        json["shaderBufferFloat64Atomics"] = gen_VkBool32(s.shaderBufferFloat64Atomics, CreateScope("shaderBufferFloat64Atomics"));
        json["shaderBufferFloat64AtomicAdd"] =
            gen_VkBool32(s.shaderBufferFloat64AtomicAdd, CreateScope("shaderBufferFloat64AtomicAdd"));
        json["shaderSharedFloat32Atomics"] = gen_VkBool32(s.shaderSharedFloat32Atomics, CreateScope("shaderSharedFloat32Atomics"));
        json["shaderSharedFloat32AtomicAdd"] =
            gen_VkBool32(s.shaderSharedFloat32AtomicAdd, CreateScope("shaderSharedFloat32AtomicAdd"));
        json["shaderSharedFloat64Atomics"] = gen_VkBool32(s.shaderSharedFloat64Atomics, CreateScope("shaderSharedFloat64Atomics"));
        json["shaderSharedFloat64AtomicAdd"] =
            gen_VkBool32(s.shaderSharedFloat64AtomicAdd, CreateScope("shaderSharedFloat64AtomicAdd"));
        json["shaderImageFloat32Atomics"] = gen_VkBool32(s.shaderImageFloat32Atomics, CreateScope("shaderImageFloat32Atomics"));
        json["shaderImageFloat32AtomicAdd"] =
            gen_VkBool32(s.shaderImageFloat32AtomicAdd, CreateScope("shaderImageFloat32AtomicAdd"));
        json["sparseImageFloat32Atomics"] = gen_VkBool32(s.sparseImageFloat32Atomics, CreateScope("sparseImageFloat32Atomics"));
        json["sparseImageFloat32AtomicAdd"] =
            gen_VkBool32(s.sparseImageFloat32AtomicAdd, CreateScope("sparseImageFloat32AtomicAdd"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceShaderAtomicInt64Features_contents(const VkPhysicalDeviceShaderAtomicInt64Features& s,
                                                                       const LocationScope& l) {
        Json::Value json;

        json["shaderBufferInt64Atomics"] = gen_VkBool32(s.shaderBufferInt64Atomics, CreateScope("shaderBufferInt64Atomics"));
        json["shaderSharedInt64Atomics"] = gen_VkBool32(s.shaderSharedInt64Atomics, CreateScope("shaderSharedInt64Atomics"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceShaderClockFeaturesKHR_contents(const VkPhysicalDeviceShaderClockFeaturesKHR& s,
                                                                    const LocationScope& l) {
        Json::Value json;

        json["shaderSubgroupClock"] = gen_VkBool32(s.shaderSubgroupClock, CreateScope("shaderSubgroupClock"));
        json["shaderDeviceClock"] = gen_VkBool32(s.shaderDeviceClock, CreateScope("shaderDeviceClock"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceShaderDemoteToHelperInvocationFeatures_contents(
        const VkPhysicalDeviceShaderDemoteToHelperInvocationFeatures& s, const LocationScope& l) {
        Json::Value json;

        json["shaderDemoteToHelperInvocation"] =
            gen_VkBool32(s.shaderDemoteToHelperInvocation, CreateScope("shaderDemoteToHelperInvocation"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceShaderDrawParametersFeatures_contents(const VkPhysicalDeviceShaderDrawParametersFeatures& s,
                                                                          const LocationScope& l) {
        Json::Value json;

        json["shaderDrawParameters"] = gen_VkBool32(s.shaderDrawParameters, CreateScope("shaderDrawParameters"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceShaderExpectAssumeFeatures_contents(const VkPhysicalDeviceShaderExpectAssumeFeatures& s,
                                                                        const LocationScope& l) {
        Json::Value json;

        json["shaderExpectAssume"] = gen_VkBool32(s.shaderExpectAssume, CreateScope("shaderExpectAssume"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceShaderFloat16Int8Features_contents(const VkPhysicalDeviceShaderFloat16Int8Features& s,
                                                                       const LocationScope& l) {
        Json::Value json;

        json["shaderFloat16"] = gen_VkBool32(s.shaderFloat16, CreateScope("shaderFloat16"));
        json["shaderInt8"] = gen_VkBool32(s.shaderInt8, CreateScope("shaderInt8"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceShaderFloatControls2Features_contents(const VkPhysicalDeviceShaderFloatControls2Features& s,
                                                                          const LocationScope& l) {
        Json::Value json;

        json["shaderFloatControls2"] = gen_VkBool32(s.shaderFloatControls2, CreateScope("shaderFloatControls2"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT_contents(
        const VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT& s, const LocationScope& l) {
        Json::Value json;

        json["shaderImageInt64Atomics"] = gen_VkBool32(s.shaderImageInt64Atomics, CreateScope("shaderImageInt64Atomics"));
        json["sparseImageInt64Atomics"] = gen_VkBool32(s.sparseImageInt64Atomics, CreateScope("sparseImageInt64Atomics"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceShaderIntegerDotProductFeatures_contents(
        const VkPhysicalDeviceShaderIntegerDotProductFeatures& s, const LocationScope& l) {
        Json::Value json;

        json["shaderIntegerDotProduct"] = gen_VkBool32(s.shaderIntegerDotProduct, CreateScope("shaderIntegerDotProduct"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceShaderSubgroupExtendedTypesFeatures_contents(
        const VkPhysicalDeviceShaderSubgroupExtendedTypesFeatures& s, const LocationScope& l) {
        Json::Value json;

        json["shaderSubgroupExtendedTypes"] =
            gen_VkBool32(s.shaderSubgroupExtendedTypes, CreateScope("shaderSubgroupExtendedTypes"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceShaderSubgroupRotateFeatures_contents(const VkPhysicalDeviceShaderSubgroupRotateFeatures& s,
                                                                          const LocationScope& l) {
        Json::Value json;

        json["shaderSubgroupRotate"] = gen_VkBool32(s.shaderSubgroupRotate, CreateScope("shaderSubgroupRotate"));
        json["shaderSubgroupRotateClustered"] =
            gen_VkBool32(s.shaderSubgroupRotateClustered, CreateScope("shaderSubgroupRotateClustered"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceShaderTerminateInvocationFeatures_contents(
        const VkPhysicalDeviceShaderTerminateInvocationFeatures& s, const LocationScope& l) {
        Json::Value json;

        json["shaderTerminateInvocation"] = gen_VkBool32(s.shaderTerminateInvocation, CreateScope("shaderTerminateInvocation"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceSubgroupSizeControlFeatures_contents(const VkPhysicalDeviceSubgroupSizeControlFeatures& s,
                                                                         const LocationScope& l) {
        Json::Value json;

        json["subgroupSizeControl"] = gen_VkBool32(s.subgroupSizeControl, CreateScope("subgroupSizeControl"));
        json["computeFullSubgroups"] = gen_VkBool32(s.computeFullSubgroups, CreateScope("computeFullSubgroups"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceSynchronization2Features_contents(const VkPhysicalDeviceSynchronization2Features& s,
                                                                      const LocationScope& l) {
        Json::Value json;

        json["synchronization2"] = gen_VkBool32(s.synchronization2, CreateScope("synchronization2"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT_contents(
        const VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT& s, const LocationScope& l) {
        Json::Value json;

        json["texelBufferAlignment"] = gen_VkBool32(s.texelBufferAlignment, CreateScope("texelBufferAlignment"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceTextureCompressionASTCHDRFeatures_contents(
        const VkPhysicalDeviceTextureCompressionASTCHDRFeatures& s, const LocationScope& l) {
        Json::Value json;

        json["textureCompressionASTC_HDR"] = gen_VkBool32(s.textureCompressionASTC_HDR, CreateScope("textureCompressionASTC_HDR"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceTimelineSemaphoreFeatures_contents(const VkPhysicalDeviceTimelineSemaphoreFeatures& s,
                                                                       const LocationScope& l) {
        Json::Value json;

        json["timelineSemaphore"] = gen_VkBool32(s.timelineSemaphore, CreateScope("timelineSemaphore"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceUniformBufferStandardLayoutFeatures_contents(
        const VkPhysicalDeviceUniformBufferStandardLayoutFeatures& s, const LocationScope& l) {
        Json::Value json;

        json["uniformBufferStandardLayout"] =
            gen_VkBool32(s.uniformBufferStandardLayout, CreateScope("uniformBufferStandardLayout"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceVariablePointersFeatures_contents(const VkPhysicalDeviceVariablePointersFeatures& s,
                                                                      const LocationScope& l) {
        Json::Value json;

        json["variablePointersStorageBuffer"] =
            gen_VkBool32(s.variablePointersStorageBuffer, CreateScope("variablePointersStorageBuffer"));
        json["variablePointers"] = gen_VkBool32(s.variablePointers, CreateScope("variablePointers"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceVertexAttributeDivisorFeatures_contents(const VkPhysicalDeviceVertexAttributeDivisorFeatures& s,
                                                                            const LocationScope& l) {
        Json::Value json;

        json["vertexAttributeInstanceRateDivisor"] =
            gen_VkBool32(s.vertexAttributeInstanceRateDivisor, CreateScope("vertexAttributeInstanceRateDivisor"));
        json["vertexAttributeInstanceRateZeroDivisor"] =
            gen_VkBool32(s.vertexAttributeInstanceRateZeroDivisor, CreateScope("vertexAttributeInstanceRateZeroDivisor"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT_contents(
        const VkPhysicalDeviceVertexInputDynamicStateFeaturesEXT& s, const LocationScope& l) {
        Json::Value json;

        json["vertexInputDynamicState"] = gen_VkBool32(s.vertexInputDynamicState, CreateScope("vertexInputDynamicState"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceVulkan11Features_contents(const VkPhysicalDeviceVulkan11Features& s, const LocationScope& l) {
        Json::Value json;

        json["storageBuffer16BitAccess"] = gen_VkBool32(s.storageBuffer16BitAccess, CreateScope("storageBuffer16BitAccess"));
        json["uniformAndStorageBuffer16BitAccess"] =
            gen_VkBool32(s.uniformAndStorageBuffer16BitAccess, CreateScope("uniformAndStorageBuffer16BitAccess"));
        json["storagePushConstant16"] = gen_VkBool32(s.storagePushConstant16, CreateScope("storagePushConstant16"));
        json["storageInputOutput16"] = gen_VkBool32(s.storageInputOutput16, CreateScope("storageInputOutput16"));
        json["multiview"] = gen_VkBool32(s.multiview, CreateScope("multiview"));
        json["multiviewGeometryShader"] = gen_VkBool32(s.multiviewGeometryShader, CreateScope("multiviewGeometryShader"));
        json["multiviewTessellationShader"] =
            gen_VkBool32(s.multiviewTessellationShader, CreateScope("multiviewTessellationShader"));
        json["variablePointersStorageBuffer"] =
            gen_VkBool32(s.variablePointersStorageBuffer, CreateScope("variablePointersStorageBuffer"));
        json["variablePointers"] = gen_VkBool32(s.variablePointers, CreateScope("variablePointers"));
        json["protectedMemory"] = gen_VkBool32(s.protectedMemory, CreateScope("protectedMemory"));
        json["samplerYcbcrConversion"] = gen_VkBool32(s.samplerYcbcrConversion, CreateScope("samplerYcbcrConversion"));
        json["shaderDrawParameters"] = gen_VkBool32(s.shaderDrawParameters, CreateScope("shaderDrawParameters"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceVulkan12Features_contents(const VkPhysicalDeviceVulkan12Features& s, const LocationScope& l) {
        Json::Value json;

        json["samplerMirrorClampToEdge"] = gen_VkBool32(s.samplerMirrorClampToEdge, CreateScope("samplerMirrorClampToEdge"));
        json["drawIndirectCount"] = gen_VkBool32(s.drawIndirectCount, CreateScope("drawIndirectCount"));
        json["storageBuffer8BitAccess"] = gen_VkBool32(s.storageBuffer8BitAccess, CreateScope("storageBuffer8BitAccess"));
        json["uniformAndStorageBuffer8BitAccess"] =
            gen_VkBool32(s.uniformAndStorageBuffer8BitAccess, CreateScope("uniformAndStorageBuffer8BitAccess"));
        json["storagePushConstant8"] = gen_VkBool32(s.storagePushConstant8, CreateScope("storagePushConstant8"));
        json["shaderBufferInt64Atomics"] = gen_VkBool32(s.shaderBufferInt64Atomics, CreateScope("shaderBufferInt64Atomics"));
        json["shaderSharedInt64Atomics"] = gen_VkBool32(s.shaderSharedInt64Atomics, CreateScope("shaderSharedInt64Atomics"));
        json["shaderFloat16"] = gen_VkBool32(s.shaderFloat16, CreateScope("shaderFloat16"));
        json["shaderInt8"] = gen_VkBool32(s.shaderInt8, CreateScope("shaderInt8"));
        json["descriptorIndexing"] = gen_VkBool32(s.descriptorIndexing, CreateScope("descriptorIndexing"));
        json["shaderInputAttachmentArrayDynamicIndexing"] =
            gen_VkBool32(s.shaderInputAttachmentArrayDynamicIndexing, CreateScope("shaderInputAttachmentArrayDynamicIndexing"));
        json["shaderUniformTexelBufferArrayDynamicIndexing"] = gen_VkBool32(
            s.shaderUniformTexelBufferArrayDynamicIndexing, CreateScope("shaderUniformTexelBufferArrayDynamicIndexing"));
        json["shaderStorageTexelBufferArrayDynamicIndexing"] = gen_VkBool32(
            s.shaderStorageTexelBufferArrayDynamicIndexing, CreateScope("shaderStorageTexelBufferArrayDynamicIndexing"));
        json["shaderUniformBufferArrayNonUniformIndexing"] =
            gen_VkBool32(s.shaderUniformBufferArrayNonUniformIndexing, CreateScope("shaderUniformBufferArrayNonUniformIndexing"));
        json["shaderSampledImageArrayNonUniformIndexing"] =
            gen_VkBool32(s.shaderSampledImageArrayNonUniformIndexing, CreateScope("shaderSampledImageArrayNonUniformIndexing"));
        json["shaderStorageBufferArrayNonUniformIndexing"] =
            gen_VkBool32(s.shaderStorageBufferArrayNonUniformIndexing, CreateScope("shaderStorageBufferArrayNonUniformIndexing"));
        json["shaderStorageImageArrayNonUniformIndexing"] =
            gen_VkBool32(s.shaderStorageImageArrayNonUniformIndexing, CreateScope("shaderStorageImageArrayNonUniformIndexing"));
        json["shaderInputAttachmentArrayNonUniformIndexing"] = gen_VkBool32(
            s.shaderInputAttachmentArrayNonUniformIndexing, CreateScope("shaderInputAttachmentArrayNonUniformIndexing"));
        json["shaderUniformTexelBufferArrayNonUniformIndexing"] = gen_VkBool32(
            s.shaderUniformTexelBufferArrayNonUniformIndexing, CreateScope("shaderUniformTexelBufferArrayNonUniformIndexing"));
        json["shaderStorageTexelBufferArrayNonUniformIndexing"] = gen_VkBool32(
            s.shaderStorageTexelBufferArrayNonUniformIndexing, CreateScope("shaderStorageTexelBufferArrayNonUniformIndexing"));
        json["descriptorBindingUniformBufferUpdateAfterBind"] = gen_VkBool32(
            s.descriptorBindingUniformBufferUpdateAfterBind, CreateScope("descriptorBindingUniformBufferUpdateAfterBind"));
        json["descriptorBindingSampledImageUpdateAfterBind"] = gen_VkBool32(
            s.descriptorBindingSampledImageUpdateAfterBind, CreateScope("descriptorBindingSampledImageUpdateAfterBind"));
        json["descriptorBindingStorageImageUpdateAfterBind"] = gen_VkBool32(
            s.descriptorBindingStorageImageUpdateAfterBind, CreateScope("descriptorBindingStorageImageUpdateAfterBind"));
        json["descriptorBindingStorageBufferUpdateAfterBind"] = gen_VkBool32(
            s.descriptorBindingStorageBufferUpdateAfterBind, CreateScope("descriptorBindingStorageBufferUpdateAfterBind"));
        json["descriptorBindingUniformTexelBufferUpdateAfterBind"] =
            gen_VkBool32(s.descriptorBindingUniformTexelBufferUpdateAfterBind,
                         CreateScope("descriptorBindingUniformTexelBufferUpdateAfterBind"));
        json["descriptorBindingStorageTexelBufferUpdateAfterBind"] =
            gen_VkBool32(s.descriptorBindingStorageTexelBufferUpdateAfterBind,
                         CreateScope("descriptorBindingStorageTexelBufferUpdateAfterBind"));
        json["descriptorBindingUpdateUnusedWhilePending"] =
            gen_VkBool32(s.descriptorBindingUpdateUnusedWhilePending, CreateScope("descriptorBindingUpdateUnusedWhilePending"));
        json["descriptorBindingPartiallyBound"] =
            gen_VkBool32(s.descriptorBindingPartiallyBound, CreateScope("descriptorBindingPartiallyBound"));
        json["descriptorBindingVariableDescriptorCount"] =
            gen_VkBool32(s.descriptorBindingVariableDescriptorCount, CreateScope("descriptorBindingVariableDescriptorCount"));
        json["runtimeDescriptorArray"] = gen_VkBool32(s.runtimeDescriptorArray, CreateScope("runtimeDescriptorArray"));
        json["samplerFilterMinmax"] = gen_VkBool32(s.samplerFilterMinmax, CreateScope("samplerFilterMinmax"));
        json["scalarBlockLayout"] = gen_VkBool32(s.scalarBlockLayout, CreateScope("scalarBlockLayout"));
        json["imagelessFramebuffer"] = gen_VkBool32(s.imagelessFramebuffer, CreateScope("imagelessFramebuffer"));
        json["uniformBufferStandardLayout"] =
            gen_VkBool32(s.uniformBufferStandardLayout, CreateScope("uniformBufferStandardLayout"));
        json["shaderSubgroupExtendedTypes"] =
            gen_VkBool32(s.shaderSubgroupExtendedTypes, CreateScope("shaderSubgroupExtendedTypes"));
        json["separateDepthStencilLayouts"] =
            gen_VkBool32(s.separateDepthStencilLayouts, CreateScope("separateDepthStencilLayouts"));
        json["hostQueryReset"] = gen_VkBool32(s.hostQueryReset, CreateScope("hostQueryReset"));
        json["timelineSemaphore"] = gen_VkBool32(s.timelineSemaphore, CreateScope("timelineSemaphore"));
        json["bufferDeviceAddress"] = gen_VkBool32(s.bufferDeviceAddress, CreateScope("bufferDeviceAddress"));
        json["bufferDeviceAddressCaptureReplay"] =
            gen_VkBool32(s.bufferDeviceAddressCaptureReplay, CreateScope("bufferDeviceAddressCaptureReplay"));
        json["bufferDeviceAddressMultiDevice"] =
            gen_VkBool32(s.bufferDeviceAddressMultiDevice, CreateScope("bufferDeviceAddressMultiDevice"));
        json["vulkanMemoryModel"] = gen_VkBool32(s.vulkanMemoryModel, CreateScope("vulkanMemoryModel"));
        json["vulkanMemoryModelDeviceScope"] =
            gen_VkBool32(s.vulkanMemoryModelDeviceScope, CreateScope("vulkanMemoryModelDeviceScope"));
        json["vulkanMemoryModelAvailabilityVisibilityChains"] = gen_VkBool32(
            s.vulkanMemoryModelAvailabilityVisibilityChains, CreateScope("vulkanMemoryModelAvailabilityVisibilityChains"));
        json["shaderOutputViewportIndex"] = gen_VkBool32(s.shaderOutputViewportIndex, CreateScope("shaderOutputViewportIndex"));
        json["shaderOutputLayer"] = gen_VkBool32(s.shaderOutputLayer, CreateScope("shaderOutputLayer"));
        json["subgroupBroadcastDynamicId"] = gen_VkBool32(s.subgroupBroadcastDynamicId, CreateScope("subgroupBroadcastDynamicId"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceVulkan13Features_contents(const VkPhysicalDeviceVulkan13Features& s, const LocationScope& l) {
        Json::Value json;

        json["robustImageAccess"] = gen_VkBool32(s.robustImageAccess, CreateScope("robustImageAccess"));
        json["inlineUniformBlock"] = gen_VkBool32(s.inlineUniformBlock, CreateScope("inlineUniformBlock"));
        json["descriptorBindingInlineUniformBlockUpdateAfterBind"] =
            gen_VkBool32(s.descriptorBindingInlineUniformBlockUpdateAfterBind,
                         CreateScope("descriptorBindingInlineUniformBlockUpdateAfterBind"));
        json["pipelineCreationCacheControl"] =
            gen_VkBool32(s.pipelineCreationCacheControl, CreateScope("pipelineCreationCacheControl"));
        json["privateData"] = gen_VkBool32(s.privateData, CreateScope("privateData"));
        json["shaderDemoteToHelperInvocation"] =
            gen_VkBool32(s.shaderDemoteToHelperInvocation, CreateScope("shaderDemoteToHelperInvocation"));
        json["shaderTerminateInvocation"] = gen_VkBool32(s.shaderTerminateInvocation, CreateScope("shaderTerminateInvocation"));
        json["subgroupSizeControl"] = gen_VkBool32(s.subgroupSizeControl, CreateScope("subgroupSizeControl"));
        json["computeFullSubgroups"] = gen_VkBool32(s.computeFullSubgroups, CreateScope("computeFullSubgroups"));
        json["synchronization2"] = gen_VkBool32(s.synchronization2, CreateScope("synchronization2"));
        json["textureCompressionASTC_HDR"] = gen_VkBool32(s.textureCompressionASTC_HDR, CreateScope("textureCompressionASTC_HDR"));
        json["shaderZeroInitializeWorkgroupMemory"] =
            gen_VkBool32(s.shaderZeroInitializeWorkgroupMemory, CreateScope("shaderZeroInitializeWorkgroupMemory"));
        json["dynamicRendering"] = gen_VkBool32(s.dynamicRendering, CreateScope("dynamicRendering"));
        json["shaderIntegerDotProduct"] = gen_VkBool32(s.shaderIntegerDotProduct, CreateScope("shaderIntegerDotProduct"));
        json["maintenance4"] = gen_VkBool32(s.maintenance4, CreateScope("maintenance4"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceVulkan14Features_contents(const VkPhysicalDeviceVulkan14Features& s, const LocationScope& l) {
        Json::Value json;

        json["globalPriorityQuery"] = gen_VkBool32(s.globalPriorityQuery, CreateScope("globalPriorityQuery"));
        json["shaderSubgroupRotate"] = gen_VkBool32(s.shaderSubgroupRotate, CreateScope("shaderSubgroupRotate"));
        json["shaderSubgroupRotateClustered"] =
            gen_VkBool32(s.shaderSubgroupRotateClustered, CreateScope("shaderSubgroupRotateClustered"));
        json["shaderFloatControls2"] = gen_VkBool32(s.shaderFloatControls2, CreateScope("shaderFloatControls2"));
        json["shaderExpectAssume"] = gen_VkBool32(s.shaderExpectAssume, CreateScope("shaderExpectAssume"));
        json["rectangularLines"] = gen_VkBool32(s.rectangularLines, CreateScope("rectangularLines"));
        json["bresenhamLines"] = gen_VkBool32(s.bresenhamLines, CreateScope("bresenhamLines"));
        json["smoothLines"] = gen_VkBool32(s.smoothLines, CreateScope("smoothLines"));
        json["stippledRectangularLines"] = gen_VkBool32(s.stippledRectangularLines, CreateScope("stippledRectangularLines"));
        json["stippledBresenhamLines"] = gen_VkBool32(s.stippledBresenhamLines, CreateScope("stippledBresenhamLines"));
        json["stippledSmoothLines"] = gen_VkBool32(s.stippledSmoothLines, CreateScope("stippledSmoothLines"));
        json["vertexAttributeInstanceRateDivisor"] =
            gen_VkBool32(s.vertexAttributeInstanceRateDivisor, CreateScope("vertexAttributeInstanceRateDivisor"));
        json["vertexAttributeInstanceRateZeroDivisor"] =
            gen_VkBool32(s.vertexAttributeInstanceRateZeroDivisor, CreateScope("vertexAttributeInstanceRateZeroDivisor"));
        json["indexTypeUint8"] = gen_VkBool32(s.indexTypeUint8, CreateScope("indexTypeUint8"));
        json["dynamicRenderingLocalRead"] = gen_VkBool32(s.dynamicRenderingLocalRead, CreateScope("dynamicRenderingLocalRead"));
        json["maintenance5"] = gen_VkBool32(s.maintenance5, CreateScope("maintenance5"));
        json["maintenance6"] = gen_VkBool32(s.maintenance6, CreateScope("maintenance6"));
        json["pipelineProtectedAccess"] = gen_VkBool32(s.pipelineProtectedAccess, CreateScope("pipelineProtectedAccess"));
        json["pipelineRobustness"] = gen_VkBool32(s.pipelineRobustness, CreateScope("pipelineRobustness"));
        json["hostImageCopy"] = gen_VkBool32(s.hostImageCopy, CreateScope("hostImageCopy"));
        json["pushDescriptor"] = gen_VkBool32(s.pushDescriptor, CreateScope("pushDescriptor"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceVulkanMemoryModelFeatures_contents(const VkPhysicalDeviceVulkanMemoryModelFeatures& s,
                                                                       const LocationScope& l) {
        Json::Value json;

        json["vulkanMemoryModel"] = gen_VkBool32(s.vulkanMemoryModel, CreateScope("vulkanMemoryModel"));
        json["vulkanMemoryModelDeviceScope"] =
            gen_VkBool32(s.vulkanMemoryModelDeviceScope, CreateScope("vulkanMemoryModelDeviceScope"));
        json["vulkanMemoryModelAvailabilityVisibilityChains"] = gen_VkBool32(
            s.vulkanMemoryModelAvailabilityVisibilityChains, CreateScope("vulkanMemoryModelAvailabilityVisibilityChains"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceVulkanSC10Features_contents(const VkPhysicalDeviceVulkanSC10Features& s,
                                                                const LocationScope& l) {
        Json::Value json;

        json["shaderAtomicInstructions"] = gen_VkBool32(s.shaderAtomicInstructions, CreateScope("shaderAtomicInstructions"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT_contents(
        const VkPhysicalDeviceYcbcr2Plane444FormatsFeaturesEXT& s, const LocationScope& l) {
        Json::Value json;

        json["ycbcr2plane444Formats"] = gen_VkBool32(s.ycbcr2plane444Formats, CreateScope("ycbcr2plane444Formats"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceYcbcrImageArraysFeaturesEXT_contents(const VkPhysicalDeviceYcbcrImageArraysFeaturesEXT& s,
                                                                         const LocationScope& l) {
        Json::Value json;

        json["ycbcrImageArrays"] = gen_VkBool32(s.ycbcrImageArrays, CreateScope("ycbcrImageArrays"));

        return json;
    }

    Json::Value gen_VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeatures_contents(
        const VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeatures& s, const LocationScope& l) {
        Json::Value json;

        json["shaderZeroInitializeWorkgroupMemory"] =
            gen_VkBool32(s.shaderZeroInitializeWorkgroupMemory, CreateScope("shaderZeroInitializeWorkgroupMemory"));

        return json;
    }

    Json::Value gen_VkAttachmentDescription_contents(const VkAttachmentDescription& s, const LocationScope& l) {
        Json::Value json;

        json["flags"] = gen_VkAttachmentDescriptionFlags(s.flags, CreateScope("flags"));
        json["format"] = gen_VkFormat(s.format, CreateScope("format"));
        json["samples"] = gen_VkSampleCountFlagBits(s.samples, CreateScope("samples"));
        json["loadOp"] = gen_VkAttachmentLoadOp(s.loadOp, CreateScope("loadOp"));
        json["storeOp"] = gen_VkAttachmentStoreOp(s.storeOp, CreateScope("storeOp"));
        json["stencilLoadOp"] = gen_VkAttachmentLoadOp(s.stencilLoadOp, CreateScope("stencilLoadOp"));
        json["stencilStoreOp"] = gen_VkAttachmentStoreOp(s.stencilStoreOp, CreateScope("stencilStoreOp"));
        json["initialLayout"] = gen_VkImageLayout(s.initialLayout, CreateScope("initialLayout"));
        json["finalLayout"] = gen_VkImageLayout(s.finalLayout, CreateScope("finalLayout"));

        return json;
    }

    Json::Value gen_VkAttachmentReference_contents(const VkAttachmentReference& s, const LocationScope& l) {
        Json::Value json;

        json["attachment"] = gen_uint32_t(s.attachment, CreateScope("attachment"));
        json["layout"] = gen_VkImageLayout(s.layout, CreateScope("layout"));

        return json;
    }

    Json::Value gen_VkSubpassDescription_contents(const VkSubpassDescription& s, const LocationScope& l) {
        Json::Value json;

        json["flags"] = gen_VkSubpassDescriptionFlags(s.flags, CreateScope("flags"));
        json["pipelineBindPoint"] = gen_VkPipelineBindPoint(s.pipelineBindPoint, CreateScope("pipelineBindPoint"));
        json["inputAttachmentCount"] = gen_uint32_t(s.inputAttachmentCount, CreateScope("inputAttachmentCount"));

        if (s.inputAttachmentCount != 0) {
            Json::Value json_array_pInputAttachments;
            if (s.pInputAttachments != nullptr) {
                for (Json::Value::ArrayIndex i = 0; i < s.inputAttachmentCount; i++) {
                    json_array_pInputAttachments[i] =
                        gen_VkAttachmentReference_contents(s.pInputAttachments[i], CreateScope("pInputAttachments", i));
                }
                json["pInputAttachments"] = json_array_pInputAttachments;
            } else {
                Error() << "pInputAttachments is NULL but its length is " << s.inputAttachmentCount;
            }
        } else {
            json["pInputAttachments"] = "NULL";
        }
        json["colorAttachmentCount"] = gen_uint32_t(s.colorAttachmentCount, CreateScope("colorAttachmentCount"));

        if (s.colorAttachmentCount != 0) {
            Json::Value json_array_pColorAttachments;
            if (s.pColorAttachments != nullptr) {
                for (Json::Value::ArrayIndex i = 0; i < s.colorAttachmentCount; i++) {
                    json_array_pColorAttachments[i] =
                        gen_VkAttachmentReference_contents(s.pColorAttachments[i], CreateScope("pColorAttachments", i));
                }
                json["pColorAttachments"] = json_array_pColorAttachments;
            } else {
                Error() << "pColorAttachments is NULL but its length is " << s.colorAttachmentCount;
            }
        } else {
            json["pColorAttachments"] = "NULL";
        }

        if (s.colorAttachmentCount != 0) {
            Json::Value json_array_pResolveAttachments;
            if (s.pResolveAttachments != nullptr) {
                for (Json::Value::ArrayIndex i = 0; i < s.colorAttachmentCount; i++) {
                    json_array_pResolveAttachments[i] =
                        gen_VkAttachmentReference_contents(s.pResolveAttachments[i], CreateScope("pResolveAttachments", i));
                }
                json["pResolveAttachments"] = json_array_pResolveAttachments;
            } else {
                json["pResolveAttachments"] = "NULL";
            }
        } else {
            json["pResolveAttachments"] = "NULL";
        }
        json["pDepthStencilAttachment"] =
            s.pDepthStencilAttachment
                ? gen_VkAttachmentReference_contents(*s.pDepthStencilAttachment, CreateScope("pDepthStencilAttachment", true))
                : "NULL";
        json["preserveAttachmentCount"] = gen_uint32_t(s.preserveAttachmentCount, CreateScope("preserveAttachmentCount"));

        if (s.preserveAttachmentCount != 0) {
            Json::Value json_array_pPreserveAttachments;
            if (s.pPreserveAttachments != nullptr) {
                for (Json::Value::ArrayIndex i = 0; i < s.preserveAttachmentCount; i++) {
                    json_array_pPreserveAttachments[i] =
                        gen_uint32_t(s.pPreserveAttachments[i], CreateScope("pPreserveAttachments", i));
                }
                json["pPreserveAttachments"] = json_array_pPreserveAttachments;
            } else {
                Error() << "pPreserveAttachments is NULL but its length is " << s.preserveAttachmentCount;
            }
        } else {
            json["pPreserveAttachments"] = "NULL";
        }

        return json;
    }

    Json::Value gen_VkSubpassDependency_contents(const VkSubpassDependency& s, const LocationScope& l) {
        Json::Value json;

        json["srcSubpass"] = gen_uint32_t(s.srcSubpass, CreateScope("srcSubpass"));
        json["dstSubpass"] = gen_uint32_t(s.dstSubpass, CreateScope("dstSubpass"));
        json["srcStageMask"] = gen_VkPipelineStageFlags(s.srcStageMask, CreateScope("srcStageMask"));
        json["dstStageMask"] = gen_VkPipelineStageFlags(s.dstStageMask, CreateScope("dstStageMask"));
        json["srcAccessMask"] = gen_VkAccessFlags(s.srcAccessMask, CreateScope("srcAccessMask"));
        json["dstAccessMask"] = gen_VkAccessFlags(s.dstAccessMask, CreateScope("dstAccessMask"));
        json["dependencyFlags"] = gen_VkDependencyFlags(s.dependencyFlags, CreateScope("dependencyFlags"));

        return json;
    }

    Json::Value gen_VkRenderPassCreateInfo_contents(const VkRenderPassCreateInfo& s, const LocationScope& l) {
        Json::Value json;

        json["flags"] = gen_VkRenderPassCreateFlags(s.flags, CreateScope("flags"));
        json["attachmentCount"] = gen_uint32_t(s.attachmentCount, CreateScope("attachmentCount"));

        if (s.attachmentCount != 0) {
            Json::Value json_array_pAttachments;
            if (s.pAttachments != nullptr) {
                for (Json::Value::ArrayIndex i = 0; i < s.attachmentCount; i++) {
                    json_array_pAttachments[i] =
                        gen_VkAttachmentDescription_contents(s.pAttachments[i], CreateScope("pAttachments", i));
                }
                json["pAttachments"] = json_array_pAttachments;
            } else {
                Error() << "pAttachments is NULL but its length is " << s.attachmentCount;
            }
        } else {
            json["pAttachments"] = "NULL";
        }
        json["subpassCount"] = gen_uint32_t(s.subpassCount, CreateScope("subpassCount"));

        if (s.subpassCount != 0) {
            Json::Value json_array_pSubpasses;
            if (s.pSubpasses != nullptr) {
                for (Json::Value::ArrayIndex i = 0; i < s.subpassCount; i++) {
                    json_array_pSubpasses[i] = gen_VkSubpassDescription_contents(s.pSubpasses[i], CreateScope("pSubpasses", i));
                }
                json["pSubpasses"] = json_array_pSubpasses;
            } else {
                Error() << "pSubpasses is NULL but its length is " << s.subpassCount;
            }
        } else {
            json["pSubpasses"] = "NULL";
        }
        json["dependencyCount"] = gen_uint32_t(s.dependencyCount, CreateScope("dependencyCount"));

        if (s.dependencyCount != 0) {
            Json::Value json_array_pDependencies;
            if (s.pDependencies != nullptr) {
                for (Json::Value::ArrayIndex i = 0; i < s.dependencyCount; i++) {
                    json_array_pDependencies[i] =
                        gen_VkSubpassDependency_contents(s.pDependencies[i], CreateScope("pDependencies", i));
                }
                json["pDependencies"] = json_array_pDependencies;
            } else {
                Error() << "pDependencies is NULL but its length is " << s.dependencyCount;
            }
        } else {
            json["pDependencies"] = "NULL";
        }

        return json;
    }

    Json::Value gen_VkInputAttachmentAspectReference_contents(const VkInputAttachmentAspectReference& s, const LocationScope& l) {
        Json::Value json;

        json["subpass"] = gen_uint32_t(s.subpass, CreateScope("subpass"));
        json["inputAttachmentIndex"] = gen_uint32_t(s.inputAttachmentIndex, CreateScope("inputAttachmentIndex"));
        json["aspectMask"] = gen_VkImageAspectFlags(s.aspectMask, CreateScope("aspectMask"));

        return json;
    }

    Json::Value gen_VkRenderPassInputAttachmentAspectCreateInfo_contents(const VkRenderPassInputAttachmentAspectCreateInfo& s,
                                                                         const LocationScope& l) {
        Json::Value json;

        json["aspectReferenceCount"] = gen_uint32_t(s.aspectReferenceCount, CreateScope("aspectReferenceCount"));

        if (s.aspectReferenceCount != 0) {
            Json::Value json_array_pAspectReferences;
            if (s.pAspectReferences != nullptr) {
                for (Json::Value::ArrayIndex i = 0; i < s.aspectReferenceCount; i++) {
                    json_array_pAspectReferences[i] =
                        gen_VkInputAttachmentAspectReference_contents(s.pAspectReferences[i], CreateScope("pAspectReferences", i));
                }
                json["pAspectReferences"] = json_array_pAspectReferences;
            } else {
                Error() << "pAspectReferences is NULL but its length is " << s.aspectReferenceCount;
            }
        } else {
            json["pAspectReferences"] = "NULL";
        }

        return json;
    }

    Json::Value gen_VkRenderPassMultiviewCreateInfo_contents(const VkRenderPassMultiviewCreateInfo& s, const LocationScope& l) {
        Json::Value json;

        json["subpassCount"] = gen_uint32_t(s.subpassCount, CreateScope("subpassCount"));

        if (s.subpassCount != 0) {
            Json::Value json_array_pViewMasks;
            if (s.pViewMasks != nullptr) {
                for (Json::Value::ArrayIndex i = 0; i < s.subpassCount; i++) {
                    json_array_pViewMasks[i] = gen_uint32_t(s.pViewMasks[i], CreateScope("pViewMasks", i));
                }
                json["pViewMasks"] = json_array_pViewMasks;
            } else {
                Error() << "pViewMasks is NULL but its length is " << s.subpassCount;
            }
        } else {
            json["pViewMasks"] = "NULL";
        }
        json["dependencyCount"] = gen_uint32_t(s.dependencyCount, CreateScope("dependencyCount"));

        if (s.dependencyCount != 0) {
            Json::Value json_array_pViewOffsets;
            if (s.pViewOffsets != nullptr) {
                for (Json::Value::ArrayIndex i = 0; i < s.dependencyCount; i++) {
                    json_array_pViewOffsets[i] = gen_int32_t(s.pViewOffsets[i], CreateScope("pViewOffsets", i));
                }
                json["pViewOffsets"] = json_array_pViewOffsets;
            } else {
                Error() << "pViewOffsets is NULL but its length is " << s.dependencyCount;
            }
        } else {
            json["pViewOffsets"] = "NULL";
        }
        json["correlationMaskCount"] = gen_uint32_t(s.correlationMaskCount, CreateScope("correlationMaskCount"));

        if (s.correlationMaskCount != 0) {
            Json::Value json_array_pCorrelationMasks;
            if (s.pCorrelationMasks != nullptr) {
                for (Json::Value::ArrayIndex i = 0; i < s.correlationMaskCount; i++) {
                    json_array_pCorrelationMasks[i] = gen_uint32_t(s.pCorrelationMasks[i], CreateScope("pCorrelationMasks", i));
                }
                json["pCorrelationMasks"] = json_array_pCorrelationMasks;
            } else {
                Error() << "pCorrelationMasks is NULL but its length is " << s.correlationMaskCount;
            }
        } else {
            json["pCorrelationMasks"] = "NULL";
        }

        return json;
    }

    Json::Value gen_VkAttachmentDescription2_contents(const VkAttachmentDescription2& s, const LocationScope& l) {
        Json::Value json;

        json["flags"] = gen_VkAttachmentDescriptionFlags(s.flags, CreateScope("flags"));
        json["format"] = gen_VkFormat(s.format, CreateScope("format"));
        json["samples"] = gen_VkSampleCountFlagBits(s.samples, CreateScope("samples"));
        json["loadOp"] = gen_VkAttachmentLoadOp(s.loadOp, CreateScope("loadOp"));
        json["storeOp"] = gen_VkAttachmentStoreOp(s.storeOp, CreateScope("storeOp"));
        json["stencilLoadOp"] = gen_VkAttachmentLoadOp(s.stencilLoadOp, CreateScope("stencilLoadOp"));
        json["stencilStoreOp"] = gen_VkAttachmentStoreOp(s.stencilStoreOp, CreateScope("stencilStoreOp"));
        json["initialLayout"] = gen_VkImageLayout(s.initialLayout, CreateScope("initialLayout"));
        json["finalLayout"] = gen_VkImageLayout(s.finalLayout, CreateScope("finalLayout"));

        return json;
    }

    Json::Value gen_VkAttachmentDescriptionStencilLayout_contents(const VkAttachmentDescriptionStencilLayout& s,
                                                                  const LocationScope& l) {
        Json::Value json;

        json["stencilInitialLayout"] = gen_VkImageLayout(s.stencilInitialLayout, CreateScope("stencilInitialLayout"));
        json["stencilFinalLayout"] = gen_VkImageLayout(s.stencilFinalLayout, CreateScope("stencilFinalLayout"));

        return json;
    }

    Json::Value gen_VkAttachmentReference2_contents(const VkAttachmentReference2& s, const LocationScope& l) {
        Json::Value json;

        json["attachment"] = gen_uint32_t(s.attachment, CreateScope("attachment"));
        json["layout"] = gen_VkImageLayout(s.layout, CreateScope("layout"));
        json["aspectMask"] = gen_VkImageAspectFlags(s.aspectMask, CreateScope("aspectMask"));

        return json;
    }

    Json::Value gen_VkAttachmentReferenceStencilLayout_contents(const VkAttachmentReferenceStencilLayout& s,
                                                                const LocationScope& l) {
        Json::Value json;

        json["stencilLayout"] = gen_VkImageLayout(s.stencilLayout, CreateScope("stencilLayout"));

        return json;
    }

    Json::Value gen_VkSubpassDescription2_contents(const VkSubpassDescription2& s, const LocationScope& l) {
        Json::Value json;

        json["flags"] = gen_VkSubpassDescriptionFlags(s.flags, CreateScope("flags"));
        json["pipelineBindPoint"] = gen_VkPipelineBindPoint(s.pipelineBindPoint, CreateScope("pipelineBindPoint"));
        json["viewMask"] = gen_uint32_t(s.viewMask, CreateScope("viewMask"));
        json["inputAttachmentCount"] = gen_uint32_t(s.inputAttachmentCount, CreateScope("inputAttachmentCount"));

        if (s.inputAttachmentCount != 0) {
            Json::Value json_array_pInputAttachments;
            if (s.pInputAttachments != nullptr) {
                for (Json::Value::ArrayIndex i = 0; i < s.inputAttachmentCount; i++) {
                    json_array_pInputAttachments[i] =
                        gen_VkAttachmentReference2(s.pInputAttachments[i], CreateScope("pInputAttachments", i));
                }
                json["pInputAttachments"] = json_array_pInputAttachments;
            } else {
                Error() << "pInputAttachments is NULL but its length is " << s.inputAttachmentCount;
            }
        } else {
            json["pInputAttachments"] = "NULL";
        }
        json["colorAttachmentCount"] = gen_uint32_t(s.colorAttachmentCount, CreateScope("colorAttachmentCount"));

        if (s.colorAttachmentCount != 0) {
            Json::Value json_array_pColorAttachments;
            if (s.pColorAttachments != nullptr) {
                for (Json::Value::ArrayIndex i = 0; i < s.colorAttachmentCount; i++) {
                    json_array_pColorAttachments[i] =
                        gen_VkAttachmentReference2(s.pColorAttachments[i], CreateScope("pColorAttachments", i));
                }
                json["pColorAttachments"] = json_array_pColorAttachments;
            } else {
                Error() << "pColorAttachments is NULL but its length is " << s.colorAttachmentCount;
            }
        } else {
            json["pColorAttachments"] = "NULL";
        }

        if (s.colorAttachmentCount != 0) {
            Json::Value json_array_pResolveAttachments;
            if (s.pResolveAttachments != nullptr) {
                for (Json::Value::ArrayIndex i = 0; i < s.colorAttachmentCount; i++) {
                    json_array_pResolveAttachments[i] =
                        gen_VkAttachmentReference2(s.pResolveAttachments[i], CreateScope("pResolveAttachments", i));
                }
                json["pResolveAttachments"] = json_array_pResolveAttachments;
            } else {
                json["pResolveAttachments"] = "NULL";
            }
        } else {
            json["pResolveAttachments"] = "NULL";
        }
        json["pDepthStencilAttachment"] =
            s.pDepthStencilAttachment
                ? gen_VkAttachmentReference2(*s.pDepthStencilAttachment, CreateScope("pDepthStencilAttachment", true))
                : "NULL";
        json["preserveAttachmentCount"] = gen_uint32_t(s.preserveAttachmentCount, CreateScope("preserveAttachmentCount"));

        if (s.preserveAttachmentCount != 0) {
            Json::Value json_array_pPreserveAttachments;
            if (s.pPreserveAttachments != nullptr) {
                for (Json::Value::ArrayIndex i = 0; i < s.preserveAttachmentCount; i++) {
                    json_array_pPreserveAttachments[i] =
                        gen_uint32_t(s.pPreserveAttachments[i], CreateScope("pPreserveAttachments", i));
                }
                json["pPreserveAttachments"] = json_array_pPreserveAttachments;
            } else {
                Error() << "pPreserveAttachments is NULL but its length is " << s.preserveAttachmentCount;
            }
        } else {
            json["pPreserveAttachments"] = "NULL";
        }

        return json;
    }

    Json::Value gen_VkFragmentShadingRateAttachmentInfoKHR_contents(const VkFragmentShadingRateAttachmentInfoKHR& s,
                                                                    const LocationScope& l) {
        Json::Value json;

        json["pFragmentShadingRateAttachment"] =
            s.pFragmentShadingRateAttachment
                ? gen_VkAttachmentReference2(*s.pFragmentShadingRateAttachment, CreateScope("pFragmentShadingRateAttachment", true))
                : "NULL";
        json["shadingRateAttachmentTexelSize"] =
            gen_VkExtent2D_contents(s.shadingRateAttachmentTexelSize, CreateScope("shadingRateAttachmentTexelSize"));

        return json;
    }

    Json::Value gen_VkSubpassDescriptionDepthStencilResolve_contents(const VkSubpassDescriptionDepthStencilResolve& s,
                                                                     const LocationScope& l) {
        Json::Value json;

        json["depthResolveMode"] = gen_VkResolveModeFlagBits(s.depthResolveMode, CreateScope("depthResolveMode"));
        json["stencilResolveMode"] = gen_VkResolveModeFlagBits(s.stencilResolveMode, CreateScope("stencilResolveMode"));
        json["pDepthStencilResolveAttachment"] =
            s.pDepthStencilResolveAttachment
                ? gen_VkAttachmentReference2(*s.pDepthStencilResolveAttachment, CreateScope("pDepthStencilResolveAttachment", true))
                : "NULL";
        return json;
    }

    Json::Value gen_VkSubpassDependency2_contents(const VkSubpassDependency2& s, const LocationScope& l) {
        Json::Value json;

        json["srcSubpass"] = gen_uint32_t(s.srcSubpass, CreateScope("srcSubpass"));
        json["dstSubpass"] = gen_uint32_t(s.dstSubpass, CreateScope("dstSubpass"));
        json["srcStageMask"] = gen_VkPipelineStageFlags(s.srcStageMask, CreateScope("srcStageMask"));
        json["dstStageMask"] = gen_VkPipelineStageFlags(s.dstStageMask, CreateScope("dstStageMask"));
        json["srcAccessMask"] = gen_VkAccessFlags(s.srcAccessMask, CreateScope("srcAccessMask"));
        json["dstAccessMask"] = gen_VkAccessFlags(s.dstAccessMask, CreateScope("dstAccessMask"));
        json["dependencyFlags"] = gen_VkDependencyFlags(s.dependencyFlags, CreateScope("dependencyFlags"));
        json["viewOffset"] = gen_int32_t(s.viewOffset, CreateScope("viewOffset"));

        return json;
    }

    Json::Value gen_VkMemoryBarrier2_contents(const VkMemoryBarrier2& s, const LocationScope& l) {
        Json::Value json;

        json["srcStageMask"] = gen_VkPipelineStageFlags2(s.srcStageMask, CreateScope("srcStageMask"));
        json["srcAccessMask"] = gen_VkAccessFlags2(s.srcAccessMask, CreateScope("srcAccessMask"));
        json["dstStageMask"] = gen_VkPipelineStageFlags2(s.dstStageMask, CreateScope("dstStageMask"));
        json["dstAccessMask"] = gen_VkAccessFlags2(s.dstAccessMask, CreateScope("dstAccessMask"));

        return json;
    }

    Json::Value gen_VkRenderPassCreateInfo2_contents(const VkRenderPassCreateInfo2& s, const LocationScope& l) {
        Json::Value json;

        json["flags"] = gen_VkRenderPassCreateFlags(s.flags, CreateScope("flags"));
        json["attachmentCount"] = gen_uint32_t(s.attachmentCount, CreateScope("attachmentCount"));

        if (s.attachmentCount != 0) {
            Json::Value json_array_pAttachments;
            if (s.pAttachments != nullptr) {
                for (Json::Value::ArrayIndex i = 0; i < s.attachmentCount; i++) {
                    json_array_pAttachments[i] = gen_VkAttachmentDescription2(s.pAttachments[i], CreateScope("pAttachments", i));
                }
                json["pAttachments"] = json_array_pAttachments;
            } else {
                Error() << "pAttachments is NULL but its length is " << s.attachmentCount;
            }
        } else {
            json["pAttachments"] = "NULL";
        }
        json["subpassCount"] = gen_uint32_t(s.subpassCount, CreateScope("subpassCount"));

        if (s.subpassCount != 0) {
            Json::Value json_array_pSubpasses;
            if (s.pSubpasses != nullptr) {
                for (Json::Value::ArrayIndex i = 0; i < s.subpassCount; i++) {
                    json_array_pSubpasses[i] = gen_VkSubpassDescription2(s.pSubpasses[i], CreateScope("pSubpasses", i));
                }
                json["pSubpasses"] = json_array_pSubpasses;
            } else {
                Error() << "pSubpasses is NULL but its length is " << s.subpassCount;
            }
        } else {
            json["pSubpasses"] = "NULL";
        }
        json["dependencyCount"] = gen_uint32_t(s.dependencyCount, CreateScope("dependencyCount"));

        if (s.dependencyCount != 0) {
            Json::Value json_array_pDependencies;
            if (s.pDependencies != nullptr) {
                for (Json::Value::ArrayIndex i = 0; i < s.dependencyCount; i++) {
                    json_array_pDependencies[i] = gen_VkSubpassDependency2(s.pDependencies[i], CreateScope("pDependencies", i));
                }
                json["pDependencies"] = json_array_pDependencies;
            } else {
                Error() << "pDependencies is NULL but its length is " << s.dependencyCount;
            }
        } else {
            json["pDependencies"] = "NULL";
        }
        json["correlatedViewMaskCount"] = gen_uint32_t(s.correlatedViewMaskCount, CreateScope("correlatedViewMaskCount"));

        if (s.correlatedViewMaskCount != 0) {
            Json::Value json_array_pCorrelatedViewMasks;
            if (s.pCorrelatedViewMasks != nullptr) {
                for (Json::Value::ArrayIndex i = 0; i < s.correlatedViewMaskCount; i++) {
                    json_array_pCorrelatedViewMasks[i] =
                        gen_uint32_t(s.pCorrelatedViewMasks[i], CreateScope("pCorrelatedViewMasks", i));
                }
                json["pCorrelatedViewMasks"] = json_array_pCorrelatedViewMasks;
            } else {
                Error() << "pCorrelatedViewMasks is NULL but its length is " << s.correlatedViewMaskCount;
            }
        } else {
            json["pCorrelatedViewMasks"] = "NULL";
        }

        return json;
    }

    Json::Value gen_VkPipelineCacheCreateInfo_contents(const VkPipelineCacheCreateInfo& s, const LocationScope& l) {
        Json::Value json;

        json["flags"] = gen_VkPipelineCacheCreateFlags(s.flags, CreateScope("flags"));
        json["initialDataSize"] = gen_size_t(s.initialDataSize, CreateScope("initialDataSize"));

        if (s.initialDataSize != 0) {
            if (s.pInitialData != nullptr) {
                json["pInitialData"] = gen_binary(s.pInitialData, s.initialDataSize);
            } else {
                Error() << "pInitialData is NULL but its length is " << s.initialDataSize;
            }
        } else {
            json["pInitialData"] = "NULL";
        }

        return json;
    }

    Json::Value gen_VkPipelinePoolSize_contents(const VkPipelinePoolSize& s, const LocationScope& l) {
        Json::Value json;

        json["poolEntrySize"] = gen_VkDeviceSize(s.poolEntrySize, CreateScope("poolEntrySize"));
        json["poolEntryCount"] = gen_uint32_t(s.poolEntryCount, CreateScope("poolEntryCount"));

        return json;
    }

    Json::Value gen_VkDeviceObjectReservationCreateInfo_contents(const VkDeviceObjectReservationCreateInfo& s,
                                                                 const LocationScope& l) {
        Json::Value json;

        json["pipelineCacheCreateInfoCount"] =
            gen_uint32_t(s.pipelineCacheCreateInfoCount, CreateScope("pipelineCacheCreateInfoCount"));

        if (s.pipelineCacheCreateInfoCount != 0) {
            Json::Value json_array_pPipelineCacheCreateInfos;
            if (s.pPipelineCacheCreateInfos != nullptr) {
                for (Json::Value::ArrayIndex i = 0; i < s.pipelineCacheCreateInfoCount; i++) {
                    json_array_pPipelineCacheCreateInfos[i] =
                        gen_VkPipelineCacheCreateInfo(s.pPipelineCacheCreateInfos[i], CreateScope("pPipelineCacheCreateInfos", i));
                }
                json["pPipelineCacheCreateInfos"] = json_array_pPipelineCacheCreateInfos;
            } else {
                Error() << "pPipelineCacheCreateInfos is NULL but its length is " << s.pipelineCacheCreateInfoCount;
            }
        } else {
            json["pPipelineCacheCreateInfos"] = "NULL";
        }
        json["pipelinePoolSizeCount"] = gen_uint32_t(s.pipelinePoolSizeCount, CreateScope("pipelinePoolSizeCount"));

        if (s.pipelinePoolSizeCount != 0) {
            Json::Value json_array_pPipelinePoolSizes;
            if (s.pPipelinePoolSizes != nullptr) {
                for (Json::Value::ArrayIndex i = 0; i < s.pipelinePoolSizeCount; i++) {
                    json_array_pPipelinePoolSizes[i] =
                        gen_VkPipelinePoolSize(s.pPipelinePoolSizes[i], CreateScope("pPipelinePoolSizes", i));
                }
                json["pPipelinePoolSizes"] = json_array_pPipelinePoolSizes;
            } else {
                Error() << "pPipelinePoolSizes is NULL but its length is " << s.pipelinePoolSizeCount;
            }
        } else {
            json["pPipelinePoolSizes"] = "NULL";
        }
        json["semaphoreRequestCount"] = gen_uint32_t(s.semaphoreRequestCount, CreateScope("semaphoreRequestCount"));
        json["commandBufferRequestCount"] = gen_uint32_t(s.commandBufferRequestCount, CreateScope("commandBufferRequestCount"));
        json["fenceRequestCount"] = gen_uint32_t(s.fenceRequestCount, CreateScope("fenceRequestCount"));
        json["deviceMemoryRequestCount"] = gen_uint32_t(s.deviceMemoryRequestCount, CreateScope("deviceMemoryRequestCount"));
        json["bufferRequestCount"] = gen_uint32_t(s.bufferRequestCount, CreateScope("bufferRequestCount"));
        json["imageRequestCount"] = gen_uint32_t(s.imageRequestCount, CreateScope("imageRequestCount"));
        json["eventRequestCount"] = gen_uint32_t(s.eventRequestCount, CreateScope("eventRequestCount"));
        json["queryPoolRequestCount"] = gen_uint32_t(s.queryPoolRequestCount, CreateScope("queryPoolRequestCount"));
        json["bufferViewRequestCount"] = gen_uint32_t(s.bufferViewRequestCount, CreateScope("bufferViewRequestCount"));
        json["imageViewRequestCount"] = gen_uint32_t(s.imageViewRequestCount, CreateScope("imageViewRequestCount"));
        json["layeredImageViewRequestCount"] =
            gen_uint32_t(s.layeredImageViewRequestCount, CreateScope("layeredImageViewRequestCount"));
        json["pipelineCacheRequestCount"] = gen_uint32_t(s.pipelineCacheRequestCount, CreateScope("pipelineCacheRequestCount"));
        json["pipelineLayoutRequestCount"] = gen_uint32_t(s.pipelineLayoutRequestCount, CreateScope("pipelineLayoutRequestCount"));
        json["renderPassRequestCount"] = gen_uint32_t(s.renderPassRequestCount, CreateScope("renderPassRequestCount"));
        json["graphicsPipelineRequestCount"] =
            gen_uint32_t(s.graphicsPipelineRequestCount, CreateScope("graphicsPipelineRequestCount"));
        json["computePipelineRequestCount"] =
            gen_uint32_t(s.computePipelineRequestCount, CreateScope("computePipelineRequestCount"));
        json["descriptorSetLayoutRequestCount"] =
            gen_uint32_t(s.descriptorSetLayoutRequestCount, CreateScope("descriptorSetLayoutRequestCount"));
        json["samplerRequestCount"] = gen_uint32_t(s.samplerRequestCount, CreateScope("samplerRequestCount"));
        json["descriptorPoolRequestCount"] = gen_uint32_t(s.descriptorPoolRequestCount, CreateScope("descriptorPoolRequestCount"));
        json["descriptorSetRequestCount"] = gen_uint32_t(s.descriptorSetRequestCount, CreateScope("descriptorSetRequestCount"));
        json["framebufferRequestCount"] = gen_uint32_t(s.framebufferRequestCount, CreateScope("framebufferRequestCount"));
        json["commandPoolRequestCount"] = gen_uint32_t(s.commandPoolRequestCount, CreateScope("commandPoolRequestCount"));
        json["samplerYcbcrConversionRequestCount"] =
            gen_uint32_t(s.samplerYcbcrConversionRequestCount, CreateScope("samplerYcbcrConversionRequestCount"));
        json["surfaceRequestCount"] = gen_uint32_t(s.surfaceRequestCount, CreateScope("surfaceRequestCount"));
        json["swapchainRequestCount"] = gen_uint32_t(s.swapchainRequestCount, CreateScope("swapchainRequestCount"));
        json["displayModeRequestCount"] = gen_uint32_t(s.displayModeRequestCount, CreateScope("displayModeRequestCount"));
        json["subpassDescriptionRequestCount"] =
            gen_uint32_t(s.subpassDescriptionRequestCount, CreateScope("subpassDescriptionRequestCount"));
        json["attachmentDescriptionRequestCount"] =
            gen_uint32_t(s.attachmentDescriptionRequestCount, CreateScope("attachmentDescriptionRequestCount"));
        json["descriptorSetLayoutBindingRequestCount"] =
            gen_uint32_t(s.descriptorSetLayoutBindingRequestCount, CreateScope("descriptorSetLayoutBindingRequestCount"));
        json["descriptorSetLayoutBindingLimit"] =
            gen_uint32_t(s.descriptorSetLayoutBindingLimit, CreateScope("descriptorSetLayoutBindingLimit"));
        json["maxImageViewMipLevels"] = gen_uint32_t(s.maxImageViewMipLevels, CreateScope("maxImageViewMipLevels"));
        json["maxImageViewArrayLayers"] = gen_uint32_t(s.maxImageViewArrayLayers, CreateScope("maxImageViewArrayLayers"));
        json["maxLayeredImageViewMipLevels"] =
            gen_uint32_t(s.maxLayeredImageViewMipLevels, CreateScope("maxLayeredImageViewMipLevels"));
        json["maxOcclusionQueriesPerPool"] = gen_uint32_t(s.maxOcclusionQueriesPerPool, CreateScope("maxOcclusionQueriesPerPool"));
        json["maxPipelineStatisticsQueriesPerPool"] =
            gen_uint32_t(s.maxPipelineStatisticsQueriesPerPool, CreateScope("maxPipelineStatisticsQueriesPerPool"));
        json["maxTimestampQueriesPerPool"] = gen_uint32_t(s.maxTimestampQueriesPerPool, CreateScope("maxTimestampQueriesPerPool"));
        json["maxImmutableSamplersPerDescriptorSetLayout"] =
            gen_uint32_t(s.maxImmutableSamplersPerDescriptorSetLayout, CreateScope("maxImmutableSamplersPerDescriptorSetLayout"));

        return json;
    }
};

}  // namespace pcjson
// NOLINTEND
