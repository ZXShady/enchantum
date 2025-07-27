#pragma once
#include "../common.hpp"
#include "../type_name.hpp"
#include "generate_arrays.hpp"
#include "string_view.hpp"
#include <algorithm>
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
      if (s.front() == '(') {
        s.remove_prefix(SZC("(enum "));
        s.remove_suffix(SZC(")0x0"));
        return s.size();
      }
      return s.substr(0, s.rfind(':') - 1).size();
    }
    else {
      if (s.front() == '(') {
        s.remove_prefix(SZC("(enum "));
        s.remove_suffix(SZC(")0x0"));
      }
      if (const auto pos = s.rfind(':'); pos != s.npos)
        return pos - 1;
      return std::size_t(0);
    }
  }

  template<auto Array>
  constexpr auto __cdecl var_name() noexcept
  {
    //auto __cdecl f<class std::array<enum `anonymous namespace'::UnscopedAnon,32>{enum `anonymous-namespace'::UnscopedAnon

    std::size_t    funcsig_off   = SZC("auto __cdecl enchantum::details::var_name<class ");
    funcsig_off += details::raw_type_name_func<decltype(Array)>().size() - 1;
    funcsig_off += SZC("{enum ") + details::raw_type_name_func<typename decltype(Array)::value_type>().size() - 1;
    return string_view(__FUNCSIG__ + funcsig_off, SZC(__FUNCSIG__) - funcsig_off - SZC("}>(void) noexcept"));
  }

  template<auto Copy>
  inline constexpr auto static_storage_for = Copy;

  template<typename E, bool NullTerminated>
  constexpr auto get_elements()
  {
    constexpr auto Min = enum_traits<E>::min;
    constexpr auto Max = enum_traits<E>::max;

    constexpr auto Array             = details::generate_arrays<E, Min, Max>();
    const E* const ArrayData         = Array.data();
    constexpr auto ConstStr          = details::var_name<Array>();
    constexpr auto StringSize        = ConstStr.size();
    constexpr auto ArraySize         = Array.size() - 1;
    const auto*    str               = ConstStr.data();
    constexpr auto type_name_len     = details::raw_type_name_func<E>().size()-1;
    constexpr auto enum_in_array_len = details::enum_in_array_name_size<E{}>();

    struct RetVal {
      E values[ArraySize]{};

      // We are making an assumption that no sane user will use an enum member name longer than 256 characters
      // if you are not sane then I don't know what to do
      std::uint8_t string_lengths[ArraySize]{};

      char        strings[StringSize - (details::Min(type_name_len, enum_in_array_len) * ArraySize)]{};
      std::size_t total_string_length = 0;
      std::size_t valid_count         = 0;
    } ret;

    // there is atleast 1 base 16 hex digit
    // MSVC adds an extra 0 prefix at front if the underlying type equals to 8 bytes.
    // Don't ask why
    constexpr auto skip_if_cast_count = SZC("(enum ") + type_name_len + SZC(")0x0") + (sizeof(E) == 8);
    // clang-format off
#if ENCHANTUM_ENABLE_MSVC_SPEEDUP
    using Underlying = std::underlying_type_t<E>;
    constexpr auto skip_work_if_neg = std::is_unsigned_v<Underlying> || sizeof(E) <= 2 ? 0 : 
// MSVC 19.31 and below don't cast int/unsigned int into `unsigned long long` (std::uint64_t)
// While higher versions do cast them
#if _MSC_VER <= 1931
        sizeof(Underlying) == 4
#else
        std::is_same_v<Underlying,char32_t> 
#endif
        ? sizeof(char32_t)*2-1 : sizeof(std::uint64_t)*2-1 - (sizeof(E)==8); // subtract 1 more from uint64_t since I am adding it in skip_if_cast_count
#endif
    // clang-format on

    for (std::size_t index = 0; index < ArraySize; ++index) {
      if (*str == '(') {
#if ENCHANTUM_ENABLE_MSVC_SPEEDUP
        if constexpr (skip_work_if_neg != 0) {
          const auto i = static_cast<std::underlying_type_t<E>>(ArrayData[index]);
          str += skip_if_cast_count + ((i < 0) * skip_work_if_neg);
        }
        else {
          str += skip_if_cast_count;
        }
#else
        str += skip_if_cast_count;
#endif
        while(*str++ != ',')
          /*intentionally empty*/;
      }
      else {
        if constexpr (enum_in_array_len != 0)
          str += enum_in_array_len + SZC("::");

        if constexpr (details::prefix_length_or_zero<E> != 0)
          str += details::prefix_length_or_zero<E>;

        ret.values[ret.valid_count]           = ArrayData[index];
        std::size_t i = 0;
        while (str[i] != ',')
          ++i;
        std::copy_n(str, i, ret.strings + ret.total_string_length);
        ret.string_lengths[ret.valid_count++] = static_cast<std::uint8_t>(i);
        ret.total_string_length += i + NullTerminated;
        str += i + 1;
      }
    }
    return ret;
  }

  template<typename E, bool NullTerminated>
  constexpr auto reflect() noexcept
  {
    constexpr auto elements = details::get_elements<E, NullTerminated>();

    constexpr auto strings = [](const auto total_length, const char* const name_data) {
      std::array<char, total_length> ret;
      std::copy_n(name_data, total_length.value, ret.data());
      return ret;
    }(std::integral_constant<std::size_t, elements.total_string_length>{}, elements.strings);

    using StringLengthType = std::conditional_t<(elements.total_string_length < UINT8_MAX), std::uint8_t, std::uint16_t>;

    struct RetVal {
      std::array<E, elements.valid_count> values{};
      // +1 for easier iteration on on last string
      std::array<StringLengthType, elements.valid_count + 1> string_indices{};
      const char*                                            strings{};
    } ret;
    ret.strings                     = static_storage_for<strings>.data();
    auto* const values_data         = ret.values.data();
    auto* const string_indices_data = ret.string_indices.data();

    std::size_t      i            = 0;
    StringLengthType string_index = 0;
    for (; i < elements.valid_count; ++i) {
      values_data[i] = elements.values[i];
      // "aabc"

      string_indices_data[i] = string_index;
      string_index += elements.string_lengths[i] + NullTerminated;
    }
    ret.string_indices[i] = string_index;

    return ret;
  }
} // namespace details
} // namespace enchantum

#undef SZC