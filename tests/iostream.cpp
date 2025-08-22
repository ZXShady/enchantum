#include "test_utility.hpp"
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <enchantum/enchantum.hpp>
#include <enchantum/iostream.hpp>
#include <sstream>

using namespace enchantum::iostream_operators;

TEMPLATE_LIST_TEST_CASE("iostream", "[iostream]", AllEnumsTestTypes)
{
  SECTION("istream")
  {
    SECTION("identity")
    {
      for (const auto& [value, string] : enchantum::entries<TestType>) {
        auto     iss = std::istringstream(std::string(string));
        TestType input{};
        iss >> input;
        CHECK(input == value);
        CHECK(iss);
      }
    }
    SECTION("invalid names")
    {
      for (const auto& [value, string] : enchantum::entries<TestType>) {
        auto     iss = std::istringstream("WorldHello" + std::string(string) + "HelloWorld");
        TestType input{};
        iss >> input;
        CHECK(input == TestType{});
        CHECK_FALSE(iss);
      }
    }
  }


  SECTION("ostream")
  {
    for (const auto& [value, string] : enchantum::entries<TestType>) {
      auto oss = std::ostringstream();
      oss << value;
      CHECK(oss.str() == string);
      CHECK(oss);
    }
  }
}


TEST_CASE("ostream operator<< return numbers if invalid enum", "[ostream]")
{
  const auto tostringoss = [](auto x) {
    auto oss = std::ostringstream();
    oss << x;
    return std::move(oss).str();
  };

  CHECK(tostringoss(Color::Red) == "Red");
  CHECK(tostringoss(Color(300)) == "300");
  CHECK(tostringoss(Color(-300)) == "-300");

  CHECK(tostringoss(Flags::Flag0 | Flags::Flag1) == "Flag0|Flag1");
  CHECK(tostringoss(Flags::Flag0 | Flags(1u << 7)) == std::to_string(1 | (1u << 7)));
}
