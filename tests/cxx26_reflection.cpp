#include <enchantum/enchantum.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>


namespace {
enum class Test {
  Active   = 1,
  Inactive = 1,
  Pending  = 1,
  Massive  = 123321
};
} // namespace


TEST_CASE("cast cast_bitflags", "[casts][bitflags]") {
  STATIC_CHECK(__cplusplus > 202302);
  STATIC_CHECK(enchantum::names<Test>[0] == "Active");
  STATIC_CHECK(enchantum::names<Test>[1] == "Massive");

  // STATIC_CHECK(enchantum::cast<Test>("Inactive") == Test::Inactive);
}