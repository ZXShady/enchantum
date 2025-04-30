#include "test_utility.hpp"
#include <catch2/catch_test_macros.hpp>
#include <enchantum/algorithms.hpp>
#include <enchantum/enchantum.hpp>

TEST_CASE("Color for_each", "[algorithms][for_each]")
{
  std::vector<std::string> strings;
  enchantum::for_each<Color>([&strings](auto ca) {
    if constexpr (ca != Color::Green)
      strings.emplace_back(enchantum::to_string(ca.value));
  });
  CHECK(strings.size() == enchantum::count<Color> - 1);
}
