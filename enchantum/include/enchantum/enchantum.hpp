#pragma once

#include "details/string_view.hpp"

#if defined(__clang__)
  #include "enchantum_clang.hpp"
#elif defined(__GNUC__) || defined(__GNUG__)
  #include "enchantum_gcc.hpp"
#elif defined(_MSC_VER)
  #include "enchantum_msvc.hpp"
#endif

#include "common.hpp"
#include <type_traits>

#ifndef ENCHANTUM_ALIAS_STRING_VIEW
  #include <string_view>
#endif


namespace enchantum {
#ifdef ENCHANTUM_ALIAS_STRING_VIEW
ENCHANTUM_ALIAS_STRING_VIEW;
#else
using ::std::string_view;
#endif


template<Enum E, typename Pair = std::pair<E, string_view>>
inline constexpr auto entries = details::reflect<E, Pair, enum_traits<E>::min, enum_traits<E>::max>();

template<Enum E>
inline constexpr bool is_contiguous = []() {
  constexpr auto& enums = entries<E>;
  using T               = std::underlying_type_t<E>;
  for (std::size_t i = 0; i < enums.size() - 1; ++i)
    if (T(enums[i].first) + 1 != T(enums[i + 1].first))
      return false;
  return true;
}();


template<EnumOfUnderlying<bool> E>
inline constexpr bool is_contiguous<E> = entries<E>.size() == 2;

template<typename E>
concept ContiguousEnum = Enum<E> && is_contiguous<E>;


template<Enum E>
inline constexpr bool is_contiguous_bitflag = []() {
  if constexpr (!BitFlagEnum<E>) {
    return false;
  }
  else {
    constexpr auto& enums = entries<E>;
    using T               = std::underlying_type_t<E>;
    for (std::size_t i = 0; i < enums.size() - 1; ++i)
      if (T(enums[i].first) << 1 != T(enums[i + 1].first))
        return false;
    return true;
  }
}();

template<typename E>
concept ContiguousBitFlagEnum = BitFlagEnum<E> && is_contiguous_bitflag<E>;


template<Enum E>
inline constexpr auto min = entries<E>.front().first;

template<Enum E>
inline constexpr auto max = entries<E>.back().first;

template<Enum E>
inline constexpr std::size_t count = entries<E>.size();


template<typename String = string_view, Enum E>
[[nodiscard]] constexpr String to_string(E value) noexcept
{
  for (const auto& [e, s] : entries<E, std::pair<E, String>>)
    if (value == e)
      return s;
  return String();
}

template<typename String = string_view, ContiguousEnum E>
[[nodiscard]] constexpr String to_string(E value) noexcept
{
  using T          = std::underlying_type_t<E>;
  const auto index = std::size_t(static_cast<T>(value) - static_cast<T>(min<E>));
  if (index < entries<E, std::pair<E, String>>.size())
    return entries<E>[index].second;
  return String();
}

template<Enum E>
inline constexpr auto values = []() {
  constexpr auto&             enums = entries<E>;
  std::array<E, enums.size()> ret;
  for (std::size_t i = 0; i < ret.size(); ++i)
    ret[i] = enums[i].first;
  return ret;
}();

template<Enum E, typename String = string_view>
inline constexpr auto names = []() {
  constexpr auto&                  enums = entries<E, std::pair<E, String>>;
  std::array<String, enums.size()> ret;
  for (std::size_t i = 0; i < ret.size(); ++i)
    ret[i] = enums[i].second;
  return ret;
}();


template<Enum E>
[[nodiscard]] constexpr bool contains(E value) noexcept
{
  for (const auto v : values<E>)
    if (v == value)
      return true;
  return false;
}

template<Enum E>
[[nodiscard]] constexpr bool contains(std::underlying_type_t<E> value) noexcept
{
  return enchantum::contains(static_cast<E>(value));
}


template<Enum E>
[[nodiscard]] constexpr bool contains(std::string_view name) noexcept
{
  for (const auto& s : names<E>)
    if (s == name)
      return true;
  return false;
}

template<ContiguousEnum E>
[[nodiscard]] constexpr bool contains(E value) noexcept
{
  using T = std::underlying_type_t<E>;
  return T(value) <= T(max<E>) && T(value) >= T(min<E>);
}

template<Enum E>
[[nodiscard]] constexpr E index_to_enum(std::size_t index) noexcept
{
  ENCHANTUM_ASSERT(index < values<E>.size(), "'index' must be less than the enums size!", index);
  return values<E>[index];
}

template<Enum E>
[[nodiscard]] constexpr std::size_t enum_to_index(E e) noexcept
{
  std::size_t i = 0;
  for (const E val : values<E>) {
    if (val == e)
      return i;
    ++i;
  }
  ENCHANTUM_ASSERT(false, "invalid enum passed to `enum_to_index` ", e);
  return std::size_t(-1); 
}

template<ContiguousEnum E>
[[nodiscard]] constexpr std::size_t enum_to_index(E e) noexcept
{
  using T = std::underlying_type_t<E>;
  if (enchantum::contains(e))
    return std::size_t(T(e) + T(min<E>));
  ENCHANTUM_ASSERT(false, "invalid enum passed to `enum_to_index` ", e);
  return std::size_t(-1);
}

} // namespace enchantum