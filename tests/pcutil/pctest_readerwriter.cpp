#include <gtest/gtest.h>

#include "pcreader.hpp"
#include "pcwriter.hpp"

#include <vulkan/vulkan_sc.h>

struct TestData
{
    TestData(uint32_t seed) :
        seed_(seed)
    {
    }

    void GenPipelineEntries(uint32_t pipeline_entry_count, uint32_t shader_stage_count = 0)
    {
        pipeline_test_datas_.resize(pipeline_entry_count);

        for (uint32_t i = 0; i < pipeline_entry_count; i++)
        {
            uint32_t uuid[VK_UUID_SIZE / 4]{i, 0x55665566, 0xbeef7ac0, 0xfeedface};
            memcpy(pipeline_test_datas_[i].uuid, uuid, VK_UUID_SIZE);

            GenShaderStages(0, shader_stage_count);
        }
    }

    void GenPipelineJsonData()
    {
        for (uint32_t i = 0; i < pipeline_test_datas_.size(); i++)
        {
            std::stringstream ss;
            ss << "{ 'msg' : 'Simple json playload #" << i << "' }";
            pipeline_test_datas_[i].json = ss.str();
        }
    }

    void GenPipelineVendorData(uint32_t vendor_data_size)
    {
        per_pipeline_vendor_data_size_ = vendor_data_size;

        for (uint32_t i = 0; i < pipeline_test_datas_.size(); i++)
        {
            pipeline_test_datas_[i].vendor_data.resize(per_pipeline_vendor_data_size_);
            for (uint32_t j = 0; j < pipeline_test_datas_[i].vendor_data.size(); j++)
            {
                pipeline_test_datas_[i].vendor_data[j] = (seed_ + i + sample_payload[i % sizeof(sample_payload)]) % 0xff;
            }
        }
    }

    void GenShaderStages(uint32_t pipeline_entry_id, uint32_t shader_stage_count)
    {
        assert(pipeline_entry_id < pipeline_test_datas_.size());
        auto &pipeline_stages = pipeline_test_datas_[pipeline_entry_id].stages;

        pipeline_stages.resize(shader_stage_count);

        for (uint32_t i = 0; i < shader_stage_count; i++)
        {
            uint32_t code[8]{0x11111111, 0x22222222, 0x33333333, i, 0x55665566, 0x66666666, 0x77887788, 0x88998899};
            pipeline_stages[i].code.resize(sizeof(code));
            memcpy(pipeline_stages[i].code.data(), reinterpret_cast<uint8_t *>(&code[0]), sizeof(code));
        }
    }

    void GenShaderStageVendorData(uint32_t vendor_data_size)
    {
        per_stage_vendor_data_size_ = vendor_data_size;

        for (auto &pipeline_test_data : pipeline_test_datas_)
        {
            auto &pipeline_stages = pipeline_test_data.stages;

            for (uint32_t i = 0; i < pipeline_stages.size(); i++)
            {
                pipeline_stages[i].vendor_data.resize(per_stage_vendor_data_size_);
                for (uint32_t j = 0; j < pipeline_stages[i].vendor_data.size(); j++)
                {
                    pipeline_stages[i].vendor_data[j] = (seed_ + 17 * i + j + sample_payload[j % sizeof(sample_payload)]) % 0xff;
                }
            }
        }
    }

    void GenGlobalVendorData(size_t size)
    {
        global_vendor_data_.resize(size);
        for (uint32_t i = 0; i < global_vendor_data_.size(); i++)
        {
            global_vendor_data_[i] = (seed_ + sample_payload[i % sizeof(sample_payload)]) % 0xff;
        }
    }

    void SetJson(uint32_t pipeline_entry_id, const std::string &payload)
    {
        pipeline_test_datas_[pipeline_entry_id].json = payload;
    }

    void SetImplementationData(uint32_t implementation_data)
    {
        implementation_data_ = implementation_data;
    }

