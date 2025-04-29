#include "test_utility.hpp"
#include <catch2/catch_test_macros.hpp>
#include <enchantum/bitflags.hpp>
#include <enchantum/std_format.hpp>
#include <format>

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
    CHECK(std::format("{}", enchantum::to_string_bitflag(Flags::Flag0 | Flags::Flag4)) == "Flag0|Flag4");
    CHECK(std::format("{}", enchantum::to_string_bitflag(Flags::Flag0 | Flags::Flag4 | Flags(200))) == "");
    CHECK(std::format("{}", enchantum::to_string_bitflag(enchantum::values_ors<Flags>)) ==
          "Flag0|Flag1|Flag2|Flag3|Flag4|Flag5|Flag6");
  }
}
