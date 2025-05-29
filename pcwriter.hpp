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

#ifndef PCWRITER_HPP
#define PCWRITER_HPP 1

#include <algorithm>
#include <cstddef>
#include <cstdint>

#ifndef VKSC_ASSERT
#include <cassert>
#define VKSC_ASSERT assert
#endif // VKSC_ASSERT
#ifndef VKSC_MEMCPY
#include <cstring>
#define VKSC_MEMCPY memcpy
#endif // VKSC_MEMCPY

#include <vulkan/vulkan_sc_core.hpp>

// VKSCStageEntry
//
// Utility class to encapsulate a single stage entry.
//
// Usage:
//   - Instantiate the class
//   - call setShaderCode to store the SPIR-V code for this stage entry.
//   - (optional) call getStageEntryExtraSize to determine how much cache storage is required for this stage entry
//   - call writeStageEntry to store the shader entry in the pipeline cache blob
//
// The SPIR-V code is NOT copied and the application must maintain the lifetime
// of the data that was passed in while this object is instantiated.
// The SPIR-V code will never be modified by this class.
//
class VKSCStageEntry
{
public:
    // initialize the stage entry with no code
    VKSCStageEntry(): m_SpirvSize(0U), m_SpirvCode(nullptr) {}

    // disable copy and assignment
    VKSCStageEntry(VKSCStageEntry const& rhs) = delete; // copy constructor
    VKSCStageEntry(VKSCStageEntry&& rhs) = delete; // move constructor
    VKSCStageEntry& operator=(VKSCStageEntry const& rhs) = delete; // copy assignment
    VKSCStageEntry& operator=(VKSCStageEntry&& rhs) = delete; // move assignment

    // set the SPIR-V code pointer and size for this stage entry.
    // <size> bytes of SPIR-V starting at <code>.
    // the SPIR-V code is not copied, but the pointer is saved.
    // The data pointed to by <code> is never modified by this class.
    void setShaderCode(uint64_t size, uint8_t const * const code)
    {
        m_SpirvSize = size;
        m_SpirvCode = code;
    }

    // param: <size> is the amount of memory in bytes for the pipeline cache memory
    // param: <data> is pointer to the beginning of the pipeline cache memory
    // param: <entryoffset> is the offset in bytes from <data> at which to store the stage index entry.
    // param: <extraoffset> is the offset in bytes from <data> at which to store the SPIR-V for this stage.
    // operation:
    //  - writes the stage entry information as a VkPipelineCacheStageValidationIndexEntry structure in <data> at <entryOffset>
    //  - store the spirv code starting at <extraOffset>
    // return: the advanced value of <extraOffset>
    // precondition: the memory at [data,data+size) is writeable for the pipeline cache
    uint64_t writeStageEntry(uint64_t size, uint8_t *data, uint64_t entryOffset, uint64_t extraOffset) const
    {
        (void)size;
        VkPipelineCacheStageValidationIndexEntry *entry = reinterpret_cast<VkPipelineCacheStageValidationIndexEntry *>(data+entryOffset);
        VKSC_ASSERT(size > entryOffset + sizeof(VkPipelineCacheStageValidationIndexEntry));

        VkPipelineCacheStageValidationIndexEntry tmpEntry {};

        uint64_t const codeSize = m_SpirvSize;
        if (codeSize > 0U)
        {
            tmpEntry.codeSize = codeSize;

            VKSC_ASSERT(size > extraOffset + codeSize);
            uint8_t *code = data + extraOffset;
            VKSC_MEMCPY(code, m_SpirvCode, static_cast<size_t>(codeSize));

            tmpEntry.codeOffset = extraOffset;
            extraOffset += codeSize;
        } else
        {
            tmpEntry.codeSize = 0U;
            tmpEntry.codeOffset = 0U;
        }

        VKSC_MEMCPY(entry, &tmpEntry, static_cast<size_t>(sizeof(tmpEntry)));
        return extraOffset;
    }

    // return: the amount of space in bytes required for the associated data (code)
    // for this stage entry.
    uint64_t getStageEntryExtraSize()
    {
        return m_SpirvSize;
    }

private:
    uint64_t m_SpirvSize;
    uint8_t const * m_SpirvCode;
};

