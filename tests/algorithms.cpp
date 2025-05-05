#include "test_utility.hpp"
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_tostring.hpp>
#include <enchantum/algorithms.hpp>

TEMPLATE_LIST_TEST_CASE("for_each", "[algorithms][for_each]", AllEnumsTestTypes)
{
  std::vector<std::string_view> names;

  enchantum::for_each<TestType>([&names](const auto c) {
    if constexpr (enchantum::count<TestType> != 0 && c != enchantum::values<TestType>[0])
      names.emplace_back(enchantum::to_string(c.value));
  });
  CHECK(names.size() + 1 == enchantum::count<TestType>);

  for (std::size_t i = 0; i < names.size(); ++i)
    CHECK(names[i] == enchantum::names<TestType>[i+1]);
}
