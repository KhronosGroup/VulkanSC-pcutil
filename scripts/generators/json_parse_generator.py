#!/usr/bin/python3 -i
#
# Copyright (c) 2024-2025 The Khronos Group Inc.
# Copyright (c) 2024-2025 RasterGrid Kft.
#
# SPDX-License-Identifier: Apache-2.0

import os
from base_generator import BaseGenerator
from generators.generator_utils import PlatformGuardHelper
from vulkan_object import (Handle, Struct, Enum, Bitmask, Flags, Member)
from generators.generator_utils import PipelineJsonHelper

class JsonParseGenerator(BaseGenerator):
    def __init__(self):
        BaseGenerator.__init__(self)

    def generate(self):
        self.generatedMethods: dict[str] = {}
        self.generatedContentsMethods: dict[str] = {}
        self.generatedArrayMethods: dict[str] = {}
        self.generatedFixedSizeArrayMethods: dict[str] = {}
        self.generatedPointerMethods: dict[str] = {}

        self.parse_basic_methods: list[str] = []
        self.parse_Handle_methods: list[str] = []
        self.parse_Enum_c_str_methods: list[str] = []
        self.parse_Enum_methods: list[str] = []
        self.parse_Flags_methods: list[str] = []
        self.parse_Bitmask_methods: list[str] = []
        self.parse_structChain_methods: list[str] = []

        self.parse_struct_contents_methods: list[str] = []

        self.basicTypes: list[str] = []

        self.genManualMethods()
        self.genBasicMethods()
        self.genTypeMethod('VkStructureType')

        for struct in [self.vk.structs[x] for x in PipelineJsonHelper.getAllParseGenStructs()]:
            self.genStructChainMethod(struct)

        out = []
        out.append(f'''
            // *** THIS FILE IS GENERATED - DO NOT EDIT ***
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

        guard_helper = PlatformGuardHelper()
        out.extend(guard_helper.add_guard(None))

        out.append(f'''
              private:
                {"".join(self.parse_Handle_methods)}
                {"".join(self.parse_Enum_c_str_methods)}
              protected:
                {"".join(self.parse_basic_methods)}
                {"".join(self.parse_Enum_methods)}
                {"".join(self.parse_Flags_methods)}
                {"".join(self.parse_Bitmask_methods)}
                {"".join(self.parse_structChain_methods)}
              private:
                {"".join(self.parse_struct_contents_methods)}
            }};

            }}  // namespace pcjson
            ''')

        out.append('// NOLINTEND') # Wrap for clang-tidy to ignore
        self.write("".join(out))

    def genManualMethods(self):
        # VkShaderModuleCreateInfo is a special case, it is not part of the Vulkan SC API but is used by the CTS
        self.parse_structChain_methods.append('''
            VkShaderModuleCreateInfo parse_VkShaderModuleCreateInfo(const Json::Value& json, const LocationScope& l) {
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
        self.basicTypes = ['int8_t', 'uint8_t', 'int16_t', 'uint16_t', 'int32_t', 'uint32_t', 'int64_t', 'uint64_t', 'float', 'size_t']
        self.basicTypes.extend(['VkBool32', 'VkDeviceSize', 'VkSampleMask'])
        for typeName in self.basicTypes:
            self.generatedMethods[typeName] = f'parse_{typeName}'

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

            float parse_float(const Json::Value& v, const LocationScope& l) {
                if (v.isDouble()) {
                    return v.asFloat();
                } else {
                    Error() << "Not a 32-bit floating-point value";
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
                    const char *first, *last;
                    v.getString(&first, &last);
                    auto str_size = std::distance(first, last);
                    auto dst = AllocMem<char>(str_size + 1); // null-terminator
                    std::copy(first, last, dst);
                    dst[str_size] = '\\0';
                    return dst;
                } else {
                    Error() << "Not a string";
                    return nullptr;
                }
            }

            void* parse_binary(const Json::Value& json, const LocationScope& l) {
                if (!json.isString()) {
                    Error() << "Not a base64 encoded binary";
                    return nullptr;
                }

                auto src_buffer = json.asString();
                size_t src_idx = 0;
                uint8_t* dst_buffer = AllocMem<uint8_t>(src_buffer.size() * 3);

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

            VkBool32 parse_VkBool32(const Json::Value& v, const LocationScope&) {
                if (v.isString()) {
                    auto value = v.asString();
                    if (value == "VK_TRUE") return VK_TRUE;
                    else if (value == "VK_FALSE") return VK_FALSE;
                    else {
                        Error() << "VKBool32 string is neither VK_TRUE nor VK_FALSE";
                        return 0;
                    }
                }
                else if (v.isUInt() && v.asUInt() <= UINT32_MAX) {
                    return v.asUInt();
                } else {
                    Error() << "Not a 32-bit unsigned integer";
                    return 0;
                }
            }

            VkSampleMask parse_VkSampleMask(const Json::Value& v, const LocationScope& l) {
                return parse_uint32_t(v, l);
            }
            ''')

    def genArrayMethod(self, typeName: str) -> str:
        if typeName in self.generatedArrayMethods:
            return self.generatedArrayMethods[typeName]

        guard_helper = PlatformGuardHelper()
        self.parse_struct_contents_methods.append(f'''
            {''.join(guard_helper.add_guard(self.vk.structs[typeName].protect)) if typeName in self.vk.structs else ''}
            {typeName}* parse_{typeName}_array(const Json::Value& json, const LocationScope& l) {{
                if (!json.isArray() && !(json.isString() && json.asString() == "NULL")) {{
                    Error() << "Not an array";
                    return nullptr;
                }}

                if (json.isString() && json.asString() == "NULL") {{
                    return nullptr;
                }}
                auto count = json.size();
                auto dst_buffer = AllocMem<{typeName}>(count);
                for (Json::Value::ArrayIndex i = 0; i < count; ++i)
                {{
                    dst_buffer[i] = {self.genTypeMethod(typeName)}(json[i], l);
                }}

                return dst_buffer;
            }}
            {''.join(guard_helper.add_guard(None))}
        ''')
        self.generatedArrayMethods[typeName] = f'parse_{typeName}_array'
        return self.generatedArrayMethods[typeName]

    def genFixedSizeArrayMethod(self, typeName: str) -> str:
        if typeName in self.generatedFixedSizeArrayMethods:
            return self.generatedFixedSizeArrayMethods[typeName]

        self.parse_struct_contents_methods.append(f'''
            void parse_{typeName}_fixed_size_array({typeName}* dst, const Json::Value& json, const LocationScope& l) {{
                if (!json.isArray()) {{
                    Error() << "Not an array";
                    return;
                }}

                auto count = json.size();
                for (Json::Value::ArrayIndex i = 0; i < count; ++i)
                {{
                    dst[i] = {self.genTypeMethod(typeName)}(json[i], l);
                }}
            }}
        ''')
        self.generatedFixedSizeArrayMethods[typeName] = f'parse_{typeName}_fixed_size_array'
        return self.generatedFixedSizeArrayMethods[typeName]

    def genPointerMethod(self, typeName: str) -> str:
        if typeName in self.generatedPointerMethods:
            return self.generatedPointerMethods[typeName]

        self.parse_struct_contents_methods.append(f'''
            {typeName}* parse_{typeName}_pointer(const Json::Value& json, const LocationScope& l) {{
                if (json.isString() && json.asString() == "NULL") {{
                    return nullptr;
                }}

                auto dst_buffer = AllocMem<{typeName}>(1);
                dst_buffer[0] = {self.genTypeMethod(typeName)}(json, l);

                return dst_buffer;
            }}
        ''')
        self.generatedPointerMethods[typeName] = f'parse_{typeName}_pointer'
        return self.generatedPointerMethods[typeName]

    def genTypeMethod(self, typeName: str) -> str:
        if typeName in self.generatedMethods:
            # Do not generate methods for types if they already are generated
            return self.generatedMethods[typeName]
        elif typeName in self.vk.handles:
            return self.genHandleMethod(self.vk.handles[typeName])
        elif typeName in self.vk.structs:
            if self.vk.structs[typeName].sType is not None:
                # This is an extensible structure, generate struct chain method for it
                return self.genStructChainMethod(self.vk.structs[typeName])
            else:
                # Otherwise generate the regular struct content method
                return self.genStructContentsMethod(self.vk.structs[typeName])
        elif typeName in self.vk.enums:
            return self.genEnumMethod(self.vk.enums[typeName])
        elif typeName in self.vk.bitmasks:
            return self.genBitmaskMethod(self.vk.bitmasks[typeName])
        elif typeName in self.vk.flags:
            return self.genFlagsMethod(self.vk.flags[typeName])
        else:
            raise Exception(f'Unexpected type name "{typeName}"')

    def genHandleMethod(self, handle: Handle) -> str:
        # Handles are converted to uint64_t values through constructor syntax
        self.parse_Handle_methods.append(f'''
            {handle.name} parse_{handle.name}(const Json::Value& json, const LocationScope& l) {{
                if (json.isString()) {{
                    return reinterpret_cast<{handle.name}>((void*)parse_string(json, l));
                }} else if (json.isUInt()) {{
                    return {handle.name}(parse_uint64_t(json, l));
                }} else {{
                    Error() << "Invalid handle";
                    return {handle.name}(0);
                }}
            }}
        ''')
        self.generatedMethods[handle.name] = f'parse_{handle.name}'
        return self.generatedMethods[handle.name]

    def genFlagsMethod(self, flags: Flags) -> str:
        if flags.bitmaskName:
            self.genBitmaskMethod(self.vk.bitmasks[flags.bitmaskName])
            self.parse_Flags_methods.append(f'''
                {flags.name} parse_{flags.name}(const Json::Value& json, const LocationScope& l) {{
                    {flags.name} result = 0;
                    if (json.isUInt() && json.asUInt() == 0) {{
                        return result;
                    }}
                    else if (json.isString()) {{
                        auto json_str = json.asCString();
                        if (strcmp(json_str, "NULL") == 0 || strcmp(json_str, "0") == 0) {{
                            return result;
                        }}
                        std::stringstream strm(json_str);
                        std::string str;
                        while (std::getline(strm, str, '|')) {{
                            str.erase(std::remove_if(str.begin(), str.end(), isspace), str.end());
                            result |= parse_{flags.bitmaskName}_c_str(str.c_str());
                        }}
                    }} else {{
                        Error() << "Invalid format";
                    }}
                    return result;
                }}
            ''')
        else:
            self.parse_Flags_methods.append(f'''
                {flags.name} parse_{flags.name}(const Json::Value& json, const LocationScope& l) {{
                    if (!((json.isUInt() && json.asUInt() == 0) || (json.isString() && strcmp(json.asCString(), "0") == 0))) {{
                        Error() << "Invalid format";
                    }}
                    return static_cast<{flags.name}>(0);
                }}
            ''')
        self.generatedMethods[flags.name] = f'parse_{flags.name}'
        return self.generatedMethods[flags.name]

    def genEnumMethod(self, enum: Enum) -> str:
        self.parse_Enum_c_str_methods.append(f'''
            {enum.name} parse_{enum.name}_c_str(const char* v) {{
                static std::unordered_map<std::string_view, {enum.name}> map = {{
                    {','.join([f'std::make_pair("{f.name}", {f.name})' for f in enum.fields])}
                }};
                auto it = map.find(v);
                if (it != map.end()) {{
                    return it->second;
                }} else {{
                    Error() << "Invalid {enum.name} constant: " << v;
                    return static_cast<{enum.name}>(0);
                }}
            }}
        ''')
        self.parse_Enum_methods.append(f'''
            {enum.name} parse_{enum.name}(const Json::Value& json, const LocationScope& l) {{
                if (json.isString()) {{
                    return parse_{enum.name}_c_str(json.asCString());
                }} else {{
                    Error() << "Invalid format";
                    return static_cast<{enum.name}>(0);
                }}
            }}
        ''')
        self.generatedMethods[enum.name] = f'parse_{enum.name}'
        return self.generatedMethods[enum.name]

    def genBitmaskMethod(self, bitmask: Bitmask) -> str:
        if bitmask.name in self.generatedMethods:
            return self.generatedMethods[bitmask.name]

        self.parse_Enum_c_str_methods.append(f'''
            {bitmask.name} parse_{bitmask.name}_c_str(const char* v) {{
                static std::unordered_map<std::string_view, {bitmask.name}> map = {{
                    std::make_pair("0", static_cast<{bitmask.name}>(0)),
                    {','.join([f'std::make_pair("{f.name}", {f.name})' for f in bitmask.flags])}
                }};
                auto it = map.find(v);
                if (it != map.end()) {{
                    return it->second;
                }} else {{
                    Error() << "Invalid {bitmask.name} bit: " << v;
                    return static_cast<{bitmask.name}>(0);
                }}
            }}
        ''')
        self.parse_Enum_methods.append(f'''
            {bitmask.name} parse_{bitmask.name}(const Json::Value& json, const LocationScope& l) {{
                if (json.isString()) {{
                    return parse_{bitmask.name}_c_str(json.asCString());
                }} else {{
                    Error() << "Invalid format";
                    return static_cast<{bitmask.name}>(0);
                }}
            }}
        ''')
        self.generatedMethods[bitmask.name] = f'parse_{bitmask.name}'
        return self.generatedMethods[bitmask.name]

    def genStructChainMethod(self, struct: Struct) -> str:
        if struct.name in self.generatedMethods:
            return self.generatedMethods[struct.name]

        extStructs = [self.vk.structs[x] for x in struct.extendedBy] if struct.extendedBy else []
        guard_helper = PlatformGuardHelper()
        self.parse_structChain_methods.append(f'''
            {struct.name} parse_{struct.name}(const Json::Value& json, const LocationScope& l) {{
                {struct.name} s = {self.genStructContentsMethod(struct)}(json, l);

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
                        {''.join([f'''
                        {''.join(guard_helper.add_guard(extStruct.protect))}
                        case {extStruct.sType}: {{
                            auto next = AllocMem<{extStruct.name}>();
                            current_pnext_ref = "pNext<{extStruct.name}>";
                            *next = {self.genStructContentsMethod(extStruct)}(*json_next, CreateScope(current_pnext_ref, true));
                            prev->pNext = reinterpret_cast<VkBaseOutStructure*>(next);
                            break;
                            }}
                        {''.join(guard_helper.add_guard(None))}
                        ''' for extStruct in extStructs]) if struct.extendedBy else ''}
                        default:
                                Error() << "Invalid structure type extending VkGraphicsPipelineCreateInfo: " << (*json_next)["sType"].asCString();
                                break;
                    }}
                    json_next = &(*json_next)["pNext"];
                    if (prev->pNext != nullptr) prev = prev->pNext;
                }}

                if (!json_next->isString() || json_next->asString() != "NULL") {{
                    Error() << "Invalid pNext format";
                }}
                return s;
            }}
        ''')

        self.generatedMethods[struct.name] = f'parse_{struct.name}'
        return self.generatedMethods[struct.name]


    def genStructContentsMethod(self, struct: Struct):
        if struct.name in self.generatedContentsMethods:
            return self.generatedContentsMethods[struct.name]

        def handle_member(self, member: Member):
            if member.const and member.pointer and member.type == 'char': # handle strings <const char*>
                return f's.{member.name} = parse_string(json["{member.name}"], CreateScope("{member.name}"));'
            elif member.pointer and member.type == 'void': # handle binary <void*>
                return f's.{member.name} = parse_binary(json["{member.name}"], CreateScope("{member.name}"));'
            elif member.fixedSizeArray: # handle fix-sized arrays
                return f'{self.genFixedSizeArrayMethod(member.type)}(s.{member.name}, json["{member.name}"], CreateScope("{member.name}"));'
            elif member.pointer and member.length is not None: # handle arrays
                return f's.{member.name} = {self.genArrayMethod(member.type)}(json["{member.name}"], CreateScope("{member.name}"));'
            elif member.pointer and member.length is None: # handle non-array pointer
                return f's.{member.name} = {self.genPointerMethod(member.type)}(json["{member.name}"], CreateScope("{member.name}"));'
            elif member.type in self.vk.flags: # handle flags
                return f's.{member.name} = {self.genTypeMethod(member.type)}(json["{member.name}"], CreateScope("{member.name}"));'
            elif member.type in self.vk.enums: # enums flags
                return f's.{member.name} = {self.genTypeMethod(member.type)}(json["{member.name}"], CreateScope("{member.name}"));'
            elif member.type in self.vk.bitmasks: # bitmasks flags
                return f's.{member.name} = {self.genTypeMethod(member.type)}(json["{member.name}"], CreateScope("{member.name}"));'
            elif member.type in self.basicTypes: # basic types
                return f's.{member.name} = {self.genTypeMethod(member.type)}(json["{member.name}"], CreateScope("{member.name}"));'
            elif member.type in self.vk.handles: # handle handles
                return f's.{member.name} = {self.genTypeMethod(member.type)}(json["{member.name}"], CreateScope("{member.name}"));'
            elif member.type in self.vk.structs: # handle all structs
                return f's.{member.name} = {self.genTypeMethod(member.type)}(json["{member.name}"], CreateScope("{member.name}"));'
            else: # TODO: Unclear how to handle unions
                raise Exception(f'Unexpected memeber type "{member.fullType}"')

        members = filter(lambda m: m.name not in ['sType', 'pNext'], struct.members)
        members_list = list(members)
        handled_members = [handle_member(self, member) for member in members_list]

        guard_helper = PlatformGuardHelper()
        self.parse_struct_contents_methods.append(f'''
            {''.join(guard_helper.add_guard(struct.protect))}
            {struct.name} parse_{struct.name}_contents(const Json::Value& json, const LocationScope& l) {{
                {struct.name} s{{}};
                {'\n'.join(handled_members)}
                return s;
            }}
            {''.join(guard_helper.add_guard(None))}
        ''')
        self.generatedContentsMethods[struct.name] = f'parse_{struct.name}_contents'
        return self.generatedContentsMethods[struct.name]
