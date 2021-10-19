/* Copyright (c) 2021, NVIDIA CORPORATION
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Author: Daniel Koch <dkoch@nvidia.com>
 */


#include <cassert>
#include <cstddef>
#include <iostream>
#include <cstring>
#include <vector>

#include "pcreader.hpp"
#include "pcwriter.hpp"

#include <vulkan/vulkan_sc_core.hpp>

// Sanity check the structure sizes to ensure proper structure packing
static_assert(sizeof(VkPipelineCacheHeaderVersionOne) == 32,
              "VkPipelineCacheHeaderVersionOne has incorrect size");
static_assert(sizeof(VkPipelineCacheStageValidationIndexEntry) == 16,
              "VkPipelineCacheStageValidationIndexEntry has incorrect size");
static_assert(sizeof(VkPipelineCacheSafetyCriticalIndexEntry) == 56,
              "VkPipelineCacheSafetyCriticalIndexEntry has incorrect size");
static_assert(sizeof(VkPipelineCacheHeaderVersionSafetyCriticalOne) == 56,
              "VkPipelineCacheHeaderVersionSafetyCriticalOne has incorrect size");

#define PRINT_STRUCTSIZE(name) \
    std::cout << "size of " #name ": " \
              << sizeof(name) << std::endl;
#define PRINT_STRUCTMEMB(struc, memb) \
    std::cout << "{offset, size} of " #memb "\t\t: " \
        << offsetof(struc, memb) << ", " \
        << sizeof(struc().memb) \
        << std::endl;

void print_pipeline_layout()
{
    std::cout << "pipeline layout test" << std::endl;

    PRINT_STRUCTSIZE(VkPipelineCacheHeaderVersionOne);
    PRINT_STRUCTMEMB(VkPipelineCacheHeaderVersionOne, headerSize);
    PRINT_STRUCTMEMB(VkPipelineCacheHeaderVersionOne, headerVersion);
    PRINT_STRUCTMEMB(VkPipelineCacheHeaderVersionOne, vendorID);
    PRINT_STRUCTMEMB(VkPipelineCacheHeaderVersionOne, deviceID);
    PRINT_STRUCTMEMB(VkPipelineCacheHeaderVersionOne, pipelineCacheUUID);

    PRINT_STRUCTSIZE(VkPipelineCacheStageValidationIndexEntry);
    PRINT_STRUCTMEMB(VkPipelineCacheStageValidationIndexEntry, codeSize);
    PRINT_STRUCTMEMB(VkPipelineCacheStageValidationIndexEntry, codeOffset);

    PRINT_STRUCTSIZE(VkPipelineCacheSafetyCriticalIndexEntry);
    PRINT_STRUCTMEMB(VkPipelineCacheSafetyCriticalIndexEntry, pipelineIdentifier);
    PRINT_STRUCTMEMB(VkPipelineCacheSafetyCriticalIndexEntry, poolEntrySize);
    PRINT_STRUCTMEMB(VkPipelineCacheSafetyCriticalIndexEntry, jsonSize);
    PRINT_STRUCTMEMB(VkPipelineCacheSafetyCriticalIndexEntry, jsonOffset);
    PRINT_STRUCTMEMB(VkPipelineCacheSafetyCriticalIndexEntry, stageIndexCount);
    PRINT_STRUCTMEMB(VkPipelineCacheSafetyCriticalIndexEntry, stageIndexStride);
    PRINT_STRUCTMEMB(VkPipelineCacheSafetyCriticalIndexEntry, stageIndexOffset);

    PRINT_STRUCTSIZE(VkPipelineCacheHeaderVersionSafetyCriticalOne);
    PRINT_STRUCTMEMB(VkPipelineCacheHeaderVersionSafetyCriticalOne, headerVersionOne);
    PRINT_STRUCTMEMB(VkPipelineCacheHeaderVersionSafetyCriticalOne, validationVersion);
    PRINT_STRUCTMEMB(VkPipelineCacheHeaderVersionSafetyCriticalOne, pipelineIndexCount);
    PRINT_STRUCTMEMB(VkPipelineCacheHeaderVersionSafetyCriticalOne, pipelineIndexStride);
    PRINT_STRUCTMEMB(VkPipelineCacheHeaderVersionSafetyCriticalOne, pipelineIndexOffset);
}

