#include <catch2/catch_all.hpp>
#include <cask/identity/entity_registry.hpp>
#include <cask/identity/uuid.hpp>
#include <cask/schema/describe_entity_registry.hpp>
#include <cask/ecs/entity_table.hpp>

SCENARIO("entity registry serialization produces uuid-keyed json", "[entity_registry_serialization]") {
    GIVEN("an entity registry with known UUID-to-entity mappings") {
        EntityTable table;
        EntityRegistry registry;

        auto uuid_a = cask::generate_uuid();
        auto uuid_b = cask::generate_uuid();
        uint32_t entity_a = registry.resolve(uuid_a, table);
        uint32_t entity_b = registry.resolve(uuid_b, table);

        auto entry = cask::describe_entity_registry("EntityRegistry", table);

        WHEN("the registry is serialized") {
            auto data = entry.serialize(&registry);

            THEN("the json contains UUID strings as keys") {
                REQUIRE(data.contains(uuids::to_string(uuid_a)));
                REQUIRE(data.contains(uuids::to_string(uuid_b)));
            }

            THEN("the values are the runtime entity IDs") {
                REQUIRE(data[uuids::to_string(uuid_a)] == entity_a);
                REQUIRE(data[uuids::to_string(uuid_b)] == entity_b);
            }
        }
    }
}

SCENARIO("entity registry deserialization resolves UUIDs and produces remap context", "[entity_registry_serialization]") {
    GIVEN("json data with UUID-to-entity mappings and a fresh registry") {
        auto uuid_a = cask::generate_uuid();
        auto uuid_b = cask::generate_uuid();

        nlohmann::json data = {
            {uuids::to_string(uuid_a), 42},
            {uuids::to_string(uuid_b), 17}
        };

        EntityTable fresh_table;
        EntityRegistry fresh_registry;
        auto entry = cask::describe_entity_registry("EntityRegistry", fresh_table);

        WHEN("the json is deserialized into the fresh registry") {
            auto context = entry.deserialize(data, &fresh_registry, nlohmann::json{});

            THEN("the fresh registry resolves the same UUIDs") {
                REQUIRE(fresh_registry.size() == 2);
                REQUIRE(fresh_registry.has(fresh_registry.resolve(uuid_a, fresh_table)));
                REQUIRE(fresh_registry.has(fresh_registry.resolve(uuid_b, fresh_table)));
            }

            THEN("the returned context contains entity_remap") {
                REQUIRE(context.contains("entity_remap"));
            }

            THEN("the remap maps file-local IDs to new runtime IDs") {
                auto remap = context["entity_remap"];
                uint32_t runtime_a = fresh_registry.resolve(uuid_a, fresh_table);
                uint32_t runtime_b = fresh_registry.resolve(uuid_b, fresh_table);

                REQUIRE(remap["42"] == runtime_a);
                REQUIRE(remap["17"] == runtime_b);
            }
        }
    }
}

SCENARIO("entity registry entry has no dependencies", "[entity_registry_serialization]") {
    GIVEN("a described entity registry entry") {
        EntityTable table;
        auto entry = cask::describe_entity_registry("EntityRegistry", table);

        THEN("the dependencies are empty") {
            REQUIRE(entry.dependencies.empty());
        }
    }
}

SCENARIO("entity registry round-trips with new runtime IDs", "[entity_registry_serialization]") {
    GIVEN("an entity registry with mappings") {
        EntityTable original_table;
        EntityRegistry original_registry;

        auto uuid_a = cask::generate_uuid();
        auto uuid_b = cask::generate_uuid();
        original_registry.resolve(uuid_a, original_table);
        original_registry.resolve(uuid_b, original_table);

        auto original_entry = cask::describe_entity_registry("EntityRegistry", original_table);

        WHEN("the registry is serialized then deserialized into a fresh registry") {
            auto data = original_entry.serialize(&original_registry);

            EntityTable fresh_table;
            EntityRegistry fresh_registry;
            auto fresh_entry = cask::describe_entity_registry("EntityRegistry", fresh_table);

            fresh_entry.deserialize(data, &fresh_registry, nlohmann::json{});

            THEN("the fresh registry has the same UUIDs") {
                REQUIRE(fresh_registry.size() == 2);
                REQUIRE_NOTHROW(fresh_registry.identify(fresh_registry.resolve(uuid_a, fresh_table)));
                REQUIRE_NOTHROW(fresh_registry.identify(fresh_registry.resolve(uuid_b, fresh_table)));
            }

            THEN("the UUIDs match the originals") {
                uint32_t new_a = fresh_registry.resolve(uuid_a, fresh_table);
                uint32_t new_b = fresh_registry.resolve(uuid_b, fresh_table);
                REQUIRE(fresh_registry.identify(new_a) == uuid_a);
                REQUIRE(fresh_registry.identify(new_b) == uuid_b);
            }
        }
    }
}
