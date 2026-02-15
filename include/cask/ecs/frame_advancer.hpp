#pragma once

#include <vector>
#include <cask/ecs/interpolated.hpp>

using AdvanceFn = void(*)(void*);

template<typename ValueType>
void advance_interpolated(void* ptr) {
    static_cast<Interpolated<ValueType>*>(ptr)->advance();
}

struct FrameAdvancer {
    struct Entry {
        void* interpolated;
        AdvanceFn fn;
    };

    std::vector<Entry> entries_;

    void add(void* interpolated, AdvanceFn fn) {
        entries_.push_back(Entry{interpolated, fn});
    }

    void advance_all() {
        for (auto& entry : entries_) {
            entry.fn(entry.interpolated);
        }
    }
};
