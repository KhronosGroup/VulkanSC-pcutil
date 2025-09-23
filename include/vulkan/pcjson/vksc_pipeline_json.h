/*
 * Copyright (c) 2025 The Khronos Group Inc.
 * Copyright (c) 2025 RasterGrid Kft.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef VKSC_PIPELINE_JSON_H_
#define VKSC_PIPELINE_JSON_H_ 1

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct VpjShaderFileName {
    /// VkShaderStageFlagBits identifying the shader stage
    int32_t stage;
    const char* pFilename;
} VpjShaderFileName;

typedef struct VpjGraphicsPipelineState {
    /// Pointer to a VkGraphicsPipelineCreateInfo structure and its pNext chain
    const void* pGraphicsPipeline;

    /// Optional pointer to array of VkSamplerYcbcrConversionCreateInfo structures and their pNext chains
    uint32_t ycbcrSamplerCount;
    const void* pYcbcrSamplers;
    /// Optional pointer to array of names to assign to each VkSamplerYcbcrConversion object
    /// (for more details on their use during pipeline JSON generation and parsing, see the corresponding commands)
    const char** ppYcbrSamplerNames;
    /// Optional pointer to array of VkSamplerCreateInfo structures and their pNext chains
    uint32_t immutableSamplerCount;
    const void* pImmutableSamplers;
    /// Optional pointer to array of names to assign to each VkSampler object
    /// (for more details on their use during pipeline JSON generation and parsing, see the corresponding commands)
    const char** ppImmutableSamplerNames;
    /// Optional pointer to array of VkDescriptorSetLayoutCreateInfo structures and their pNext chains
    uint32_t descriptorSetLayoutCount;
    const void* pDescriptorSetLayouts;
    /// Optional pointer to array of names to assign to each VkDescriptorSetLayout object
    /// (for more details on their use during pipeline JSON generation and parsing, see the corresponding commands)
    const char** ppDescriptorSetLayoutNames;
    /// Pointer to a VkPipelineLayoutCreateInfo structure and its pNext chain
    const void* pPipelineLayout;

    uint32_t shaderFileNameCount;
    const VpjShaderFileName* pShaderFileNames;
    /// Optional pointer to a VkPhysicalDeviceFeatures2 structure and its pNext chain
    const void* pPhysicalDeviceFeatures;

    /// Pointer to a VkRenderPassCreateInfo or VkRenderPassCreateInfo2 structure and its pNext chain
    const void* pRenderPass;
} VpjGraphicsPipelineState;

typedef struct VpjComputePipelineState {
    /// Pointer to a VkComputePipelineCreateInfo structure and its pNext chain
    const void* pComputePipeline;

    /// Optional pointer to array of VkSamplerYcbcrConversionCreateInfo structures and their pNext chains
    uint32_t ycbcrSamplerCount;
    const void* pYcbcrSamplers;
    /// Optional pointer to array of names to assign to each VkSamplerYcbcrConversion object
    /// (for more details on their use during pipeline JSON generation and parsing, see the corresponding commands)
    const char** ppYcbrSamplerNames;
    /// Optional pointer to array of VkSamplerCreateInfo structures and their pNext chains
    uint32_t immutableSamplerCount;
    const void* pImmutableSamplers;
    /// Optional pointer to array of names to assign to each VkSampler object
    /// (for more details on their use during pipeline JSON generation and parsing, see the corresponding commands)
    const char** ppImmutableSamplerNames;
    /// Optional pointer to array of VkDescriptorSetLayoutCreateInfo structures and their pNext chains
    uint32_t descriptorSetLayoutCount;
    const void* pDescriptorSetLayouts;
    /// Optional pointer to array of names to assign to each VkDescriptorSetLayout object
    /// (for more details on their use during pipeline JSON generation and parsing, see the corresponding commands)
    const char** ppDescriptorSetLayoutNames;
    /// Pointer to a VkPipelineLayoutCreateInfo structure and its pNext chain
    const void* pPipelineLayout;

    uint32_t shaderFileNameCount;
    const VpjShaderFileName* pShaderFileNames;
    /// Optional pointer to a VkPhysicalDeviceFeatures2 structure and its pNext chain
    const void* pPhysicalDeviceFeatures;
} VpjComputePipelineState;

typedef struct VpjData {
    union {
        VpjGraphicsPipelineState graphicsPipelineState;
        VpjComputePipelineState computePipelineState;
    };
    uint8_t pipelineUUID[16];
    uint32_t enabledExtensionCount;
    const char** ppEnabledExtensions;
} VpjData;

typedef void* VpjGenerator;

/**
 * @brief Creates a pipeline JSON generator.
 * 
 * @return The handle of the created JSON generator or NULL if allocation failed
 */
