/*
 * Copyright (c) 2021-2025 The Khronos Group Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdlib.h>

// Basic Logging Support
#ifdef __ANDROID__
#include <android/log.h>
#define LOG(...) ((void)__android_log_print(ANDROID_LOG_INFO, "VulkanExtensionLayer", __VA_ARGS__))
#define LOG_FATAL(...)                                                                 \
    (void)__android_log_print(ANDROID_LOG_FATAL, "VulkanExtensionLayer", __VA_ARGS__); \
    exit(1);
#else  // __ANDROID__
#include <stdio.h>
#define LOG(...)                  \
    fprintf(stdout, __VA_ARGS__); \
    fflush(stdout);
#endif

// Define own assert because <cassert> on android will not actually assert anything
#ifdef __ANDROID__
#define ASSERT(condition)                                                      \
    do {                                                                       \
        if (!(condition)) {                                                    \
            LOG_FATAL("ASSERT: %s at %s:%d\n", #condition, __FILE__, __LINE__) \
        }                                                                      \
    } while (0)
#else  // __ANDROID__
#ifdef __cplusplus
#include <cassert>
#else  // __cplusplus
#include <assert.h>
#endif  // __cplusplus
#define ASSERT(condition) assert(condition);
#endif
