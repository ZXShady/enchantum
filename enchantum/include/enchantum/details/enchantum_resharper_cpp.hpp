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

  // WORKAROUND
  // resharper seems to not copy values of arrays correctly in constexpr contexts.
  // it copies the last element of the array to the WHOLE array
  // giving the array a default value other than default-init fixes the issue
  // as for why 'Count' is explicitly taken although it is equal to sizeof...(Is)
  // is to workaround another bug, which seems to think sizeof...(Is) is 0
  template<std::size_t Count, typename Value, std::size_t... Is>
  constexpr auto rscpp_make_defaulted_array_of(const Value value, std::index_sequence<Is...>)
  {
    return std::array<Value, Count>{(Is, void(), value)...};
  }


  template<typename E, bool NullTerminated, auto Min, std::size_t... Is>
  constexpr auto reflect(std::index_sequence<Is...>) noexcept
  {
    using MinT = decltype(Min);
    using T    = std::underlying_type_t<E>;
    using U    = std::make_unsigned_t<std::conditional_t<std::is_same_v<bool, T>, unsigned char, T>>;
    constexpr bool        IsBitFlag    = is_bitflag<E>;
    constexpr std::size_t max_elements = sizeof...(Is) + IsBitFlag;

    constexpr auto elements_local = [] {
      const char* names[max_elements]{};
      T           values[max_elements]{};
      std::size_t count = 0;

      if constexpr (IsBitFlag) {
        if (const auto* name = __rscpp_enumerator_name(E(0))) {
          names[count]    = name;
          values[count++] = 0;
        }

        for (std::size_t i = 0; i < max_elements; ++i) {
          const auto val  = T(U(1) << i);
          const auto name = __rscpp_enumerator_name(E(val));
          if (name) {
            names[count]    = name;
            values[count++] = val;
          }
        }
      }
      else {
        for (std::size_t i = 0; i < max_elements; ++i) {
          const auto val  = T(MinT(i) + Min);
          const auto name = __rscpp_enumerator_name(E(val));
          if (name) {
            names[count]    = name;
            values[count++] = val;
          }
        }
      }

      auto ret = ReflectStringReturnValue<T, max_elements>{};
      for (std::size_t i = 0; i < count; ++i) {
        const auto        str = names[i] + prefix_length_or_zero<E>;
        const std::size_t len = __builtin_strlen(str);
        ret.values[i]         = values[i];
        ret.string_lengths[i] = len;
        for (std::size_t j = 0; j < len; ++j)
          ret.strings[ret.total_string_length + j] = str[j];
        ret.total_string_length += len + (NullTerminated ? 1 : 0);
      }
      ret.valid_count = count;
      return ret;
    }();

    using Strings = std::array<char, elements_local.total_string_length>;

    struct {
      decltype(elements_local) elements;
      Strings                  strings{};
    } data = {elements_local};

    const auto size = data.strings.size();
    const auto str  = data.strings.data();
    for (std::size_t i = 0; i < size; ++i)
      str[i] = elements_local.strings[i];
    return data;
  }

} // namespace details
} // namespace enchantum