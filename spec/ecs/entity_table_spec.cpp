#include <catch2/catch_all.hpp>
#include <cask/ecs/entity_table.hpp>

SCENARIO("creating entities produces unique sequential IDs", "[entity_table]") {
    GIVEN("an empty entity table") {
        EntityTable table;

        WHEN("three entities are created") {
            auto first = table.create();
            auto second = table.create();
            auto third = table.create();

            THEN("they receive IDs 0, 1, and 2") {
                REQUIRE(first == 0);
                REQUIRE(second == 1);
                REQUIRE(third == 2);
            }

            THEN("all three are alive") {
                REQUIRE(table.alive(first));
                REQUIRE(table.alive(second));
                REQUIRE(table.alive(third));
            }
        }
    }
}

SCENARIO("querying entities by component signature", "[entity_table]") {
    GIVEN("three entities with different component signatures") {
        EntityTable table;
        auto entity_a = table.create();
        auto entity_b = table.create();
        auto entity_c = table.create();

        uint32_t TRANSFORM = 0;
        uint32_t VELOCITY = 1;
        uint32_t MESH = 2;

        table.add_component(entity_a, TRANSFORM);
        table.add_component(entity_a, VELOCITY);
        table.add_component(entity_b, TRANSFORM);
        table.add_component(entity_c, TRANSFORM);
        table.add_component(entity_c, MESH);

        WHEN("querying for entities with Transform") {
            Signature query_sig;
            query_sig.set(TRANSFORM);
            auto& results = table.query(query_sig);

            THEN("all three entities are returned") {
                REQUIRE(results.size() == 3);
                REQUIRE(std::find(results.begin(), results.end(), entity_a) != results.end());
                REQUIRE(std::find(results.begin(), results.end(), entity_b) != results.end());
                REQUIRE(std::find(results.begin(), results.end(), entity_c) != results.end());
            }
        }

        WHEN("querying for entities with Transform and Velocity") {
            Signature query_sig;
            query_sig.set(TRANSFORM);
            query_sig.set(VELOCITY);
            auto& results = table.query(query_sig);

            THEN("only entity A is returned") {
                REQUIRE(results.size() == 1);
                REQUIRE(results[0] == entity_a);
            }
        }
    }
}

SCENARIO("destroyed entity IDs are recycled", "[entity_table]") {
    GIVEN("an entity table with three entities") {
        EntityTable table;
        auto first = table.create();
        auto second = table.create();
        auto third = table.create();

        WHEN("the second entity is destroyed and a new entity is created") {
            table.destroy(second);

            THEN("the destroyed entity is no longer alive") {
                REQUIRE_FALSE(table.alive(second));
            }

            AND_WHEN("a new entity is created") {
                auto recycled = table.create();

                THEN("it receives the recycled ID") {
                    REQUIRE(recycled == 1);
                }

                THEN("the recycled entity is alive") {
                    REQUIRE(table.alive(recycled));
                }
            }
        }
    }
}

SCENARIO("removing a component bit unsets it in the signature", "[entity_table]") {
    GIVEN("an entity with component bit 3 set") {
        EntityTable table;
        auto entity = table.create();
        uint32_t HEALTH = 3;
        table.add_component(entity, HEALTH);

        REQUIRE(table.signatures_[entity].test(HEALTH));

        WHEN("the component is removed") {
            table.remove_component(entity, HEALTH);

            THEN("the bit is no longer set") {
                REQUIRE_FALSE(table.signatures_[entity].test(HEALTH));
            }
        }
    }
}

SCENARIO("entity no longer matches queries requiring removed component", "[entity_table]") {
    GIVEN("an entity with components on bits 1 and 2") {
        EntityTable table;
        auto entity = table.create();
        uint32_t VELOCITY = 1;
        uint32_t MESH = 2;
        table.add_component(entity, VELOCITY);
        table.add_component(entity, MESH);

        WHEN("component 2 is removed") {
            table.remove_component(entity, MESH);

            THEN("it no longer matches a query requiring both bits") {
                Signature both;
                both.set(VELOCITY);
                both.set(MESH);
                auto& results = table.query(both);
                REQUIRE(results.empty());
            }

            THEN("it still matches a query requiring only bit 1") {
                Signature just_velocity;
                just_velocity.set(VELOCITY);
                auto& results = table.query(just_velocity);
                REQUIRE(results.size() == 1);
                REQUIRE(results[0] == entity);
            }
        }
    }
}

SCENARIO("removing a bit that was never set is a no-op", "[entity_table]") {
    GIVEN("an entity with no components") {
        EntityTable table;
        auto entity = table.create();
        uint32_t SPRITE = 5;
        auto signature_before = table.signatures_[entity];

        WHEN("a never-set bit is removed") {
            table.remove_component(entity, SPRITE);

            THEN("the signature is unchanged") {
                REQUIRE(table.signatures_[entity] == signature_before);
            }
        }
    }
}
