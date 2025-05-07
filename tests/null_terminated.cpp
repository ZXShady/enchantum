#include "test_utility.hpp"
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cstring>
#include <enchantum/entries.hpp>

TEMPLATE_LIST_TEST_CASE("entries<E> strings are null-terminated character arrays", "[entries][null_termination]", AllEnumsTestTypes)
{
  for (const auto& [_, s] : enchantum::entries<TestType>) {
    (void)_;
    CHECK(s.size() == std::strlen(s.data()));
  }
}
