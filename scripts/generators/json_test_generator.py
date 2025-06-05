#!/usr/bin/env python3

# Copyright (c) 2025 The Khronos Group Inc.
# Copyright (c) 2025 RasterGrid Kft.
#
# SPDX-License-Identifier: Apache-2.0

import os

from base_generator import BaseGenerator

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

    const std::string schema_path = std::string(SCHEMA_PATH) + "vksc_pipeline_schema.json";

    auto success = json_validator.LoadAndValidateSchema(schema_path);
    
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
"""

validatePipelineJsonTestcase = """
TEST(test_validate_pipeline_json, {test_case}) {{
    std::string json_file = std::string(JSON_DATA_PATH) + "{pipeline_json}.json";
    EXPECT_TRUE(ValidatePipelineJson(json_file.c_str()));
}}
"""

class JsonTestGenerator(BaseGenerator):
    def __init__(self):
        BaseGenerator.__init__(self)
    
    def generate(self):
        self.write(copyright)
        self.write(predefinedCode)

        pipeline_jsons = os.listdir("../tests/json/data");

        for pipeline_json in pipeline_jsons:
            pipeline_json = pipeline_json[:-5]
            test_case = pipeline_json.replace(".", "_")
            self.write(validatePipelineJsonTestcase.format(pipeline_json = pipeline_json, test_case = test_case))
