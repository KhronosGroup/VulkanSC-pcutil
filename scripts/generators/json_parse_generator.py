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
from generators.generator_utils import PipelineJsonHelper, StructMemberHelper, TypeCategory

class JsonParseGenerator(BaseGenerator):
    def __init__(self):
        BaseGenerator.__init__(self)

    def generate(self):
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

        self.generatedMethods: dict[str] = {}

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
                    if (strcmp(json_stype.asCString(), "VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO") == 0) {
                        s.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
                    } else {
                        Error() << "Invalid sType value: " << json_stype.asCString();
                    }
                } else {
                    Error() << "Invalid sType format";
                }

                const auto& json_pnext = json["pNext"];
                if (!json_pnext.isString() || strcmp(json_pnext.asCString(), "NULL") != 0) {
                    Error() << "Unexpected non-NULL pNext";
                }

                const auto& json_flags = json["flags"];
                if (!json_flags.isUInt() || json_flags.asUInt() != 0) {
                    Error() << "Unexpected non-zero flags";
                }

                size_t size = 0;
                s.codeSize = parse_size_t(json["codeSize"], CreateScope("codeSize"));
                s.pCode = reinterpret_cast<const uint32_t*>(parse_binary(json["pCode"], CreateScope("pCode"), size));
                if (size != s.codeSize) {
                    Error() << "pCode binary size (" << size << ") does not match expected size (" << s.codeSize << ")";
                }

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

            void* parse_binary(const Json::Value& v, const LocationScope&, size_t& size) {
                if (v.isArray()) { // TODO: Consider removing. Array path is preserved for historical reasons.
                    if (v.size() == 0) {
                        size = 0;
                        return nullptr;
                    }
                    uint8_t* result = AllocMem<uint8_t>(v.size());
                    for (Json::Value::ArrayIndex i = 0; i < v.size(); ++i) {
                        if (v[i].isUInt() && v[i].asUInt() <= 255) {
                            result[i] = v[i].asUInt();
                        } else {
                            Error() << "Binary array has non-unsigned integral value or is out of [0-255] range.";
                            return nullptr;
                        }
                    }
                    size = v.size();
                    return result;
                }
                if (!v.isString()) {
                    Error() << "Base64 encoded binary not a string";
                    return nullptr;
                }

                const char *first, *last;
                v.getString(&first, &last);
                auto str_size = std::distance(first, last);
                std::string_view str(first, str_size);

                static const std::string_view base64_alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
                auto init_base64_lookup_table = [&] {
                    std::array<uint8_t, UINT8_MAX + 1> lookup_table{};
                    for (size_t i = 0; i <= UINT8_MAX; ++i) {
                        lookup_table[i] = UINT8_MAX;
                    }
                    for (size_t i = 0; i < base64_alphabet.size(); ++i) {
                        lookup_table[base64_alphabet[i]] = static_cast<uint8_t>(i);
                    }
                    return lookup_table;
                };
                static const std::array<uint8_t, UINT8_MAX + 1> base64_lookup_table = init_base64_lookup_table();

                constexpr uint32_t bits_in_base64 = 6;
                constexpr uint32_t bits_in_byte = 8;

                uint8_t* result = AllocMem<uint8_t>((str.size() * bits_in_base64 + bits_in_byte - 1) / bits_in_byte);
                size = 0;
                uint32_t current_word = 0;
                uint32_t current_bits = 0;
                for (auto ch : str) {
                    if (ch == '=') {
                        // End of data padding
                        break;
                    } else if (base64_lookup_table[ch] == UINT8_MAX) {
                        Error() << "Invalid base64 character '" << ch << "'";
                        return nullptr;
                    }
                    current_word = (current_word << bits_in_base64) + base64_lookup_table[ch];
                    current_bits += bits_in_base64;
                    if (current_bits >= bits_in_byte) {
                        result[size++] = static_cast<uint8_t>((current_word >> (current_bits - bits_in_byte)) & ((1 << bits_in_byte) - 1));
                        current_bits -= bits_in_byte;
                    }
                }
                return result;
            }

            VkBool32 parse_VkBool32(const Json::Value& v, const LocationScope&) {
                if (v.isString()) {
                    auto value = v.asCString();
                    if (strcmp(value, "VK_TRUE") == 0) {
                        return VK_TRUE;
                    } else if (strcmp(value, "VK_FALSE") == 0) {
                        return VK_FALSE;
                    } else {
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
                    auto json_str = json.asCString();
                    if (strcmp(json_str, "") == 0) {{
                        return VK_NULL_HANDLE;
                    }} else {{
                        return reinterpret_cast<{handle.name}>((void*)parse_string(json, l));
                    }}
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
        enum_fields = []
        for field in enum.fields:
            enum_fields.append(field.name)
            for alias in field.aliases:
                enum_fields.append(alias)
        enum_fields = [f'std::make_pair("{f}", {f})' for f in enum_fields]
        if enum.name == 'VkStructureType':
            # VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO does not exist in Vulkan SC, but the CTS has to [de]serialize it
            enum_fields.append('std::make_pair("VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO", VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO)')
        self.parse_Enum_c_str_methods.append(f'''
            {enum.name} parse_{enum.name}_c_str(const char* v) {{
                static std::unordered_map<std::string_view, {enum.name}> map = {{
                    {','.join(enum_fields)}
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
                }} else if (json.isInt()) {{
                    return static_cast<{enum.name}>(json.asInt());
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
        self.genStructContentsMethod(struct)

        extStructs = [self.vk.structs[x] for x in struct.extendedBy] if struct.extendedBy else []
        guard_helper = PlatformGuardHelper()
        cases = ''.join([f"""
                        {''.join(guard_helper.add_guard(extStruct.protect))}
                        case {extStruct.sType}: {{
                            auto next = AllocMem<{extStruct.name}>();
                            current_pnext_ref = "pNext<{extStruct.name}>";
                            *next = {self.genStructContentsMethod(extStruct)}(*json_next, CreateScope(current_pnext_ref, true));
                            next->sType = {extStruct.sType};
                            prev->pNext = reinterpret_cast<VkBaseOutStructure*>(next);
                            break;
                            }}
                        {''.join(guard_helper.add_guard(None))}
                        """ for extStruct in extStructs]) if struct.extendedBy else ''
        self.parse_structChain_methods.append(f'''
            {struct.name} parse_{struct.name}(const Json::Value& json, const LocationScope& l) {{
                {struct.name} s = {self.genStructContentsMethod(struct)}(json, l);

                const auto& json_stype = json["sType"];
                if (json_stype.isString()) {{
                    if (strcmp(json_stype.asCString(), "{struct.sType}") == 0) {{
                        s.sType = {struct.sType};
                    }} else {{
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
                        {cases}
                        default:
                                Error() << "Invalid structure type extending {struct.name}: " << (*json_next)["sType"].asCString();
                                break;
                    }}
                    json_next = &(*json_next)["pNext"];
                    if (prev->pNext != nullptr) prev = prev->pNext;
                }}

                if (!json_next->isString() || strcmp(json_next->asCString(), "NULL") != 0) {{
                    Error() << "Invalid pNext format";
                }}
                return s;
            }}
        ''')

        self.generatedMethods[struct.name] = f'parse_{struct.name}'
        return self.generatedMethods[struct.name]


    def genStructContentsMethod(self, struct: Struct):
        if struct.name in self.generatedMethods:
            return f'parse_{struct.name}_contents'

        guard_helper = PlatformGuardHelper()

        out = []
        out.extend(guard_helper.add_guard(struct.protect))

        # TODO: Unions have to be handled as struct for lack of a better option
        out.append(f'''
            {struct.name} parse_{struct.name}_contents(const Json::Value& json, const LocationScope& l) {{
                {struct.name} s{{}};
        ''')

        for member in struct.members:
            if member.name in ['sType', 'pNext']:
                # Do not handle sType and pNext here as it is already handled in genStructChainMethod
                continue

            match StructMemberHelper.getMemberType(self.vk, self.basicTypes, member):
                case TypeCategory.STRING:
                    out.append(f's.{member.name} = parse_string(json["{member.name}"], CreateScope("{member.name}"));')

                case TypeCategory.STRING_FIXED_SIZE_ARRAY | TypeCategory.STRING_ARRAY:
                    raise Exception(f"Unhandled string array")

                case TypeCategory.BINARY:
                    out.append(f'''
                        {{
                            size_t size = 0;
                            s.{member.name} = parse_binary(json["{member.name}"], CreateScope("{member.name}"), size);
                            if (size != s.{member.length}) {{
                                Error() << "{member.name} binary size (" << size << ") does not match expected size (" << s.{member.length} << ")";
                            }}
                        }}
                        ''')

                case TypeCategory.POINTER:
                    out.append(f'''
                        {{
                            const Json::Value& json_member = json["{member.name}"];
                            if (json_member.isString() && strcmp(json_member.asCString(), "NULL") == 0) {{
                                s.{member.name} = nullptr;
                            }} else {{
                                auto dst_buffer = AllocMem<{member.type}>();
                                *dst_buffer = {self.genTypeMethod(member.type)}(json_member, CreateScope("{member.name}", true));
                                s.{member.name} = dst_buffer;
                            }}
                        }}
                    ''')

                case TypeCategory.FIXED_SIZE_ARRAY | TypeCategory.ARRAY:
                    if member.fixedSizeArray and (len(member.fixedSizeArray) != 1 or member.length != member.fixedSizeArray[0]):
                        raise Exception(f"Unhandled fixedSizedArray: {struct.name}.{member.name} size: {member.fixedSizeArray}")

                    lengthExpr = member.length
                    if "rasterizationSamples" in lengthExpr:
                        lengthExpr = "size_t(" + lengthExpr.replace("rasterizationSamples", "s.rasterizationSamples") + ")"
                    elif lengthExpr.isdigit():
                        pass
                    elif lengthExpr == "VK_UUID_SIZE":
                        # TODO: Add constant handling once we have constants
                        pass
                    else:
                        lengthExpr = "s." + lengthExpr

                    out.append(f'''
                        {{
                            const Json::Value& json_member = json["{member.name}"];
                        ''')

                    if not member.fixedSizeArray:
                        out.append(f'''
                            if ({lengthExpr} == 0) {{
                                s.{member.name} = nullptr;
                                if (!json_member.isString() || strcmp(json_member.asCString(), "NULL") != 0) {{
                                    Warn() << "{member.name} is not NULL but its length is zero";
                                }}
                            }} else ''')

                    out.append(f'''{{
                                if (json_member.isArray()) {{
                                    if (json_member.size() == {lengthExpr}) {{
                        ''')

                    if member.fixedSizeArray:
                        out.append(f''' for (Json::Value::ArrayIndex i = 0; i < json_member.size(); ++i) {{
                                            s.{member.name}[i] = {self.genTypeMethod(member.type)}(json_member[i], CreateScope("{member.name}", i));
                                        }}
                            ''')
                    else:
                        out.append(f''' auto dst_buffer = AllocMem<{member.type}>(json_member.size());
                                        for (Json::Value::ArrayIndex i = 0; i < json_member.size(); ++i) {{
                                            dst_buffer[i] = {self.genTypeMethod(member.type)}(json_member[i], CreateScope("{member.name}", i));
                                        }}
                                        s.{member.name} = dst_buffer;
                            ''')

                    out.append(f''' }} else {{
                                        Error() << "{member.name} array size (" << json_member.size() << ") does not match expected length (" << {lengthExpr} << ")";
                                    }}
                                }} else ''')

                    if (not member.fixedSizeArray and member.noAutoValidity) or member.optional:
                        out.append(f'''if (json_member.isString() && strcmp(json_member.asCString(), "NULL") == 0) {{
                                        s.{member.name} = nullptr;
                                    }} else {{
                                        Error() << "{member.name} is not an array and is not NULL";
                                    }}
                            ''')
                    else:
                        out.append(f'''{{
                                   Error() << "{member.name} is not an array";
                                   }}''')

                    out.append(f'''}}
                        }}
                        ''')

                case TypeCategory.HANDLE | TypeCategory.ENUM | TypeCategory.FLAGS | TypeCategory.BITMASK | TypeCategory.BASIC | TypeCategory.STRUCT:
                    out.append(f's.{member.name} = {self.genTypeMethod(member.type)}(json["{member.name}"], CreateScope("{member.name}"));')

        out.append(f'''
                return s;
            }}
        ''')

        out.extend(guard_helper.add_guard(None))

        self.parse_struct_contents_methods.extend(out)
        self.generatedMethods[struct.name] = f'parse_{struct.name}_contents'
        return self.generatedMethods[struct.name]
