/*
 * Copyright (c) 2024 Bronek Kozicki <brok@spamcop.net>
 * Copyright (c) 2024 Al Grant <al.grant@arm.com>
 * Copyright (c) 2024 Tim Northover <tnorthover@apple.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <algorithm>
#include <atomic>
#include <cstdint>

namespace vk_json {
/// NOTE: Based on the implementation found in P0493r3 published at https://github.com/Bronek/wg21-p0493
template <typename T>
static void atomic_max(std::atomic<T>& pv, typename std::atomic<T>::value_type v, std::memory_order m = std::memory_order_seq_cst) {
#ifdef __cpp_lib_atomic_min_max
    std::atomic_fetch_max(*pv, v, m);
#else
    auto t = pv.load(m);
    while (std::max(v, t) != t) {
        if (pv.compare_exchange_weak(t, v, m, m)) return;
    }
    // additional dummy write for release operation
    if (m == std::memory_order_release || m == std::memory_order_acq_rel || m == std::memory_order_seq_cst) pv.fetch_add(0, m);
    return;
#endif
}
}  // namespace vk_json
