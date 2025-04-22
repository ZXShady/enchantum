#pragma once

#if defined(_MSC_VER)
  #include "enchantum_msvc.hpp"
#endif

#include "common.hpp"

namespace enchantum {

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
constexpr auto min = entries<E>.front().first;

template<Enum E>
constexpr auto max = entries<E>.back().first;

template<Enum E>
constexpr std::size_t count = entries<E>.size();


template<Enum E>
[[nodiscard]] constexpr std::string_view to_string(E value) noexcept
{
  for (const auto& [e, s] : entries<E>)
    if (value == e)
      return s;
  return std::string_view();
}

template<ContiguousEnum E>
[[nodiscard]] constexpr std::string_view to_string(E value) noexcept
{
  using T          = std::underlying_type_t<E>;
  const auto index = std::size_t(static_cast<T>(value) - static_cast<T>(min<E>));
  if (index < entries<E>.size())
    return entries<E>[index].second;
  return "";
}

template<Enum E>
inline constexpr auto values = []() {
  constexpr auto&             enums = entries<E>;
  std::array<E, enums.size()> ret;
  for (std::size_t i = 0; i < ret.size(); ++i)
    ret[i] = enums[i].first;
  return ret;
}();

template<Enum E, typename String = std::string_view>
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
  constexpr auto& enums = entries<E>;
  for (auto& [e, _] : enums)
    if (e == value)
      return true;
  return false;
}

template<Enum E>
[[nodiscard]] constexpr bool contains(std::underlying_type_t<E> value) noexcept
{
  return contains(static_cast<E>(value));
}


template<Enum E>
[[nodiscard]] constexpr bool contains(std::string_view name) noexcept
{
  constexpr auto& enums = entries<E>;
  for (auto& [_, s] : enums)
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
}

template<ContiguousEnum E>
[[nodiscard]] constexpr std::size_t enum_to_index(E e) noexcept
{
  using T = std::underlying_type_t<E>;
  if (enchantum::contains(e))
    return std::size_t(T(e) + T(min<E>));
  ENCHANTUM_ASSERT(false, "invalid enum passed to `enum_to_index` ", e);
}

} // namespace enchantum