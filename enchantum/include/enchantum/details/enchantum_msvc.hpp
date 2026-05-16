#pragma once
#include "../common.hpp"
#include "../type_name.hpp"
#include "shared.hpp"
#include "string_view.hpp"
#include <cassert>
#include <climits>
#include <cstdint>
#include <type_traits>
#include <utility>

// This macro controls the compile time optimization of msvc
// This macro may break some enums with very large enum ranges selected.
// **may** as in I have not found a case where it does
// but it speeds up compilation massivly.
// from 20 secs to 14.6 secs
// from 119 secs to 85
#ifndef ENCHANTUM_ENABLE_MSVC_SPEEDUP
  #define ENCHANTUM_ENABLE_MSVC_SPEEDUP 1
#endif
namespace enchantum {


#define SZC(x) (sizeof(x) - 1)
namespace details {

  constexpr std::size_t find_type_value_separator(const string_view s, const std::size_t start) noexcept
  {
    std::size_t depth = 0;
    for (std::size_t i = start; i < s.size(); ++i) {
      if (s[i] == '<')
        ++depth;
      else if (s[i] == '>')
        --depth;
      else if (s[i] == ',' && depth == 0)
        return i;
    }
    return s.npos;
  }

#if ENCHANTUM_DETAILS_CXX_STD >= 201703L
  template<auto Enum>
  constexpr auto enum_in_array_name_size() noexcept
  {
    auto s = string_view{__FUNCSIG__ + SZC("auto __cdecl enchantum::details::enum_in_array_name_size<"),
                         SZC(__FUNCSIG__) - SZC("auto __cdecl enchantum::details::enum_in_array_name_size<>(void) noexcept")};
    using E = decltype(Enum);

    if constexpr (is_scoped_enum<E>) {
      if (s[0] == '(') {
        s.remove_prefix(SZC("(enum "));
        s.remove_suffix(SZC(")0x0") + (sizeof(Enum) == 8)); // MSVC adds a extra 0 at the end for some reason for 8 bit enums
        return s.size();
      }
      return s.substr(0, s.rfind(':') - 1).size();
    }
    else {
      if (s[0] == '(') {
        s.remove_prefix(SZC("(enum "));
        s.remove_suffix(SZC(")0x0") + (sizeof(Enum) == 8)); // MSVC adds a extra 0 at the end for some reason for 8 bit enums
      }
      if (const auto pos = s.rfind(':'); pos != s.npos)
        return pos - 1;
      return std::size_t(0);
    }
  }
#else
  template<typename E, E Enum>
  constexpr auto enum_in_array_name_size() noexcept
  {
    auto s = string_view{__FUNCSIG__ + SZC("auto __cdecl enchantum::details::enum_in_array_name_size<"),
                         SZC(__FUNCSIG__) - SZC("auto __cdecl enchantum::details::enum_in_array_name_size<>(void) noexcept")};
    s.remove_prefix(details::find_type_value_separator(s, 0) + 1);

    if (is_scoped_enum<E>) {
      if (s[0] == '(') {
        s.remove_prefix(SZC("(enum "));
        s.remove_suffix(SZC(")0x0") + (sizeof(Enum) == 8)); // MSVC adds a extra 0 at the end for some reason for 8 bit enums
        return s.size();
      }
      return s.substr(0, s.rfind(':') - 1).size();
    }
    else {
      if (s[0] == '(') {
        s.remove_prefix(SZC("(enum "));
        s.remove_suffix(SZC(")0x0") + (sizeof(Enum) == 8)); // MSVC adds a extra 0 at the end for some reason for 8 bit enums
      }
      const auto pos = s.rfind(':');
      if (pos != s.npos)
        return pos - 1;
      return std::size_t(0);
    }
  }
#endif

#if ENCHANTUM_DETAILS_CXX_STD >= 201703L
  template<auto... Vs>
  constexpr auto __cdecl var_name() noexcept
  {
    //auto __cdecl f<class std::array<enum `anonymous namespace'::UnscopedAnon,32>{enum `anonymous-namespace'::UnscopedAnon
    return __FUNCSIG__ + SZC("auto __cdecl enchantum::details::var_name<");
  }
#else
  template<typename E, E... Vs>
  constexpr auto __cdecl var_name() noexcept
  {
    //auto __cdecl f<class std::array<enum `anonymous namespace'::UnscopedAnon,32>{enum `anonymous-namespace'::UnscopedAnon
    constexpr string_view sig{__FUNCSIG__, SZC(__FUNCSIG__)};
    constexpr auto        pos = details::find_type_value_separator(sig, SZC("auto __cdecl enchantum::details::var_name<"));
    return __FUNCSIG__ + pos + SZC(",");
  }
#endif
  template<typename IntType>
  constexpr bool is_out_of_range_parse(const char*       str,
                                       const bool        skip_work_if_neg,
                                       const std::size_t least_length_when_casting,
                                       const IntType     min,
                                       const std::size_t array_size)
  {
    for (std::size_t index = 0; index < array_size; ++index) {
#if _MSC_VER <= 1924
      // if it starts with the number 0 (because of 0x0) then it is a value
      // and you cannot start an enum name with a digit so this is safe
      if (*str == '0') {
#else
      // if it starts with a '(' it is a cast!
      if (*str == '(') {
#endif
        if (skip_work_if_neg != 0) {
          const auto i = min + static_cast<IntType>(index);
          str += least_length_when_casting + ((i < 0) * skip_work_if_neg);
        }
        else {
          str += least_length_when_casting;
        }
        while (*str++ != ',')
          /*intentionally empty*/;
      }
      else {
        return true;
      }
    }
    return false;
  }

  template<typename IntType>
  constexpr IntType parse_string_value(const std::size_t index, const IntType, std::true_type) noexcept
  {
    return index == 0 ? IntType{} : static_cast<IntType>(IntType{1} << (index - 1));
  }

  template<typename IntType>
  constexpr IntType parse_string_value(const std::size_t index, const IntType min, std::false_type) noexcept
  {
    return static_cast<IntType>(min + static_cast<IntType>(index));
  }

  template<typename IntType, std::size_t SkipIfNegative>
  constexpr const char* skip_casted_value_string(
    const char* str, const std::size_t least_length_when_casting, const IntType min, const std::size_t index, std::integral_constant<std::size_t, SkipIfNegative>) noexcept
  {
    const auto i = min + static_cast<IntType>(index);
    return str + least_length_when_casting + ((i < 0) * SkipIfNegative);
  }

  template<typename IntType>
  constexpr const char* skip_casted_value_string(
    const char* str, const std::size_t least_length_when_casting, const IntType, const std::size_t, std::integral_constant<std::size_t, 0>) noexcept
  {
    return str + least_length_when_casting;
  }

  template<bool IsBitFlag, typename IntType>
  constexpr void parse_string(
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
    // clang-format off
#if ENCHANTUM_ENABLE_MSVC_SPEEDUP
    constexpr auto skip_work_if_neg = IsBitFlag || std::is_unsigned<IntType>::value || sizeof(IntType) <= 2 ? 0 :
// MSVC 19.31 and below don't cast int/unsigned int into `unsigned long long` (std::uint64_t)
// While higher versions do cast them
#if _MSC_VER <= 1931
        sizeof(IntType) == 4
#else
        std::is_same<IntType,char32_t>::value
#endif
        ? sizeof(char32_t)*2-1 : sizeof(std::uint64_t)*2-1 - (sizeof(IntType)==8); // subtract 1 more from uint64_t since I am adding it in skip_if_cast_count
#endif
    // clang-format on
    for (std::size_t index = 0; index < array_size; ++index) {
#if _MSC_VER <= 1924
      // if it starts with the number 0 (because of 0x0) then it is a value
      // and you cannot start an enum name with a digit so this is safe
      if (*str == '0') {
#else
      // if it starts with a '(' it is a cast!
      if (*str == '(') {
#endif
#if ENCHANTUM_ENABLE_MSVC_SPEEDUP
        str = details::skip_casted_value_string(str,
                                                least_length_when_casting,
                                                min,
                                                index,
                                                std::integral_constant<std::size_t, skip_work_if_neg>{});
#else
        str += least_length_when_casting;
#endif
        while (*str++ != ',')
          /*intentionally empty*/;
      }
      else {
        str += least_length_when_value;

        // although gcc implementation of std::char_traits::find is using a for loop internally
        // copying the code of the function makes it way slower to compile, this was surprising.


        values[valid_count] = details::parse_string_value(index, min, std::integral_constant<bool, IsBitFlag>{});

        std::size_t i = 0;
        while (str[i] != ',')
          strings[total_string_length++] = str[i++];
        string_lengths[valid_count++] = static_cast<std::uint8_t>(i);

        total_string_length += null_terminated;
        str += i + SZC(",");
      }
    }
  }

  template<typename E, typename MinT, MinT Min, typename Underlying, std::size_t... Is>
  constexpr const char* reflect_var_name(std::true_type) noexcept
  {
#if ENCHANTUM_DETAILS_CXX_STD >= 201703L
    return details::var_name<E{}, static_cast<E>(Underlying(1) << Is)..., 0>();
#else
    return details::var_name<E, E{}, static_cast<E>(Underlying(1) << Is)..., E{}>();
#endif
  }

  template<typename E, typename MinT, MinT Min, typename Underlying, std::size_t... Is>
  constexpr const char* reflect_var_name(std::false_type) noexcept
  {
#if ENCHANTUM_DETAILS_CXX_STD >= 201703L
    return details::var_name<static_cast<E>(static_cast<MinT>(Is) + Min)..., 0>();
#else
    return details::var_name<E, static_cast<E>(static_cast<MinT>(Is) + Min)..., E{}>();
#endif
  }

  template<typename E, bool NullTerminated, typename MinT, MinT Min, std::size_t... Is>
  constexpr auto reflect_elements(std::index_sequence<Is...>) noexcept
  {
    constexpr auto ArraySize = sizeof...(Is) + is_bitflag<E>;
    using Under              = std::underlying_type_t<E>;
    using Underlying = std::make_unsigned_t<std::conditional_t<std::is_same<bool, Under>::value, unsigned char, Under>>;


    constexpr auto str               = details::reflect_var_name<E, MinT, Min, Underlying, Is...>(std::integral_constant<bool, is_bitflag<E>>{});
    constexpr auto type_name_len     = details::raw_type_name_func<E>().size() - 1;
#if ENCHANTUM_DETAILS_CXX_STD >= 201703L
    constexpr auto enum_in_array_len = details::enum_in_array_name_size<E{}>();
#else
    constexpr auto enum_in_array_len = details::enum_in_array_name_size<E, E{}>();
#endif

    ReflectStringReturnValue<std::underlying_type_t<E>, ArraySize> ret;
    details::parse_string<is_bitflag<E>>(
      /*str = */ str,
#if _MSC_VER <= 1924
      /*least_length_when_casting=*/SZC("0x0"),
#else
      /*least_length_when_casting=*/SZC("(enum ") + type_name_len + SZC(")0x0") + (sizeof(E) == 8),
#endif
      /*least_length_when_value=*/details::prefix_length_or_zero<E> +
        (enum_in_array_len != 0 ? enum_in_array_len + SZC("::") : 0),
      /*min = */ static_cast<std::underlying_type_t<E>>(Min),
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

    const auto  size        = data.strings.size();
    auto* const data_string = data.strings.data();
    for (std::size_t i = 0; i < size; ++i)
      data_string[i] = elements_local.strings[i];
    return data;
  }


  template<typename E, typename MinT, MinT Min, std::size_t... Is>
  constexpr bool is_out_of_range(std::index_sequence<Is...>) noexcept
  {
    constexpr auto ArraySize = sizeof...(Is);
    using Under              = std::underlying_type_t<E>;

#if ENCHANTUM_ENABLE_MSVC_SPEEDUP
    constexpr auto skip_work_if_neg = std::is_unsigned<Under>::value || sizeof(Under) <= 2 ? 0 :
  // MSVC 19.31 and below don't cast int/unsigned int into `unsigned long long` (std::uint64_t)
  // While higher versions do cast them
  #if _MSC_VER <= 1931
      sizeof(Under) == 4
  #else
      std::is_same<Under, char32_t>::value
  #endif
      ? sizeof(char32_t) * 2 - 1
      : sizeof(std::uint64_t) * 2 - 1 -
        (sizeof(Under) == 8); // subtract 1 more from uint64_t since I am adding it in skip_if_cast_count
#else
    constexpr auto skip_work_if_neg = false;
#endif
#if ENCHANTUM_DETAILS_CXX_STD >= 201703L
    const auto str           = details::var_name<static_cast<E>(static_cast<MinT>(Is) + Min)..., 0>();
#else
    const auto str           = details::var_name<E, static_cast<E>(static_cast<MinT>(Is) + Min)..., E{}>();
#endif
    const auto type_name_len = details::raw_type_name_func<E>().size() - 1;

    return details::is_out_of_range_parse(
      /*str = */ str,
      skip_work_if_neg,
#if _MSC_VER <= 1924
      /*least_length_when_casting=*/SZC("0x0"),
#else
      /*least_length_when_casting=*/SZC("(enum ") + type_name_len + SZC(")0x0") + (sizeof(E) == 8),
#endif
      /*min = */ static_cast<std::underlying_type_t<E>>(Min),
      /*array_size = */ ArraySize);
  }

} // namespace details
} // namespace enchantum

#undef SZC