VpjGenerator vpjCreateGenerator();

/**
 * @brief Controls whether the generator will generate pipeline UUIDs by constructing the MD5 hash of the input pipeline data.
 *
 * @param generator The JSON generator object
 * @param enable If true, MD5 pipeline UUID is enabled, otherwise it is disabled (by default it is disabled).
 */
void vpjSetMD5PipelineUUIDGeneration(VpjGenerator generator, bool enable);

/**
 * @brief Generates a pipeline JSON from the input pipeline data.
 *
 * @param generator The JSON generator object
 * @param pPipelineData Pointer to input pipeline data
 * @param ppPipelineJson Pointer to the output JSON string (the backing storage remains valid until vpjFreeGeneratorOutputs or
 * vpjDestroyGenerator is called)
 * @param ppMessages Optional pointer to the output messages (the backing storage remains valid until vpjFreeGeneratorOutputs,
 * vpjDestroyGenerator, or another vpjGenerate* command is called)
 * @return True, if the generation was successful, false otherwise.
 *
 * If the input VpjData specifies names for specific object types, then the handle members in create info structures
 * referencing these object types are expected to be in the [0..count) range, where count is the corresponding member
 * of the pipeline state structure specifying the number of create info structures for the object type in question.
 * During generation, these handle values (effectively indices) are used to look up the name array and will replace
 * the handle values with the specified names in the generated pipeline JSON.
 */
bool vpjGeneratePipelineJson(VpjGenerator generator, const VpjData* pPipelineData, const char** ppPipelineJson, const char** ppMessages);

/**
 * @brief Retrieves the generated pipeline UUID for the pipeline JSON produced by the last vpjGeneratePipelineJson call.
 *
 * @param generator The JSON generator object
 * @param pPipelineUUID Pointer to the pipeline UUID storage to return the generated pipeline UUID to.
 * @param ppMessages Optional pointer to the output messages (the backing storage remains valid until vpjFreeGeneratorOutputs,
 * vpjDestroyGenerator, or another vpjGenerate* command is called)
 * @return True, if retreival was successful, false otherwise.
 */
bool vpjGetGeneratedPipelineUUID(VpjGenerator generator, uint8_t* pPipelineUUID, const char** ppMessages);

/**
 * @brief Generates a JSON from a single input structure (including its pNext chain).
 *
 * Supported structure types:
 *  * VkGraphicsPipelineCreateInfo
 *  * VkComputePipelineCreateInfo
 *  * VkSamplerYcbcrConversionCreateInfo
 *  * VkSamplerCreateInfo
 *  * VkDescriptorSetLayoutCreateInfo
 *  * VkPipelineLayoutCreateInfo
 *  * VkPhysicalDeviceFeatures2
 *  * VkRenderPassCreateInfo
 *  * VkRenderPassCreateInfo2
 *  * VkShaderModuleCreateInfo
 *  * VkDeviceObjectReservationCreateInfo
 *  * VkPipelineOfflineCreateInfo
 *
 * @param generator The JSON generator object
 * @param pStruct Pointer to input structure
 * @param ppJson Pointer to the output JSON string (the backing storage remains valid until vpjFreeGeneratorOutputs or
 * vpjDestroyGenerator is called)
 * @param ppMessages Optional pointer to the output messages (the backing storage remains valid until vpjFreeGeneratorOutputs,
 * vpjDestroyGenerator, or another vpjGenerate* command is called)
 * @return True, if the generation was successful, false otherwise.
 */
bool vpjGenerateSingleStructJson(VpjGenerator generator, const void* pStruct, const char** ppJson, const char** ppMessages);

