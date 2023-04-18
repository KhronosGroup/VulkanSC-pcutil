# Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# Author: Daniel Koch <dkoch@nvidia.com>


import os
import subprocess
import argparse
import sys
import difflib
import itertools


def RunTest(executable):

    p = subprocess.run(executable, stdout=subprocess.PIPE)
    return p.stdout.decode('utf-8'), p.stderr

def CompareWithGolden(current_output, golden_output):
    result = True
    print("==>Comparing current output from " + current_output + " with golden output from " + golden_output)

    with open(current_output) as output_f:
        output_text = output_f.readlines()
    with open(golden_output) as golden_f:
        golden_text = golden_f.readlines()

    difflines = difflib.unified_diff(golden_text, output_text, fromfile=golden_output, tofile=current_output, lineterm='\n')
    delta = ''.join(difflines)

    if not delta:
        print("==>Generated output file matches with Golden output")
    else:
        result = False
        print("==>Generated Json file does not match with Golden Json")
        print("Find the diff below - ")
        print("----------------------")
        print(delta)

    return result

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--test_binary",
        dest="test_binary",
        help="Path to the installed pctest binary."
    )

    args = parser.parse_args()

    test_binary = os.path.abspath(args.test_binary)
    print("Running test with: " + test_binary)

    src_directory = os.path.realpath(os.path.dirname(__file__))
    print("src_directory is: " + src_directory)

    print("Run PCUtil Test")
    print("========================")
    stdout, stderr = RunTest(test_binary)

    current_directory = os.getcwd()
    print("current_directory is : " + current_directory) 
    output_file_name = os.path.join(current_directory, "output.txt")
    print("output_file_name is" + output_file_name)
    output = open(output_file_name, "w")
    output.write(stdout)
    output.close()

    print("PCUtil Test ==> DONE")
    print("=============================")

    golden_output = os.path.join(src_directory, "golden_results.txt")
    if CompareWithGolden(output_file_name, golden_output):
        print("[TEST PASSED]")
    else:
        print("[TEST FAILED]")

if __name__ == '__main__':
    main()



