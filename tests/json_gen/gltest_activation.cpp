/*
 * Copyright (c) 2025 The Khronos Group Inc.
 * Copyright (c) 2025 RasterGrid Kft.
 * Copyright (c) 2021, NVIDIA CORPORATION
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <vulkan/pcjson/vksc_pipeline_json.h>
#include <vulkan/vulkan.h>

#include <gtest/gtest.h>

class GenLayerTest : public testing::Test {
  public:
    GenLayerTest() = default;
    GenLayerTest(const GenLayerTest&) = delete;
    GenLayerTest(GenLayerTest&&) = default;
    ~GenLayerTest() = default;

  protected:
};

TEST_F(GenLayerTest, VkSamplerCreateInfo) {}