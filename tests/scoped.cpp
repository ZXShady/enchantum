#include "test_utility.hpp"
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <enchantum/enchantum.hpp>
#include <enchantum/scoped.hpp>

TEMPLATE_LIST_TEST_CASE("scoped::to_string", "[stringify]", AllEnumsTestTypes)
{
  for (const auto v : enchantum::values<TestType>) {
    const auto string = enchantum::scoped::to_string(v);
    REQUIRE(string.find("::") != std::string::npos);
    REQUIRE(string.find("::") == string.rfind("::"));

    CHECK(enchantum::scoped::cast<TestType>(string).value() == v);
    CHECK(enchantum::scoped::contains<TestType>(string));
    CHECK_FALSE(enchantum::scoped::cast<TestType>(enchantum::to_string(v)));
    CHECK_FALSE(enchantum::scoped::cast<TestType>("::" + std::string(enchantum::to_string(v))));
    CHECK_FALSE(enchantum::cast<TestType>(string));
  }
}

TEST_CASE("scoped::to_string")
{
  SECTION("Color")
  {
    CHECK(enchantum::scoped::to_string(Color::Red) == "Color::Red");
    CHECK(enchantum::scoped::to_string(Color::Aqua) == "Color::Aqua");
    CHECK(enchantum::scoped::to_string(Color::Purple) == "Color::Purple");
    CHECK(enchantum::scoped::to_string(Color::Green) == "Color::Green");
    CHECK(enchantum::scoped::to_string(Color::Blue) == "Color::Blue");
  }

  SECTION("Flags")
  {
    CHECK(enchantum::scoped::to_string(Flags::Flag0) == "Flags::Flag0");
    CHECK(enchantum::scoped::to_string(Flags::Flag1) == "Flags::Flag1");
    CHECK(enchantum::scoped::to_string(Flags::Flag2) == "Flags::Flag2");
    CHECK(enchantum::scoped::to_string(Flags::Flag3) == "Flags::Flag3");
    CHECK(enchantum::scoped::to_string(Flags::Flag4) == "Flags::Flag4");
    CHECK(enchantum::scoped::to_string(Flags::Flag5) == "Flags::Flag5");
    CHECK(enchantum::scoped::to_string(Flags::Flag6) == "Flags::Flag6");
  }

  SECTION("UnscopedColor")
  {
    CHECK(enchantum::scoped::to_string(UnscopedColor::Aqua) == "UnscopedColor::Aqua");
    CHECK(enchantum::scoped::to_string(UnscopedColor::Purple) == "UnscopedColor::Purple");
    CHECK(enchantum::scoped::to_string(UnscopedColor::Green) == "UnscopedColor::Green");
    CHECK(enchantum::scoped::to_string(UnscopedColor::Red) == "UnscopedColor::Red");
    CHECK(enchantum::scoped::to_string(UnscopedColor::Blue) == "UnscopedColor::Blue");
  }


  SECTION("ImGuiFreeTypeBuilderFlags")
  {
    CHECK(enchantum::scoped::to_string(ImGuiFreeTypeBuilderFlags::ImGuiFreeTypeBuilderFlags_NoHinting) ==
          "ImGuiFreeTypeBuilderFlags::NoHinting");
    CHECK(enchantum::scoped::to_string(ImGuiFreeTypeBuilderFlags::ImGuiFreeTypeBuilderFlags_NoAutoHint) ==
          "ImGuiFreeTypeBuilderFlags::NoAutoHint");
    CHECK(enchantum::scoped::to_string(ImGuiFreeTypeBuilderFlags::ImGuiFreeTypeBuilderFlags_ForceAutoHint) ==
          "ImGuiFreeTypeBuilderFlags::ForceAutoHint");
    CHECK(enchantum::scoped::to_string(ImGuiFreeTypeBuilderFlags::ImGuiFreeTypeBuilderFlags_LightHinting) ==
          "ImGuiFreeTypeBuilderFlags::LightHinting");
    CHECK(enchantum::scoped::to_string(ImGuiFreeTypeBuilderFlags::ImGuiFreeTypeBuilderFlags_MonoHinting) ==
          "ImGuiFreeTypeBuilderFlags::MonoHinting");
    CHECK(enchantum::scoped::to_string(ImGuiFreeTypeBuilderFlags::ImGuiFreeTypeBuilderFlags_Bold) ==
          "ImGuiFreeTypeBuilderFlags::Bold");
    CHECK(enchantum::scoped::to_string(ImGuiFreeTypeBuilderFlags::ImGuiFreeTypeBuilderFlags_Oblique) ==
          "ImGuiFreeTypeBuilderFlags::Oblique");
    CHECK(enchantum::scoped::to_string(ImGuiFreeTypeBuilderFlags::ImGuiFreeTypeBuilderFlags_Monochrome) ==
          "ImGuiFreeTypeBuilderFlags::Monochrome");
    CHECK(enchantum::scoped::to_string(ImGuiFreeTypeBuilderFlags::ImGuiFreeTypeBuilderFlags_LoadColor) ==
          "ImGuiFreeTypeBuilderFlags::LoadColor");
    CHECK(enchantum::scoped::to_string(ImGuiFreeTypeBuilderFlags::ImGuiFreeTypeBuilderFlags_Bitmap) ==
          "ImGuiFreeTypeBuilderFlags::Bitmap");
  }


  SECTION("MinMaxValuesCStyle")
  {
    CHECK(enchantum::scoped::to_string(MinMaxValuesCStyle::MinMaxValuesCStyle_min) ==
          "MinMaxValuesCStyle::MinMaxValuesCStyle_min");
    CHECK(enchantum::scoped::to_string(MinMaxValuesCStyle::MinMaxValuesCStyle_max) ==
          "MinMaxValuesCStyle::MinMaxValuesCStyle_max");
  }

  SECTION("MinMaxValues")
  {
    CHECK(enchantum::scoped::to_string(MinMaxValues::min) == "MinMaxValues::min");
    CHECK(enchantum::scoped::to_string(MinMaxValues::max) == "MinMaxValues::max");
  }


  SECTION("Letters")
  {
    for (const auto letter : enchantum::values<Letters>) {
      CHECK(enchantum::scoped::to_string(letter) == "Letters::" + std::string(enchantum::to_string(letter)));
    }
  }
}

TEMPLATE_LIST_TEST_CASE("scoped::cast_bitflag identities", "", AllFlagsTestTypes)
{
  constexpr auto value_ors = enchantum::value_ors<TestType>;
  CHECK(value_ors == enchantum::scoped::cast_bitflag<TestType>(enchantum::scoped::to_string_bitflag(value_ors)));
  CHECK(value_ors == enchantum::scoped::cast_bitflag<TestType>(enchantum::scoped::to_string_bitflag(value_ors, ','),','));
}

TEST_CASE("scoped::to_string_bitflag")
{
  CHECK(enchantum::scoped::to_string_bitflag(enchantum::value_ors<Flags>) ==
        "Flags::Flag0|Flags::Flag1|Flags::Flag2|Flags::Flag3|Flags::Flag4|Flags::Flag5|Flags::Flag6");
  CHECK(enchantum::scoped::to_string_bitflag(enchantum::value_ors<Flags>, ',') ==
        "Flags::Flag0,Flags::Flag1,Flags::Flag2,Flags::Flag3,Flags::Flag4,Flags::Flag5,Flags::Flag6");
}