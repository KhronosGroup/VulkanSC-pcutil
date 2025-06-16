/*
 * Copyright (c) 2024-2025 The Khronos Group Inc.
 * Copyright (c) 2024-2025 RasterGrid Kft.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <spirv-tools/libspirv.h>
#include <memory>
#include <vector>
#include <unordered_set>

inline std::vector<uint32_t> CompileSPV(const char* spv, const spv_target_env target_env = SPV_ENV_VULKAN_1_2) {
    std::vector<uint32_t> output{};

    spv_binary binary;
    spv_diagnostic diagnostic = nullptr;
    spv_context context = spvContextCreate(target_env);
    spv_result_t error = spvTextToBinaryWithOptions(context, spv, strlen(spv), 0, &binary, &diagnostic);
    spvContextDestroy(context);
    if (error) {
        spvDiagnosticPrint(diagnostic);
        spvDiagnosticDestroy(diagnostic);
        return output;
    }

    output.insert(output.end(), binary->code, binary->code + binary->wordCount);
    spvBinaryDestroy(binary);

    return output;
}

[[maybe_unused]] static char const kSampleComputePipelineJson[] = R"json(
    {
        "ComputePipelineState": {
            "YcbcrSamplers": [],
            "ImmutableSamplers": [],
            "DescriptorSetLayouts": [],
            "PipelineLayout": {
                "sType": "VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO",
                "pNext": "NULL",
                "flags": 0,
                "setLayoutCount": 0,
                "pSetLayouts": "NULL",
                "pushConstantRangeCount": 0,
                "pPushConstantRanges": "NULL"
            },
            "ComputePipeline": {
                "sType": "VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO",
                "pNext": "NULL",
                "flags": "0",
                "stage": {
                    "sType": "VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO",
                    "pNext": "NULL",
                    "flags": "0",
                    "stage": "VK_SHADER_STAGE_COMPUTE_BIT",
                    "module": "",
                    "pName": "main",
                    "pSpecializationInfo": "NULL"
                },
                "layout": "",
                "basePipelineHandle": "",
                "basePipelineIndex": 0
            }
        }
    }
)json";

[[maybe_unused]] static char const kSampleGraphicsPipelineJson[] = R"json(
    {
        "GraphicsPipelineState": {
            "Renderpass": {
                "sType": "VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO",
                "pNext":"NULL",
                "flags": "0",
                "attachmentCount": 0,
                "pAttachments": [],
                "subpassCount": 1,
                "pSubpasses": [
                    {
                        "flags": "0",
                        "pipelineBindPoint": "VK_PIPELINE_BIND_POINT_GRAPHICS",
                        "inputAttachmentCount": 0,
                        "pInputAttachments": "NULL",
                        "colorAttachmentCount" : 0,
                        "pColorAttachments": "NULL",
                        "pResolveAttachments": "NULL",
                        "pDepthStencilAttachment": "NULL",
                        "preserveAttachmentCount": 0,
                        "pPreserveAttachments": "NULL"
                    }
                ],
                "dependencyCount": 0,
                "pDependencies": "NULL"
            },
            "YcbcrSamplers": [],
            "ImmutableSamplers": [],
            "DescriptorSetLayouts": [],
            "PipelineLayout": {
                "sType": "VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO",
                "pNext": "NULL",
                "flags": 0,
                "setLayoutCount": 0,
                "pSetLayouts": "NULL",
                "pushConstantRangeCount": 0,
                "pPushConstantRanges": "NULL"
            },
            "GraphicsPipeline": {
                "sType": "VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO",
                "pNext": "NULL",
                "flags": "0",
                "stageCount": 2,
                "pStages": [
                    {
                        "sType": "VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO",
                        "pNext": "NULL",
                        "flags": "0",
                        "stage": "VK_SHADER_STAGE_VERTEX_BIT",
                        "module": "",
                        "pName": "main",
                        "pSpecializationInfo": "NULL"
                    },
                    {
                        "sType": "VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO",
                        "pNext": "NULL",
                        "flags": "0",
                        "stage": "VK_SHADER_STAGE_FRAGMENT_BIT",
                        "module": "",
                        "pName": "main",
                        "pSpecializationInfo": "NULL"
                    }
                ],
                "pVertexInputState": "NULL",
                "pInputAssemblyState": {
                    "sType": "VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO",
                    "pNext": "NULL",
                    "flags": 0,
                    "topology": "VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST",
                    "primitiveRestartEnable": "VK_FALSE"
                },
                "pTessellationState": "NULL",
                "pViewportState": {
                    "sType": "VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO",
                    "pNext": "NULL",
                    "flags": 0,
                    "viewportCount": 1,
                    "pViewports": "NULL",
                    "scissorCount": 1,
                    "pScissors": "NULL"
                },
                "pRasterizationState": {
                    "sType": "VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO",
                    "pNext":"NULL",
                    "flags": 0,
                    "depthClampEnable": "VK_FALSE",
                    "rasterizerDiscardEnable": "VK_FALSE",
                    "polygonMode": "VK_POLYGON_MODE_FILL",
                    "cullMode": "0",
                    "frontFace": "VK_FRONT_FACE_COUNTER_CLOCKWISE",
                    "depthBiasEnable": "VK_FALSE",
                    "depthBiasConstantFactor" : 0,
                    "depthBiasClamp": 0,
                    "depthBiasSlopeFactor" : 0,
                    "lineWidth": 1
                },
                "pMultisampleState": {
                    "sType": "VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO",
                    "pNext": "NULL",
                    "flags": 0,
                    "rasterizationSamples": "VK_SAMPLE_COUNT_1_BIT",
                    "sampleShadingEnable": "VK_FALSE",
                    "minSampleShading": 0,
                    "pSampleMask": "NULL",
                    "alphaToCoverageEnable": "VK_FALSE",
                    "alphaToOneEnable": "VK_FALSE"
                },
                "pDepthStencilState": "NULL",
                "pColorBlendState": {
                    "sType": "VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO",
                    "pNext": "NULL",
                    "flags": "0",
                    "logicOpEnable": "VK_FALSE",
                    "logicOp": "VK_LOGIC_OP_CLEAR",
                    "attachmentCount": 0,
                    "pAttachments": "NULL",
                    "blendConstants": [ 0, 0, 0, 0 ]
                },
                "pDynamicState": {
                    "sType": "VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO",
                    "pNext": "NULL",
                    "flags": 0,
                    "dynamicStateCount": 2,
                    "pDynamicStates": [
                        "VK_DYNAMIC_STATE_VIEWPORT",
                        "VK_DYNAMIC_STATE_SCISSOR"
                    ]
                },
                "layout": "",
                "renderPass": "",
                "subpass": 0,
                "basePipelineHandle": "",
                "basePipelineIndex": 0
            }
        }
    }
)json";

[[maybe_unused]] static char const kSampleComputeShaderSpv[] = R"spirv(
            OpCapability Shader
       %1 = OpExtInstImport "GLSL.std.450"
            OpMemoryModel Logical GLSL450
            OpEntryPoint GLCompute %main "main"
            OpExecutionMode %main LocalSize 2 1 1
            OpSource GLSL 450
    %void = OpTypeVoid
       %3 = OpTypeFunction %void
    %main = OpFunction %void None %3
       %5 = OpLabel
            OpReturn
            OpFunctionEnd
)spirv";

[[maybe_unused]] static char const kSampleVertexShaderSpv[] = R"spirv(
            OpCapability Shader
            OpMemoryModel Logical GLSL450
            OpEntryPoint Vertex %main "main"
            OpSource GLSL 450
    %void = OpTypeVoid
       %3 = OpTypeFunction %void
    %main = OpFunction %void None %3
       %5 = OpLabel
            OpReturn
            OpFunctionEnd
)spirv";

[[maybe_unused]] static char const kSampleFragmentShaderSpv[] = R"spirv(
            OpCapability Shader
            OpCapability ImageBuffer
            OpMemoryModel Logical GLSL450
            OpEntryPoint Fragment %main "main"
            OpExecutionMode %main OriginUpperLeft
    %void = OpTypeVoid
       %3 = OpTypeFunction %void
    %main = OpFunction %void None %3
       %5 = OpLabel
            OpReturn
            OpFunctionEnd
)spirv";
