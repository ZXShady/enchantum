#include "test_utility.hpp"
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cstring>
#include <enchantum/entries.hpp>

TEMPLATE_LIST_TEST_CASE("entries<E> strings null-terminated character arrays", "[entries][null_termination]", AllEnumsTestTypes)
{
  SECTION("Null terminated")
  {
    for (const auto& s : enchantum::names<TestType>) {
      CHECK(s.size() == std::strlen(s.data()));
    }
  }

  SECTION("Not Null Terminated")
  {
    constexpr auto names  = enchantum::names<TestType, std::string_view, false>;
    const auto* names_ptr = names[0].data(); // implementation detail but all strings are allocated next to each other
    for (std::ptrdiff_t i = 0; i < static_cast<std::ptrdiff_t>(names.size()) - 1; ++i) {
      const auto& name = names[static_cast<std::size_t>(i)];
      CHECK(*names_ptr != '\0');
      names_ptr += name.size();
    }
  }
}