std::ostream & operator<< (std::ostream &out, uint8_t i)
{
    out << static_cast<unsigned int>(i);
    return out;
}

std::ostream & operator<< (std::ostream &out, const uint8_t uuid[VK_UUID_SIZE])
{
    out << "{";
    for (uint32_t i=0; i<VK_UUID_SIZE; ++i)
    {
        if (i) out << ", ";
        out << uuid[i];
    }
    out << "}";
    return out;
}

std::ostream & operator<< (std::ostream &out, VkPipelineCacheHeaderVersionOne const &s)
{
    out << "{" << std::endl;
    out << s.headerSize << std::endl;
    out << s.headerVersion << std::endl;
    out << s.vendorID << std::endl;
    out << s.deviceID << std::endl;
    out << s.pipelineCacheUUID << std::endl;
    out << "}";
    return out;
}

std::ostream & operator<< (std::ostream &out, VkPipelineCacheHeaderVersionSafetyCriticalOne const &s)
{
    out << "{" << std::endl;
    out << s.headerVersionOne << std::endl;
    out << s.validationVersion << std::endl;
    out << s.pipelineIndexCount << std::endl;
    out << s.pipelineIndexStride << std::endl;
    out << s.pipelineIndexOffset << std::endl;
    out << "}";
    return out;
}

std::ostream & operator<< (std::ostream &out, VkPipelineCacheSafetyCriticalIndexEntry const &s)
{
    out << "{" << std::endl;
    out << "identifier " << s.pipelineIdentifier << std::endl;
    out << "poolEntrySize " << s.poolEntrySize << std::endl;
    out << "jsonSize " << s.jsonSize << std::endl;
    out << "jsonOffset " << s.jsonOffset << std::endl;
    out << "stageIndexCount " << s.stageIndexCount << std::endl;
    out << "stageIndexStride " << s.stageIndexStride << std::endl;
    out << "stageIndexOffset " << s.stageIndexOffset << std::endl;
    out << "}";
    return out;
}

std::ostream & operator<< (std::ostream &out, VkPipelineCacheStageValidationIndexEntry const &s)
{
    out << "{" << std::endl;
    out << s.codeSize << std::endl;
    out << s.codeOffset << std::endl;
    out << "}";
    return out;
}