// VKSCPipelineEntry
//
// Class to encapsulate pipeline information for a single pipeline cache entry.
//
// Usage:
//   - Instantiate the class with the pipeline identifier and required poolSize.
//   - call allocateStages to allocate storage for per-stage entries (if being stored)
//   - call setShaderStageCode to set the SPIR-V code for each stage of this pipeline (if being stored)
//   - call setJsonCode to set the json code for this pipeline (if being stored)
//   - (optional) call getPipelineEntrySize to determine how much cache storage is required for this pipeline entry
//   - call writePipelineEntry to store the pipeline entry and any associated data in the pipeline cache blob
//
// The SPIR-V and JSON code is NOT copied and the application must maintain the lifetime
// of the data that was passed in while this object is instantiated.
// The SPIR-V and JSON code will never be modified by this class.
//
class VKSCPipelineEntry
{
public:
    // initialize a pipeline entry with the pipeline <identifier> and required <memorySize>
    VKSCPipelineEntry(uint8_t const identifier[VK_UUID_SIZE], uint64_t memorySize) : m_MemorySize(memorySize)
    {
        VKSC_MEMCPY(m_Identifier, identifier, VK_UUID_SIZE);
    }

    // disable copy and assignment
    VKSCPipelineEntry(VKSCPipelineEntry const& rhs) = delete; // copy constructor
    VKSCPipelineEntry(VKSCPipelineEntry&& rhs) = delete; // move constructor
    VKSCPipelineEntry& operator=(VKSCPipelineEntry const& rhs) = delete; // copy assignment
    VKSCPipelineEntry& operator=(VKSCPipelineEntry&& rhs) = delete; // move assignment

    // destructor - delete any memory this class allocated
    ~VKSCPipelineEntry()
    {
        if (m_Stages)
        {
            delete [] m_Stages;
        }
    }

    // allocate storage for <stageCount> stage entries
    // this function can only be called once
    // return: true if memory allocated, false otherwise
    bool allocateStages(uint32_t stageCount)
    {
        if (m_Stages) return false;

        m_StageCount = stageCount;
        m_Stages = new VKSCStageEntry[m_StageCount];
        return true;
    }

    // store the <size> and a pointer to the SPIR-V <code> for stage <stage>
    // in the pipline entry.
    // Shader stages must be stored in the order they are provided to pipeline creation.
    // The SPIR-V code is NOT copied and the application must maintain the lifetime
    // of the data that was passed in while this object is instantiated.
    // The data pointed to by <code> is never modified by this class.
    // precondition: <stage> must be less than the value that was passed to allocateStages
    void setShaderStageCode(uint32_t stage, uint64_t size, uint8_t const * code)
    {
        VKSC_ASSERT(stage < m_StageCount);
        m_Stages[stage].setShaderCode(size, code);
    }

    // store the <size> and a pointer to the <json> data in the pipeline cache entry.
    // The json code is NOT copied and the application must maintain the lifetime
    // of the data that was passed in while this object is instantiated.
    // The data pointed to by <json> is never modified by this class.
    void setJsonCode(uint64_t size, uint8_t const * json)
    {
        m_JsonSize = size;
        m_JsonPointer = json;
    }

