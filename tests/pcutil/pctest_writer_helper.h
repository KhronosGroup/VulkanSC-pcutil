/*
 * Copyright (c) 2025 The Khronos Group Inc.
 * Copyright (c) 2025 RasterGrid Kft.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "pcwriter.hpp"
#include <vulkan/vulkan_sc.h>

#include <cstdint>
#include <string>
#include <vector>
#include <sstream>
#include <cassert>

struct PCWriterTestData {
    PCWriterTestData(uint32_t seed) : seed_(seed) {}

    void GenPipelineEntries(uint32_t pipeline_entry_count, uint32_t shader_stage_count = 0) {
        pipeline_test_datas_.resize(pipeline_entry_count);

        for (uint32_t i = 0; i < pipeline_entry_count; i++) {
            uint32_t uuid[VK_UUID_SIZE / 4]{i, 0x55665566, 0xbeef7ac0, 0xfeedface};
            memcpy(pipeline_test_datas_[i].uuid, uuid, VK_UUID_SIZE);

            GenShaderStages(0, shader_stage_count);
        }
    }

    void GenPipelineJsonData() {
        for (uint32_t i = 0; i < pipeline_test_datas_.size(); i++) {
            std::stringstream ss;
            ss << "{ 'msg' : 'Simple json playload #" << i << "' }";
            pipeline_test_datas_[i].json = ss.str();
        }
    }

    void GenPipelineVendorData(uint32_t vendor_data_size) {
        per_pipeline_vendor_data_size_ = vendor_data_size;

        for (uint32_t i = 0; i < pipeline_test_datas_.size(); i++) {
            pipeline_test_datas_[i].vendor_data.resize(per_pipeline_vendor_data_size_);
            for (uint32_t j = 0; j < pipeline_test_datas_[i].vendor_data.size(); j++) {
                pipeline_test_datas_[i].vendor_data[j] = (seed_ + i + sample_payload[i % sizeof(sample_payload)]) % 0xff;
            }
        }
    }

    void GenShaderStages(uint32_t pipeline_entry_id, uint32_t shader_stage_count) {
        assert(pipeline_entry_id < pipeline_test_datas_.size());
        auto &pipeline_stages = pipeline_test_datas_[pipeline_entry_id].stages;

        pipeline_stages.resize(shader_stage_count);

        for (uint32_t i = 0; i < shader_stage_count; i++) {
            uint32_t code[8]{0x11111111, 0x22222222, 0x33333333, i, 0x55665566, 0x66666666, 0x77887788, 0x88998899};
            pipeline_stages[i].code.resize(sizeof(code));
            memcpy(pipeline_stages[i].code.data(), reinterpret_cast<uint8_t *>(&code[0]), sizeof(code));
        }
    }

    void GenShaderStageVendorData(uint32_t vendor_data_size) {
        per_stage_vendor_data_size_ = vendor_data_size;

        for (auto &pipeline_test_data : pipeline_test_datas_) {
            auto &pipeline_stages = pipeline_test_data.stages;

            for (uint32_t i = 0; i < pipeline_stages.size(); i++) {
                pipeline_stages[i].vendor_data.resize(per_stage_vendor_data_size_);
                for (uint32_t j = 0; j < pipeline_stages[i].vendor_data.size(); j++) {
                    pipeline_stages[i].vendor_data[j] = (seed_ + 17 * i + j + sample_payload[j % sizeof(sample_payload)]) % 0xff;
                }
            }
        }
    }

    void GenGlobalVendorData(size_t size) {
        global_vendor_data_.resize(size);
        for (uint32_t i = 0; i < global_vendor_data_.size(); i++) {
            global_vendor_data_[i] = (seed_ + sample_payload[i % sizeof(sample_payload)]) % 0xff;
        }
    }

    void SetJson(uint32_t pipeline_entry_id, const std::string &payload) { pipeline_test_datas_[pipeline_entry_id].json = payload; }

    void SetImplementationData(uint32_t implementation_data) { implementation_data_ = implementation_data; }

    void SetShaderStageCode(uint32_t pipeline_entry_id, uint32_t shader_stage_id, const std::vector<uint8_t> &code) {
        pipeline_test_datas_[pipeline_entry_id].stages[shader_stage_id].code = code;
    }

    uint32_t seed_ = 0;
    uint32_t vendor_id_ = 0x10de;
    uint32_t device_id_ = 0xabcd;
    uint8_t pipeline_cache_uuid_[VK_UUID_SIZE] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf};
    uint32_t implementation_data_ = 0xbeef7ac0;

    struct StageTestData {
        std::vector<uint8_t> code;
        std::vector<uint8_t> vendor_data;
    };

    struct PipelineTestData {
        uint8_t uuid[VK_UUID_SIZE];
        std::string json;
        std::vector<uint8_t> vendor_data;
        std::vector<StageTestData> stages;
    };

    std::vector<PipelineTestData> pipeline_test_datas_;

    uint32_t per_pipeline_vendor_data_size_ = 0;
    uint32_t per_stage_vendor_data_size_ = 0;

    std::vector<uint8_t> global_vendor_data_;

    static const uint8_t sample_payload[8];
    static const uint64_t default_cache_size_ = 10 * 1024 * 1024;  // 10 MB
};

void WriteTestDataToCache(VKSCPipelineCacheHeaderWriter &pcw, std::vector<uint8_t> &cache, const PCWriterTestData &td);
