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
from generators.generator_utils import PipelineJsonHelper, StructMemberHelper, TypeCategory

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
            #include <math.h>

            #include "vksc_pipeline_json_base.hpp"

            namespace pcjson {

            class GeneratorBase : protected Base {
            ''')

        self.generatedMethods: dict[str] = dict()

        self.gen_basic_methods: list[str] = []
        self.gen_Handle_methods: list[str] = []
        self.gen_Enum_c_str_methods: list[str] = []
        self.gen_Enum_methods: list[str] = []
        self.gen_Flags_methods: list[str] = []
        self.gen_structChain_methods: list[str] = []
        self.gen_struct_contents_methods: list[str] = []

        self.basicTypes: list[str] = []

        self.genManualMethods()
        self.genBasicMethods()

        for struct in [self.vk.structs[x] for x in PipelineJsonHelper.getAllParseGenStructs()]:
            self.genStructChainMethod(struct)

        out.append(f'''
              private:
                {"".join(self.gen_Handle_methods)}
                {"".join(self.gen_Enum_c_str_methods)}
              protected:
                {"".join(self.gen_basic_methods)}
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
                json["codeSize"] = gen_size_t(s.codeSize, CreateScope("codeSize"));
                json["pCode"] = gen_binary(s.pCode, s.codeSize);
                return json;
            }
            ''')

        # VkPhysicalDeviceFeatures2 structure filtering is another special case for which we provide an explicit API
        self.gen_basic_methods.append('''
            void* filter_VkPhysicalDeviceFeatures2(const void* pDeviceCreateInfoPNext, const LocationScope& l) {
                auto base = AllocMem<VkPhysicalDeviceFeatures2>();
                *base = {};
                void* pnext = nullptr;
                auto p = reinterpret_cast<const VkBaseInStructure*>(pDeviceCreateInfoPNext);
                while (p != nullptr) {
                    switch (p->sType) {
                        case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2: {
                            *base = *reinterpret_cast<const VkPhysicalDeviceFeatures2*>(p);
                            break;
                        }
            ''')
        guard_helper = PlatformGuardHelper()
        for feature_struct_name in self.vk.structs['VkPhysicalDeviceFeatures2'].extendedBy:
            feature_struct = self.vk.structs[feature_struct_name]
            self.gen_basic_methods.extend(guard_helper.add_guard(feature_struct.protect))
            self.gen_basic_methods.append(f'''
                        case {feature_struct.sType}: {{
                            auto s = AllocMem<{feature_struct.name}>();
                            *s = *reinterpret_cast<const {feature_struct.name}*>(p);
                            s->pNext = pnext;
                            pnext = s;
                            break;
                        }}
                ''')
        self.gen_basic_methods.extend(guard_helper.add_guard(None))
        self.gen_basic_methods.append('''
                        default: break;
                    }
                    p = p->pNext;
                }

                base->sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
                base->pNext = pnext;

                return base;
            }
            ''')

    def genBasicMethods(self):
        # For basic types we generate simple wrappers
        self.basicTypes = ['int8_t', 'uint8_t', 'int16_t', 'uint16_t', 'int32_t', 'uint32_t', 'int64_t', 'uint64_t', 'float']
        self.basicTypes.extend(['VkDeviceSize', 'VkSampleMask'])

        for typeName in self.basicTypes:
            self.generatedMethods[typeName] = f'gen_{typeName}'
            return_statement = 'return v;' if typeName != 'float' else 'if (std::isnan(v)) return "NaN"; else return v;'
            self.gen_basic_methods.append(f'Json::Value gen_{typeName}(const {typeName} v, const LocationScope&) {{ {return_statement} }}\n')

        self.basicTypes.append('size_t')
        self.basicTypes.append('VkBool32')
        self.generatedMethods['size_t'] = 'gen_size_t'
        self.generatedMethods['VkBool32'] = 'gen_VkBool32'
        self.gen_basic_methods.append('''
            // Apple Clang has distinct size_t type (not typedef)
            // Json::Value CTOR requires disambiguation
            Json::Value gen_size_t(const size_t v, const LocationScope&) { return static_cast<Json::UInt64>(v); }

            Json::Value gen_VkBool32(const VkBool32 v, const LocationScope&) {
                return Json::Value(v ? "VK_TRUE" : "VK_FALSE");
            }
        ''')

        # We also need to add a base64 encoder for encoding binary data
        self.gen_basic_methods.append('''
            std::string gen_string(const char* str) {
                return str;
            }

            Json::Value gen_binary(const void* ptr, const size_t size) {
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
                    size_t num_read = std::min(size_t(3), size - src_idx);

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

                if (result != "NULL") {
                    return result;
                }

                Json::Value result_array = Json::arrayValue;
                result_array.resize(Json::Value::ArrayIndex(size));
                for (size_t i = 0; i < size; i++) {
                    result[i] = data[i];
                }
                return result_array;
            }
            ''')

    def genTypeMethod(self, typeName: str) -> str:
        if typeName in self.generatedMethods:
            # Do not generate methods for types if they already are generated
            return self.generatedMethods[typeName]
        elif typeName in self.vk.handles:
            return self.genHandleMethod(self.vk.handles[typeName])
        elif typeName in self.vk.structs:
            struct = self.vk.structs[typeName]
            if struct.sType is not None:
                # This is an extensible structure, generate struct chain method for it
                return self.genStructChainMethod(struct)
            else:
                # Otherwise generate the regular struct content method
                return self.genStructContentsMethod(struct)
        elif typeName in self.vk.enums:
            return self.genEnumMethod(self.vk.enums[typeName])
        elif typeName in self.vk.bitmasks:
            return self.genBitmaskMethod(self.vk.bitmasks[typeName])
        elif typeName in self.vk.flags:
            return self.genFlagsMethod(self.vk.flags[typeName])
        else:
            raise Exception(f'Unexpected type name "{typeName}"')

    def genHandleMethod(self, handle: Handle) -> str:
        # Handles are converted to uint64_t values through constructor syntax or to NULL
        self.gen_Handle_methods.append(f'''
            Json::Value gen_{handle.name}(const {handle.name} v, const LocationScope&) {{
                if (v == VK_NULL_HANDLE) {{
                    return "";
                }} else {{
                    return uint64_t(v);
                }}
            }}
            ''')
        self.generatedMethods[handle.name] = f'gen_{handle.name}'
        return self.generatedMethods[handle.name]
    
    def genFlagsMethod(self, flags: Flags) -> str:
        if flags.bitmaskName:
            self.genBitmaskMethod(self.vk.bitmasks[flags.bitmaskName])

        out = []
        out.append(f'''
            Json::Value gen_{flags.name}(const {flags.name} v, const LocationScope&) {{
        ''')

        if flags.bitmaskName:
            base_type = "uint32_t" if flags.bitWidth == 32 else "uint64_t"
            out.append(f'''
                if (!v) {{
                    return 0;
                }}
                std::stringstream strm;''')

            multi_bit_flags = [flag for flag in self.vk.bitmasks[flags.bitmaskName].flags if flag.multiBit]
            if multi_bit_flags:
                out.append(f'''
                    std::array<{flags.bitmaskName}, {len(multi_bit_flags)}> multi_bit_flags{{{{
                        {','.join([f'{flags.bitmaskName}::{f.name}' for f in multi_bit_flags])}
                    }}}};
                    std::vector<{flags.bitmaskName}> matched_multi_bit_flags;
                    for (auto multi_bit_flag : multi_bit_flags) {{
                        if (v == multi_bit_flag) {{
                            matched_multi_bit_flags.push_back(multi_bit_flag);
                            if (strm.rdbuf()->in_avail() > 0) {{
                                strm << " | ";
                            }}
                            strm << gen_{flags.bitmaskName}_c_str(static_cast<{flags.bitmaskName}>(multi_bit_flag));
                        }}
                    }}
                    auto isnt_part_of_any_matched_multi_bit_flags = [&](const auto bit){{
                        return std::none_of(matched_multi_bit_flags.begin(), matched_multi_bit_flags.end(), [bit](const auto multi_bit_flag){{ return multi_bit_flag & bit; }});
                    }};''')
            out.append(f'''
                for (int i = 0; i < {flags.bitWidth - 1}; ++i) {{
                    auto bit = {base_type}(1) << i;
                    if ((v & bit) != 0{' && isnt_part_of_any_matched_multi_bit_flags(bit)' if multi_bit_flags else ''}) {{
                        if (strm.rdbuf()->in_avail() > 0) {{
                            strm << " | ";
                        }}
                        strm << gen_{flags.bitmaskName}_c_str(static_cast<{flags.bitmaskName}>(bit));
                    }}
                }}

                return strm.str();
            ''')
        else:
            out.append(f'''
                if (!v) {{
                    return 0;
                }}

                Error() << "Invalid flags value";
                return "Invalid flags value";   
            ''')
        
        out.append(f'''
            }}
        ''')

        self.gen_Flags_methods.extend(out)
        self.generatedMethods[flags.name] = f'gen_{flags.name}'
        return self.generatedMethods[flags.name]
    
    def genEnumMethod(self, enum: Enum) -> str:
        out = []
        out.append(f'''
            const char* gen_{enum.name}_c_str(const {enum.name} v) {{
                switch (v) {{
                    {''.join([f'case {enumField.name}: return "{enumField.name}";' for enumField in enum.fields])}
                        default:
                            break;
                    }}
                    Warn() << "Invalid {enum.name} enum value";
                    return nullptr;
                }}
        ''')

        self.gen_Enum_c_str_methods.extend(out)

        self.gen_Enum_methods.append(f'''
            Json::Value gen_{enum.name}(const {enum.name} v, const LocationScope&) {{
                const char* enum_str = gen_{enum.name}_c_str(v);
                return enum_str ? Json::Value(enum_str) : Json::Value(v);
            }}
        ''')

        self.generatedMethods[enum.name] = f'gen_{enum.name}'
        return self.generatedMethods[enum.name]
    
    def genBitmaskMethod(self, bitmask: Bitmask) -> str:
        if bitmask.name in self.generatedMethods:
            return self.generatedMethods[bitmask.name]

        out = []
        out.append(f'''
            const char* gen_{bitmask.name}_c_str(const {bitmask.name} v) {{
                switch (v) {{
                    {''.join([f'case {flag.name}: return "{flag.name}";' for flag in bitmask.flags])}
                        default:
                            break;
                    }}
                    Error() << "Invalid bitmask value";
                    return "Invalid bitmask value";
                }}
        ''')

        self.gen_Enum_c_str_methods.extend(out)

        self.gen_Enum_methods.append(f'''
            Json::Value gen_{bitmask.name}(const {bitmask.name} v, const LocationScope&) {{
                return gen_{bitmask.name}_c_str(v);
            }}
        ''')

        self.generatedMethods[bitmask.name] = f'gen_{bitmask.name}'
        return self.generatedMethods[bitmask.name]

    def genStructChainMethod(self, struct: Struct) -> str:
        self.genStructContentsMethod(struct)

        out = []
        out.append(f'''
            Json::Value gen_{struct.name}(const {struct.name}& s, const LocationScope& l) {{
                Json::Value json = gen_{struct.name}_contents(s, l);

                json["sType"] = "{struct.sType}";

                auto next = reinterpret_cast<const VkBaseInStructure*>(s.pNext);
                Json::Value* json_next = &json["pNext"];

                while (next != nullptr) {{
                    switch (next->sType) {{
            ''')

        if struct.extendedBy:
            guard_helper = PlatformGuardHelper()

            for extStruct in [self.vk.structs[x] for x in struct.extendedBy]:
                out.extend(guard_helper.add_guard(extStruct.protect))

                out.append(f'''
                            case {extStruct.sType}: {{
                                *json_next = {self.genStructContentsMethod(extStruct)}(
                                    *reinterpret_cast<const {extStruct.name}*>(next),
                                    CreateScope("pNext<{extStruct.name}>", true));
                                (*json_next)["sType"] = "{extStruct.sType}";
                                break;
                            }}
                    ''')

            out.extend(guard_helper.add_guard(None))

        out.append(f'''
                        default:
                            Error() << "Invalid structure type extending {struct.name}: " << next->sType;
                            break;
                    }}
                    next = next->pNext;
                    if (!json_next->isNull()) {{
                        json_next = &(*json_next)["pNext"];
                    }} 
                }}

                *json_next = "NULL";

                return json;
            }};
            ''')

        self.gen_structChain_methods.extend(out)

        self.generatedMethods[struct.name] = f'gen_{struct.name}'
        return self.generatedMethods[struct.name]

    def genStructContentsMethod(self, struct: Struct):
        if struct.name in self.generatedMethods:
            return f'gen_{struct.name}_contents'
        
        guard_helper = PlatformGuardHelper()

        out = []
        out.extend(guard_helper.add_guard(struct.protect))

        # TODO: Unions have to be handled as struct for lack of a better option

        out.append(f'''
            Json::Value gen_{struct.name}_contents (const {struct.name}& s, const LocationScope& l) {{
                Json::Value json;

        ''')

        for member in struct.members:
            if member.name in ['sType', 'pNext']:
                # Do not handle sType and pNext here as it is already handled in genStructChainMethod
                continue

            match StructMemberHelper.getMemberType(self.vk, self.basicTypes, member):
                case TypeCategory.STRING:
                    out.append(f'json["{member.name}"] = gen_string(s.{member.name});')

                case TypeCategory.STRING_FIXED_SIZE_ARRAY | TypeCategory.STRING_ARRAY:
                    raise Exception(f"Unhandled string array")

                case TypeCategory.BINARY:
                    out.append(f'''
                        if (s.{member.length} != 0) {{
                            if (s.{member.name} != nullptr) {{
                                json["{member.name}"] = gen_binary(s.{member.name}, s.{member.length});
                            }} else {{
                        ''')

                    if member.noAutoValidity or member.optional:
                        out.append(f'json["{member.name}"] = "NULL";\n')
                    else:
                        out.append(f'Error() << "{member.name} is NULL but its length is " << s.{member.length};\n')

                    out.append(f'''
                            }}
                        }} else {{
                            json["{member.name}"] = "NULL";
                        }}
                        ''')

                case TypeCategory.POINTER:
                    out.append(f'json["{member.name}"] = s.{member.name} ? {self.genTypeMethod(member.type)}(*s.{member.name}, CreateScope("{member.name}", true)) : "NULL";')

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
                        if ({lengthExpr} != 0) {{
                            Json::Value json_array_{member.name};
                        ''')

                    if not member.fixedSizeArray:
                        out.append(f'if (s.{member.name} != nullptr)')

                    out.append(f'''{{
                                for (Json::Value::ArrayIndex i = 0; i < {lengthExpr}; i++) {{
                                    json_array_{member.name}[i] = {self.genTypeMethod(member.type)}(s.{member.name}[i], CreateScope("{member.name}", i));
                                }}
                                json["{member.name}"] = json_array_{member.name};
                        ''')

                    if not member.fixedSizeArray:
                        out.append('} else {')
                        if member.noAutoValidity or member.optional:
                            out.append(f'json["{member.name}"] = "NULL";\n')
                        else:
                            out.append(f'Error() << "{member.name} is NULL but its length is " << {lengthExpr};\n')

                    out.append(f'''
                            }}
                        }} else {{
                            json["{member.name}"] = "NULL";
                        }}
                    ''')

                case TypeCategory.HANDLE | TypeCategory.ENUM | TypeCategory.FLAGS | TypeCategory.BITMASK | TypeCategory.BASIC | TypeCategory.STRUCT:
                    out.append(f'json["{member.name}"] = {self.genTypeMethod(member.type)}(s.{member.name}, CreateScope("{member.name}"));\n')

        out.append(f'''
                return json;
            }}
        ''')

        out.extend(guard_helper.add_guard(None))

        self.gen_struct_contents_methods.extend(out)
        self.generatedMethods[struct.name] = f'gen_{struct.name}_contents'
        return self.generatedMethods[struct.name]
