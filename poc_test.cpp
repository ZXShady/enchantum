#include "enchantum/include/enchantum/enchantum.hpp"
#include <iostream>
#include <string_view>
#include <vector> // For more comprehensive test

// Test default prefix_length (0)
enum class MySimpleEnum { Foo, Bar, Baz };

// More robust test structure
struct TestCase {
    std::string_view name;
    bool (*test_func)();
};

bool testSimpleEnum() {
    bool success = true;
    auto val = MySimpleEnum::Foo;
    auto sv = enchantum::to_string(val);
    if (sv != "Foo") {
        std::cerr << "Test Failed (testSimpleEnum - Foo): Expected 'Foo', Got '" << sv << "'" << std::endl;
        success = false;
    }
    val = MySimpleEnum::Bar;
    sv = enchantum::to_string(val);
    if (sv != "Bar") {
        std::cerr << "Test Failed (testSimpleEnum - Bar): Expected 'Bar', Got '" << sv << "'" << std::endl;
        success = false;
    }
    return success;
}

// Add a test for an enum that might have a different underlying type or values
enum LongEnum : long { First = -10, Second = 0, Third = 100 };
bool testLongEnum() {
    bool success = true;
    auto val = LongEnum::First;
    auto sv = enchantum::to_string(val);
    if (sv != "First") {
        std::cerr << "Test Failed (testLongEnum - First): Expected 'First', Got '" << sv << "'" << std::endl;
        success = false;
    }
    val = LongEnum::Third;
    sv = enchantum::to_string(val);
    if (sv != "Third") {
        std::cerr << "Test Failed (testLongEnum - Third): Expected 'Third', Got '" << sv << "'" << std::endl;
        success = false;
    }
    return success;
}


int main() {
    std::vector<TestCase> tests = {
        {"SimpleEnum", testSimpleEnum},
        {"LongEnum", testLongEnum}
    };

    int failed_count = 0;
    for (const auto& test : tests) {
        std::cout << "Running test: " << test.name << std::endl;
        if (!test.test_func()) {
            failed_count++;
        }
    }

    if (failed_count == 0) {
        std::cout << "All PoC tests passed!" << std::endl;
        return 0;
    } else {
        std::cerr << failed_count << " PoC test(s) failed." << std::endl;
        return 1;
    }
}
