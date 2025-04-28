#!/usr/bin/env python3
#
# Copyright 2013-2024 The Khronos Group Inc.
#
# SPDX-License-Identifier: Apache-2.0

import argparse
import os
import pdb
import re
import sys
import copy
import time
import xml.etree.ElementTree as etree

# Simple timer functions
startTime = None


def startTimer(timeit):
    global startTime
    if timeit:
        startTime = time.process_time()


def endTimer(timeit, msg):
    global startTime
    if timeit and startTime is not None:
        endTime = time.process_time()
        logDiag(msg, endTime - startTime)
        startTime = None


def makeREstring(strings, default=None, strings_are_regex=False):
    """Turn a list of strings into a regexp string matching exactly those strings."""
    if strings or default is None:
        if not strings_are_regex:
            strings = (re.escape(s) for s in strings)
        return '^(' + '|'.join(strings) + ')$'
    return default


def makeGenOpts(args):
    """Returns a directory of [ generator function, generator options ] indexed
    by specified short names. The generator options incorporate the following
    parameters:

    args is a parsed argument object; see below for the fields that are used."""
    global genOpts
    genOpts = {}

    # Additional extensions to include (list of extensions)
    extensions = args.extension

    # Extensions to remove (list of extensions)
    removeExtensions = args.removeExtensions

    # Extensions to emit (list of extensions)
    emitExtensions = args.emitExtensions

    # Features to include (list of features)
    features = args.feature

    # Whether to disable inclusion protect in headers
    protect = args.protect

    # Output target directory
    directory = args.directory

    # Descriptive names for various regexp patterns used to select
    # versions and extensions
    allFeatures = allExtensions = r'.*'

    # Turn lists of names/patterns into matching regular expressions
    addExtensionsPat     = makeREstring(extensions, None)
    removeExtensionsPat  = makeREstring(removeExtensions, None)
    emitExtensionsPat    = makeREstring(emitExtensions, allExtensions)
    featuresPat          = makeREstring(features, allFeatures)

    # Copyright text prefixing all headers (list of strings).
    # The SPDX formatting below works around constraints of the 'reuse' tool
    prefixStrings = [
        '/*',
        '** Copyright 2015-2024 The Khronos Group Inc.',
        '**',
        '** SPDX-License-Identifier' + ': Apache-2.0',
        '*/',
        ''
    ]

    # Text specific to Vulkan headers
    vkPrefixStrings = [
        '/*',
        '** This header is generated from the Khronos Vulkan XML API Registry.',
        '**',
        '*/',
        ''
    ]

    # Defaults for generating re-inclusion protection wrappers (or not)
    protectFile = protect

    # An API style conventions object
    from apiconventions import APIConventions
    conventions = APIConventions()

    # Platform extensions, in their own header files
    # Each element of the platforms[] array defines information for
    # generating a single platform:
    #   [0] is the generated header file name
    #   [1] is the set of platform extensions to generate
    #   [2] is additional extensions whose interfaces should be considered,
    #   but suppressed in the output, to avoid duplicate definitions of
    #   dependent types like VkDisplayKHR and VkSurfaceKHR which come from
    #   non-platform extensions.

    # Track all platform extensions, for exclusion from vulkan_core.h
    allPlatformExtensions = []

    # Extensions suppressed for all WSI platforms (WSI extensions required
    # by all platforms)
    commonSuppressExtensions = [ 'VK_KHR_display', 'VK_KHR_swapchain' ]

    # Extensions required and suppressed for beta "platform". This can
    # probably eventually be derived from the requires= attributes of
    # the extension blocks.
    betaRequireExtensions = [
        'VK_KHR_portability_subset',
        'VK_NV_displacement_micromap',
        'VK_AMDX_shader_enqueue',
    ]

    betaSuppressExtensions = [
        'VK_EXT_opacity_micromap',
        'VK_KHR_pipeline_library',
    ]

    platforms = [
        [ 'vulkan_android.h',     [ 'VK_KHR_android_surface',
                                    'VK_ANDROID_external_memory_android_hardware_buffer',
                                    'VK_ANDROID_external_format_resolve'
                                                                  ], commonSuppressExtensions +
                                                                     [ 'VK_KHR_format_feature_flags2',
                                                                     ] ],
        [ 'vulkan_fuchsia.h',     [ 'VK_FUCHSIA_imagepipe_surface',
                                    'VK_FUCHSIA_external_memory',
                                    'VK_FUCHSIA_external_semaphore',
                                    'VK_FUCHSIA_buffer_collection' ], commonSuppressExtensions ],
        [ 'vulkan_ggp.h',         [ 'VK_GGP_stream_descriptor_surface',
                                    'VK_GGP_frame_token'          ], commonSuppressExtensions ],
        [ 'vulkan_ios.h',         [ 'VK_MVK_ios_surface'          ], commonSuppressExtensions ],
        [ 'vulkan_macos.h',       [ 'VK_MVK_macos_surface'        ], commonSuppressExtensions ],
        [ 'vulkan_vi.h',          [ 'VK_NN_vi_surface'            ], commonSuppressExtensions ],
        [ 'vulkan_wayland.h',     [ 'VK_KHR_wayland_surface'      ], commonSuppressExtensions ],
        [ 'vulkan_win32.h',       [ 'VK_.*_win32(|_.*)', 'VK_.*_winrt(|_.*)', 'VK_EXT_full_screen_exclusive' ],
                                                                     commonSuppressExtensions +
                                                                     [ 'VK_KHR_external_semaphore',
                                                                       'VK_KHR_external_memory_capabilities',
                                                                       'VK_KHR_external_fence',
                                                                       'VK_KHR_external_fence_capabilities',
                                                                       'VK_KHR_get_surface_capabilities2',
                                                                       'VK_NV_external_memory_capabilities',
                                                                     ] ],
        [ 'vulkan_xcb.h',         [ 'VK_KHR_xcb_surface'          ], commonSuppressExtensions ],
        [ 'vulkan_xlib.h',        [ 'VK_KHR_xlib_surface'         ], commonSuppressExtensions ],
        [ 'vulkan_directfb.h',    [ 'VK_EXT_directfb_surface'     ], commonSuppressExtensions ],
        [ 'vulkan_xlib_xrandr.h', [ 'VK_EXT_acquire_xlib_display' ], commonSuppressExtensions ],
        [ 'vulkan_metal.h',       [ 'VK_EXT_metal_surface',
                                    'VK_EXT_metal_objects'        ], commonSuppressExtensions ],
        [ 'vulkan_screen.h',      [ 'VK_QNX_screen_surface',
                                    'VK_QNX_external_memory_screen_buffer' ], commonSuppressExtensions ],
        [ 'vulkan_sci.h',         [ 'VK_NV_external_sci_sync',
                                    'VK_NV_external_sci_sync2',
                                    'VK_NV_external_memory_sci_buf'], commonSuppressExtensions ],
        [ 'vulkan_beta.h',        betaRequireExtensions,             betaSuppressExtensions ],
    ]

    for platform in platforms:
        allPlatformExtensions += platform[1]

    # Header for core API + extensions.
    # To generate just the core API,
    # change to 'defaultExtensions = None' below.
    #
    # By default this adds all enabled, non-platform extensions.
    # It removes all platform extensions (from the platform headers options
    # constructed above) as well as any explicitly specified removals.

    removeExtensionsPat = makeREstring(
        allPlatformExtensions + removeExtensions, None, strings_are_regex=True)

    # Vulkan versions to include for SC header - SC *removes* features from 1.0/1.1/1.2
    scVersions = makeREstring(['VK_VERSION_1_0', 'VK_VERSION_1_1', 'VK_VERSION_1_2', 'VKSC_VERSION_1_0'])

    # Try to set up Vulkan SC JSON generators if the needed modules are available
    try:
        from json_parser import JSONParserGenerator, JSONParserOptions
        from schema_generator import SchemaGeneratorOptions, SchemaOutputGenerator
        from json_generator import JSONGeneratorOptions, JSONOutputGenerator
        from json_h_generator import JSONHeaderOutputGenerator, JSONHeaderGeneratorOptions
        from json_c_generator import JSONCOutputGenerator, JSONCGeneratorOptions
        from json_gen_generator import JSONGenGeneratorOptions, JSONGenOutputGenerator, COMMON_CODEGEN_JSON_GEN

        genOpts['vk.json'] = [
            SchemaOutputGenerator,
            SchemaGeneratorOptions(
                conventions       = conventions,
                filename          = 'vk.json',
                directory         = directory,
                apiname           = 'vulkan',
                profile           = None,
                versions          = scVersions,
                emitversions      = scVersions,
                defaultExtensions = 'vulkan',
                addExtensions     = addExtensionsPat,
                removeExtensions  = removeExtensionsPat,
                emitExtensions    = emitExtensionsPat,
                prefixText        = prefixStrings + vkPrefixStrings,
                genFuncPointers   = True,
                protectFile       = protectFile,
                protectFeature    = False,
                protectProto      = '#ifndef',
                protectProtoStr   = 'VK_NO_PROTOTYPES',
                apicall           = 'VKAPI_ATTR ',
                apientry          = 'VKAPI_CALL ',
                apientryp         = 'VKAPI_PTR *',
                alignFuncParam    = 48)
            ]

        genOpts['vulkan_json_layer_data.hpp'] = [
            JSONOutputGenerator,
            JSONGeneratorOptions(
                conventions       = conventions,
                filename          = 'vulkan_json_layer_data.hpp',
                directory         = directory,
                apiname           = 'vulkan',
                profile           = None,
                versions          = featuresPat,
                emitversions      = featuresPat,
                defaultExtensions = 'vulkan',
                addExtensions     = addExtensionsPat,
                removeExtensions  = None,
                emitExtensions    = None,
                vulkanLayer       = True,
                prefixText        = prefixStrings + vkPrefixStrings,
                genFuncPointers   = True,
                protectFile       = protectFile,
                protectFeature    = False,
                protectProto      = '#ifndef',
                protectProtoStr   = 'VK_NO_PROTOTYPES',
                apicall           = 'VKAPI_ATTR ',
                apientry          = 'VKAPI_CALL ',
                apientryp         = 'VKAPI_PTR *',
                alignFuncParam    = 48)
            ]

        genOpts['vulkan_json_data.hpp'] = [
        JSONOutputGenerator,
        JSONGeneratorOptions(
            conventions       = conventions,
            filename          = 'vulkan_json_data.hpp',
            directory         = directory,
            apiname           = 'vulkan',
            profile           = None,
            versions          = scVersions,
            emitversions      = scVersions,
            defaultExtensions = 'vulkan',
            addExtensions     = addExtensionsPat,
            removeExtensions  = removeExtensionsPat,
            emitExtensions    = emitExtensionsPat,
            vulkanLayer       = False,
            prefixText        = prefixStrings + vkPrefixStrings,
            genFuncPointers   = True,
            protectFile       = protectFile,
            protectFeature    = False,
            protectProto      = '#ifndef',
            protectProtoStr   = 'VK_NO_PROTOTYPES',
            apicall           = 'VKAPI_ATTR ',
            apientry          = 'VKAPI_CALL ',
            apientryp         = 'VKAPI_PTR *',
            alignFuncParam    = 48)
        ]

        # keep any relevant platform extensions for the following generators
        # (needed for e.g. the vulkan_sci extensions)
        explicitRemoveExtensionsPat = makeREstring(
            removeExtensions, None, strings_are_regex=True)

        # Raw C header file generator.
        genOpts['vulkan_json_gen.h'] = [
            JSONHeaderOutputGenerator,
            JSONHeaderGeneratorOptions(
                conventions       = conventions,
                filename          = 'vulkan_json_gen.h',
                directory         = directory,
                apiname           = 'vulkansc',
                profile           = None,
                versions          = scVersions,
                emitversions      = scVersions,
                defaultExtensions = 'vulkansc',
                addExtensions     = addExtensionsPat,
                removeExtensions  = explicitRemoveExtensionsPat,
                emitExtensions    = emitExtensionsPat,
                prefixText        = prefixStrings + vkPrefixStrings,
                genFuncPointers   = True,
                protectFile       = protectFile,
                protectFeature    = False,
                protectProto      = '#ifndef',
                protectProtoStr   = 'VK_NO_PROTOTYPES',
                apicall           = 'VKAPI_ATTR ',
                apientry          = 'VKAPI_CALL ',
                apientryp         = 'VKAPI_PTR *',
                alignFuncParam    = 48)
            ]

        # Raw C source file generator.
        genOpts['vulkan_json_gen.c'] = [
            JSONCOutputGenerator,
            JSONCGeneratorOptions(
                conventions       = conventions,
                filename          = 'vulkan_json_gen.c',
                directory         = directory,
                apiname           = 'vulkansc',
                profile           = None,
                versions          = scVersions,
                emitversions      = scVersions,
                defaultExtensions = 'vulkansc',
                addExtensions     = addExtensionsPat,
                removeExtensions  = explicitRemoveExtensionsPat,
                emitExtensions    = emitExtensionsPat,
                prefixText        = prefixStrings + vkPrefixStrings,
                genFuncPointers   = True,
                protectFile       = protectFile,
                protectFeature    = False,
                protectProto      = '#ifndef',
                protectProtoStr   = 'VK_NO_PROTOTYPES',
                apicall           = 'VKAPI_ATTR ',
                apientry          = 'VKAPI_CALL ',
                apientryp         = 'VKAPI_PTR *',
                alignFuncParam    = 48)
            ]

        genOpts['vulkan_json_parser.hpp'] = [
            JSONParserGenerator,
            JSONParserOptions(
                conventions       = conventions,
                filename          = 'vulkan_json_parser.hpp',
                directory         = directory,
                apiname           = 'vulkansc',
                profile           = None,
                versions          = scVersions,
                emitversions      = scVersions,
                defaultExtensions = 'vulkansc',
                addExtensions     = addExtensionsPat,
                removeExtensions  = explicitRemoveExtensionsPat,
                emitExtensions    = emitExtensionsPat,
                prefixText        = prefixStrings + vkPrefixStrings,
                genFuncPointers   = True,
                protectFile       = protectFile,
                protectFeature    = False,
                protectProto      = '#ifndef',
                protectProtoStr   = 'VK_NO_PROTOTYPES',
                apicall           = 'VKAPI_ATTR ',
                apientry          = 'VKAPI_CALL ',
                apientryp         = 'VKAPI_PTR *',
                alignFuncParam    = 48)
            ]

        # API dump generator options for json_gen.cpp
        genOpts['json_gen.cpp'] = [
            JSONGenOutputGenerator,
            JSONGenGeneratorOptions(
                conventions       = conventions,
                input             = COMMON_CODEGEN_JSON_GEN,
                filename          = 'json_gen.cpp',
                directory         = directory,
                apiname           = 'vulkan',
                genpath           = None,
                profile           = None,
                versions          = featuresPat,
                emitversions      = featuresPat,
                defaultExtensions = 'vulkan',
                addExtensions     = addExtensionsPat,
                removeExtensions  = removeExtensionsPat,
                emitExtensions    = emitExtensionsPat,
                prefixText        = prefixStrings + vkPrefixStrings,
                genFuncPointers   = True,
                protectFile       = protect,
                protectFeature    = False,
                protectProto      = None,
                protectProtoStr   = 'VK_NO_PROTOTYPES',
                apicall           = 'VKAPI_ATTR ',
                apientry          = 'VKAPI_CALL ',
                apientryp         = 'VKAPI_PTR *',
                alignFuncParam    = 48,
                expandEnumerants = False)
        ]
    except ImportError:
        # Module dependencies are not available for Vulkan SC JSON generation
        pass


