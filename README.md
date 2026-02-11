# KhronosGroup/VulkanSC-pcutil

This project provides Vulkan SC pipeline cache utilities and tools.

## Intro

The following components are available in this repository:

- [*Vulkan SC Pipeline JSON Schema*](json/)
- [*Vulkan SC Pipeline JSON Generator/Parser Library*](include/vulkan/pcjson/)
- [*Vulkan SC Pipeline Cache Reader/Wrtier Library*](include/vulkan/pcutil/)
- [*Vulkan SC Pipeline Cache Command Line Utility*](vkscpcinfo/)
- [*Vulkan SC Pipeline Cache Compiler Helper Tool*](vkscpcctool/)
- [*Vulkan Pipeline JSON Generator Layer (VK_LAYER_KHRONOS_json_gen)*](layers/json_gen/)

The repository also contains the tooling used for discovering Pipeline Cache Compiler toolchains supporting specific target architectures/devices through PCC manifest files, as described in the [PCC discovery](docs/PccDiscovery.md) documentation.

## Contact Information
* [Daniel Rakos](mailto:daniel.rakos@rastergrid.com)

## Information for Developing or Contributing:

Please see the [CONTRIBUTING.md](CONTRIBUTING.md) file in this repository for more details.

## How to Build and Run

- [BUILD.md](BUILD.md) includes directions for building all components.
- [layers/json_gen/README.md](layers/json_gen/README.md) includes information about the Vulkan Pipeline JSON Generator Layer (`VK_LAYER_KHRONOS_json_gen`) and its use.

## Version Tagging Scheme

Updates to this repository which correspond to a new Vulkan SC specification release are tagged using the following format: `vksc<`_`version`_`>.<`_`patch`_`>` (e.g., `vksc1.0.12`).

## License
This work is released as open source under a Apache-style license from Khronos including a Khronos copyright.

See LICENSE.txt for a full list of licenses used in this repository.
