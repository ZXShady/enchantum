#include "test_utility.hpp"
#include <catch2/catch_test_macros.hpp>
#include <enchantum/algorithms.hpp>
#include <enchantum/enchantum.hpp>

TEST_CASE("UnscopedCStyle for_each", "[algorithms][for_each]")
{
  std::vector<std::string> strings;
  enchantum::for_each<UnscopedCStyle>([&strings](auto ca) {
    if constexpr (ca != Unscoped_CStyle_Val0)
      strings.emplace_back(enchantum::to_string(ca.value));
  });
  CHECK(strings.size() == enchantum::count<UnscopedCStyle> - 1);
}
