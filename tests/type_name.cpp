#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include "test_utility.hpp"
#include <enchantum/type_name.hpp>

namespace Namespace {
enum UnscopedEnum : int;
enum class Enum;
struct Struct;
class Class;
} // namespace Namespace

enum UnscopedEnum : int;
enum class Enum;
struct Struct;
class Class;

TEST_CASE("type_name", "[type_name]")
{
  STATIC_CHECK(enchantum::type_name<Namespace::UnscopedEnum> == "Namespace::UnscopedEnum");
  STATIC_CHECK(enchantum::type_name<Namespace::Enum> == "Namespace::Enum");
  STATIC_CHECK(enchantum::type_name<Namespace::Struct> == "Namespace::Struct");
  STATIC_CHECK(enchantum::type_name<Namespace::Class> == "Namespace::Class");
  STATIC_CHECK(enchantum::type_name<UnscopedEnum> == "UnscopedEnum");
  STATIC_CHECK(enchantum::type_name<Enum> == "Enum");
  STATIC_CHECK(enchantum::type_name<Struct> == "Struct");
  STATIC_CHECK(enchantum::type_name<Class> == "Class");
}


TEMPLATE_LIST_TEST_CASE("type_name is null terminated", "[type_name]", AllEnumsTestTypes)
{
  STATIC_CHECK('\0' == enchantum::type_name<TestType>.data()[enchantum::type_name<TestType>.size()]);
}



