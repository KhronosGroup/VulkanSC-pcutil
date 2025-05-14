/*
 * Copyright (c) 2025 The Khronos Group Inc.
 * Copyright (c) 2025 RasterGrid Kft.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "vksc_pipeline_json_parse.hpp"
#include "vksc_pipeline_json.h"

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
                parse_ComputePipelineState(json["GraphicsPipelineState"]);
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
                    *reinterpret_cast<VkGraphicsPipelineCreateInfo*>(pStruct) = parse_VkGraphicsPipelineCreateInfo(json, CreateScope("$"));
                    break;
                case VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO:
                    *reinterpret_cast<VkComputePipelineCreateInfo*>(pStruct) = parse_VkComputePipelineCreateInfo(json, CreateScope("$"));
                    break;
                case VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_CREATE_INFO:
                    *reinterpret_cast<VkSamplerYcbcrConversionCreateInfo*>(pStruct) = parse_VkSamplerYcbcrConversionCreateInfo(json, CreateScope("$"));
                    break;
                case VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO:
                    *reinterpret_cast<VkSamplerCreateInfo*>(pStruct) = parse_VkSamplerCreateInfo(json, CreateScope("$"));
                    break;
                case VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO:
                    *reinterpret_cast<VkDescriptorSetLayoutCreateInfo*>(pStruct) = parse_VkDescriptorSetLayoutCreateInfo(json, CreateScope("$"));
                    break;
                case VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO:
                    *reinterpret_cast<VkPipelineLayoutCreateInfo*>(pStruct) = parse_VkPipelineLayoutCreateInfo(json, CreateScope("$"));
                    break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2:
                    *reinterpret_cast<VkPhysicalDeviceFeatures2*>(pStruct) = parse_VkPhysicalDeviceFeatures2(json, CreateScope("$"));
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
                    *reinterpret_cast<VkDeviceObjectReservationCreateInfo*>(pStruct) = parse_VkDeviceObjectReservationCreateInfo(json, CreateScope("$"));
                    break;
                case VK_STRUCTURE_TYPE_PIPELINE_OFFLINE_CREATE_INFO:
                    *reinterpret_cast<VkPipelineOfflineCreateInfo*>(pStruct) = parse_VkPipelineOfflineCreateInfo(json, CreateScope("$"));
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

    void FreeOutputs() {
        FreeAllMem();
    }

    VpjParser Handle() {
        return reinterpret_cast<VpjParser>(this);
    }

    static Parser* FromHandle(VpjParser handle) {
        return reinterpret_cast<Parser*>(handle);
    }

  private:
    template <typename T>
    void parse_CommonPipelineState(const Json::Value& json, T& state) {
        if (json.isMember("YcbcrSamplers")) {
            const auto& json_ycbcr_samplers = json["YcbcrSamplers"];
            if (json_ycbcr_samplers.isArray()) {
                state.ycbcrSamplerCount = json_ycbcr_samplers.size();
                auto ycbcr_samplers = AllocMem<VkSamplerYcbcrConversionCreateInfo>(state.ycbcrSamplerCount);
                state.pYcbcrSamplers = ycbcr_samplers;
                for (uint32_t i = 0; i < json_ycbcr_samplers.size(); ++i) {
                    ycbcr_samplers[i] = parse_VkSamplerYcbcrConversionCreateInfo(json_ycbcr_samplers[i], CreateScope("YcbcrSamplers", i));
                }
            } else {
                Error() << "Invalid YcbcrSamplers format";
            }
        }

        if (json.isMember("ImmutableSamplers")) {
            const auto& json_samplers = json["ImmutableSamplers"];
            if (json_samplers.isArray()) {
                state.immutableSamplerCount = json_samplers.size();
                auto samplers = AllocMem<VkSamplerCreateInfo>(state.immutableSamplerCount);
                state.pImmutableSamplers = samplers;
                for (uint32_t i = 0; i < json_samplers.size(); ++i) {
                    samplers[i] = parse_VkSamplerCreateInfo(json_samplers[i], CreateScope("ImmutableSamplers", i));
                }
            } else {
                Error() << "Invalid ImmutableSamplers format";
            }
        }

        if (json.isMember("DescriptorSetLayouts")) {
            const auto& json_ds_layouts = json["DescriptorSetLayouts"];
            if (json_ds_layouts.isArray()) {
                state.descriptorSetLayoutCount = json_ds_layouts.size();
                auto ds_layouts = AllocMem<VkDescriptorSetLayoutCreateInfo>(state.descriptorSetLayoutCount);
                state.pImmutableSamplers = ds_layouts;
                for (uint32_t i = 0; i < json_ds_layouts.size(); ++i) {
                    ds_layouts[i] = parse_VkDescriptorSetLayoutCreateInfo(json_ds_layouts[i], CreateScope("DescriptorSetLayouts", i));
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
                for (uint32_t i = 0; i < json_shaders.size(); ++i) {
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

    void parse_GraphicsPipelineState(const Json::Value& json) {
        if (json.isObject()) {
            auto graphics_pipeline_state_loc = CreateScope("$.GraphicsPipelineState");
            parse_CommonPipelineState(json, data_.graphicsPipelineState);

            if (json.isMember("GraphicsPipeline")) {
                auto graphics_pipeline = AllocMem<VkGraphicsPipelineCreateInfo>();
                data_.graphicsPipelineState.pGraphicsPipeline = graphics_pipeline;
                *graphics_pipeline = parse_VkGraphicsPipelineCreateInfo(json["GraphicsPipeline"], CreateScope("GraphicsPipeline"));
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

    void parse_ComputePipelineState(const Json::Value& json) {
        if (json.isObject()) {
            auto compute_pipeline_state_loc = CreateScope("$.ComputePipelineState");
            parse_CommonPipelineState(json, data_.computePipelineState);

            if (json.isMember("ComputePipeline")) {
                auto compute_pipeline = AllocMem<VkComputePipelineCreateInfo>();
                data_.computePipelineState.pComputePipeline = compute_pipeline;
                *compute_pipeline = parse_VkComputePipelineCreateInfo(json["ComputePipeline"], CreateScope("ComputePipeline"));
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

    VpjData data_;
};

}  // namespace pcjson

extern "C" {

VpjGenerator vpjCreateParser() {
    return (new pcjson::Parser())->Handle();
}

bool vpjParsePipelineJson(VpjParser parser, const char* pPipelineJson, VpjData* pPipelineData, const char** ppMessages) {
    return pcjson::Parser::FromHandle(parser)->ParsePipelineJson(pPipelineJson, pPipelineData, ppMessages);
}

bool vpjParseSingleStructJson(VpjParser parser, const char* pJson, void* pStruct, const char** ppMessages) {
    return pcjson::Parser::FromHandle(parser)->ParseSingleStructJson(pJson, pStruct, ppMessages);
}

void vpjFreeParserOutputs(VpjParser parser) {
    pcjson::Parser::FromHandle(parser)->FreeOutputs();
}

void vpjDestroyParser(VpjParser parser) {
    delete pcjson::Parser::FromHandle(parser);
}

}  // extern "C"
