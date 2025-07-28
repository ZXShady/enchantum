#include "test_utility.hpp"
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cstddef>
#include <enchantum/generators.hpp>
#ifdef __cpp_concepts
  #include <concepts>
#endif
namespace {
#define ADD_COMP(OP, NAME)                              \
  template<typename T, typename U = T, typename = void> \
  constexpr bool is_##NAME##_comparable = false;        \
  template<typename T, typename U>                      \
  constexpr bool is_##NAME##_comparable<                \
    T,                                                  \
    U,                                                  \
    decltype((void(std::declval<T>() OP std::declval<U>()), void(std::declval<U>() OP std::declval<T>())))> = true


ADD_COMP(==, eq);
ADD_COMP(!=, neq);
ADD_COMP(<, less);
ADD_COMP(>, greater);
ADD_COMP(>=, greater_equal);
ADD_COMP(<=, less_equal);

} // namespace

enum class A {
  a,
  b,
  c
};

using entries          = decltype(enchantum::entries_generator<A>.begin());
using values           = decltype(enchantum::values_generator<A>.begin());
using names            = decltype(enchantum::names_generator<A>.begin());
using entries_senitiel = decltype(enchantum::entries_generator<A>.end());
using values_senitiel  = decltype(enchantum::values_generator<A>.end());
using names_senitiel   = decltype(enchantum::names_generator<A>.end());

using AllIterators = type_list<std::pair<entries, entries_senitiel>, std::pair<values, values_senitiel>, std::pair<names, names_senitiel>
>;

TEMPLATE_LIST_TEST_CASE("generator iterators", "[generators][iterators]", AllIterators)
{
  using iterator = typename TestType::first_type;
  using senitiel = typename TestType::second_type;

  SECTION("comparable")
  {
#ifdef __cpp_concepts
    STATIC_CHECK(std::random_access_iterator<iterator>);
#endif
    STATIC_CHECK(is_eq_comparable<iterator>);
    STATIC_CHECK(is_neq_comparable<iterator>);
    STATIC_CHECK(is_less_comparable<iterator>);
    STATIC_CHECK(is_greater_comparable<iterator>);
    STATIC_CHECK(is_less_equal_comparable<iterator>);
    STATIC_CHECK(is_greater_equal_comparable<iterator>);

    STATIC_CHECK(is_eq_comparable<iterator, senitiel>);
    STATIC_CHECK(is_neq_comparable<iterator, senitiel>);
    STATIC_CHECK(is_less_comparable<iterator, senitiel>);
    STATIC_CHECK(is_greater_comparable<iterator, senitiel>);
    STATIC_CHECK(is_less_equal_comparable<iterator, senitiel>);
    STATIC_CHECK(is_greater_equal_comparable<iterator, senitiel>);
  }

  SECTION("comparisons")
  {
    auto check = [](auto a, auto b) {
      CAPTURE(b - a);
      CHECK(a < b);
      CHECK_FALSE(a > b);
      CHECK(a <= b);
      CHECK_FALSE(a >= b);

      CHECK_FALSE(a == b);
      CHECK(a != b);

      CHECK_FALSE(b < a);
      CHECK(b > a);
      CHECK(a <= b);
      CHECK_FALSE(a >= b);

      CHECK_FALSE(a == b);
      CHECK(a != b);
    };

    check(iterator{}, iterator{} + 1);
    check(iterator{},senitiel{});
  }
}