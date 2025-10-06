/*
 * Copyright (c) 2025 The Khronos Group Inc.
 * Copyright (c) 2025 RasterGrid Kft.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "vksc_pipeline_json_gen.hpp"
#include "vksc_pipeline_json.h"

#include <assert.h>
#include <memory.h>
#include <string.h>

#include "hashlib/md5.hpp"

namespace pcjson {

class Generator : private GeneratorBase {
  public:
    void SetMD5PipelineUUIDGeneration(bool enable) { md5_generator_enabled_ = enable; }

    bool GeneratePipelineJSON(const VpjData* pPipelineData, const char** ppPipelineJson, const char** ppMessages) {
        Json::Value json{};

        generated_uuid_ = nullptr;

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

            EliminateIrrelevantHandleValues(json);

            // Pipeline UUID is set last to be able to do MD5 generation from the full data if needed
            if (md5_generator_enabled_) {
                hashlib::md5 md5{};
                gen_JsonMD5(md5, json);
                generated_uuid_ = AllocMem<uint8_t>(VK_UUID_SIZE);
                memcpy(generated_uuid_, md5.digest().data(), VK_UUID_SIZE);
                json["PipelineUUID"] = gen_PipelineUUID(generated_uuid_);
            } else {
                json["PipelineUUID"] = gen_PipelineUUID(pPipelineData->pipelineUUID);
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

    bool GetGeneratedPipelineUUID(uint8_t* pPipelineUUID, const char** ppMessages) {
        if (!IsStatusOK()) {
            Error() << "Cannot retrieve generated pipeline UUID if the generation failed";
        }

        if (pPipelineUUID == nullptr) {
            Error() << "pPipelineUUID is NULL";
        }

        if (generated_uuid_ == nullptr) {
            Error() << "No pipeline UUID was generated";
        }

        if (IsStatusOK()) {
            memcpy(pPipelineUUID, generated_uuid_, VK_UUID_SIZE);
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
                    json = gen_VkGraphicsPipelineCreateInfo(*reinterpret_cast<const VkGraphicsPipelineCreateInfo*>(pStruct),
                                                            CreateScope("pStruct", true));
                    break;
                case VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO:
                    json = gen_VkComputePipelineCreateInfo(*reinterpret_cast<const VkComputePipelineCreateInfo*>(pStruct),
                                                           CreateScope("pStruct", true));
                    break;
                case VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_CREATE_INFO:
                    json = gen_VkSamplerYcbcrConversionCreateInfo(
                        *reinterpret_cast<const VkSamplerYcbcrConversionCreateInfo*>(pStruct), CreateScope("pStruct", true));
                    break;
                case VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO:
                    json = gen_VkSamplerCreateInfo(*reinterpret_cast<const VkSamplerCreateInfo*>(pStruct),
                                                   CreateScope("pStruct", true));
                    break;
                case VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO:
                    json = gen_VkDescriptorSetLayoutCreateInfo(*reinterpret_cast<const VkDescriptorSetLayoutCreateInfo*>(pStruct),
                                                               CreateScope("pStruct", true));
                    break;
                case VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO:
                    json = gen_VkPipelineLayoutCreateInfo(*reinterpret_cast<const VkPipelineLayoutCreateInfo*>(pStruct),
                                                          CreateScope("pStruct", true));
                    break;
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2:
                    json = gen_VkPhysicalDeviceFeatures2(*reinterpret_cast<const VkPhysicalDeviceFeatures2*>(pStruct),
                                                         CreateScope("pStruct", true));
                    break;
                case VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO:
                    json = gen_VkRenderPassCreateInfo(*reinterpret_cast<const VkRenderPassCreateInfo*>(pStruct),
                                                      CreateScope("pStruct", true));
                    break;
                case VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2:
                    json = gen_VkRenderPassCreateInfo2(*reinterpret_cast<const VkRenderPassCreateInfo2*>(pStruct),
                                                       CreateScope("pStruct", true));
                    break;
                case VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO:
                    json = gen_VkShaderModuleCreateInfo(*reinterpret_cast<const VkShaderModuleCreateInfo*>(pStruct),
                                                        CreateScope("pStruct", true));
                    break;
                case VK_STRUCTURE_TYPE_DEVICE_OBJECT_RESERVATION_CREATE_INFO:
                    json = gen_VkDeviceObjectReservationCreateInfo(
                        *reinterpret_cast<const VkDeviceObjectReservationCreateInfo*>(pStruct), CreateScope("pStruct", true));
                    break;
                case VK_STRUCTURE_TYPE_PIPELINE_OFFLINE_CREATE_INFO:
                    json = gen_VkPipelineOfflineCreateInfo(*reinterpret_cast<const VkPipelineOfflineCreateInfo*>(pStruct),
                                                           CreateScope("pStruct", true));
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

    bool FilterDeviceFeatures(const void* pDeviceCreateInfoPNext, const void** ppPhysicalDeviceFeatures, const char** ppMessages) {
        ClearStatusAndMessages();

        if (pDeviceCreateInfoPNext == nullptr) {
            Error() << "pDeviceCreateInfoPNext is NULL";
        }

        if (ppPhysicalDeviceFeatures == nullptr) {
            Error() << "ppPhysicalDeviceFeatures is NULL";
        }

        if (IsStatusOK()) {
            *ppPhysicalDeviceFeatures =
                filter_VkPhysicalDeviceFeatures2(pDeviceCreateInfoPNext, CreateScope("pDeviceCreateInfoPNext", true));
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

    VpjGenerator Handle() { return reinterpret_cast<VpjGenerator>(this); }

    static Generator* FromHandle(VpjGenerator handle) { return reinterpret_cast<Generator*>(handle); }

  private:
    void gen_MD5(hashlib::md5& md5, const void* data, size_t size) {
        const uint8_t* ptr = reinterpret_cast<const uint8_t*>(data);
        md5.update(ptr, ptr + size);
    }

    template <typename T>
    void gen_MD5(hashlib::md5& md5, const T& data) {
        gen_MD5(md5, &data, sizeof(data));
    }

    void gen_JsonMD5(hashlib::md5& md5, const Json::Value& value) {
        if (value.isArray()) {
            uint32_t array_size = value.size();
            gen_MD5(md5, array_size);
            for (uint32_t i = 0; i < array_size; ++i) {
                gen_JsonMD5(md5, value[i]);
            }
        } else if (value.isObject()) {
            for (const auto& member : value.getMemberNames()) {
                gen_JsonMD5(md5, value[member]);
            }
        } else if (value.isBool()) {
            gen_MD5(md5, value.asBool());
        } else if (value.isString()) {
            auto str = value.asString();
            gen_MD5(md5, str.size());
            gen_MD5(md5, str.data(), str.size());
        } else if (value.isDouble()) {
            gen_MD5(md5, value.asDouble());
        } else if (value.isInt()) {
            gen_MD5(md5, value.asInt());
        } else if (value.isUInt()) {
            gen_MD5(md5, value.asUInt());
        } else if (value.isInt64()) {
            gen_MD5(md5, value.asInt64());
        } else if (value.isUInt64()) {
            gen_MD5(md5, value.asUInt());
        } else {
            Error() << "Unexpected JSON value type";
        }
    }

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
                        Json::Value key_ycbcr_sampler_pair = Json::objectValue;
                        key_ycbcr_sampler_pair[std::to_string(i)] =
                            gen_VkSamplerYcbcrConversionCreateInfo(ycbcr_samplers[i], CreateScope("pYcbcrSamplers", i));
                        json_ycbcr_samplers[i] = key_ycbcr_sampler_pair;
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
                        Json::Value key_immutable_sampler_pair = Json::objectValue;
                        key_immutable_sampler_pair[std::to_string(i)] =
                            gen_VkSamplerCreateInfo(samplers[i], CreateScope("pImmutableSamplers", i));
                        json_samplers[i] = key_immutable_sampler_pair;
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
                        Json::Value key_ds_pair = Json::objectValue;
                        key_ds_pair[std::to_string(i)] =
                            gen_VkDescriptorSetLayoutCreateInfo(ds_layouts[i], CreateScope("pDescriptorSetLayouts", i));
                        json_ds_layouts[i] = key_ds_pair;
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
                    json_shaders[i]["stage"] = gen_VkShaderStageFlagBits(
                        static_cast<VkShaderStageFlagBits>(state.pShaderFileNames[i].stage), CreateScope("stage"));
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
                json["PhysicalDeviceFeatures"] =
                    gen_VkPhysicalDeviceFeatures2(*features, CreateScope("pPhysicalDeviceFeatures", true));
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

        if (IsStatusOK()) {
            ResolveObjectNames(json, state);
        }

        return json;
    }

    Json::Value gen_ComputePipelineState(const VpjComputePipelineState& state) {
        auto graphics_pipeline_state_loc = CreateScope("pPipelineData->computePipelineState");
        Json::Value json = gen_CommonPipelineState(state);

        auto compute_pipeline = reinterpret_cast<const VkComputePipelineCreateInfo*>(state.pComputePipeline);
        json["ComputePipeline"] = gen_VkComputePipelineCreateInfo(*compute_pipeline, CreateScope("pComputePipeline"));

        if (IsStatusOK()) {
            ResolveObjectNames(json, state);
        }

        return json;
    }

    Json::Value gen_PipelineUUID(const uint8_t* pPipelineUUID) {
        Json::Value json = Json::arrayValue;
        json.resize(VK_UUID_SIZE);
        for (uint32_t i = 0; i < json.size(); ++i) {
            json[i] = pPipelineUUID[i];
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

    template <typename T>
    void ResolveObjectNames(Json::Value& pipeline_state_json, const T& state) {
        const auto update_key = [&](Json::Value& json, const auto old_key, const auto new_key, const char* object_set_name) {
            Json::Value value = json[old_key];
            if (value.isNull()) {
                Error() << "Did not find " << object_set_name << " with key \"" << old_key << '\"';
            }
            json.removeMember(old_key);
            json[new_key] = value;
        };

        const auto update_keys_in_array = [&](Json::Value& json_array, uint32_t count, const char** names,
                                              const char* object_set_name) {
            if (json_array.size() != count) {
                Error() << "Mismatch between specified and generated JSON object count for " << object_set_name;
                return;
            }
            for (uint32_t i = 0; i < count; ++i) {
                update_key(json_array[i], std::to_string(i), names[i], object_set_name);
            }
        };

        const auto update_array_with_names = [&](Json::Value& json_array, uint32_t count, const char** names,
                                                 const char* object_set_name) {
            Json::Value new_array = Json::arrayValue;
            for (uint32_t i = 0; i < json_array.size(); i++) {
                auto& name_id_json = json_array[i];
                uint64_t name_id = (name_id_json.isString() && name_id_json.asString() == "") ? 0 : name_id_json.asUInt64();
                if (name_id < count) {
                    new_array[i] = names[name_id];
                } else {
                    Error() << "Ouf of range " << object_set_name << " id (" << name_id << ") during resolving names";
                }
            }
            json_array.copy(new_array);
        };

        if (state.ppDescriptorSetLayoutNames != nullptr) {
            update_keys_in_array(pipeline_state_json["DescriptorSetLayouts"], state.descriptorSetLayoutCount,
                                 state.ppDescriptorSetLayoutNames, "DescriptorSetLayouts");
            update_array_with_names(pipeline_state_json["PipelineLayout"]["pSetLayouts"], state.descriptorSetLayoutCount,
                                    state.ppDescriptorSetLayoutNames, "DescriptorSetLayouts");
        }

        if (state.ppYcbcrSamplerNames != nullptr) {
            update_keys_in_array(pipeline_state_json["YcbcrSamplers"], state.ycbcrSamplerCount, state.ppYcbcrSamplerNames,
                                 "YcbcrSamplers");

            for (auto& immutable_sampler : pipeline_state_json["ImmutableSamplers"]) {
                if (immutable_sampler.size() != 1) {
                    Error() << "Invalid ImmutableSamplers format containing multiple keys in element";
                }
                Json::Value* pNext = &(*immutable_sampler.begin())["pNext"];

                while (*pNext != "NULL") {
                    if ((*pNext)["sType"] == "VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_INFO") {
                        break;
                    }
                    pNext = &(*pNext)["pNext"];
                }

                if (*pNext != "NULL") {
                    Json::Value& conversion = (*pNext)["conversion"];
                    uint64_t ycbcr_sampler_id = (conversion.isString() && conversion.asString() == "") ? 0 : conversion.asUInt64();
                    if (ycbcr_sampler_id < state.ycbcrSamplerCount) {
                        conversion.copy(state.ppYcbcrSamplerNames[ycbcr_sampler_id]);
                    } else {
                        Error() << "Ouf of range YCbCr sampler reference";
                    }
                }
            }
        }

        if (state.ppImmutableSamplerNames != nullptr) {
            update_keys_in_array(pipeline_state_json["ImmutableSamplers"], state.immutableSamplerCount,
                                 state.ppImmutableSamplerNames, "ImmutableSamplers");

            for (auto& descriptor_set_layout : pipeline_state_json["DescriptorSetLayouts"]) {
                if (descriptor_set_layout.size() != 1) {
                    Error() << "Invalid DescriptorSetLayouts format containing multiple keys in element";
                }
                for (auto& binding : (*descriptor_set_layout.begin())["pBindings"]) {
                    update_array_with_names(binding["pImmutableSamplers"], state.immutableSamplerCount,
                                            state.ppImmutableSamplerNames, "ImmutableSampler");
                }
            }
        }
    }

    void EliminateIrrelevantHandleValues(Json::Value& json) {
        // Here we eliminate all handle values that are irrelevant from the perspective of the pipeline JSON:
        // * the VkRenderPass handle in VkGraphicsPipelineCreateInfo which always has a 1:1 association to
        //   the render pass create info
        // * the VkPipelineLayout handle in both graphics and compute pipeline create infos which always have
        //   1:1 association to the pipeline layout create info
        // * the basePipelineHandle in both graphics and compute pipeline create infos which is not relevant
        // * the VkShaderModule handles in VkPipelineShaderStageCreateInfo which are not relevant

        if (json.isMember("GraphicsPipelineState")) {
            auto& pipeline_json = json["GraphicsPipelineState"]["GraphicsPipeline"];
            pipeline_json["renderPass"] = "";
            pipeline_json["basePipelineHandle"] = "";

            auto& stages = pipeline_json["pStages"];
            if (stages.isArray()) {
                for (auto& stage_info : stages) {
                    stage_info["module"] = "";
                }
            }
        } else if (json.isMember("ComputePipelineState")) {
            auto& pipeline_json = json["ComputePipelineState"]["ComputePipeline"];
            pipeline_json["basePipelineHandle"] = "";

            auto& stage_info = pipeline_json["stage"];
            stage_info["module"] = "";
        }
    }

    std::vector<std::string> json_outputs_;

    bool md5_generator_enabled_{false};
    uint8_t* generated_uuid_{nullptr};
};

}  // namespace pcjson

extern "C" {

VpjGenerator vpjCreateGenerator() { return (new pcjson::Generator())->Handle(); }

void vpjSetMD5PipelineUUIDGeneration(VpjGenerator generator, bool enable) {
    pcjson::Generator::FromHandle(generator)->SetMD5PipelineUUIDGeneration(enable);
}

bool vpjGeneratePipelineJson(VpjGenerator generator, const VpjData* pPipelineData, const char** ppPipelineJson,
                             const char** ppMessages) {
    return pcjson::Generator::FromHandle(generator)->GeneratePipelineJSON(pPipelineData, ppPipelineJson, ppMessages);
}

bool vpjGetGeneratedPipelineUUID(VpjGenerator generator, uint8_t* pPipelineUUID, const char** ppMessages) {
    return pcjson::Generator::FromHandle(generator)->GetGeneratedPipelineUUID(pPipelineUUID, ppMessages);
}

bool vpjGenerateSingleStructJson(VpjGenerator generator, const void* pStruct, const char** ppJson, const char** ppMessages) {
    return pcjson::Generator::FromHandle(generator)->GenerateSingleStructJson(pStruct, ppJson, ppMessages);
}

bool vpjFilterDeviceFeatures(VpjGenerator generator, const void* pDeviceCreateInfoPNext, const void** ppPhysicalDeviceFeatures,
                             const char** ppMessages) {
    return pcjson::Generator::FromHandle(generator)->FilterDeviceFeatures(pDeviceCreateInfoPNext, ppPhysicalDeviceFeatures,
                                                                          ppMessages);
}

void vpjFreeGeneratorOutputs(VpjGenerator generator) { pcjson::Generator::FromHandle(generator)->FreeOutputs(); }

void vpjDestroyGenerator(VpjGenerator generator) { delete pcjson::Generator::FromHandle(generator); }

}  // extern "C"
