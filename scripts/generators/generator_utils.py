#!/usr/bin/python3 -i
#
# Copyright 2023-2025 The Khronos Group Inc.
# Copyright 2023 Valve Corporation
# Copyright 2023 LunarG, Inc.
# Copyright 2023-2025 RasterGrid Kft.
#
# SPDX-License-Identifier: Apache-2.0

from vulkan_object import (VulkanObject, Member)
from enum import Enum

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

class TypeCategory(Enum):
    STRING = 1
    STRING_FIXED_SIZE_ARRAY = 2
    STRING_ARRAY = 3
    BINARY = 4
    POINTER = 5
    FIXED_SIZE_ARRAY = 6
    ARRAY = 7
    HANDLE = 8
    ENUM = 9
    FLAGS = 10
    BITMASK = 11
    BASIC = 12
    STRUCT = 13

class StructMemberHelper:
    # Dispatches struct member handling based on the type of the member
    @staticmethod
    def getMemberType(vk: VulkanObject, basicTypes: dict, member: Member) -> TypeCategory:
        if member.nullTerminated:
            if member.fixedSizeArray:
                return TypeCategory.STRING_FIXED_SIZE_ARRAY
            elif member.length:
                return TypeCategory.STRING_ARRAY
            else:
                return TypeCategory.STRING
        elif member.pointer and member.type == 'void':
            return TypeCategory.BINARY
        elif member.pointer and member.length is None:
            return TypeCategory.POINTER
        elif member.fixedSizeArray:
            return TypeCategory.FIXED_SIZE_ARRAY
        elif member.pointer and member.length is not None:
            return TypeCategory.ARRAY
        elif member.type in vk.handles:
            return TypeCategory.HANDLE
        elif member.type in vk.enums:
            return TypeCategory.ENUM
        elif member.type in vk.flags:
            return TypeCategory.FLAGS
        elif member.type in vk.bitmasks:
            return TypeCategory.BITMASK
        elif member.type in basicTypes:
            return TypeCategory.BASIC
        elif member.type in vk.structs:
            return TypeCategory.STRUCT
        else:
            raise Exception(f'Unexpected member type "{member.fullType}"')
