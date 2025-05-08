#include "test_utility.hpp"
#include <algorithm>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <enchantum/enchantum.hpp>

TEMPLATE_LIST_TEST_CASE("array size checks", "[constants]", AllEnumsTestTypes)
{
  SECTION("count<E>") { STATIC_CHECK(enchantum::count<TestType> == enchantum::entries<TestType>.size()); }
  SECTION("names<E>") { STATIC_CHECK(enchantum::names<TestType>.size() == enchantum::entries<TestType>.size()); }
  SECTION("values<E>") { STATIC_CHECK(enchantum::values<TestType>.size() == enchantum::entries<TestType>.size()); }

  SECTION("names<E> and values<E> equal to entries<E>")
  {
    for (std::size_t i = 0; i < enchantum::count<TestType>; ++i) {
      CHECK(enchantum::values<TestType>[i] == enchantum::entries<TestType>[i].first);
      CHECK(enchantum::names<TestType>[i] == enchantum::entries<TestType>[i].second);
    }
  }
}


TEST_CASE("Color enum to_string", "[to_string]")
{
  STATIC_CHECK(enchantum::to_string(Color::Green) == "Green");
  STATIC_CHECK(enchantum::to_string(Color::Red) == "Red");
  STATIC_CHECK(enchantum::to_string(Color::Blue) == "Blue");
  STATIC_CHECK(enchantum::to_string(Color::Purple) == "Purple");
  STATIC_CHECK(enchantum::to_string(Color::Aqua) == "Aqua");
}

TEST_CASE("Color enum cast", "[cast]")
{
  STATIC_CHECK(enchantum::cast<Color>("Green") == Color::Green);
  STATIC_CHECK(enchantum::cast<Color>("Red") == Color::Red);
  STATIC_CHECK(enchantum::cast<Color>("Blue") == Color::Blue);
  STATIC_CHECK(enchantum::cast<Color>("Purple") == Color::Purple);
  STATIC_CHECK(enchantum::cast<Color>("Aqua") == Color::Aqua);

  STATIC_CHECK_FALSE(enchantum::cast<Color>("ZXShady"));
  STATIC_CHECK_FALSE(enchantum::cast<Color>("red"));   // case-sensitive
  STATIC_CHECK_FALSE(enchantum::cast<Color>("GREEN")); // all caps
}

TEST_CASE("Color enum min/max", "[range][min_max]")
{
  STATIC_CHECK(enchantum::min<Color> == Color::Aqua);
  STATIC_CHECK(enchantum::max<Color> == Color::Blue);

  STATIC_CHECK(static_cast<std::underlying_type_t<Color>>(enchantum::min<Color>) == -42);
  STATIC_CHECK(static_cast<std::underlying_type_t<Color>>(enchantum::max<Color>) == 214);
}

TEST_CASE("Color enum cast from underlying type", "[cast]")
{
  using enchantum::cast;
  using T = std::underlying_type_t<Color>;
  STATIC_CHECK(enchantum::cast<Color>(T(Color::Green)) == Color::Green);
  STATIC_CHECK(enchantum::cast<Color>(T(Color::Red)) == Color::Red);
  STATIC_CHECK(enchantum::cast<Color>(T(Color::Blue)) == Color::Blue);
  STATIC_CHECK(enchantum::cast<Color>(T(Color::Purple)) == Color::Purple);
  STATIC_CHECK(enchantum::cast<Color>(T(Color::Aqua)) == Color::Aqua);
  STATIC_CHECK_FALSE(enchantum::cast<Color>(T(2138)));
}

TEST_CASE("Color enum cast from string_view", "[cast]")
{
  STATIC_CHECK(enchantum::cast<Color>("Green") == Color::Green);
  STATIC_CHECK(enchantum::cast<Color>("Red") == Color::Red);
  STATIC_CHECK(enchantum::cast<Color>("Blue") == Color::Blue);
  STATIC_CHECK(enchantum::cast<Color>("Purple") == Color::Purple);
  STATIC_CHECK(enchantum::cast<Color>("Aqua") == Color::Aqua);

  STATIC_CHECK_FALSE(enchantum::cast<Color>("Chartreuse"));
  STATIC_CHECK_FALSE(enchantum::cast<Color>("BLUE"));
}


TEST_CASE("Color enum cast with custom binary predicate (case insensitive)", "[cast]")
{
  STATIC_CHECK(enchantum::cast<Color>("green", case_insensitive) == Color::Green);
  STATIC_CHECK(enchantum::cast<Color>("RED", case_insensitive) == Color::Red);
  STATIC_CHECK(enchantum::cast<Color>("bLuE", case_insensitive) == Color::Blue);
  STATIC_CHECK(enchantum::cast<Color>("purple", case_insensitive) == Color::Purple);
  STATIC_CHECK(enchantum::cast<Color>("AQUA", case_insensitive) == Color::Aqua);
  STATIC_CHECK_FALSE(enchantum::cast<Color>("zxSHADY", case_insensitive));
}

TEST_CASE("Color enum index_to_enum", "[index_to_enum]")
{
  STATIC_CHECK(enchantum::index_to_enum<Color>(0) == Color::Aqua);
  STATIC_CHECK(enchantum::index_to_enum<Color>(1) == Color::Purple);
  STATIC_CHECK(enchantum::index_to_enum<Color>(2) == Color::Green);
  STATIC_CHECK(enchantum::index_to_enum<Color>(3) == Color::Red);
  STATIC_CHECK(enchantum::index_to_enum<Color>(4) == Color::Blue);
}
TEST_CASE("Color count", "[count]")
{
  STATIC_CHECK(enchantum::count<Color> == 5);
  STATIC_CHECK(enchantum::count<MinMaxValues> == 2);
  STATIC_CHECK(enchantum::count<Letters> == 26);
  STATIC_CHECK(enchantum::count<BoolEnum> == 2);
  STATIC_CHECK(enchantum::count<Direction2D> == 5);
  STATIC_CHECK(enchantum::count<Direction3D> == 7);
}
