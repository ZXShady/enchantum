#include <enchantum/enchantum.hpp>
#include <enchantum/bitwise_operators.hpp>
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

enum class Flags {
  None,
  Write = 1 << 0,
  Read = 1 << 1,
  ReadWrite = Read | Write,
  Execute = 1 << 2,
};

ENCHANTUM_DEFINE_BITWISE_FOR(Flags)

enum AutoEmpty {

};

TEST_CASE("cxx 26 reflection", "[cxx26_reflection]") {
  STATIC_CHECK(__cplusplus > 202302);
  STATIC_CHECK(enchantum::names<Test>[0] == "Active");
  STATIC_CHECK(enchantum::names<Test>[1] == "Massive");
  
  STATIC_CHECK(enchantum::count<Flags> == 4);
  STATIC_CHECK(enchantum::names<Flags>[3] == "Execute");
  STATIC_CHECK(enchantum::values<Flags>[3] == Flags::Execute);

  STATIC_CHECK(enchantum::count<Ignored> == 1);
  STATIC_CHECK(enchantum::names<Ignored>[0] == "Something");
  STATIC_CHECK(enchantum::values<Ignored>[0] == Ignored::Something);

  STATIC_CHECK(enchantum::count<AutoEmpty> == 0);
  // STATIC_CHECK(enchantum::cast<Test>("Inactive") == Test::Inactive);
}