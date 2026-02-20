#include <catch2/catch_all.hpp>
#include <cask/identity/entity_registry.hpp>

SCENARIO("resolve with a new UUID creates an entity and records the mapping", "[entity_registry]") {
    GIVEN("an empty registry and entity table") {
        EntityRegistry registry;
        EntityTable table;

        WHEN("a new UUID is resolved") {
            auto uuid = cask::generate_uuid();
            auto entity = registry.resolve(uuid, table);

            THEN("the entity is alive in the table") {
                REQUIRE(table.alive(entity));
            }

            THEN("the registry tracks one mapping") {
                REQUIRE(registry.size() == 1);
            }

            THEN("the entity can be identified by its UUID") {
                REQUIRE(registry.identify(entity) == uuid);
            }
        }
    }
}

SCENARIO("resolve with the same UUID twice returns the same entity ID", "[entity_registry]") {
    GIVEN("a registry with one resolved UUID") {
        EntityRegistry registry;
        EntityTable table;
        auto uuid = cask::generate_uuid();
        auto first_entity = registry.resolve(uuid, table);

        WHEN("the same UUID is resolved again") {
            auto second_entity = registry.resolve(uuid, table);

            THEN("the same entity ID is returned") {
                REQUIRE(second_entity == first_entity);
            }

            THEN("no additional entity is created") {
                REQUIRE(registry.size() == 1);
            }
        }
    }
}

SCENARIO("identify returns the UUID assigned to an entity", "[entity_registry]") {
    GIVEN("a registry with a resolved UUID") {
        EntityRegistry registry;
        EntityTable table;
        auto uuid = cask::generate_uuid();
        auto entity = registry.resolve(uuid, table);

        WHEN("identify is called with the entity") {
            auto result = registry.identify(entity);

            THEN("the original UUID is returned") {
                REQUIRE(result == uuid);
            }
        }
    }
}

SCENARIO("identify throws for an entity with no UUID", "[entity_registry]") {
    GIVEN("a registry with no mappings") {
        EntityRegistry registry;
        EntityTable table;
        auto entity = table.create();

        WHEN("identify is called with an unmapped entity") {
            THEN("an exception is thrown") {
                REQUIRE_THROWS(registry.identify(entity));
            }
        }
    }
}

SCENARIO("has returns true for entities with UUIDs", "[entity_registry]") {
    GIVEN("a registry with a resolved UUID") {
        EntityRegistry registry;
        EntityTable table;
        auto uuid = cask::generate_uuid();
        auto entity = registry.resolve(uuid, table);

        WHEN("has is called with the mapped entity") {
            auto result = registry.has(entity);

            THEN("it returns true") {
                REQUIRE(result == true);
            }
        }
    }
}

SCENARIO("has returns false for entities without UUIDs", "[entity_registry]") {
    GIVEN("a registry with no mappings") {
        EntityRegistry registry;
        EntityTable table;
        auto entity = table.create();

        WHEN("has is called with an unmapped entity") {
            auto result = registry.has(entity);

            THEN("it returns false") {
                REQUIRE(result == false);
            }
        }
    }
}

SCENARIO("assign links an existing entity to a UUID", "[entity_registry]") {
    GIVEN("an entity table with an existing entity") {
        EntityRegistry registry;
        EntityTable table;
        auto entity = table.create();
        auto uuid = cask::generate_uuid();

        WHEN("the entity is assigned a UUID") {
            registry.assign(entity, uuid);

            THEN("identify returns the assigned UUID") {
                REQUIRE(registry.identify(entity) == uuid);
            }

            THEN("has returns true") {
                REQUIRE(registry.has(entity) == true);
            }

            THEN("the registry tracks one mapping") {
                REQUIRE(registry.size() == 1);
            }
        }
    }
}

SCENARIO("assign throws if the UUID is already mapped to a different entity", "[entity_registry]") {
    GIVEN("a registry with a UUID already mapped to an entity") {
        EntityRegistry registry;
        EntityTable table;
        auto uuid = cask::generate_uuid();
        auto first_entity = registry.resolve(uuid, table);
        auto second_entity = table.create();

        WHEN("assign is called with the same UUID for a different entity") {
            THEN("an exception is thrown") {
                REQUIRE_THROWS(registry.assign(second_entity, uuid));
            }
        }
    }
}

SCENARIO("assign throws if the entity already has a different UUID", "[entity_registry]") {
    GIVEN("a registry with an entity already mapped to a UUID") {
        EntityRegistry registry;
        EntityTable table;
        auto first_uuid = cask::generate_uuid();
        auto entity = registry.resolve(first_uuid, table);
        auto second_uuid = cask::generate_uuid();

        WHEN("assign is called with a different UUID for the same entity") {
            THEN("an exception is thrown") {
                REQUIRE_THROWS(registry.assign(entity, second_uuid));
            }
        }
    }
}

SCENARIO("remove clears both sides of the mapping", "[entity_registry]") {
    GIVEN("a registry with a resolved UUID") {
        EntityRegistry registry;
        EntityTable table;
        auto uuid = cask::generate_uuid();
        auto entity = registry.resolve(uuid, table);

        WHEN("the entity is removed") {
            registry.remove(entity);

            THEN("has returns false") {
                REQUIRE(registry.has(entity) == false);
            }

            THEN("the registry is empty") {
                REQUIRE(registry.size() == 0);
            }

            THEN("identify throws for the removed entity") {
                REQUIRE_THROWS(registry.identify(entity));
            }
        }
    }
}

SCENARIO("remove is a no-op for entities without UUIDs", "[entity_registry]") {
    GIVEN("a registry with no mappings") {
        EntityRegistry registry;
        EntityTable table;
        auto entity = table.create();

        WHEN("remove is called on an unmapped entity") {
            registry.remove(entity);

            THEN("the registry remains empty") {
                REQUIRE(registry.size() == 0);
            }
        }
    }
}

SCENARIO("after remove the UUID can be reused with a new entity", "[entity_registry]") {
    GIVEN("a registry where a UUID was mapped then removed") {
        EntityRegistry registry;
        EntityTable table;
        auto uuid = cask::generate_uuid();
        auto old_entity = registry.resolve(uuid, table);
        registry.remove(old_entity);

        WHEN("the same UUID is resolved again") {
            auto new_entity = registry.resolve(uuid, table);

            THEN("has returns false for the old entity") {
                REQUIRE(registry.has(old_entity) == false);
            }

            THEN("the new entity is alive") {
                REQUIRE(table.alive(new_entity));
            }

            THEN("the registry tracks one mapping") {
                REQUIRE(registry.size() == 1);
            }
        }
    }
}

SCENARIO("resolve after remove creates a fresh entity", "[entity_registry]") {
    GIVEN("a registry where a UUID was mapped then removed") {
        EntityRegistry registry;
        EntityTable table;
        auto uuid = cask::generate_uuid();
        auto old_entity = registry.resolve(uuid, table);
        registry.remove(old_entity);

        WHEN("the same UUID is resolved again") {
            auto new_entity = registry.resolve(uuid, table);

            THEN("the new entity is different from the old one") {
                REQUIRE(new_entity != old_entity);
            }
        }
    }
}
