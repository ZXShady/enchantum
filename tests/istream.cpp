#include "test_utility.hpp"
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <enchantum/enchantum.hpp>
#include <enchantum/iostream.hpp>
#include <sstream>


using enchantum::iostream_operators::operator>>;
TEMPLATE_LIST_TEST_CASE("istream operator>>", "[istream]", AllEnumsTestTypes)
{
  SECTION("Parses correctly")
  {
    for (const auto& [value, string] : enchantum::entries<TestType>) {
      auto     iss = std::istringstream(std::string(string));
      TestType input{};
      iss >> input;
      CHECK(input == value);
      CHECK(iss);
    }
  }

  SECTION("Invalid names")
  {
    for (const auto& [value, string] : enchantum::entries<TestType>) {
      auto     iss = std::istringstream("WorldHello" + std::string(string) + "HelloWorld");
      TestType           input{};
      iss >> input;
      CHECK(input == TestType{});
      CHECK_FALSE(iss);
    }
  }
}
