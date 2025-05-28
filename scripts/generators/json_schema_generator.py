#!/usr/bin/python3 -i
#
# Copyright (c) 2024-2025 The Khronos Group Inc.
# Copyright (c) 2024-2025 RasterGrid Kft.
#
# SPDX-License-Identifier: Apache-2.0

import json
from base_generator import BaseGenerator
from vulkan_object import (Handle, Struct, Enum, Bitmask, Flags)
from generators.generator_utils import PipelineJsonHelper

class JsonSchemaGenerator(BaseGenerator):
    def __init__(self):
        BaseGenerator.__init__(self)

    def generate(self):
        self.schema = {
            "$schema": "http://json-schema.org/draft-04/schema#",
            "id": "https://schema.khronos.org/vulkan/vkpcc.json#",
            "title": "JSON schema for Vulkan pipeline state",
            "description": "Schema for representing Vulkan pipeline state for use with the offline Pipeline Cache Compiler.",
            "type": "object",
            "additionalProperties": True,

            "definitions": {
                "ShaderInfo" : {
                    # TODO: stage should definitely not be URI but VkShaderStageFlagBits
                    "stage" : {"type": "string", "format": "uri"},
                    # TODO: unclear whether filename should be URI
                    "filename" : {"type": "string", "format": "uri"}
                },

                "GraphicsPipelineState": {
                    "type": "object",
                    "additionalProperties": False,
                    "properties": {
                        "Renderpass": {"$ref": "#/definitions/VkRenderPassCreateInfo"},
                        "Renderpass2": {"$ref": "#/definitions/VkRenderPassCreateInfo2"},
                        "YcbcrSamplers": {"type": "array", "minItems": 0, "maxItems": 255, "items": {"type": "object", "patternProperties": {"^\\w+$": {"$ref": "#/definitions/VkSamplerYcbcrConversionCreateInfo"}}}},
                        "ImmutableSamplers": {"type": "array", "minItems": 0, "maxItems": 255, "items": {"type": "object", "patternProperties": {"^\\w+$": {"$ref": "#/definitions/VkSamplerCreateInfo"}}}},
                        "DescriptorSetLayouts": {"type": "array", "minItems": 0, "maxItems": 255, "items": {"type": "object", "patternProperties": {"^\\w+$": {"$ref": "#/definitions/VkDescriptorSetLayoutCreateInfo"}}}},
                        "PipelineLayout": {"$ref": "#/definitions/VkPipelineLayoutCreateInfo"},
                        "GraphicsPipeline": {"$ref": "#/definitions/VkGraphicsPipelineCreateInfo"},
                        "ShaderFileNames": {"type": "array", "minItems": 0, "maxItems": 255, "items": {"$ref": "#/definitions/ShaderInfo"}},
                        "PhysicalDeviceFeatures": {"$ref": "#/definitions/VkPhysicalDeviceFeatures2"}
                    },
                    "oneOf" : [{"required" : ["Renderpass"]}, {"required" : ["Renderpass2"]}],
                    "required" : ["PipelineLayout", "GraphicsPipeline", "ShaderFileNames"]
                },

                "ComputePipelineState": {
                    "type": "object",
                    "additionalProperties": False,
                    "properties": {
                        "YcbcrSamplers": {"type": "array", "minItems": 0, "maxItems": 255, "items": {"type": "object", "patternProperties": {"^\\w+$": {"$ref": "#/definitions/VkSamplerYcbcrConversionCreateInfo"}}}},
                        "ImmutableSamplers": {"type": "array", "minItems": 0, "maxItems": 255, "items": {"type": "object", "patternProperties": {"^\\w+$": {"$ref": "#/definitions/VkSamplerCreateInfo"}}}},
                        "DescriptorSetLayouts": {"type": "array", "minItems": 0, "maxItems": 255, "items": {"type": "object", "patternProperties": {"^\\w+$": {"$ref": "#/definitions/VkDescriptorSetLayoutCreateInfo"}}}},
                        "PipelineLayout": {"$ref": "#/definitions/VkPipelineLayoutCreateInfo"},
                        "ComputePipeline": {"$ref": "#/definitions/VkComputePipelineCreateInfo"},
                        "ShaderFileNames": {"$ref": "#/definitions/ShaderInfo"},
                        "PhysicalDeviceFeatures": {"$ref": "#/definitions/VkPhysicalDeviceFeatures2"}
                    },
                    "required" : ["PipelineLayout", "ComputePipeline", "ShaderFileNames"]
                }
            },

            "properties": {
                "GraphicsPipelineState"  : {"$ref": "#/definitions/GraphicsPipelineState"},
                "ComputePipelineState"   : {"$ref": "#/definitions/ComputePipelineState"},
                "PipelineUUID"           : {"type": "array", "minItems": 16, "maxItems": 16, "items": {"$ref": "#/definitions/uint8_t"}},
                # TODO: should not be URI, maybe should include a regex pattern based on all extensions? e.g.:
                # "EnabledExtensions"      : {"type": "array", "items": {"$ref": "#/definitions/ExtensionName"}} ?
                "EnabledExtensions"      : {"type": "array", "items": {"type": "string", "format": "uri"}}
            },

            # TODO: Should PipelineUUID also be required?
            "anyOf": [
                {"required": ["GraphicsPipelineState"]},
                {"required": ["ComputePipelineState"]}
            ]
        }

        self.genBasicDefinitions()

        for struct in [self.vk.structs[x] for x in PipelineJsonHelper.getPipelineJsonStructs()]:
            self.genStructDefinition(struct)

        self.write(json.dumps(self.schema, indent=4))

    def genBasicDefinitions(self):
        self.schema["definitions"].update({
            "uint8_t": {"type": "integer", "minimum": 0, "maximum": 255},
            "int32_t": {"type": "integer", "minimum": -2147483648, "maximum": 2147483647},
            "uint32_t": {"type": "integer", "minimum": 0, "maximum": 4294967295},
            "uint64_t": {"oneOf": [{"enum": [""]}, {"type": "integer"}]},
            "float": {"type": "number"},
            "size_t": {"$ref": "#/definitions/uint64_t"},
            "VkDeviceSize": {"$ref": "#/definitions/uint64_t"},
        })

    def genAliases(self, typeName: str, aliases: list[str]):
        for alias in aliases:
            self.schema["definitions"][alias] = { "$ref": f"#/definitions/{typeName}" }

    def genTypeDefinition(self, typeName: str):
        if typeName in self.schema["definitions"]:
            # Do not generate type definition if it already exists
            return
        elif typeName in self.vk.handles:
            self.genHandleDefinition(self.vk.handles[typeName])
        elif typeName in self.vk.structs:
            self.genStructDefinition(self.vk.structs[typeName])
        elif typeName in self.vk.enums:
            # TODO: self.genEnumDefinition(self.vk.enums[typeName])
            pass
        elif typeName in self.vk.bitmasks:
            # TODO: self.genBitmaskDefinition(self.vk.bitmasks[typeName])
            pass
        elif typeName in self.vk.flags:
            # TODO: self.genFlagsDefinition(self.vk.flags[typeName])
            # For flags we should probably validate with at least an ugly regex of form:
            #   "(<flagbit1>|<flagbit2>|...)( *\| *(<flagbit1>|<flagbit2>|...))*"
            # so that we accept |-separated flag bit values with optional spaces in between
            pass
        else:
            raise Exception(f'Unexpected type name "{typeName}"')

    def genHandleDefinition(self, handle: Handle):
        self.schema["definitions"][handle.name] = {"$ref": "#/definitions/uint64_t"}
        self.genAliases(handle.name, handle.aliases)

    def genStructDefinition(self, struct: Struct, extStructNames: list[str] = None):
        # Do not generate struct definition if it already exists
        if struct.name in self.schema["definitions"]:
            return

        props: dict = {}

        for member in struct.members:
            if member.name == 'pNext':
                # pNext has to be handled specially, we have to generate schema to allow accepting chained structs
                oneOfList = [ "NULL" ]

                # TODO: We try to propagate the extendedBy here so that each struct chained to the pNext actually
                # get the same allowed extension structure list that the one coming from the base struct.
                # This works as long as there are no structures that could be chained to multiple ones.
                # We have to do these shenanigans because of the weird pNext chain representation in the pipeline JSON.
                extStructNames = struct.extendedBy if extStructNames is None else extStructNames
                # If this does not work out then we need to switch the alternative below:
                # Because the pNext chains are recursively added, we do not know whether this struct is the first
                # one in the chain, or this is already in the chain of another struct, so we have to relax the
                # schema a bit to allow not just the extension structs of this struct, but also the extension
                # structs of any struct that this extension extends.
                # There is just no way around this because of the awkward way pNext chains are represented in
                # pipeline JSONs.
                #   extStructNames = set(struct.extendedBy)
                #   for baseStructName in struct.extends:
                #       extStructNames = extStructNames.union(set(self.vk.structs[baseStructName].extendedBy))

                for extStructName in extStructNames:
                    # Generate definition of chained struct if not already done
                    self.genStructDefinition(extStructName, extStructNames)
                    # Add the definition to the list of possibilities
                    oneOfList.append(f"#/definitions/{extStructName}")
                props['pNext'] = { "oneOf": oneOfList }
            else:
                # Generate definition of member base type name
                self.genTypeDefinition(member.type)
                # TODO: generate definition for each member, typical case would be:
                props[member.name] = { "$ref": f"#/definitions/{member.type}" }
                # but need to handle arrays (with length, see member.length and member.fixedSizeArray)
                # strings are a special case of arrays with char elements (fixed size or pointer)
                # also need to handle optionality (see member.optional)
                # if pointer (see member.pointer) or array, then optional means can be "NULL", otherwise it only means can be zero
                # Note: member.optionalPointer should be ignored it is a special case we do not care about
                # etc. etc.
                pass

        self.schema["definitions"][struct.name] = {
            "type": "object",
            "additionalProperties": False,
            "properties": props
        }
        self.genAliases(struct.name, struct.aliases)
