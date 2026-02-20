#pragma once

#include <cask/ecs/component_store.hpp>
#include <cask/schema/serialization_registry.hpp>
#include <string>

namespace cask {

template<typename T>
SerializeFn build_store_serialize(const RegistryEntry& value_entry) {
    return [value_entry](const void* instance) -> nlohmann::json {
        const auto* store = static_cast<const ComponentStore<T>*>(instance);
        nlohmann::json result = nlohmann::json::object();

        store->each([&result, &value_entry](uint32_t entity, const T& value) {
            result[std::to_string(entity)] = value_entry.serialize(&value);
        });

        return result;
    };
}

template<typename T>
DeserializeFn build_store_deserialize(const RegistryEntry& value_entry) {
    return [value_entry](const nlohmann::json& json, void* instance, const nlohmann::json& context) -> nlohmann::json {
        auto* store = static_cast<ComponentStore<T>*>(instance);
        const auto& remap = context.at("entity_remap");

        for (const auto& [key, value_json] : json.items()) {
            auto remap_found = remap.find(key);
            if (remap_found == remap.end()) {
                throw std::runtime_error("entity_remap missing key: " + key);
            }
            uint32_t entity = remap_found->get<uint32_t>();
            T value{};
            value_entry.deserialize(value_json, &value, nlohmann::json{});
            store->insert(entity, std::move(value));
        }

        return nlohmann::json::object();
    };
}

template<typename T>
RegistryEntry describe_component_store(const char* name, const RegistryEntry& value_entry) {
    std::string value_type_name = value_entry.schema["name"].get<std::string>();

    nlohmann::json schema = {
        {"name", name},
        {"container", "component_store"},
        {"value_type", value_type_name}
    };

    return RegistryEntry{
        std::move(schema),
        build_store_serialize<T>(value_entry),
        build_store_deserialize<T>(value_entry),
        {"EntityRegistry"}
    };
}

}