int main(void)
{
    print_pipeline_layout();

    const uint64_t cacheSize = 10*1024*1024; // 10 MB
    std::vector<uint8_t> cache(cacheSize);

    uint8_t pcUuid[VK_UUID_SIZE]{0,1,2,3,4,5,6,7,8,9,0xa,0xb,0xc,0xd,0xe,0xf};
    VKSCPipelineCacheHeaderWriter pcw(0x10de,0xabcd,pcUuid);

    {
        const uint32_t testSize = 8;
        std::vector<std::string> jsonStore(testSize);
        std::vector<std::vector<uint8_t>> codeStore(testSize*2);
        std::vector<VKSCPipelineEntry *> pipeStore(testSize);
        pcw.allocatePipelineIndex(testSize);

        for (uint8_t i=0; i<testSize; i++)
        {
            uint8_t peUuid[VK_UUID_SIZE]{i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i};
            VKSCPipelineEntry *pe = new VKSCPipelineEntry(peUuid, 32+i*11);

            pe->allocateStages(2);

            std::string fakeJson("<json>Let's pretend this is a Json file </json>");
            jsonStore[i] = fakeJson;
            pe->setJsonCode(jsonStore[i].size(), reinterpret_cast<const uint8_t *>(jsonStore[i].data()));

            uint32_t code[8] {0x11111111, 0x22222222, 0x33333333, i,
                              0x55665566, 0x66666666, 0x77887788, 0x88998899};
            codeStore[i*2].resize(sizeof(code));
            memcpy(codeStore[i*2].data(),  reinterpret_cast<uint8_t *>(&code[0]), sizeof(code));

            pe->setShaderStageCode(0, codeStore[i*2].size(), codeStore[i*2].data());

            uint32_t code2[4] {0xdeaddead, 0xbeef7ac0, 0xfeedface, 2u*i};
            codeStore[i*2+1].resize(sizeof(code2));
            memcpy(codeStore[i*2+1].data(), reinterpret_cast<uint8_t *>(&code2[0]), sizeof(code2));
            pe->setShaderStageCode(1, codeStore[i*2+1].size(), codeStore[i*2+1].data());

            pipeStore[i] = pe;
            pcw.setPipelineEntry(i, pe);
        }

        pcw.writeHeaderSafetyCriticalOne(cacheSize, cache.data());
        uint64_t end = pcw.writePipelineIndex(cacheSize, cache.data());
        uint64_t pipelineIndexSize = pcw.getPipelineIndexSize();

        for (auto& entry : pipeStore)
        {
            delete entry;
        }
        std::cout << "pipeline index ends at " << end << " bytes" << std::endl;
        std::cout << "pipelineIndexSize is " << pipelineIndexSize << " bytes" << std::endl;
    }


    VKSCPipelineCacheHeaderReader pcr(cacheSize, cache.data());
    std::cout << "isValid = " << pcr.isValid() << std::endl;

    const VkPipelineCacheHeaderVersionSafetyCriticalOne * sc1 = pcr.getSafetyCriticalOneHeader();
    std::cout << "header = " << *sc1 << std::endl;

    // iterate over each pipeline and print the UUID
    for (uint32_t i=0; i < sc1->pipelineIndexCount; i++)
    {
        const VkPipelineCacheSafetyCriticalIndexEntry *pie = pcr.getPipelineIndexEntry(i);
        if (nullptr != pie)
        {
            std::cout << "pie " << i << ": " << pie->pipelineIdentifier << std::endl;
        } else {
            std::cout << "pie " << i << ": not found" << std::endl;
        }
    }

    // query for pipeline by UUID
    for (uint8_t i=0; i<8; i++)
    {
        uint8_t id = static_cast<uint8_t>(7-i);
        uint8_t queryUUID[VK_UUID_SIZE]{id,id,id,id,id,id,id,id,id,id,id,id,id,id,id,id};
        const VkPipelineCacheSafetyCriticalIndexEntry *pie = pcr.getPipelineIndexEntry(queryUUID);
        if (nullptr != pie)
        {
            std::cout << "pie " << id << ": " << *pie << std::endl;
        } else {
            std::cout << "pie " << id << ": not found" << std::endl;
        }
        const char *jsonPtr = reinterpret_cast<const char *>(pcr.getJson(pie));
        if (nullptr != jsonPtr)
        {
            std::string json(jsonPtr, static_cast<size_t>(pie->jsonSize));
            std::cout << "json: " << json << std::endl;
        }
        for (uint32_t stage=0; stage < pie->stageIndexCount; stage++)
        {
            const VkPipelineCacheStageValidationIndexEntry *sie = pcr.getStageIndexEntry(pie, stage);
            if (nullptr != sie)
            {
                std::cout << "sie " << stage << ": " << *sie << std::endl;
                const uint8_t *spirv = pcr.getSPIRV(sie);
                const uint32_t *code = reinterpret_cast<const uint32_t *>(spirv);
                for (uint32_t c=0; c<sie->codeSize/4; c++)
                {
                    std::cout << std::hex << code[c] << ", ";
                }
                std::cout << std::dec << std::endl;
            } else {
                std::cout << "sie " << stage << ": not found" << std::endl;
            }
        }
    }

    return 0;
}
