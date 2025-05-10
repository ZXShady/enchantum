#include "test_utility.hpp"
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <enchantum/next_value.hpp>


TEMPLATE_LIST_TEST_CASE("next_value identities", "[next_value]", AllEnumsTestTypes)
{
  constexpr auto count = enchantum::count<TestType>;
  for (const auto v : enchantum::values<TestType>) {
    CHECK(v == enchantum::next_value_circular(v, count));
    CHECK(v == enchantum::prev_value_circular(v, count));
    CHECK_FALSE(enchantum::next_value(v, count).has_value());
    CHECK_FALSE(enchantum::prev_value(v, count).has_value());
  }
}

TEST_CASE("Color next_value", "[next_value]")
{
  STATIC_REQUIRE(enchantum::next_value(Color::Aqua) == Color::Purple);
  STATIC_REQUIRE(enchantum::next_value(Color::Purple) == Color::Green);
  STATIC_REQUIRE(enchantum::next_value(Color::Green) == Color::Red);
  STATIC_REQUIRE(enchantum::next_value(Color::Red) == Color::Blue);
  STATIC_REQUIRE_FALSE(enchantum::next_value(Color::Blue).has_value());
}

TEST_CASE("Color prev_value", "[prev_value]")
{
  STATIC_REQUIRE(enchantum::prev_value(Color::Blue) == Color::Red);
  STATIC_REQUIRE(enchantum::prev_value(Color::Red) == Color::Green);
  STATIC_REQUIRE(enchantum::prev_value(Color::Green) == Color::Purple);
  STATIC_REQUIRE(enchantum::prev_value(Color::Purple) == Color::Aqua);
  STATIC_REQUIRE_FALSE(enchantum::prev_value(Color::Aqua).has_value());
}

TEST_CASE("Flags next_value", "[next_value]")
{
  STATIC_REQUIRE(enchantum::next_value(Flags::Flag0) == Flags::Flag1);
  STATIC_REQUIRE(enchantum::next_value(Flags::Flag1) == Flags::Flag2);
  STATIC_REQUIRE(enchantum::next_value(Flags::Flag2) == Flags::Flag3);
  STATIC_REQUIRE(enchantum::next_value(Flags::Flag3) == Flags::Flag4);
  STATIC_REQUIRE(enchantum::next_value(Flags::Flag4) == Flags::Flag5);
  STATIC_REQUIRE(enchantum::next_value(Flags::Flag5) == Flags::Flag6);
  STATIC_REQUIRE_FALSE(enchantum::next_value(Flags::Flag6).has_value());
}

TEST_CASE("Flags prev_value", "[prev_value]")
{
  STATIC_REQUIRE(enchantum::prev_value(Flags::Flag6) == Flags::Flag5);
  STATIC_REQUIRE(enchantum::prev_value(Flags::Flag1) == Flags::Flag0);
  STATIC_REQUIRE_FALSE(enchantum::prev_value(Flags::Flag0).has_value());
}

TEST_CASE("UnscopedColor next_value", "[next_value]")
{
  STATIC_REQUIRE(enchantum::next_value(UnscopedColor::Aqua) == UnscopedColor::Purple);
  STATIC_REQUIRE(enchantum::next_value(UnscopedColor::Purple) == UnscopedColor::Green);
  STATIC_REQUIRE(enchantum::next_value(UnscopedColor::Red) == UnscopedColor::Blue);
  STATIC_REQUIRE_FALSE(enchantum::next_value(UnscopedColor::Blue).has_value());
}

TEST_CASE("UnscopedColor prev_value", "[prev_value]")
{
  STATIC_REQUIRE(enchantum::prev_value(UnscopedColor::Blue) == UnscopedColor::Red);
  STATIC_REQUIRE(enchantum::prev_value(UnscopedColor::Green) == UnscopedColor::Purple);
  STATIC_REQUIRE_FALSE(enchantum::prev_value(UnscopedColor::Aqua).has_value());
}

