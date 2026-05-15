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
#include "shared.hpp"
#include "string_view.hpp"
#include <cassert>
#include <climits>
#include <cstdint>
#include <type_traits>
#include <utility>

namespace enchantum {

namespace details {
  constexpr auto enum_in_array_name(const string_view raw_type_name, const bool is_scoped_enum) noexcept
  {
    if (is_scoped_enum)
      return raw_type_name;

    const auto pos = raw_type_name.rfind(':');
    if (pos != string_view::npos)
      return raw_type_name.substr(0, pos - 1);
    return string_view();
  }

#define SZC(x) (sizeof(x) - 1)

  constexpr const char* find_clang_values_pack(const char* s) noexcept
  {
    for (std::size_t i = 0; true; ++i) {
      if (s[i] == 'V' && s[i + 1] == 's' && s[i + 2] == ' ' && s[i + 3] == '=' && s[i + 4] == ' ' && s[i + 5] == '<')
        return s + i + SZC("Vs = <");
    }
  }

  template<typename E, E... Vs>
  constexpr auto var_name() noexcept
  {
    // "auto enchantum::details::var_name() [Vs = <(A)0, a, b, c, e, d, (A)6>]"
    return details::find_clang_values_pack(__PRETTY_FUNCTION__);
  }


  constexpr bool is_out_of_range_parse(
    std::size_t       index_check,
    const char*       str,
    const std::size_t least_length_when_casting,
    const std::size_t array_size)
  {
    (void)index_check;
    for (std::size_t index = 0; index < array_size; ++index) {
#if __clang_major__ > 12
      // check if cast (starts with '(')
      if (str[index_check] == '(')
#else
      // check if it is a number or negative sign
      if (str[0] == '-' || (str[0] >= '0' && str[0] <= '9'))
#endif
      {
        str = __builtin_char_memchr(str + least_length_when_casting, ',', UINT8_MAX) + SZC(", ");
      }
      else {
        return true;
      }
    }
    return false;
  }


  template<bool IsBitFlag, typename IntType>
  constexpr void parse_string(
    std::size_t         index_check,
    const char*         str,
    const std::size_t   least_length_when_casting,
    const std::size_t   least_length_when_value,
    const IntType       min,
    const std::size_t   array_size,
    const bool          null_terminated,
    IntType* const      values,
    std::uint8_t* const string_lengths,
    char* const         strings,
    std::size_t&        total_string_length,
    std::size_t&        valid_count)
  {
    (void)index_check;
    for (std::size_t index = 0; index < array_size; ++index) {
#if __clang_major__ > 12
      // check if cast (starts with '(')
      if (str[index_check] == '(')
#else
      // check if it is a number or negative sign
      if (str[0] == '-' || (str[0] >= '0' && str[0] <= '9'))
#endif
      {
        str = __builtin_char_memchr(str + least_length_when_casting, ',', UINT8_MAX) + SZC(", ");
      }
      else {
        str += least_length_when_value;
        const auto commapos = static_cast<std::size_t>(__builtin_char_memchr(str, ',', UINT8_MAX) - str);
        if (IsBitFlag)
          values[valid_count] = index == 0 ? IntType{} : static_cast<IntType>(IntType{1} << (index - 1));
        else
          values[valid_count] = static_cast<IntType>(min + static_cast<IntType>(index));
        string_lengths[valid_count++] = static_cast<std::uint8_t>(commapos);
        __builtin_memcpy(strings + total_string_length, str, commapos);
        total_string_length += commapos + null_terminated;
        str += commapos + SZC(", ");
      }
    }
  }

  template<typename E, typename MinT, MinT Min, typename Underlying, std::size_t... Is>
  constexpr const char* reflect_var_name(std::true_type) noexcept
  {
    return details::var_name<E, E{}, static_cast<E>(Underlying(1) << Is)..., E{}>();
  }

