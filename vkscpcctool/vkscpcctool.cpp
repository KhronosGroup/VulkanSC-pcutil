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
#include <cfgmgr32.h>
#else
#include <dlfcn.h>
#endif

#include <vulkan/vulkan_sc.h>
#include <vulkan/vk_enum_string_helper.h>

#include <vector>

#ifdef _WIN32
bool get_device_registry_entry(DEVINST dev_id, LPCSTR value_name, std::vector<std::string> &results) {
    HKEY hkrKey = 0;
    DWORD requiredSize, data_type;

    CONFIGRET status = CM_Open_DevNode_Key(dev_id, KEY_QUERY_VALUE, 0, RegDisposition_OpenExisting, &hkrKey, CM_REGISTRY_SOFTWARE);
    if (status != CR_SUCCESS) {
        fprintf(stderr, "vkscpcctool: Failed to open registry key for DeviceID (%ld)\n", dev_id);
        return false;
    }

    // query value
    LSTATUS ret = RegQueryValueEx(hkrKey, value_name, NULL, NULL, NULL, &requiredSize);

    if (ret != ERROR_SUCCESS) {
        if (ret == ERROR_FILE_NOT_FOUND) {
            fprintf(stderr, "vkscpcctool: Device ID (%ld) Does not contain a value for \"%s\"\n", dev_id, value_name);
        } else {
            fprintf(stderr, "vkscpcctool: Device ID (%ld) Failed to obtain %s size\n", dev_id, value_name);
        }
        RegCloseKey(hkrKey);
        return false;
    }

    // Registry string values are not guaranteed to be stored with a terminating null (see RegQueryValueEx). Reserve two
    // extra bytes so the value can be terminated after it is read, before it is walked with strlen below.
    std::vector<BYTE> buffer;
    buffer.resize(requiredSize + 2);

    ret = RegQueryValueEx(hkrKey, value_name, NULL, &data_type, (BYTE *)buffer.data(), &requiredSize);

    if (ret != ERROR_SUCCESS) {
        fprintf(stderr, "vkscpcctool: DeviceID(%ld) Failed to obtain %s\n", dev_id, value_name);
        RegCloseKey(hkrKey);
        return false;
    }

    if (data_type != REG_SZ && data_type != REG_MULTI_SZ) {
        fprintf(stderr, "vkscpcctool: Invalid %s data type. Expected REG_SZ or REG_MULTI_SZ.\n", value_name);
        RegCloseKey(hkrKey);
        return false;
    }

    // requiredSize now holds the number of bytes RegQueryValueEx wrote, which is at most the size queried for the
    // allocation, so the two terminators always fit. This keeps the strlen walk in windows_add_json_entry in bounds.
    buffer[requiredSize] = '\0';
    buffer[requiredSize + 1] = '\0';

    // In case of REG_MULTI_SZ value the buffer contains multiple null terminated strings
    auto it = buffer.begin();
    while (it != buffer.end()) {
        auto null_term_it = std::find(it, buffer.end(), '\0');
        std::string value(it, null_term_it);

        if (value.size() != 0) {
            results.push_back(value);
        }

        it = null_term_it;
        if (it != buffer.end()) {
            it++;
        }
    }

    return true;
}

