#pragma once
#include "../common.hpp"
#include <array>
#include <climits>
#include <cstddef>
#include <type_traits>

namespace enchantum::details {


template<typename E, typename = void>
inline constexpr std::size_t prefix_length_or_zero = 0;

template<typename E>
inline constexpr auto prefix_length_or_zero<E, decltype((void)enum_traits<E>::prefix_length)> = std::size_t{
  enum_traits<E>::prefix_length};

template<typename Enum, auto Min, decltype(Min) Max>
constexpr auto generate_arrays()
{
  if constexpr (is_bitflag<Enum>) {
    constexpr std::size_t                             bits = sizeof(Enum) * CHAR_BIT;
    std::array<Enum, bits + 1 + 1 /*1 here to make*/> ret{}; // 0 value reflected
    for (std::size_t i = 0; i < bits; ++i)
      ret[i + 1] = static_cast<Enum>(static_cast<std::make_unsigned_t<std::underlying_type_t<Enum>>>(1) << i);

    return ret;
  }
  else {
    static_assert(Min < Max, "enum_traits::min must be less than enum_traits::max");
    std::array<Enum, (Max - Min) + 1 + 1 /*+ 1 here to make */> array{};
    auto* const                                                 array_data = array.data();
    for (std::size_t i = 0, size = Max - Min + 1; i < size; ++i)
      array_data[i] = static_cast<Enum>(static_cast<decltype(Min)>(i) + Min);
    return array;
  }
}

} // namespace enchantum::details