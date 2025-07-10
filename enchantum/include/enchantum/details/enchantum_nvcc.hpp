#include <array>
#include <cassert>
#include <climits>
#include <cstdint>
#include <type_traits>
#include <utility>
#include "../common.hpp"
#include "../type_name.hpp"
#include "generate_arrays.hpp"
namespace enchantum {

namespace details {
  constexpr std::size_t find_semicolon(const char* const s)
  {
    for (std::size_t i = 0; true; ++i)
      if (s[i] == ';')
        return i;
  }
  constexpr auto enum_in_array_name(const std::string_view raw_type_name, const bool is_scoped_enum) noexcept
  {
    if (is_scoped_enum)
      return raw_type_name;

    if (const auto pos = raw_type_name.rfind(':'); pos != string_view::npos)
      return raw_type_name.substr(0, pos - 1);
    return string_view();
  }

#define SZC(x) (sizeof(x) - 1)

  template<auto... Vs>
  constexpr auto var_name() noexcept
  {
    constexpr auto funcsig_off = SZC("constexpr auto enchantum::details::var_name() noexcept [with _ Vs = _{}; ");
    return string_view(__PRETTY_FUNCTION__ + funcsig_off, SZC(__PRETTY_FUNCTION__) - funcsig_off - SZC(" _ Vs = 0]"));
  }

  template<auto Copy>
  inline constexpr auto static_storage_for = Copy;

  template<typename E, std::size_t Length, std::size_t StringLength>
  struct ReflectRetVal {
    E values[Length]{};

    // We are making an assumption that no sane user will use an enum member name longer than 256 characters
    // if you are not sane then I don't know what to do
    std::uint8_t string_lengths[Length]{};

    char        strings[StringLength]{};
    std::size_t total_string_length = 0;
    std::size_t valid_count         = 0;
  };

  template<typename E, bool NullTerminated>
  constexpr auto reflect() noexcept
  {
    constexpr auto Min      = enum_traits<E>::min;
    constexpr auto Max      = enum_traits<E>::max;
    constexpr auto bits     = (sizeof(E) * CHAR_BIT) - std::is_signed_v<E>;
    constexpr auto elements = []() {
      using Under      = std::underlying_type_t<E>;
      using Underlying = std::make_unsigned_t<std::conditional_t<std::is_same_v<bool, Under>, unsigned char, Under>>;
      constexpr auto ArraySize = is_bitflag<E> ? 1 + bits : (Max - Min) + 1;

// NVCC seems to not consider else branches of if constexpr as always returning so I have to
// disable this warning
#pragma diag_suppress implicit_return_from_non_void_function
      constexpr auto ConstStr = []<std::size_t... Idx>(std::index_sequence<Idx...>) -> std::string_view {
        // dummy 0
        constexpr struct _ {
        } A;                                           // forces NVCC to shorten the string types
        if constexpr (sizeof...(Idx) && is_bitflag<E>) // sizeof... to make contest dependant
          return details::var_name<A, static_cast<E>(0), static_cast<E>(Underlying(1) << Idx)..., 0>();
        else
          return details::var_name<A, static_cast<E>(static_cast<decltype(Min)>(Idx) + Min)..., 0>();
      }(std::make_index_sequence<is_bitflag<E> ? bits : ArraySize>());
#pragma diag_default implicit_return_from_non_void_function

      const auto* str = ConstStr.data();

      constexpr auto enum_in_array_name = details::enum_in_array_name(raw_type_name<E>, ScopedEnum<E>);
      constexpr auto enum_in_array_len  = enum_in_array_name.size();
      ReflectRetVal<E, ArraySize, ConstStr.size() + (NullTerminated * ArraySize)> ret;

      for (std::size_t index = 0; index < ArraySize; ++index) {
        str += SZC("_ Vs = ");
        // check if cast (starts with '(')
        if (*str == '(') {
          str += SZC("(") + enum_in_array_len + SZC(")0"); // there is atleast 1 base 10 digit
          str += details::find_semicolon(str) + SZC("; ");
        }
        else {
          if constexpr (enum_in_array_len != 0)
            str += enum_in_array_len + SZC("::");

          if constexpr (details::prefix_length_or_zero<E> != 0)
            str += details::prefix_length_or_zero<E>;

          const auto name_size = details::find_semicolon(str);
          {
            if constexpr (is_bitflag<E>)
              ret.values[ret.valid_count] = {index == 0 ? E() : E(Underlying{1} << (index - 1))};
            else
              ret.values[ret.valid_count] = {E(Min + static_cast<decltype(Min)>(index))};

            ret.string_lengths[ret.valid_count++] = name_size;
            
            for(std::size_t i =0;i<name_size;++i)
              ret.strings[i+ret.total_string_length] = str[i];
            // __builtin_memcpy(ret.strings + ret.total_string_length, str, name_size);
            ret.total_string_length += name_size + NullTerminated;
          }
          str += name_size + SZC("; ");
        }
      }
      return ret;
    }();

    using StringLengthType = std::conditional_t<(elements.total_string_length < UINT8_MAX), std::uint8_t, std::uint16_t>;

    struct RetVal {
      std::array<E, elements.valid_count> values{};
      // +1 for easier iteration on on last string
      std::array<StringLengthType, elements.valid_count + 1> string_indices{};
      const char*                                            strings{};
    } ret;
    for(std::size_t i=0;i<elements.valid_count;++i)
      ret.values[i] = elements.values[i];

    // __builtin_memcpy(ret.values.data(), elements.values, sizeof(ret.values));

    constexpr auto strings = [](const auto total_length, const char* data) {
      std::array<char, total_length.value> strings{};
      for(std::size_t i = 0;i<total_length.value;++i)
        strings[i] = data[i];
        // __builtin_memcpy(strings.data(), data, total_length.value);
      return strings;
    }(std::integral_constant<std::size_t, elements.total_string_length>{}, elements.strings);
    ret.strings = static_storage_for<strings>.data();

    auto* const      string_indices_data = ret.string_indices.data();
    std::size_t      i                   = 0;
    StringLengthType string_index        = 0;
    for (; i < elements.valid_count; ++i) {
      string_indices_data[i] = string_index;
      string_index += elements.string_lengths[i] + NullTerminated;
    }
    ret.string_indices[i] = string_index;
    return ret;
  }

} // namespace details
} // namespace enchantum