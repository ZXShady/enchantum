#include <catch2/catch_test_macros.hpp>
#include <enchantum/generators.hpp>


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

TEST_CASE("sized_iterators", "[sized_iterator]")
{
  using entries = decltype(enchantum::entries_generator<A>.begin());
  using values  = decltype(enchantum::values_generator<A>.begin());
  using names   = decltype(enchantum::names_generator<A>.begin());

  using entries_senitiel = decltype(enchantum::entries_generator<A>.end());
  using values_senitiel  = decltype(enchantum::values_generator<A>.end());
  using names_senitiel   = decltype(enchantum::names_generator<A>.end());


#define CHECK_OPS(a, b)                             \
  STATIC_CHECK(!is_eq_comparable<a, b>);            \
  STATIC_CHECK(!is_neq_comparable<a, b>);           \
  STATIC_CHECK(!is_less_comparable<a, b>);          \
  STATIC_CHECK(!is_greater_comparable<a, b>);       \
  STATIC_CHECK(!is_less_equal_comparable<a, b>);    \
  STATIC_CHECK(!is_greater_equal_comparable<a, b>); \
  STATIC_CHECK(is_eq_comparable<a>);                \
  STATIC_CHECK(is_neq_comparable<a>);               \
  STATIC_CHECK(is_less_comparable<a>);              \
  STATIC_CHECK(is_greater_comparable<a>);           \
  STATIC_CHECK(is_less_equal_comparable<a>);        \
  STATIC_CHECK(is_greater_equal_comparable<a>);     \
  STATIC_CHECK(is_eq_comparable<b>);                \
  STATIC_CHECK(is_neq_comparable<b>);               \
  STATIC_CHECK(is_less_comparable<b>);              \
  STATIC_CHECK(is_greater_comparable<b>);           \
  STATIC_CHECK(is_less_equal_comparable<b>);        \
  STATIC_CHECK(is_greater_equal_comparable<b>);

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

  CHECK_OPS_CVREF(entries, names);
  CHECK_OPS_CVREF(entries, values);

  CHECK_OPS_CVREF(names, entries);
  CHECK_OPS_CVREF(names, values);

  CHECK_OPS_CVREF(values, entries);
  CHECK_OPS_CVREF(values, names);

#define CHECK_SENITIEL(a, s)                    \
  STATIC_CHECK(is_eq_comparable<a, s>);         \
  STATIC_CHECK(is_neq_comparable<a, s>);        \
  STATIC_CHECK(is_less_comparable<a, s>);       \
  STATIC_CHECK(is_greater_comparable<a, s>);    \
  STATIC_CHECK(is_less_equal_comparable<a, s>); \
  STATIC_CHECK(is_greater_equal_comparable<a, s>);

  CHECK_SENITIEL(entries, entries_senitiel);
  CHECK_SENITIEL(values, values_senitiel);
  CHECK_SENITIEL(values, names_senitiel);
}