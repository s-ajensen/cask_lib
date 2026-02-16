#include <catch2/catch_all.hpp>
#include <cask/resource/texture_data.hpp>

SCENARIO("valid texture data can be constructed with dimensions channels and pixels", "[texture_data]") {
    GIVEN("a 2x2 RGBA texture with 16 bytes of pixel data") {
        uint32_t width = 2;
        uint32_t height = 2;
        uint32_t channels = 4;
        std::vector<uint8_t> pixels(16, 0xFF);

        WHEN("texture data is constructed") {
            TextureData texture(width, height, channels, pixels);

            THEN("width is accessible") {
                REQUIRE(texture.width() == 2);
            }

            THEN("height is accessible") {
                REQUIRE(texture.height() == 2);
            }

            THEN("channels is accessible") {
                REQUIRE(texture.channels() == 4);
            }

            THEN("pixels are accessible with correct size") {
                REQUIRE(texture.pixels().size() == 16);
            }
        }
    }
}

SCENARIO("construction throws when width is zero", "[texture_data]") {
    GIVEN("zero width with valid height channels and matching pixels") {
        uint32_t width = 0;
        uint32_t height = 2;
        uint32_t channels = 4;
        std::vector<uint8_t> pixels(0);

        WHEN("texture data is constructed") {
            THEN("it throws with a message about width") {
                REQUIRE_THROWS_WITH(
                    TextureData(width, height, channels, pixels),
                    Catch::Matchers::ContainsSubstring("width")
                );
            }
        }
    }
}

SCENARIO("construction throws when height is zero", "[texture_data]") {
    GIVEN("zero height with valid width channels and matching pixels") {
        uint32_t width = 2;
        uint32_t height = 0;
        uint32_t channels = 4;
        std::vector<uint8_t> pixels(0);

        WHEN("texture data is constructed") {
            THEN("it throws with a message about height") {
                REQUIRE_THROWS_WITH(
                    TextureData(width, height, channels, pixels),
                    Catch::Matchers::ContainsSubstring("height")
                );
            }
        }
    }
}

SCENARIO("construction throws when channels is invalid", "[texture_data]") {
    GIVEN("an unsupported channel count of 2") {
        uint32_t width = 2;
        uint32_t height = 2;
        uint32_t channels = 2;
        std::vector<uint8_t> pixels(8, 0xFF);

        WHEN("texture data is constructed") {
            THEN("it throws with a message about channels") {
                REQUIRE_THROWS_WITH(
                    TextureData(width, height, channels, pixels),
                    Catch::Matchers::ContainsSubstring("channels")
                );
            }
        }
    }
}

SCENARIO("construction throws when pixel data size does not match dimensions", "[texture_data]") {
    GIVEN("a 2x2 RGBA texture with only 8 bytes instead of 16") {
        uint32_t width = 2;
        uint32_t height = 2;
        uint32_t channels = 4;
        std::vector<uint8_t> pixels(8, 0xFF);

        WHEN("texture data is constructed") {
            THEN("it throws with a message about pixels") {
                REQUIRE_THROWS_WITH(
                    TextureData(width, height, channels, pixels),
                    Catch::Matchers::ContainsSubstring("pixels")
                );
            }
        }
    }
}
