// Fake Vulkan SC Core header
// just enough to understand the pipeline cache header

#define VK_UUID_SIZE                      16U
typedef uint64_t VkDeviceSize;

typedef enum VkPipelineCacheHeaderVersion {
    VK_PIPELINE_CACHE_HEADER_VERSION_ONE = 1,
    VK_PIPELINE_CACHE_HEADER_VERSION_SAFETY_CRITICAL_ONE = 1000298000,
    VK_PIPELINE_CACHE_HEADER_VERSION_MAX_ENUM = 0x7FFFFFFF
} VkPipelineCacheHeaderVersion;

typedef struct VkPipelineCacheHeaderVersionOne {
    uint32_t                        headerSize;
    VkPipelineCacheHeaderVersion    headerVersion;
    uint32_t                        vendorID;
    uint32_t                        deviceID;
    uint8_t                         pipelineCacheUUID[VK_UUID_SIZE];
} VkPipelineCacheHeaderVersionOne;

typedef enum VkPipelineCacheValidationVersion {
    VK_PIPELINE_CACHE_VALIDATION_VERSION_SAFETY_CRITICAL_ONE = 1,
    VK_PIPELINE_CACHE_VALIDATION_VERSION_MAX_ENUM = 0x7FFFFFFF
} VkPipelineCacheValidationVersion;

typedef struct VkPipelineCacheStageValidationIndexEntry {
    uint64_t    codeSize;
    uint64_t    codeOffset;
} VkPipelineCacheStageValidationIndexEntry;

typedef struct VkPipelineCacheSafetyCriticalIndexEntry {
    uint8_t     pipelineIdentifier[VK_UUID_SIZE];
    uint64_t    poolEntrySize;
    uint64_t    jsonSize;
    uint64_t    jsonOffset;
    uint32_t    stageIndexCount;
    uint32_t    stageIndexStride;
    uint64_t    stageIndexOffset;
} VkPipelineCacheSafetyCriticalIndexEntry;

typedef struct VkPipelineCacheHeaderVersionSafetyCriticalOne {
    VkPipelineCacheHeaderVersionOne     headerVersionOne;
    VkPipelineCacheValidationVersion    validationVersion;
    uint32_t                            pipelineIndexCount;
    uint32_t                            pipelineIndexStride;
    uint64_t                            pipelineIndexOffset;
} VkPipelineCacheHeaderVersionSafetyCriticalOne;