    // param: <size> is the amount of memory in bytes for the pipeline cache memory
    // param: <data> is pointer to the beginning of the pipeline cache memory
    // param: <entryoffset> is the offset in bytes from <data> at which to store the pipeline index entry.
    // param: <extraoffset> is the offset in bytes from <data> at which to store the SPIR-V for this stage.
    // param: <stageStride> is the stride between successive entries in the stage index.
    // operation:
    //  - writes the pipeline entry as a VkPipelineCacheSafetyCriticalIndexEntry structure in <data> at <entryOffset>
    //  - store any json and stage info starting at <extraOffset>
    // return: the advanced value of <extraOffset>
    // precondition: the memory at [data,data+size) is writeable for the pipeline cache
    uint64_t writePipelineEntry(uint64_t size, uint8_t *data, uint64_t entryOffset, uint64_t extraOffset, uint32_t const stageStride) const
    {
        VkPipelineCacheSafetyCriticalIndexEntry *entry = reinterpret_cast<VkPipelineCacheSafetyCriticalIndexEntry *>(data+entryOffset);
        VKSC_ASSERT(size > entryOffset + sizeof(VkPipelineCacheSafetyCriticalIndexEntry));

        VkPipelineCacheSafetyCriticalIndexEntry tmpEntry {};

        VKSC_MEMCPY(tmpEntry.pipelineIdentifier, m_Identifier, VK_UUID_SIZE);
        tmpEntry.pipelineMemorySize = m_MemorySize;

        // write optional json
        uint64_t const jsonSize = m_JsonSize;
        if (jsonSize > 0U)
        {
            tmpEntry.jsonSize = jsonSize;

            VKSC_ASSERT(size > extraOffset + jsonSize);
            uint8_t *json = data + extraOffset;
            VKSC_MEMCPY(json, m_JsonPointer, static_cast<size_t>(jsonSize));

            tmpEntry.jsonOffset = extraOffset;

            extraOffset += jsonSize;
        } else
        {
            tmpEntry.jsonSize = 0U;
            tmpEntry.jsonOffset = 0U;
        }

        // write the stageIndex
        uint32_t const stageCount = m_StageCount;
        if (stageCount > 0U)
        {
            uint64_t indexSize = stageCount * stageStride;
            VKSC_ASSERT(size > extraOffset + indexSize);
            uint64_t currentOffset = extraOffset;

            tmpEntry.stageIndexCount = stageCount;
            tmpEntry.stageIndexStride = stageStride;
            tmpEntry.stageIndexOffset = currentOffset;
            extraOffset += indexSize;

            for (uint32_t i = 0U; i < stageCount; ++i)
            {
                extraOffset = m_Stages[i].writeStageEntry(size, data, currentOffset, extraOffset);
                currentOffset += stageStride;
            }
            VKSC_ASSERT(currentOffset == tmpEntry.stageIndexOffset + indexSize);
        } else
        {
            tmpEntry.stageIndexCount = 0U;
            tmpEntry.stageIndexStride = 0U;
            tmpEntry.stageIndexOffset = 0U;
        }

        VKSC_MEMCPY(entry, &tmpEntry, static_cast<size_t>(sizeof(tmpEntry)));
        return extraOffset;
    }

    // param: <stageStride> is the stride between successive entries in the stage index.
    // return: the amount of space in bytes required for the additional data for this pipeline entry,
    // includes json, stage index, and all associated stage data (code).
    uint64_t getPipelineEntrySize(uint32_t const stageStride)
    {
        uint64_t stageIndexSize = 0U;

        uint32_t const stageCount = m_StageCount;
        if (stageCount > 0U)
        {
            stageIndexSize = stageCount * stageStride;
            uint64_t extraSize = 0U;

            for (uint32_t i = 0U; i < stageCount; ++i)
            {
                extraSize += m_Stages[i].getStageEntryExtraSize();
            }
            stageIndexSize += extraSize;
        }

        return m_JsonSize + stageIndexSize;
    }


private:
    uint8_t         m_Identifier[VK_UUID_SIZE];
    uint64_t const  m_MemorySize;
    uint64_t        m_JsonSize{0U};
    uint8_t const * m_JsonPointer{nullptr};
    uint32_t        m_StageCount{0U};
    VKSCStageEntry* m_Stages{nullptr};
};

// VKSCPipelineCacheHeaderWriter
//
// Utility class to collect pipeline cache entries and utlity to write
// the Vulkan Safety Critical header and pipeline cache entries into a
// pipeline cache blob.
//
// Usage:
//   - Instantiate the class with the vendorID, deviceID, and pipelineCacheUUID
//     or Instantiate the the class with default constructor
//        and call setVendorID, setDeviceID, and setPipelineCacheUUID
//   - call allocatePipelineIndex to allocate storage for pointers to pipeline entries
//   - call setPipelineEntry for each pipeline entry to be stored in the pipeline cache
//   - (optional) call setImplementationData to store implementation-specific data in the pipeline index
//   - (optional) call setPipelineIndexStride to override the default stride for the pipeline index
//   - (optional) call setPipelineIndexOffset to override the default offset for the pipeline index
//   - (optional) call setStageIndexStride to override the default stride for the stage entry index
//   - (optional) call getPipelineIndexSize to determine how much cache storage is required for the
//     pipeline index and associated data
//   - call writeHeaderSafetyCriticalOne to write the header at the start of the pipeline cache blob
//   - call writePipelineIndex to write the pipline index and all associated data starting at
//     the previously set offset


// setPipelineIndexStride - set the stride between successive entries in the pipeline index.
//   defaults to size of VkPipelineCacheSafetyCriticalIndexEntry.
//
// setPipelineIndexOffset - set the offset into the cache blob where the pipelne index should be written
//   defaults to immediately after the pipeline cache header.
//
// setStageIndexStride - set the stride between successive entries in the per-pipeline stage index
//   defaults to size of VkPipelineCacheStageValidationIndexEntry.
//
// writeHeaderSafetyCriticalOne - write the safety critical header at the start of the cache blob
//
// writePipelineIndex - write the pipeline index and all associated data starting at the previously set offset
//

