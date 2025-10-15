/*
 * Copyright (c) 2025 The Khronos Group Inc.
 * Copyright (c) 2025 RasterGrid Kft.
 * Copyright (c) 2021, NVIDIA CORPORATION
 *
 * SPDX-License-Identifier: Apache-2.0
 */

 #pragma once

#include <vulkan/vulkan.h>

#include <gtest/gtest.h>


class SAXPY {
    public:
    SAXPY(VkInstance inst, VkDevice dev, size_t push_constant_size = 4);
    SAXPY(const SAXPY&) = delete;
    SAXPY(SAXPY&&) = delete;
    ~SAXPY();

    static VkInstance create_instance();
    static VkDevice create_device(VkInstance instance);
    static const int pipeline_increment = 3;
    VkPipeline get_pipeline() const;

    void run();

    private:
    VkInstance instance;
    VkDevice device;
    VkShaderModule shader_module;
    VkBuffer buf_x, buf_y;
    VkDeviceMemory mem_x, mem_y;
    VkDescriptorSetLayout buf_ds_layout;
    VkCommandPool cmd_pool;
    VkCommandBuffer cmd_buf;
    VkPipelineLayout pipeline_layout;
    VkPipelineCache pipeline_cache;
    VkPipeline pipeline;
    VkDescriptorPool desc_pool;
    VkFence fence;
    size_t sizeof_push_constant;
};
