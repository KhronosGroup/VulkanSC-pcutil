
// *** THIS FILE IS GENERATED - DO NOT EDIT ***
// See json_test_generator.py for modifications

/*
** Copyright 2025 The Khronos Group Inc.
** Copyright (c) 2025 RasterGrid Kft.
**
** SPDX-License-Identifier: Apache-2.0
*/

#include "json_validator.h"

#include <gtest/gtest.h>

TEST(ValidatePipelineJson, compute_pipeline_0) {
    std::string json_file = std::string(JSON_DATA_PATH) + "compute_pipeline_0.json";
    EXPECT_TRUE(ValidatePipelineJsonFile(json_file.c_str()));
}

TEST(ValidatePipelineJson, compute_pipeline_1) {
    std::string json_file = std::string(JSON_DATA_PATH) + "compute_pipeline_1.json";
    EXPECT_TRUE(ValidatePipelineJsonFile(json_file.c_str()));
}

TEST(ValidatePipelineJson, compute_pipeline_2) {
    std::string json_file = std::string(JSON_DATA_PATH) + "compute_pipeline_2.json";
    EXPECT_TRUE(ValidatePipelineJsonFile(json_file.c_str()));
}

TEST(ValidatePipelineJson, compute_pipeline_3) {
    std::string json_file = std::string(JSON_DATA_PATH) + "compute_pipeline_3.json";
    EXPECT_TRUE(ValidatePipelineJsonFile(json_file.c_str()));
}

TEST(ValidatePipelineJson, compute_w_arbitrary_names) {
    std::string json_file = std::string(JSON_DATA_PATH) + "compute_w_arbitrary_names.json";
    EXPECT_TRUE(ValidatePipelineJsonFile(json_file.c_str()));
}

TEST(ValidatePipelineJson, cube_pc) {
    std::string json_file = std::string(JSON_DATA_PATH) + "cube.pc.json";
    EXPECT_TRUE(ValidatePipelineJsonFile(json_file.c_str()));
}

TEST(ValidatePipelineJson, graphics_pipeline_0) {
    std::string json_file = std::string(JSON_DATA_PATH) + "graphics_pipeline_0.json";
    EXPECT_TRUE(ValidatePipelineJsonFile(json_file.c_str()));
}

TEST(ValidatePipelineJson, graphics_pipeline_1) {
    std::string json_file = std::string(JSON_DATA_PATH) + "graphics_pipeline_1.json";
    EXPECT_TRUE(ValidatePipelineJsonFile(json_file.c_str()));
}

TEST(ValidatePipelineJson, graphics_pipeline_2) {
    std::string json_file = std::string(JSON_DATA_PATH) + "graphics_pipeline_2.json";
    EXPECT_TRUE(ValidatePipelineJsonFile(json_file.c_str()));
}

TEST(ValidatePipelineJson, graphics_pipeline_3) {
    std::string json_file = std::string(JSON_DATA_PATH) + "graphics_pipeline_3.json";
    EXPECT_TRUE(ValidatePipelineJsonFile(json_file.c_str()));
}
