#include <catch2/catch_all.hpp>
#include <cask/schema/serialization_registry.hpp>

SCENARIO("registry stores and retrieves entries", "[serialization_registry]") {
    GIVEN("a registry with an entry added") {
        cask::SerializationRegistry registry;

        cask::RegistryEntry entry{
            nlohmann::json{{"name", "Position"}},
            [](const void*) { return nlohmann::json{{"x", 1.0}}; },
            [](const nlohmann::json&, void*, const nlohmann::json&) { return nlohmann::json::object(); },
            {}
        };

        registry.add("Position", entry);

        WHEN("has is called with the registered name") {
            THEN("it returns true") {
                REQUIRE(registry.has("Position"));
            }
        }

        WHEN("has is called with an unregistered name") {
            THEN("it returns false") {
                REQUIRE_FALSE(registry.has("Velocity"));
            }
        }

        WHEN("get is called with the registered name") {
            const auto& retrieved = registry.get("Position");

            THEN("the schema matches what was added") {
                REQUIRE(retrieved.schema["name"] == "Position");
            }

            THEN("the serialize function works") {
                auto result = retrieved.serialize(nullptr);
                REQUIRE(result["x"] == 1.0);
            }
        }
    }
}

SCENARIO("registry entry has empty dependencies by default", "[serialization_registry]") {
    GIVEN("a registry entry with no dependencies") {
        cask::RegistryEntry entry{
            nlohmann::json{{"name", "Position"}},
            [](const void*) { return nlohmann::json{}; },
            [](const nlohmann::json&, void*, const nlohmann::json&) { return nlohmann::json::object(); },
            {}
        };

        THEN("the dependencies vector is empty") {
            REQUIRE(entry.dependencies.empty());
        }
    }
}

SCENARIO("registry entry stores dependencies", "[serialization_registry]") {
    GIVEN("a registry entry with dependencies") {
        cask::RegistryEntry entry{
            nlohmann::json{{"name", "Positions"}},
            [](const void*) { return nlohmann::json{}; },
            [](const nlohmann::json&, void*, const nlohmann::json&) { return nlohmann::json::object(); },
            {"EntityRegistry"}
        };

        THEN("the dependencies vector contains the declared dependency") {
            REQUIRE(entry.dependencies.size() == 1);
            REQUIRE(entry.dependencies[0] == "EntityRegistry");
        }
    }
}

SCENARIO("deserialize function receives context and returns context contributions", "[serialization_registry]") {
    GIVEN("a registry entry whose deserializer produces context") {
        cask::RegistryEntry entry{
            nlohmann::json{{"name", "Producer"}},
            [](const void*) { return nlohmann::json{}; },
            [](const nlohmann::json&, void*, const nlohmann::json&) {
                return nlohmann::json{{"entity_remap", {{"42", 8}}}};
            },
            {}
        };

        WHEN("deserialize is called") {
            nlohmann::json context;
            auto contributions = entry.deserialize(nlohmann::json{}, nullptr, context);

            THEN("it returns the context contributions") {
                REQUIRE(contributions["entity_remap"]["42"] == 8);
            }
        }
    }
}

SCENARIO("registry throws for missing entries", "[serialization_registry]") {
    GIVEN("an empty registry") {
        cask::SerializationRegistry registry;

        WHEN("get is called with a missing name") {
            THEN("it throws") {
                REQUIRE_THROWS(registry.get("Missing"));
            }
        }
    }
}
