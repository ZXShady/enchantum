#pragma once

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
  constexpr auto count = static_cast<std::ptrdiff_t>(count<E>);
  if (!enchantum::contains(value))
    return optional<E>{};

  const auto i     = enchantum::enum_to_index<E>(value);
  const auto index = static_cast<std::ptrdiff_t>(*i) + n;
  if (index >= 0 && index < count)
    return optional<E>{values<E>[static_cast<std::size_t>(index)]};
  return optional<E>{};
}

template<Enum E>
[[nodiscard]] constexpr optional<E> next_value_circular(E value, std::ptrdiff_t n = 1) noexcept
{
  if (!enchantum::contains(value))
    return optional<E>{};

  constexpr auto count = static_cast<std::ptrdiff_t>(count<E>);

  const auto     i    = enchantum::enum_to_index<E>(value);
  std::ptrdiff_t next = ((*i + n) % count + count) % count; // handles wrap around and negative n
  return optional<E>{values<E>[static_cast<std::size_t>(next)]};
}

template<Enum E>
[[nodiscard]] constexpr optional<E> prev_value(E value, std::ptrdiff_t n = 1) noexcept
{
  return enchantum::next_value(value, -n);
}

template<Enum E>
[[nodiscard]] constexpr optional<E> prev_value_circular(E value, std::ptrdiff_t n = 1) noexcept
{
  return enchantum::next_value_circular(value, -n);
}

} // namespace enchantum