/*
 * Copyright (c) 2025 The Khronos Group Inc.
 * Copyright (c) 2025 RasterGrid Kft.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "vksc_pipeline_json_parse.hpp"
#include "vksc_pipeline_json.h"

#include <unordered_map>
#include <string_view>
#include <string.h>

namespace pcjson {

class Parser : private ParserBase {
  public:
    bool ParsePipelineJson(const char* pPipelineJson, VpjData* pPipelineData, const char** ppMessages) {
        ClearStatusAndMessages();

        if (pPipelineJson == nullptr) {
            Error() << "pPipelineJson is NULL";
        }

        if (pPipelineData == nullptr) {
            Error() << "pPipelineData is NULL";
        }

        Json::Value json{};
        if (IsStatusOK()) {
            Json::CharReaderBuilder builder{};
            std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
            Json::String json_errors{};
            if (!reader->parse(pPipelineJson, pPipelineJson + strlen(pPipelineJson), &json, &json_errors)) {
                Error() << "Failed to parse pipeline JSON:" << std::endl << json_errors;
            }
        }

        if (IsStatusOK()) {
            parse_PipelineUUID(json);
            parse_EnabledExtensions(json);

            if (json.isMember("GraphicsPipelineState")) {
                parse_GraphicsPipelineState(json["GraphicsPipelineState"]);
            } else if (json.isMember("ComputePipelineState")) {
                parse_ComputePipelineState(json["ComputePipelineState"]);
            } else {
                Error() << "Unknown pipeline type (no GraphicsPipelineState or ComputePipelineState is found)";
            }
        }

        if (IsStatusOK()) {
            *pPipelineData = data_;
        }

        if (ppMessages != nullptr) {
            *ppMessages = GetMessages();
        }

        return IsStatusOK();
    }

    bool ParseSingleStructJson(const char* pJson, void* pStruct, const char** ppMessages) {
        ClearStatusAndMessages();

        if (pJson == nullptr) {
            Error() << "pJson is NULL";
        }

        if (pStruct == nullptr) {
            Error() << "pStruct is NULL";
        }

        Json::Value json{};
        if (IsStatusOK()) {
            Json::CharReaderBuilder builder{};
            std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
            Json::String json_errors{};
            if (!reader->parse(pJson, pJson + strlen(pJson), &json, &json_errors)) {
                Error() << "Failed to parse JSON:" << std::endl << json_errors;
            }
        }

        VkStructureType stype{};
        if (IsStatusOK()) {
            if (json.isMember("sType")) {
                stype = parse_VkStructureType(json["sType"], CreateScope("$.sType"));
            } else {
                Error() << "Missing sType from JSON";
            }
        }

        if (IsStatusOK()) {
            switch (stype) {
                case VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO:
                    *reinterpret_cast<VkGraphicsPipelineCreateInfo*>(pStruct) =
                        parse_VkGraphicsPipelineCreateInfo(json, CreateScope("$"));
                    break;
                case VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO:
                    *reinterpret_cast<VkComputePipelineCreateInfo*>(pStruct) =
                        parse_VkComputePipelineCreateInfo(json, CreateScope("$"));
                    break;
                case VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_CREATE_INFO:
                    *reinterpret_cast<VkSamplerYcbcrConversionCreateInfo*>(pStruct) =
                        parse_VkSamplerYcbcrConversionCreateInfo(json, CreateScope("$"));
                    break;
                case VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO:
                    *reinterpret_cast<VkSamplerCreateInfo*>(pStruct) = parse_VkSamplerCreateInfo(json, CreateScope("$"));
                    break;
                case VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO:
                    *reinterpret_cast<VkDescriptorSetLayoutCreateInfo*>(pStruct) =
                        parse_VkDescriptorSetLayoutCreateInfo(json, CreateScope("$"));
                    break;
                case VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO:
                    *reinterpret_cast<VkPipelineLayoutCreateInfo*>(pStruct) =
                        parse_VkPipelineLayoutCreateInfo(json, CreateScope("$"));
                    break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2:
                    *reinterpret_cast<VkPhysicalDeviceFeatures2*>(pStruct) =
                        parse_VkPhysicalDeviceFeatures2(json, CreateScope("$"));
                    break;
                case VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO:
                    *reinterpret_cast<VkRenderPassCreateInfo*>(pStruct) = parse_VkRenderPassCreateInfo(json, CreateScope("$"));
                    break;
                case VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2:
                    *reinterpret_cast<VkRenderPassCreateInfo2*>(pStruct) = parse_VkRenderPassCreateInfo2(json, CreateScope("$"));
                    break;
                case VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO:
                    *reinterpret_cast<VkShaderModuleCreateInfo*>(pStruct) = parse_VkShaderModuleCreateInfo(json, CreateScope("$"));
                    break;
                case VK_STRUCTURE_TYPE_DEVICE_OBJECT_RESERVATION_CREATE_INFO:
                    *reinterpret_cast<VkDeviceObjectReservationCreateInfo*>(pStruct) =
                        parse_VkDeviceObjectReservationCreateInfo(json, CreateScope("$"));
                    break;
                case VK_STRUCTURE_TYPE_PIPELINE_OFFLINE_CREATE_INFO:
                    *reinterpret_cast<VkPipelineOfflineCreateInfo*>(pStruct) =
                        parse_VkPipelineOfflineCreateInfo(json, CreateScope("$"));
                    break;
                default:
                    Error() << "Unsupported structure type: " << stype;
                    break;
            }
        }

        if (ppMessages != nullptr) {
            *ppMessages = GetMessages();
        }

        return IsStatusOK();
    }

    void FreeOutputs() { FreeAllMem(); }

    VpjParser Handle() { return reinterpret_cast<VpjParser>(this); }

    static Parser* FromHandle(VpjParser handle) { return reinterpret_cast<Parser*>(handle); }

  private:
    template <typename T>
    void parse_CommonPipelineState(Json::Value& json, T& state) {
        if (json.isMember("YcbcrSamplers")) {
            const auto& json_ycbcr_samplers = json["YcbcrSamplers"];
            if (json_ycbcr_samplers.isArray()) {
                state.ycbcrSamplerCount = json_ycbcr_samplers.size();
                auto ycbcr_samplers = AllocMem<VkSamplerYcbcrConversionCreateInfo>(state.ycbcrSamplerCount);
                state.pYcbcrSamplers = ycbcr_samplers;
                for (Json::Value::ArrayIndex i = 0; i < json_ycbcr_samplers.size(); ++i) {
                    auto sampler_iter = json_ycbcr_samplers[i].begin();
                    ycbcr_samplers[i] = parse_VkSamplerYcbcrConversionCreateInfo(*sampler_iter, CreateScope("YcbcrSamplers", i));
                }
            } else {
                Error() << "Invalid YcbcrSamplers format";
            }
        }

        if (json.isMember("ImmutableSamplers")) {
            auto& json_immutable_samplers = json["ImmutableSamplers"];
            if (json_immutable_samplers.isArray()) {
                state.immutableSamplerCount = json_immutable_samplers.size();
                auto immutable_samplers = AllocMem<VkSamplerCreateInfo>(state.immutableSamplerCount);
                state.pImmutableSamplers = immutable_samplers;
                for (Json::Value::ArrayIndex i = 0; i < json_immutable_samplers.size(); ++i) {
                    auto sampler_iter = json_immutable_samplers[i].begin();
                    immutable_samplers[i] = parse_VkSamplerCreateInfo(*sampler_iter, CreateScope("ImmutableSamplers", i));
                }
            } else {
                Error() << "Invalid ImmutableSamplers format";
            }
        }

        if (json.isMember("DescriptorSetLayouts")) {
            auto& json_ds_layouts = json["DescriptorSetLayouts"];
            if (json_ds_layouts.isArray()) {
                state.descriptorSetLayoutCount = json_ds_layouts.size();
                auto ds_layouts = AllocMem<VkDescriptorSetLayoutCreateInfo>(state.descriptorSetLayoutCount);
                state.pDescriptorSetLayouts = ds_layouts;
                for (Json::Value::ArrayIndex i = 0; i < json_ds_layouts.size(); ++i) {
                    auto ds_layout_iter = json_ds_layouts[i].begin();
                    ds_layouts[i] = parse_VkDescriptorSetLayoutCreateInfo(*ds_layout_iter, CreateScope("DescriptorSetLayouts", i));
                }
            } else {
                Error() << "Invalid DescriptorSetLayouts format";
            }
        }

        if (json.isMember("PipelineLayout")) {
            auto pipeline_layout = AllocMem<VkPipelineLayoutCreateInfo>();
            state.pPipelineLayout = pipeline_layout;
            *pipeline_layout = parse_VkPipelineLayoutCreateInfo(json["PipelineLayout"], CreateScope("PipelineLayout"));
        } else {
            Error() << "Missing PipelineLayout";
        }

        if (json.isMember("ShaderFileNames")) {
            const auto& json_shaders = json["ShaderFileNames"];
            if (json_shaders.isArray()) {
                state.shaderFileNameCount = json_shaders.size();
                auto shaders = AllocMem<VpjShaderFileName>(state.shaderFileNameCount);
                state.pShaderFileNames = shaders;
                for (Json::Value::ArrayIndex i = 0; i < json_shaders.size(); ++i) {
                    auto shader_loc = CreateScope("ShaderFileNames", i);
                    if (json_shaders[i].isObject()) {
                        shaders[i].stage = parse_VkShaderStageFlagBits(json_shaders[i]["stage"], CreateScope("stage"));
                        shaders[i].pFilename = parse_string(json_shaders[i]["filename"], CreateScope("filename"));
                    } else {
                        Error() << "Invalid format";
                    }
                }
            } else {
                Error() << "Invalid ShaderFileNames format";
            }
        } else {
            Error() << "Missing ShaderFileNames";
        }

        if (json.isMember("PhysicalDeviceFeatures")) {
            auto feature = AllocMem<VkPhysicalDeviceFeatures2>();
            state.pPhysicalDeviceFeatures = feature;
            *feature = parse_VkPhysicalDeviceFeatures2(json["PhysicalDeviceFeatures"], CreateScope("PhysicalDeviceFeatures"));
        }
    }

    template <typename T>
    void ResolveObjectNames(Json::Value& json, T& state) {
        std::unordered_map<std::string_view, uint32_t> ycbcr_sampler_indices{};
        std::unordered_map<std::string_view, uint32_t> immutable_sampler_indices{};
        std::unordered_map<std::string_view, uint32_t> ds_layout_indices{};

        auto alloc_and_copy_name = [&](const std::string_view& src) {
            auto dst = AllocMem<char>(src.length() + 1);  // null-terminator
            std::copy(src.begin(), src.end(), dst);
            dst[src.length()] = '\0';
            return dst;
        };

        if (json.isMember("YcbcrSamplers")) {
            const auto& json_ycbcr_samplers = json["YcbcrSamplers"];
            if (json_ycbcr_samplers.isArray()) {
                state.ppYcbcrSamplerNames = AllocMem<const char*>(json_ycbcr_samplers.size());
                for (Json::Value::ArrayIndex i = 0; i < json_ycbcr_samplers.size(); ++i) {
                    std::string_view ycbcr_sampler_name = json_ycbcr_samplers[i].begin().key().asCString();
                    state.ppYcbcrSamplerNames[i] = alloc_and_copy_name(ycbcr_sampler_name);  // Remember YCbCr sampler name
                    ycbcr_sampler_indices[state.ppYcbcrSamplerNames[i]] = i;                 // Remember which index the name is at
                }
            } else {
                Error() << "Invalid YcbcrSamplers format";
            }
        }

        if (json.isMember("ImmutableSamplers")) {
            auto& json_immutable_samplers = json["ImmutableSamplers"];
            if (json_immutable_samplers.isArray()) {
                state.ppImmutableSamplerNames = AllocMem<const char*>(json_immutable_samplers.size());
                for (Json::Value::ArrayIndex i = 0; i < json_immutable_samplers.size(); ++i) {
                    auto sampler_iter = json_immutable_samplers[i].begin();
                    std::string_view sampler_name = sampler_iter.key().asCString();
                    state.ppImmutableSamplerNames[i] = alloc_and_copy_name(sampler_name);  // Remember sampler name
                    immutable_sampler_indices[state.ppImmutableSamplerNames[i]] = i;       // Remember which index the name is at
                    if (auto ycbcr_info_ptr =
                            find_StructInChain(&(*sampler_iter)["pNext"], "VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_INFO");
                        ycbcr_info_ptr != nullptr) {
                        auto& ycbcr_conversion = (*ycbcr_info_ptr)["conversion"];
                        // Rewrite name to index
                        auto it = ycbcr_sampler_indices.find(ycbcr_conversion.asCString());
                        if (it != ycbcr_sampler_indices.end()) {
                            ycbcr_conversion = it->second;
                        } else {
                            Error() << "Invalid YcbcrSamplers name \"" << ycbcr_conversion.asCString() << '\"';
                        }
                    }
                }
            } else {
                Error() << "Invalid ImmutableSamplers format";
            }
        }

        if (json.isMember("DescriptorSetLayouts")) {
            auto& json_ds_layouts = json["DescriptorSetLayouts"];
            if (json_ds_layouts.isArray()) {
                state.ppDescriptorSetLayoutNames = AllocMem<const char*>(json_ds_layouts.size());
                for (Json::Value::ArrayIndex i = 0; i < json_ds_layouts.size(); ++i) {
                    auto ds_layout_iter = json_ds_layouts[i].begin();
                    std::string_view ds_layout_name = ds_layout_iter.key().asCString();
                    state.ppDescriptorSetLayoutNames[i] =
                        alloc_and_copy_name(ds_layout_name);                     // Remember descriptor set layout name
                    ds_layout_indices[state.ppDescriptorSetLayoutNames[i]] = i;  // Remember which index the name is at
                    if (auto& bindings = (*ds_layout_iter)["pBindings"]; bindings.isArray()) {
                        for (Json::Value::ArrayIndex j = 0; j < bindings.size(); ++j) {
                            if (auto& immutable_samplers = bindings[j]["pImmutableSamplers"]; immutable_samplers.isArray()) {
                                for (Json::Value::ArrayIndex k = 0; k < immutable_samplers.size(); ++k) {
                                    // Rewrite name to index
                                    auto it = immutable_sampler_indices.find(immutable_samplers[k].asCString());
                                    if (it != immutable_sampler_indices.end()) {
                                        immutable_samplers[k] = it->second;
                                    } else {
                                        Error() << "Invalid ImmutableSamplers name \"" << immutable_samplers.asCString() << '\"';
                                    }
                                }
                            }
                        }
                    }
                }
            } else {
                Error() << "Invalid DescriptorSetLayouts format";
            }
        }

        if (json.isMember("PipelineLayout")) {
            auto& ds_layouts = json["PipelineLayout"]["pSetLayouts"];
            for (Json::Value::ArrayIndex i = 0; i < ds_layouts.size(); ++i) {
                // Rewrite name to index
                auto it = ds_layout_indices.find(ds_layouts[i].asCString());
                if (it != ds_layout_indices.end()) {
                    ds_layouts[i] = it->second;
                } else {
                    Error() << "Invalid DescriptorSetLayouts name \"" << ds_layouts[i].asCString() << '\"';
                }
            }
        } else {
            Error() << "Missing PipelineLayout";
        }

        if (json.isMember("ShaderFileNames")) {
            const auto& json_shaders = json["ShaderFileNames"];
            if (json_shaders.isArray()) {
                state.shaderFileNameCount = json_shaders.size();
                auto shaders = AllocMem<VpjShaderFileName>(state.shaderFileNameCount);
                state.pShaderFileNames = shaders;
                for (Json::Value::ArrayIndex i = 0; i < json_shaders.size(); ++i) {
                    auto shader_loc = CreateScope("ShaderFileNames", i);
                    if (json_shaders[i].isObject()) {
                        shaders[i].stage = parse_VkShaderStageFlagBits(json_shaders[i]["stage"], CreateScope("stage"));
                        shaders[i].pFilename = parse_string(json_shaders[i]["filename"], CreateScope("filename"));
                    } else {
                        Error() << "Invalid format";
                    }
                }
            } else {
                Error() << "Invalid ShaderFileNames format";
            }
        } else {
            Error() << "Missing ShaderFileNames";
        }
    }

    void parse_GraphicsPipelineState(Json::Value& json) {
        if (json.isObject()) {
            ResolveObjectNames(json, data_.graphicsPipelineState);

            auto graphics_pipeline_state_loc = CreateScope("$.GraphicsPipelineState");
            parse_CommonPipelineState(json, data_.graphicsPipelineState);

            if (json.isMember("GraphicsPipeline")) {
                auto& pipeline_json = json["ComputePipeline"];
                auto graphics_pipeline = AllocMem<VkGraphicsPipelineCreateInfo>();
                data_.graphicsPipelineState.pGraphicsPipeline = graphics_pipeline;
                *graphics_pipeline = parse_VkGraphicsPipelineCreateInfo(pipeline_json, CreateScope("GraphicsPipeline"));
            } else {
                Error() << "Missing GraphicsPipeline";
            }

            if (json.isMember("Renderpass")) {
                auto renderpass = AllocMem<VkRenderPassCreateInfo>();
                data_.graphicsPipelineState.pRenderPass = renderpass;
                *renderpass = parse_VkRenderPassCreateInfo(json["Renderpass"], CreateScope("Renderpass"));
            } else if (json.isMember("Renderpass2")) {
                auto renderpass2 = AllocMem<VkRenderPassCreateInfo2>();
                data_.graphicsPipelineState.pRenderPass = renderpass2;
                *renderpass2 = parse_VkRenderPassCreateInfo2(json["Renderpass2"], CreateScope("Renderpass2"));
            } else {
                Error() << "Missing both Renderpass and Renderpass2";
            }
        } else {
            Error() << "Invalid GraphicsPipelineState format";
        }
    }

    void parse_ComputePipelineState(Json::Value& json) {
        if (json.isObject()) {
            ResolveObjectNames(json, data_.computePipelineState);

            auto compute_pipeline_state_loc = CreateScope("$.ComputePipelineState");
            parse_CommonPipelineState(json, data_.computePipelineState);

            if (json.isMember("ComputePipeline")) {
                auto& pipeline_json = json["ComputePipeline"];
                auto compute_pipeline = AllocMem<VkComputePipelineCreateInfo>();
                data_.computePipelineState.pComputePipeline = compute_pipeline;
                *compute_pipeline = parse_VkComputePipelineCreateInfo(pipeline_json, CreateScope("ComputePipeline"));
            } else {
                Error() << "Missing ComputePipeline";
            }
        } else {
            Error() << "Invalid ComputePipelineState format";
        }
    }

    void parse_PipelineUUID(const Json::Value& json) {
        if (json.isMember("PipelineUUID")) {
            const auto& json_uuid = json["PipelineUUID"];
            if (json_uuid.isArray() && json_uuid.size() == 16) {
                for (uint32_t i = 0; i < 16; ++i) {
                    if (json_uuid[i].isIntegral()) {
                        data_.pipelineUUID[i] = parse_uint8_t(json_uuid[i], CreateScope("$.PipelineUUID", i));
                    } else {
                        Error() << "Invalid PipelineUUID format";
                    }
                }
            } else {
                Error() << "Invalid PipelineUUID format";
            }
        } else {
            Warn() << "No PipelineUUID";
            memset(data_.pipelineUUID, 0, 16);
        }
    }

    void parse_EnabledExtensions(const Json::Value& json) {
        if (json.isMember("EnabledExtensions")) {
            const auto& json_exts = json["EnabledExtensions"];
            if (json_exts.isArray()) {
                data_.enabledExtensionCount = json_exts.size();
                data_.ppEnabledExtensions = AllocMem<const char*>(data_.enabledExtensionCount);
                for (uint32_t i = 0; i < json_exts.size(); ++i) {
                    data_.ppEnabledExtensions[i] = parse_string(json_exts[i], CreateScope("$.EnabledExtensions", i));
                }
            } else {
                Error() << "Invalid EnabledExtensions format";
            }
        }
    }

    Json::Value* find_StructInChain(Json::Value* json, const char* sType) {
        while (json->isObject()) {
            if ((*json)["sType"].asString() == sType) {
                return json;
            } else {
                json = &(*json)["pNext"];
            }
        }
        return nullptr;
    }

    VpjData data_;
};

}  // namespace pcjson

extern "C" {

VpjGenerator vpjCreateParser() { return (new pcjson::Parser())->Handle(); }

bool vpjParsePipelineJson(VpjParser parser, const char* pPipelineJson, VpjData* pPipelineData, const char** ppMessages) {
    return pcjson::Parser::FromHandle(parser)->ParsePipelineJson(pPipelineJson, pPipelineData, ppMessages);
}

bool vpjParseSingleStructJson(VpjParser parser, const char* pJson, void* pStruct, const char** ppMessages) {
    return pcjson::Parser::FromHandle(parser)->ParseSingleStructJson(pJson, pStruct, ppMessages);
}

void vpjFreeParserOutputs(VpjParser parser) { pcjson::Parser::FromHandle(parser)->FreeOutputs(); }

void vpjDestroyParser(VpjParser parser) { delete pcjson::Parser::FromHandle(parser); }

}  // extern "C"
