#include "test_utility.hpp"
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <enchantum/bitset.hpp>
#include <algorithm>

TEMPLATE_LIST_TEST_CASE("bitset identities", "[bitset]", AllEnumsTestTypes)
{
  enchantum::bitset<TestType> bitset;

  SECTION("set reset test")
  {
    for (const auto value : enchantum::values<TestType>) {
      bitset.set(value);
      REQUIRE(bitset.test(value));
      bitset.reset(value);
      REQUIRE_FALSE(bitset.test(value));
    }
  }

  SECTION("count")
  {
    for (const auto value : enchantum::values<TestType>)
      bitset.set(value);
    REQUIRE(bitset.count() == bitset.size());
  }

}

TEST_CASE("bitset: default construction", "[bitset]")
{
  constexpr enchantum::bitset<Color> colors;

  REQUIRE(colors.none());
  REQUIRE_FALSE(colors.any());
  REQUIRE(colors.count() == 0);
}

TEST_CASE("bitset: initializer_list construction", "[bitset]")
{
  enchantum::bitset<Color> colors{Color::Aqua, Color::Green};

  REQUIRE(colors.test(Color::Aqua));
  REQUIRE(colors.test(Color::Green));
  REQUIRE_FALSE(colors.test(Color::Purple));
  REQUIRE(colors.count() == 2);
}

TEST_CASE("bitset: set and reset", "[bitset]")
{
  enchantum::bitset<Color> colors;

  colors.set(Color::Red);
  REQUIRE(colors.test(Color::Red));

  colors.reset(Color::Red);
  REQUIRE_FALSE(colors.test(Color::Red));
}

TEST_CASE("bitset: flip", "[bitset]")
{
  enchantum::bitset<Color> colors;

  colors.flip(Color::Blue);
  REQUIRE(colors.test(Color::Blue));

  colors.flip(Color::Blue);
  REQUIRE_FALSE(colors.test(Color::Blue));
}

TEST_CASE("bitset: to_string named output", "[bitset]")
{
  enchantum::bitset<Color> colors{Color::Aqua, Color::Red};

  std::string result = colors.to_string();
  REQUIRE(colors.to_string('.') == "Aqua.Red");
  REQUIRE(colors.to_string() == "Aqua|Red");

  REQUIRE(colors.to_string('0', '1') == std::bitset<enchantum::count<Color>>(colors.to_ulong()).to_string());
}

TEST_CASE("bitset: to_string binary", "[bitset]")
{
  enchantum::bitset<Color> colors{Color::Red};

  std::string binary = colors.to_string('0', '1');
  REQUIRE(std::count(binary.begin(), binary.end(), '1') == 1);
}

TEST_CASE("bitset: operator[] and reference access", "[bitset]")
{
  enchantum::bitset<Color> colors;

  colors[Color::Purple] = true;
  REQUIRE(colors.test(Color::Purple));

  colors[Color::Purple] = false;
  REQUIRE_FALSE(colors.test(Color::Purple));
}
