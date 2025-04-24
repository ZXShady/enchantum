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
    STATIC_CHECK(enchantum::ContiguousEnum<BoolEnum>);
    STATIC_CHECK(!enchantum::ContiguousEnum<Color>);
    STATIC_CHECK(!enchantum::ContiguousEnum<Flags>);
  }

  SECTION("ScopedEnum concept check")
  {
    STATIC_CHECK(!enchantum::ScopedEnum<int>);
    STATIC_CHECK(enchantum::ScopedEnum<Letters>);
    STATIC_CHECK(enchantum::ScopedEnum<Color>);
    STATIC_CHECK(enchantum::ScopedEnum<Flags>);
    STATIC_CHECK(enchantum::ScopedEnum<BoolEnum>);
    STATIC_CHECK(!enchantum::ScopedEnum<Unscoped>);
  }

  SECTION("UnscopedEnum concept check")
  {
    STATIC_CHECK(!enchantum::UnscopedEnum<int>);
    STATIC_CHECK(!enchantum::UnscopedEnum<Color>);
    STATIC_CHECK(!enchantum::UnscopedEnum<Letters>);
    STATIC_CHECK(!enchantum::UnscopedEnum<Flags>);
    STATIC_CHECK(!enchantum::UnscopedEnum<BoolEnum>);
    STATIC_CHECK(enchantum::UnscopedEnum<Unscoped>);
  }

  SECTION("SignedEnum concept check")
  {
    STATIC_CHECK(!enchantum::SignedEnum<int>);
    STATIC_CHECK(enchantum::SignedEnum<Color>);
    STATIC_CHECK(enchantum::SignedEnum<Letters>);
    STATIC_CHECK(!enchantum::SignedEnum<Flags>);
    STATIC_CHECK(!enchantum::SignedEnum<BoolEnum>);
    STATIC_CHECK(enchantum::SignedEnum<Unscoped>);
  }

  SECTION("UnsignedEnum concept check")
  {
    STATIC_CHECK(!enchantum::UnsignedEnum<int>);
    STATIC_CHECK(!enchantum::UnsignedEnum<Color>);
    STATIC_CHECK(!enchantum::UnsignedEnum<Letters>);
    STATIC_CHECK(enchantum::UnsignedEnum<Flags>);
    STATIC_CHECK(enchantum::UnsignedEnum<BoolEnum>);
    STATIC_CHECK(!enchantum::UnsignedEnum<Unscoped>);
  }


  SECTION("BitFlagEnum concept check")
  {
    STATIC_CHECK(!enchantum::BitFlagEnum<int>);
    STATIC_CHECK(!enchantum::BitFlagEnum<Letters>);
    STATIC_CHECK(!enchantum::BitFlagEnum<BoolEnum>);
    STATIC_CHECK(!enchantum::BitFlagEnum<Unscoped>);
    STATIC_CHECK(!enchantum::BitFlagEnum<Color>);
    STATIC_CHECK(enchantum::BitFlagEnum<Flags>);
  }
}
