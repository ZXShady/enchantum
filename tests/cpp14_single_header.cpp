#include "cpp14_config.hpp"
#include "../single_include/enchantum_single_header.hpp"

#include <cassert>
#include <cstddef>
#include <sstream>
#include <string>
#include <type_traits>

namespace single_header_tests {

enum class SingleColor { Red = 0, Blue = 1 };

enum class SinglePermission { None = 0, Read = 1, Write = 2, Execute = 4 };

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

  static_assert(std::is_enum<SingleColor>::value, "single-header test enum must be an enum");
  static_assert(enchantum::count<SingleColor> == 2, "single header reflects C++14 enums");
  static_assert(enchantum::to_string(SingleColor::Blue) == enchantum::string_view("Blue", 4), "single header to_string works");
  static_assert(enchantum::values<SingleColor>[0] == SingleColor::Red, "single header values work in C++14 constexpr context");
  static_assert(enchantum::names<SingleColor>[1] == enchantum::string_view("Blue", 4), "single header names work in C++14 constexpr context");
  static_assert(enchantum::entries<SingleColor>[1].first == SingleColor::Blue,
                "single header entries values work in C++14 constexpr context");
  static_assert(enchantum::entries<SingleColor>[1].second == enchantum::string_view("Blue", 4),
                "single header entries names work in C++14 constexpr context");
  static_assert(enchantum::names<SingleColor, enchantum::string_view, false>[0] == enchantum::string_view("Red", 3),
                "single header non-null-terminated names work in C++14 constexpr context");

  const std::string scoped_blue = enchantum::scoped::to_string(SingleColor::Blue);
  assert(scoped_blue == "SingleColor::Blue");

  assert((enchantum::contains<SingleColor>(enchantum::string_view("blue", 4), single_header_tests::case_insensitive_char_equal{})));
  assert((enchantum::contains<SingleColor>(enchantum::string_view("Blue", 4), single_header_tests::string_view_equal{})));

  const auto value = enchantum::cast<SingleColor>(enchantum::string_view("Red", 3));
  assert(value && *value == SingleColor::Red);

  const auto blue_case_insensitive =
    enchantum::cast<SingleColor>(enchantum::string_view("blue", 4), single_header_tests::case_insensitive_char_equal{});
  assert(blue_case_insensitive && *blue_case_insensitive == SingleColor::Blue);

  const auto scoped_value = enchantum::scoped::cast<SingleColor>(enchantum::string_view("SingleColor::Red", 16));
  assert(scoped_value && *scoped_value == SingleColor::Red);

  const auto blue_index = enchantum::enum_to_index(SingleColor::Blue);
  assert(blue_index && *blue_index == 1u);
  const auto blue_from_index = enchantum::index_to_enum<SingleColor>(1);
  assert(blue_from_index && *blue_from_index == SingleColor::Blue);

  assert(enchantum::values_generator<SingleColor>.size() == enchantum::count<SingleColor>);
  assert(enchantum::values_generator<SingleColor>[0] == SingleColor::Red);
  assert(enchantum::names_generator<SingleColor>[1] == enchantum::string_view("Blue", 4));
  assert((enchantum::names_generator<SingleColor, enchantum::string_view, false>[0] == enchantum::string_view("Red", 3)));
  assert(enchantum::entries_generator<SingleColor>[1].first == SingleColor::Blue);
  assert(enchantum::entries_generator<SingleColor>[1].second == enchantum::string_view("Blue", 4));

  auto values_it = enchantum::values_generator<SingleColor>.begin();
  assert(*values_it == SingleColor::Red);
  assert(values_it[1] == SingleColor::Blue);
  values_it += 1;
  assert(*values_it == SingleColor::Blue);

  std::size_t visited_colors = 0;
  enchantum::for_each<SingleColor>([&visited_colors](const auto color) {
    assert(enchantum::contains(color.value));
    ++visited_colors;
  });
  assert(visited_colors == enchantum::count<SingleColor>);

  enchantum::array<SingleColor, int> weights{};
  weights[SingleColor::Red]  = 10;
  weights[SingleColor::Blue] = 20;
  assert(weights.size() == enchantum::count<SingleColor>);
  assert(weights.at(SingleColor::Blue) == 20);

  enchantum::bitset<SingleColor> selected_colors{SingleColor::Red};
  assert(selected_colors.test(SingleColor::Red));
  assert(!selected_colors.test(SingleColor::Blue));
  assert(selected_colors.to_string() == "Red");
  selected_colors.flip(SingleColor::Blue);
  assert(selected_colors.to_string('.') == "Red.Blue");
  selected_colors[SingleColor::Red] = false;
  assert(!selected_colors.test(SingleColor::Red));

  const auto next = enchantum::next_value(SingleColor::Red);
  assert(next && *next == SingleColor::Blue);
  const auto prev = enchantum::prev_value(SingleColor::Blue);
  assert(prev && *prev == SingleColor::Red);
  assert(!enchantum::prev_value(SingleColor::Red));
  assert(enchantum::next_value_circular(SingleColor::Blue) == SingleColor::Red);
  assert(enchantum::prev_value_circular(SingleColor::Red) == SingleColor::Blue);

  static_assert(enchantum::is_bitflag<SinglePermission>, "single header detects bitflag enums");
  static_assert(enchantum::has_zero_flag<SinglePermission>, "single header detects zero bitflag value");
  assert(enchantum::contains_bitflag(SinglePermission::Read | SinglePermission::Write));
  assert(enchantum::contains_bitflag(SinglePermission::None));
  assert((enchantum::contains_bitflag<SinglePermission>(enchantum::string_view("Read|Execute", 12))));

  const std::string permission_name = enchantum::to_string_bitflag(SinglePermission::Read | SinglePermission::Execute);
  assert(permission_name == "Read|Execute");
  assert(enchantum::to_string_bitflag(SinglePermission::None) == "None");
  assert(enchantum::to_string_bitflag(SinglePermission::Read | SinglePermission::Write, ',') == "Read,Write");

  const auto permission = enchantum::cast_bitflag<SinglePermission>(enchantum::string_view("Read|Write", 10));
  assert(permission && *permission == (SinglePermission::Read | SinglePermission::Write));
  const auto permission_none = enchantum::cast_bitflag<SinglePermission>(0);
  assert(permission_none && *permission_none == SinglePermission::None);

  const std::string scoped_permission_name = enchantum::scoped::to_string_bitflag(SinglePermission::Read | SinglePermission::Execute);
  assert(scoped_permission_name == "SinglePermission::Read|SinglePermission::Execute");

  const auto scoped_permission =
    enchantum::scoped::cast_bitflag<SinglePermission>(enchantum::string_view("SinglePermission::Read|SinglePermission::Write", 46));
  assert(scoped_permission && *scoped_permission == (SinglePermission::Read | SinglePermission::Write));

  std::istringstream color_stream("Blue");
  using namespace enchantum::iostream_operators;
  SingleColor streamed_color{};
  color_stream >> streamed_color;
  assert(color_stream && streamed_color == SingleColor::Blue);

  std::istringstream permission_stream("Read|Execute");
  SinglePermission streamed_permission{};
  permission_stream >> streamed_permission;
  assert(permission_stream && streamed_permission == (SinglePermission::Read | SinglePermission::Execute));
}
