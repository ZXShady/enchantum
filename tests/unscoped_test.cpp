#include "test_utility.hpp"
#include <catch2/catch_test_macros.hpp>
#include <enchantum/enchantum.hpp>

TEST_CASE("Unscoped C enum")
{
  STATIC_CHECK(enchantum::count<UnscopedCStyle> == 5);
  STATIC_CHECK(enchantum::entries<UnscopedCStyle>.size() == 5);
}