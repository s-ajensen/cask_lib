#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

template<typename Resource>
struct ResourceStore {
    std::vector<Resource> resources_;
    std::unordered_map<std::string, uint32_t> key_to_handle_;

    uint32_t store(const std::string& key, Resource data) {
        auto existing = key_to_handle_.find(key);
        if (existing != key_to_handle_.end()) {
            return existing->second;
        }
        uint32_t handle = resources_.size();
        resources_.push_back(data);
        key_to_handle_[key] = handle;
        return handle;
    }

    Resource& get(uint32_t handle) {
        return resources_[handle];
    }
};
