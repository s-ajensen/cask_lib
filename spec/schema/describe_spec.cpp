#include <catch2/catch_all.hpp>
#include <cask/schema/describe.hpp>
#include <cstdint>

namespace {

struct Position {
    float x;
    float y;
    float z;
};

struct Mixed {
    int32_t id;
    float value;
    bool active;
};

}

SCENARIO("describe builds schema metadata for a struct", "[describe]") {
    GIVEN("a Position struct described with three float fields") {
        auto entry = cask::describe<Position>("Position", {
            cask::field("x", &Position::x),
            cask::field("y", &Position::y),
            cask::field("z", &Position::z)
        });

        WHEN("the schema is inspected") {
            const auto& schema = entry.schema;

            THEN("it has the correct name") {
                REQUIRE(schema["name"] == "Position");
            }

            THEN("it has the correct size") {
                REQUIRE(schema["size"] == sizeof(Position));
            }

            THEN("it has three fields") {
                REQUIRE(schema["fields"].size() == 3);
            }

            THEN("each field has correct metadata") {
                auto fields = schema["fields"];

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

SCENARIO("describe produces working serialize function", "[describe]") {
    GIVEN("a described Position entry") {
        auto entry = cask::describe<Position>("Position", {
            cask::field("x", &Position::x),
            cask::field("y", &Position::y),
            cask::field("z", &Position::z)
        });

        WHEN("a Position instance is serialized") {
            Position pos{1.0f, 2.0f, 3.0f};
            auto data = entry.serialize(&pos);

            THEN("the json contains the correct field values") {
                REQUIRE(data["x"] == Catch::Approx(1.0));
                REQUIRE(data["y"] == Catch::Approx(2.0));
                REQUIRE(data["z"] == Catch::Approx(3.0));
            }
        }
    }
}

SCENARIO("describe produces working deserialize function", "[describe]") {
    GIVEN("a described Position entry and json data") {
        auto entry = cask::describe<Position>("Position", {
            cask::field("x", &Position::x),
            cask::field("y", &Position::y),
            cask::field("z", &Position::z)
        });

        nlohmann::json data = {{"x", 4.0}, {"y", 5.0}, {"z", 6.0}};

        WHEN("the json is deserialized into a Position") {
            Position pos{};
            entry.deserialize(data, &pos, nlohmann::json{});

            THEN("the position has the correct values") {
                REQUIRE(pos.x == Catch::Approx(4.0));
                REQUIRE(pos.y == Catch::Approx(5.0));
                REQUIRE(pos.z == Catch::Approx(6.0));
            }
        }
    }
}

SCENARIO("describe round-trips serialization", "[describe]") {
    GIVEN("a described Position entry") {
        auto entry = cask::describe<Position>("Position", {
            cask::field("x", &Position::x),
            cask::field("y", &Position::y),
            cask::field("z", &Position::z)
        });

        WHEN("a Position is serialized then deserialized") {
            Position original{7.0f, 8.0f, 9.0f};
            auto data = entry.serialize(&original);

            Position restored{};
            entry.deserialize(data, &restored, nlohmann::json{});

            THEN("the restored position matches the original") {
                REQUIRE(restored.x == Catch::Approx(original.x));
                REQUIRE(restored.y == Catch::Approx(original.y));
                REQUIRE(restored.z == Catch::Approx(original.z));
            }
        }
    }
}

SCENARIO("describe works with mixed types including padding", "[describe]") {
    GIVEN("a Mixed struct with int32, float, and bool fields") {
        auto entry = cask::describe<Mixed>("Mixed", {
            cask::field("id", &Mixed::id),
            cask::field("value", &Mixed::value),
            cask::field("active", &Mixed::active)
        });

        WHEN("a Mixed instance is serialized and deserialized") {
            Mixed original{42, 3.14f, true};
            auto data = entry.serialize(&original);

            Mixed restored{};
            entry.deserialize(data, &restored, nlohmann::json{});

            THEN("all fields round-trip correctly") {
                REQUIRE(restored.id == 42);
                REQUIRE(restored.value == Catch::Approx(3.14f));
                REQUIRE(restored.active == true);
            }
        }

        WHEN("the schema is inspected") {
            THEN("it has correct size including padding") {
                REQUIRE(entry.schema["size"] == sizeof(Mixed));
            }

            THEN("field types are correct") {
                REQUIRE(entry.schema["fields"][0]["type"] == "int32");
                REQUIRE(entry.schema["fields"][1]["type"] == "float32");
                REQUIRE(entry.schema["fields"][2]["type"] == "bool");
            }
        }
    }
}

SCENARIO("describe rejects fields outside struct bounds", "[describe]") {
    GIVEN("a field info constructed with offset beyond sizeof(Position)") {
        cask::FieldInfo bad_field{
            nlohmann::json{{"name", "bad"}, {"type", "float32"}, {"size", sizeof(float)}},
            sizeof(Position),
            [](const void*) { return nlohmann::json{}; },
            [](const nlohmann::json&, void*) {}
        };

        WHEN("describe is called with the out-of-bounds field") {
            THEN("it throws") {
                REQUIRE_THROWS(cask::describe<Position>("Bad", {bad_field}));
            }
        }
    }
}

SCENARIO("describe rejects overlapping fields", "[describe]") {
    GIVEN("two field infos with overlapping offsets") {
        cask::FieldInfo field_a{
            nlohmann::json{{"name", "a"}, {"type", "float32"}, {"size", sizeof(float)}},
            0,
            [](const void*) { return nlohmann::json{}; },
            [](const nlohmann::json&, void*) {}
        };

        cask::FieldInfo field_b{
            nlohmann::json{{"name", "b"}, {"type", "float32"}, {"size", sizeof(float)}},
            2,
            [](const void*) { return nlohmann::json{}; },
            [](const nlohmann::json&, void*) {}
        };

        WHEN("describe is called with overlapping fields") {
            THEN("it throws") {
                REQUIRE_THROWS(cask::describe<Position>("Bad", {field_a, field_b}));
            }
        }
    }
}
