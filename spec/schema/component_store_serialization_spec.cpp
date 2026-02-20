#include <catch2/catch_all.hpp>
#include <cask/schema/describe.hpp>
#include <cask/schema/describe_component_store.hpp>
#include <cask/ecs/component_store.hpp>

namespace {

struct Position {
    float x;
    float y;
    float z;
};

cask::RegistryEntry position_entry() {
    return cask::describe<Position>("Position", {
        cask::field("x", &Position::x),
        cask::field("y", &Position::y),
        cask::field("z", &Position::z)
    });
}

nlohmann::json identity_remap(std::initializer_list<uint32_t> entities) {
    nlohmann::json remap = nlohmann::json::object();
    for (uint32_t entity : entities) {
        remap[std::to_string(entity)] = entity;
    }
    return {{"entity_remap", remap}};
}

}

SCENARIO("component store serialization produces entity-keyed json", "[component_store_serialization]") {
    GIVEN("a component store with two entities and a described Position entry") {
        ComponentStore<Position> store;
        store.insert(42, Position{1.0f, 2.0f, 3.0f});
        store.insert(17, Position{4.0f, 5.0f, 6.0f});

        auto value_entry = position_entry();
        auto store_entry = cask::describe_component_store<Position>("Positions", value_entry);

        WHEN("the store is serialized") {
            auto data = store_entry.serialize(&store);

            THEN("the json contains entity 42 with correct values") {
                REQUIRE(data.contains("42"));
                REQUIRE(data["42"]["x"] == Catch::Approx(1.0));
                REQUIRE(data["42"]["y"] == Catch::Approx(2.0));
                REQUIRE(data["42"]["z"] == Catch::Approx(3.0));
            }

            THEN("the json contains entity 17 with correct values") {
                REQUIRE(data.contains("17"));
                REQUIRE(data["17"]["x"] == Catch::Approx(4.0));
                REQUIRE(data["17"]["y"] == Catch::Approx(5.0));
                REQUIRE(data["17"]["z"] == Catch::Approx(6.0));
            }
        }
    }
}

SCENARIO("component store deserialization rebuilds the store with remapped entities", "[component_store_serialization]") {
    GIVEN("json data representing two entities and an identity remap") {
        nlohmann::json data = {
            {"42", {{"x", 1.0}, {"y", 2.0}, {"z", 3.0}}},
            {"17", {{"x", 4.0}, {"y", 5.0}, {"z", 6.0}}}
        };

        auto value_entry = position_entry();
        auto store_entry = cask::describe_component_store<Position>("Positions", value_entry);
        auto context = identity_remap({42, 17});

        WHEN("the json is deserialized into a ComponentStore") {
            ComponentStore<Position> store;
            store_entry.deserialize(data, &store, context);

            THEN("entity 42 has the correct values") {
                auto& pos = store.get(42);
                REQUIRE(pos.x == Catch::Approx(1.0));
                REQUIRE(pos.y == Catch::Approx(2.0));
                REQUIRE(pos.z == Catch::Approx(3.0));
            }

            THEN("entity 17 has the correct values") {
                auto& pos = store.get(17);
                REQUIRE(pos.x == Catch::Approx(4.0));
                REQUIRE(pos.y == Catch::Approx(5.0));
                REQUIRE(pos.z == Catch::Approx(6.0));
            }
        }
    }
}

SCENARIO("component store deserialization applies entity remapping", "[component_store_serialization]") {
    GIVEN("json data with file-local entity IDs and a remap table") {
        nlohmann::json data = {
            {"42", {{"x", 1.0}, {"y", 2.0}, {"z", 3.0}}},
            {"17", {{"x", 4.0}, {"y", 5.0}, {"z", 6.0}}}
        };

        nlohmann::json context = {
            {"entity_remap", {{"42", 100}, {"17", 200}}}
        };

        auto value_entry = position_entry();
        auto store_entry = cask::describe_component_store<Position>("Positions", value_entry);

        WHEN("the json is deserialized with remapped IDs") {
            ComponentStore<Position> store;
            store_entry.deserialize(data, &store, context);

            THEN("the store contains entries at the remapped entity IDs") {
                auto& pos100 = store.get(100);
                REQUIRE(pos100.x == Catch::Approx(1.0));
                REQUIRE(pos100.y == Catch::Approx(2.0));
                REQUIRE(pos100.z == Catch::Approx(3.0));

                auto& pos200 = store.get(200);
                REQUIRE(pos200.x == Catch::Approx(4.0));
                REQUIRE(pos200.y == Catch::Approx(5.0));
                REQUIRE(pos200.z == Catch::Approx(6.0));
            }
        }
    }
}

