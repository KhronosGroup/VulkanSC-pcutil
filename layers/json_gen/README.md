# Vulkan Pipeline JSON Generator Layer

This directory contains the Vulkan JSON Gen Layer (`VK_LAYER_KHRONOS_json_gen`) that enables collecting information about the pipelines of a Vulkan application in the form of Vulkan SC pipeline JSON files and included SPIR-V shader module binaries. These then can be fed as input to a Vulkan SC Pipeline Cache Compiler to produce Vulkan SC pipeline binaries.

Note that not all Vulkan features are supported in Vulkan SC, therefore the pipeline JSON files captured from a Vulkan application will only be valid inputs to a Vulkan SC Pipeline Cache Compiler if they only use features also available in Vulkan SC.

# Using the Vulkan JSON Gen Layer

To enable using the `VK_LAYER_KHRONOS_json_gen` layer in a Vulkan application, make sure to install it to any of the locations the Vulkan Loader uses to search for available explicit layers or set the `VK_LAYER_PATH` environment variable to point to the directory containing the JSON manifest of the layer (`VkLayer_khronos_json_gen.json`).

In addition, the layer binary must be on the system-specific library loading path.

On Linux, this can be achieved e.g. by configuring the following environment variables:

```bash
export LD_LIBRARY_PATH=<install_dir>/lib
export VK_LAYER_PATH=<install_dir>/share/vulkan/explicit_layer.d
```

On Windows, this can be achieved e.g. by configuring the following environment variables:

```
set PATH=<install_dir>\bin;%PATH%
set VK_LAYER_PATH=<install_dir>\bin
```

Furthermore, in order to run a Vulkan application with the JSON Gen Layer, similar to other layers, it can be enabled through environment variables, e.g.:

  * Windows: `set VK_LOADER_LAYERS_ENABLE=VK_LAYER_KHRONOS_json_gen`
  * Linux: `export VK_LOADER_LAYERS_ENABLE=VK_LAYER_KHRONOS_json_gen`

In addition, the `VK_JSON_FILE_PATH` environment variable needs to be set to the location of the destination directory where the pipeline JSON and SPIR-V files will be output.
