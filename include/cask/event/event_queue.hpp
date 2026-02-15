#pragma once

#include <utility>
#include <vector>

template<typename Event>
struct EventQueue {
    std::vector<Event> current_;
    std::vector<Event> previous_;

    void emit(Event event) {
        current_.push_back(event);
    }

    void swap() {
        std::swap(previous_, current_);
        current_.clear();
    }

    const std::vector<Event>& poll() {
        return previous_;
    }
};
