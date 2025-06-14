#define FMT_UNICODE 0
#include "../single_include/enchantum_single_header.hpp"
#include <catch2/catch_test_macros.hpp>

enum class Simple {
  A,
  B,
  C
};

TEST_CASE("single_include")
{
  STATIC_CHECK(enchantum::to_string(Simple::A) == "A");
  STATIC_CHECK(enchantum::to_string(Simple::B) == "B");
  STATIC_CHECK(enchantum::to_string(Simple::C) == "C");
}