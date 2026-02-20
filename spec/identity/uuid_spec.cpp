#include <catch2/catch_all.hpp>
#include <cask/identity/uuid.hpp>
#include <unordered_map>
#include <unordered_set>

SCENARIO("generated UUIDs are unique", "[uuid]") {
    GIVEN("a batch of generated UUIDs") {
        constexpr int BATCH_SIZE = 100;
        std::vector<cask::UUID> uuids;
        for (int index = 0; index < BATCH_SIZE; index++) {
            uuids.push_back(cask::generate_uuid());
        }

        THEN("no two UUIDs are equal") {
            std::unordered_set<cask::UUID> unique_uuids(uuids.begin(), uuids.end());
            REQUIRE(unique_uuids.size() == uuids.size());
        }
    }
}

SCENARIO("a UUID equals itself", "[uuid]") {
    GIVEN("a generated UUID") {
        auto uuid = cask::generate_uuid();

        THEN("it is equal to itself") {
            REQUIRE(uuid == uuid);
        }
    }
}

SCENARIO("a generated UUID is not nil", "[uuid]") {
    GIVEN("a generated UUID") {
        auto uuid = cask::generate_uuid();

        THEN("it is not nil") {
            REQUIRE_FALSE(uuid.is_nil());
        }
    }
}

SCENARIO("a UUID can be used as an unordered_map key", "[uuid]") {
    GIVEN("two generated UUIDs") {
        auto first = cask::generate_uuid();
        auto second = cask::generate_uuid();

        WHEN("they are used as keys in an unordered_map") {
            std::unordered_map<cask::UUID, std::string> map;
            map[first] = "alpha";
            map[second] = "beta";

            THEN("values can be retrieved by UUID key") {
                REQUIRE(map.at(first) == "alpha");
                REQUIRE(map.at(second) == "beta");
            }
        }
    }
}
