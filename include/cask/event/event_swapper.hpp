#pragma once

#include <vector>
#include <cask/event/event_queue.hpp>

using SwapFn = void(*)(void*);

template<typename Event>
void swap_queue(void* ptr) {
    static_cast<EventQueue<Event>*>(ptr)->swap();
}

struct EventSwapper {
    struct Entry {
        void* queue;
        SwapFn fn;
    };

    std::vector<Entry> entries_;

    void add(void* queue, SwapFn fn) {
        entries_.push_back(Entry{queue, fn});
    }

    void swap_all() {
        for (auto& entry : entries_) {
            entry.fn(entry.queue);
        }
    }
};
