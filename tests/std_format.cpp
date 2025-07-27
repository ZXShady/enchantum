#include "test_utility.hpp"
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <enchantum/bitflags.hpp>
#include <enchantum/std_format.hpp>
#include <format>

TEMPLATE_LIST_TEST_CASE("std::format", "[stringify][std_format]", AllEnumsTestTypes)
{
   
  for (const auto& [value, string] : enchantum::entries<TestType>)
    CHECK(std::format("{}", value) == string);
  using T = std::underlying_type_t<TestType>;
  if constexpr (!enchantum::is_bitflag<TestType> && !std::is_same_v<bool, std::underlying_type_t<TestType>>)
    if constexpr (123 <= T(enchantum::max<TestType>) && !enchantum::contains<TestType>(123))
      CHECK("123" == std::format("{}", TestType(123)));
}


TEST_CASE("Color enum std::format", "[stringify][std_format]")
{
  CHECK(std::format("{}", Color::Green) == "Green");
  CHECK(std::format("{}", Color::Red) == "Red");
  CHECK(std::format("{}", Color::Blue) == "Blue");
  CHECK(std::format("{}", Color::Purple) == "Purple");
  CHECK(std::format("{}", Color::Aqua) == "Aqua");
}

TEST_CASE("UnscopedColor enum std::format", "[stringify][std_format]")
{
  CHECK(std::format("{}", UnscopedColor::Green) == "Green");
  CHECK(std::format("{}", UnscopedColor::Red) == "Red");
  CHECK(std::format("{}", UnscopedColor::Blue) == "Blue");
  CHECK(std::format("{}", UnscopedColor::Purple) == "Purple");
  CHECK(std::format("{}", UnscopedColor::Aqua) == "Aqua");
}

TEST_CASE("Flags enum std::format", "[stringify][std_format]")
{
  SECTION("Normal std::format")
  {
    CHECK(std::format("{}", Flags::Flag0) == "Flag0");
    CHECK(std::format("{}", Flags::Flag1) == "Flag1");
    CHECK(std::format("{}", Flags::Flag2) == "Flag2");
    CHECK(std::format("{}", Flags::Flag3) == "Flag3");
    CHECK(std::format("{}", Flags::Flag4) == "Flag4");
  }
  SECTION("std::format with enchantum::to_string_bitflag")
  {
    CHECK(std::format("{}", Flags::Flag0 | Flags::Flag4) == "Flag0|Flag4");
    CHECK(std::format("{}", Flags::Flag0 | Flags::Flag4 | Flags(200)) == "217");
    CHECK(std::format("{}", enchantum::value_ors<Flags>) == "Flag0|Flag1|Flag2|Flag3|Flag4|Flag5|Flag6");
  }
}
