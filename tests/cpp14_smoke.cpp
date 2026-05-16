#include <enchantum/algorithms.hpp>
#include <enchantum/array.hpp>
#include <enchantum/bitflags.hpp>
#include <enchantum/bitset.hpp>
#include <enchantum/enchantum.hpp>
#include <enchantum/generators.hpp>
#include <enchantum/iostream.hpp>
#include <enchantum/next_value.hpp>
#include <enchantum/scoped.hpp>

#include <cassert>
#include <cstddef>
#include <sstream>
#include <string>
#include <type_traits>

namespace {

enum class Color { Red = -1, Green = 0, Blue = 2 };

enum class Permission { None = 0, Read = 1, Write = 2, Execute = 4 };

struct case_insensitive_char_equal {
  constexpr char lower(char c) const noexcept
  {
    return c >= 'A' && c <= 'Z' ? static_cast<char>(c - 'A' + 'a') : c;
  }

  constexpr bool operator()(char a, char b) const noexcept { return lower(a) == lower(b); }
};

struct string_view_equal {
  constexpr bool operator()(enchantum::string_view a, enchantum::string_view b) const noexcept { return a == b; }
};

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
  static_assert(enchantum::values<Color>[0] == Color::Red, "values works in C++14 constexpr context");
  static_assert(enchantum::names<Color>[1] == enchantum::string_view("Green", 5), "names works in C++14 constexpr context");
  static_assert(enchantum::entries<Color>[2].first == Color::Blue, "entries values work in C++14 constexpr context");
  static_assert(enchantum::entries<Color>[2].second == enchantum::string_view("Blue", 4), "entries names work in C++14 constexpr context");
  static_assert(enchantum::names<Color, enchantum::string_view, false>[0] == enchantum::string_view("Red", 3),
                "non-null-terminated names work in C++14 constexpr context");

  const std::string scoped_blue = enchantum::scoped::to_string(Color::Blue);
  assert(scoped_blue == "Color::Blue");

  assert((enchantum::contains<Color>(enchantum::string_view("green", 5), case_insensitive_char_equal{})));
  assert((enchantum::contains<Color>(enchantum::string_view("Green", 5), string_view_equal{})));

  const auto green = enchantum::cast<Color>(enchantum::string_view("Green", 5));
  assert(green && *green == Color::Green);

  const auto blue_case_insensitive = enchantum::cast<Color>(enchantum::string_view("blue", 4), case_insensitive_char_equal{});
  assert(blue_case_insensitive && *blue_case_insensitive == Color::Blue);

  const auto scoped_green = enchantum::scoped::cast<Color>(enchantum::string_view("Color::Green", 12));
  assert(scoped_green && *scoped_green == Color::Green);

  const auto blue_index = enchantum::enum_to_index(Color::Blue);
  assert(blue_index && *blue_index == 2u);
  const auto blue_from_index = enchantum::index_to_enum<Color>(2);
  assert(blue_from_index && *blue_from_index == Color::Blue);

  assert(enchantum::values_generator<Color>.size() == enchantum::count<Color>);
  assert(enchantum::values_generator<Color>[0] == Color::Red);
  assert(enchantum::names_generator<Color>[1] == enchantum::string_view("Green", 5));
  assert((enchantum::names_generator<Color, enchantum::string_view, false>[2] == enchantum::string_view("Blue", 4)));
  assert(enchantum::entries_generator<Color>[2].first == Color::Blue);
  assert(enchantum::entries_generator<Color>[2].second == enchantum::string_view("Blue", 4));

  auto values_it = enchantum::values_generator<Color>.begin();
  assert(*values_it == Color::Red);
  assert(values_it[2] == Color::Blue);
  values_it += 1;
  assert(*values_it == Color::Green);

  std::size_t visited_colors = 0;
  enchantum::for_each<Color>([&visited_colors](const auto color) {
    assert(enchantum::contains(color.value));
    ++visited_colors;
  });
  assert(visited_colors == enchantum::count<Color>);

  enchantum::array<Color, int> weights{};
  weights[Color::Red]   = 10;
  weights[Color::Green] = 20;
  weights[Color::Blue]  = 30;
  assert(weights.size() == enchantum::count<Color>);
  assert(weights.at(Color::Blue) == 30);

  enchantum::bitset<Color> selected_colors{Color::Red, Color::Blue};
  assert(selected_colors.test(Color::Red));
  assert(!selected_colors.test(Color::Green));
  assert(selected_colors.to_string() == "Red|Blue");
  selected_colors.flip(Color::Green);
  assert(selected_colors.to_string('.') == "Red.Green.Blue");
  selected_colors[Color::Blue] = false;
  assert(!selected_colors.test(Color::Blue));

  const auto next = enchantum::next_value(Color::Red);
  assert(next && *next == Color::Green);
  const auto prev = enchantum::prev_value(Color::Blue);
  assert(prev && *prev == Color::Green);
  assert(!enchantum::prev_value(Color::Red));
  assert(enchantum::next_value_circular(Color::Blue) == Color::Red);
  assert(enchantum::prev_value_circular(Color::Red) == Color::Blue);

  static_assert(enchantum::is_bitflag<Permission>, "Permission is detected as a bitflag enum");
  static_assert(enchantum::has_zero_flag<Permission>, "Permission has an explicit zero flag");
  assert(enchantum::contains_bitflag(Permission::Read | Permission::Write));
  assert(enchantum::contains_bitflag(Permission::None));
  assert((enchantum::contains_bitflag<Permission>(enchantum::string_view("Read|Execute", 12))));

  const std::string permission_name = enchantum::to_string_bitflag(Permission::Read | Permission::Execute);
  assert(permission_name == "Read|Execute");
  assert(enchantum::to_string_bitflag(Permission::None) == "None");
  assert(enchantum::to_string_bitflag(Permission::Read | Permission::Write, ',') == "Read,Write");

  const auto permission = enchantum::cast_bitflag<Permission>(enchantum::string_view("Read|Write", 10));
  assert(permission && *permission == (Permission::Read | Permission::Write));
  const auto permission_none = enchantum::cast_bitflag<Permission>(0);
  assert(permission_none && *permission_none == Permission::None);

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
