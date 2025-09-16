#pragma once

#include "../common.hpp"
#include "shared.hpp"
#include "string_view.hpp"
#include <array>
#include <cstdint>
#include <type_traits>
#include <utility>

namespace enchantum {
namespace details {

  template<bool IsBitFlag, typename IntType>
    constexpr void parse_string(
      const char*const*         strs,
      const IntType       min,
      const std::size_t   array_size,
      const bool          null_terminated,
      IntType* const      values,
      std::uint8_t* const string_lengths,
      char* const         strings,
      std::size_t&        total_string_length,
      std::size_t&        valid_count)
  {
    for (std::size_t index = 0; index < array_size; ++index) {
      if (const auto* str = strs[index])
      {
        if constexpr (IsBitFlag)
          values[valid_count] = index == 0 ? IntType{} : static_cast<IntType>(IntType{1} << (index - 1));
        else
          values[valid_count] = static_cast<IntType>(min + static_cast<IntType>(index));
        const auto len = __builtin_strlen(str);
        string_lengths[valid_count++] = len;
        for (std::size_t i = 0; i < len; ++i)
          strings[total_string_length + i] = str[i];
        total_string_length += len + null_terminated;
      }
    }
  }

  template<typename E, bool NullTerminated, auto Min, std::size_t... Is>
  constexpr auto reflect(std::index_sequence<Is...>) noexcept
  {
    using MinT       = decltype(Min);
    using T          = std::underlying_type_t<E>;
    using Underlying = std::make_unsigned_t<std::conditional_t<std::is_same_v<bool, T>, unsigned char, T>>;

    constexpr auto elements_local = [](){
    const auto strs = [](auto dependant) {
      constexpr bool always_true = sizeof(dependant) != 0;
      // dummy 0
      if constexpr (always_true && is_bitflag<E>)
        return std::array<const char*,sizeof...(Is)+1>{__rscpp_enumerator_name(E(!always_true)),__rscpp_enumerator_name(static_cast<E>(Underlying(1) << Is))...};
      else
        return std::array<const char*,sizeof...(Is)+std::size_t(!always_true)>{__rscpp_enumerator_name(static_cast<E>(static_cast<MinT>(Is) + Min))...};
    }(0);

    details::ReflectStringReturnValue<std::underlying_type_t<E>, strs.size()> ret;

    details::parse_string<is_bitflag<E>>(
        /*str = */ strs.data(),
        /*min = */ static_cast<std::underlying_type_t<E>>(Min),
        /*array_size = */ strs.size(),
        /*null_terminated= */ NullTerminated,
        /*enum_values= */ ret.values,
        /*string_lengths= */ ret.string_lengths,
        /*strings= */ ret.strings,
        /*total_string_length*/ ret.total_string_length,
        /*valid_count*/ ret.valid_count);
      return ret;
    }();

    using Strings = std::array<char, elements_local.total_string_length>;

    struct {
      decltype(elements_local) elements;
      Strings                  strings{};
    } data = {elements_local};
    const auto  size        = data.strings.size();
    auto* const data_string = data.strings.data();
    for (std::size_t i = 0; i < size; ++i)
      data_string[i] = elements_local.strings[i];
    return data;
  } // namespace details

} // namespace details

} // namespace enchantum

#undef SZC