SCENARIO("component store deserialization throws when remap table is missing", "[component_store_serialization]") {
    GIVEN("json data and an empty context") {
        nlohmann::json data = {
            {"42", {{"x", 1.0}, {"y", 2.0}, {"z", 3.0}}}
        };

        auto value_entry = position_entry();
        auto store_entry = cask::describe_component_store<Position>("Positions", value_entry);

        WHEN("deserialization is attempted without entity_remap in context") {
            ComponentStore<Position> store;

            THEN("it throws a descriptive error") {
                REQUIRE_THROWS(store_entry.deserialize(data, &store, nlohmann::json{}));
            }
        }
    }
}

SCENARIO("component store deserialization throws when remap key is missing", "[component_store_serialization]") {
    GIVEN("json data with an entity ID not in the remap table") {
        nlohmann::json data = {
            {"42", {{"x", 1.0}, {"y", 2.0}, {"z", 3.0}}}
        };

        nlohmann::json context = {
            {"entity_remap", {{"99", 100}}}
        };

        auto value_entry = position_entry();
        auto store_entry = cask::describe_component_store<Position>("Positions", value_entry);

        WHEN("deserialization is attempted with missing remap key") {
            ComponentStore<Position> store;

            THEN("it throws a descriptive error") {
                REQUIRE_THROWS(store_entry.deserialize(data, &store, context));
            }
        }
    }
}

SCENARIO("component store round-trips through serialization with identity remap", "[component_store_serialization]") {
    GIVEN("a component store with entries") {
        ComponentStore<Position> original;
        original.insert(42, Position{1.0f, 2.0f, 3.0f});
        original.insert(17, Position{4.0f, 5.0f, 6.0f});

        auto value_entry = position_entry();
        auto store_entry = cask::describe_component_store<Position>("Positions", value_entry);

        WHEN("the store is serialized then deserialized with identity remap") {
            auto data = store_entry.serialize(&original);
            auto context = identity_remap({42, 17});

            ComponentStore<Position> restored;
            store_entry.deserialize(data, &restored, context);

            THEN("both entities round-trip correctly") {
                auto& pos42 = restored.get(42);
                REQUIRE(pos42.x == Catch::Approx(1.0));
                REQUIRE(pos42.y == Catch::Approx(2.0));
                REQUIRE(pos42.z == Catch::Approx(3.0));

                auto& pos17 = restored.get(17);
                REQUIRE(pos17.x == Catch::Approx(4.0));
                REQUIRE(pos17.y == Catch::Approx(5.0));
                REQUIRE(pos17.z == Catch::Approx(6.0));
            }
        }
    }
}

SCENARIO("empty component store serializes to empty json", "[component_store_serialization]") {
    GIVEN("an empty component store") {
        ComponentStore<Position> store;

        auto value_entry = position_entry();
        auto store_entry = cask::describe_component_store<Position>("Positions", value_entry);

        WHEN("the empty store is serialized") {
            auto data = store_entry.serialize(&store);

            THEN("the result is an empty json object") {
                REQUIRE(data.is_object());
                REQUIRE(data.empty());
            }
        }
    }
}

SCENARIO("component store schema has container metadata", "[component_store_serialization]") {
    GIVEN("a described component store entry") {
        auto value_entry = position_entry();
        auto store_entry = cask::describe_component_store<Position>("Positions", value_entry);

        WHEN("the schema is inspected") {
            THEN("it has the correct name") {
                REQUIRE(store_entry.schema["name"] == "Positions");
            }

            THEN("it identifies as a component_store container") {
                REQUIRE(store_entry.schema["container"] == "component_store");
            }

            THEN("it references the value type") {
                REQUIRE(store_entry.schema["value_type"] == "Position");
            }
        }
    }
}

SCENARIO("component store entry declares EntityRegistry dependency", "[component_store_serialization]") {
    GIVEN("a described component store entry") {
        auto value_entry = position_entry();
        auto store_entry = cask::describe_component_store<Position>("Positions", value_entry);

        THEN("the entry depends on EntityRegistry") {
            REQUIRE(store_entry.dependencies.size() == 1);
            REQUIRE(store_entry.dependencies[0] == "EntityRegistry");
        }
    }
}

SCENARIO("component store deserialization returns empty context", "[component_store_serialization]") {
    GIVEN("json data and a valid remap context") {
        nlohmann::json data = {
            {"42", {{"x", 1.0}, {"y", 2.0}, {"z", 3.0}}}
        };
        nlohmann::json context = {{"entity_remap", {{"42", 100}}}};

        auto value_entry = position_entry();
        auto store_entry = cask::describe_component_store<Position>("Positions", value_entry);

        WHEN("the store is deserialized") {
            ComponentStore<Position> store;
            auto contributions = store_entry.deserialize(data, &store, context);

            THEN("the returned context contributions are empty") {
                REQUIRE(contributions.empty());
            }
        }
    }
}
