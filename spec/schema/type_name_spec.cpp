#include <catch2/catch_all.hpp>
#include <cask/schema/type_name.hpp>
#include <cstdint>
#include <string>

SCENARIO("type_name maps C++ types to string names", "[type_name]") {
    GIVEN("the type_name trait") {
        THEN("float maps to float32") {
            REQUIRE(std::string(cask::type_name<float>::value) == "float32");
        }

        THEN("double maps to float64") {
            REQUIRE(std::string(cask::type_name<double>::value) == "float64");
        }

        THEN("int32_t maps to int32") {
            REQUIRE(std::string(cask::type_name<int32_t>::value) == "int32");
        }

        THEN("uint32_t maps to uint32") {
            REQUIRE(std::string(cask::type_name<uint32_t>::value) == "uint32");
        }

        THEN("int64_t maps to int64") {
            REQUIRE(std::string(cask::type_name<int64_t>::value) == "int64");
        }

        THEN("uint64_t maps to uint64") {
            REQUIRE(std::string(cask::type_name<uint64_t>::value) == "uint64");
        }

        THEN("bool maps to bool") {
            REQUIRE(std::string(cask::type_name<bool>::value) == "bool");
        }

        THEN("std::string maps to string") {
            REQUIRE(std::string(cask::type_name<std::string>::value) == "string");
        }
    }
}
