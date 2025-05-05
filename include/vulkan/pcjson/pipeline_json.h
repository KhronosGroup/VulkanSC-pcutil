#ifndef VKSC_PIPELINE_JSON_H_
#define VKSC_PIPELINE_JSON_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct VpjShaderFileName {
    int32_t stage;                      // VkShaderStageFlagBits identifying the shader stage
    const char* filename;
} VpjShaderFileName;

typedef struct VpjGraphicsPipelineState {
    const void* renderpass;             // Optional pointer to VkRenderPassCreateInfo pNext chain
    const void* renderpass2;            // Optional pointer to VkRenderPassCreateInfo2 pNext chain

    uint32_t ycbcrSamplerCount;
    const void** ycbcrSamplers;         // Optional array of pointers to VkSamplerYcbcrConversionCreateInfo pNext chains
    uint32_t immutableSamplerCount;
    const void** immutableSamplers;     // Optional array of pointers to VkSamplerCreateInfo pNext chains
    uint32_t descriptorSetLayoutCount;
    const void** descriptorSetLayouts;  // Optional array of pointers to VkDescriptorSetLayoutCreateInfo pNext chains
    const void* pipelineLayout;         // Pointer to VkPipelineLayoutCreateInfo pNext chain

    const void* graphicsPipeline;       // Pointer to VkGraphicsPipelineCreateInfo pNext chain

    uint32_t shaderFileNameCount;
    const VpjShaderFileName* shaderFileNames;
    const void* physicalDeviceFeatures; // Optional pointer to VkPhysicalDeviceFeatures2 pNext chain
} VpjGraphicsPipelineState;

typedef struct VpjComputePipelineState {
    uint32_t ycbcrSamplerCount;
    const void** ycbcrSamplers;         // Optional array of pointers to VkSamplerYcbcrConversionCreateInfo pNext chains
    uint32_t immutableSamplerCount;
    const void** immutableSamplers;     // Optional array of pointers to VkSamplerCreateInfo pNext chains
    uint32_t descriptorSetLayoutCount;
    const void** descriptorSetLayouts;  // Optional array of pointers to VkDescriptorSetLayoutCreateInfo pNext chains
    const void* pipelineLayout;         // Pointer to VkPipelineLayoutCreateInfo pNext chain

    const void* computePipeline;        // Pointer to VkComputePipelineCreateInfo pNext chain

    uint32_t shaderFileNameCount;
    const VpjShaderFileName* shaderFileNames;
    const void* physicalDeviceFeatures; // Optional pointer to VkPhysicalDeviceFeatures2 pNext chain
} VpjComputePipelineState;

typedef struct VpjData {
    const VpjGraphicsPipelineState* graphicsPipelineState;
    const VpjComputePipelineState* computePipelineState;
    uint8_t pipelineUUID[16];
    uint32_t deviceExtensionCount;
    const char** deviceExtensions;
} VpjData;

typedef void* VpjGenerator;

VpjGenerator vpjCreateGenerator();
bool vpjGeneratePipelineJson(VpjGenerator generator, const VpjData* pipeline_data, const char** pipeline_json, const char** messages);
void vpjDestroyGenerator(VpjGenerator generator);

typedef void* VpjParser;

VpjParser vpjCreateParser();
bool vpjParsePipelineJson(VpjParser parser, const char* pipeline_json, const VpjData** pipeline_data, const char** messages);
void vpjDestroyParser(VpjParser parser);

#endif  // VKSC_PIPELINE_JSON_H_
