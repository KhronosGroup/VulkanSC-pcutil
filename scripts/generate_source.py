#!/usr/bin/env python3
# Copyright (c) 2019-2025 The Khronos Group Inc.
# Copyright (c) 2019-2025 Valve Corporation
# Copyright (c) 2019-2025 LunarG, Inc.
# Copyright (c) 2019-2025 Google Inc.
# Copyright (c) 2023-2025 RasterGrid Kft.
#
# SPDX-License-Identifier: Apache-2.0
#
# Author: Mike Schuchardt <mikes@lunarg.com>

import argparse
import filecmp
import os
import json
import shutil
import subprocess
import sys
import tempfile

import common_codegen

def main(argv):
    parser = argparse.ArgumentParser(description='Generate source code for this repository')
    parser.add_argument('--clang-format',
                        default='clang-format',
                        help='Specify clang-format executable to use')
    parser.add_argument('--generated-version', help='sets the header version used to generate the repo')
    parser.add_argument('registry', metavar='REGISTRY_PATH', help='path to the Vulkan-Headers registry directory')
    args = parser.parse_args(argv)

    # output paths and the list of files in the path
    files_to_gen = {
        'json': [
            'vk.json'
        ],
        str(os.path.join('layers','json_gen')): [
            'json_gen.cpp',
            'vulkan_json_layer_data.hpp'
        ],
        str(os.path.join('include','vulkan','pcjson')): [
            'vulkan_json_data.hpp',
            'vulkan_json_parser.hpp',
            'vulkan_json_gen.h'
        ],
        str(os.path.join('library','pcjson')): [
            'vulkan_json_gen.c'
        ]
    }

    # base directory for the source repository
    repo_dir = common_codegen.repo_relative('')

    # Update the api_version in the respective json files
    if args.generated_version:
        json_files = [common_codegen.repo_relative(os.path.join('layers','json_gen','VkLayer_json_gen.json.in'))]
        for json_file in json_files:
            with open(json_file) as f:
                data = json.load(f)

            data["layer"]["api_version"] = args.generated_version

            with open(json_file, mode='w', encoding='utf-8', newline='\n') as f:
                f.write(json.dumps(data, indent=4))

    registry = os.path.abspath(os.path.join(args.registry, 'vk.xml'))
    if not os.path.isfile(registry):
        registry = os.path.abspath(os.path.join(args.registry, 'Vulkan-Headers/registry/vk.xml'))
        if not os.path.isfile(registry):
            print(f'cannot find vk.xml in {args.registry}')
            return -1
        
    clang_format = shutil.which(args.clang_format)
    if clang_format is None:
        print("WARNING: Unable to find clang-format!")

    # run each code generator
    for path, filenames in files_to_gen.items():
        for filename in filenames:
            output_path = common_codegen.repo_relative(path)

            cmd = [common_codegen.repo_relative(os.path.join('scripts','pcutil_genvk.py')),
                '-registry', registry,
                '-quiet',
                '-o', output_path, filename]
            print(' '.join(cmd))
            try:
                subprocess.check_call([sys.executable] + cmd, cwd=repo_dir)

                if clang_format:
                    subprocess.check_call([clang_format, '-i', '--style=file', os.path.join(output_path, filename)], cwd=repo_dir)

            except Exception as e:
                print('ERROR:', str(e))
                return 1

    return 0

if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))