/**
 * @brief Returns the VkPhysicalDeviceFeatures2 pNext chain from a VkDeviceCreateInfo pNext chain.
 *
 * Used to filter out non-device-feature structs out when the VkPhysicalDeviceFeatures2 pNext chain is not readily available
 *
 * @param generator The JSON generator object
 * @param pDeviceCreateInfoPNext The input pNext chain pointer of the VkDeviceCreateInfo structure
 * @param ppPhysicalDeviceFeatures Pointer to the output VkPhysicalDeviceFeatures2 structure (the backing storage remains valid
 * until vpjFreeGeneratorOutputs or vpjDestroyGenerator is called)
 * @param ppMessages Optional pointer to the output messages (the backing storage remains valid until vpjFreeGeneratorOutputs,
 * vpjDestroyGenerator, or another vpjGenerate* command is called)
 * @return True, if the generation was successful, false otherwise.
 */
bool vpjFilterDeviceFeatures(VpjGenerator generator, const void* pDeviceCreateInfoPNext, const void** ppPhysicalDeviceFeatures,
                             const char** ppMessages);

/**
 * @brief Frees any storage of previous pipeline JSON generator outputs.
 * 
 * @param generator The JSON generator to free output storage for
 */
void vpjFreeGeneratorOutputs(VpjGenerator generator);

/**
 * @brief Destroys a pipeline JSON generator.
 * 
 * @param generator The JSON generator to destroy
 */
void vpjDestroyGenerator(VpjGenerator generator);

typedef void* VpjParser;

/**
 * @brief Creates a pipeline JSON parser.
 * 
 * @return The handle of the created JSON parser or NULL if allocation failed
 */
VpjParser vpjCreateParser();

/**
 * @brief Parses a pipeline JSON from the input JSON string.
 *
 * @param parser The JSON parser object
 * @param pPipelineJson The input JSON string
 * @param pPipelineData Pointer to the pointer output pipeline data (the backing storage of nested data remains valid until
 * vpjFreeParserOutputs or vpjDestroyParser is called)
 * @param ppMessages Optional pointer to the output messages (the backing storage remains valid until vpjFreeParserOutputs,
 * vpjDestroyParser, or another vpjParse* command is called)
 * @return True, if the parsing was successful, false otherwise.
 *
 * The pipeline JSON allows handle names to be arbitrary strings. Therefore the output VpjData will return these in
 * the name arrays corresponding to each object type. The handle values actually returned in the parsed API create
 * info structures therefore will always be indices into these name arrays taking values in the range [0..count),
 * where count is the corresponding member of the pipeline state structure containing the number of create info
 * structures for the object type in question.
 */
bool vpjParsePipelineJson(VpjParser parser, const char* pPipelineJson, VpjData* pPipelineData, const char** ppMessages);

/**
 * @brief Parses a single structure (including its pNext chain) from the input JSON string.
 *
 * Supported structure types:
 *  * VkGraphicsPipelineCreateInfo
 *  * VkComputePipelineCreateInfo
 *  * VkSamplerYcbcrConversionCreateInfo
 *  * VkSamplerCreateInfo
 *  * VkDescriptorSetLayoutCreateInfo
 *  * VkPipelineLayoutCreateInfo
 *  * VkPhysicalDeviceFeatures2
 *  * VkRenderPassCreateInfo
 *  * VkRenderPassCreateInfo2
 *  * VkShaderModuleCreateInfo
 *  * VkDeviceObjectReservationCreateInfo
 *  * VkPipelineOfflineCreateInfo
 *
 * @param parser The JSON parser object
 * @param pJson The input JSON string
 * @param pStruct Pointer to the output structure (the backing storage of nested data remains valid until vpjFreeParserOutputs
 * or vpjDestroyParser is called)
 * @param ppMessages Optional pointer to the output messages (the backing storage remains valid until vpjFreeParserOutputs,
 * vpjDestroyParser, or another vpjParse* command is called)
 * @return True, if the parsing was successful, false otherwise.
 */
bool vpjParseSingleStructJson(VpjParser parser, const char* pJson, void* pStruct, const char** ppMessages);

/**
 * @brief Frees any storage of previous pipeline JSON parser outputs.
 * 
 * @param parser The JSON parser to free output storage for
 */
void vpjFreeParserOutputs(VpjParser parser);

/**
 * @brief Destroys a pipeline JSON parser.
 * 
 * @param parser The JSON parser to destroy
 */
void vpjDestroyParser(VpjParser parser);

#ifdef __cplusplus
}
#endif

#endif  // VKSC_PIPELINE_JSON_H_
