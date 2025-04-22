#pragma once

#include "common.hpp"
#include <type_traits>
/*
Note this header is an extremely easy way to cause ODR issues.

class Flags { F1 = 1 << 0,F2 = 1<< 1};
// **note I did not define any operators**

enchantum::contains(Flags::F1); // considered a classical `Enum` concept 

using namespace enchantum::bitwise_operators;

enchantum::contains(Flags::F1); // considered `BitFlagEnum` concept woops! ODR! 

*/

namespace enchantum::bitwise_operators {
template<Enum E>
[[nodiscard]] constexpr E operator~(E e) noexcept
{
  return static_cast<E>(~static_cast<std::underlying_type_t<E>>(e));
}

template<Enum E>
[[nodiscard]] constexpr E operator|(E a, E b) noexcept
{
  using T = std::underlying_type_t<E>;
  return static_cast<E>(static_cast<T>(a) | static_cast<T>(b));
}

template<Enum E>
[[nodiscard]] constexpr E operator&(E a, E b) noexcept
{
  using T = std::underlying_type_t<E>;
  return static_cast<E>(static_cast<T>(a) & static_cast<T>(b));
}

template<Enum E>
[[nodiscard]] constexpr E operator^(E a, E b) noexcept
{
  using T = std::underlying_type_t<E>;
  return static_cast<E>(static_cast<T>(a) ^ static_cast<T>(b));
}

template<Enum E>
[[nodiscard]] constexpr E& operator|=(E& a, E b) noexcept
{
  using T  = std::underlying_type_t<E>;
  return a = static_cast<E>(static_cast<T>(a) | static_cast<T>(b));
}

template<Enum E>
[[nodiscard]] constexpr E& operator&=(E& a, E b) noexcept
{
  using T  = std::underlying_type_t<E>;
  return a = static_cast<E>(static_cast<T>(a) & static_cast<T>(b));
}

template<Enum E>
[[nodiscard]] constexpr E& operator^=(E& a, E b) noexcept
{
  using T  = std::underlying_type_t<E>;
  return a = static_cast<E>(static_cast<T>(a) ^ static_cast<T>(b));
}

} // namespace enchantum::bitwise_operators
