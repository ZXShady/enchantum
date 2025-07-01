// In file: tests/format_test.cpp

#include "test_utility.hpp" // Should be included first for PCH
#include <enchantum/fmt_format.hpp>
#include <enchantum/std_format.hpp>
#include <enchantum/bitwise_operators.hpp> // Include the header for ENCHANTUM_DEFINE_BITWISE_FOR

// Step 1: Define a regular enum class for the bitflags at file scope.
enum class Permissions : unsigned int {
    None    = 0,
    Read    = 1 << 0,
    Write   = 1 << 1,
    Execute = 1 << 2
};
// Step 2: Use the correct macro to enable bitwise operators for the enum at file scope.
ENCHANTUM_DEFINE_BITWISE_FOR(Permissions);

// Test case specifically for std::format
TEST_CASE("Formatting enums with std::format", "[format][std]") {

    SECTION("Basic enum formatting") {
        enum class Color { Red, Green, Blue }; // Local enum for this section
        REQUIRE(std::format("{}", Color::Green) == "Green");
        REQUIRE(std::format("{}", Color::Red) == "Red");
    }

    SECTION("Bitflag enum formatting") {
        // Permissions enum and its operators are now globally visible in this file.
        auto p1 = Permissions::Read | Permissions::Write;
        REQUIRE(std::format("{}", p1) == "Read|Write");

        auto p2 = Permissions::Execute | Permissions::Read;
        REQUIRE(std::format("{}", p2) == "Read|Execute");

        REQUIRE(std::format("{}", Permissions::None) == "None");

        auto p_all = static_cast<Permissions>(0b111);
        REQUIRE(std::format("{}", p_all) == "Read|Write|Execute");
    }

    SECTION("Fallback for invalid values") {
        enum class Status { Ok = 0, Fail = 1 }; // Local enum for this section
        REQUIRE(std::format("{}", static_cast<Status>(123)) == "123");
    }
}

// Test case specifically for fmt::format
TEST_CASE("Formatting enums with fmt::format", "[format][fmt]") {

    SECTION("Basic enum formatting") {
        enum class Color { Red, Green, Blue }; // Local enum for this section
        REQUIRE(fmt::format(fmt::runtime("{}"), Color::Green) == "Green");
        REQUIRE(fmt::format(fmt::runtime("{}"), Color::Red) == "Red");
    }

    SECTION("Bitflag enum formatting") {
        // Permissions enum and its operators are now globally visible in this file.
        auto p1 = Permissions::Read | Permissions::Write;
        REQUIRE(fmt::format(fmt::runtime("{}"), p1) == "Read|Write");

        auto p2 = Permissions::Execute | Permissions::Read;
        REQUIRE(fmt::format(fmt::runtime("{}"), p2) == "Read|Execute");

        REQUIRE(fmt::format(fmt::runtime("{}"), Permissions::None) == "None");

        auto p_all = static_cast<Permissions>(0b111);
        REQUIRE(fmt::format(fmt::runtime("{}"), p_all) == "Read|Write|Execute");
    }

    SECTION("Fallback for invalid values") {
        enum class Status { Ok = 0, Fail = 1 }; // Local enum for this section
        REQUIRE(fmt::format(fmt::runtime("{}"), static_cast<Status>(123)) == "123");
    }
}