class VKSCPipelineCacheHeaderWriter
{
public:
    // initialize a pipeline cache header writer
    // caller must explicitly set vendorID, deviceID, pipelineCacheUUID before writing the header
    VKSCPipelineCacheHeaderWriter() {}

    // initialize a pipeline cache header writer with the given <vendorID>, <deviceID>, and <pipelineCacheUUID>
    VKSCPipelineCacheHeaderWriter(uint32_t vendorID, uint32_t deviceID, uint8_t const pipelineCacheUUID[VK_UUID_SIZE])
        : m_VendorID(vendorID), m_DeviceID(deviceID)
    {
        VKSC_MEMCPY(m_PipelineCacheUUID, pipelineCacheUUID, VK_UUID_SIZE);
    }

    // destructor - delete any memory this class allocated
    ~VKSCPipelineCacheHeaderWriter()
    {
        if (m_PipelineIndex)
        {
            delete [] m_PipelineIndex;
        }
    }

    // allocate storage for <pipelineCount> pointers for pipeline index entries
    // this function can only be called once
    // return: true if memory allocated, false otherwise
    bool allocatePipelineIndex(uint32_t pipelineCount)
    {
        if (m_PipelineIndex || m_PipelineCount > 0U) return false;

        m_PipelineCount = pipelineCount;
        m_PipelineIndex = new VKSCPipelineEntry*[m_PipelineCount];
        return true;
    }

    // save a pointer to the pipeline <entry> at the specified <index> in the pipeline index
    // The pipeline entry is NOT copied and the application must maintain the lifetime
    // of the data that was passed in while this object is instantiated.
    // The data pointed to by <entry> is never modified by this class.
    // precondition: <index> must be less than the value that was passed to allocatePipelineIndex
    void setPipelineEntry(uint32_t index, VKSCPipelineEntry* entry)
    {
        VKSC_ASSERT(index < m_PipelineCount && m_PipelineIndex);
        m_PipelineIndex[index] = entry;
    }

    // set the stride in bytes between successive entries in the pipeline index.
    // By default this is equal to size of VkPipelineCacheSafetyCriticalIndexEntry
    // If set by the user, it must be no smaller than this.
    // If the user wishes to store additional per-pipeline metadata in the index, the stride should be increased accordingly.
    void setPipelineIndexStride(uint32_t stride)
    {
        m_PipelineIndexStride = std::max(static_cast<uint32_t>(sizeof(VkPipelineCacheSafetyCriticalIndexEntry)), stride);
    }

    // set the offset in bytes into the pipeline cache where the pipeline index should be written.
    // By default it is written immediately following the header.
    // If set by the user, it must be be after the header.
    // If the user wishes to store implementation-specific data before the pipeline index, this offset should be
    // set to the location following the implementation-specific data.
    void setPipelineIndexOffset(uint32_t offset)
    {
        VKSC_ASSERT(offset > sizeof(VkPipelineCacheHeaderVersionSafetyCriticalOne));
        m_PipelineIndexOffset = offset;
    }

    // set the stride in bytes between successive entries in the stage index
    // By default this is equal to the size of VkPipelineCacheStageValidationIndexEntry
    // If set by the user, it must be no smaller than this.
    // If the user wishes to store additional per-stage metadata in the index, the stride should be increased accordingly.
    void setStageIndexStride(uint32_t stride)
    {
        m_StageIndexStride = std::max(static_cast<uint32_t>(sizeof(VkPipelineCacheStageValidationIndexEntry)), stride);
    }

    // set the vendorID
    void setVendorID(uint32_t vendorID)
    {
        m_VendorID = vendorID;
    }

    // set the deviceID
    void setDeviceID(uint32_t deviceID)
    {
         m_DeviceID = deviceID;
    }

    // set the pipelineCacheUUID
    void setPipelineCacheUUID(uint8_t const pipelineCacheUUID[VK_UUID_SIZE])
    {
        VKSC_MEMCPY(m_PipelineCacheUUID, pipelineCacheUUID, VK_UUID_SIZE);
    }

    // set the implementationData field
    void setImplementationData(uint32_t implementationData)
    {
         m_ImplementationData = implementationData;
    }

