/*
 * Copyright (c) 2025-2026 The Khronos Group Inc.
 * Copyright (c) 2025-2026 RasterGrid Kft.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdlib.h>

#include <cxxopts.hpp>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#include <vulkan/vulkan_sc.h>
#include <vulkan/vk_enum_string_helper.h>

#include <vector>

class API {
  public:
    API() {
#if defined(_WIN32)
        loader_module_ = LoadLibraryA("vulkansc-1.dll");
#else
        loader_module_ = dlopen("libvulkansc.so.1", RTLD_NOW | RTLD_LOCAL);
        if (!loader_module_) {
            loader_module_ = dlopen("libvulkansc.so", RTLD_NOW | RTLD_LOCAL);
        }
#endif
        if (!loader_module_) {
            fprintf(stderr, "ERROR: Failed to load the Vulkan SC Loader.\n");
            return;
        }

#if defined(_WIN32)
        gipa_ = (PFN_vkGetInstanceProcAddr)(void (*)(void))GetProcAddress((HMODULE)loader_module_, "vkGetInstanceProcAddr");
#else
        gipa_ = (PFN_vkGetInstanceProcAddr)dlsym(loader_module_, "vkGetInstanceProcAddr");
#endif
        if (gipa_ == nullptr) {
            fprintf(stderr, "ERROR: Failed to load entry point vkGetInstanceProcAddr.\n");
            Close();
            return;
        }

#define VK_GET_PROC_ADDR(name)                                               \
    name = reinterpret_cast<PFN_vk##name>(gipa_(nullptr, "vk" #name));       \
    if (name == nullptr) {                                                   \
        fprintf(stderr, "ERROR: Failed to load entry point vk" #name ".\n"); \
        Close();                                                             \
        return;                                                              \
    }
        VK_GET_PROC_ADDR(CreateInstance);
#undef VK_GET_PROC_ADDR
    }

    void LoadInstanceEntryPoints(VkInstance instance) {
#define VK_GET_PROC_ADDR(name)                                               \
    name = reinterpret_cast<PFN_vk##name>(gipa_(instance, "vk" #name));      \
    if (name == nullptr) {                                                   \
        fprintf(stderr, "ERROR: Failed to load entry point vk" #name ".\n"); \
        Close();                                                             \
        return;                                                              \
    }
        VK_GET_PROC_ADDR(DestroyInstance);
        VK_GET_PROC_ADDR(EnumeratePhysicalDevices);
        VK_GET_PROC_ADDR(GetPhysicalDeviceProperties2);
#undef VK_GET_PROC_ADDR
    }

    ~API() { Close(); }

    bool IsValid() const { return loader_module_; }

    PFN_vkCreateInstance CreateInstance{nullptr};
    PFN_vkDestroyInstance DestroyInstance{nullptr};
    PFN_vkEnumeratePhysicalDevices EnumeratePhysicalDevices{nullptr};
    PFN_vkGetPhysicalDeviceProperties2 GetPhysicalDeviceProperties2{nullptr};

  private:
    void Close() {
        if (loader_module_) {
#if defined(_Win32)
            FreeLibrary((HMODULE)loader_module_);
#else
            dlclose(loader_module_);
#endif
            loader_module_ = nullptr;
        }
    }

    void* loader_module_{nullptr};
    PFN_vkGetInstanceProcAddr gipa_{nullptr};
};

void setenv(const char* envvar, const char* value) {
#if defined(_WIN32)
    if (_putenv_s(envvar, value) != 0) {
#else
    if (setenv(envvar, value, 1) != 0) {
#endif
        fprintf(stderr, "ERROR: Failed to set environment variable %s to \"%s\"\n", envvar, value);
        exit(EXIT_FAILURE);
    }
}

void list_devices() {
    API api{};
    if (!api.IsValid()) {
        fprintf(stderr, "WARNING: No device enumeration was performed due to missing appropriate Vulkan SC Loader.\n");
        exit(EXIT_SUCCESS);
    }

    VkResult result = VK_SUCCESS;

    VkApplicationInfo app_info = {.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                                  .pNext = NULL,
                                  .pApplicationName = "vkscpcctool",
                                  .applicationVersion = 0,
                                  .pEngineName = NULL,
                                  .engineVersion = 0,
                                  .apiVersion = VKSC_API_VERSION_1_0};
    VkInstanceCreateInfo inst_info = {.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                                      .pNext = NULL,
                                      .flags = 0,
                                      .pApplicationInfo = &app_info,
                                      .enabledLayerCount = 0,
                                      .ppEnabledLayerNames = NULL,
                                      .enabledExtensionCount = 0,
                                      .ppEnabledExtensionNames = NULL};
    VkInstance instance = VK_NULL_HANDLE;
    result = api.CreateInstance(&inst_info, nullptr, &instance);
    if (result != VK_SUCCESS) {
        fprintf(stderr, "Failed to create Vulkan SC instance: %s\n", string_VkResult(result));
        exit(EXIT_FAILURE);
    }

    api.LoadInstanceEntryPoints(instance);
    if (!api.IsValid()) {
        fprintf(stderr, "WARNING: No device enumeration was performed due to missing instance entry points.\n");
        exit(EXIT_SUCCESS);
    }

    uint32_t phys_dev_count = 0;
    result = api.EnumeratePhysicalDevices(instance, &phys_dev_count, nullptr);
    if (result != VK_SUCCESS) {
        fprintf(stderr, "Failed to enumerate physical device count: %s\n", string_VkResult(result));
        api.DestroyInstance(instance, nullptr);
        exit(EXIT_FAILURE);
    }

    std::vector<VkPhysicalDevice> phys_devs{};
    phys_devs.resize(phys_dev_count);
    result = api.EnumeratePhysicalDevices(instance, &phys_dev_count, phys_devs.data());
    if (result != VK_SUCCESS) {
        fprintf(stderr, "Failed to enumerate physical device count: %s\n", string_VkResult(result));
        api.DestroyInstance(instance, nullptr);
        exit(EXIT_FAILURE);
    }

    for (auto phys_dev : phys_devs) {
        VkPhysicalDeviceProperties2 phys_props = {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2, .pNext = NULL};
        api.GetPhysicalDeviceProperties2(phys_dev, &phys_props);

        printf("0x%08x %s\n", phys_props.properties.deviceID, phys_props.properties.deviceName);
    }

    api.DestroyInstance(instance, nullptr);
}

int main(int argc, char* argv[]) {
    // clang-format off
    cxxopts::Options options(argv[0], "Pipeline Cache Compiler Helper Tool (For Internal Use)");
    options.add_options()
        ("list-devices", "List available device IDs and names matching the filtering criteria.",
            cxxopts::value<bool>()->default_value("false"))
        ("vendor-id-filter", "Vendor ID filter following the convention of VK_LOADER_VENDOR_ID_FILTER.",
            cxxopts::value<std::string>(), "<id-filter>")
        ("device-id-filter", "Device ID filter following the convention of VK_LOADER_DEVICE_ID_FILTER.",
            cxxopts::value<std::string>(), "<id-filter>")
        ("driver-id-filter", "Driver ID filter following the convention of VK_LOADER_DRIVER_ID_FILTER.",
            cxxopts::value<std::string>(), "<id-filter>")
        ("help", "Show this help.");
    // clang-format on

    cxxopts::ParseResult args{};
    try {
        args = options.parse(argc, argv);
    } catch (cxxopts::exceptions::exception& excp) {
        fprintf(stderr, "ERROR: %s\n", excp.what());
        fprintf(stdout, "%s\n", options.help().c_str());
        return EXIT_FAILURE;
    }

    if (args.count("help")) {
        fprintf(stdout, "%s\n", options.help().c_str());
        return EXIT_SUCCESS;
    } else if (args.count("list-devices")) {
        // List devices
        if (args.count("vendor-id-filter")) {
            setenv("VK_LOADER_VENDOR_ID_FILTER", args["vendor-id-filter"].as<std::string>().c_str());
        }
        if (args.count("device-id-filter")) {
            setenv("VK_LOADER_DEVICE_ID_FILTER", args["device-id-filter"].as<std::string>().c_str());
        }
        if (args.count("driver-id-filter")) {
            setenv("VK_LOADER_DRIVER_ID_FILTER", args["driver-id-filter"].as<std::string>().c_str());
        }
        list_devices();
        return EXIT_SUCCESS;
    } else {
        fprintf(stderr, "ERROR: No operation was requested.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
