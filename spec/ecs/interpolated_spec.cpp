#include <catch2/catch_all.hpp>
#include <cask/ecs/interpolated.hpp>

SCENARIO("advance copies current to previous", "[interpolated]") {
    GIVEN("an interpolated float with previous 0 and current 10") {
        Interpolated<float> value{0.0f, 10.0f};

        WHEN("advance is called") {
            value.advance();

            THEN("previous becomes current and current is unchanged") {
                REQUIRE(value.previous == 10.0f);
                REQUIRE(value.current == 10.0f);
            }
        }
    }
}

SCENARIO("current can be modified after advance without affecting previous", "[interpolated]") {
    GIVEN("an interpolated float with previous 0 and current 10") {
        Interpolated<float> value{0.0f, 10.0f};

        WHEN("advance is called and current is modified") {
            value.advance();
            value.current = 20.0f;

            THEN("previous retains the old current and current has the new value") {
                REQUIRE(value.previous == 10.0f);
                REQUIRE(value.current == 20.0f);
            }
        }
    }
}
