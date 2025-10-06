/*
 * Copyright (c) 2020-2025 The Khronos Group Inc.
 * Copyright (c) 2020-2025 LunarG, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "allocator.h"
#include <cstdlib>

namespace extension_layer {
// currently none of the extension layers have special alignment requirements so we just use malloc / free.
static VKAPI_ATTR void* VKAPI_CALL DefaultAlloc(void*, size_t size, size_t alignment, VkSystemAllocationScope) {
    return std::malloc(size);
}

static VKAPI_ATTR void VKAPI_CALL DefaultFree(void*, void* pMem) { std::free(pMem); }

static VKAPI_ATTR void* VKAPI_CALL DefaultRealloc(void*, void* pOriginal, size_t size, size_t alignment, VkSystemAllocationScope) {
    return std::realloc(pOriginal, size);
}

const VkAllocationCallbacks kDefaultAllocator = {
    nullptr, DefaultAlloc, DefaultRealloc, DefaultFree, nullptr, nullptr,
};

}  // namespace extension_layer
