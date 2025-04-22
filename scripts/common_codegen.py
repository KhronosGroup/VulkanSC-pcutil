# Copyright (c) 2015-2017, 2019-2025 The Khronos Group Inc.
# Copyright (c) 2015-2017, 2019-2021 Valve Corporation
# Copyright (c) 2015-2017, 2019-2021 LunarG, Inc.
#
# SPDX-License-Identifier: Apache-2.0

import os

# Copyright text prefixing all headers (list of strings).
prefixStrings = ["""
/*
** Copyright (c) 2015-2017, 2019-2025 The Khronos Group Inc.
** Copyright (c) 2015-2017, 2019-2021 Valve Corporation
** Copyright (c) 2015-2017, 2019-2021 LunarG, Inc.
** Copyright (c) 2015-2017, 2019-2021 Google Inc.
**
** SPDX-License-Identifier: Apache-2.0
*/
"""
]


platform_dict = {
    'android' : 'VK_USE_PLATFORM_ANDROID_KHR',
    'fuchsia' : 'VK_USE_PLATFORM_FUCHSIA',
    'ggp': 'VK_USE_PLATFORM_GGP',
    'ios' : 'VK_USE_PLATFORM_IOS_MVK',
    'macos' : 'VK_USE_PLATFORM_MACOS_MVK',
    'metal' : 'VK_USE_PLATFORM_METAL_EXT',
    'vi' : 'VK_USE_PLATFORM_VI_NN',
    'wayland' : 'VK_USE_PLATFORM_WAYLAND_KHR',
    'win32' : 'VK_USE_PLATFORM_WIN32_KHR',
    'xcb' : 'VK_USE_PLATFORM_XCB_KHR',
    'xlib' : 'VK_USE_PLATFORM_XLIB_KHR',
    'xlib_xrandr' : 'VK_USE_PLATFORM_XLIB_XRANDR_EXT',
    'provisional' : 'VK_ENABLE_BETA_EXTENSIONS',
    'directfb' : 'VK_USE_PLATFORM_DIRECTFB_EXT',
    'screen' : 'VK_USE_PLATFORM_SCREEN_QNX',
    'sci' : 'VK_USE_PLATFORM_SCI',
}

#
# Return appropriate feature protect string from 'platform' tag on feature
def GetFeatureProtect(interface):
    """Get platform protection string"""
    platform = interface.get('platform')
    if platform is not None:
        return platform_dict[platform]

    provisional = interface.get('provisional')
    if provisional == 'true':
        return platform_dict['provisional']


# helper to define paths relative to the repo root
def repo_relative(path):
    return os.path.abspath(os.path.join(os.path.dirname(__file__), '..', path))
