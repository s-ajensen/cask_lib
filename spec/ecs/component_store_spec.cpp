#include <catch2/catch_all.hpp>
#include <cask/ecs/component_store.hpp>

struct Position {
    float x;
    float y;
};

SCENARIO("component store retrieves inserted data", "[component_store]") {
    GIVEN("a component store with data inserted for an entity") {
        ComponentStore<Position> store;
        uint32_t entity = 5;
        store.insert(entity, Position{3.0f, 7.0f});

        WHEN("the data is retrieved by entity id") {
            auto& position = store.get(entity);

            THEN("the retrieved data matches what was inserted") {
                REQUIRE(position.x == 3.0f);
                REQUIRE(position.y == 7.0f);
            }
        }
    }
}

SCENARIO("type-erased removal deletes entity from store", "[component_store]") {
    GIVEN("a component store with data for two entities") {
        ComponentStore<Position> store;
        store.insert(10, Position{1.0f, 2.0f});
        store.insert(20, Position{3.0f, 4.0f});

        WHEN("remove_component is called through a type-erased function pointer") {
            RemoveFn erased_remove = remove_component<Position>;
            erased_remove(&store, 10);

            THEN("the removed entity is gone and the remaining entity is intact") {
                REQUIRE(store.dense_.size() == 1);
                auto& pos20 = store.get(20);
                REQUIRE(pos20.x == 3.0f);
                REQUIRE(pos20.y == 4.0f);
            }
        }
    }
}

SCENARIO("component store iterates all entity-value pairs", "[component_store]") {
    GIVEN("a component store with data for three entities") {
        ComponentStore<Position> store;
        store.insert(10, Position{1.0f, 2.0f});
        store.insert(20, Position{3.0f, 4.0f});
        store.insert(30, Position{5.0f, 6.0f});

        WHEN("each is called with a callback") {
            std::unordered_map<uint32_t, Position> visited;
            store.each([&visited](uint32_t entity, const Position& pos) {
                visited[entity] = pos;
            });

            THEN("all entity-value pairs are visited") {
                REQUIRE(visited.size() == 3);
                REQUIRE(visited[10].x == 1.0f);
                REQUIRE(visited[10].y == 2.0f);
                REQUIRE(visited[20].x == 3.0f);
                REQUIRE(visited[20].y == 4.0f);
                REQUIRE(visited[30].x == 5.0f);
                REQUIRE(visited[30].y == 6.0f);
            }
        }
    }
}

SCENARIO("component store each on empty store visits nothing", "[component_store]") {
    GIVEN("an empty component store") {
        ComponentStore<Position> store;

        WHEN("each is called") {
            size_t count = 0;
            store.each([&count](uint32_t, const Position&) { ++count; });

            THEN("no entities are visited") {
                REQUIRE(count == 0);
            }
        }
    }
}

SCENARIO("component store compacts on removal", "[component_store]") {
    GIVEN("a component store with data for three entities") {
        ComponentStore<Position> store;
        store.insert(10, Position{1.0f, 2.0f});
        store.insert(20, Position{3.0f, 4.0f});
        store.insert(30, Position{5.0f, 6.0f});

        WHEN("the middle entity is removed") {
            store.remove(20);

            THEN("the dense array has no holes") {
                REQUIRE(store.dense_.size() == 2);
            }

            THEN("remaining entities return correct data") {
                auto& pos10 = store.get(10);
                REQUIRE(pos10.x == 1.0f);
                REQUIRE(pos10.y == 2.0f);

                auto& pos30 = store.get(30);
                REQUIRE(pos30.x == 5.0f);
                REQUIRE(pos30.y == 6.0f);
            }
        }
    }
}
