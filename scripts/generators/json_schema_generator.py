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
            "$schema": "http://json-schema.org/draft-07/schema#",
            "id": f"https://schema.khronos.org/vulkan/{self.filename}#",
            "title": "JSON schema for Vulkan pipeline state",
            "description": "Schema for representing Vulkan pipeline state for use with the offline Pipeline Cache Compiler.",
            "type": "object",
            "additionalProperties": True,

            "definitions": {
                "ShaderInfo" : {
                    "stage" : {"$ref": "#/definitions/VkShaderStageFlagBits"},
                    "filename" : {"type": "string", "format": "string"}
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
                "EnabledExtensions"      : {"type": "array", "items": {"$ref": "#/definitions/ExtensionName"}}
            },

            "anyOf": [
                {"required": ["GraphicsPipelineState"]},
                {"required": ["ComputePipelineState"]}
            ]
        }

        self.genBasicDefinitions()
        self.schema["definitions"]["ExtensionName"] = { "enum" : list(self.vk.extensions.keys()) }

        for struct in [self.vk.structs[x] for x in PipelineJsonHelper.getPipelineJsonStructs()]:
            self.genStructDefinition(struct)

        self.write(json.dumps(self.schema, indent=4))

    def genBasicDefinitions(self):
        self.schema["definitions"].update({
            "uint8_t": {"type": "integer", "minimum": 0, "maximum": 255},
            "uint16_t": {"type": "integer", "minimum": 0, "maximum": 65535},
            "int32_t": {"type": "integer", "minimum": -2147483648, "maximum": 2147483647},
            "uint32_t": {"type": "integer", "minimum": 0, "maximum": 4294967295},
            "char": {"type": "string" },
            "binary": { "type": "string" }, # TODO: Maybe restrict to base64 characters
            "float": {"type": "number"},

            "uint64_t": {"oneOf":
                [
                    { "type": "string", "pattern": "[0-9]*"},
                    { "type": "string", "pattern": "VK_WHOLE_SIZE"},
                    { "type": "integer"}
                ]
            },

            "size_t": {"$ref": "#/definitions/uint64_t"},
            "VkDeviceSize": {"$ref": "#/definitions/uint64_t"},

            "subpass_id": {"oneOf": [{"enum": [ "VK_SUBPASS_EXTERNAL"] }, {"$ref": "#/definitions/uint32_t"}]},
            "VkBool32": { "oneOf": [ { "$ref": "#/definitions/uint32_t" }, { "enum": [ "VK_TRUE", "VK_FALSE" ] } ] },
            "VkSampleMask": { "$ref": "#/definitions/uint32_t" },
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
            self.genEnumDefinition(self.vk.enums[typeName])
        elif typeName in self.vk.bitmasks:
            self.genBitmaskDefinition(self.vk.bitmasks[typeName])
        elif typeName in self.vk.flags:
            self.genFlagsDefinition(self.vk.flags[typeName])
        else:
            raise Exception(f'Unexpected type name "{typeName}"')

    def genHandleDefinition(self, handle: Handle):
        self.schema["definitions"][handle.name] = { "anyOf":
            [
                {"$ref": "#/definitions/uint64_t"},
                {"type": "string"}
            ]
        }
        self.genAliases(handle.name, handle.aliases)

    def genEnumDefinition(self, enum: Enum):
        enum_values = list(map(lambda x: x.name, enum.fields))
        self.schema["definitions"][enum.name] = {"enum": enum_values}

    def genBitmaskDefinition(self, bitmask: Bitmask):
        flags = list(map(lambda x: x.name, bitmask.flags))
        self.schema["definitions"][bitmask.name] = {"enum": flags}

    def genFlagsDefinition(self, flags: Flags):
        self.schema["definitions"][flags.name] = { "anyOf":
            [
                {"type": "string", "pattern" : "0"},
                {"type": "integer", "minimum": 0, "maximum": 0},
            ]
        }
        if flags.bitmaskName != None:
            bitmask = self.vk.bitmasks[flags.bitmaskName]
            flagList = '|'.join(map(lambda x: x.name, bitmask.flags))
            flagType = {"type": "string", "pattern" : f"({flagList})( *| *{flagList})*"}
            self.schema["definitions"][flags.name]["anyOf"].insert(0, flagType)

    def getStructProperties(self, struct: Struct):
        props: dict = {}

        for member in struct.members:
            if member.name == 'pNext':
                continue

            elif member.name == "sType":
                # TODO: workaround for generated invalid VkPhysicalDeviceFeatures2 by the CTS
                if struct.name == 'VkPhysicalDeviceFeatures2':
                    props[member.name] = {
                        "type": "string",
                        "pattern": "(VK_STRUCTURE_TYPE_DEVICE_OBJECT_RESERVATION_CREATE_INFO|VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2)"
                    }
                else:
                    sTypes = [ struct.sType ]
                    sTypes += self.getSTypeAliases(struct.sType)
                    sTypes = "|".join(sTypes)
                    props[member.name] = { "type": "string", "pattern": f"{(sTypes)}" }

            elif (member.name == "srcSubpass" or member.name == "dstSubpass") and struct.name == "VkSubpassDependency":
                props[member.name] = { "$ref": "#/definitions/subpass_id" }

            else:
                if member.type == "void" and (member.fixedSizeArray or member.length or member.pointer):
                    props[member.name] = { "$ref": "#/definitions/binary" }
                    continue

                # Generate definition of member base type name
                self.genTypeDefinition(member.type)

                # NOTE: strings (i.e. const char*) and arrays of them do not need special handling
                # as char itself maps to JSON string type therefore the current logic will work out
                # as expected
                if member.fixedSizeArray:
                    # TODO: add restriction for array size
                    props[member.name] = {"type": "array", "items": {"$ref": f"#/definitions/{member.type}"}}
                elif member.length:
                    props[member.name] = {
                        "oneOf": [
                            {
                                "enum" : [ "NULL" ]
                            },
                            {
                                "type": "array",
                                "items": {"$ref": f"#/definitions/{member.type}"}
                            }
                        ]
                    }
                elif member.pointer or member.optional:
                    props[member.name] = {
                        "oneOf" if member.type != "char" else "anyOf": [
                            {
                                "enum" : [ "NULL" ]
                            },
                            {
                                "$ref": f"#/definitions/{member.type}"
                            }
                        ]
                    }
                else:
                    props[member.name] = { "$ref": f"#/definitions/{member.type}" }

        res = { "additionalProperties": False, "properties": props }
        if not struct.union:
            res["required"] = list(props.keys())
        return res

    def genStructPNext(self, struct: Struct):
        # Do not generate pnext definition if it already exists
        if struct.name + "_pNext" in self.schema["definitions"]:
            return

        extendedBy = struct.extendedBy

        # There is no pNext chain for this struct
        if extendedBy is None:
            return

        # TODO: workaround for generated invalid VkPhysicalDeviceFeatures2 by the CTS
        # it has a chained instance of itself
        if struct.name == "VkPhysicalDeviceFeatures2":
            extendedBy.append("VkPhysicalDeviceFeatures2")

        pNextChain = {
            "oneOf": [
                {
                    "enum": [
                        "NULL"
                    ]
                },
                {
                    "type": "object",
                    "allOf" : [
                        {
                            "additionalProperties": True,
                            "properties": {
                                "pNext": {
                                    "$ref": f"#/definitions/{struct.name}_pNext"
                                }
                            }
                        },
                        {
                            "anyOf": [ ]
                        }
                    ]
                }
            ]
        }

        anyOfList = pNextChain["oneOf"][1]["allOf"][1]["anyOf"]

        for extStructName in extendedBy:
            extStruct = self.vk.structs[extStructName]
            extStructProperties = self.getStructProperties(extStruct)
            extStructProperties["properties"]["pNext"] = {}
            anyOfList.append(extStructProperties)

        self.schema["definitions"][f"{struct.name}_pNext"] = pNextChain

    def genStructDefinition(self, struct: Struct):
        # Do not generate struct definition if it already exists
        if struct.name in self.schema["definitions"]:
            return

        structDef = self.getStructProperties(struct)
        structDef["type"] = "object"

        for member in struct.members:
            if member.name == 'pNext':
                pNext = None

                # pNext has to be handled specially, we have to generate schema to allow accepting chained structs
                if struct.extendedBy is not None:
                    self.genStructPNext(struct)
                    pNext = { "$ref" : f"#/definitions/{struct.name}_pNext" }
                else:
                    oneOfList = [ { "enum" : ["NULL"] } ]
                    pNext = { "oneOf": oneOfList }

                structDef["properties"]["pNext"] = pNext

        self.schema["definitions"][struct.name] = structDef
        self.genAliases(struct.name, struct.aliases)

    def getSTypeAliases(self, sType : str):
        structureTypeFields = self.vk.enums["VkStructureType"].fields
        for structTypeField in structureTypeFields:
            if structTypeField.name == sType:
                return structTypeField.aliases
        return [ ]
