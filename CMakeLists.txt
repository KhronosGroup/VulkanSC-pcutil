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

cmake_minimum_required(VERSION 3.10.2)

project(VULKANSC_PC_UTILS)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_C_COMPILER_ID MATCHES "Clang")
    set(COMMON_COMPILE_FLAGS "-Wall -Wextra -Werror -Wconversion")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_COMPILE_FLAGS}")
endif()

add_executable(pcinfo pcinfo.cpp)

target_include_directories(pcinfo PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/fake/)

add_executable(pctest pipeline.cpp)

target_include_directories(pctest PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/fake/)

