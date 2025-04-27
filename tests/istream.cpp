#include "test_utility.hpp"
#include <catch2/catch_test_macros.hpp>
#include <enchantum/enchantum.hpp>
#include <enchantum/istream.hpp>
#include <sstream>

using namespace enchantum::istream_operators;

TEST_CASE("istream operator>> parses valid enum name")
{
  std::istringstream iss("Blue");
  Color              c{};
  iss >> c;
  REQUIRE(c == Color::Blue);
  REQUIRE(iss);
}

TEST_CASE("istream operator>> fails on invalid enum name")
{
  std::istringstream iss("XxZXShadyxX");//so cool 
  Color              c = Color::Red;
  iss >> c;
  REQUIRE(c == Color::Red);
  REQUIRE(iss.fail());
}
