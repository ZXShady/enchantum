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

enum Ignored {
  IAmIgnored  [[=enchantum::ignore]] = 0xdead,
  Something   = 10000,
};

enum AutoEmpty {

};

TEST_CASE("cxx 26 reflection", "[cxx26_reflection]") {
  STATIC_CHECK(__cplusplus > 202302);
  STATIC_CHECK(enchantum::names<Test>[0] == "Active");
  STATIC_CHECK(enchantum::names<Test>[1] == "Massive");
  
  STATIC_CHECK(enchantum::count<Ignored> == 1);
  STATIC_CHECK(enchantum::names<Ignored>[0] == "Something");
  STATIC_CHECK(enchantum::values<Ignored>[0] == Ignored::Something);

  STATIC_CHECK(enchantum::count<AutoEmpty> == 0);
  // STATIC_CHECK(enchantum::cast<Test>("Inactive") == Test::Inactive);
}