    // write the Safety Critical header at the the beginning of <data>
    // param: <size> is the amount of memory in bytes for the pipeline cache memory
    // param: <data> is pointer to the beginning of the pipeline cache memory
    // precondition: the memory at [data,data+size) is writeable for the pipeline cache
    void writeHeaderSafetyCriticalOne(uint64_t size, uint8_t *data) const
    {
        (void)size;
        VKSC_ASSERT(size > sizeof(VkPipelineCacheHeaderVersionSafetyCriticalOne));
        VkPipelineCacheHeaderVersionSafetyCriticalOne *sc1 = reinterpret_cast<VkPipelineCacheHeaderVersionSafetyCriticalOne *>(data);
        sc1->headerVersionOne.headerSize    = sizeof(VkPipelineCacheHeaderVersionSafetyCriticalOne);
        sc1->headerVersionOne.headerVersion = VK_PIPELINE_CACHE_HEADER_VERSION_SAFETY_CRITICAL_ONE;
        sc1->headerVersionOne.vendorID      = m_VendorID;
        sc1->headerVersionOne.deviceID      = m_DeviceID;
        VKSC_MEMCPY(sc1->headerVersionOne.pipelineCacheUUID, m_PipelineCacheUUID, VK_UUID_SIZE);
        sc1->validationVersion = VK_PIPELINE_CACHE_VALIDATION_VERSION_SAFETY_CRITICAL_ONE;
        sc1->implementationData = m_ImplementationData;
        sc1->pipelineIndexCount = m_PipelineCount;
        sc1->pipelineIndexStride = m_PipelineIndexStride;
        sc1->pipelineIndexOffset = m_PipelineIndexOffset;
    }

    // write the Pipeline Index and all associated data at mPipelineIndexOffset bytes into <data>
    // param: <size> is the amount of memory in bytes for the pipeline cache memory
    // param: <data> is pointer to the beginning of the pipeline cache memory
    // returns the offset in bytes into <data> which immediately follows the written information
    // precondition: the memory at [data,data+size) is writeable for the pipeline cache
    // No implementation-specific per-pipeline or per-stage metadata is written, but space is reserved if
    // setPipelineIndexStride and/or setStageIndexStride were called appropriately.
    uint64_t writePipelineIndex(uint64_t size, uint8_t *data) const
    {
        uint64_t indexSize = m_PipelineCount * m_PipelineIndexStride;
        uint64_t extraOffset = m_PipelineIndexOffset + indexSize;
        VKSC_ASSERT(size > extraOffset);

        uint64_t currentOffset = m_PipelineIndexOffset;
        for (uint32_t i = 0U; i < m_PipelineCount; ++i)
        {
            extraOffset = m_PipelineIndex[i]->writePipelineEntry(size, data, currentOffset, extraOffset, m_StageIndexStride);
            currentOffset += m_PipelineIndexStride;
        }

        return extraOffset;
    }

    // return: the amount of space in bytes required for the pipeline index and all associated data.
    // includes pipeline index and all associated pipeline entry data (json, stage index, stage code).
    // Does NOT include the size of the initial VkPipelineCacheHeaderVersionSafetyCriticalOne header.
    uint64_t getPipelineIndexSize() const
    {
        uint64_t indexSize = m_PipelineCount * m_PipelineIndexStride;
        uint64_t extraSize = 0U;

        for (uint32_t i = 0U; i < m_PipelineCount; ++i)
        {
            extraSize += m_PipelineIndex[i]->getPipelineEntrySize(m_StageIndexStride);
        }

        return indexSize + extraSize;
    }

private:
    uint32_t            m_VendorID              {0U};
    uint32_t            m_DeviceID              {0U};
    uint8_t             m_PipelineCacheUUID[VK_UUID_SIZE]{0U};
    uint32_t            m_ImplementationData    {0U};

    uint32_t            m_PipelineIndexStride  {sizeof(VkPipelineCacheSafetyCriticalIndexEntry)};
    uint64_t            m_PipelineIndexOffset  {sizeof(VkPipelineCacheHeaderVersionSafetyCriticalOne)};
    uint32_t            m_StageIndexStride     {sizeof(VkPipelineCacheStageValidationIndexEntry)};

    uint32_t            m_PipelineCount        {0U};
    VKSCPipelineEntry** m_PipelineIndex        {nullptr};
};

#endif // PCWRITER_HPP
