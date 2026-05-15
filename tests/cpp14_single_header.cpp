#include "cpp14_config.hpp"
#include "../single_include/enchantum_single_header.hpp"

#include <cassert>
#include <string>

namespace single_header_tests {

enum class SingleColor { Red = 0, Blue = 1 };

enum class SinglePermission { None = 0, Read = 1, Write = 2, Execute = 4 };

constexpr SinglePermission operator~(SinglePermission v)
{
  return static_cast<SinglePermission>(~static_cast<int>(v));
}

constexpr SinglePermission operator|(SinglePermission a, SinglePermission b)
{
  return static_cast<SinglePermission>(static_cast<int>(a) | static_cast<int>(b));
}

constexpr SinglePermission operator&(SinglePermission a, SinglePermission b)
{
  return static_cast<SinglePermission>(static_cast<int>(a) & static_cast<int>(b));
}

SinglePermission& operator|=(SinglePermission& a, SinglePermission b)
{
  a = a | b;
  return a;
}

SinglePermission& operator&=(SinglePermission& a, SinglePermission b)
{
  a = a & b;
  return a;
}

} // namespace single_header_tests

template<>
struct enchantum::enum_traits<single_header_tests::SingleColor> {
  static constexpr int min = 0;
  static constexpr int max = 1;
};

template<>
struct enchantum::enum_traits<single_header_tests::SinglePermission> {
  static constexpr int min = 0;
  static constexpr int max = 4;
};

int main()
{
  using single_header_tests::SingleColor;
  using single_header_tests::SinglePermission;

  static_assert(enchantum::count<SingleColor> == 2, "single header reflects C++14 enums");
  static_assert(enchantum::to_string(SingleColor::Blue) == enchantum::string_view("Blue", 4), "single header to_string works");

  const std::string scoped_blue = enchantum::scoped::to_string(SingleColor::Blue);
  assert(scoped_blue == "SingleColor::Blue");

  const auto value = enchantum::cast<SingleColor>(enchantum::string_view("Red", 3));
  assert(value && *value == SingleColor::Red);

  const auto scoped_value = enchantum::scoped::cast<SingleColor>(enchantum::string_view("SingleColor::Red", 16));
  assert(scoped_value && *scoped_value == SingleColor::Red);

  static_assert(enchantum::is_bitflag<SinglePermission>, "single header detects bitflag enums");

  const std::string permission_name = enchantum::to_string_bitflag(SinglePermission::Read | SinglePermission::Execute);
  assert(permission_name == "Read|Execute");

  const auto permission = enchantum::cast_bitflag<SinglePermission>(enchantum::string_view("Read|Write", 10));
  assert(permission && *permission == (SinglePermission::Read | SinglePermission::Write));

  const std::string scoped_permission_name = enchantum::scoped::to_string_bitflag(SinglePermission::Read | SinglePermission::Execute);
  assert(scoped_permission_name == "SinglePermission::Read|SinglePermission::Execute");

  const auto scoped_permission =
    enchantum::scoped::cast_bitflag<SinglePermission>(enchantum::string_view("SinglePermission::Read|SinglePermission::Write", 46));
  assert(scoped_permission && *scoped_permission == (SinglePermission::Read | SinglePermission::Write));
}
