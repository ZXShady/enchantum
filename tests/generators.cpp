#include <catch2/catch_test_macros.hpp>
#include <enchantum/generators.hpp>


namespace {
#define ADD_COMP(OP, NAME)                              \
  template<typename T, typename U = T, typename = void> \
  constexpr bool is_##NAME##_comparable = false;        \
  template<typename T, typename U>                      \
  constexpr bool is_##NAME##_comparable<T, U, decltype(void(std::declval<T>() OP std::declval<U>()))> = true


ADD_COMP(==, eq);
ADD_COMP(!=, neq);
ADD_COMP(<, less);
ADD_COMP(>, greater);
ADD_COMP(>=, greater_equal);
ADD_COMP(<=, less_equal);

} // namespace

enum class A {
	a,b,c
};

TEST_CASE("sized_iterators", "[sized_iterator]")
{
  using entries = decltype(enchantum::entries_generator<A>);
  using values  = decltype(enchantum::values_generator<A>);
  using names   = decltype(enchantum::names_generator<A>);

#define CHECK_OPS(a, b)                             \
  STATIC_CHECK(!is_eq_comparable<a, b>);            \
  STATIC_CHECK(!is_neq_comparable<a, b>);           \
  STATIC_CHECK(!is_less_comparable<a, b>);          \
  STATIC_CHECK(!is_greater_comparable<a, b>);       \
  STATIC_CHECK(!is_less_equal_comparable<a, b>);    \
  STATIC_CHECK(!is_greater_equal_comparable<a, b>); \
  STATIC_CHECK(is_eq_comparable<a, a>);             \
  STATIC_CHECK(is_neq_comparable<a, a>);            \
  STATIC_CHECK(is_less_comparable<a, a>);           \
  STATIC_CHECK(is_greater_comparable<a, a>);        \
  STATIC_CHECK(is_less_equal_comparable<a, a>);     \
  STATIC_CHECK(is_greater_equal_comparable<a, a>);  \
  STATIC_CHECK(is_eq_comparable<b, b>);             \
  STATIC_CHECK(is_neq_comparable<b, b>);            \
  STATIC_CHECK(is_less_comparable<b, b>);           \
  STATIC_CHECK(is_greater_comparable<b, b>);        \
  STATIC_CHECK(is_less_equal_comparable<b, b>);     \
  STATIC_CHECK(is_greater_equal_comparable<b, b>);  \
  STATIC_CHECK(!is_eq_comparable<b, a>);            \
  STATIC_CHECK(!is_neq_comparable<b, a>);           \
  STATIC_CHECK(!is_less_comparable<b, a>);          \
  STATIC_CHECK(!is_greater_comparable<b, a>);       \
  STATIC_CHECK(!is_less_equal_comparable<b, a>);    \
  STATIC_CHECK(!is_greater_equal_comparable<b, a>);


#define CHECK_OPS_CVREF(a, b)    \
  CHECK_OPS(a&, b&)              \
  CHECK_OPS(a&, b&&)             \
  CHECK_OPS(a&, const b&)        \
  CHECK_OPS(a&, const b&&)       \
  CHECK_OPS(a&&, b&)             \
  CHECK_OPS(a&&, b&&)            \
  CHECK_OPS(a&&, const b&)       \
  CHECK_OPS(a&&, const b&&)      \
  CHECK_OPS(const a&, b&)        \
  CHECK_OPS(const a&, b&&)       \
  CHECK_OPS(const a&, const b&)  \
  CHECK_OPS(const a&, const b&&) \
  CHECK_OPS(const a&&, b&)       \
  CHECK_OPS(const a&&, b&&)      \
  CHECK_OPS(const a&&, const b&) \
  CHECK_OPS(const a&&, const b&&)

  CHECK_OPS_CVREF(entries::iterator, names::iterator);
  CHECK_OPS_CVREF(entries::iterator, values::iterator);

  CHECK_OPS_CVREF(names::iterator, entries::iterator);
  CHECK_OPS_CVREF(names::iterator, values::iterator);

  CHECK_OPS_CVREF(values::iterator, entries::iterator);
  CHECK_OPS_CVREF(values::iterator, names::iterator);
}