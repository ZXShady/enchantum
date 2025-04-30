#include "test_utility.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Unscoped C enum")
{
  CHECK(enchantum::enum_traits<UnscopedCStyle>::min != 0);
  CHECK(enchantum::enum_traits<UnscopedCStyle>::max != 0);
}