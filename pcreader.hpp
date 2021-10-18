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

#ifndef PCREADER_HPP
#define PCREADER_HPP

#ifndef VKSC_ASSERT
#define VKSC_ASSERT assert
#endif // VKSC_ASSERT
#ifndef VKSC_MEMCMP
#define VKSC_MEMCMP memcmp
#endif // VKSC_MEMCPY


// VKSCPipelineCacheHeaderReader
//
// Utility class to handle extracting information about pipelines from a pipeline cache blob.
//
// Instantiate the class with a pointer to the pipeline cache blob and the size.
// The pipeline cache blob is NOT copied and the application must maintain the lifetime
// of the data that was passed in while this object is instantiated.
// The cache blob will never be modified by this class.
//
// getSafetyCriticalOneHeader - return the safety critical header field
//
// getPipelineIndexEntry(index) - return the pipeline index entry for a specified index in the header
//
// getPipelineIndexEntry(UUID) - return the pipeline index entry for a specified pipeline identifier
//
// getJson - get a pointer to the json for a specfied pipeline index entry
//
// getStageIndexEntry - return the stage index entry for a specified pipeline index entry and stage
//
// getSPIRV - get a pointer to the SPIRV code for a specified stage index entry
//

class VKSCPipelineCacheHeaderReader
{
public:
    // initialize the pipeline cache header reader with <cacheSize> bytes of data starting at <cacheData>
    // the pipeline cache is not copied, but the pointer is saved
    // cacheData is never modified
    VKSCPipelineCacheHeaderReader(uint64_t cacheSize, const uint8_t* cacheData): m_CacheSize{cacheSize}, m_CacheData{cacheData} {}

    // basic sanity check of the referenced pipeline cache data
    // make sure m_CacheData starts with a well-formed VkPipelineCacheHeaderVersionSafetyCriticalOne structure
    bool isValid() const
    {
        const VkPipelineCacheHeaderVersionSafetyCriticalOne* const sc1 =
            reinterpret_cast<const VkPipelineCacheHeaderVersionSafetyCriticalOne* const>(m_CacheData);

        if (sc1->headerVersionOne.headerSize != sizeof(VkPipelineCacheHeaderVersionSafetyCriticalOne) ||
            sc1->headerVersionOne.headerVersion != VK_PIPELINE_CACHE_HEADER_VERSION_SAFETY_CRITICAL_ONE ||
            sc1->validationVersion != VK_PIPELINE_CACHE_VALIDATION_VERSION_SAFETY_CRITICAL_ONE)
        {
            return false;
        }
        return true;
    }

    // return pointer to the pipeline cache SafetyCriticalOne structure
    const VkPipelineCacheHeaderVersionSafetyCriticalOne* getSafetyCriticalOneHeader() const
    {
        const VkPipelineCacheHeaderVersionSafetyCriticalOne* const sc1 =
            reinterpret_cast<const VkPipelineCacheHeaderVersionSafetyCriticalOne* const>(m_CacheData);

        return sc1;
    }

    // return pointer to pipeline index entry by <index> in pipeline header
    // typically used for iterating over all pipelines in the cache
    // nullptr is returned if <index> is out of range
    const VkPipelineCacheSafetyCriticalIndexEntry* getPipelineIndexEntry(uint32_t index) const
    {
        const VkPipelineCacheHeaderVersionSafetyCriticalOne* const sc1 = getSafetyCriticalOneHeader();

        if (index >= sc1->pipelineIndexCount)
        {
            return nullptr;
        }

        uint64_t offset = sc1->pipelineIndexOffset + (index * sc1->pipelineIndexStride);
        VKSC_ASSERT(offset + sizeof(VkPipelineCacheSafetyCriticalIndexEntry) <= m_CacheSize);

        const VkPipelineCacheSafetyCriticalIndexEntry* const pipelineIndexEntry =
            reinterpret_cast<const VkPipelineCacheSafetyCriticalIndexEntry* const>(m_CacheData + offset);

        return pipelineIndexEntry;
    }

    // return pointer to pipeline index entry for requested pipeline identifier
    // nullptr is returned if not found
    const VkPipelineCacheSafetyCriticalIndexEntry* getPipelineIndexEntry(const uint8_t identifier[VK_UUID_SIZE]) const
    {
        const VkPipelineCacheHeaderVersionSafetyCriticalOne* const sc1 = getSafetyCriticalOneHeader();

        for (uint32_t i = 0U; i < sc1->pipelineIndexCount; ++i)
        {
            uint64_t offset = sc1->pipelineIndexOffset + (i * sc1->pipelineIndexStride);
            VKSC_ASSERT(offset + sizeof(VkPipelineCacheSafetyCriticalIndexEntry) <= m_CacheSize);

            const VkPipelineCacheSafetyCriticalIndexEntry* const pipelineIndexEntry =
                reinterpret_cast<const VkPipelineCacheSafetyCriticalIndexEntry* const>(m_CacheData + offset);

            if (VKSC_MEMCMP(identifier, pipelineIndexEntry->pipelineIdentifier, VK_UUID_SIZE) == 0U)
            {
                return pipelineIndexEntry;
            }
        }

        return nullptr;
    }

    // return pointer to json for a given pipeline index entry
    // nullptr is returned if not present
    const uint8_t* getJson(const VkPipelineCacheSafetyCriticalIndexEntry* const pipelineIndexEntry) const
    {
        uint64_t offset = pipelineIndexEntry->jsonOffset;
        if (0U == offset) return nullptr;

        VKSC_ASSERT(offset + pipelineIndexEntry->jsonSize <= m_CacheSize);

        return (m_CacheData + offset);
    }

    // return pointer to stage validation index entry given a pipeline index entry <pipelineIndexEntry> and <stage>
    // nullptr is returned if not present
    const VkPipelineCacheStageValidationIndexEntry* getStageIndexEntry(const VkPipelineCacheSafetyCriticalIndexEntry* const pipelineIndexEntry, uint32_t stage) const
    {
        if (stage >= pipelineIndexEntry->stageIndexCount) return nullptr;

        uint64_t offset = pipelineIndexEntry->stageIndexOffset + (stage * pipelineIndexEntry->stageIndexStride);
        VKSC_ASSERT(offset + sizeof(VkPipelineCacheStageValidationIndexEntry) <= m_CacheSize);

        const VkPipelineCacheStageValidationIndexEntry* const stageIndexEntry =
            reinterpret_cast<const VkPipelineCacheStageValidationIndexEntry* const>(m_CacheData + offset);

        return stageIndexEntry;
    }

    // return pointer to spirv code in the pipeline cache for a given stage index entry
    // nullptr is returned if not present
    const uint8_t* getSPIRV(const VkPipelineCacheStageValidationIndexEntry* const stageIndexEntry) const
    {
        uint64_t offset = stageIndexEntry->codeOffset;
        if (0U == offset) return nullptr;

        VKSC_ASSERT(offset + stageIndexEntry->codeSize <= m_CacheSize);

        return (m_CacheData + offset);
    }

private:

    const uint64_t m_CacheSize;          // size of data pointed to by m_CacheData in bytes
    const uint8_t* const m_CacheData;    // pipeline cache data being read by this reader

};

#endif // PCREADER_HPP
