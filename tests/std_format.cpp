#include "test_utility.hpp"
#include <catch2/catch_test_macros.hpp>
#include <enchantum/std_format.hpp>
#include <format>

TEST_CASE("Color enum std::format", "[stringify][std_format]")
{
  CHECK(std::format("{}",Color::Green) == "Green");
  CHECK(std::format("{}",Color::Red) == "Red");
  CHECK(std::format("{}",Color::Blue) == "Blue");
  CHECK(std::format("{}",Color::Purple) == "Purple");
  CHECK(std::format("{}",Color::Aqua) == "Aqua");
}
