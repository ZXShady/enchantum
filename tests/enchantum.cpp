#include "test_utility.hpp"
#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <enchantum/cast.hpp>
#include <enchantum/enchantum.hpp>
#include <enchantum/algorithms.hpp>

TEST_CASE("Color enum concept checks", "[concepts]")
{
  STATIC_REQUIRE(enchantum::Enum<Color>);
  STATIC_REQUIRE(enchantum::ScopedEnum<Color>);
  STATIC_REQUIRE_FALSE(enchantum::UnscopedEnum<Color>);
}

TEST_CASE("Color enum to_string / cast", "[stringify]")
{
  using enchantum::cast;
  using enchantum::to_string;

  // to_string tests
  STATIC_CHECK(to_string(Color::Green) == "Green");
  STATIC_CHECK(to_string(Color::Red) == "Red");
  STATIC_CHECK(to_string(Color::Blue) == "Blue");
  STATIC_CHECK(to_string(Color::Purple) == "Purple");
  STATIC_CHECK(to_string(Color::Aqua) == "Aqua");

  // cast tests (valid)
  STATIC_CHECK(cast<Color>("Green") == Color::Green);
  STATIC_CHECK(cast<Color>("Red") == Color::Red);
  STATIC_CHECK(cast<Color>("Blue") == Color::Blue);
  STATIC_CHECK(cast<Color>("Purple") == Color::Purple);
  STATIC_CHECK(cast<Color>("Aqua") == Color::Aqua);

  // cast tests (invalid)
  STATIC_CHECK(!cast<Color>("ZXShady"));
  STATIC_CHECK(!cast<Color>("red"));   // case-sensitive
  STATIC_CHECK(!cast<Color>("GREEN")); // all caps
}


TEST_CASE("Color enum min/max", "[range]")
{

  STATIC_CHECK(enchantum::min<Color> == Color::Aqua);
  STATIC_CHECK(enchantum::max<Color> == Color::Blue);

  REQUIRE(static_cast<std::underlying_type_t<Color>>(enchantum::min<Color>) == -42);
  REQUIRE(static_cast<std::underlying_type_t<Color>>(enchantum::max<Color>) == 214);
}

TEST_CASE("Color enum cast from underlying type", "[cast]")
{
  using enchantum::cast;

  STATIC_CHECK(cast<Color>(124) == Color::Green);
  STATIC_CHECK(cast<Color>(213) == Color::Red);
  STATIC_CHECK(cast<Color>(214) == Color::Blue);
  STATIC_CHECK(cast<Color>(21) == Color::Purple);
  STATIC_CHECK(cast<Color>(-42) == Color::Aqua);
  STATIC_CHECK(!cast<Color>(999));
}

TEST_CASE("Color enum cast from string_view", "[cast]")
{
  using enchantum::cast;

  STATIC_CHECK(cast<Color>("Green") == Color::Green);
  STATIC_CHECK(cast<Color>("Red") == Color::Red);
  STATIC_CHECK(cast<Color>("Blue") == Color::Blue);
  STATIC_CHECK(cast<Color>("Purple") == Color::Purple);
  STATIC_CHECK(cast<Color>("Aqua") == Color::Aqua);

  STATIC_CHECK(!cast<Color>("Chartreuse"));
  STATIC_CHECK(!cast<Color>("BLUE"));
}

struct {
  constexpr bool operator()(std::string_view lhs, std::string_view rhs)
  {
    constexpr auto tolower = [](char c) { return (c >= 'A' && c <= 'Z') ? c + ('a' - 'A') : c; };
    if (lhs.size() != rhs.size())
      return false;
    for (std::size_t i = 0; i < lhs.size(); ++i)
      if (tolower(lhs[i]) != tolower(rhs[i]))
        return false;
    return true;
  }
} constexpr case_insensitive;
TEST_CASE("Color enum cast with custom binary predicate (case insensitive)", "[cast]")
{
  using enchantum::cast;

  STATIC_CHECK(cast<Color>("green", case_insensitive) == Color::Green);
  STATIC_CHECK(cast<Color>("RED", case_insensitive) == Color::Red);
  STATIC_CHECK(cast<Color>("bLuE", case_insensitive) == Color::Blue);
  STATIC_CHECK(cast<Color>("purple", case_insensitive) == Color::Purple);
  STATIC_CHECK(cast<Color>("AQUA", case_insensitive) == Color::Aqua);
  STATIC_CHECK(!cast<Color>("zxSHADY", case_insensitive));
}

TEST_CASE("Color enum index_to_enum", "[index_to_enum]")
{
  using enchantum::index_to_enum;
  STATIC_CHECK(index_to_enum<Color>(0) == Color::Aqua);
  STATIC_CHECK(index_to_enum<Color>(1) == Color::Purple);
  STATIC_CHECK(index_to_enum<Color>(2) == Color::Green);
  STATIC_CHECK(index_to_enum<Color>(3) == Color::Red);
  STATIC_CHECK(index_to_enum<Color>(4) == Color::Blue);
}

TEST_CASE("Color for_each", "[for_each]")
{
  std::vector<Color> visited;

  enchantum::for_each<Color>([&](auto val) { visited.push_back(val); });

  constexpr auto& expected = enchantum::values<Color>;

  REQUIRE(visited.size() == expected.size());
  for (std::size_t i = 0; i < expected.size(); ++i) {
    REQUIRE(visited[i] == expected[i]);
  }
}
