#pragma once
#include "../common.hpp"
#include "../type_name.hpp"
#include "shared.hpp"
#include "string_view.hpp"
#include <array>
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

  template<auto Enum>
  constexpr auto enum_in_array_name_size() noexcept
  {
    auto s = string_view{__FUNCSIG__ + SZC("auto __cdecl enchantum::details::enum_in_array_name_size<"),
                         SZC(__FUNCSIG__) - SZC("auto __cdecl enchantum::details::enum_in_array_name_size<>(void) noexcept")};

    if constexpr (is_scoped_enum<decltype(Enum)>) {
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

  template<auto... Vs>
  constexpr auto __cdecl var_name() noexcept
  {
    //auto __cdecl f<class std::array<enum `anonymous namespace'::UnscopedAnon,32>{enum `anonymous-namespace'::UnscopedAnon
    return __FUNCSIG__ + SZC("auto __cdecl enchantum::details::var_name<");
  }
  template<auto... Vs>
  constexpr auto __cdecl var_name2() noexcept
  {
    // !__FUNCSIG__[1000000];
    return SZC(__FUNCSIG__) - SZC("auto __cdecl enchantum::details::var_name2<>(void) noexcept");
  }
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
    constexpr auto skip_work_if_neg = IsBitFlag || std::is_unsigned_v<IntType> || sizeof(IntType) <= 2 ? 0 : 
// MSVC 19.31 and below don't cast int/unsigned int into `unsigned long long` (std::uint64_t)
// While higher versions do cast them
#if _MSC_VER <= 1931
        sizeof(IntType) == 4
#else
        std::is_same_v<IntType,char32_t> 
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
        if constexpr (skip_work_if_neg != 0) {
          const auto i = min + static_cast<IntType>(index);
          str += least_length_when_casting + ((i < 0) * skip_work_if_neg);
        }
        else {
          str += least_length_when_casting;
        }
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


        if constexpr (IsBitFlag)
          values[valid_count] = index == 0 ? IntType{} : static_cast<IntType>(IntType{1} << (index - 1));
        else
          values[valid_count] = static_cast<IntType>(min + static_cast<IntType>(index));

        std::size_t i = 0;
        while (str[i] != ',')
          strings[total_string_length++] = str[i++];
        string_lengths[valid_count++] = static_cast<std::uint8_t>(i);

        total_string_length += null_terminated;
        str += i + SZC(",");
      }
    }
  }

  template<typename E, bool NullTerminated, auto Min, std::size_t... Is>
  constexpr auto reflect(std::index_sequence<Is...>) noexcept
  {
    constexpr auto elements_local = []() {
      constexpr auto ArraySize = sizeof...(Is) + is_bitflag<E>;
      using MinT               = decltype(Min);
      using Under              = std::underlying_type_t<E>;
      using Underlying = std::make_unsigned_t<std::conditional_t<std::is_same_v<bool, Under>, unsigned char, Under>>;


      constexpr auto str = [](const auto dependant) {
        constexpr bool always_true = sizeof(dependant) != 0;
        // dummy 0
        if constexpr (always_true && is_bitflag<E>) // sizeof... to make contest dependant
          return details::var_name<static_cast<E>(!always_true), static_cast<E>(Underlying(1) << Is)..., 0>();
        else
          return details::var_name<static_cast<E>(static_cast<MinT>(Is) + Min)..., int(!always_true)>();
      }(0);
      constexpr auto type_name_len     = details::raw_type_name_func<E>().size() - 1;
      constexpr auto enum_in_array_len = details::enum_in_array_name_size<E{}>();

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
  }

  constexpr uint64_t count16(uint64_t min) {
    auto total = uint64_t(0);
    
    if(min >= 0x1000000000000000ull && min <= 0xffffffffffffffffull)
    {
        total += (min-0x1000000000000000ull+1)*16;
        min = 0x1000000000000000ull-1;
    }
    

    if(min >= 0x100000000000000ull && min <= 0xfffffffffffffffull)
    {
        total += (min-0x100000000000000ull+1)*15;
        min = 0x100000000000000ull-1;
    }
    

    if(min >= 0x10000000000000ull && min <= 0xffffffffffffffull)
    {
        total += (min-0x10000000000000ull+1)*14;
        min = 0x10000000000000ull-1;
    }
    

    if(min >= 0x1000000000000ull && min <= 0xfffffffffffffull)
    {
        total += (min-0x1000000000000ull+1)*13;
        min = 0x1000000000000ull-1;
    }
    

    if(min >= 0x100000000000ull && min <= 0xffffffffffffull)
    {
        total += (min-0x100000000000ull+1)*12;
        min = 0x100000000000ull-1;
    }
    

    if(min >= 0x10000000000ull && min <= 0xfffffffffffull)
    {
        total += (min-0x10000000000ull+1)*11;
        min = 0x10000000000ull-1;
    }
    

    if(min >= 0x1000000000ull && min <= 0xffffffffffull)
    {
        total += (min-0x1000000000ull+1)*10;
        min = 0x1000000000ull-1;
    }
    

    if(min >= 0x100000000ull && min <= 0xfffffffffull)
    {
        total += (min-0x100000000ull+1)*9;
        min = 0x100000000ull-1;
    }
    

    if(min >= 0x10000000ull && min <= 0xffffffffull)
    {
        total += (min-0x10000000ull+1)*8;
        min = 0x10000000ull-1;
    }
    

    if(min >= 0x1000000ull && min <= 0xfffffffull)
    {
        total += (min-0x1000000ull+1)*7;
        min = 0x1000000ull-1;
    }
    

    if(min >= 0x100000ull && min <= 0xffffffull)
    {
        total += (min-0x100000ull+1)*6;
        min = 0x100000ull-1;
    }
    

    if(min >= 0x10000ull && min <= 0xfffffull)
    {
        total += (min-0x10000ull+1)*5;
        min = 0x10000ull-1;
    }
    

    if(min >= 0x1000ull && min <= 0xffffull)
    {
        total += (min-0x1000ull+1)*4;
        min = 0x1000ull-1;
    }
    

    if(min >= 0x100ull && min <= 0xfffull)
    {
        total += (min-0x100ull+1)*3;
        min = 0x100ull-1;
    }
    

    if(min >= 0x10ull && min <= 0xffull)
    {
        total += (min-0x10ull+1)*2;
        min = 0x10ull-1;
    }
    

    if(min >= 0x0ull && min <= 0xfull)
    {
        total += (min-0x0ull+1)*1;
        min = 0x0ull;
    }
    
    return total;
  }
  constexpr uint8_t count_letters(uint64_t x) {
    // clang-format off
      if(x <= 0xfull)               return 1;
      if(x <= 0xffull)              return 2;
      if(x <= 0xfffull)             return 3;
      if(x <= 0xffffull)            return 4;
      if(x <= 0xfffffull)           return 5;
      if(x <= 0xffffffull)          return 6;
      if(x <= 0xfffffffull)         return 7;
      if(x <= 0xffffffffull)        return 8;
      if(x <= 0xfffffffffull)       return 9;
      if(x <= 0xffffffffffull)      return 10;
      if(x <= 0xfffffffffffull)     return 11;
      if(x <= 0xffffffffffffull)    return 12;
      if(x <= 0xfffffffffffffull)   return 13;
      if(x <= 0xffffffffffffffull)  return 14;
      if(x <= 0xfffffffffffffffull) return 15;
    // clang-format on
      return 16;
  }

constexpr uint64_t count_numbers(int64_t min,int64_t max,int treat_as)
{
    if(max < min)
      return 0;
    if(min < 0 || max < 0)
    {
        uint64_t len = 0;
        switch(treat_as) 
        {
            case sizeof(char):
            for(int64_t i = min;i <= max;++i)
                len += count_letters(static_cast<unsigned char>(i));
            break;
            case sizeof(short):
            for(int64_t i = min;i <=max;++i)
                len += count_letters(static_cast<unsigned short>(i));
            break;
            case sizeof(int):
            for(int64_t i = min;i <=max;++i)
                len += count_letters(static_cast<unsigned int>(i));
            break;
            case sizeof(long long):
              return std::uint64_t(max-min+1)*16;
            // for(int64_t i = min;i <= max;++i) 
                // len += count_letters(static_cast<unsigned long long>(i));
            break;
        }
        return len;
        // + details::count16(std::uint64_t(max));
    }

    return details::count16(std::uint64_t(max))-details::count16(std::uint64_t(min)-1);
}


  template<typename E,std::int32_t... Is>
  constexpr auto is_out_of_range( std::int32_t Min0,std::int32_t Max0,std::int32_t Min1,std::int32_t Max1, std::integer_sequence<int32_t,Is...>) noexcept
  {
    using T = std::underlying_type_t<E>;
    const auto totalNumbers = static_cast<std::size_t>(Max0-Min0 + Max1-Min1 +2);
    auto len =  enchantum::details::var_name2<E(Is)...>();
    constexpr auto size = std::is_same_v<T,int> || std::is_same_v<T, long> ? sizeof(long long) : sizeof(T);
    #if _MSC_VER > 1924
    len -= (SZC("(enum )") + enchantum::raw_type_name<E>.size()) * totalNumbers;
    #endif
    len -= (totalNumbers-1) * SZC(",");
    len -= totalNumbers * SZC("0x");
    if constexpr(std::is_same_v<T,signed long long> || std::is_same_v<T,unsigned long long>)
        len -= (Max1-Min1+1) * SZC("0");
      // char c[1];
      // c[details::count_numbers(Min0,Max0,size)] = 0;
      // + details::count_numbers(Min1,Max1,size)] = 0;
      len -= details::count_numbers(Min0,Max0,size) + details::count_numbers(Min1,Max1,size);
      // c[details::count_numbers(Min1,Max1,size)] = 0;
      return len != 0;
  }

} // namespace details
} // namespace enchantum

#undef SZC