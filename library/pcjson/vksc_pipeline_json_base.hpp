/*
 * Copyright (c) 2025 The Khronos Group Inc.
 * Copyright (c) 2025 RasterGrid Kft.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <string>
#include <sstream>
#include <stdlib.h>

namespace pcjson {

class Base {
  protected:
    struct LocationDesc {
        const char* name;
        bool isPointer;
        bool isArray;
        uint32_t arrayIndex;
    };

    class LocationScope {
      public:
        LocationScope(std::vector<LocationDesc>& location, const char* name, bool pointer = false) : location_(location) {
            location_.push_back({name, pointer, false, 0});
        }

        LocationScope(std::vector<LocationDesc>& location, const char* name, uint32_t index) : location_(location) {
            location_.push_back({name, false, true, index});
        }

        ~LocationScope() {
            location_.pop_back();
        }

        LocationScope(const LocationScope&) = delete;
        LocationScope& operator=(const LocationScope&) = delete;

      private:
        std::vector<LocationDesc>& location_;
    };

    class MemoryBlock {
      public:
        static constexpr size_t kDefaultSize = 32768;
        static constexpr size_t kDefaultAlignment = 64;

        MemoryBlock(size_t size = kDefaultSize) : size_(size), used_bytes_(0) {
#ifdef _WIN32
            block_ = reinterpret_cast<uint8_t*>(_aligned_malloc(size, kDefaultAlignment));
#else
            block_ = reinterpret_cast<uint8_t*>(aligned_alloc(kDefaultAlignment, size));
#endif
        }

        ~MemoryBlock() { Reset(); }

        MemoryBlock(MemoryBlock&& other) { MoveFrom(std::move(other)); }
        MemoryBlock& operator=(MemoryBlock&& other) { return MoveFrom(std::move(other)); }

        void* Alloc(size_t alignment, size_t size) {
            const size_t aligned_alloc_offset = (used_bytes_ + alignment - 1) & ~(alignment - 1);
            if (aligned_alloc_offset + size <= size_) {
                used_bytes_ = aligned_alloc_offset + size;
                return &block_[0] + aligned_alloc_offset;
            } else {
                return nullptr;
            }
        }

      private:
        MemoryBlock(const MemoryBlock&) = delete;
        MemoryBlock& operator=(const MemoryBlock&) = delete;

        MemoryBlock& MoveFrom(MemoryBlock&& other) {
            if (&other != this) {
                Reset();
                block_ = other.block_;
                size_ = other.size_;
                used_bytes_ = other.used_bytes_;
                other.block_ = nullptr;
                other.size_ = 0;
                other.used_bytes_ = 0;
            }
            return *this;
        }

        void Reset() {
            if (block_ != nullptr) {
#ifdef _WIN32
                _aligned_free(block_);
#else
                free(block_);
#endif
            }
            block_ = nullptr;
            size_ = 0;
            used_bytes_ = 0;
        }

        uint8_t* block_{nullptr};
        size_t size_{0};
        size_t used_bytes_{0};
    };

    Base() {
        location_.reserve(64);
        memory_blocks_.reserve(32);
    }

    void ClearStatusAndMessages() {
        status_ = true;
        message_sink_.clear();
    }

    bool IsStatusOK() const {
        return status_;
    }

    LocationScope CreateScope(const char* scope, bool pointer = false) {
        return LocationScope(location_, scope, pointer);
    }

    LocationScope CreateScope(const char* scope, uint32_t index) {
        return LocationScope(location_, scope, index);
    }

    template <typename T>
    T* AllocMem(size_t count = 1) {
        if (memory_blocks_.empty()) {
            memory_blocks_.emplace_back();
        }
        void* ptr = memory_blocks_.back().Alloc(alignof(T), count * sizeof(T));
        if (ptr == nullptr) {
            memory_blocks_.emplace_back(std::max(MemoryBlock::kDefaultSize, count * sizeof(T)));
            ptr = memory_blocks_.back().Alloc(alignof(T), count * sizeof(T));
        }
        return reinterpret_cast<T*>(ptr);
    }

    void FreeAllMem() {
        memory_blocks_.clear();
    }

    std::stringstream& Error() {
        if (message_sink_.rdbuf()->in_avail() > 0) {
            message_sink_ << std::endl;
        }
        message_sink_ << "[ERROR] ";
        status_ = false;
        AddLocationInfo();
        return message_sink_;
    }

    std::stringstream& Warn() {
        if (message_sink_.rdbuf()->in_avail() > 0) {
            message_sink_ << std::endl;
        }
        message_sink_ << "[WARNING] ";
        AddLocationInfo();
        return message_sink_;
    }

    const char* GetMessages() {
        if (message_sink_.rdbuf()->in_avail() > 0) {
            messages_ = message_sink_.str();
            return messages_.c_str();
        } else {
            return nullptr;
        }
    }

  private:
    void AddLocationInfo() {
        if (!location_.empty()) { 
            const LocationDesc* prev_loc = nullptr;
            for (const auto& loc : location_) {
                if (prev_loc != nullptr) {
                    message_sink_ << (prev_loc->isPointer ? "->" : ".");
                }
                message_sink_ << loc.name;
                if (loc.isArray) {
                    message_sink_ << "[" << loc.arrayIndex << "]";
                }
                prev_loc = &loc;
            }
            message_sink_ << ": ";
        }
    }

    bool status_{};

    std::stringstream message_sink_{};
    std::string messages_{};

    std::vector<LocationDesc> location_{};

    std::vector<MemoryBlock> memory_blocks_{};
};

}  // namespace pcjson
