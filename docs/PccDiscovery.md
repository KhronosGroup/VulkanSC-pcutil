# Pipeline Cache Compiler discovery

Vulkan SC requires offline compiled pipelines, vendors therefore provide such offline Pipeline Cache Compilers (PCCs) alongside their Vulkan SC driver implementation that can be used to build pipeline cache binaries from a set of pipeline JSON files and SPIR-V modules.

Aside from a few standard command line options enforced by the [Vulkan SC Conformance Test Suite](https://github.com/KhronosGroup/VK-GL-CTS), the command line interface of these vendor-specific PCCs is not uniform across implementations but generally provide the ability to build pipeline cache binaries for specific hardware architectures or specific devices. In addition, they usually also provide the ability to embed the source pipeline JSON files and SPIR-V module data into the produced pipeline cache binaries that helps debugging applications with the [Vulkan SC Validation Layers](https://github.com/KhronosGroup/VulkanSC-ValidationLayers) by enabling validation dependent on the availability of SPIR-V module information (without such debug information in the pipeline cache binaries, the validation layers are not able to perform any such cross-validation of API state and shader code).

The Pipeline Cache Compiler JSON manifests and the discovery mechanism presented in this document enables application developers to write applications (and particularly CMake projects) that are agnostic to the underlying vendor-specific PCC toolchain.

## PCC manifest format

The PCC manifest file is a JSON file similar to the [ICD](https://github.com/KhronosGroup/VulkanSC-Loader/blob/main/docs/LoaderDriverInterface.md#driver-manifest-file-format) and [layer](https://github.com/KhronosGroup/Vulkan-Loader/blob/main/docs/LoaderLayerInterface.md#layer-manifest-file-format) manifest files.

Here is an example PCC manifest file:

```json
{
    "file_format_version": "1.0.0",
    "name": "<vendor-name>",
    "architectures": {
        "<architecture-name-1>": {
            "pipeline_cache_compiler": "<path-to-pcc-executable>",
            "pipeline_cache_flags": "<architecture-specific-command-line-flags>",
            "pipeline_cache_debug_flag": "<command-line-flag-enabling-debug-info>",
            "vendor_id_filter" : "<optional-vendor-id-filter>",
            "driver_id_filter" : "<optional-driver-id-filter>",
            "device_id_filter" : "<optional-device-id-filter>"
        },
        "<architecture-name-2>": {
            ...
        },
        ...
    },
    "devices": [
        {
            "pipeline_cache_compiler": "<path-to-pcc-executable>",
            "pipeline_cache_device_flags": "<command-line-flag-taking-a-device-id-argument>",
            "pipeline_cache_debug_flag": "<command-line-flag-enabling-debug-info>",
            "vendor_id_filter" : "<optional-vendor-id-filter>",
            "driver_id_filter" : "<optional-driver-id-filter>",
            "device_id_filter" : "<optional-device-id-filter>"
        }
    ]
}
```

The top-level `"name"` JSON property specifies the human-readable name of the PCC toolchain (e.g. "Emulation" for the [Vulkan SC Emulation](https://github.com/KhronosGroup/VulkanSC-Emulation/) or "NVIDIA" for the NVIDIA Vulkan SC implementation).

The `"architectures"` and `"devices"` JSON objects are optional blocks enabling the declaration of a set of target architectures and device configurations.

### Architecture declarations

The target architectures declared in the `"architectures"` block enable declaring specific target device architectures that the PCC toolchain can build pipeline cache binaries for. This block is an JSON object where each property's name specifies the human-readable name of the target architecture. This name is concatenated with the toolchain name in the form "`<toolchain>` - `<architecture>`" to produce the human-readable name of a specific target architecture the toolchain can build pipeline cache binaries for.

The JSON properties supported within each such architecture JSON object are described in the table below:

| Property Name | Type | Description |
| ------------- | ---- | ----------- |
| "pipeline_cache_compiler" | Required | Specifies the name and path of the PCC executable that is invoked when targeting the architecture (see [PCC executable path format](#pcc-executable-path-format) for more details) |
| "pipeline_cache_flags" | Optional | Specifies the command line arguments to pass to the PCC executable when targeting the architecture (e.g. `--chip gv11b`) |
| "pipeline_cache_debug_flag" | Optional | Specifies the command line argument to pass to the PCC executable when inclusion of pipeline JSON and SPIR-V debug information is requested |
| "vendor_id_filter" | Optional | Specifies the Vulkan SC vendor ID filter to restrict the execution of applications built for the target architecture |
| "driver_id_filter" | Optional | Specifies the Vulkan SC driver ID filter to restrict the execution of applications built for the target architecture |
| "device_id_filter" | Optional | Specifies the Vulkan SC device ID filter to restrict the execution of applications built for the target architecture |

While many of the properties are optional, it is generally recommended for toolchains to include the following:

  * The `"pipeline_cache_debug_flags"` property in order to enable building pipeline cache binaries with debug information to enable full API validation support
  * At least the `"vendor_id_filter"` and `"driver_id_filter"` property in order to avoid the built applications from accidentally running against the incorrect Vulkan SC driver or device on a development machine ( (see [Vendor/driver/device filter format](#vendordriverdevice-filter-format) for more details))

### Device declarations

The device rules declared in the `"devices"` block enable declaring rules to apply when a specific device (contrarily to an architecture) is targeted with the PCC toolchain. This block is a JSON array where each element declares a specific rule. These are used to enable targeting specific device present on a development machine.

The full human-readable name of a specific target device the toolchain can build pipeline cache binaries for will take the form "`<toolchain>` - `<device>`", where `<device>` is the name of a Vulkan SC device detected on the development machine that matches a corresponding rule in the `"devices"` block.

The JSON properties supported within each such architecture JSON object are described in the table below:

| Property Name | Type | Description |
| ------------- | ---- | ----------- |
| "pipeline_cache_compiler" | Required | Specifies the name and path of the PCC executable that is invoked when targeting the architecture (see [PCC executable path format](#pcc-executable-path-format) for more details) |
| "pipeline_cache_device_flag" | Required | Specifies the command line option name that accepts a device ID as argument (e.g. `--device` which will trigger PCC commands such as `<pcc-executable> --device <device-id>`) |
| "pipeline_cache_debug_flag" | Optional | Specifies the command line argument to pass to the PCC executable when inclusion of pipeline JSON and SPIR-V debug information is requested |
| "vendor_id_filter" | Optional | Specifies the Vulkan SC vendor ID filter to restrict the execution of applications built for the target architecture |
| "driver_id_filter" | Optional | Specifies the Vulkan SC driver ID filter to restrict the execution of applications built for the target architecture |
| "device_id_filter" | Optional | Specifies the Vulkan SC device ID filter to restrict the execution of applications built for the target architecture |

While many of the properties are optional, it is generally recommended for toolchains to include the following:

  * The `"pipeline_cache_debug_flags"` property in order to enable building pipeline cache binaries with debug information to enable full API validation support
  * At least the `"vendor_id_filter"` and `"driver_id_filter"` property in order to avoid the built applications from accidentally running against the incorrect Vulkan SC driver or device on a development machine ( (see [Vendor/driver/device filter format](#vendordriverdevice-filter-format) for more details))

### PCC executable path format

The PCC executable path specified in the `"pipeline_cache_compiler"` property can take one of the following forms:

  * An absolute path (e.g. `C:\MyDriver\MyPCC.exe` or `/opt/mydriver/mypcc`)
  * A relative path (e.g. `.\MyPCC.exe` or `./mypcc`)
  * An unqualified file name (e.g. `MyPCC.exe` or `mypcc`)

An absolute path is generally recommended to be used when the exact installation path of the toolchain is known.

A relative path is generally recommended when the PCC manifest file and the PCC executable are in the same folder but one that may not be decided by the toolchain installation (e.g. `.\MyPCC.exe` is the best choice if the PCC is expected to be installed on Windows in the driver store location alongside the driver and the manifest files themselves).

An unqualified file name is generally recommended when the PCC executable is expected to be on the executable search path (e.g. in one of the folders included in the `PATH` environment variable).

### Vendor/driver/device filter format

The vendor, driver, and device filters specified in the PCC manifest can take one of the following forms:

  * A single decimal (e.g. `42`) or hexadecimal (e.g. `0x10de`) value
  * A range of decimal (e.g. `24-42`) or hexadecimal (e.g. `0x1eb1-0x1eb3`) values
  * A comma separated list of any of the above (e.g. `0x10001-0x1FFFF,0x1eb1`)

These values are used on development machines by the toolchain to instruct the [Vulkan SC Loader](https://github.com/KhronosGroup/VulkanSC-Loader) to filter out any Vulkan SC devices that do not match the specified vendor/driver/device filters, therefore avoiding the accidental execution of the Vulkan SC application using the built pipeline cache binaries against an incompatible Vulkan SC device, as pipeline cache binaries are typically vendor-, driver-, and/or device-specific.

Note that these values do not have any effect on applications targeting actual native Vulkan SC platforms, as they depend on the [Vulkan SC Loader](https://github.com/KhronosGroup/VulkanSC-Loader) that is only supported on development platforms. Furthermore, the filters only affect applications that are built with the PCC integration tooling that automatically embeds a stub in the executables built for development environments to apply the filters.

## PCC discovery

The PCC manifests are discovered by the [VulkanSCPccDiscovery](https://github.com/KhronosGroup/VulkanSC-pcutil/blob/main/cmake/Modules/VulkanSCPccDiscovery.cmake) CMake module that is invoked when e.g. the `VulkanSC` CMake package is added to a CMake project.

The discovery mechanism will try to discover PCC manifest files in a set of locations specific to the target platform.

The discovery mechanism will first look for PCC manifest files on the path `%VULKANSC_SDK%\share\vulkansc\pcc.d\` (Windows) / `%VULKANSC_SDK/share/vulkansc/icd.d/` (Linux), i.e. look for any PCC manifests installed by the active [Vulkan SC SDK](https://github.com/KhronosGroup/VulkanSC-SDK).

Then, on Windows, the process is followed by looking for registry keys containing PCC manifest file paths in the following registry locations and order:

  * `HKEY_CURRENT_USER\SOFTWARE\Khronos\VulkanSC\PCC\`
  * `HKEY_LOCAL_MACHINE\SOFTWARE\Khronos\VulkanSC\PCC\`

While on Linux, the process is followed by looking for PCC manifest files in the following file system location  and order:

  * `$HOME/.config/vulkansc/pcc.d/`
  * `/etc/xdg/vulkansc/pcc.d/`
  * `/usr/local/etc/vulkansc/pcc.d/`
  * `/etc/vulkansc/pcc.d/`
  * `$HOME/.local/share/vulkansc/pcc.d/`
  * `/usr/local/share/vulkansc/pcc.d/`
  * `/usr/share/vulkansc/pcc.d`

## PCC integration

The [VulkanSCPccUtilities](https://github.com/KhronosGroup/VulkanSC-pcutil/blob/main/cmake/Modules/VulkanSCPccUtilities.cmake) CMake module provides the following CMake function to be able to add Vulkan SC pipeline cache binary targets to a CMake project:

```
add_vksc_pipeline_cache(<target-name>
    [DEBUG]
    PATH <pipeline-json-dir>
    [FLAGS <flags>]
    OUT <out-binary>
)
```

Where:

  * `<target-name>` is the name of the pipeline cache binary target
  * `DEBUG` is an optional flag that can be specified to request the PCC to include debug information in the built pipeline cache binary
  * `<pipeline-json-dir>` is the path of the directory containing the source pipeline JSON files to build the pipeline cache binary with
  * `FLAGS <flags>` is an optional argument that allows specifying additional flags to pass to the invoked PCC
  * `<out-file>` is the path of the pipeline cache binary file to write to

This enables writing portable Vulkan SC CMake projects that are agnostic to the underlying Vulkan SC PCC toolchain.

The most common form that applications are expected to invoke the `add_vksc_pipeline_cache` is as follows:

```
add_vksc_pipeline_cache(<target-name>
    $<$<CONFIG:Debug,RelWithDebInfo>:DEBUG>
    PATH <pipeline-json-dir>
    OUT <out-binary>
)
```

Where the generator expression will automatically add the `DEBUG` option when building debug builds of the project.

The integration tooling will also include a stub in the produced application binaries linked with the `VulkanSC::VulkanSC` target that will automatically inform the [Vulkan SC Loader](https://github.com/KhronosGroup/VulkanSC-Loader) about the vendor/driver/device filters to apply when running the built applications to avoid accidentally running the application on a development machine against the wrong driver or device.
