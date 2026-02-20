#pragma once

#include <cask/schema/loader.hpp>
#include <cask/schema/serialization_registry.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace cask {

inline nlohmann::json save(
    const std::vector<std::string>& component_names,
    const SerializationRegistry& registry,
    ComponentResolver resolver
) {
    nlohmann::json components = nlohmann::json::object();
    nlohmann::json dependencies = nlohmann::json::object();

    for (const auto& name : component_names) {
        const auto& entry = registry.get(name);
        void* instance = resolver(name);
        components[name] = entry.serialize(instance);

        if (!entry.dependencies.empty()) {
            dependencies[name] = entry.dependencies;
        }
    }

    return {{"dependencies", dependencies}, {"components", components}};
}

}
