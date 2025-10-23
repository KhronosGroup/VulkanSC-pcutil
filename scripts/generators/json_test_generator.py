#!/usr/bin/python3 -i
#
# Copyright (c) 2025 The Khronos Group Inc.
# Copyright (c) 2025 RasterGrid Kft.
#
# SPDX-License-Identifier: Apache-2.0

import os

from base_generator import BaseGenerator
import common_codegen

class JsonTestGenerator(BaseGenerator):
    def __init__(self):
        BaseGenerator.__init__(self)
    
    def generate(self):
        self.write(f"""
            // *** THIS FILE IS GENERATED - DO NOT EDIT ***
            // See {os.path.basename(__file__)} for modifications

            /*
            ** Copyright 2025 The Khronos Group Inc.
            ** Copyright (c) 2025 RasterGrid Kft.
            **
            ** SPDX-License-Identifier: Apache-2.0
            */

            #include "json_validator.h"

            #include <gtest/gtest.h>
            """)

        validatePipelineJsonTestcase = """
            TEST(ValidatePipelineJson, {test_case}) {{
                std::string json_file = std::string(JSON_DATA_PATH) + "{pipeline_json}.json";
                EXPECT_TRUE(ValidatePipelineJsonFile(json_file.c_str()));
            }}
        """

        pipeline_jsons = os.listdir(common_codegen.repo_relative("tests/json/data"))

        for pipeline_json in sorted(pipeline_jsons):
            pipeline_json = pipeline_json[:-5]
            test_case = pipeline_json.replace(".", "_")
            self.write(validatePipelineJsonTestcase.format(pipeline_json = pipeline_json, test_case = test_case))
