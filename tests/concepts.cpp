#include "test_utility.hpp"
#include <catch2/catch_test_macros.hpp>
#include <enchantum/enchantum.hpp>

TEST_CASE("Concept checks", "[concepts]")
{
  SECTION("Enum concept check")
  {
    STATIC_CHECK(!enchantum::Enum<int>);
    STATIC_CHECK(enchantum::Enum<Letters>);
    STATIC_CHECK(enchantum::Enum<Color>);
    STATIC_CHECK(enchantum::Enum<Flags>);
  }

  SECTION("ContiguousEnum concept check")
  {
    STATIC_CHECK(!enchantum::ContiguousEnum<int>);
    STATIC_CHECK(enchantum::ContiguousEnum<Letters>);
    STATIC_CHECK(!enchantum::ContiguousEnum<Color>);
    STATIC_CHECK(!enchantum::ContiguousEnum<Flags>);
  }

  SECTION("ScopedEnum concept check")
  {
    STATIC_CHECK(enchantum::ScopedEnum<Letters>);
    STATIC_CHECK(enchantum::ScopedEnum<Color>);
    STATIC_CHECK(enchantum::ScopedEnum<Flags>);
    STATIC_CHECK(!enchantum::ScopedEnum<Unscoped>);
    STATIC_CHECK(!enchantum::ScopedEnum<int>);
  }

  SECTION("UnscopedEnum concept check")
  {
    STATIC_CHECK(!enchantum::UnscopedEnum<Color>);
    STATIC_CHECK(!enchantum::UnscopedEnum<Letters>);
    STATIC_CHECK(!enchantum::UnscopedEnum<Flags>);
    STATIC_CHECK(enchantum::UnscopedEnum<Unscoped>);
    STATIC_CHECK(!enchantum::UnscopedEnum<int>);
  }

  SECTION("BitFlagEnum concept check")
  {
    STATIC_CHECK(!enchantum::BitFlagEnum<int>);
    STATIC_CHECK(!enchantum::BitFlagEnum<Letters>);
    STATIC_CHECK(!enchantum::BitFlagEnum<Unscoped>);
    STATIC_CHECK(!enchantum::BitFlagEnum<Color>);
    STATIC_CHECK(enchantum::BitFlagEnum<Flags>);
  }
}
