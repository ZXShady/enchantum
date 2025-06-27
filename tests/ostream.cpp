#include "test_utility.hpp"
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <enchantum/enchantum.hpp>
#include <enchantum/iostream.hpp>
#include <sstream>

using enchantum::iostream_operators::operator<<;

TEMPLATE_LIST_TEST_CASE("ostream operator<<", "[ostream]", AllEnumsTestTypes)
{
  SECTION("Valid enums return to_string")
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


}
