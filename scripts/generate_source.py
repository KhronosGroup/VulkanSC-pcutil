#!/usr/bin/env python3
# Copyright 2023-2025 The Khronos Group Inc.
# Copyright 2023 Valve Corporation
# Copyright 2023 LunarG, Inc.
# Copyright 2023-2025 RasterGrid Kft.
#
# SPDX-License-Identifier: Apache-2.0

import argparse
import os
import sys
import shutil
import subprocess
from xml.etree import ElementTree

# Runs a command in a directory and returns its return code.
# Directory is project root by default, or a relative path from project root
def RunShellCmd(command):
    # Flush stdout here. Helps when debugging on CI.
    sys.stdout.flush()
    cmd_list = command.split(" ")
    print(cmd_list)
    subprocess.check_call(cmd_list)


def RunGenerators(api: str, registry: str, styleFile: str, targetFilter: str) -> None:

    has_clang_format = shutil.which('clang-format') is not None
    if not has_clang_format:
        print("WARNING: Unable to find clang-format!")

    # These live in the Vulkan-Docs repo, but are pulled in via the
    # Vulkan-Headers/registry folder
    # At runtime we inject python path to find these helper scripts
    scripts = os.path.dirname(registry)
    scripts_directory_path = os.path.dirname(os.path.abspath(__file__))
    registry_headers_path = os.path.join(scripts_directory_path, scripts)
    sys.path.insert(0, registry_headers_path)
    try:
        from reg import Registry
    except:
        print("ModuleNotFoundError: No module named 'reg'") # normal python error message
        print(f'{registry_headers_path} is not pointing to the Vulkan-Headers registry directory.')
        print("Inside Vulkan-Headers there is a registry/reg.py file that is used.")
        sys.exit(1) # Return without call stack so easy to spot error

    from base_generator import BaseGeneratorOptions
    from generators.json_schema_generator import JsonSchemaGenerator
    from generators.json_test_generator import JsonTestGenerator
    from generators.json_gen_generator import JsonGenGenerator
    from generators.json_parse_generator import JsonParseGenerator

    # These set fields that are needed by both OutputGenerator and BaseGenerator,
    # but are uniform and don't need to be set at a per-generated file level
    from base_generator import (SetTargetApiName, SetMergedApiNames)
    SetTargetApiName(api)

    # Build up a list of all generators and custom options
    generators = {
        'vksc_pipeline_schema.json' : {
           'generator' : JsonSchemaGenerator,
           'genCombined': False,
           'directory' : f'json',
        },
        'json_test.cpp' : {
            'generator' : JsonTestGenerator,
            'genCombined': False,
            'directory' : f'tests/json/generated'
        },
        'vksc_pipeline_json_gen.hpp' : {
           'generator' : JsonGenGenerator,
           'genCombined': False,
           'directory' : f'library/pcjson/generated',
        },
        ##'vksc_pipeline_json_parse.hpp' : {
        ##   'generator' : JsonParseGenerator,
        ##   'genCombined': False,
        ##   'directory' : f'library/pcjson/generated',
        ##},
    }

    unknownTargets = [x for x in (targetFilter if targetFilter else []) if x not in generators.keys()]
    if unknownTargets:
        print(f'ERROR: No generator options for unknown target(s): {", ".join(unknownTargets)}', file=sys.stderr)
        return 1

    # Filter if --target is passed in
    targets = [x for x in generators.keys() if not targetFilter or x in targetFilter]

    for index, target in enumerate(targets, start=1):
        print(f'[{index}|{len(targets)}] Generating {target}')

        # First grab a class contructor object and create an instance
        generator = generators[target]['generator']
        gen = generator()

        # This code and the 'genCombined' generator metadata is used by downstream
        # users to generate code with all Vulkan APIs merged into the target API variant
        # (e.g. Vulkan SC) when needed. The constructed apiList is also used to filter
        # out non-applicable extensions later below.
        apiList = [api]
        if api != 'vulkan' and generators[target]['genCombined']:
            SetMergedApiNames('vulkan')
            apiList.append('vulkan')
        else:
            SetMergedApiNames(None)

        outDirectory = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', generators[target]['directory']))
        options = BaseGeneratorOptions(
            customFileName  = target,
            customDirectory = outDirectory)

        # Create the registry object with the specified generator and generator
        # options. The options are set before XML loading as they may affect it.
        reg = Registry(gen, options)

        # Parse the specified registry XML into an ElementTree object
        tree = ElementTree.parse(registry)

        # Load the XML tree into the registry object
        reg.loadElementTree(tree)

        # Finally, use the output generator to create the requested target
        reg.apiGen()

        # Run clang-format on the file
        if has_clang_format:
            RunShellCmd(f'clang-format -i --style=file:{styleFile} {os.path.join(outDirectory, target)}')

def main(argv):
    parser = argparse.ArgumentParser(description='Generate source code for this repository')
    parser.add_argument('--api',
                        default='vulkansc',
                        choices=['vulkansc'],
                        help='Specify API name to generate')
    parser.add_argument('registry', metavar='REGISTRY_PATH', help='path to the Vulkan-Headers registry directory')
    group = parser.add_mutually_exclusive_group()
    group.add_argument('--target', nargs='+', help='only generate file name passed in')
    args = parser.parse_args(argv)

    registry = os.path.abspath(os.path.join(args.registry,  'vk.xml'))
    if not os.path.isfile(registry):
        registry = os.path.abspath(os.path.join(args.registry, 'Vulkan-Headers/registry/vk.xml'))
        if not os.path.isfile(registry):
            print(f'cannot find vk.xml in {args.registry}')
            return -1

    style_file = os.path.join(os.path.dirname(__file__), '../.clang-format')
    RunGenerators(args.api, registry, style_file, args.target)

    return 0

if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))

