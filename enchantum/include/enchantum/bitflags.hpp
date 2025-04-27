#pragma once
#include "cast.hpp"
#include "common.hpp"
#include "enchantum.hpp"

#ifndef ENCHANTUM_ALIAS_STRING_VIEW
  #include <string_view>
#endif

#ifndef ENCHANTUM_ALIAS_STRING
  #include <string>
#endif

namespace enchantum {
#ifdef ENCHANTUM_ALIAS_STRING
ENCHANTUM_ALIAS_STRING;
#else
using ::std::basic_string;
#endif
#ifdef ENCHANTUM_ALIAS_STRING_VIEW
ENCHANTUM_ALIAS_STRING_VIEW;
#else
using ::std::basic_string_view;
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

template<typename CharType = char, BitFlagEnum E>
[[nodiscard]] basic_string<CharType> to_string_bitflag(E value, CharType sep = static_cast<CharType>('|'))
{
  using T = std::underlying_type_t<E>;
  basic_string<CharType> name;
  T                      check_value = 0;
  for (const auto& [v, s] : enchantum::entries<E>) {
    if (static_cast<T>(value) & static_cast<T>(v)) {
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
  return basic_string<CharType>{};
}

template<BitFlagEnum E, typename CharType = char>
[[nodiscard]] optional<E> cast_bitflag(basic_string_view<CharType> s, CharType sep = static_cast<CharType>('|')) noexcept
{
  using T = std::underlying_type_t<E>;
  T           check_value{};
  std::size_t pos = 0;
  for (std::size_t i = s.find(sep); i != s.npos; i = s.find(sep, pos)) {
    if (const auto v = cast<E>(s.substr(pos, i - pos)))
      check_value |= static_cast<T>(*v);
    else
      return optional<E>();
    pos = i + 1;
  }

  if (const auto v = cast<E>(s.substr(pos)))
    check_value |= static_cast<T>(*v); // Combine the last bit flag
  else
    return optional<E>{}; // Invalid value, return empty optional

  return optional<E>{static_cast<E>(check_value)};
}

template<BitFlagEnum E, typename CharType = char>
[[nodiscard]] optional<E> cast_bitflag(E e, CharType sep = static_cast<CharType>('|')) noexcept
{
  using T          = std::underlying_type_t<E>;
  auto check_value = T{0};
  for (const auto v : values<E>)
    if (static_cast<T>(value) & static_cast<T>(v))
      check_value |= v;

  if (check_value != 0 && check_value == value)
    return optional<E>(static_cast<T>(value));
  return optional<E>{}; // Invalid value or out of range.
}

} // namespace enchantum