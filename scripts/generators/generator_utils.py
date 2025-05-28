#!/usr/bin/python3 -i
#
# Copyright 2023-2025 The Khronos Group Inc.
# Copyright 2023 Valve Corporation
# Copyright 2023 LunarG, Inc.
# Copyright 2023-2025 RasterGrid Kft.
#
# SPDX-License-Identifier: Apache-2.0

from vulkan_object import (VulkanObject, Struct, Member, Flags)

class PlatformGuardHelper():
    """Used to elide platform guards together, so redundant #endif then #ifdefs are removed
    Note - be sure to call add_guard(None) when done to add a trailing #endif if needed
    """
    def __init__(self):
        self.current_guard = None

    def add_guard(self, guard, extra_newline = False):
        out = []
        if self.current_guard is not guard and self.current_guard is not None:
            out.append(f'#endif  // {self.current_guard}\n')
        if extra_newline:
            out.append('\n')
        if self.current_guard is not guard and guard is not None:
            out.append(f'#ifdef {guard}\n')
        self.current_guard = guard
        return out

class PipelineJsonHelper:
    # Returns the list of structure names relevant for pipeline JSON generation
    @staticmethod
    def getPipelineJsonStructs() -> list[str]:
        return [
            'VkGraphicsPipelineCreateInfo',
            'VkComputePipelineCreateInfo',
            'VkSamplerYcbcrConversionCreateInfo',
            'VkSamplerCreateInfo',
            'VkDescriptorSetLayoutCreateInfo',
            'VkPipelineLayoutCreateInfo',
            'VkPhysicalDeviceFeatures2',
            'VkRenderPassCreateInfo',
            'VkRenderPassCreateInfo2'
        ]

    # Returns the list of all structure names supported by the parser/generator
    @staticmethod
    def getAllParseGenStructs() -> list[str]:
        return PipelineJsonHelper.getPipelineJsonStructs() + [
            'VkDeviceObjectReservationCreateInfo',
            'VkPipelineOfflineCreateInfo'
        ]