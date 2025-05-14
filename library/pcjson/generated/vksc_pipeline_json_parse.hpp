/*
 * Copyright (c) 2025 The Khronos Group Inc.
 * Copyright (c) 2025 RasterGrid Kft.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// TODO: Generated pipeline JSON parser utils

#pragma once

#include <json/json.h>
#include <vulkan/vulkan.h>

#include <string>
#include <sstream>
#include <string_view>
#include <unordered_map>
#include <algorithm>

#include "vksc_pipeline_json_base.hpp"

namespace pcjson {

class ParserBase : protected Base {
  protected:
    // Hard-coded parsers for "built-in" types
    int8_t parse_int8_t(const Json::Value& v, const LocationScope&) {
        if (v.isInt() && v.asInt() >= INT8_MIN && v.asInt() <= INT8_MAX) {
            return v.asInt();
        } else {
            Error() << "Not an 8-bit signed integer";
            return 0;
        }
    }

    int16_t parse_int16_t(const Json::Value& v, const LocationScope&) {
        if (v.isInt() && v.asInt() >= INT16_MIN && v.asInt() <= INT16_MAX) {
            return v.asInt();
        } else {
            Error() << "Not a 16-bit signed integer";
            return 0;
        }
    }

    int32_t parse_int32_t(const Json::Value& v, const LocationScope&) {
        if (v.isInt() && v.asInt() >= INT32_MIN && v.asInt() <= INT32_MAX) {
            return v.asInt();
        } else {
            Error() << "Not a 32-bit signed integer";
            return 0;
        }
    }

    int64_t parse_int64_t(const Json::Value& v, const LocationScope&) {
        if (v.isInt64()) {
            return v.asInt64();
        } else {
            Error() << "Not a 64-bit signed integer";
            return 0;
        }
    }

    uint8_t parse_uint8_t(const Json::Value& v, const LocationScope&) {
        if (v.isUInt() && v.asUInt() <= UINT8_MAX) {
            return v.asUInt();
        } else {
            Error() << "Not an 8-bit unsigned integer";
            return 0;
        }
    }

    uint16_t parse_uint16_t(const Json::Value& v, const LocationScope&) {
        if (v.isUInt() && v.asUInt() <= UINT16_MAX) {
            return v.asUInt();
        } else {
            Error() << "Not a 16-bit unsigned integer";
            return 0;
        }
    }

    uint32_t parse_uint32_t(const Json::Value& v, const LocationScope&) {
        if (v.isUInt() && v.asUInt() <= UINT32_MAX) {
            return v.asUInt();
        } else {
            Error() << "Not a 32-bit unsigned integer";
            return 0;
        }
    }

    uint64_t parse_uint64_t(const Json::Value& v, const LocationScope&) {
        if (v.isUInt64()) {
            return v.asUInt64();
        } else {
            Error() << "Not a 64-bit unsigned integer";
            return 0;
        }
    }

    VkDeviceSize parse_VkDeviceSize(const Json::Value& v, const LocationScope& l) {
        return parse_uint64_t(v, l);
    }

    const char* parse_string(const Json::Value& v, const LocationScope&) {
        if (v.isString()) {
            const char *src = v.asCString();
            size_t len = strlen(src);
            char* dst = AllocMem<char>(len + 1);
            strcpy(dst, src);
            return dst;
        } else {
            Error() << "Not a string";
            return nullptr;
        }
    }

    // ...

    // VkStructureType is also a special case
    VkStructureType parse_VkStructureType(const Json::Value& json, const LocationScope&) {
        std::unordered_map<std::string_view, VkStructureType> map = {
            std::make_pair("VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO", VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO),
            // ... other constants
        };
        if (json.isString()) {
            auto it = map.find(json.asCString());
            if (it != map.end()) {
                return it->second;
            } else {
                Error() << "Invalid VkStructureType value: " << json.asCString();
            }
        } else {
            Error() << "Invalid format";
        }
        return static_cast<VkStructureType>(0);
    }

  private:
    VkShaderStageFlagBits parse_VkShaderStageFlagBits_c_str(const char* v) {
        // Sample code to generate for flag bits or other enum types
        std::unordered_map<std::string_view, VkShaderStageFlagBits> map = {
            std::make_pair("VK_SHADER_STAGE_VERTEX_BIT", VK_SHADER_STAGE_VERTEX_BIT),
            // ... other bit/enum constants
        };
        auto it = map.find(v);
        if (it != map.end()) {
            return it->second;
        } else {
            Error() << "Invalid VkStructureType value: " << v;
            return static_cast<VkShaderStageFlagBits>(0);
        }
    }

  protected:
    VkShaderStageFlagBits parse_VkShaderStageFlagBits(const Json::Value& json, const LocationScope&) {
        // Sample code to generate for flag bits or other enum types
        if (json.isString()) {
            return parse_VkShaderStageFlagBits_c_str(json.asCString());
        } else {
            Error() << "Invalid format";
            return static_cast<VkShaderStageFlagBits>(0);
        }
    }

    VkShaderStageFlags parse_VkShaderStageFlags(const Json::Value& json, const LocationScope& l) {
        VkShaderStageFlags result = 0;
        if (json.isString()) {
            std::stringstream strm(json.asString());
            std::string str;
            while (std::getline(strm, str, '|')) {
                str.erase(std::remove_if(str.begin(), str.end(), isspace), str.end());
                result |= parse_VkShaderStageFlagBits_c_str(str.c_str());
            }
        } else {
            Error() << "Invalid format";
        }
        return result;
    }

    VkGraphicsPipelineCreateInfo parse_VkGraphicsPipelineCreateInfo(const Json::Value& json, const LocationScope& l) {
        // Sample code to generate for structure chain parsing
        VkGraphicsPipelineCreateInfo s = parse_VkGraphicsPipelineCreateInfo_contents(json, l);

        const auto& json_stype = json["sType"];
        if (json_stype.isString()) {
            if (json_stype.asString() != "VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO") {
                Error() << "Invalid sType value: " << json_stype.asCString();
            }
        } else {
            Error() << "Invalid sType format";
        }

        [[maybe_unused]] auto prev = reinterpret_cast<VkBaseOutStructure*>(&s);
        const Json::Value* json_next = &json["pNext"];
        const char* current_pnext_ref = "pNext";
        while (json_next->isObject()) {
            auto next_stype = parse_VkStructureType((*json_next)["sType"], CreateScope(current_pnext_ref));
            switch (next_stype) {
                case VK_STRUCTURE_TYPE_PIPELINE_OFFLINE_CREATE_INFO: {
                    auto next = AllocMem<VkPipelineOfflineCreateInfo>();
                    current_pnext_ref = "pNext<VkPipelineOfflineCreateInfo>";
                    *next = parse_VkPipelineOfflineCreateInfo_contents(*json_next, CreateScope(current_pnext_ref, true));
                    prev->pNext = reinterpret_cast<VkBaseOutStructure*>(next);
                    break;
                }
                // ... other structure types that can extend this structure
                default:
                    Error() << "Invalid structure type extending VkGraphicsPipelineCreateInfo: " << (*json_next)["sType"].asCString();
                    break;
            }
            json_next = &(*json_next)["pNext"];
            if (prev->pNext != nullptr) prev = prev->pNext;
        }

        if (!json_next->isString() || json_next->asString() != "NULL") {
            Error() << "Invalid pNext format";
        }

        return s;
    }

    // Note: these are the main struct chains we care about (+ the VkGraphicsPipelineCreateInfo above)
    VkComputePipelineCreateInfo parse_VkComputePipelineCreateInfo(const Json::Value& json, const LocationScope& l) { return {}; }
    VkSamplerYcbcrConversionCreateInfo parse_VkSamplerYcbcrConversionCreateInfo(const Json::Value& json, const LocationScope& l) { return {}; }
    VkSamplerCreateInfo parse_VkSamplerCreateInfo(const Json::Value& json, const LocationScope& l) { return {}; }
    VkDescriptorSetLayoutCreateInfo parse_VkDescriptorSetLayoutCreateInfo(const Json::Value& json, const LocationScope& l) { return {}; }
    VkPipelineLayoutCreateInfo parse_VkPipelineLayoutCreateInfo(const Json::Value& json, const LocationScope& l) { return {}; }
    VkPhysicalDeviceFeatures2 parse_VkPhysicalDeviceFeatures2(const Json::Value& json, const LocationScope& l) { return {}; }
    VkRenderPassCreateInfo parse_VkRenderPassCreateInfo(const Json::Value& json, const LocationScope& l) { return {}; }
    VkRenderPassCreateInfo2 parse_VkRenderPassCreateInfo2(const Json::Value& json, const LocationScope& l) { return {}; }

    // Note: there are 3 more special cases used by the CTS, so we include these too
    // Void pointer data such as VkShaderModuleCreateInfo::pData is encoded as base64
    // Also note that other struct chains (e.g. VkPipelineShaderStageCreateInfo) that are included in other struct chains
    // need to be handled too
    VkShaderModuleCreateInfo parse_VkShaderModuleCreateInfo(const Json::Value& json, const LocationScope& l) { return {}; }
    VkDeviceObjectReservationCreateInfo parse_VkDeviceObjectReservationCreateInfo(const Json::Value& json, const LocationScope& l) { return {}; }
    VkPipelineOfflineCreateInfo parse_VkPipelineOfflineCreateInfo(const Json::Value& json, const LocationScope& l) { return {}; }

  private:
    VkGraphicsPipelineCreateInfo parse_VkGraphicsPipelineCreateInfo_contents(const Json::Value& json, const LocationScope&) {
        // Sample code to generate for structures (we initialize sType and pNext here if it's extensible)
        VkGraphicsPipelineCreateInfo s{ VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO, nullptr };

        //s.flags = parse_VkPipelineCreateFlags(json["flags"], CreateScope("flags"));
        s.stageCount = parse_uint32_t(json["stageCount"], CreateScope("stageCount"));
        // ...

        return s;
    }

    VkPipelineOfflineCreateInfo parse_VkPipelineOfflineCreateInfo_contents(const Json::Value& json, const LocationScope&) { return {}; }
};

}  // namespace pcjson
