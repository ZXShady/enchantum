#pragma once
#include "cast.hpp"
#include "common.hpp"
#include "details/string_view.hpp"
#include "enchantum.hpp"

#ifndef ENCHANTUM_ALIAS_STRING
  #include <string>
#endif

namespace enchantum {
#ifdef ENCHANTUM_ALIAS_STRING
ENCHANTUM_ALIAS_STRING;
#else
using ::std::string;
#endif
} // namespace enchantum


namespace enchantum {


template<BitFlagEnum E>
inline constexpr E values_ors = [] {
  E ret{};
  for (const auto val : values<E>)
    ret |= val;
  return ret;
}();

template<BitFlagEnum E>
constexpr bool contains_bitflag(E value) noexcept
{
  using T = std::underlying_type_t<E>;
  T check_value{0};
  for (const auto v : values<E>)
    if (v == (static_cast<T>(value) & static_cast<T>(v)))
      check_value |= v;
  return check_value != 0 && check_value == value;
}

template<ContiguousBitFlagEnum E>
constexpr bool contains_bitflag(E value) noexcept
{
  using T = std::underlying_type_t<E>;
  return static_cast<T>(value) >= static_cast<T>(min<E>) && static_cast<T>(value) <= static_cast<T>(max<E>);
}

template<BitFlagEnum E>
[[nodiscard]] constexpr string to_string_bitflag(E value, char sep = '|')
{
  using T = std::underlying_type_t<E>;
  string name;
  T      check_value = 0;
  for (const auto& [v, s] : entries<E, std::pair<E, string_view>>) {
    if (v == (value & v)) {
      if (!name.empty())
        name.append(1, sep);           // append separator if not the first value
      name.append(s.data(), s.size()); // not using operator += since this may not be std::string_view always
      check_value |= static_cast<T>(v);
    }
  }
  // If all bits in the value are accounted for, return the name string
  if (check_value == static_cast<T>(value))
    return name;
  // Return empty string if invalid or out-of-range bit flags are present
  return string{};
}

template<BitFlagEnum E, std::predicate<string_view, string_view> BinaryPred>
[[nodiscard]] constexpr optional<E> cast_bitflag(string_view s, char sep, BinaryPred binary_pred) noexcept
{
  using T = std::underlying_type_t<E>;
  T           check_value{};
  std::size_t pos = 0;
  for (std::size_t i = s.find(sep); i != s.npos; i = s.find(sep, pos)) {
    if (const auto v = enchantum::cast<E>(s.substr(pos, i - pos)))
      check_value |= static_cast<T>(*v);
    else
      return optional<E>();
    pos = i + 1;
  }

  if (const auto v = enchantum::cast<E>(s.substr(pos), binary_pred))
    check_value |= static_cast<T>(*v); // Combine the last bit flag
  else
    return optional<E>{}; // Invalid value, return empty optional

  return optional<E>{static_cast<E>(check_value)};
}

template<BitFlagEnum E>
[[nodiscard]] constexpr optional<E> cast_bitflag(string_view s, char sep = '|') noexcept
{
  return enchantum::cast_bitflag<E>(s, sep, [](const auto& a, const auto& b) { return a == b; });
}

template<BitFlagEnum E>
[[nodiscard]] constexpr optional<E> cast_bitflag(E value) noexcept
{
  using T          = std::underlying_type_t<E>;
  auto check_value = T{0};
  for (const auto v : values<E>)
    if (v == (static_cast<T>(value) & static_cast<T>(v)))
      check_value |= v;

  if (check_value != 0 && check_value == value)
    return optional<E>(static_cast<T>(value));
  return optional<E>{}; // Invalid value or out of range.
}

} // namespace enchantum