    void SetShaderStageCode(uint32_t pipeline_entry_id, uint32_t shader_stage_id, const std::vector<uint8_t> &code)
    {
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

const uint8_t TestData::sample_payload[8] = {0xde, 0xad, 0xde, 0xad, 0xbe, 0xef, 0x7a, 0xc0 };


void Write(VKSCPipelineCacheHeaderWriter &pcw, std::vector<uint8_t> &cache, const TestData &td)
{
    uint32_t pipeline_entry_count_ = uint32_t(td.pipeline_test_datas_.size());

    std::vector<VKSCPipelineEntry *> pipe_store(pipeline_entry_count_);
    pcw.allocatePipelineIndex(pipeline_entry_count_);

    pcw.setImplementationData(td.implementation_data_);

    uint32_t pipeline_index_offset = uint32_t(sizeof(VkPipelineCacheHeaderVersionSafetyCriticalOne) + td.global_vendor_data_.size());
    if (td.global_vendor_data_.size()) {
        pcw.setPipelineIndexOffset(pipeline_index_offset);
    }

    memcpy(cache.data() + sizeof(VkPipelineCacheHeaderVersionSafetyCriticalOne), td.global_vendor_data_.data(), td.global_vendor_data_.size());

    uint32_t pipeline_index_stride = uint32_t(sizeof(VkPipelineCacheSafetyCriticalIndexEntry) + td.per_pipeline_vendor_data_size_);
    pcw.setPipelineIndexStride(pipeline_index_stride);
    
    uint32_t stage_index_stride = uint32_t(sizeof(VkPipelineCacheSafetyCriticalIndexEntry) + td.per_stage_vendor_data_size_);
    pcw.setStageIndexStride(stage_index_stride);

    size_t next_pipeline_vendor_data_offset = pipeline_index_offset + sizeof(VkPipelineCacheSafetyCriticalIndexEntry);

    for (uint32_t i = 0; i < pipeline_entry_count_; i++) {
        VKSCPipelineEntry *pe = new VKSCPipelineEntry(td.pipeline_test_datas_[i].uuid, 32 + i * 11);
        auto &pipeline_stages = td.pipeline_test_datas_[i].stages;

        if (td.pipeline_test_datas_[i].json.size())
        {
            pe->setJsonCode(td.pipeline_test_datas_[i].json.size(), reinterpret_cast<uint8_t const *>(td.pipeline_test_datas_[i].json.data()));
        }

        memcpy(cache.data() + next_pipeline_vendor_data_offset, td.pipeline_test_datas_[i].vendor_data.data(), td.per_pipeline_vendor_data_size_);

        pe->allocateStages(uint32_t(pipeline_stages.size()));

        for (uint32_t stage_id = 0; stage_id < pipeline_stages.size(); stage_id++)
        {
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

        for (const auto& pipeline_test_data : td.pipeline_test_datas_)
        {
            VkPipelineCacheSafetyCriticalIndexEntry const *pie = pcr.getPipelineIndexEntry(pipeline_test_data.uuid);

            size_t next_stage_vendor_data_offset = pie->stageIndexOffset + sizeof(VkPipelineCacheStageValidationIndexEntry);

            for (uint32_t stage_id = 0; stage_id < pipeline_test_data.stages.size(); stage_id++)
            {
                const auto& stage_data = pipeline_test_data.stages[stage_id];

                memcpy(cache.data() + next_stage_vendor_data_offset, stage_data.vendor_data.data(), stage_data.vendor_data.size());

                next_stage_vendor_data_offset += pie->stageIndexStride;
            }
        }

    }
}


bool ReadAndValidate(VKSCPipelineCacheHeaderReader &pcr, std::vector<uint8_t> &cache, const TestData &td)
{
    uint32_t pipeline_entry_count_ = uint32_t(td.pipeline_test_datas_.size());

    uint32_t pipeline_entry_count = pcr.getPipelineIndexCount();
    EXPECT_TRUE(pipeline_entry_count == pipeline_entry_count_);

    EXPECT_TRUE(td.implementation_data_ == pcr.getImplementationData());

    EXPECT_TRUE(pcr.getPipelineIndexOffset() == sizeof(VkPipelineCacheHeaderVersionSafetyCriticalOne) + td.global_vendor_data_.size());

    EXPECT_TRUE(memcmp(cache.data() + sizeof(VkPipelineCacheHeaderVersionSafetyCriticalOne),
                    td.global_vendor_data_.data(), td.global_vendor_data_.size()) == 0);

    for (uint32_t pipe_index = 0; pipe_index < pipeline_entry_count; pipe_index++)
    {
        VkPipelineCacheSafetyCriticalIndexEntry const *pie = pcr.getPipelineIndexEntry(pipe_index);
        EXPECT_TRUE(pie);
    }

    // query non existent pipeline entry by id
    {
        VkPipelineCacheSafetyCriticalIndexEntry const *pie = pcr.getPipelineIndexEntry(pipeline_entry_count);
        EXPECT_FALSE(pie);
    }

    for (uint32_t i = 0; i < pipeline_entry_count; i++)
    {
        uint32_t pipe_id = (i + 7) % pipeline_entry_count;

        VkPipelineCacheSafetyCriticalIndexEntry const *pie = pcr.getPipelineIndexEntry(td.pipeline_test_datas_[pipe_id].uuid);
        EXPECT_TRUE(pie);

        const uint8_t* pipeline_vendor_data_ptr = reinterpret_cast<const uint8_t*>(pie) + sizeof(VkPipelineCacheSafetyCriticalIndexEntry);
        EXPECT_TRUE(memcmp(pipeline_vendor_data_ptr, td.pipeline_test_datas_[pipe_id].vendor_data.data(), td.per_pipeline_vendor_data_size_) == 0);

        char const* json_ptr = reinterpret_cast<char const *>(pcr.getJson(*pie));

        if (json_ptr)
        {
            const auto& td_json = td.pipeline_test_datas_[pipe_id].json;
            EXPECT_TRUE(pie->jsonSize == td_json.size());
            EXPECT_TRUE(memcmp(json_ptr, td_json.data(), pie->jsonSize) == 0);
        }

        uint32_t stage_index_count = pie->stageIndexCount;
        const auto &pipeline_stages = td.pipeline_test_datas_[pipe_id].stages;
        EXPECT_TRUE(stage_index_count == pipeline_stages.size());

        // get non existent stage entry
        EXPECT_FALSE(pcr.getStageIndexEntry(*pie, stage_index_count));

        for (uint32_t stage_id = 0; stage_id < stage_index_count; stage_id++)
        {
            const auto& stage_data = pipeline_stages[stage_id];
            VkPipelineCacheStageValidationIndexEntry const *sie = pcr.getStageIndexEntry(*pie, stage_id);
            EXPECT_TRUE(sie);

            EXPECT_TRUE(sie->codeSize == stage_data.code.size());

            uint8_t const *spirv = pcr.getSPIRV(*sie);
            EXPECT_TRUE(memcmp(spirv, stage_data.code.data(), sie->codeSize) == 0);

            const uint8_t* stage_vendor_data_ptr = reinterpret_cast<const uint8_t*>(sie) + sizeof(VkPipelineCacheStageValidationIndexEntry);
            EXPECT_TRUE(memcmp(stage_vendor_data_ptr, stage_data.vendor_data.data(), td.per_stage_vendor_data_size_) == 0);
        }
    }

    return true;
}


TEST(PCReaderWriterTest, Empty)
{
    std::vector<uint8_t> cache(TestData::default_cache_size_);

    TestData test_data(__LINE__);

    {
        VKSCPipelineCacheHeaderWriter pcw(test_data.vendor_id_, test_data.device_id_, test_data.pipeline_cache_uuid_);
        Write(pcw, cache, test_data);
    }

    {
        VKSCPipelineCacheHeaderReader pcr(cache.size(), cache.data());

        EXPECT_TRUE(pcr.isValid());
        EXPECT_TRUE(ReadAndValidate(pcr, cache, test_data));
    }
}


TEST(PCReaderWriterTest, GlobalVendorDataOnly)
{
    std::vector<uint8_t> cache(TestData::default_cache_size_);

    TestData test_data(__LINE__);
    test_data.GenGlobalVendorData(17);

    {
        VKSCPipelineCacheHeaderWriter pcw(test_data.vendor_id_, test_data.device_id_, test_data.pipeline_cache_uuid_);
        Write(pcw, cache, test_data);
    }

    {
        VKSCPipelineCacheHeaderReader pcr(cache.size(), cache.data());

        EXPECT_TRUE(pcr.isValid());
        EXPECT_TRUE(ReadAndValidate(pcr, cache, test_data));
    }
}


TEST(PCReaderWriterTest, OnePipeline)
{
    std::vector<uint8_t> cache(TestData::default_cache_size_);

    TestData test_data(__LINE__);
    test_data.GenPipelineEntries(1);
    test_data.GenShaderStages(0, 2);

    {
        VKSCPipelineCacheHeaderWriter pcw(test_data.vendor_id_, test_data.device_id_, test_data.pipeline_cache_uuid_);
        Write(pcw, cache, test_data);
    }

    {
        VKSCPipelineCacheHeaderReader pcr(cache.size(), cache.data());

        EXPECT_TRUE(pcr.isValid());
        EXPECT_TRUE(ReadAndValidate(pcr, cache, test_data));
    }
}


TEST(PCReaderWriterTest, OnePipelineWithJson)
{
    std::vector<uint8_t> cache(TestData::default_cache_size_);

    TestData test_data(__LINE__);
    test_data.GenPipelineEntries(1);
    test_data.GenShaderStages(0, 2);
    test_data.GenPipelineJsonData();

    {
        VKSCPipelineCacheHeaderWriter pcw(test_data.vendor_id_, test_data.device_id_, test_data.pipeline_cache_uuid_);
        Write(pcw, cache, test_data);
    }

    {
        VKSCPipelineCacheHeaderReader pcr(cache.size(), cache.data());

        EXPECT_TRUE(pcr.isValid());
        EXPECT_TRUE(ReadAndValidate(pcr, cache, test_data));
    }
}


TEST(PCReaderWriterTest, OnePipelineAndPipelineVendorData)
{
    std::vector<uint8_t> cache(TestData::default_cache_size_);

    TestData test_data(__LINE__);
    test_data.GenPipelineEntries(1);
    test_data.GenShaderStages(0, 2);
    test_data.GenPipelineVendorData(17);

    {
        VKSCPipelineCacheHeaderWriter pcw(test_data.vendor_id_, test_data.device_id_, test_data.pipeline_cache_uuid_);
        Write(pcw, cache, test_data);
    }

    {
        VKSCPipelineCacheHeaderReader pcr(cache.size(), cache.data());

        EXPECT_TRUE(pcr.isValid());
        EXPECT_TRUE(ReadAndValidate(pcr, cache, test_data));
    }
}


TEST(PCReaderWriterTest, OnePipelineAndStageVendorData)
{
    std::vector<uint8_t> cache(TestData::default_cache_size_);

    TestData test_data(__LINE__);
    test_data.GenPipelineEntries(1);
    test_data.GenShaderStages(0, 2);
    test_data.GenShaderStageVendorData(17);

    {
        VKSCPipelineCacheHeaderWriter pcw(test_data.vendor_id_, test_data.device_id_, test_data.pipeline_cache_uuid_);
        Write(pcw, cache, test_data);
    }

    {
        VKSCPipelineCacheHeaderReader pcr(cache.size(), cache.data());

        EXPECT_TRUE(pcr.isValid());
        EXPECT_TRUE(ReadAndValidate(pcr, cache, test_data));
    }
}


TEST(PCReaderWriterTest, MultiplePipelineVariousAmountOfShaderStage)
{
    std::vector<uint8_t> cache(TestData::default_cache_size_);

    TestData test_data(__LINE__);
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
        Write(pcw, cache, test_data);
    }

    {
        VKSCPipelineCacheHeaderReader pcr(cache.size(), cache.data());

        EXPECT_TRUE(pcr.isValid());
        EXPECT_TRUE(ReadAndValidate(pcr, cache, test_data));
    }
}


TEST(PCReaderWriterTest, MultiplePipelines)
{
    std::vector<uint8_t> cache(TestData::default_cache_size_);

    TestData test_data(__LINE__);
    test_data.GenPipelineEntries(17, 11);

    {
        VKSCPipelineCacheHeaderWriter pcw(test_data.vendor_id_, test_data.device_id_, test_data.pipeline_cache_uuid_);
        Write(pcw, cache, test_data);
    }

    {
        VKSCPipelineCacheHeaderReader pcr(cache.size(), cache.data());

        EXPECT_TRUE(pcr.isValid());
        EXPECT_TRUE(ReadAndValidate(pcr, cache, test_data));
    }
}


TEST(PCReaderWriterTest, MultiplePipelinesAllData)
{
    std::vector<uint8_t> cache(TestData::default_cache_size_);

    TestData test_data(__LINE__);
    test_data.GenPipelineEntries(17, 11);
    test_data.GenPipelineJsonData();
    test_data.GenPipelineVendorData(19);
    test_data.GenShaderStageVendorData(17);
    test_data.GenGlobalVendorData(23);

    {
        VKSCPipelineCacheHeaderWriter pcw(test_data.vendor_id_, test_data.device_id_, test_data.pipeline_cache_uuid_);
        Write(pcw, cache, test_data);
    }

    {
        VKSCPipelineCacheHeaderReader pcr(cache.size(), cache.data());

        EXPECT_TRUE(pcr.isValid());
        EXPECT_TRUE(ReadAndValidate(pcr, cache, test_data));
    }
}


TEST(PCReaderWriterTest, CustomShaderData)
{
    std::vector<uint8_t> cache(TestData::default_cache_size_);

    TestData test_data(__LINE__);
    test_data.GenPipelineEntries(1);
    test_data.GenShaderStages(0, 2);

    std::vector<uint8_t> shader_code = {0xde, 0xad, 0xde, 0xad, 0xbe, 0xef, 0x7a, 0xc0 };
    test_data.SetShaderStageCode(0, 1, shader_code);

    {
        VKSCPipelineCacheHeaderWriter pcw(test_data.vendor_id_, test_data.device_id_, test_data.pipeline_cache_uuid_);
        Write(pcw, cache, test_data);
    }

    {
        VKSCPipelineCacheHeaderReader pcr(cache.size(), cache.data());

        EXPECT_TRUE(pcr.isValid());
        EXPECT_TRUE(ReadAndValidate(pcr, cache, test_data));
    }
}


TEST(PCReaderWriterTest, CustomJson)
{
    std::vector<uint8_t> cache(TestData::default_cache_size_);

    TestData test_data(__LINE__);
    test_data.GenPipelineEntries(1);
    test_data.GenShaderStages(0, 2);

    std::string payload = "Custom json data";
    test_data.SetJson(0, payload);

    {
        VKSCPipelineCacheHeaderWriter pcw(test_data.vendor_id_, test_data.device_id_, test_data.pipeline_cache_uuid_);
        Write(pcw, cache, test_data);
    }

    {
        VKSCPipelineCacheHeaderReader pcr(cache.size(), cache.data());

        EXPECT_TRUE(pcr.isValid());
        EXPECT_TRUE(ReadAndValidate(pcr, cache, test_data));
    }
}

