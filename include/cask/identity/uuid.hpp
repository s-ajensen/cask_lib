#pragma once

#include <random>
#include <uuid.h>

namespace cask {

using UUID = uuids::uuid;

inline UUID generate_uuid() {
    static thread_local std::mt19937 engine{std::random_device{}()};
    static thread_local uuids::uuid_random_generator generator{engine};
    return generator();
}

}
