#pragma once

#include "details/string_view.hpp"

#if defined(__clang__)
  #include "details/enchantum_clang.hpp"
#elif defined(__GNUC__) || defined(__GNUG__)
  #include "details/enchantum_gcc.hpp"
#elif defined(_MSC_VER)
  #include "details/enchantum_msvc.hpp"
#endif

#include "common.hpp"
#include <type_traits>
#include <utility>

namespace enchantum {

#ifdef __cpp_lib_to_underlying
using ::std::to_underlying;
#else
template<Enum E>
[[nodiscard]] constexpr auto to_underlying(const E e) noexcept
{
  return static_cast<std::underlying_type_t<E>>(e);
}
#endif

template<Enum E, typename Pair = std::pair<E, string_view>>
inline constexpr auto entries = details::reflect<E, Pair, enum_traits<E>::min, enum_traits<E>::max>();

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

template<typename>
inline constexpr bool is_contiguous = false;

template<Enum E>
inline constexpr bool is_contiguous<E> = []() {
  using T = std::underlying_type_t<E>;
  if constexpr (std::is_same_v<T, bool>) {
    return true;
  }
  else {
    constexpr auto& enums = entries<E>;
    for (std::size_t i = 0; i < enums.size() - 1; ++i)
      if (T(enums[i].first) + 1 != T(enums[i + 1].first))
        return false;
    return true;
  }
}();


template<typename E>
concept ContiguousEnum = Enum<E> && is_contiguous<E>;

template<typename>
inline constexpr bool has_zero_flag = false;

template<BitFlagEnum E>
inline constexpr bool has_zero_flag<E> = []() {
  for (const auto v : values<E>)
    if (static_cast<std::underlying_type_t<E>>(v) == 0)
      return true;
  return false;
}();


template<typename>
inline constexpr bool is_contiguous_bitflag = false;

template<BitFlagEnum E>
inline constexpr bool is_contiguous_bitflag<E> = []() {
  constexpr auto& enums = entries<E>;
  using T               = std::underlying_type_t<E>;
  for (std::size_t i = 0; i < enums.size() - 1; ++i)
    if (T(enums[i].first) << 1 != T(enums[i + 1].first))
      return false;
  return true;
}();

template<typename E>
concept ContiguousBitFlagEnum = BitFlagEnum<E> && is_contiguous_bitflag<E>;


template<Enum E>
inline constexpr auto min = entries<E>.front().first;

template<Enum E>
inline constexpr auto max = entries<E>.back().first;

template<Enum E>
inline constexpr std::size_t count = entries<E>.size();


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
[[nodiscard]] constexpr bool contains(string_view name) noexcept
{
  for (const auto& s : names<E>)
    if (s == name)
      return true;
  return false;
}


template<Enum E, std::predicate<string_view, string_view> BinaryPredicate>
[[nodiscard]] constexpr bool contains(string_view name, BinaryPredicate binary_predicate) noexcept
{
  for (const auto& s : names<E>)
    if (binary_predicate(name, s))
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
  return std::size_t(-1);
}

template<ContiguousEnum E>
[[nodiscard]] constexpr std::size_t enum_to_index(E e) noexcept
{
  using T = std::underlying_type_t<E>;
  if (enchantum::contains(e))
    return std::size_t(T(e) + T(min<E>));
  return std::size_t(-1);
}

namespace details {
  struct to_string_functor {
    template<Enum E>
    [[nodiscard]] constexpr
#ifdef __cpp_static_call_operator
      static
#endif
      string_view
      operator()(const E value)
#ifndef __cpp_static_call_operator
        const
#endif
      noexcept
    {
      if (const auto i = enchantum::enum_to_index(value); i != std::size_t(-1))
        return names<E>[i];
      return string_view{};
    }
  };
} // namespace details
inline constexpr details::to_string_functor to_string;


} // namespace enchantum