
/*
** Copyright 2025 The Khronos Group Inc.
** Copyright (c) 2025 RasterGrid Kft.
**
** SPDX-License-Identifier: Apache-2.0
*/

#include "json_validator.h"

#include <gtest/gtest.h>

bool ValidatePipelineJson(const char* pipeline_jsonfile) {
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

TEST(test_validate_pipeline_json, compute_pipeline_0) {
    std::string json_file = std::string(JSON_DATA_PATH) + "compute_pipeline_0.json";
    EXPECT_TRUE(ValidatePipelineJson(json_file.c_str()));
}

TEST(test_validate_pipeline_json, compute_pipeline_1) {
    std::string json_file = std::string(JSON_DATA_PATH) + "compute_pipeline_1.json";
    EXPECT_TRUE(ValidatePipelineJson(json_file.c_str()));
}

TEST(test_validate_pipeline_json, compute_pipeline_2) {
    std::string json_file = std::string(JSON_DATA_PATH) + "compute_pipeline_2.json";
    EXPECT_TRUE(ValidatePipelineJson(json_file.c_str()));
}

TEST(test_validate_pipeline_json, compute_pipeline_3) {
    std::string json_file = std::string(JSON_DATA_PATH) + "compute_pipeline_3.json";
    EXPECT_TRUE(ValidatePipelineJson(json_file.c_str()));
}

TEST(test_validate_pipeline_json, compute_w_arbitrary_names) {
    std::string json_file = std::string(JSON_DATA_PATH) + "compute_w_arbitrary_names.json";
    EXPECT_TRUE(ValidatePipelineJson(json_file.c_str()));
}

TEST(test_validate_pipeline_json, cube_pc) {
    std::string json_file = std::string(JSON_DATA_PATH) + "cube.pc.json";
    EXPECT_TRUE(ValidatePipelineJson(json_file.c_str()));
}

TEST(test_validate_pipeline_json, graphics_pipeline_0) {
    std::string json_file = std::string(JSON_DATA_PATH) + "graphics_pipeline_0.json";
    EXPECT_TRUE(ValidatePipelineJson(json_file.c_str()));
}

TEST(test_validate_pipeline_json, graphics_pipeline_1) {
    std::string json_file = std::string(JSON_DATA_PATH) + "graphics_pipeline_1.json";
    EXPECT_TRUE(ValidatePipelineJson(json_file.c_str()));
}

TEST(test_validate_pipeline_json, graphics_pipeline_2) {
    std::string json_file = std::string(JSON_DATA_PATH) + "graphics_pipeline_2.json";
    EXPECT_TRUE(ValidatePipelineJson(json_file.c_str()));
}

TEST(test_validate_pipeline_json, graphics_pipeline_3) {
    std::string json_file = std::string(JSON_DATA_PATH) + "graphics_pipeline_3.json";
    EXPECT_TRUE(ValidatePipelineJson(json_file.c_str()));
}
