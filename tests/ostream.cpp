#include "test_utility.hpp"
#include <catch2/catch_test_macros.hpp>
#include <enchantum/enchantum.hpp>
#include <enchantum/ostream.hpp>
#include <sstream>

using namespace enchantum::ostream_operators;

TEST_CASE("ostream operator<< outputs enum name")
{
  std::ostringstream oss;
  SECTION("Valid Enum")
  {
    oss << Color::Green;
    REQUIRE(oss.str() == "Green");
  }

  SECTION("Invalid Enum")
  {
    oss << Color(0xdead);
    REQUIRE(oss.str().empty());
  }
}

