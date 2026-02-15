#include <catch2/catch_all.hpp>
#include <cask/ecs/interpolated.hpp>
#include <cask/ecs/frame_advancer.hpp>

SCENARIO("advance_all advances all registered interpolated values", "[frame_advancer]") {
    GIVEN("a frame advancer with two registered interpolated values of different types") {
        Interpolated<float> position{0.0f, 10.0f};
        Interpolated<int> health{80, 100};
        FrameAdvancer advancer;
        advancer.add(&position, advance_interpolated<float>);
        advancer.add(&health, advance_interpolated<int>);

        WHEN("advance_all is called") {
            advancer.advance_all();

            THEN("all interpolated values have previous copied from current") {
                REQUIRE(position.previous == 10.0f);
                REQUIRE(health.previous == 100);
            }
        }
    }
}
