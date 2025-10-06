/*
 * Copyright (c) 2020-2025 The Khronos Group Inc.
 * Copyright (c) 2019-2025 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <assert.h>
#include <string.h>
#include <vulkan/vulkan.h>

static inline void *vk_alloc(const VkAllocationCallbacks *alloc, size_t size, size_t alignment, VkSystemAllocationScope scope) {
    return alloc->pfnAllocation(alloc->pUserData, size, alignment, scope);
}

static inline void *vk_alloc2(const VkAllocationCallbacks *alloc, const VkAllocationCallbacks *user_alloc, size_t size,
                              size_t alignment, VkSystemAllocationScope scope) {
    return vk_alloc(user_alloc ? user_alloc : alloc, size, alignment, scope);
}

static inline void *vk_zalloc(const VkAllocationCallbacks *alloc, size_t size, size_t alignment, VkSystemAllocationScope scope) {
    void *ptr = alloc->pfnAllocation(alloc->pUserData, size, alignment, scope);
    if (ptr) memset(ptr, 0, size);
    return ptr;
}

static inline void vk_free(const VkAllocationCallbacks *alloc, void *ptr) { alloc->pfnFree(alloc->pUserData, ptr); }

static inline void vk_free2(const VkAllocationCallbacks *alloc, const VkAllocationCallbacks *user_alloc, void *ptr) {
    vk_free(user_alloc ? user_alloc : alloc, ptr);
}
