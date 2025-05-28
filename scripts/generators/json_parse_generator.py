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

class JsonParseGenerator(BaseGenerator):
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
            #include <string_view>
            #include <unordered_map>
            #include <algorithm>

            #include "vksc_pipeline_json_base.hpp"

            namespace pcjson {

            class ParserBase : protected Base {
            ''')

        self.generatedMethods: dict[str] = []

        self.parse_basic_methods: list[str] = []
        self.parse_Handle_methods: list[str] = []
        self.parse_Enum_c_str_methods: list[str] = []
        self.parse_Enum_methods: list[str] = []
        self.parse_Flags_methods: list[str] = []
        self.parse_structChain_methods: list[str] = []
        self.parse_struct_contents_methods: list[str] = []

        out.append(f'''
              private:
                {"".join(self.parse_basic_methods)}
                {"".join(self.parse_Handle_methods)}
                {"".join(self.parse_Enum_c_str_methods)}
              protected:
                {"".join(self.parse_Enum_methods)}
                {"".join(self.parse_Flags_methods)}
                {"".join(self.parse_structChain_methods)}
              private:
                {"".join(self.parse_struct_contents_methods)}
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
        self.parse_structChain_methods.append('''
            VkShaderModuleCreateInfo gen_VkShaderModuleCreateInfo(const Json::Value& json, const LocationScope& l) {
                VkShaderModuleCreateInfo s{ VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO, nullptr };

                const auto& json_stype = json["sType"];
                if (json_stype.isString()) {
                    if (json_stype.asString() != "VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO") {
                        Error() << "Invalid sType value: " << json_stype.asCString();
                    }
                } else {
                    Error() << "Invalid sType format";
                }

                const auto& json_pnext = json["pNext"];
                if (!json_pnext.isString() || json_pnext.asString() != "NULL") {
                    Error() << "Unexpected non-NULL pNext";
                }

                const auto& json_flags = json["flags"];
                if (!json_flags.isUInt() || json_flags.asUInt() != 0) {
                    Error() << "Unexpected non-zero flags";
                }

                s.codeSize = parse_size_t(json["codeSize"], CreateScope("codeSize"));
                s.pCode = reinterpret_cast<const uint32_t*>(parse_binary(json["pCode"], CreateScope("pCode")));

                return s;
            }
            ''')

    def genBasicMethods(self):
        basicTypes = ['int8_t', 'uint8_t', 'int16_t', 'uint16_t', 'int32_t', 'uint32_t', 'int64_t', 'uint64_t', 'float', 'size_t', 'VkDeviceSize']
        for typeName in basicTypes:
            self.generatedMethods[typeName] = f'gen_{typeName}'

        self.parse_basic_methods.append('''
            int8_t parse_int8_t(const Json::Value& v, const LocationScope&) {
                if (v.isInt() && v.asInt() >= INT8_MIN && v.asInt() <= INT8_MAX) {
                    return v.asInt();
                } else {
                    Error() << "Not an 8-bit signed integer";
                    return 0;
                }
            }

            int16_t parse_int16_t(const Json::Value& v, const LocationScope&) {
                if (v.isInt() && v.asInt() >= INT16_MIN && v.asInt() <= INT16_MAX) {
                    return v.asInt();
                } else {
                    Error() << "Not a 16-bit signed integer";
                    return 0;
                }
            }

            int32_t parse_int32_t(const Json::Value& v, const LocationScope&) {
                if (v.isInt() && v.asInt() >= INT32_MIN && v.asInt() <= INT32_MAX) {
                    return v.asInt();
                } else {
                    Error() << "Not a 32-bit signed integer";
                    return 0;
                }
            }

            int64_t parse_int64_t(const Json::Value& v, const LocationScope&) {
                if (v.isInt64()) {
                    return v.asInt64();
                } else {
                    Error() << "Not a 64-bit signed integer";
                    return 0;
                }
            }

            uint8_t parse_uint8_t(const Json::Value& v, const LocationScope&) {
                if (v.isUInt() && v.asUInt() <= UINT8_MAX) {
                    return v.asUInt();
                } else {
                    Error() << "Not an 8-bit unsigned integer";
                    return 0;
                }
            }

            uint16_t parse_uint16_t(const Json::Value& v, const LocationScope&) {
                if (v.isUInt() && v.asUInt() <= UINT16_MAX) {
                    return v.asUInt();
                } else {
                    Error() << "Not a 16-bit unsigned integer";
                    return 0;
                }
            }

            uint32_t parse_uint32_t(const Json::Value& v, const LocationScope&) {
                if (v.isUInt() && v.asUInt() <= UINT32_MAX) {
                    return v.asUInt();
                } else {
                    Error() << "Not a 32-bit unsigned integer";
                    return 0;
                }
            }

            uint64_t parse_uint64_t(const Json::Value& v, const LocationScope&) {
                if (v.isUInt64()) {
                    return v.asUInt64();
                } else {
                    Error() << "Not a 64-bit unsigned integer";
                    return 0;
                }
            }

            size_t parse_size_t(const Json::Value& v, const LocationScope& l) {
                return parse_uint64_t(v, l);
            }

            VkDeviceSize parse_VkDeviceSize(const Json::Value& v, const LocationScope& l) {
                return parse_uint64_t(v, l);
            }

            const char* parse_string(const Json::Value& v, const LocationScope&) {
                if (v.isString()) {
                    const char *src = v.asCString();
                    size_t len = strlen(src);
                    char* dst = AllocMem<char>(len + 1);
                    strcpy(dst, src);
                    return dst;
                } else {
                    Error() << "Not a string";
                    return nullptr;
                }
            }

            const void* parse_binary(const Json::Value& json, const LocationScope& l) {
                if (!json.isString()) {
                    Error() << "Not a base64 encoded binary";
                    return nullptr;
                }

                auto src_buffer = json.asString();
                size_t src_idx = 0;
                uint8_t dst_buffer = AllocMem<uint8_t>(src_buffer.size() * 3);

                for (auto c : src_buffer) {
                    uint8_t decoded_bits = 0;
                    if ('A' <= c && c <= 'Z') {
                        decoded_bits = uint8_t(c - 'A');
                    } else if ('a' <= c && c <= 'z') {
                        decoded_bits = uint8_t(('Z' - 'A' + 1) + (c - 'a'));
                    } if ('0' <= c && c <= '9') {
                        decoded_bits = uint8_t(('Z' - 'A' + 1) + ('z' - 'a' + 1) + (c - '0'));
                    } else if (c == '+') {
                        decoded_bits = uint8_t(('Z' - 'A' + 1) + ('z' - 'a' + 1) + ('9' - '0' + 1));
                    } else if (c == '/') {
                        decoded_bits = uint8_t(('Z' - 'A' + 1) + ('z' - 'a' + 1) + ('9' - '0' + 2));
                    } else {
                        Error() << "Invalid base64 character '" << c << "'";
                        return nullptr;
                    }

                    auto dst_ptr = &dst_buffer[(src_idx >> 2) * 3];

                    switch (src_idx % 4) {
                        case 0:
                            dst_ptr[0] |= decoded_bits << 2;
                            break;
                        case 1:
                            dst_ptr[0] = uint8_t(dst_ptr[0] | uint8_t(decoded_bits >> 4));
                            dst_ptr[1] = uint8_t(dst_ptr[1] | uint8_t((decoded_bits & 0xF) << 4));
                            break;
                        case 2:
                            dst_ptr[1] = uint8_t(dst_ptr[1] | uint8_t(decoded_bits >> 2));
                            dst_ptr[2] = uint8_t(dst_ptr[2] | uint8_t((decoded_bits & 0x3) << 6));
                            break;
                        case 3:
                            dst_ptr[2] |= decoded_bits;
                            break;
                        default:
                            break;
                    }

                    src_idx++;
                }

                return dst_buffer;
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
        self.parse_Handle_methods.append(f'{handle.name} parse_{handle.name}(const Json::Value& json, const LocationScope& l) {{ return {handle.name}(parse_uint64_t(json, l)); }}')
        self.generatedMethods[handle.name] = f'parse_{handle.name}'
        return self.generatedMethods[handle.name]

    def genStructChainMethod(self, struct: Struct) -> str:
        self.genStructContentsMethod(self, struct)

        out = []
        out.append(f'''
            {struct.name} parse_{struct.name}(const Json::Value& json, const LocationScope& l) {{
                {struct.name} s = parse_{struct.name}_contents(json, l);

                const auto& json_stype = json["sType"];
                if (json_stype.isString()) {{
                    if (json_stype.asString() != "{struct.sType}") {{
                        Error() << "Invalid sType value: " << json_stype.asCString();
                    }}
                }} else {{
                    Error() << "Invalid sType format";
                }}

                [[maybe_unused]] auto prev = reinterpret_cast<VkBaseOutStructure*>(&s);
                const Json::Value* json_next = &json["pNext"];
                const char* current_pnext_ref = "pNext";
                while (json_next->isObject()) {{
                    auto next_stype = parse_VkStructureType((*json_next)["sType"], CreateScope(current_pnext_ref));
                    switch (next_stype) {{
            ''')

        for extStruct in [self.vk.structs[x] for x in struct.extendedBy]:
            out.append(f'''
                        case {extStruct.sType}: {{
                            auto next = AllocMem<{extStruct.name}>();
                            current_pnext_ref = "pNext<{extStruct.name}>";
                            *next = parse_{extStruct.name}_contents(*json_next, CreateScope(current_pnext_ref, true));
                            prev->pNext = reinterpret_cast<VkBaseOutStructure*>(next);
                            break;
                        }}
                ''')

        out.append('''
                        default:
                            Error() << "Invalid structure type extending VkGraphicsPipelineCreateInfo: " << (*json_next)["sType"].asCString();
                            break;
                    }
                    json_next = &(*json_next)["pNext"];
                    if (prev->pNext != nullptr) prev = prev->pNext;
                }

                if (!json_next->isString() || json_next->asString() != "NULL") {
                    Error() << "Invalid pNext format";
                }

                return s;
            ''')

        self.parse_structChain_methods.extend(out)

        self.generatedMethods[struct.name] = f'parse_{struct.name}'
        return self.generatedMethods[struct.name]


    def genStructContentsMethod(self, struct: Struct):
        out = []

        # TODO: Unclear how to handle unions

        for member in struct.members:
            if member.name in ['sType', 'pNext']:
                # Do not handle sType and pNext here as it is already handled in genStructChainMethod
                continue

            # TODO: convert to JSON for each member, typical case would be
            out.append(f'json["{member.name}"] = {self.genTypeMethod(member.type)}(json["{member.name}"], CreateScope("{member.name}"));')
            # but need to handle arrays, strings, optionality, pointers, etc. so it gets complicated
            # For pointers or arrays, the CreateScope takes an additional argument (see its interface)
            # For binary data we have to decode it as base64 with parse_binary
            # For string data we may have to decode it using parse_string

        self.parse_struct_contents_methods.extend(out)
        self.generatedMethods[struct.name] = f'parse_{struct.name}_contents'
        return self.generatedMethods[struct.name]