bool get_device_registry_entries(LPCSTR value_name, std::vector<std::string> &results) {
    const wchar_t *softwareComponentGUID = L"{5c4c3332-344d-483c-8739-259e934c9cc8}";
    const wchar_t *displayGUID = L"{4d36e968-e325-11ce-bfc1-08002be10318}";

#if defined(CM_GETIDLIST_FILTER_PRESENT)
    const ULONG flags = CM_GETIDLIST_FILTER_CLASS | CM_GETIDLIST_FILTER_PRESENT;
#else
    const ULONG flags = 0x300;
#endif

    results.clear();

    DEVINST devID = 0, childID = 0;

    std::vector<wchar_t> deviceNames;
    wchar_t *pDeviceNames = NULL;

    ULONG deviceNamesSize = 0;

    // if after obtaining the DeviceNameSize, new device is added start over
    do {
        CM_Get_Device_ID_List_SizeW(&deviceNamesSize, displayGUID, flags);

        deviceNames.resize(deviceNamesSize);
        pDeviceNames = deviceNames.data();

    } while (CM_Get_Device_ID_ListW(displayGUID, pDeviceNames, deviceNamesSize, flags) == CR_BUFFER_SMALL);

    if (pDeviceNames) {
        for (wchar_t *deviceName = pDeviceNames; *deviceName; deviceName += wcslen(deviceName) + 1) {
            CONFIGRET status = CM_Locate_DevNodeW(&devID, deviceName, CM_LOCATE_DEVNODE_NORMAL);

            if (CR_SUCCESS != status) {
                fprintf(stderr, "vkscpcctool: failed to open DevNode %ls\n", deviceName);
                continue;
            }

            ULONG ulStatus, ulProblem;
            status = CM_Get_DevNode_Status(&ulStatus, &ulProblem, devID, 0);

            if (CR_SUCCESS != status) {
                fprintf(stderr, "vkscpcctool: failed to probe device status %ls\n", deviceName);
                continue;
            }
            if ((ulStatus & DN_HAS_PROBLEM) && (ulProblem == CM_PROB_NEED_RESTART || ulProblem == DN_NEED_RESTART)) {
                fprintf(stderr, "vkscpcctool: device %ls is pending reboot, skipping ...\n", deviceName);
                continue;
            }

            get_device_registry_entry(devID, value_name, results);

            status = CM_Get_Child(&childID, devID, 0);
            if (status != CR_SUCCESS) {
                fprintf(stderr, "vkscpcctool: unable to open child-device error:%ld\n", status);
                continue;
            }

            wchar_t childGuid[MAX_GUID_STRING_LEN + 2];  // +2 for brackets {}
            for (uint32_t i = 0; i < MAX_GUID_STRING_LEN + 2; i++) {
                childGuid[i] = L'\0';
            }
            ULONG childGuidSize = sizeof(childGuid);

            do {
                wchar_t buffer[MAX_DEVICE_ID_LEN];
                CM_Get_Device_IDW(childID, buffer, MAX_DEVICE_ID_LEN, 0);

                status = CM_Get_DevNode_Registry_PropertyW(childID, CM_DRP_CLASSGUID, NULL, &childGuid, &childGuidSize, 0);
                if (status != CR_SUCCESS) {
                    fprintf(stderr, "vkscpcctool: unable to obtain GUID for:%ld error:%ld\n", childID, status);
                    continue;
                }

                if (wcscmp(childGuid, softwareComponentGUID) != 0) {
                    fprintf(stderr, "vkscpcctool: GUID for %ld is not SoftwareComponent skipping\n", childID);
                    continue;
                }

                get_device_registry_entry(childID, value_name, results);
            } while (CM_Get_Sibling(&childID, childID, 0) == CR_SUCCESS);
        }
    }

    return true;
}

bool pcc_discovery() {
    std::vector<std::string> results;

    get_device_registry_entries("VulkanSCPccManifest", results);

    const auto remove_duplicates = [](const std::vector<std::string> &values) {
        std::vector<std::string> results;
        std::unordered_set<std::string> seen;

        results.reserve(values.size());

        for (const auto &value : values) {
            if (seen.count(value) == 0) {
                seen.insert(value);
                results.push_back(value);
            }
        }

        return results;
    };

    results = remove_duplicates(results);

    for (const auto &result : results) {
        printf("%s\n", result.c_str());
    }

    return true;
}
#endif  // _WIN32

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
#if defined(_WIN32)
            FreeLibrary((HMODULE)loader_module_);
#else
            dlclose(loader_module_);
#endif
            loader_module_ = nullptr;
        }
    }

    void *loader_module_{nullptr};
    PFN_vkGetInstanceProcAddr gipa_{nullptr};
};

void setenv(const char *envvar, const char *value) {
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

    VkApplicationInfo app_info = {/*.sType = */ VK_STRUCTURE_TYPE_APPLICATION_INFO,
                                  /*.pNext = */ NULL,
                                  /*.pApplicationName = */ "vkscpcctool",
                                  /*.applicationVersion = */ 0,
                                  /*.pEngineName = */ NULL,
                                  /*.engineVersion = */ 0,
                                  /*.apiVersion = */ VKSC_API_VERSION_1_0};
    VkInstanceCreateInfo inst_info = {/*.sType = */ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                                      /*.pNext = */ NULL,
                                      /*.flags = */ 0,
                                      /*.pApplicationInfo = */ &app_info,
                                      /*.enabledLayerCount = */ 0,
                                      /*.ppEnabledLayerNames = */ NULL,
                                      /*.enabledExtensionCount = */ 0,
                                      /*.ppEnabledExtensionNames = */ NULL};
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
        VkPhysicalDeviceProperties2 phys_props = {/*.sType = */ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
                                                  /*.pNext = */ NULL};
        api.GetPhysicalDeviceProperties2(phys_dev, &phys_props);

        printf("0x%08x %s\n", phys_props.properties.deviceID, phys_props.properties.deviceName);
    }

    api.DestroyInstance(instance, nullptr);
}

int main(int argc, char *argv[]) {
    // clang-format off
    cxxopts::Options options(argv[0], "Pipeline Cache Compiler Helper Tool (For Internal Use)");
    options.add_options()
        ("list-devices", "List available device IDs and names matching the filtering criteria.",
            cxxopts::value<bool>()->default_value("false"))
#ifdef _WIN32
        ("windows-driver-store-pcc-discovery", "List available PCC manifests JSONs discovered from Windows Driver Store locations.",
            cxxopts::value<bool>()->default_value("false"))
#endif
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
    } catch (cxxopts::exceptions::exception &excp) {
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
#ifdef _WIN32
    } else if (args.count("windows-driver-store-pcc-discovery")) {
        bool success = pcc_discovery();
        return success ? EXIT_SUCCESS : EXIT_FAILURE;
#endif
    } else {
        fprintf(stderr, "ERROR: No operation was requested.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