  template<typename E, typename MinT, MinT Min, typename Underlying, std::size_t... Is>
  constexpr const char* reflect_var_name(std::false_type) noexcept
  {
    return details::var_name<E, static_cast<E>(static_cast<MinT>(Is) + Min)..., E{}>();
  }

  template<typename E, bool NullTerminated, typename MinT, MinT Min, std::size_t... Is>
  constexpr auto reflect_elements(std::index_sequence<Is...>) noexcept
  {
    using T          = std::underlying_type_t<E>;
    using Underlying = std::make_unsigned_t<std::conditional_t<std::is_same<bool, T>::value, unsigned char, T>>;

    constexpr auto ArraySize = sizeof...(Is) + is_bitflag<E>;
    const auto     str       = details::reflect_var_name<E, MinT, Min, Underlying, Is...>(std::integral_constant<bool, is_bitflag<E>>{});

    constexpr auto enum_in_array_name = details::enum_in_array_name(raw_type_name<E>, is_scoped_enum<E>);
    constexpr auto enum_in_array_len  = enum_in_array_name.size();
    // Ubuntu Clang 20 complains about using local constexpr variables in a local struct
    ReflectStringReturnValue<std::underlying_type_t<E>, ArraySize> ret;

    // ((anonymous namespace)::A)0
    // (anonymous namespace)::a
    // this is needed to determine whether the above are cast expression if 2 braces are
    // next to eachother then it is a cast but only for anonymoused namespaced enums
    constexpr std::size_t index_check = enum_in_array_name.size() != 0 && enum_in_array_name[0] == '(' ? 1 : 0;

    details::parse_string<is_bitflag<E>>(
      /*index_check=*/index_check,
      /*str = */ str,
#if __clang_major__ > 12
      /*least_length_when_casting=*/SZC("(") + enum_in_array_len + SZC(")0"),
#else
      /*least_length_when_casting=*/1,
#endif
      /*least_length_when_value=*/details::prefix_length_or_zero<E> +
        (enum_in_array_len != 0 ? enum_in_array_len + SZC("::") : 0),
      /*min = */ static_cast<T>(Min),
      /*array_size = */ ArraySize,
      /*null_terminated= */ NullTerminated,
      /*enum_values= */ ret.values,
      /*string_lengths= */ ret.string_lengths,
      /*strings= */ ret.strings,
      /*total_string_length*/ ret.total_string_length,
      /*valid_count*/ ret.valid_count);

    return ret;
  }

  template<typename E, bool NullTerminated, typename MinT, MinT Min, std::size_t... Is>
  constexpr auto reflect(std::index_sequence<Is...>) noexcept
  {
    constexpr auto elements_local = reflect_elements<E, NullTerminated, MinT, Min>(std::index_sequence<Is...>{});

    using Strings = details::array<char, elements_local.total_string_length>;

    struct {
      decltype(elements_local) elements;
      Strings                  strings{};
    } data = {elements_local};
    __builtin_memcpy(data.strings.data(), elements_local.strings, data.strings.size());
    return data;
  }

  template<typename E, typename MinT, MinT Min, std::size_t... Is>
  constexpr bool is_out_of_range(std::index_sequence<Is...>) noexcept
  {
    constexpr auto ArraySize = sizeof...(Is);
    const auto     str       = details::var_name<E, static_cast<E>(static_cast<MinT>(Is) + Min)..., E{}>();

    constexpr auto enum_in_array_name = details::enum_in_array_name(raw_type_name<E>, is_scoped_enum<E>);
    constexpr auto enum_in_array_len  = enum_in_array_name.size();
    constexpr std::size_t index_check = enum_in_array_name.size() != 0 && enum_in_array_name[0] == '(' ? 1 : 0;
    (void)enum_in_array_len; // not used until Clang 13
    return details::is_out_of_range_parse(
      /*index_check=*/index_check,
      /*str = */ str,
#if __clang_major__ > 12
      /*least_length_when_casting=*/SZC("(") + enum_in_array_len + SZC(")0"),
#else
      /*least_length_when_casting=*/1,
#endif
      /*array_size = */ ArraySize);
  } 

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
