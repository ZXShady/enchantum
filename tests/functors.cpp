#include "test_utility.hpp"
#include <algorithm>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_tostring.hpp>
#include <enchantum/enchantum.hpp>

template<typename It, typename Out, typename Func>
void transform_unwrap(It begin, const It end, Out out, const Func f)
{
  while (begin != end)
    *out++ = f(**begin++);
}

TEMPLATE_LIST_TEST_CASE("cast is a functor", "[functors]", AllEnumsTestTypes)
{
  constexpr auto&                                     strings = enchantum::names<TestType>;
  std::array<std::optional<TestType>, strings.size()> values;

  std::transform(strings.begin(), strings.end(), values.begin(), enchantum::cast<TestType>);

  CHECK(std::equal(enchantum::values<TestType>.begin(), enchantum::values<TestType>.end(), values.begin()));
}

TEMPLATE_LIST_TEST_CASE("enum_to_index and index_to_enum are functors", "[functors]", AllEnumsTestTypes)
{
  constexpr auto&                                       values = enchantum::values<TestType>;
  std::array<std::optional<std::size_t>, values.size()> indices;
  std::array<std::optional<TestType>, values.size()>    values_transformed;

  std::transform(values.begin(), values.end(), indices.begin(), enchantum::enum_to_index);
  transform_unwrap(indices.begin(), indices.end(), values_transformed.begin(), enchantum::index_to_enum<TestType>);

  for (std::size_t i = 0; i < indices.size(); ++i) {
    CHECK(indices[i] == i);
  }

  for (std::size_t i = 0; i < values_transformed.size(); ++i) {
    CHECK(values_transformed[i] == enchantum::values<TestType>[i]);
  }
}

