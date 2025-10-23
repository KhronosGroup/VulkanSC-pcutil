/*
 * Copyright (c) 2020-2025 The Khronos Group Inc.
 * Copyright (c) 2020-2025 LunarG, Inc.
 * Copyright (c) 2020-2025 Advanced Micro Devices, Inc.
 * Copyright (c) 2022-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * Copyright (c) 2025 RasterGrid Kft.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "vk_json_layer.hpp"

#include "log.h"

#ifdef _WIN32
#include <windows.h>
#endif
#ifdef __linux__
#include <unistd.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <utility>
#include <string>
#include <algorithm>

namespace vk_json {

std::string getProcessName() {
    static constexpr int MAX_NAME_SIZE = 255;
    static char m_exeName[MAX_NAME_SIZE] = {0};
    static bool init = [&]() {
#ifdef _WIN32
        if (GetModuleFileName(0, m_exeName, MAX_NAME_SIZE) == 0) {
            LOG("[%s] ERROR: Failed to obtaining process name.", VK_EXT_PIPELINE_PROPERTIES_EXTENSION_NAME);
            exit(-1);
        }

        // Some raw C code to get the .exe name on Windows.
        char* p = m_exeName + (strnlen(m_exeName, MAX_NAME_SIZE) - 1);
        int writePos = 0;

        while (*p != '.') --p;
        while (*p != '\\') --p;
        for (++p; *p != '.'; ++p) m_exeName[writePos++] = *p;
        m_exeName[writePos] = 0;
#endif

#ifdef __linux__
        char exeName[1024];
        int ret = readlink("/proc/self/exe", exeName, sizeof(exeName) - 1);
        if (ret == -1) {
            LOG("[%s] ERROR: Failed to obtaining process name.", VK_EXT_PIPELINE_PROPERTIES_EXTENSION_NAME);
            exit(-1);
        }
        exeName[ret] = 0;

        char* p = exeName + (ret - 1);
        int writePos = 0;
        while (*p != '/') m_exeName[writePos++] = *p--;

        m_exeName[writePos] = 0;
        std::string tmp(m_exeName);
        std::reverse(tmp.begin(), tmp.end());
        strncpy(m_exeName, tmp.c_str(), sizeof(m_exeName) - 1);
#endif
        return true;
    }();
    std::ignore = init;
    return m_exeName;
}

std::string getBaseDirectoryPath() {
    std::string m_baseDir = getenv("VK_JSON_FILE_PATH") ? getenv("VK_JSON_FILE_PATH") : "";
    if (!m_baseDir.empty()) {
#ifdef _WIN32
        m_baseDir += "\\";
#else
        m_baseDir += "/";
#endif
    } else {
        LOG("[%s] ERROR: Failed to query VK_JSON_FILE_PATH. Please set the environment variable.",
            VK_EXT_PIPELINE_PROPERTIES_EXTENSION_NAME);
        exit(-1);
    }
    return m_baseDir;
}

}  // namespace vk_json
