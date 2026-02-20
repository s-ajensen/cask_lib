#pragma once

#include <cask/ecs/entity_table.hpp>
#include <cask/identity/entity_registry.hpp>
#include <cask/identity/uuid.hpp>
#include <cask/schema/serialization_registry.hpp>
#include <stdexcept>
#include <string>

namespace cask {

inline SerializeFn build_entity_registry_serialize() {
    return [](const void* instance) -> nlohmann::json {
        const auto* registry = static_cast<const EntityRegistry*>(instance);
        nlohmann::json result = nlohmann::json::object();

        registry->each([&result](uint32_t entity, const UUID& uuid) {
            result[uuids::to_string(uuid)] = entity;
        });

        return result;
    };
}

inline DeserializeFn build_entity_registry_deserialize(EntityTable& table) {
    return [&table](const nlohmann::json& data, void* instance, const nlohmann::json&) -> nlohmann::json {
        auto* registry = static_cast<EntityRegistry*>(instance);
        nlohmann::json remap = nlohmann::json::object();

        for (const auto& [uuid_string, file_local_id] : data.items()) {
            auto parsed = uuids::uuid::from_string(uuid_string);
            if (!parsed.has_value()) {
                throw std::runtime_error("invalid UUID string: " + uuid_string);
            }
            uint32_t runtime_id = registry->resolve(parsed.value(), table);
            remap[std::to_string(file_local_id.get<uint32_t>())] = runtime_id;
        }

        return {{"entity_remap", remap}};
    };
}

inline RegistryEntry describe_entity_registry(const char* name, EntityTable& table) {
    nlohmann::json schema = {
        {"name", name},
        {"type", "entity_registry"}
    };

    return RegistryEntry{
        std::move(schema),
        build_entity_registry_serialize(),
        build_entity_registry_deserialize(table),
        {}
    };
}

}
