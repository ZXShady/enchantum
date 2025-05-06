#include "test_utility.hpp"
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <enchantum/enchantum.hpp>
#include <enchantum/ostream.hpp>
#include <sstream>

using namespace enchantum::ostream_operators;

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
  SECTION("Invalid enums return empty string")
  {
    auto       oss                     = std::ostringstream();
    const auto output_if_not_contained = [&oss](TestType value) {
      using T = std::underlying_type_t<TestType>;
      auto v  = static_cast<TestType>(T(value) + 1);
      if (!enchantum::contains(v))
        oss << v;
      CHECK(oss.str().empty());
      CHECK(oss);
    };
    output_if_not_contained(enchantum::max<TestType>);
    output_if_not_contained(enchantum::min<TestType>);
  }
}
