#pragma once

#include <cask/schema/serialization_registry.hpp>
#include <cask/schema/type_name.hpp>
#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <string>
#include <vector>

namespace cask {

struct FieldInfo {
    nlohmann::json metadata;
    size_t offset;
    std::function<nlohmann::json(const void*)> serialize;
    std::function<void(const nlohmann::json&, void*)> deserialize;
};

template<typename T, typename M>
FieldInfo field(const char* name, M T::* member) {
    alignas(T) char storage[sizeof(T)]{};
    T* dummy = reinterpret_cast<T*>(storage);
    size_t offset = reinterpret_cast<size_t>(&(dummy->*member)) - reinterpret_cast<size_t>(dummy);

    nlohmann::json metadata = {
        {"name", name},
        {"type", type_name<M>::value},
        {"size", sizeof(M)}
    };

    return FieldInfo{
        std::move(metadata),
        offset,
        [offset](const void* instance) -> nlohmann::json {
            const auto* base = static_cast<const char*>(instance);
            const auto* field_ptr = reinterpret_cast<const M*>(base + offset);
            return nlohmann::json(*field_ptr);
        },
        [offset](const nlohmann::json& json, void* instance) {
            auto* base = static_cast<char*>(instance);
            auto* field_ptr = reinterpret_cast<M*>(base + offset);
            *field_ptr = json.get<M>();
        }
    };
}

inline void validate_field_bounds(const std::vector<FieldInfo>& fields, size_t struct_size) {
    for (const auto& field_info : fields) {
        size_t field_end = field_info.offset + field_info.metadata["size"].get<size_t>();
        if (field_end > struct_size) {
            throw std::runtime_error(
                "Field '" + field_info.metadata["name"].get<std::string>() +
                "' extends beyond struct bounds"
            );
        }
    }
}

inline void validate_no_overlaps(const std::vector<FieldInfo>& fields) {
    for (size_t outer = 0; outer < fields.size(); ++outer) {
        size_t outer_start = fields[outer].offset;
        size_t outer_end = outer_start + fields[outer].metadata["size"].get<size_t>();

        for (size_t inner = outer + 1; inner < fields.size(); ++inner) {
            size_t inner_start = fields[inner].offset;
            size_t inner_end = inner_start + fields[inner].metadata["size"].get<size_t>();

            if (outer_start < inner_end && inner_start < outer_end) {
                throw std::runtime_error(
                    "Fields '" + fields[outer].metadata["name"].get<std::string>() +
                    "' and '" + fields[inner].metadata["name"].get<std::string>() +
                    "' overlap"
                );
            }
        }
    }
}

inline nlohmann::json build_schema(const char* name, size_t struct_size, const std::vector<FieldInfo>& fields) {
    nlohmann::json fields_json = nlohmann::json::array();
    for (const auto& field_info : fields) {
        fields_json.push_back(field_info.metadata);
    }
    return {{"name", name}, {"size", struct_size}, {"fields", fields_json}};
}

inline SerializeFn build_serialize(const std::vector<FieldInfo>& fields) {
    return [fields](const void* instance) -> nlohmann::json {
        nlohmann::json result = nlohmann::json::object();
        for (const auto& field_info : fields) {
            std::string field_name = field_info.metadata["name"].get<std::string>();
            result[field_name] = field_info.serialize(instance);
        }
        return result;
    };
}

inline DeserializeFn build_deserialize(const std::vector<FieldInfo>& fields) {
    return [fields](const nlohmann::json& json, void* instance, const nlohmann::json&) -> nlohmann::json {
        for (const auto& field_info : fields) {
            std::string field_name = field_info.metadata["name"].get<std::string>();
            field_info.deserialize(json.at(field_name), instance);
        }
        return nlohmann::json::object();
    };
}

template<typename T>
RegistryEntry describe(const char* name, std::initializer_list<FieldInfo> field_list) {
    std::vector<FieldInfo> fields(field_list);

    validate_field_bounds(fields, sizeof(T));
    validate_no_overlaps(fields);

    return RegistryEntry{
        build_schema(name, sizeof(T), fields),
        build_serialize(fields),
        build_deserialize(fields),
        {}
    };
}

}
