#include "enchantum/include/enchantum/enchantum.hpp"
#include <iostream>

enum class TestEnum { First, Second, Third, Fourth, Fifth };

int main() {
    // Test that our implementation compiles and works
    auto result1 = enchantum::cast<TestEnum>("First");
    auto result2 = enchantum::cast<TestEnum>("Third");
    auto result3 = enchantum::cast<TestEnum>("NonExistent");
    
    std::cout << "Testing enhanced cast implementation:\n";
    std::cout << "cast<TestEnum>(\"First\"): " << (result1.has_value() ? "found" : "not found") << "\n";
    std::cout << "cast<TestEnum>(\"Third\"): " << (result2.has_value() ? "found" : "not found") << "\n";
    std::cout << "cast<TestEnum>(\"NonExistent\"): " << (result3.has_value() ? "found" : "not found") << "\n";
    
    // Test contains
    bool contains1 = enchantum::contains<TestEnum>("Second");
    bool contains2 = enchantum::contains<TestEnum>("Invalid");
    
    std::cout << "contains<TestEnum>(\"Second\"): " << (contains1 ? "true" : "false") << "\n";
    std::cout << "contains<TestEnum>(\"Invalid\"): " << (contains2 ? "true" : "false") << "\n";
    
    return 0;
}