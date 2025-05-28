#!/usr/bin/python3 -i
#
# Copyright (c) 2024-2025 The Khronos Group Inc.
# Copyright (c) 2024-2025 RasterGrid Kft.
#
# SPDX-License-Identifier: Apache-2.0

import os
from base_generator import BaseGenerator
from generators.generator_utils import PlatformGuardHelper
from vulkan_object import (Handle, Struct, Enum, Bitmask, Flags)
from generators.generator_utils import PipelineJsonHelper

class JsonGenGenerator(BaseGenerator):
    def __init__(self):
        BaseGenerator.__init__(self)

    def generate(self):
        out = []
        out.append(f'''// *** THIS FILE IS GENERATED - DO NOT EDIT ***
            // See {os.path.basename(__file__)} for modifications

            /*
             * Copyright (c) 2024-2025 The Khronos Group Inc.
             * Copyright (c) 2024-2025 RasterGrid Kft.
             *
             * SPDX-License-Identifier: Apache-2.0
             */

             #pragma once
             ''')
        out.append('// NOLINTBEGIN') # Wrap for clang-tidy to ignore
        out.append('''
            #include <json/json.h>
            #include <vulkan/vulkan.h>

            #include <string>
            #include <sstream>
            #include <algorithm>

            #include "vksc_pipeline_json_base.hpp"

            namespace pcjson {

            class GeneratorBase : protected Base {
            ''')

        self.generatedMethods: dict[str] = []

        self.gen_basic_methods: list[str] = []
        self.gen_Handle_methods: list[str] = []
        self.gen_Enum_c_str_methods: list[str] = []
        self.gen_Enum_methods: list[str] = []
        self.gen_Flags_methods: list[str] = []
        self.gen_structChain_methods: list[str] = []
        self.gen_struct_contents_methods: list[str] = []

        out.append(f'''
              private:
                {"".join(self.gen_basic_methods)}
                {"".join(self.gen_Handle_methods)}
                {"".join(self.gen_Enum_c_str_methods)}
              protected:
                {"".join(self.gen_Enum_methods)}
                {"".join(self.gen_Flags_methods)}
                {"".join(self.gen_structChain_methods)}
              private:
                {"".join(self.gen_struct_contents_methods)}
            }};

            }}  // namespace pcjson
            ''')

        out.append('// NOLINTEND') # Wrap for clang-tidy to ignore
        self.write("".join(out))

        self.genManualMethods()
        self.genBasicMethods()

        for struct in [self.vk.structs[x] for x in PipelineJsonHelper.getAllParseGenStructs()]:
            self.genStructChainMethod(struct)

        guard_helper = PlatformGuardHelper()
        out.extend(guard_helper.add_guard(None))

    def genManualMethods(self):
        # VkShaderModuleCreateInfo is a special case, it is not part of the Vulkan SC API but is used by the CTS
        self.gen_structChain_methods.append('''
            Json::Value gen_VkShaderModuleCreateInfo(const VkShaderModuleCreateInfo& s, const LocationScope& l) {
                Json::Value json = Json::objectValue;
                json["sType"] = "VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO";
                json["pNext"] = "NULL";
                json["flags"] = 0;
                json["codeSize"] = s.codeSize;
                json["pCode"] = gen_binary(s.pCode, s.codeSize);
                return json;
            }
            ''')

    def genBasicMethods(self):
        # For basic types we generate simple wrappers
        basicTypes = ['int8_t', 'uint8_t', 'int16_t', 'uint16_t', 'int32_t', 'uint32_t', 'int64_t', 'uint64_t', 'float', 'size_t', 'VkDeviceSize']
        for typeName in basicTypes:
            self.generatedMethods[typeName] = f'gen_{typeName}'
            self.gen_basic_methods.append(f'Json::Value gen_{typeName}(const {typeName} v, const LocationScope&) {{ return v; }}')

        # We also need to add a base64 encoder for encoding binary data
        self.gen_basic_methods.append('''
            std::string gen_binary(const void* ptr, const size_t size) {
                static const char base64_table[64] = {
                    'A','B','C','D','E','F','G','H','I','J','K','L','M',
                    'N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
                    'a','b','c','d','e','f','g','h','i','j','k','l','m',
                    'n','o','p','q','r','s','t','u','v','w','x','y','z',
                    '0','1','2','3','4','5','6','7','8','9','+','/'
                };

                const uint8_t* data = reinterpret_cast<const uint8_t*>(ptr);
                size_t src_idx = 0;
                std::string result;
                result.reserve(size * 4);

                while (src_idx < size) {
                    size_t num_read = std::min(3, size - src_idx);

                    uint8_t	s0 = data[src_idx];
                    uint8_t	s1 = (num_read >= 2) ? data[src_idx + 1] : 0;
                    uint8_t	s2 = (num_read >= 3) ? data[src_idx + 2] : 0;

                    src_idx += num_read;

                    result.push_back(base64_table[s0 >> 2]);
                    result.push_back(base64_table[((s0 & 0x3) << 4) | (s1 >> 4)]);
                    result.push_back(base64_table[((s1 & 0xF) << 2) | (s2 >> 6)]);
                    result.push_back(base64_table[s2 & 0x3F]);

                    if (num_read < 3) result[result.size() - 1] = '=';
                    if (num_read < 2) result[result.size() - 2] = '=';
                }
                return result;
            }
            ''')

    def genTypeMethod(self, typeName: str) -> str:
        if typeName in self.generatedMethods:
            # Do not generate methods for types if they already are generated
            return self.generatedMethods[typeName]
        elif typeName in self.vk.handles:
            return self.genHandleMethod(self.vk.handles[typeName])
        elif typeName in self.vk.structs:
            if self.vk.structs[typeName].sType is not None:
                # This is an extensible structure, generate struct chain method for it
                return self.genStructChainMethod()
            else:
                # Otherwise generate the regular struct content method
                return self.genStructContentsMethod()
        elif typeName in self.vk.enums:
            # TODO: return self.genEnumMethod(self.vk.enums[typeName])
            return None
        elif typeName in self.vk.bitmasks:
            # TODO: return self.genBitmaskMethod(self.vk.bitmasks[typeName])
            return None
        elif typeName in self.vk.flags:
            # TODO: return self.genFlagsMethod(self.vk.flags[typeName])
            return None
        else:
            raise Exception(f'Unexpected type name "{typeName}"')

    def genHandleMethod(self, handle: Handle) -> str:
        # Handles are converted to uint64_t values through constructor syntax
        self.gen_Handle_methods.append(f'Json::Value gen_{handle.name}(const {handle.name} v, const LocationScope&) {{ return uint64_t(v); }}')
        self.generatedMethods[handle.name] = f'gen_{handle.name}'
        return self.generatedMethods[handle.name]

    def genStructChainMethod(self, struct: Struct) -> str:
        self.genStructContentsMethod(self, struct)

        out = []
        out.append(f'''
            Json::Value gen_{struct.name}(const {struct.name}& s, const LocationScope& l) {{
                Json::Value json = gen_{struct.name}_contents(s, l);

                json["sType"] = "{struct.sType}";

                auto next = reinterpret_cast<const VkBaseInStructure*>(s.pNext);
                auto& json_next = json["pNext"];

                while (next != nullptr) {{
                    switch (next->sType) {{
            ''')

        for extStruct in [self.vk.structs[x] for x in struct.extendedBy]:
            out.append(f'''
                        case {extStruct.sType}: {{
                            json_next = {self.genStructContentsMethod(extStruct)}(
                                *reinterpret_cast<const {extStruct.name}*>(next),
                                CreateScope("pNext<{extStruct.name}>", true));
                            json_next["sType"] = "{extStruct.sType}";
                            break;
                ''')

        out.append('''
                        default:
                            Error() << "Invalid structure type extending VkGraphicsPipelineCreateInfo: " << next->sType;
                            break;
                    }
                    next = next->pNext;
                    if (!json_next.isNull()) json_next = json_next["pNext"];
                }

                json_next = "NULL";

                return json;
            ''')

        self.gen_structChain_methods.extend(out)

        self.generatedMethods[struct.name] = f'gen_{struct.name}'
        return self.generatedMethods[struct.name]

    def genStructContentsMethod(self, struct: Struct):
        out = []

        # TODO: Unclear how to handle unions

        for member in struct.members:
            if member.name in ['sType', 'pNext']:
                # Do not handle sType and pNext here as it is already handled in genStructChainMethod
                continue

            # TODO: convert to JSON for each member, typical case would be
            out.append(f'json["{member.name}"] = {self.genTypeMethod(member.type)}(s.{member.name}, CreateScope("{member.name}"));')
            # but need to handle arrays, strings, optionality, pointers, etc. so it gets complicated
            # For pointers or arrays, the CreateScope takes an additional argument (see its interface)
            # For binary data we have to encode it as base64 with gen_binary

        self.gen_struct_contents_methods.extend(out)
        self.generatedMethods[struct.name] = f'gen_{struct.name}_contents'
        return self.generatedMethods[struct.name]