def genTarget(args):
    """Create an API generator and corresponding generator options based on
    the requested target and command line options.

    This is encapsulated in a function so it can be profiled and/or timed.
    The args parameter is a parsed argument object containing the following
    fields that are used:

    - target - target to generate
    - directory - directory to generate it in
    - protect - True if re-inclusion wrappers should be created
    - extensions - list of additional extensions to include in generated interfaces"""

    # Create generator options with parameters specified on command line
    makeGenOpts(args)

    # Select a generator matching the requested target
    if args.target in genOpts:
        createGenerator = genOpts[args.target][0]
        options = genOpts[args.target][1]

        logDiag('* Building', options.filename)
        logDiag('* options.apiname           =', options.apiname)
        logDiag('* options.versions          =', options.versions)
        logDiag('* options.emitversions      =', options.emitversions)
        logDiag('* options.defaultExtensions =', options.defaultExtensions)
        logDiag('* options.addExtensions     =', options.addExtensions)
        logDiag('* options.removeExtensions  =', options.removeExtensions)
        logDiag('* options.emitExtensions    =', options.emitExtensions)
        logDiag('* options.emitSpirv         =', options.emitSpirv)
        logDiag('* options.emitFormats       =', options.emitFormats)

        gen = createGenerator(errFile=errWarn,
                              warnFile=errWarn,
                              diagFile=diag)
        return (gen, options)
    else:
        logErr('No generator options for unknown target:', args.target)
        return None


