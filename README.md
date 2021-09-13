# Vulkan SC Pipeline Cache Utility

## Overview

The Vulkan SC Pipeline Cache Utilty provides header-only utility implementations
for reading (pcreader.hpp) and writing (pcwriter.hpp) Vulkan SC-specific pipeline
cache files.

These are intended to be included in other projects as needed in order to ensure
correct reading and writing of the pipeline cache header.

Expected use cases:

 - pcreader.hpp
   - IHV driver to facilitate parsing the pipeline cache header
   - Applications (including CTS) to retrieve pipeline entry information (identifier, poolEntrySize)
   - validation layers and tools to extract Json and shader code

 - pcwriter.hpp
   - offline pipeline cache compiler to facilitate writing the header information
     to the pipeline cache

## Usage

Include the appropriate header implementation in your project.

These are written as C++ 11 headers, which assume the relevant headers are already included.

See pipeline.cpp for examples of minimal usage and some basic self-tests.

## PC Reader

The PC Reader utility performs no dynamic memory allocations.

It makes use of the `assert` and `memcmp` standard library functions.
If a user of the library wishes to disable asserts or provide an alternative memcmp
implementation this can be done by setting the `VKSC_ASSERT` and `VKSC_MEMCMP`
macros before including pcreader.hpp.

## PC Writer

The PC Writer utility performs dynamic memory allocations using the standard `new`
and `delete` array operators.

It also makes use of the `assert` and `memcpy` standard library functions.
If a user of the library wishes to disable asserts or provide an alternative memcpy
implementation this can be done by setting the `VKSC_ASSERT` and `VKSC_MEMCPY`
macros before including pcwriter.hpp.
