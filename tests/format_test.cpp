// In file: tests/format_test.cpp
#include "test_utility.hpp"
#include <enchantum/fmt_format.hpp>
#include <enchantum/std_format.hpp>
#include <enchantum/bitflags.hpp>
#include <string> // Required for std::string on some platforms/compilers

// Define a helper struct for std::format
struct StdFormatter {
    template<typename... Args>
    static std::string format(Args&&... args) {
        return std::format(std::forward<Args>(args)...);
    }
};

// Define a helper struct for fmt::format
struct FmtFormatter {
    template<typename... Args>
    static std::string format(Args&&... args) {
        return fmt::format(std::forward<Args>(args)...);
    }
};

// Declare bitflags enum outside of the test case to make it available globally in this file
ENCHANTUM_BITFLAGS(Permissions, None = 0, Read = 1, Write = 2, Execute = 4);

// A template for testing different formatters (StdFormatter and FmtFormatter)
TEMPLATE_TEST_CASE("Formatting enums with different formatters", "[format]", StdFormatter, FmtFormatter) {
    auto format_func = [](auto&&... args) {
        return TestType::format(std::forward<decltype(args)>(args)...);
    };

    SECTION("Basic enum formatting") {
        enum class Color { Red, Green, Blue };
        REQUIRE(format_func("{}", Color::Green) == "Green");
    }

    SECTION("Bitflag enum formatting") {
        // Permissions is now declared globally in this file
        auto p = Permissions::Read | Permissions::Write;
        REQUIRE(format_func("{}", p) == "Read|Write");
        REQUIRE(format_func("{}", Permissions::None) == "None");
        REQUIRE(format_func("{}", static_cast<Permissions>(0b111)) == "Read|Write|Execute");
    }

    SECTION("Fallback for invalid values") {
        enum class Status { Ok = 0, Fail = 1 };
        // Test fallback for a value not defined in the enum
        REQUIRE(format_func("{}", static_cast<Status>(123)) == "123");
    }
}
