/*
 * Copyright (c) 2025 The Khronos Group Inc.
 * Copyright (c) 2025 RasterGrid Kft.
 * Copyright (c) 2021, NVIDIA CORPORATION
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <cstddef>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>
#include <vector>
#include <algorithm>
#include <limits>
#include <cctype>
#include <string>

#include <vulkan/pcutil/pcreader.hpp>
#include <vulkan/vulkan_sc.h>

using namespace std;

typedef enum DetailMode {
    DETAIL_MODE_NONE = 0,
    DETAIL_MODE_BASIC,
    DETAIL_MODE_FIRST_HEADER,
    DETAIL_MODE_ALL_HEADERS,
    DETAIL_MODE_ALL,
} DetailMode;

std::ostream &operator<<(std::ostream &out, uint8_t i) {
    out << static_cast<unsigned int>(i);
    return out;
}

std::ostream &operator<<(std::ostream &out, uint8_t const uuid[VK_UUID_SIZE]) {
    out << "0x" << std::uppercase << std::hex;
    for (uint32_t i = 0; i < VK_UUID_SIZE; ++i) {
        out << std::setfill('0') << std::setw(2) << uuid[i];
    }
    out << std::dec;
    return out;
}

std::ostream &operator<<(std::ostream &out, VkPipelineCacheHeaderVersionOne const &s) {
    out << "{" << std::endl;
    out << s.headerSize << std::endl;
    out << s.headerVersion << std::endl;
    out << std::hex;
    out << "0x" << s.vendorID << std::endl;
    out << "0x" << s.deviceID << std::endl;
    out << std::dec;
    out << s.pipelineCacheUUID << std::endl;
    out << "}";
    return out;
}

std::ostream &operator<<(std::ostream &out, VkPipelineCacheHeaderVersionSafetyCriticalOne const &s) {
    out << "{" << std::endl;
    out << s.headerVersionOne << std::endl;
    out << s.validationVersion << std::endl;
    out << s.implementationData << std::endl;
    out << s.pipelineIndexCount << std::endl;
    out << s.pipelineIndexStride << std::endl;
    out << s.pipelineIndexOffset << std::endl;
    out << "}";
    return out;
}

std::ostream &operator<<(std::ostream &out, VkPipelineCacheSafetyCriticalIndexEntry const &s) {
    out << "{" << std::endl;
    out << "identifier " << s.pipelineIdentifier << std::endl;
    out << "pipelineMemorySize " << s.pipelineMemorySize << std::endl;
    out << "jsonSize " << s.jsonSize << std::endl;
    out << "jsonOffset " << s.jsonOffset << std::endl;
    out << "stageIndexCount " << s.stageIndexCount << std::endl;
    out << "stageIndexStride " << s.stageIndexStride << std::endl;
    out << "stageIndexOffset " << s.stageIndexOffset << std::endl;
    out << "}";
    return out;
}

std::ostream &operator<<(std::ostream &out, VkPipelineCacheStageValidationIndexEntry const &s) {
    out << "{" << std::endl;
    out << s.codeSize << std::endl;
    out << s.codeOffset << std::endl;
    out << "}";
    return out;
}

struct PipelinePool {
    PipelinePool(uint64_t size) : poolSize(size){};

    uint64_t poolSize;
    uint64_t poolMin{std::numeric_limits<uint64_t>::max()};
    uint64_t poolMax{};
    uint64_t poolTotal{};
    std::vector<uint32_t> poolEntries{};
};

std::ostream &operator<<(std::ostream &out, PipelinePool const &pool) {
    out << "pool size: " << pool.poolSize << ", pool entries: " << pool.poolEntries.size() << std::endl;
    return out;
}

// <pools> must be sorted in increasing order by poolSize
bool bucketPipelines(VKSCPipelineCacheHeaderReader &pcr, std::vector<PipelinePool> &pools) {
    bool allAdded = true;

    // iterate through the pipelines and add to the smallest pool that they fit
    for (uint32_t i = 0; i < pcr.getPipelineIndexCount(); i++) {
        VkPipelineCacheSafetyCriticalIndexEntry const *pie = pcr.getPipelineIndexEntry(i);
        assert(pie != nullptr);
        bool addedToPool = false;

        for (PipelinePool &pool : pools) {
            if (pie->pipelineMemorySize <= pool.poolSize) {
                // cout << "adding index " << i << " to pool " << pool.poolSize << endl;
                pool.poolEntries.push_back(i);
                pool.poolMin = std::min(pool.poolMin, pie->pipelineMemorySize);
                pool.poolMax = std::max(pool.poolMax, pie->pipelineMemorySize);
                pool.poolTotal += pie->pipelineMemorySize;
                addedToPool = true;
                break;
            }
        }
        if (!addedToPool) {
            // pipeline didn't fit in any pool
            cerr << "WARNING: index " << i << " id: " << pie->pipelineIdentifier
                 << " pipelineMemorySize: " << pie->pipelineMemorySize << " did not fit in any pool!" << std::endl;
            allAdded = false;
        }
    }

    return allAdded;
}

bool printCacheInfo(VKSCPipelineCacheHeaderReader &pcr, DetailMode details) {
    VkPipelineCacheHeaderVersionOne const *hv1 = pcr.getHeaderVersionOne();

    uint64_t minPoolSize = std::numeric_limits<uint64_t>::max();
    uint64_t maxPoolSize = 0;

    if (details >= DETAIL_MODE_FIRST_HEADER) {
        std::cout << std::endl
                  << "headerSize:          " << hv1->headerSize << std::endl
                  << "headerVersion:       " << hv1->headerVersion << std::endl
                  << std::setbase(16) << "vendorID:            0x" << hv1->vendorID << std::endl
                  << "deviceID:            0x" << hv1->deviceID << std::endl
                  << "pipelineCacheUUID:   " << hv1->pipelineCacheUUID << std::endl
                  << std::setbase(10) << "validationVersion:   " << pcr.getValidationVersion() << std::endl
                  << "implementationData:  " << pcr.getImplementationData() << std::endl
                  << "pipelineIndexCount:  " << pcr.getPipelineIndexCount() << std::endl
                  << "pipelineIndexStride: " << pcr.getPipelineIndexStride() << std::endl
                  << "pipelineIndexOffset: " << pcr.getPipelineIndexOffset() << std::endl
                  << std::endl;
    }

    // iterate over each pipeline and print the UUID
    for (uint32_t i = 0; i < pcr.getPipelineIndexCount(); i++) {
        VkPipelineCacheSafetyCriticalIndexEntry const *pie = pcr.getPipelineIndexEntry(i);
        if (nullptr != pie) {
            if (details >= DETAIL_MODE_ALL_HEADERS) {
                std::cout << "pipeline " << i << ":" << std::endl
                          << "  pipelineIdentifier: " << pie->pipelineIdentifier << std::endl
                          << "  pipelineMemorySize: " << pie->pipelineMemorySize << std::endl
                          << "  jsonSize:           " << pie->jsonSize << std::endl
                          << "  jsonOffset:         " << pie->jsonOffset << std::endl
                          << "  stageIndexCount:    " << pie->stageIndexCount << std::endl
                          << "  stageIndexStride:   " << pie->stageIndexStride << std::endl
                          << "  stageIndexOffset:   " << pie->stageIndexOffset << std::endl;
                for (uint32_t j = 0; j < pie->stageIndexCount; j++) {
                    VkPipelineCacheStageValidationIndexEntry const *vie = pcr.getStageIndexEntry(*pie, j);
                    if (nullptr != vie) {
                        std::cout << "  stage " << j << ":" << std::endl
                                  << "    codeSize:         " << vie->codeSize << std::endl
                                  << "    codeOffset:       " << vie->codeOffset << std::endl;
                        if (details >= DETAIL_MODE_ALL) {
                            uint32_t const *spirv = reinterpret_cast<uint32_t const *>(pcr.getSPIRV(*vie));
                            std::cout << "    spirv:            ";
                            for (uint32_t k = 0; k < vie->codeSize / 4; k++) {
                                std::cout << "0x" << std::setw(8) << std::setbase(16) << spirv[k] << ",";
                                if ((k % 8) == 7) {
                                    std::cout << std::endl << "                      ";
                                }
                            }
                            std::cout << std::setbase(10) << std::endl;
                        }
                    }
                }
                if (details >= DETAIL_MODE_ALL) {
                    std::string const json =
                        std::string(reinterpret_cast<char const *>(pcr.getJson(*pie)), static_cast<size_t>(pie->jsonSize));
                    std::cout << "  json:" << std::endl << json << std::endl;
                }
                std::cout << std::endl;
            } else if (details == DETAIL_MODE_BASIC) {
                std::cout << "index: " << std::setw(3) << i << " id: " << pie->pipelineIdentifier
                          << " pipelineMemorySize: " << pie->pipelineMemorySize << std::endl;
            }
            minPoolSize = std::min(minPoolSize, pie->pipelineMemorySize);
            maxPoolSize = std::max(maxPoolSize, pie->pipelineMemorySize);
            // std::cout << "pie " << i << ": " << *pie << std::endl;
        } else {
            std::cout << "pie " << i << ": not found" << std::endl;
            std::cout << "malformed pipeline cache!" << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    cout << "found " << pcr.getPipelineIndexCount() << " pipelines, sizes [" << minPoolSize << ", " << maxPoolSize << "]" << endl;

    return true;
}

void printUsageAndExit(char *executable, int exitCode) {
    std::cerr << "usage: " << executable << " [-h|--help] [-l|-f|-d|-a] [-pool <poolsize>] <pipeline_cache_file>" << std::endl
              << std::endl;
    std::cerr << "  -h | --help: print this usage message" << std::endl << std::endl;
    std::cerr << "  -l: list basic pipeline info (index, identifier, poolSize)" << std::endl << std::endl;
    std::cerr << "  -f: list first header (device info, pipeline index)" << std::endl << std::endl;
    std::cerr << "  -d: list detailed pipeline info (-l, plus all header fields)" << std::endl << std::endl;
    std::cerr << "  -a: list all pipeline info (-d, plus JSON and SPIR-V)" << std::endl << std::endl;
    std::cerr << "  -pool <poolsize>: add a bucket of size <poolsize>" << endl << endl;
    std::cerr << "  <pipeline_cache_file>: the pipeline cache file to parse (generated by PCC tool)" << std::endl << std::endl;

    exit(exitCode);
}

int main(int argc, char **argv) {
    ifstream file_cache;
    char *cache_filename{nullptr};
    int const lastArg{argc};
    DetailMode listMode{DETAIL_MODE_NONE};
    std::vector<PipelinePool> pools{};

    if (argc < 2) {
        printUsageAndExit(argv[0], EXIT_FAILURE);
    }

    // handle any optional arguments
    for (int i = 1; i < lastArg; i++) {
        if (strcmp(argv[i], "-l") == 0) {
            listMode = DETAIL_MODE_BASIC;
        } else if (strcmp(argv[i], "-f") == 0) {
            listMode = DETAIL_MODE_FIRST_HEADER;
        } else if (strcmp(argv[i], "-d") == 0) {
            listMode = DETAIL_MODE_ALL_HEADERS;
        } else if (strcmp(argv[i], "-a") == 0) {
            listMode = DETAIL_MODE_ALL;
        } else if (strcmp(argv[i], "-pool") == 0) {
            if (++i >= lastArg) {
                cout << "missing parameter <poolsize>" << endl << endl;
                printUsageAndExit(argv[0], EXIT_FAILURE);
            }

            std::string sizeString(argv[i]);
            bool isNumber = true;
            for (auto c : sizeString) {
                if (!std::isdigit(c)) isNumber = false;
            }
            if (!isNumber) {
                cout << "parameter <poolsize> must be numeric, found " << sizeString << endl << endl;
                printUsageAndExit(argv[0], EXIT_FAILURE);
            }

            uint64_t size = stoul(sizeString);
            cout << "adding pool of size " << size << endl;
            PipelinePool poolToAdd(size);
            pools.push_back(poolToAdd);
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            printUsageAndExit(argv[0], EXIT_SUCCESS);
        } else if (argv[i][0] != '-') {
            cache_filename = argv[i];
        } else {
            cerr << "unrecognized argument: " << argv[i] << endl << endl;
            printUsageAndExit(argv[0], EXIT_FAILURE);
        }
    }

    if (cache_filename == nullptr) {
        cerr << "missing parameter <pipeline_cache_file>" << endl << endl;
        printUsageAndExit(argv[0], EXIT_FAILURE);
    }

    std::cout << "parsing file: " << cache_filename << std::endl;
    file_cache.open(cache_filename, ios::in | ios::binary | ios::ate);
    if (!file_cache.is_open()) {
        std::cerr << "error: failed to open " << cache_filename << std::endl;
        return EXIT_FAILURE;
    }

    streampos size = file_cache.tellg();

    std::vector<uint8_t> mem_cache(static_cast<size_t>(size));
    file_cache.seekg(0, ios::beg);
    file_cache.read(reinterpret_cast<char *>(mem_cache.data()), static_cast<streamsize>(mem_cache.size()));
    file_cache.close();

    std::cout << "read " << size << " bytes from file" << std::endl;

    VKSCPipelineCacheHeaderReader pcr(mem_cache.size(), mem_cache.data());
    if (!pcr.isValid()) {
        std::cerr << "Requested file is not a valid VK_PIPELINE_CACHE_HEADER_VERSION_SAFETY_CRITICAL_ONE "
                     "cache. Aborting!"
                  << std::endl;
        return EXIT_FAILURE;
    }

    if (listMode >= DETAIL_MODE_BASIC) {
        printCacheInfo(pcr, listMode);
    }

    if (pools.size() > 0) {
        // sort the pools
        std::sort(pools.begin(), pools.end(), [](PipelinePool &a, PipelinePool &b) { return (a.poolSize < b.poolSize); });

        cout << "requested pools" << endl;
        for (auto pool : pools) {
            cout << "    pool size: " << pool.poolSize << endl;
        }

        bucketPipelines(pcr, pools);

        // print the pools, basic stats, and pipelines in them
        for (auto pool : pools) {
            cout << "======================================" << endl;
            cout << pool;
            cout << "pool min size: " << pool.poolMin << endl;
            cout << "pool max size: " << pool.poolMax << endl;
            cout << "pool average size: " << (pool.poolTotal / pool.poolEntries.size()) << endl;
            cout << "pool allocated space: " << (pool.poolSize * pool.poolEntries.size()) << endl;
            cout << "pool wasted space: " << (pool.poolSize * pool.poolEntries.size() - pool.poolTotal) << endl;
            cout << "pool entry identifiers (hex):" << endl;
            for (auto id : pool.poolEntries) {
                VkPipelineCacheSafetyCriticalIndexEntry const *pie = pcr.getPipelineIndexEntry(id);
                cout << "    " << pie->pipelineIdentifier << endl;
            }
        }
    }

    return EXIT_SUCCESS;
}