# -feature name
# -extension name
# For both, "name" may be a single name, or a space-separated list
# of names, or a regular expression.
if __name__ == '__main__':
    parser = argparse.ArgumentParser()

    parser.add_argument('-defaultExtensions', action='store',
                        default=None,
                        help='Specify a single class of extensions to add to targets')
    parser.add_argument('-extension', action='append',
                        default=[],
                        help='Specify an extension or extensions to add to targets')
    parser.add_argument('-removeExtensions', action='append',
                        default=[],
                        help='Specify an extension or extensions to remove from targets')
    parser.add_argument('-emitExtensions', action='append',
                        default=[],
                        help='Specify an extension or extensions to emit in targets')
    parser.add_argument('-emitSpirv', action='append',
                        default=[],
                        help='Specify a SPIR-V extension or capability to emit in targets')
    parser.add_argument('-emitFormats', action='append',
                        default=[],
                        help='Specify Vulkan Formats to emit in targets')
    parser.add_argument('-feature', action='append',
                        default=[],
                        help='Specify a core API feature name or names to add to targets')
    parser.add_argument('-debug', action='store_true',
                        help='Enable debugging')
    parser.add_argument('-dump', action='store_true',
                        help='Enable dump to stderr')
    parser.add_argument('-diagfile', action='store',
                        default=None,
                        help='Write diagnostics to specified file')
    parser.add_argument('-errfile', action='store',
                        default=None,
                        help='Write errors and warnings to specified file instead of stderr')
    parser.add_argument('-noprotect', dest='protect', action='store_false',
                        help='Disable inclusion protection in output headers')
    parser.add_argument('-profile', action='store_true',
                        help='Enable profiling')
    parser.add_argument('-registry', action='store',
                        default='vk.xml',
                        help='Use specified registry file instead of vk.xml')
    parser.add_argument('-time', action='store_true',
                        help='Enable timing')
    parser.add_argument('-genpath', action='store', default='gen',
                        help='Path to generated files')
    parser.add_argument('-o', action='store', dest='directory',
                        default='.',
                        help='Create target and related files in specified directory')
    parser.add_argument('target', metavar='target', nargs='?',
                        help='Specify target')
    parser.add_argument('-quiet', action='store_true', default=True,
                        help='Suppress script output during normal execution.')
    parser.add_argument('-verbose', action='store_false', dest='quiet', default=True,
                        help='Enable script output during normal execution.')

    # This argument tells us where to load the script from the Vulkan-Headers registry
    parser.add_argument('-scripts', action='store',
                        help='Find additional scripts in this directory')

    args = parser.parse_args()

    # default scripts path to be same as registry
    if not args.scripts:
        args.scripts = os.path.dirname(args.registry)

    scripts_directory_path = os.path.dirname(os.path.abspath(__file__))
    registry_headers_path = os.path.join(scripts_directory_path, args.scripts)
    sys.path.insert(0, registry_headers_path)

    from cgenerator import CGeneratorOptions, COutputGenerator
    from reflib import logDiag, logWarn, logErr, setLogFile
    from reg import Registry

    # This splits arguments which are space-separated lists
    args.feature = [name for arg in args.feature for name in arg.split()]
    args.extension = [name for arg in args.extension for name in arg.split()]

    # create error/warning & diagnostic files
    if args.errfile:
        errWarn = open(args.errfile, 'w', encoding='utf-8')
    else:
        errWarn = sys.stderr

    if args.diagfile:
        diag = open(args.diagfile, 'w', encoding='utf-8')
    else:
        diag = None

    if args.time:
        # Log diagnostics and warnings
        setLogFile(setDiag = True, setWarn = True, filename = '-')

    # Create the API generator & generator options
    (gen, options) = genTarget(args)

    # Create the registry object with the specified generator and generator
    # options. The options are set before XML loading as they may affect it.
    reg = Registry(gen, options)

    # Parse the specified registry XML into an ElementTree object
    startTimer(args.time)
    tree = etree.parse(args.registry)
    endTimer(args.time, '* Time to make ElementTree =')

    # Load the XML tree into the registry object
    startTimer(args.time)
    reg.loadElementTree(tree)
    endTimer(args.time, '* Time to parse ElementTree =')

    if args.dump:
        logDiag('* Dumping registry to regdump.txt')
        reg.dumpReg(filehandle=open('regdump.txt', 'w', encoding='utf-8'))

    # Finally, use the output generator to create the requested target
    if args.debug:
        pdb.run('reg.apiGen()')
    else:
        startTimer(args.time)
        reg.apiGen()
        endTimer(args.time, '* Time to generate ' + options.filename + ' =')

    if not args.quiet:
        logDiag('* Generated', options.filename)
