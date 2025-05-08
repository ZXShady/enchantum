#include "test_utility.hpp"
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_tostring.hpp>
#include <enchantum/enchantum.hpp>

template<typename E>
struct ValueAndString {
  E                value;
  std::string_view string;
};

TEMPLATE_LIST_TEST_CASE("KV as entries key-value pair", "[entries][override_key_value_pair]", AllEnumsTestTypes)
{
  for (const auto& kv : enchantum::entries<TestType, ValueAndString<TestType>>) {
    CHECK(kv.string == enchantum::to_string(kv.value));
  }
}
