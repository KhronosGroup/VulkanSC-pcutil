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

#define VKCHECK(func)                                                                                     \
    do {                                                                                                  \
        if (VkResult res = func; res < 0) {                                                               \
            std::string msg{#func};                                                                       \
            msg.append(std::to_string(res));                                                              \
            GTEST_MESSAGE_AT_(__FILE__, __LINE__, msg.c_str(), ::testing::TestPartResult::kFatalFailure); \
            throw testing::AssertionException(                                                            \
                testing::TestPartResult(testing::TestPartResult::kFatalFailure, __FILE__, __LINE__, "")); \
        }                                                                                                 \
    } while (0)
