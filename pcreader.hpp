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
#define PCREADER_HPP 1

#include <cstddef>
#include <limits>

#ifndef VKSC_ASSERT
#include <cassert>
#define VKSC_ASSERT assert
#endif // VKSC_ASSERT
#ifndef VKSC_MEMCMP
#include <cstring>
#define VKSC_MEMCMP memcmp
#endif // VKSC_MEMCPY

// Must be version 1.0.6 or newer
#include <vulkan/vulkan_sc_core.hpp>

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
    VKSCPipelineCacheHeaderReader(uint64_t cacheSize, uint8_t const * cacheData)
        : m_CacheSize{cacheSize}, m_CacheData{cacheData} {}

    // basic sanity check of the referenced pipeline cache data
    // make sure m_CacheData starts with a well-formed VkPipelineCacheHeaderVersionSafetyCriticalOne structure
    bool isValid() const
    {
        VkPipelineCacheHeaderVersionSafetyCriticalOne const * const sc1 =
            reinterpret_cast<VkPipelineCacheHeaderVersionSafetyCriticalOne const *>(m_CacheData);

        if (sc1->headerVersionOne.headerSize != sizeof(VkPipelineCacheHeaderVersionSafetyCriticalOne) ||
            sc1->headerVersionOne.headerVersion != VK_PIPELINE_CACHE_HEADER_VERSION_SAFETY_CRITICAL_ONE ||
            sc1->validationVersion != VK_PIPELINE_CACHE_VALIDATION_VERSION_SAFETY_CRITICAL_ONE)
        {
            return false;
        }
        return true;
    }

    // return pointer to the VkPipelineCacheHeaderVersionOne structure
    VkPipelineCacheHeaderVersionOne const * getHeaderVersionOne() const
    {
        VkPipelineCacheHeaderVersionOne const * const hv1 =
            reinterpret_cast<VkPipelineCacheHeaderVersionOne const *>(m_CacheData);

        return hv1;
    }

    // return the validation version from the SC1 header
    VkPipelineCacheValidationVersion getValidationVersion() const
    {
        VkPipelineCacheHeaderVersionSafetyCriticalOne const * const sc1 = getSafetyCriticalOneHeader();
        return sc1->validationVersion;
    }

    // return the implementation data field from the SC1 header
    uint32_t getImplementationData() const
    {
        VkPipelineCacheHeaderVersionSafetyCriticalOne const * const sc1 = getSafetyCriticalOneHeader();
        return sc1->implementationData;
    }

    // return the number of pipelines in the index
    uint32_t getPipelineIndexCount() const
    {
        VkPipelineCacheHeaderVersionSafetyCriticalOne const * const sc1 = getSafetyCriticalOneHeader();
        return sc1->pipelineIndexCount;
    }

    // return the stride between pipeline index entries in the index
    uint32_t getPipelineIndexStride() const
    {
        VkPipelineCacheHeaderVersionSafetyCriticalOne const * const sc1 = getSafetyCriticalOneHeader();
        return sc1->pipelineIndexStride;
    }

    // returns the offset to the start of pipeline index entries in the cache
    uint64_t getPipelineIndexOffset() const
    {
        VkPipelineCacheHeaderVersionSafetyCriticalOne const * const sc1 = getSafetyCriticalOneHeader();
        return sc1->pipelineIndexOffset;
    }

    // return pointer to pipeline index entry by <index> in pipeline header
    // typically used for iterating over all pipelines in the cache
    // nullptr is returned if <index> is out of range
    VkPipelineCacheSafetyCriticalIndexEntry const * getPipelineIndexEntry(uint32_t index) const
    {
        if (m_CacheSize < sizeof(VkPipelineCacheSafetyCriticalIndexEntry))
        {
            return nullptr;
        }

        if (index >= getPipelineIndexCount())
        {
            return nullptr;
        }

        uint64_t offset = uint64_t{index} * getPipelineIndexStride();

        if (std::numeric_limits<uint64_t>::max() - getPipelineIndexOffset() < offset)
        {
            return nullptr;
        }

        offset += getPipelineIndexOffset();

        if (m_CacheSize - sizeof(VkPipelineCacheSafetyCriticalIndexEntry) < offset)
        {
            return nullptr;
        }

        VkPipelineCacheSafetyCriticalIndexEntry const * const pipelineIndexEntry =
            reinterpret_cast<VkPipelineCacheSafetyCriticalIndexEntry const *>(m_CacheData + offset);

        return pipelineIndexEntry;
    }

    // return pointer to pipeline index entry for requested pipeline identifier
    // nullptr is returned if not found
    VkPipelineCacheSafetyCriticalIndexEntry const * getPipelineIndexEntry(uint8_t const identifier[VK_UUID_SIZE]) const
    {
        uint32_t const pipelineIndexCount = getPipelineIndexCount();
        uint32_t const pipelineIndexStride = getPipelineIndexStride();
        uint64_t const pipelineIndexOffset = getPipelineIndexOffset();

        if (m_CacheSize < sizeof(VkPipelineCacheSafetyCriticalIndexEntry))
        {
            return nullptr;
        }

        uint64_t offset = pipelineIndexOffset;

        for (uint32_t i = 0U; i < pipelineIndexCount; ++i)
        {
            if (offset > m_CacheSize - sizeof(VkPipelineCacheSafetyCriticalIndexEntry))
            {
                return nullptr;
            }

            VkPipelineCacheSafetyCriticalIndexEntry const * const pipelineIndexEntry =
                reinterpret_cast<VkPipelineCacheSafetyCriticalIndexEntry const *>(m_CacheData + offset);

            if (VKSC_MEMCMP(identifier, pipelineIndexEntry->pipelineIdentifier, VK_UUID_SIZE) == 0)
            {
                return pipelineIndexEntry;
            }

            if (std::numeric_limits<uint64_t>::max() - offset < pipelineIndexStride)
            {
                return nullptr;
            }

            offset += pipelineIndexStride;
        }

        return nullptr;
    }

    // return pointer to json for a given pipeline index entry
    // nullptr is returned if not present
    uint8_t const * getJson(VkPipelineCacheSafetyCriticalIndexEntry const * const pipelineIndexEntry) const
    {
        uint64_t offset = pipelineIndexEntry->jsonOffset;
        
        if (0U == offset) 
        {
            return nullptr;
        }

        if ((m_CacheSize <= offset) ||
            (m_CacheSize - offset < pipelineIndexEntry->jsonSize))
        {
            return nullptr;
        }

        return (m_CacheData + offset);
    }

    // return pointer to stage validation index entry given a pipeline index entry <pipelineIndexEntry> and <stage>
    // nullptr is returned if not present
    VkPipelineCacheStageValidationIndexEntry const * getStageIndexEntry(VkPipelineCacheSafetyCriticalIndexEntry const * const pipelineIndexEntry, uint32_t stage) const
    {
        if (m_CacheSize < sizeof(VkPipelineCacheStageValidationIndexEntry))
        {
            return nullptr;
        }

        if (stage >= pipelineIndexEntry->stageIndexCount)
        {
            return nullptr;
        }

        uint64_t offset = uint64_t{stage} * pipelineIndexEntry->stageIndexStride;

        if (std::numeric_limits<uint64_t>::max() - offset < pipelineIndexEntry->stageIndexOffset)
        {
            return nullptr;
        }

        offset += pipelineIndexEntry->stageIndexOffset;

        if (m_CacheSize - sizeof(VkPipelineCacheStageValidationIndexEntry) < offset)
        {
            return nullptr;
        }

        VkPipelineCacheStageValidationIndexEntry const * const stageIndexEntry =
            reinterpret_cast<VkPipelineCacheStageValidationIndexEntry const *>(m_CacheData + offset);

        return stageIndexEntry;
    }

    // return pointer to spirv code in the pipeline cache for a given stage index entry
    // nullptr is returned if not present
    uint8_t const * getSPIRV(VkPipelineCacheStageValidationIndexEntry const * const stageIndexEntry) const
    {
        uint64_t offset = stageIndexEntry->codeOffset;
        if (0U == offset)
        {
            return nullptr;
        }

        if ((m_CacheSize < stageIndexEntry->codeSize) ||
            (m_CacheSize - stageIndexEntry->codeSize < offset))
        {
            return nullptr;
        }

        return (m_CacheData + offset);
    }

private:
    // return pointer to the pipeline cache SafetyCriticalOne structure
    VkPipelineCacheHeaderVersionSafetyCriticalOne const * getSafetyCriticalOneHeader() const
    {
        VkPipelineCacheHeaderVersionSafetyCriticalOne const * const sc1 =
            reinterpret_cast<VkPipelineCacheHeaderVersionSafetyCriticalOne const *>(m_CacheData);

        return sc1;
    }

    uint64_t const m_CacheSize;          // size of data pointed to by m_CacheData in bytes
    uint8_t const * const m_CacheData;   // pipeline cache data being read by this reader

};

#endif // PCREADER_HPP
