/*
 * Copyright (c) 2025 The Khronos Group Inc.
 * Copyright (c) 2025 RasterGrid Kft.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "pcutil_test_writer_helper.h"

#include <vulkan/pcutil/pcreader.hpp>

const uint8_t PCWriterTestData::sample_payload[8] = {0xde, 0xad, 0xde, 0xad, 0xbe, 0xef, 0x7a, 0xc0};

void WriteTestDataToCache(VKSCPipelineCacheHeaderWriter &pcw, std::vector<uint8_t> &cache, const PCWriterTestData &td) {
    uint32_t pipeline_entry_count_ = uint32_t(td.pipeline_test_datas_.size());

    std::vector<VKSCPipelineEntry *> pipe_store(pipeline_entry_count_);
    pcw.allocatePipelineIndex(pipeline_entry_count_);

    pcw.setImplementationData(td.implementation_data_);

    uint32_t pipeline_index_offset =
        uint32_t(sizeof(VkPipelineCacheHeaderVersionSafetyCriticalOne) + td.global_vendor_data_.size());
    if (td.global_vendor_data_.size()) {
        pcw.setPipelineIndexOffset(pipeline_index_offset);
    }

    memcpy(cache.data() + sizeof(VkPipelineCacheHeaderVersionSafetyCriticalOne), td.global_vendor_data_.data(),
           td.global_vendor_data_.size());

    uint32_t pipeline_index_stride = uint32_t(sizeof(VkPipelineCacheSafetyCriticalIndexEntry) + td.per_pipeline_vendor_data_size_);
    pcw.setPipelineIndexStride(pipeline_index_stride);

    uint32_t stage_index_stride = uint32_t(sizeof(VkPipelineCacheSafetyCriticalIndexEntry) + td.per_stage_vendor_data_size_);
    pcw.setStageIndexStride(stage_index_stride);

    size_t next_pipeline_vendor_data_offset = pipeline_index_offset + sizeof(VkPipelineCacheSafetyCriticalIndexEntry);

    for (uint32_t i = 0; i < pipeline_entry_count_; i++) {
        VKSCPipelineEntry *pe = new VKSCPipelineEntry(td.pipeline_test_datas_[i].uuid, 32 + i * 11);
        auto &pipeline_stages = td.pipeline_test_datas_[i].stages;

        if (td.pipeline_test_datas_[i].json.size()) {
            pe->setJsonCode(td.pipeline_test_datas_[i].json.size(),
                            reinterpret_cast<uint8_t const *>(td.pipeline_test_datas_[i].json.data()));
        }

        memcpy(cache.data() + next_pipeline_vendor_data_offset, td.pipeline_test_datas_[i].vendor_data.data(),
               td.per_pipeline_vendor_data_size_);

        pe->allocateStages(uint32_t(pipeline_stages.size()));

        for (uint32_t stage_id = 0; stage_id < pipeline_stages.size(); stage_id++) {
            pe->setShaderStageCode(stage_id, pipeline_stages[stage_id].code.size(), pipeline_stages[stage_id].code.data());
        }

        pipe_store[i] = pe;
        pcw.setPipelineEntry(i, pe);

        next_pipeline_vendor_data_offset += pipeline_index_stride;
    }

    pcw.writeHeaderSafetyCriticalOne(cache.size(), cache.data());
    pcw.writePipelineIndex(cache.size(), cache.data());

    for (auto &entry : pipe_store) {
        delete entry;
    }

    // since the stageIndexOffset is calculated during writePipelineIndex
    // we have to fill vendor specific stage data here
    {
        VKSCPipelineCacheHeaderReader pcr(cache.size(), cache.data());

        for (const auto &pipeline_test_data : td.pipeline_test_datas_) {
            VkPipelineCacheSafetyCriticalIndexEntry const *pie = pcr.getPipelineIndexEntry(pipeline_test_data.uuid);

            size_t next_stage_vendor_data_offset =
                static_cast<size_t>(pie->stageIndexOffset + sizeof(VkPipelineCacheStageValidationIndexEntry));

            for (uint32_t stage_id = 0; stage_id < pipeline_test_data.stages.size(); stage_id++) {
                const auto &stage_data = pipeline_test_data.stages[stage_id];

                memcpy(cache.data() + next_stage_vendor_data_offset, stage_data.vendor_data.data(), stage_data.vendor_data.size());

                next_stage_vendor_data_offset += pie->stageIndexStride;
            }
        }
    }
}
