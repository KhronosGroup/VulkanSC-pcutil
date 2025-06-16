/*
 * Copyright (c) 2025 The Khronos Group Inc.
 * Copyright (c) 2025 RasterGrid Kft.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <gtest/gtest.h>

#include "pcreader.hpp"

#include "pctest_writer_helper.h"

bool ReadAndValidate(VKSCPipelineCacheHeaderReader &pcr, std::vector<uint8_t> &cache, const PCWriterTestData &td) {
    uint32_t pipeline_entry_count_ = uint32_t(td.pipeline_test_datas_.size());

    uint32_t pipeline_entry_count = pcr.getPipelineIndexCount();
    EXPECT_TRUE(pipeline_entry_count == pipeline_entry_count_);

    EXPECT_TRUE(td.implementation_data_ == pcr.getImplementationData());

    EXPECT_TRUE(pcr.getPipelineIndexOffset() ==
                sizeof(VkPipelineCacheHeaderVersionSafetyCriticalOne) + td.global_vendor_data_.size());

    EXPECT_TRUE(memcmp(cache.data() + sizeof(VkPipelineCacheHeaderVersionSafetyCriticalOne), td.global_vendor_data_.data(),
                       td.global_vendor_data_.size()) == 0);

    for (uint32_t pipe_index = 0; pipe_index < pipeline_entry_count; pipe_index++) {
        VkPipelineCacheSafetyCriticalIndexEntry const *pie = pcr.getPipelineIndexEntry(pipe_index);
        EXPECT_TRUE(pie);
    }

    // query non existent pipeline entry by id
    {
        VkPipelineCacheSafetyCriticalIndexEntry const *pie = pcr.getPipelineIndexEntry(pipeline_entry_count);
        EXPECT_FALSE(pie);
    }

    for (uint32_t i = 0; i < pipeline_entry_count; i++) {
        uint32_t pipe_id = (i + 7) % pipeline_entry_count;

        VkPipelineCacheSafetyCriticalIndexEntry const *pie = pcr.getPipelineIndexEntry(td.pipeline_test_datas_[pipe_id].uuid);
        EXPECT_TRUE(pie);

        const uint8_t *pipeline_vendor_data_ptr =
            reinterpret_cast<const uint8_t *>(pie) + sizeof(VkPipelineCacheSafetyCriticalIndexEntry);
        EXPECT_TRUE(memcmp(pipeline_vendor_data_ptr, td.pipeline_test_datas_[pipe_id].vendor_data.data(),
                           td.per_pipeline_vendor_data_size_) == 0);

        char const *json_ptr = reinterpret_cast<char const *>(pcr.getJson(*pie));

        if (json_ptr) {
            const auto &td_json = td.pipeline_test_datas_[pipe_id].json;
            EXPECT_TRUE(pie->jsonSize == td_json.size());
            EXPECT_TRUE(memcmp(json_ptr, td_json.data(), pie->jsonSize) == 0);
        }

        uint32_t stage_index_count = pie->stageIndexCount;
        const auto &pipeline_stages = td.pipeline_test_datas_[pipe_id].stages;
        EXPECT_TRUE(stage_index_count == pipeline_stages.size());

        // get non existent stage entry
        EXPECT_FALSE(pcr.getStageIndexEntry(*pie, stage_index_count));

        for (uint32_t stage_id = 0; stage_id < stage_index_count; stage_id++) {
            const auto &stage_data = pipeline_stages[stage_id];
            VkPipelineCacheStageValidationIndexEntry const *sie = pcr.getStageIndexEntry(*pie, stage_id);
            EXPECT_TRUE(sie);

            EXPECT_TRUE(sie->codeSize == stage_data.code.size());

            uint8_t const *spirv = pcr.getSPIRV(*sie);
            EXPECT_TRUE(memcmp(spirv, stage_data.code.data(), sie->codeSize) == 0);

            const uint8_t *stage_vendor_data_ptr =
                reinterpret_cast<const uint8_t *>(sie) + sizeof(VkPipelineCacheStageValidationIndexEntry);
            EXPECT_TRUE(memcmp(stage_vendor_data_ptr, stage_data.vendor_data.data(), td.per_stage_vendor_data_size_) == 0);
        }
    }

    return true;
}

TEST(PCReaderWriterTest, Empty) {
    std::vector<uint8_t> cache(PCWriterTestData::default_cache_size_);

    PCWriterTestData test_data(__LINE__);

    {
        VKSCPipelineCacheHeaderWriter pcw(test_data.vendor_id_, test_data.device_id_, test_data.pipeline_cache_uuid_);
        WriteTestDataToCache(pcw, cache, test_data);
    }

    {
        VKSCPipelineCacheHeaderReader pcr(cache.size(), cache.data());

        EXPECT_TRUE(pcr.isValid());
        EXPECT_TRUE(ReadAndValidate(pcr, cache, test_data));
    }
}

TEST(PCReaderWriterTest, GlobalVendorDataOnly) {
    std::vector<uint8_t> cache(PCWriterTestData::default_cache_size_);

    PCWriterTestData test_data(__LINE__);
    test_data.GenGlobalVendorData(17);

    {
        VKSCPipelineCacheHeaderWriter pcw(test_data.vendor_id_, test_data.device_id_, test_data.pipeline_cache_uuid_);
        WriteTestDataToCache(pcw, cache, test_data);
    }

    {
        VKSCPipelineCacheHeaderReader pcr(cache.size(), cache.data());

        EXPECT_TRUE(pcr.isValid());
        EXPECT_TRUE(ReadAndValidate(pcr, cache, test_data));
    }
}

TEST(PCReaderWriterTest, OnePipeline) {
    std::vector<uint8_t> cache(PCWriterTestData::default_cache_size_);

    PCWriterTestData test_data(__LINE__);
    test_data.GenPipelineEntries(1);
    test_data.GenShaderStages(0, 2);

    {
        VKSCPipelineCacheHeaderWriter pcw(test_data.vendor_id_, test_data.device_id_, test_data.pipeline_cache_uuid_);
        WriteTestDataToCache(pcw, cache, test_data);
    }

    {
        VKSCPipelineCacheHeaderReader pcr(cache.size(), cache.data());

        EXPECT_TRUE(pcr.isValid());
        EXPECT_TRUE(ReadAndValidate(pcr, cache, test_data));
    }
}

TEST(PCReaderWriterTest, OnePipelineWithJson) {
    std::vector<uint8_t> cache(PCWriterTestData::default_cache_size_);

    PCWriterTestData test_data(__LINE__);
    test_data.GenPipelineEntries(1);
    test_data.GenShaderStages(0, 2);
    test_data.GenPipelineJsonData();

    {
        VKSCPipelineCacheHeaderWriter pcw(test_data.vendor_id_, test_data.device_id_, test_data.pipeline_cache_uuid_);
        WriteTestDataToCache(pcw, cache, test_data);
    }

    {
        VKSCPipelineCacheHeaderReader pcr(cache.size(), cache.data());

        EXPECT_TRUE(pcr.isValid());
        EXPECT_TRUE(ReadAndValidate(pcr, cache, test_data));
    }
}

TEST(PCReaderWriterTest, OnePipelineAndPipelineVendorData) {
    std::vector<uint8_t> cache(PCWriterTestData::default_cache_size_);

    PCWriterTestData test_data(__LINE__);
    test_data.GenPipelineEntries(1);
    test_data.GenShaderStages(0, 2);
    test_data.GenPipelineVendorData(17);

    {
        VKSCPipelineCacheHeaderWriter pcw(test_data.vendor_id_, test_data.device_id_, test_data.pipeline_cache_uuid_);
        WriteTestDataToCache(pcw, cache, test_data);
    }

    {
        VKSCPipelineCacheHeaderReader pcr(cache.size(), cache.data());

        EXPECT_TRUE(pcr.isValid());
        EXPECT_TRUE(ReadAndValidate(pcr, cache, test_data));
    }
}

TEST(PCReaderWriterTest, OnePipelineAndStageVendorData) {
    std::vector<uint8_t> cache(PCWriterTestData::default_cache_size_);

    PCWriterTestData test_data(__LINE__);
    test_data.GenPipelineEntries(1);
    test_data.GenShaderStages(0, 2);
    test_data.GenShaderStageVendorData(17);

    {
        VKSCPipelineCacheHeaderWriter pcw(test_data.vendor_id_, test_data.device_id_, test_data.pipeline_cache_uuid_);
        WriteTestDataToCache(pcw, cache, test_data);
    }

    {
        VKSCPipelineCacheHeaderReader pcr(cache.size(), cache.data());

        EXPECT_TRUE(pcr.isValid());
        EXPECT_TRUE(ReadAndValidate(pcr, cache, test_data));
    }
}

TEST(PCReaderWriterTest, MultiplePipelineVariousAmountOfShaderStage) {
    std::vector<uint8_t> cache(PCWriterTestData::default_cache_size_);

    PCWriterTestData test_data(__LINE__);
    test_data.GenPipelineEntries(5);
    test_data.GenShaderStages(0, 2);
    test_data.GenShaderStages(1, 1);
    test_data.GenShaderStages(2, 3);
    test_data.GenShaderStages(4, 5);
    test_data.GenPipelineJsonData();
    test_data.GenPipelineVendorData(19);
    test_data.GenShaderStageVendorData(17);
    test_data.GenGlobalVendorData(23);

    {
        VKSCPipelineCacheHeaderWriter pcw(test_data.vendor_id_, test_data.device_id_, test_data.pipeline_cache_uuid_);
        WriteTestDataToCache(pcw, cache, test_data);
    }

    {
        VKSCPipelineCacheHeaderReader pcr(cache.size(), cache.data());

        EXPECT_TRUE(pcr.isValid());
        EXPECT_TRUE(ReadAndValidate(pcr, cache, test_data));
    }
}

TEST(PCReaderWriterTest, MultiplePipelines) {
    std::vector<uint8_t> cache(PCWriterTestData::default_cache_size_);

    PCWriterTestData test_data(__LINE__);
    test_data.GenPipelineEntries(17, 11);

    {
        VKSCPipelineCacheHeaderWriter pcw(test_data.vendor_id_, test_data.device_id_, test_data.pipeline_cache_uuid_);
        WriteTestDataToCache(pcw, cache, test_data);
    }

    {
        VKSCPipelineCacheHeaderReader pcr(cache.size(), cache.data());

        EXPECT_TRUE(pcr.isValid());
        EXPECT_TRUE(ReadAndValidate(pcr, cache, test_data));
    }
}

TEST(PCReaderWriterTest, MultiplePipelinesAllData) {
    std::vector<uint8_t> cache(PCWriterTestData::default_cache_size_);

    PCWriterTestData test_data(__LINE__);
    test_data.GenPipelineEntries(17, 11);
    test_data.GenPipelineJsonData();
    test_data.GenPipelineVendorData(19);
    test_data.GenShaderStageVendorData(17);
    test_data.GenGlobalVendorData(23);

    {
        VKSCPipelineCacheHeaderWriter pcw(test_data.vendor_id_, test_data.device_id_, test_data.pipeline_cache_uuid_);
        WriteTestDataToCache(pcw, cache, test_data);
    }

    {
        VKSCPipelineCacheHeaderReader pcr(cache.size(), cache.data());

        EXPECT_TRUE(pcr.isValid());
        EXPECT_TRUE(ReadAndValidate(pcr, cache, test_data));
    }
}

TEST(PCReaderWriterTest, CustomShaderData) {
    std::vector<uint8_t> cache(PCWriterTestData::default_cache_size_);

    PCWriterTestData test_data(__LINE__);
    test_data.GenPipelineEntries(1);
    test_data.GenShaderStages(0, 2);

    std::vector<uint8_t> shader_code = {0xde, 0xad, 0xde, 0xad, 0xbe, 0xef, 0x7a, 0xc0};
    test_data.SetShaderStageCode(0, 1, shader_code);

    {
        VKSCPipelineCacheHeaderWriter pcw(test_data.vendor_id_, test_data.device_id_, test_data.pipeline_cache_uuid_);
        WriteTestDataToCache(pcw, cache, test_data);
    }

    {
        VKSCPipelineCacheHeaderReader pcr(cache.size(), cache.data());

        EXPECT_TRUE(pcr.isValid());
        EXPECT_TRUE(ReadAndValidate(pcr, cache, test_data));
    }
}

TEST(PCReaderWriterTest, CustomJson) {
    std::vector<uint8_t> cache(PCWriterTestData::default_cache_size_);

    PCWriterTestData test_data(__LINE__);
    test_data.GenPipelineEntries(1);
    test_data.GenShaderStages(0, 2);

    std::string payload = "Custom json data";
    test_data.SetJson(0, payload);

    {
        VKSCPipelineCacheHeaderWriter pcw(test_data.vendor_id_, test_data.device_id_, test_data.pipeline_cache_uuid_);
        WriteTestDataToCache(pcw, cache, test_data);
    }

    {
        VKSCPipelineCacheHeaderReader pcr(cache.size(), cache.data());

        EXPECT_TRUE(pcr.isValid());
        EXPECT_TRUE(ReadAndValidate(pcr, cache, test_data));
    }
}
