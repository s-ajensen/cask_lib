#include <catch2/catch_all.hpp>
#include <cask/schema/cask_component.hpp>
#include <cstdint>
#include <string>

CASK_COMPONENT(Position, (float, x), (float, y), (float, z))

SCENARIO("CASK_COMPONENT defines a struct with expected fields", "[cask_component]") {
    GIVEN("a Position defined with CASK_COMPONENT") {
        WHEN("a Position is instantiated") {
            Position pos{1.0f, 2.0f, 3.0f};

            THEN("the fields have the expected values") {
                REQUIRE(pos.x == Catch::Approx(1.0f));
                REQUIRE(pos.y == Catch::Approx(2.0f));
                REQUIRE(pos.z == Catch::Approx(3.0f));
            }
        }
    }
}

SCENARIO("CASK_COMPONENT generates a describe method", "[cask_component]") {
    GIVEN("a Position defined with CASK_COMPONENT") {
        WHEN("describe is called") {
            auto entry = Position::describe();

            THEN("it returns a valid RegistryEntry with correct schema name") {
                REQUIRE(entry.schema["name"] == "Position");
            }

            THEN("it has the correct struct size") {
                REQUIRE(entry.schema["size"] == sizeof(Position));
            }

            THEN("it has three fields") {
                REQUIRE(entry.schema["fields"].size() == 3);
            }

            THEN("fields have correct metadata") {
                auto fields = entry.schema["fields"];

                REQUIRE(fields[0]["name"] == "x");
                REQUIRE(fields[0]["type"] == "float32");
                REQUIRE(fields[0]["size"] == sizeof(float));

                REQUIRE(fields[1]["name"] == "y");
                REQUIRE(fields[1]["type"] == "float32");
                REQUIRE(fields[1]["size"] == sizeof(float));

                REQUIRE(fields[2]["name"] == "z");
                REQUIRE(fields[2]["type"] == "float32");
                REQUIRE(fields[2]["size"] == sizeof(float));
            }
        }
    }
}

SCENARIO("CASK_COMPONENT serializes and deserializes", "[cask_component]") {
    GIVEN("a Position defined with CASK_COMPONENT") {
        auto entry = Position::describe();

        WHEN("a Position is serialized") {
            Position pos{1.0f, 2.0f, 3.0f};
            auto data = entry.serialize(&pos);

            THEN("the json has the correct values") {
                REQUIRE(data["x"] == Catch::Approx(1.0));
                REQUIRE(data["y"] == Catch::Approx(2.0));
                REQUIRE(data["z"] == Catch::Approx(3.0));
            }
        }

        WHEN("a Position is serialized and deserialized") {
            Position original{7.0f, 8.0f, 9.0f};
            auto data = entry.serialize(&original);

            Position restored{};
            entry.deserialize(data, &restored, nlohmann::json::object());

            THEN("the restored position matches the original") {
                REQUIRE(restored.x == Catch::Approx(original.x));
                REQUIRE(restored.y == Catch::Approx(original.y));
                REQUIRE(restored.z == Catch::Approx(original.z));
            }
        }
    }
}

CASK_COMPONENT(GameState, (int32_t, score), (float, time_remaining), (bool, paused))

SCENARIO("CASK_COMPONENT works with mixed types", "[cask_component]") {
    GIVEN("a GameState with int32_t, float, and bool fields") {
        WHEN("a GameState is instantiated") {
            GameState state{42, 3.14f, true};

            THEN("the fields have the expected values") {
                REQUIRE(state.score == 42);
                REQUIRE(state.time_remaining == Catch::Approx(3.14f));
                REQUIRE(state.paused == true);
            }
        }

        WHEN("a GameState round-trips through serialization") {
            auto entry = GameState::describe();
            GameState original{100, 59.5f, false};
            auto data = entry.serialize(&original);

            GameState restored{};
            entry.deserialize(data, &restored, nlohmann::json::object());

            THEN("all fields round-trip correctly") {
                REQUIRE(restored.score == 100);
                REQUIRE(restored.time_remaining == Catch::Approx(59.5f));
                REQUIRE(restored.paused == false);
            }
        }

        WHEN("the schema is inspected") {
            auto entry = GameState::describe();

            THEN("field types are correct") {
                REQUIRE(entry.schema["fields"][0]["type"] == "int32");
                REQUIRE(entry.schema["fields"][1]["type"] == "float32");
                REQUIRE(entry.schema["fields"][2]["type"] == "bool");
            }
        }
    }
}

CASK_COMPONENT(SingleField, (double, value))

SCENARIO("CASK_COMPONENT works with a single field", "[cask_component]") {
    GIVEN("a SingleField defined with one field") {
        WHEN("it is instantiated and described") {
            SingleField item{42.0};
            auto entry = SingleField::describe();

            THEN("the schema has one field") {
                REQUIRE(entry.schema["fields"].size() == 1);
                REQUIRE(entry.schema["fields"][0]["name"] == "value");
                REQUIRE(entry.schema["fields"][0]["type"] == "float64");
            }

            THEN("serialization round-trips correctly") {
                auto data = entry.serialize(&item);
                SingleField restored{};
                entry.deserialize(data, &restored, nlohmann::json::object());
                REQUIRE(restored.value == Catch::Approx(42.0));
            }
        }
    }
}

CASK_COMPONENT(ManyFields,
    (float, a), (float, b), (float, c), (float, d),
    (int32_t, e), (int32_t, f), (int32_t, g), (int32_t, h))

SCENARIO("CASK_COMPONENT works with many fields", "[cask_component]") {
    GIVEN("a ManyFields struct with 8 fields") {
        WHEN("it is instantiated and described") {
            ManyFields many{1.0f, 2.0f, 3.0f, 4.0f, 5, 6, 7, 8};
            auto entry = ManyFields::describe();

            THEN("the schema has 8 fields") {
                REQUIRE(entry.schema["fields"].size() == 8);
            }

            THEN("serialization round-trips correctly") {
                auto data = entry.serialize(&many);
                ManyFields restored{};
                entry.deserialize(data, &restored, nlohmann::json::object());

                REQUIRE(restored.a == Catch::Approx(1.0f));
                REQUIRE(restored.b == Catch::Approx(2.0f));
                REQUIRE(restored.c == Catch::Approx(3.0f));
                REQUIRE(restored.d == Catch::Approx(4.0f));
                REQUIRE(restored.e == 5);
                REQUIRE(restored.f == 6);
                REQUIRE(restored.g == 7);
                REQUIRE(restored.h == 8);
            }
        }
    }
}

SCENARIO("CASK_COMPONENT describe matches manual describe", "[cask_component]") {
    GIVEN("a Position defined with CASK_COMPONENT") {
        WHEN("comparing macro describe to manual describe") {
            auto macro_entry = Position::describe();
            auto manual_entry = cask::describe<Position>("Position", {
                cask::field("x", &Position::x),
                cask::field("y", &Position::y),
                cask::field("z", &Position::z)
            });

            THEN("the schemas are identical") {
                REQUIRE(macro_entry.schema == manual_entry.schema);
            }

            THEN("serialization produces identical output") {
                Position pos{1.0f, 2.0f, 3.0f};
                auto macro_data = macro_entry.serialize(&pos);
                auto manual_data = manual_entry.serialize(&pos);
                REQUIRE(macro_data == manual_data);
            }
        }
    }
}
