#!/usr/bin/env python3

# Copyright (c) 2025 The Khronos Group Inc.
# Copyright (c) 2025 RasterGrid Kft.
#
# SPDX-License-Identifier: Apache-2.0

import os

copyright = """
/*
** Copyright 2025 The Khronos Group Inc.
** Copyright (c) 2025 RasterGrid Kft.
**
** SPDX-License-Identifier: Apache-2.0
*/
"""

predefinedCode = """
#include "json_validator.h"

#include <gtest/gtest.h>

bool ValidatePipelineJson(const char* pipeline_jsonfile)
{
    JsonValidator json_validator;

    const std::string schema_path = "d:\\\\work_pcu\\\\VulkanSC-pcutil\\\\json\\\\vkpcc.json";
    const JsonValidator::RemoteSchemaRemap remote_schema_remap = {
        { "https://schema.khronos.org/vulkan/vk.json", "d:\\\\work_pcu\\\\VulkanSC-pcutil\\\\json\\\\vk.json" }
    };

    auto success = json_validator.LoadAndValidateSchema(schema_path, remote_schema_remap);
    
    if (!success) {
        std::cout << json_validator.GetMessage() << std::endl;
        return success;
    }

    success = json_validator.ValidateJsonFromFile(pipeline_jsonfile);

    if (!success) {
        std::cout << json_validator.GetMessage() << std::endl;
        return success;
    }

    return true;
}

TEST(test_validate_pipeline_json, vkpcc_schema)
{
    JsonValidator json_validator;

    const std::string schema_path = "d:\\\\work_pcu\\\\VulkanSC-pcutil\\\\tests\\\\json\\\\json-schema-draft-04.json";

    auto success = json_validator.LoadAndValidateSchema(schema_path);

    if (!success) {
        std::cout << json_validator.GetMessage() << std::endl;
    }

    EXPECT_TRUE(success);

    const std::string json_file = "d:\\\\work_pcu\\\\VulkanSC-pcutil\\\\json\\\\vkpcc.json";
    success = json_validator.ValidateJsonFromFile(json_file);

    if (!success) {
        std::cout << json_validator.GetMessage() << std::endl;
        return;
    }

    EXPECT_TRUE(success);
}
"""

validatePipelineJsonTestcase = """
TEST(test_validate_pipeline_json, {pipeline_json}) {{
    const char* json_file = "d:\\\\work_pcu\\\\VulkanSC-pcutil\\\\tests\\\\json\\\\data\\\\{pipeline_json}.json";
    EXPECT_TRUE(ValidatePipelineJson(json_file));
}}
"""

if __name__ == '__main__':
    
    with open("../tests/json/json_test.cpp", "w") as out:
        out.write(copyright)
        out.write(predefinedCode)

        pipeline_jsons = os.listdir("../tests/json/data");

        for pipeline_json in pipeline_jsons:
            pipeline_json = pipeline_json[:-5]
            out.write(validatePipelineJsonTestcase.format(pipeline_json = pipeline_json))
