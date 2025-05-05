#define FMT_HEADER_ONLY
#define FMT_UNICODE 0 // we don't need unicode
#include <fmt/format.h>

#include "test_utility.hpp"
#include <catch2/catch_test_macros.hpp>
#include <enchantum/bitflags.hpp>
#include <enchantum/fmt_format.hpp>

#include <catch2/catch_template_test_macros.hpp>

TEMPLATE_LIST_TEST_CASE("fmt::format", "[stringify][fmt_format]", AllEnumsTestTypes)
{
  for (const auto& [value, string] : enchantum::entries<TestType>)
    CHECK(fmt::format("{}", value) == string);
}


TEST_CASE("Color enum fmt::format", "[stringify][fmt_format]")
{
  CHECK(fmt::format("{}", Color::Green) == "Green");
  CHECK(fmt::format("{}", Color::Red) == "Red");
  CHECK(fmt::format("{}", Color::Blue) == "Blue");
  CHECK(fmt::format("{}", Color::Purple) == "Purple");
  CHECK(fmt::format("{}", Color::Aqua) == "Aqua");
}

TEST_CASE("UnscopedColor enum fmt::format", "[stringify][fmt_format]")
{
  CHECK(fmt::format("{}", UnscopedColor::Green) == "Green");
  CHECK(fmt::format("{}", UnscopedColor::Red) == "Red");
  CHECK(fmt::format("{}", UnscopedColor::Blue) == "Blue");
  CHECK(fmt::format("{}", UnscopedColor::Purple) == "Purple");
  CHECK(fmt::format("{}", UnscopedColor::Aqua) == "Aqua");
}

TEST_CASE("Flags enum fmt::format", "[stringify][fmt_format]")
{
  SECTION("Normal fmt::format")
  {
    CHECK(fmt::format("{}", Flags::Flag0) == "Flag0");
    CHECK(fmt::format("{}", Flags::Flag1) == "Flag1");
    CHECK(fmt::format("{}", Flags::Flag2) == "Flag2");
    CHECK(fmt::format("{}", Flags::Flag3) == "Flag3");
    CHECK(fmt::format("{}", Flags::Flag4) == "Flag4");
  }
  SECTION("fmt::format with enchantum::to_string_bitflag")
  {
    CHECK(fmt::format("{}", enchantum::to_string_bitflag(Flags::Flag0 | Flags::Flag4)) == "Flag0|Flag4");
    CHECK(fmt::format("{}", enchantum::to_string_bitflag(Flags::Flag0 | Flags::Flag4 | Flags(200))) == "");
    CHECK(fmt::format("{}", enchantum::to_string_bitflag(enchantum::values_ors<Flags>)) ==
          "Flag0|Flag1|Flag2|Flag3|Flag4|Flag5|Flag6");
  }
}
