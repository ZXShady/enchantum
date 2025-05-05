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
      #if 0
    if (kv.string != enchantum::to_string(kv.value)) {
      INFO(R"(CHECK(string == enchantum::to_string(value))
with expansion:
)");

      INFO('"' << kv.string << '"' << " != " << '"' << enchantum::to_string(kv.value) << '"');
      CHECK(false);
    }
    #endif
    CHECK(std::string(kv.string) == enchantum::to_string(kv.value));
  }
}
