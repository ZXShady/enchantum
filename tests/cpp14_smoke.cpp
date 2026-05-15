#include <enchantum/bitflags.hpp>
#include <enchantum/enchantum.hpp>
#include <enchantum/iostream.hpp>
#include <enchantum/next_value.hpp>
#include <enchantum/scoped.hpp>

#include <cassert>
#include <sstream>
#include <string>
#include <type_traits>

namespace {

enum class Color { Red = -1, Green = 0, Blue = 2 };

enum class Permission { None = 0, Read = 1, Write = 2, Execute = 4 };

constexpr Permission operator~(Permission v)
{
  return static_cast<Permission>(~static_cast<int>(v));
}

constexpr Permission operator|(Permission a, Permission b)
{
  return static_cast<Permission>(static_cast<int>(a) | static_cast<int>(b));
}

constexpr Permission operator&(Permission a, Permission b)
{
  return static_cast<Permission>(static_cast<int>(a) & static_cast<int>(b));
}

Permission& operator|=(Permission& a, Permission b)
{
  a = a | b;
  return a;
}

Permission& operator&=(Permission& a, Permission b)
{
  a = a & b;
  return a;
}

} // namespace

template<>
struct enchantum::enum_traits<Color> {
  static constexpr int min = -1;
  static constexpr int max = 2;
};

template<>
struct enchantum::enum_traits<Permission> {
  static constexpr int min = 0;
  static constexpr int max = 4;
};

int main()
{
  static_assert(std::is_enum<Color>::value, "test enum must be an enum");
  static_assert(enchantum::count<Color> == 3, "Color has three reflected values");
  static_assert(enchantum::contains(Color::Red), "Color::Red is reflected");
  static_assert(enchantum::to_string(Color::Blue) == enchantum::string_view("Blue", 4), "to_string works in C++14 constexpr context");

  const std::string scoped_blue = enchantum::scoped::to_string(Color::Blue);
  assert(scoped_blue == "Color::Blue");

  const auto green = enchantum::cast<Color>(enchantum::string_view("Green", 5));
  assert(green && *green == Color::Green);

  const auto scoped_green = enchantum::scoped::cast<Color>(enchantum::string_view("Color::Green", 12));
  assert(scoped_green && *scoped_green == Color::Green);

  const auto blue_index = enchantum::enum_to_index(Color::Blue);
  assert(blue_index && *blue_index == 2u);

  const auto next = enchantum::next_value(Color::Red);
  assert(next && *next == Color::Green);

  static_assert(enchantum::is_bitflag<Permission>, "Permission is detected as a bitflag enum");
  assert(enchantum::contains_bitflag(Permission::Read | Permission::Write));

  const std::string permission_name = enchantum::to_string_bitflag(Permission::Read | Permission::Execute);
  assert(permission_name == "Read|Execute");

  const auto permission = enchantum::cast_bitflag<Permission>(enchantum::string_view("Read|Write", 10));
  assert(permission && *permission == (Permission::Read | Permission::Write));

  const std::string scoped_permission_name = enchantum::scoped::to_string_bitflag(Permission::Read | Permission::Execute);
  assert(scoped_permission_name == "Permission::Read|Permission::Execute");

  const auto scoped_permission =
    enchantum::scoped::cast_bitflag<Permission>(enchantum::string_view("Permission::Read|Permission::Write", 34));
  assert(scoped_permission && *scoped_permission == (Permission::Read | Permission::Write));

  std::istringstream color_stream("Blue");
  using namespace enchantum::iostream_operators;
  Color streamed_color{};
  color_stream >> streamed_color;
  assert(color_stream && streamed_color == Color::Blue);

  std::istringstream permission_stream("Read|Execute");
  Permission streamed_permission{};
  permission_stream >> streamed_permission;
  assert(permission_stream && streamed_permission == (Permission::Read | Permission::Execute));
}
