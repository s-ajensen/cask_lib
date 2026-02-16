#include <catch2/catch_all.hpp>
#include <cask/resource/mesh_data.hpp>

SCENARIO("valid mesh data can be constructed with positions and indices", "[mesh_data]") {
    GIVEN("positions for 3 vertices and indices for 1 triangle") {
        std::vector<float> positions = {0, 0, 0, 1, 0, 0, 0, 1, 0};
        std::vector<uint32_t> indices = {0, 1, 2};

        WHEN("mesh data is constructed") {
            MeshData mesh(positions, indices);

            THEN("it does not have normals") {
                REQUIRE_FALSE(mesh.has_normals());
            }

            THEN("it does not have uvs") {
                REQUIRE_FALSE(mesh.has_uvs());
            }

            THEN("positions are accessible with correct size") {
                REQUIRE(mesh.positions().size() == 9);
            }

            THEN("indices are accessible with correct size") {
                REQUIRE(mesh.indices().size() == 3);
            }
        }
    }
}

SCENARIO("construction throws when positions are empty", "[mesh_data]") {
    GIVEN("empty positions and valid indices") {
        std::vector<float> positions = {};
        std::vector<uint32_t> indices = {0, 1, 2};

        WHEN("mesh data is constructed") {
            THEN("it throws with a message about positions") {
                REQUIRE_THROWS_WITH(
                    MeshData(positions, indices),
                    Catch::Matchers::ContainsSubstring("positions")
                );
            }
        }
    }
}

SCENARIO("construction throws when indices are empty", "[mesh_data]") {
    GIVEN("valid positions and empty indices") {
        std::vector<float> positions = {0, 0, 0, 1, 0, 0, 0, 1, 0};
        std::vector<uint32_t> indices = {};

        WHEN("mesh data is constructed") {
            THEN("it throws with a message about indices") {
                REQUIRE_THROWS_WITH(
                    MeshData(positions, indices),
                    Catch::Matchers::ContainsSubstring("indices")
                );
            }
        }
    }
}

SCENARIO("construction throws when positions are not divisible by 3", "[mesh_data]") {
    GIVEN("positions with an incomplete vertex") {
        std::vector<float> positions = {0, 0};
        std::vector<uint32_t> indices = {0};

        WHEN("mesh data is constructed") {
            THEN("it throws with a message about positions count") {
                REQUIRE_THROWS_WITH(
                    MeshData(positions, indices),
                    Catch::Matchers::ContainsSubstring("positions")
                );
            }
        }
    }
}

SCENARIO("construction throws when normals size does not match positions", "[mesh_data]") {
    GIVEN("3 vertices worth of positions but only 2 vertices worth of normals") {
        std::vector<float> positions = {0, 0, 0, 1, 0, 0, 0, 1, 0};
        std::vector<uint32_t> indices = {0, 1, 2};
        std::vector<float> normals = {0, 0, 1, 0, 0, 1};

        WHEN("mesh data is constructed") {
            THEN("it throws with a message about normals") {
                REQUIRE_THROWS_WITH(
                    MeshData(positions, indices, normals),
                    Catch::Matchers::ContainsSubstring("normals")
                );
            }
        }
    }
}

SCENARIO("valid mesh data can be constructed with normals and uvs", "[mesh_data]") {
    GIVEN("3 vertices with positions, indices, normals, and uvs") {
        std::vector<float> positions = {0, 0, 0, 1, 0, 0, 0, 1, 0};
        std::vector<uint32_t> indices = {0, 1, 2};
        std::vector<float> normals = {0, 0, 1, 0, 0, 1, 0, 0, 1};
        std::vector<float> uvs = {0, 0, 1, 0, 0, 1};

        WHEN("mesh data is constructed") {
            MeshData mesh(positions, indices, normals, uvs);

            THEN("it has normals") {
                REQUIRE(mesh.has_normals());
            }

            THEN("it has uvs") {
                REQUIRE(mesh.has_uvs());
            }

            THEN("uvs are accessible with correct size") {
                REQUIRE(mesh.uvs().size() == 6);
            }
        }
    }
}

SCENARIO("normals are accessible when provided", "[mesh_data]") {
    GIVEN("3 vertices with positions, indices, and normals") {
        std::vector<float> positions = {0, 0, 0, 1, 0, 0, 0, 1, 0};
        std::vector<uint32_t> indices = {0, 1, 2};
        std::vector<float> normals = {0, 1, 0, 0, 1, 0, 0, 1, 0};

        WHEN("mesh data is constructed") {
            MeshData mesh(positions, indices, normals);

            THEN("it has normals") {
                REQUIRE(mesh.has_normals());
            }

            THEN("normals are accessible with correct size") {
                REQUIRE(mesh.normals().size() == 9);
            }

            THEN("the first normal y component is correct") {
                REQUIRE(mesh.normals()[1] == 1.0f);
            }
        }
    }
}

SCENARIO("construction throws when uvs size does not match vertex count", "[mesh_data]") {
    GIVEN("3 vertices worth of positions but mismatched uvs") {
        std::vector<float> positions = {0, 0, 0, 1, 0, 0, 0, 1, 0};
        std::vector<uint32_t> indices = {0, 1, 2};
        std::vector<float> normals = {};
        std::vector<float> uvs = {0, 0, 1, 0};

        WHEN("mesh data is constructed") {
            THEN("it throws with a message about uvs") {
                REQUIRE_THROWS_WITH(
                    MeshData(positions, indices, normals, uvs),
                    Catch::Matchers::ContainsSubstring("uvs")
                );
            }
        }
    }
}
