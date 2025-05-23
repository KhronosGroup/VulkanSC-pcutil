/*
 * Copyright (c) 2025 The Khronos Group Inc.
 * Copyright (c) 2025 RasterGrid Kft.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "vksc_pipeline_json_gen.hpp"
#include "vksc_pipeline_json.h"

namespace pcjson {

class Generator : private GeneratorBase {
  public:
    bool GeneratePipelineJSON(const VpjData* pPipelineData, const char** ppPipelineJson, const char** ppMessages) {
        Json::Value json{};

        ClearStatusAndMessages();

        if (pPipelineData == nullptr) {
            Error() << "pPipelineData is NULL";
        }

        if (ppPipelineJson == nullptr) {
            Error() << "ppPipelineJson is NULL";
        }

        VkStructureType pipeline_stype{};
        if (IsStatusOK()) {
            auto base_ci = reinterpret_cast<const VkBaseInStructure*>(pPipelineData->graphicsPipelineState.pGraphicsPipeline);
            if (base_ci != nullptr) {
                pipeline_stype = base_ci->sType;
            } else {
                Error() << "Pipeline create info structure pointer is NULL";
            }
        }

        if (IsStatusOK()) {
            json["PipelineUUID"] = gen_PipelineUUID(*pPipelineData);
            json["EnabledExtensions"] = gen_EnabledExtensions(*pPipelineData);

            switch (pipeline_stype) {
                case VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO:
                    json["GraphicsPipelineState"] = gen_GraphicsPipelineState(pPipelineData->graphicsPipelineState);
                    break;

                case VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO:
                    json["ComputePipelineState"] = gen_ComputePipelineState(pPipelineData->computePipelineState);
                    break;

                default:
                    Error() << "Unknown pipeline create info structure type: " << pipeline_stype;
                    break;
            }
        }

        if (IsStatusOK()) {
            Json::StreamWriterBuilder builder{};
            json_outputs_.emplace_back(Json::writeString(builder, json));
            *ppPipelineJson = json_outputs_.back().c_str();
        }

        if (ppMessages != nullptr) {
            *ppMessages = GetMessages();
        }

        return IsStatusOK();
    }

    bool GenerateSingleStructJson(const void* pStruct, const char** ppJson, const char** ppMessages) {
        Json::Value json{};

        ClearStatusAndMessages();

        if (pStruct == nullptr) {
            Error() << "pStruct is NULL";
        }

        if (ppJson == nullptr) {
            Error() << "ppJson is NULL";
        }

        if (IsStatusOK()) {
            auto base = reinterpret_cast<const VkBaseInStructure*>(pStruct);
            switch (base->sType) {
                case VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO:
                    json = gen_VkGraphicsPipelineCreateInfo(*reinterpret_cast<const VkGraphicsPipelineCreateInfo*>(pStruct), CreateScope("pStruct", true));
                    break;
                case VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO:
                    json = gen_VkComputePipelineCreateInfo(*reinterpret_cast<const VkComputePipelineCreateInfo*>(pStruct), CreateScope("pStruct", true));
                    break;
                case VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_CREATE_INFO:
                    json = gen_VkSamplerYcbcrConversionCreateInfo(*reinterpret_cast<const VkSamplerYcbcrConversionCreateInfo*>(pStruct), CreateScope("pStruct", true));
                    break;
                case VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO:
                    json = gen_VkSamplerCreateInfo(*reinterpret_cast<const VkSamplerCreateInfo*>(pStruct), CreateScope("pStruct", true));
                    break;
                case VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO:
                    json = gen_VkDescriptorSetLayoutCreateInfo(*reinterpret_cast<const VkDescriptorSetLayoutCreateInfo*>(pStruct), CreateScope("pStruct", true));
                    break;
                case VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO:
                    json = gen_VkPipelineLayoutCreateInfo(*reinterpret_cast<const VkPipelineLayoutCreateInfo*>(pStruct), CreateScope("pStruct", true));
                    break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2:
                    json = gen_VkPhysicalDeviceFeatures2(*reinterpret_cast<const VkPhysicalDeviceFeatures2*>(pStruct), CreateScope("pStruct", true));
                    break;
                case VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO:
                    json = gen_VkRenderPassCreateInfo(*reinterpret_cast<const VkRenderPassCreateInfo*>(pStruct), CreateScope("pStruct", true));
                    break;
                case VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2:
                    json = gen_VkRenderPassCreateInfo2(*reinterpret_cast<const VkRenderPassCreateInfo2*>(pStruct), CreateScope("pStruct", true));
                    break;
                case VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO:
                    json = gen_VkShaderModuleCreateInfo(*reinterpret_cast<const VkShaderModuleCreateInfo*>(pStruct), CreateScope("pStruct", true));
                    break;
                case VK_STRUCTURE_TYPE_DEVICE_OBJECT_RESERVATION_CREATE_INFO:
                    json = gen_VkDeviceObjectReservationCreateInfo(*reinterpret_cast<const VkDeviceObjectReservationCreateInfo*>(pStruct), CreateScope("pStruct", true));
                    break;
                case VK_STRUCTURE_TYPE_PIPELINE_OFFLINE_CREATE_INFO:
                    json = gen_VkPipelineOfflineCreateInfo(*reinterpret_cast<const VkPipelineOfflineCreateInfo*>(pStruct), CreateScope("pStruct", true));
                    break;
                default:
                    Error() << "Unsupported structure type: " << base->sType;
                    break;
            }
        }

        if (IsStatusOK()) {
            Json::StreamWriterBuilder builder{};
            json_outputs_.emplace_back(Json::writeString(builder, json));
            *ppJson = json_outputs_.back().c_str();
        }

        if (ppMessages != nullptr) {
            *ppMessages = GetMessages();
        }

        return IsStatusOK();
    }

    void FreeOutputs() {
        json_outputs_.clear();
        FreeAllMem();
    }

    VpjGenerator Handle() {
        return reinterpret_cast<VpjGenerator>(this);
    }

    static Generator* FromHandle(VpjGenerator handle) {
        return reinterpret_cast<Generator*>(handle);
    }

  private:
    template <typename T>
    Json::Value gen_CommonPipelineState(const T& state) {
        Json::Value json(Json::objectValue);

        if (state.ycbcrSamplerCount > 0) {
            if (state.pYcbcrSamplers != nullptr) {
                auto ycbcr_samplers = reinterpret_cast<const VkSamplerYcbcrConversionCreateInfo*>(state.pYcbcrSamplers);
                auto& json_ycbcr_samplers = json["YcbcrSamplers"] = Json::arrayValue;
                json_ycbcr_samplers.resize(state.ycbcrSamplerCount);
                for (uint32_t i = 0; i < state.ycbcrSamplerCount; ++i) {
                    if (ycbcr_samplers[i].sType == VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_CREATE_INFO) {
                        json_ycbcr_samplers[i] = gen_VkSamplerYcbcrConversionCreateInfo(ycbcr_samplers[i], CreateScope("pYcbcrSamplers", i));
                    } else {
                        Error() << "pYcbcrSamplers[" << i << "] has invalid structure type: " << ycbcr_samplers[i].sType;
                    }
                }
            } else {
                Error() << "ycbcrSamplerCount is not zero but pYcbcrSamplers is NULL";
            }
        }

        if (state.immutableSamplerCount > 0) {
            if (state.pImmutableSamplers != nullptr) {
                auto samplers = reinterpret_cast<const VkSamplerCreateInfo*>(state.pImmutableSamplers);
                auto& json_samplers = json["ImmutableSamplers"] = Json::arrayValue;
                json_samplers.resize(state.immutableSamplerCount);
                for (uint32_t i = 0; i < state.immutableSamplerCount; ++i) {
                    if (samplers[i].sType == VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO) {
                        json_samplers[i] = gen_VkSamplerCreateInfo(samplers[i], CreateScope("pImmutableSamplers", i));
                    } else {
                        Error() << "pImmutableSamplers[" << i << "] has invalid structure type: " << samplers[i].sType;
                    }
                }
            } else {
                Error() << "immutableSamplerCount is not zero but pImmutableSamplers is NULL";
            }
        }

        if (state.descriptorSetLayoutCount > 0) {
            if (state.pDescriptorSetLayouts != nullptr) {
                auto ds_layouts = reinterpret_cast<const VkDescriptorSetLayoutCreateInfo*>(state.pDescriptorSetLayouts);
                auto& json_ds_layouts = json["DescriptorSetLayouts"] = Json::arrayValue;
                json_ds_layouts.resize(state.descriptorSetLayoutCount);
                for (uint32_t i = 0; i < state.descriptorSetLayoutCount; ++i) {
                    if (ds_layouts[i].sType == VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO) {
                        json_ds_layouts[i] = gen_VkDescriptorSetLayoutCreateInfo(ds_layouts[i], CreateScope("pDescriptorSetLayouts", i));
                    } else {
                        Error() << "pDescriptorSetLayouts[" << i << "] has invalid structure type: " << ds_layouts[i].sType;
                    }
                }
            } else {
                Error() << "descriptorSetLayoutCount is not zero but pDescriptorSetLayouts is NULL";
            }
        }

        if (state.pPipelineLayout != nullptr) {
            auto pipeline_layout = reinterpret_cast<const VkPipelineLayoutCreateInfo*>(state.pPipelineLayout);
            if (pipeline_layout->sType == VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO) {
                json["PipelineLayout"] = gen_VkPipelineLayoutCreateInfo(*pipeline_layout, CreateScope("pPipelineLayout", true));
            } else {
                Error() << "pPipelineLayout has invalid structure type: " << pipeline_layout->sType;
            }
        } else {
            Error() << "pPipelineLayout is NULL";
        }

        if (state.shaderFileNameCount > 0) {
            if (state.pShaderFileNames != nullptr) {
                auto& json_shaders = json["ShaderFileNames"] = Json::arrayValue;
                json_shaders.resize(state.shaderFileNameCount);
                for (uint32_t i = 0; i < state.shaderFileNameCount; ++i) {
                    auto shader_loc = CreateScope("pShaderFileNames", i);
                    json_shaders[i]["stage"] = gen_VkShaderStageFlagBits(static_cast<VkShaderStageFlagBits>(state.pShaderFileNames[i].stage),
                                                                         CreateScope("stage"));
                    if (state.pShaderFileNames[i].pFilename != nullptr) {
                        json_shaders[i]["filename"] = state.pShaderFileNames[i].pFilename;
                    } else {
                        Error() << "pFilename is NULL";
                    }
                }
            } else {
                Error() << "shaderFileNameCount is not zero but pShaderFileNames is NULL";
            }
        }

        auto features = reinterpret_cast<const VkPhysicalDeviceFeatures2*>(state.pPhysicalDeviceFeatures);
        if (features != nullptr) {
            if (features->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2) {
                json["PhysicalDeviceFeatures"] = gen_VkPhysicalDeviceFeatures2(*features, CreateScope("pPhysicalDeviceFeatures", true));
            } else {
                Error() << "pPhysicalDeviceFeatures has invalid structure type: " << features->sType;
            }
        }

        return json;
    }

    Json::Value gen_GraphicsPipelineState(const VpjGraphicsPipelineState& state) {
        auto graphics_pipeline_state_loc = CreateScope("pPipelineData->graphicsPipelineState");
        Json::Value json = gen_CommonPipelineState(state);

        auto graphics_pipeline = reinterpret_cast<const VkGraphicsPipelineCreateInfo*>(state.pGraphicsPipeline);
        json["GraphicsPipeline"] = gen_VkGraphicsPipelineCreateInfo(*graphics_pipeline, CreateScope("pGraphicsPipeline", true));

        auto renderpass = reinterpret_cast<const VkRenderPassCreateInfo*>(state.pRenderPass);
        if (renderpass != nullptr) {
            if (renderpass->sType == VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO) {
                json["Renderpass"] = gen_VkRenderPassCreateInfo(*renderpass, CreateScope("pRenderPass", true));
            } else if (renderpass->sType == VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2) {
                auto renderpass2 = reinterpret_cast<const VkRenderPassCreateInfo2*>(state.pRenderPass);
                json["Renderpass2"] = gen_VkRenderPassCreateInfo2(*renderpass2, CreateScope("pRenderPass", true));
            } else {
                Error() << "Unknown render pass create info structure type: " << renderpass->sType;
            }
        } else {
            Error() << "pRenderPass is NULL";
        }

        return json;
    }

    Json::Value gen_ComputePipelineState(const VpjComputePipelineState& state) {
        auto graphics_pipeline_state_loc = CreateScope("pPipelineData->computePipelineState");
        Json::Value json = gen_CommonPipelineState(state);

        auto compute_pipeline = reinterpret_cast<const VkComputePipelineCreateInfo*>(state.pComputePipeline);
        json["ComputePipeline"] = gen_VkComputePipelineCreateInfo(*compute_pipeline, CreateScope("pComputePipeline"));

        return json;
    }

    Json::Value gen_PipelineUUID(const VpjData& data) {
        Json::Value json = Json::arrayValue;
        json.resize(16);
        for (uint32_t i = 0; i < json.size(); ++i) {
            json[i] = data.pipelineUUID[i];
        }
        return json;
    }

    Json::Value gen_EnabledExtensions(const VpjData& data) {
        Json::Value json = Json::arrayValue;
        json.resize(data.enabledExtensionCount);
        if (data.enabledExtensionCount > 0 && data.ppEnabledExtensions == nullptr) {
            Error() << "pPipelineData->enabledExtensionCount is not zero but pPipelineData->ppEnabledExtensions is NULL";
        }
        if (data.ppEnabledExtensions != nullptr) {
            for (uint32_t i = 0; i < data.enabledExtensionCount; ++i) {
                if (data.ppEnabledExtensions[i] != nullptr) {
                    json[i] = data.ppEnabledExtensions[i];
                } else {
                    Error() << "pPipelineData->ppEnabledExtensions[" << i << "] is NULL";
                }
            }
        }
        return json;
    }

    std::vector<std::string> json_outputs_;
};

}  // namespace pcjson

extern "C" {

VpjGenerator vpjCreateGenerator() {
    return (new pcjson::Generator())->Handle();
}

bool vpjGeneratePipelineJson(VpjGenerator generator, const VpjData* pPipelineData, const char** ppPipelineJson, const char** ppMessages) {
    return pcjson::Generator::FromHandle(generator)->GeneratePipelineJSON(pPipelineData, ppPipelineJson, ppMessages);
}

bool vpjGenerateSingleStructJson(VpjGenerator generator, const void* pStruct, const char** ppJson, const char** ppMessages) {
    return pcjson::Generator::FromHandle(generator)->GenerateSingleStructJson(pStruct, ppJson, ppMessages);
}

void vpjFreeGeneratorOutputs(VpjGenerator generator) {
    pcjson::Generator::FromHandle(generator)->FreeOutputs();
}

void vpjDestroyGenerator(VpjGenerator generator) {
    delete pcjson::Generator::FromHandle(generator);
}

}  // extern "C"
