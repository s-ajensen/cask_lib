#pragma once

#include <cask/ecs/entity_table.hpp>
#include <cask/identity/uuid.hpp>
#include <stdexcept>
#include <unordered_map>

struct EntityRegistry {
    std::unordered_map<cask::UUID, uint32_t> uuid_to_entity_;
    std::unordered_map<uint32_t, cask::UUID> entity_to_uuid_;

    uint32_t resolve(const cask::UUID& uuid, EntityTable& table) {
        auto found = uuid_to_entity_.find(uuid);
        if (found != uuid_to_entity_.end()) {
            return found->second;
        }
        uint32_t entity = table.create();
        uuid_to_entity_[uuid] = entity;
        entity_to_uuid_[entity] = uuid;
        return entity;
    }

    cask::UUID identify(uint32_t entity) const {
        auto found = entity_to_uuid_.find(entity);
        if (found == entity_to_uuid_.end()) {
            throw std::runtime_error("entity has no UUID");
        }
        return found->second;
    }

    bool has(uint32_t entity) const {
        return entity_to_uuid_.count(entity) > 0;
    }

    void assign(uint32_t entity, const cask::UUID& uuid) {
        auto uuid_found = uuid_to_entity_.find(uuid);
        if (uuid_found != uuid_to_entity_.end() && uuid_found->second != entity) {
            throw std::runtime_error("UUID already mapped to a different entity");
        }
        auto entity_found = entity_to_uuid_.find(entity);
        if (entity_found != entity_to_uuid_.end() && entity_found->second != uuid) {
            throw std::runtime_error("entity already has a different UUID");
        }
        uuid_to_entity_[uuid] = entity;
        entity_to_uuid_[entity] = uuid;
    }

    void remove(uint32_t entity) {
        auto found = entity_to_uuid_.find(entity);
        if (found == entity_to_uuid_.end()) {
            return;
        }
        cask::UUID uuid = found->second;
        entity_to_uuid_.erase(found);
        uuid_to_entity_.erase(uuid);
    }

    size_t size() const {
        return entity_to_uuid_.size();
    }
};
