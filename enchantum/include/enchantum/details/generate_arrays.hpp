#pragma once
#include "../common.hpp"
#include <type_traits>

namespace enchantum::details {
template<typename Enum, auto Min, decltype(Min) Max>
constexpr auto generate_arrays()
{

  if constexpr (BitFlagEnum<Enum>) {
    constexpr std::size_t      bits = sizeof(Enum) * CHAR_BIT;
    std::array<Enum, bits> a{}; // 0 value reflected
    for (std::size_t i = 0; i < bits; ++i)
      a[i] = static_cast<Enum>(static_cast<std::make_unsigned_t<std::underlying_type_t<Enum>>>(1) << i);
    return a;
  }
  else {
    static_assert(Min < Max, "enum_traits::min must be less than enum_traits::max");
    std::array<Enum, (Max - Min) + 1> array;
    auto* const                       array_data = array.data();
    for (std::size_t i = 0,size = array.size(); i < size; ++i)
      array_data[i] = static_cast<Enum>(static_cast<decltype(Min)>(i) + Min);
    return array;
  }
}
} // namespace enchantum::details