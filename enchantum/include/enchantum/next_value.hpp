#pragma once

#include "common.hpp"
#include "enchantum.hpp"
#include <cstddef>

#ifndef ENCHANTUM_ALIAS_OPTIONAL
  #include <optional>
#endif
namespace enchantum {

#ifdef ENCHANTUM_ALIAS_OPTIONAL
ENCHANTUM_ALIAS_OPTIONAL;
#else
using std::optional;
#endif
} // namespace enchantum

namespace enchantum {

template<Enum E>
[[nodiscard]] constexpr optional<E> next_value(E value, std::ptrdiff_t n = 1) noexcept
{
  if (!enchantum::contains(value))
    return optional<E>{};

  const auto i     = enchantum::enum_to_index<E>(value);
  const auto index = static_cast<std::ptrdiff_t>(i) + n;
  if (index >= 0 && index < static_cast<std::ptrdiff_t>(count<E>))
    return optional<E>{values<E>[static_cast<std::size_t>(index)]};
  return optional<E>{};
}

template<Enum E>
[[nodiscard]] constexpr E next_value_circular(E value, std::ptrdiff_t n = 1) noexcept
{
  ENCHANTUM_ASSERT(enchantum::contains(value), "next/prev_value_circular requires 'value' to be a valid enum member", value);

  const auto     i    = static_cast<std::ptrdiff_t>(enchantum::enum_to_index<E>(value));
  constexpr auto count = static_cast<std::ptrdiff_t>(enchantum::count<E>);
  std::ptrdiff_t next = ((i + n) % count + count) % count; // handles wrap around and negative n
  return values<E>[static_cast<std::size_t>(next)];
}

template<Enum E>
[[nodiscard]] constexpr optional<E> prev_value(E value, std::ptrdiff_t n = 1) noexcept
{
  return enchantum::next_value(value, -n);
}

template<Enum E>
[[nodiscard]] constexpr E prev_value_circular(E value, std::ptrdiff_t n = 1) noexcept
{
  return enchantum::next_value_circular(value, -n);
}

} // namespace enchantum