TEST_CASE("Color next_value_circular", "[next_value_circular]")
{
  STATIC_REQUIRE(enchantum::next_value_circular(Color::Aqua) == Color::Purple);
  STATIC_REQUIRE(enchantum::next_value_circular(Color::Purple) == Color::Green);
  STATIC_REQUIRE(enchantum::next_value_circular(Color::Green) == Color::Red);
  STATIC_REQUIRE(enchantum::next_value_circular(Color::Red) == Color::Blue);
  STATIC_REQUIRE(enchantum::next_value_circular(Color::Blue) == Color::Aqua);
}

TEST_CASE("Flags next_value_circular", "[next_value_circular]")
{
  STATIC_REQUIRE(enchantum::next_value_circular(Flags::Flag0) == Flags::Flag1);
  STATIC_REQUIRE(enchantum::next_value_circular(Flags::Flag1) == Flags::Flag2);
  STATIC_REQUIRE(enchantum::next_value_circular(Flags::Flag2) == Flags::Flag3);
  STATIC_REQUIRE(enchantum::next_value_circular(Flags::Flag3) == Flags::Flag4);
  STATIC_REQUIRE(enchantum::next_value_circular(Flags::Flag4) == Flags::Flag5);
  STATIC_REQUIRE(enchantum::next_value_circular(Flags::Flag5) == Flags::Flag6);
  STATIC_REQUIRE(enchantum::next_value_circular(Flags::Flag6) == Flags::Flag0);
}

TEST_CASE("UnscopedColor next_value_circular", "[next_value_circular]")
{
  STATIC_REQUIRE(enchantum::next_value_circular(UnscopedColor::Aqua) == UnscopedColor::Purple);
  STATIC_REQUIRE(enchantum::next_value_circular(UnscopedColor::Purple) == UnscopedColor::Green);
  STATIC_REQUIRE(enchantum::next_value_circular(UnscopedColor::Green) == UnscopedColor::Red);
  STATIC_REQUIRE(enchantum::next_value_circular(UnscopedColor::Red) == UnscopedColor::Blue);
  STATIC_REQUIRE(enchantum::next_value_circular(UnscopedColor::Blue) == UnscopedColor::Aqua);
}

TEST_CASE("Color prev_value_circular", "[prev_value_circular]")
{
  STATIC_REQUIRE(enchantum::prev_value_circular(Color::Blue) == Color::Red);
  STATIC_REQUIRE(enchantum::prev_value_circular(Color::Red) == Color::Green);
  STATIC_REQUIRE(enchantum::prev_value_circular(Color::Green) == Color::Purple);
  STATIC_REQUIRE(enchantum::prev_value_circular(Color::Purple) == Color::Aqua);
  STATIC_REQUIRE(enchantum::prev_value_circular(Color::Aqua) == Color::Blue);
}

TEST_CASE("Flags prev_value_circular", "[prev_value_circular]")
{
  STATIC_REQUIRE(enchantum::prev_value_circular(Flags::Flag6) == Flags::Flag5);
  STATIC_REQUIRE(enchantum::prev_value_circular(Flags::Flag5) == Flags::Flag4);
  STATIC_REQUIRE(enchantum::prev_value_circular(Flags::Flag4) == Flags::Flag3);
  STATIC_REQUIRE(enchantum::prev_value_circular(Flags::Flag3) == Flags::Flag2);
  STATIC_REQUIRE(enchantum::prev_value_circular(Flags::Flag2) == Flags::Flag1);
  STATIC_REQUIRE(enchantum::prev_value_circular(Flags::Flag1) == Flags::Flag0);
  STATIC_REQUIRE(enchantum::prev_value_circular(Flags::Flag0) == Flags::Flag6);
}

TEST_CASE("UnscopedColor prev_value_circular", "[prev_value_circular]")
{
  STATIC_REQUIRE(enchantum::prev_value_circular(UnscopedColor::Blue) == UnscopedColor::Red);
  STATIC_REQUIRE(enchantum::prev_value_circular(UnscopedColor::Red) == UnscopedColor::Green);
  STATIC_REQUIRE(enchantum::prev_value_circular(UnscopedColor::Green) == UnscopedColor::Purple);
  STATIC_REQUIRE(enchantum::prev_value_circular(UnscopedColor::Purple) == UnscopedColor::Aqua);
  STATIC_REQUIRE(enchantum::prev_value_circular(UnscopedColor::Aqua) == UnscopedColor::Blue);
}
