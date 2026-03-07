#include <enchantum/entries.hpp>
#include <enchantum/enchantum.hpp>
#include <enchantum/generators.hpp>
#include <enchantum/scoped.hpp>

#include <catch2/catch_test_macros.hpp>

enum class EmptyEnum;
ENCHANTUM_DECLARE_EMPTY(EmptyEnum);

TEST_CASE("empty enum", "[empty]")
{
  STATIC_CHECK(enchantum::count<EmptyEnum> == 0);
  STATIC_CHECK(enchantum::names<EmptyEnum>.empty());
  STATIC_CHECK(enchantum::values<EmptyEnum>.empty());
  STATIC_CHECK(enchantum::entries<EmptyEnum>.empty());

  STATIC_CHECK(enchantum::names_generator<EmptyEnum>.size() == 0);
  STATIC_CHECK(enchantum::values_generator<EmptyEnum>.size() == 0);
  STATIC_CHECK(enchantum::entries_generator<EmptyEnum>.size() == 0);
  STATIC_CHECK(enchantum::to_string(EmptyEnum{}).empty());
  STATIC_CHECK_FALSE(enchantum::enum_to_index(EmptyEnum{}));
  STATIC_CHECK_FALSE(enchantum::index_to_enum<EmptyEnum>(0));
  STATIC_CHECK_FALSE(enchantum::cast<EmptyEnum>(0));
  STATIC_CHECK_FALSE(enchantum::cast<EmptyEnum>(""));
  STATIC_CHECK_FALSE(enchantum::contains<EmptyEnum>(0));
  STATIC_CHECK_FALSE(enchantum::contains<EmptyEnum>(""));

  CHECK(enchantum::scoped::to_string(EmptyEnum{}).empty());
  CHECK_FALSE(enchantum::scoped::cast<EmptyEnum>(""));
  CHECK_FALSE(enchantum::scoped::contains<EmptyEnum>(""));
}

