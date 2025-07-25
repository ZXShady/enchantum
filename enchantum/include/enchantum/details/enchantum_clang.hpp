#pragma once

// Clang <= 12 outputs "NUMBER" if casting
// Clang > 12 outputs "(E)NUMBER".

#if defined __has_warning 
  #if __has_warning("-Wenum-constexpr-conversion")
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wenum-constexpr-conversion"
  #endif
#endif

#include "../common.hpp"
#include "../type_name.hpp"
#include "generate_arrays.hpp"
#include "string_view.hpp"
#include <array>
#include <cassert>
#include <climits>
#include <cstdint>
#include <type_traits>
#include <utility>

namespace enchantum {

namespace details {
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
    // "auto enchantum::details::var_name() [Vs = <(A)0, a, b, c, e, d, (A)6>]"
    constexpr auto funcsig_off = SZC("auto enchantum::details::var_name() [Vs = <");
    return string_view(__PRETTY_FUNCTION__ + funcsig_off, SZC(__PRETTY_FUNCTION__) - funcsig_off - SZC(">]"));
  }

#if __clang_major__ <= 11
  template<char... Chars>
  inline constexpr auto static_storage_for_chars = std::array<char, sizeof...(Chars)>{Chars...};
#else
  template<auto Copy>
  inline constexpr auto static_storage_for = Copy;
#endif
  template<typename E, bool NullTerminated>
  constexpr auto reflect() noexcept
  {
    constexpr auto Min  = enum_traits<E>::min;
    constexpr auto Max  = enum_traits<E>::max;
    constexpr auto bits = []() {
#if __clang_major__ < 20
      return (sizeof(E) * CHAR_BIT) - std::is_signed_v<E>;
#else
      if constexpr (EnumFixedUnderlying<E>) {
        return (sizeof(E) * CHAR_BIT) - std::is_signed_v<E>;
      }
      else {
        auto        v = Max;
        std::size_t r = 1;
        while (v >>= 1)
          r++;
        return r;
      }
#endif
    }();
    constexpr auto elements = []() {
      using Under      = std::underlying_type_t<E>;
      using Underlying = std::make_unsigned_t<std::conditional_t<std::is_same_v<bool, Under>, unsigned char, Under>>;
      constexpr auto ArraySize = is_bitflag<E> ? 1 + bits : (Max - Min) + 1;
      constexpr auto ConstStr  = []<std::size_t... Idx>(std::index_sequence<Idx...>) {
        // dummy 0
        if constexpr (sizeof...(Idx) && is_bitflag<E>) // sizeof... to make contest dependant
          return details::var_name<E{}, static_cast<E>(Underlying(1) << Idx)..., 0>();
        else
          return details::var_name<static_cast<E>(static_cast<decltype(Min)>(Idx) + Min)..., 0>();
      }(std::make_index_sequence<is_bitflag<E> ? bits : ArraySize>());

      const auto* str = ConstStr.data();

      constexpr auto enum_in_array_name = details::enum_in_array_name(raw_type_name<E>, ScopedEnum<E>);
      constexpr auto enum_in_array_len  = enum_in_array_name.size();
      // Ubuntu Clang 20 complains about using local constexpr variables in a local struct
      using CharArray = char[ConstStr.size() + (NullTerminated * ArraySize)];
      struct RetVal {
        E values[ArraySize]{};

        // We are making an assumption that no sane user will use an enum member name longer than 256 characters
        // if you are not sane then I don't know what to do
        std::uint8_t string_lengths[ArraySize]{};

        CharArray   strings{};
        std::size_t total_string_length = 0;
        std::size_t valid_count         = 0;
      } ret;


// ((anonymous namespace)::A)0
// (anonymous namespace)::a

// this is needed to determine whether the above are cast expression if 2 braces are
// next to eachother then it is a cast but only for anonymoused namespaced enums
#if __clang_major__ > 12
      constexpr std::size_t index_check = !enum_in_array_name.empty() && enum_in_array_name.front() == '(' ? 1 : 0;
#endif
      for (std::size_t index = 0; index < ArraySize; ++index) {
#if __clang_major__ > 12
        // check if cast (starts with '(')
        if (str[index_check] == '(')
#else
        // check if it is a number or negative sign
        if (str[0] == '-' || (str[0] >= '0' && str[0] <= '9'))
#endif
        {
#if __clang_major__ > 12
          str += SZC("(") + enum_in_array_len + SZC(")0"); // there is atleast 1 base 10 digit
#endif

          // https://clang.llvm.org/docs/LanguageExtensions.html#string-builtins
          //char* __builtin_char_memchr(const char* haystack, int needle, size_t size);
          str = __builtin_char_memchr(str, ',', UINT8_MAX) + SZC(", ");
        }
        else {
          if constexpr (enum_in_array_len != 0)
            str += enum_in_array_len + SZC("::");

          if constexpr (details::prefix_length_or_zero<E> != 0)
            str += details::prefix_length_or_zero<E>;

          const auto name_size = static_cast<std::uint8_t>(__builtin_char_memchr(str, ',', UINT8_MAX) - str);
          {
            if constexpr (is_bitflag<E>)
              ret.values[ret.valid_count] = index == 0 ? E() : static_cast<E>(Underlying{1} << (index - 1));
            else
              ret.values[ret.valid_count] = static_cast<E>(Min + static_cast<decltype(Min)>(index));

            ret.string_lengths[ret.valid_count++] = name_size;
            __builtin_memcpy(ret.strings + ret.total_string_length, str, name_size);
            ret.total_string_length += name_size + NullTerminated;
          }
          str += name_size + SZC(", ");
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
    __builtin_memcpy(ret.values.data(), elements.values, sizeof(ret.values));

    // intentionally >= 12, clang 11 does not support class non type template parameters
#if __clang_major__ >= 12
    constexpr auto strings = [](const auto total_length, const char* data) {
      std::array<char, total_length.value> strings;
      __builtin_memcpy(strings.data(), data, total_length.value);
      return strings;
    }(std::integral_constant<std::size_t, elements.total_string_length>{}, elements.strings);
    ret.strings = static_storage_for<strings>.data();
#else
    ret.strings = [elements]<std::size_t... Idx>(std::index_sequence<Idx...>) {
      return static_storage_for_chars<elements.strings[Idx]...>.data();
    }(std::make_index_sequence<elements.total_string_length>{});
#endif

    auto* const      string_indices_data = ret.string_indices.data();
    std::size_t      i                   = 0;
    StringLengthType string_index        = 0;
    for (; i < elements.valid_count; ++i) {
      string_indices_data[i] = string_index;
      string_index += elements.string_lengths[i] + NullTerminated;
    }
    ret.string_indices[i] = string_index;
    return ret;
  } // namespace details

} // namespace details


//template<Enum E>
//constexpr std::size_t enum_count = details::enum_count<E>;


} // namespace enchantum

#if defined __has_warning
  #if __has_warning("-Wenum-constexpr-conversion")
    #pragma clang diagnostic pop
  #endif
#endif
#undef SZC