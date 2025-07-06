#include "test_utility.hpp"
#include <algorithm>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <enchantum/enchantum.hpp>
#include <enchantum/generators.hpp>

TEMPLATE_LIST_TEST_CASE("array size checks", "[constants]", AllEnumsTestTypes)
{
  constexpr auto count = enchantum::count<TestType>;
  //constexpr auto  min     = enchantum::min<TestType>;
  //constexpr auto  max     = enchantum::max<TestType>;
  constexpr auto& names   = enchantum::names<TestType>;
  constexpr auto& values  = enchantum::values<TestType>;
  constexpr auto& entries = enchantum::entries<TestType>;


  STATIC_CHECK(count == entries.size());
  STATIC_CHECK(names.size() == entries.size());
  STATIC_CHECK(values.size() == entries.size());

  SECTION("names<E> and values<E> equal to entries<E>")
  {
    for (std::size_t i = 0; i < count; ++i) {
      CHECK(values[i] == entries[i].first);
      CHECK(names[i] == entries[i].second);
    }
  }

  SECTION("entries_generator<E> and entries<E> are equal")
  {
    for (std::size_t i = 0; i < count; ++i) {
      CHECK(enchantum::entries_generator<TestType>[i] == entries[i]);
    }
  }

  SECTION("names_generator<E> and names<E> are equal")
  {
    for (std::size_t i = 0; i < count; ++i) {
      CHECK(enchantum::names_generator<TestType>[i] == names[i]);
    }
  }

  SECTION("values_generator<E> and values<E> are equal")
  {
    for (std::size_t i = 0; i < count; ++i) {
      CHECK(enchantum::values_generator<TestType>[i] == values[i]);
    }
  }

  SECTION("enum_to_index identities")
  {
    for (std::size_t i = 0; i < count; ++i) {
      CHECK(i == enchantum::enum_to_index(values[i]));
    }
  }

  SECTION("index_to_enum identities")
  {
    for (std::size_t i = 0; i < count; ++i) {
      CHECK(enchantum::index_to_enum<TestType>(i) == values[i]);
      CHECK_FALSE(enchantum::index_to_enum<TestType>(i + count).has_value());
    }
  }

  SECTION("cast(to_string()) identities")
  {
    for (std::size_t i = 0; i < count; ++i) {
      CHECK(values[i] == enchantum::cast<TestType>(enchantum::to_string(values[i])));
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

TEST_CASE("NonContigFlagsWithNoneCStyle contains", "[contains]")
{
  STATIC_CHECK_FALSE(enchantum::contains(NonContigFlagsWithNoneCStyle(1 << 3)));
  STATIC_CHECK_FALSE(enchantum::contains(NonContigFlagsWithNoneCStyle(1 << 4)));
  STATIC_CHECK_FALSE(enchantum::contains(NonContigFlagsWithNoneCStyle(1 << 5)));
  STATIC_CHECK_FALSE(enchantum::contains(NonContigFlagsWithNoneCStyle(1 << 7)));
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
  STATIC_CHECK(enchantum::count<UnderlyingTypeWChar_t> == 10);
  STATIC_CHECK(enchantum::count<UnderlyingTypeChar16_t> == 10);
  STATIC_CHECK(enchantum::count<UnderlyingTypeChar32_t> == 10);

  STATIC_CHECK(enchantum::count<Color> == 5);
  STATIC_CHECK(enchantum::count<MinMaxValues> == 2);
  STATIC_CHECK(enchantum::count<Letters> == 26);
  STATIC_CHECK(enchantum::count<BoolEnum> == 2);
  STATIC_CHECK(enchantum::count<Direction2D> == 5);
  STATIC_CHECK(enchantum::count<Direction3D> == 7);
}

TEST_CASE("enchantum::cast (string_view) specific tests", "[cast][string_view][hash_optimization]")
{
  // Test with Direction2D
  STATIC_CHECK(enchantum::cast<Direction2D>("Up") == Direction2D::Up);
  STATIC_CHECK(enchantum::cast<Direction2D>("Down") == Direction2D::Down);
  STATIC_CHECK(enchantum::cast<Direction2D>("Left") == Direction2D::Left);
  STATIC_CHECK(enchantum::cast<Direction2D>("Right") == Direction2D::Right);
  STATIC_CHECK(enchantum::cast<Direction2D>("None") == Direction2D::None);

  // Edge cases for string inputs
  STATIC_CHECK_FALSE(enchantum::cast<Direction2D>("").has_value());         // Empty string
  STATIC_CHECK_FALSE(enchantum::cast<Direction2D>("up").has_value());       // Incorrect case
  STATIC_CHECK_FALSE(enchantum::cast<Direction2D>("UP").has_value());       // Incorrect case
  STATIC_CHECK_FALSE(enchantum::cast<Direction2D>("U").has_value());        // Prefix
  STATIC_CHECK_FALSE(enchantum::cast<Direction2D>("Upp").has_value());      // Suffix/Typo
  STATIC_CHECK_FALSE(enchantum::cast<Direction2D>(" Up").has_value());      // Leading space
  STATIC_CHECK_FALSE(enchantum::cast<Direction2D>("Up ").has_value());      // Trailing space
  STATIC_CHECK_FALSE(enchantum::cast<Direction2D>("NotValid").has_value()); // Completely invalid

  // Test with Color enum for variety
  STATIC_CHECK(enchantum::cast<Color>("Green") == Color::Green);
  STATIC_CHECK_FALSE(enchantum::cast<Color>("green").has_value());
  STATIC_CHECK_FALSE(enchantum::cast<Color>("").has_value()); // Empty string for Color
}
