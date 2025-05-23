/*
 * Copyright (c) 2025 The Khronos Group Inc.
 * Copyright (c) 2025 RasterGrid Kft.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// TODO: Generated pipeline JSON generator utils

#pragma once

#include <json/json.h>
#include <vulkan/vulkan.h>

#include <string>
#include <sstream>

#include "vksc_pipeline_json_base.hpp"

namespace pcjson {

class GeneratorBase : protected Base {
  private:
    const char* gen_VkShaderStageFlagBits_c_str(const VkShaderStageFlagBits v) {
        // Sample code to generate for flag bits or other enum types
        switch (v) {
            case VK_SHADER_STAGE_VERTEX_BIT:
                return "VK_SHADER_STAGE_VERTEX_BIT";
            // ... other bit/enum constants
            default:
                Error() << "Invalid VkShaderStageFlagBits value: " << v;
                return NULL;
        }
    }

  protected:
    Json::Value gen_VkShaderStageFlagBits(const VkShaderStageFlagBits v, const LocationScope&) {
        // Sample code to generate for flag bits or other enum types
        return gen_VkShaderStageFlagBits_c_str(v);
    }

    Json::Value gen_VkShaderStageFlags(const VkShaderStageFlags v, const LocationScope&) {
        // Sample code to generate for flags types (for 64-bit flags, obviously, the loop condition changes)
        std::stringstream strm;
        for (uint32_t bit = 0; bit < (1u << 31); bit <<= 1) {
            if ((v & bit) != 0) {
                if (strm.rdbuf()->in_avail() > 0) {
                    strm << " | ";
                }
                strm << gen_VkShaderStageFlagBits_c_str(static_cast<VkShaderStageFlagBits>(bit));
            }
        }
        return strm.str().c_str();
    }

    Json::Value gen_VkGraphicsPipelineCreateInfo(const VkGraphicsPipelineCreateInfo& s, const LocationScope& l) {
        // Sample code to generate for structure chain generation
        Json::Value json = gen_VkGraphicsPipelineCreateInfo_contents(s, l);

        // Can hard-code this, structure type is already expected to be verified at the caller side
        json["sType"] = "VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO";

        auto next = reinterpret_cast<const VkBaseInStructure*>(s.pNext);
        auto& json_next = json["pNext"];

        while (next != nullptr) {
            switch (next->sType) {
                case VK_STRUCTURE_TYPE_PIPELINE_OFFLINE_CREATE_INFO: {
                    json_next = gen_VkPipelineOfflineCreateInfo_contents(*reinterpret_cast<const VkPipelineOfflineCreateInfo*>(next),
                                                                         CreateScope("pNext<VkPipelineOfflineCreateInfo>", true));
                    json_next["sType"] = "VK_STRUCTURE_TYPE_PIPELINE_OFFLINE_CREATE_INFO";
                    break;
                }
                // ... other structure types that can extend this structure
                default:
                    Error() << "Invalid structure type extending VkGraphicsPipelineCreateInfo: " << next->sType;
                    break;
            }
            next = next->pNext;
            if (!json_next.isNull()) json_next = json_next["pNext"];
        }

        // "Close off" the chain
        json_next = "NULL";

        return json;
    }

    // Note: these are the main struct chains we care about (+ the VkGraphicsPipelineCreateInfo above)
    Json::Value gen_VkComputePipelineCreateInfo(const VkComputePipelineCreateInfo& s, const LocationScope& l) { return {}; }
    Json::Value gen_VkSamplerYcbcrConversionCreateInfo(const VkSamplerYcbcrConversionCreateInfo& s, const LocationScope& l) { return {}; }
    Json::Value gen_VkSamplerCreateInfo(const VkSamplerCreateInfo& s, const LocationScope& l) { return {}; }
    Json::Value gen_VkDescriptorSetLayoutCreateInfo(const VkDescriptorSetLayoutCreateInfo& s, const LocationScope& l) { return {}; }
    Json::Value gen_VkPipelineLayoutCreateInfo(const VkPipelineLayoutCreateInfo& s, const LocationScope& l) { return {}; }
    Json::Value gen_VkPhysicalDeviceFeatures2(const VkPhysicalDeviceFeatures2& s, const LocationScope& l) { return {}; }
    Json::Value gen_VkRenderPassCreateInfo(const VkRenderPassCreateInfo s, const LocationScope& l) { return {}; }
    Json::Value gen_VkRenderPassCreateInfo2(const VkRenderPassCreateInfo2 s, const LocationScope& l) { return {}; }

    // Note: there are 3 more special cases used by the CTS, so we include these too
    // Void pointer data such as VkShaderModuleCreateInfo::pData is encoded as base64
    // Also note that other struct chains (e.g. VkPipelineShaderStageCreateInfo) that are included in other struct chains
    // need to be handled too
    Json::Value gen_VkShaderModuleCreateInfo(const VkShaderModuleCreateInfo& s, const LocationScope& l) { return {}; }
    Json::Value gen_VkDeviceObjectReservationCreateInfo(const VkDeviceObjectReservationCreateInfo& s, const LocationScope& l) { return {}; }
    Json::Value gen_VkPipelineOfflineCreateInfo(const VkPipelineOfflineCreateInfo& s, const LocationScope& l) { return {}; }

  private:
    Json::Value gen_VkGraphicsPipelineCreateInfo_contents(const VkGraphicsPipelineCreateInfo& s, const LocationScope&) {
        // Sample code to generate for structures (we intentionally don't care about the sType and pNext
        // members of extensible structures, that is to be set by the caller that creates the chain)
        Json::Value json = Json::objectValue;

        //json["flags"] = gen_VkPipelineCreateFlags(s.flags, CreateScope("flags"));
        json["stageCount"] = s.stageCount;
        // ...

        return json;
    }

    Json::Value gen_VkPipelineOfflineCreateInfo_contents(const VkPipelineOfflineCreateInfo& s, const LocationScope&) { return {}; }
    // ...
};

}  // namespace pcjson
