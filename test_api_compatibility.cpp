#include "enchantum/include/enchantum/enchantum.hpp"
#include <iostream>
#include <cassert>

// Test enum with various patterns
enum class TestEnum { 
    Alpha, Beta, Gamma, Delta, Epsilon, Zeta, Eta, Theta, Iota, Kappa, Lambda, Mu,
    Apple, Banana, Cherry, Date
};

// Small enum for testing tiny path
enum class SmallEnum { A, B, C };

// Large enum for testing LEN_FIRST path  
enum class LargeEnum {
    Entry01, Entry02, Entry03, Entry04, Entry05, Entry06, Entry07, Entry08,
    Entry09, Entry10, Entry11, Entry12, Entry13, Entry14, Entry15, Entry16,
    Entry17, Entry18, Entry19, Entry20
};

void test_basic_functionality() {
    std::cout << "Testing basic cast functionality..." << std::endl;
    
    // Test positive cases
    auto alpha = enchantum::cast<TestEnum>("Alpha");
    auto beta = enchantum::cast<TestEnum>("Beta");
    auto apple = enchantum::cast<TestEnum>("Apple");
    
    assert(alpha.has_value());
    assert(beta.has_value());
    assert(apple.has_value());
    
    assert(*alpha == TestEnum::Alpha);
    assert(*beta == TestEnum::Beta);
    assert(*apple == TestEnum::Apple);
    
    // Test negative cases
    auto invalid1 = enchantum::cast<TestEnum>("InvalidName");
    auto invalid2 = enchantum::cast<TestEnum>("alpha"); // wrong case
    auto invalid3 = enchantum::cast<TestEnum>("VeryLongInvalidName");
    
    assert(!invalid1.has_value());
    assert(!invalid2.has_value());
    assert(!invalid3.has_value());
    
    std::cout << "  ✓ Basic cast tests passed" << std::endl;
}

void test_contains_functionality() {
    std::cout << "Testing contains functionality..." << std::endl;
    
    // Test positive cases
    assert(enchantum::contains<TestEnum>("Alpha"));
    assert(enchantum::contains<TestEnum>("Beta"));
    assert(enchantum::contains<TestEnum>("Apple"));
    assert(enchantum::contains<TestEnum>("Mu"));
    
    // Test negative cases
    assert(!enchantum::contains<TestEnum>("InvalidName"));
    assert(!enchantum::contains<TestEnum>("alpha"));
    assert(!enchantum::contains<TestEnum>("VeryLongInvalidName"));
    
    std::cout << "  ✓ Contains tests passed" << std::endl;
}

void test_tiny_enum() {
    std::cout << "Testing tiny enum (≤3 entries)..." << std::endl;
    
    auto a = enchantum::cast<SmallEnum>("A");
    auto b = enchantum::cast<SmallEnum>("B");
    auto c = enchantum::cast<SmallEnum>("C");
    auto invalid = enchantum::cast<SmallEnum>("D");
    
    assert(a.has_value() && *a == SmallEnum::A);
    assert(b.has_value() && *b == SmallEnum::B);
    assert(c.has_value() && *c == SmallEnum::C);
    assert(!invalid.has_value());
    
    assert(enchantum::contains<SmallEnum>("A"));
    assert(enchantum::contains<SmallEnum>("B"));
    assert(enchantum::contains<SmallEnum>("C"));
    assert(!enchantum::contains<SmallEnum>("D"));
    
    std::cout << "  ✓ Tiny enum tests passed" << std::endl;
}

void test_large_enum() {
    std::cout << "Testing large enum (>12 entries)..." << std::endl;
    
    auto entry01 = enchantum::cast<LargeEnum>("Entry01");
    auto entry10 = enchantum::cast<LargeEnum>("Entry10");
    auto entry20 = enchantum::cast<LargeEnum>("Entry20");
    auto invalid = enchantum::cast<LargeEnum>("Entry21");
    
    assert(entry01.has_value() && *entry01 == LargeEnum::Entry01);
    assert(entry10.has_value() && *entry10 == LargeEnum::Entry10);
    assert(entry20.has_value() && *entry20 == LargeEnum::Entry20);
    assert(!invalid.has_value());
    
    assert(enchantum::contains<LargeEnum>("Entry01"));
    assert(enchantum::contains<LargeEnum>("Entry10"));
    assert(enchantum::contains<LargeEnum>("Entry20"));
    assert(!enchantum::contains<LargeEnum>("Entry21"));
    
    std::cout << "  ✓ Large enum tests passed" << std::endl;
}

void test_edge_cases() {
    std::cout << "Testing edge cases..." << std::endl;
    
    // Empty string
    auto empty = enchantum::cast<TestEnum>("");
    assert(!empty.has_value());
    assert(!enchantum::contains<TestEnum>(""));
    
    // Very long string
    auto long_str = enchantum::cast<TestEnum>(std::string(300, 'x'));
    assert(!long_str.has_value());
    assert(!enchantum::contains<TestEnum>(std::string(300, 'x')));
    
    // String with same length but different content
    auto same_len = enchantum::cast<TestEnum>("Betaa"); // "Beta" + "a"
    assert(!same_len.has_value());
    assert(!enchantum::contains<TestEnum>("Betaa"));
    
    std::cout << "  ✓ Edge case tests passed" << std::endl;
}

void test_to_string_compatibility() {
    std::cout << "Testing to_string compatibility..." << std::endl;
    
    // Test that to_string still works
    auto alpha_str = enchantum::to_string(TestEnum::Alpha);
    auto beta_str = enchantum::to_string(TestEnum::Beta);
    
    assert(alpha_str == "Alpha");
    assert(beta_str == "Beta");
    
    // Test round-trip conversion
    auto alpha_back = enchantum::cast<TestEnum>(alpha_str);
    assert(alpha_back.has_value() && *alpha_back == TestEnum::Alpha);
    
    std::cout << "  ✓ to_string compatibility tests passed" << std::endl;
}

int main() {
    std::cout << "Running comprehensive API compatibility tests..." << std::endl;
    std::cout << "=============================================" << std::endl;
    
    test_basic_functionality();
    test_contains_functionality();
    test_tiny_enum();
    test_large_enum();
    test_edge_cases();
    test_to_string_compatibility();
    
    std::cout << "=============================================" << std::endl;
    std::cout << "✅ All tests passed! API compatibility verified." << std::endl;
    
    return 0;
}