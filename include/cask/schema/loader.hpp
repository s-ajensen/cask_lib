#pragma once

#include <cask/schema/serialization_registry.hpp>
#include <functional>
#include <nlohmann/json.hpp>
#include <queue>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace cask {

using ComponentResolver = std::function<void*(const std::string&)>;

inline std::vector<std::string> topological_sort(
    const std::vector<std::string>& names,
    const std::unordered_map<std::string, std::vector<std::string>>& dep_map
) {
    std::unordered_map<std::string, int> in_degree;
    std::unordered_map<std::string, std::vector<std::string>> adjacency;

    for (const auto& name : names) {
        in_degree[name] = 0;
    }

    for (const auto& name : names) {
        auto found = dep_map.find(name);
        if (found == dep_map.end()) {
            continue;
        }
        for (const auto& dep : found->second) {
            adjacency[dep].push_back(name);
            in_degree[name]++;
        }
    }

    std::queue<std::string> ready;
    for (const auto& [name, degree] : in_degree) {
        if (degree == 0) {
            ready.push(name);
        }
    }

    std::vector<std::string> sorted;
    while (!ready.empty()) {
        auto current = ready.front();
        ready.pop();
        sorted.push_back(current);

        auto adj_found = adjacency.find(current);
        if (adj_found == adjacency.end()) {
            continue;
        }
        for (const auto& dependent : adj_found->second) {
            in_degree[dependent]--;
            if (in_degree[dependent] == 0) {
                ready.push(dependent);
            }
        }
    }

    if (sorted.size() != names.size()) {
        throw std::runtime_error("circular dependency detected in component graph");
    }

    return sorted;
}

inline nlohmann::json load(
    const nlohmann::json& file_data,
    const SerializationRegistry& registry,
    ComponentResolver resolver
) {
    const auto& dependencies_section = file_data.at("dependencies");
    const auto& components_section = file_data.at("components");

    std::vector<std::string> names;
    for (const auto& [name, _] : components_section.items()) {
        names.push_back(name);
    }

    std::unordered_map<std::string, std::vector<std::string>> dep_map;
    for (const auto& [name, deps] : dependencies_section.items()) {
        std::vector<std::string> dep_list;
        for (const auto& dep : deps) {
            dep_list.push_back(dep.get<std::string>());
        }
        dep_map[name] = std::move(dep_list);
    }

    auto sorted = topological_sort(names, dep_map);

    nlohmann::json accumulated_context = nlohmann::json::object();

    for (const auto& name : sorted) {
        const auto& entry = registry.get(name);
        void* instance = resolver(name);
        const auto& component_data = components_section.at(name);

        auto contributions = entry.deserialize(component_data, instance, accumulated_context);

        accumulated_context.merge_patch(contributions);
    }

    return accumulated_context;
}

}
