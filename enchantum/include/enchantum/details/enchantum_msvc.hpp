#pragma once
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

struct simple_string_view {
  const char*           begin;
  const char*           end;
  constexpr std::size_t find(const char c) const noexcept
  {
    for (auto copy = begin; copy != end; ++copy)
      if (*copy == c)
        return std::size_t(copy - begin);
  }
  constexpr std::size_t find_comma() const noexcept
  {
    for (auto copy = begin; copy != end; ++copy)
      if (*copy == ',')
        return std::size_t(copy - begin);
  }
};

#define SZC(x) (sizeof(x) - 1)
namespace details {
  template<auto Enum>
  constexpr auto enum_in_array_name_size() noexcept
  {
    string_view s = __FUNCSIG__ + SZC("auto __cdecl enchantum::details::enum_in_array_name_size<");
    s.remove_suffix(SZC(">(void) noexcept"));

    if constexpr (ScopedEnum<decltype(Enum)>) {
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
  constexpr auto var_name() noexcept
  {
    //auto __cdecl f<class std::array<enum `anonymous namespace'::UnscopedAnon,32>{enum `anonymous-namespace'::UnscopedAnon

    using T                      = typename decltype(Array)::value_type;
    std::size_t    funcsig_off   = SZC("auto __cdecl enchantum::details::var_name<class std::array<enum ");
    constexpr auto type_name_len = raw_type_name<T>.size();
    funcsig_off += type_name_len + SZC(",");
    constexpr auto Size = Array.size();
    // clang-format off
    funcsig_off += Size < 10 ? 1
    : Size < 100 ? 2
    : Size < 1000 ? 3
    : Size < 10000 ? 4
    : Size < 100000 ? 5
    : Size < 1000000 ? 6
    : Size < 10000000 ? 7
    : Size < 100000000 ? 8
    : Size < 1000000000 ? 9
    : 10;
    // clang-format on
    funcsig_off += SZC(">{enum ") + type_name_len;
    return string_view(__FUNCSIG__ + funcsig_off, SZC(__FUNCSIG__) - funcsig_off - SZC("}>(void) noexcept"));
  }

  template<auto Copy>
  inline constexpr auto static_storage_for = Copy;

  template<typename E, bool ShouldNullTerminate>
  constexpr auto get_elements()
  {
    constexpr auto Min = enum_traits<E>::min;
    constexpr auto Max = enum_traits<E>::max;

    constexpr auto Array             = details::generate_arrays<E, Min, Max>();
    const E* const ArrayData         = Array.data();
    constexpr auto ConstStr          = var_name<Array>();
    constexpr auto StringSize        = ConstStr.size();
    constexpr auto ArraySize         = Array.size() - 1;
    auto           str               = simple_string_view(ConstStr.data(), ConstStr.data() + ConstStr.size());
    constexpr auto type_name_len     = raw_type_name<E>.size();
    constexpr auto enum_in_array_len = details::enum_in_array_name_size<E{}>();

    struct RetVal {
      struct ElementPair {
        E           value;
        std::size_t string_length;
      };
      ElementPair pairs[ArraySize]{};
      char        strings[StringSize - (details::Min(type_name_len, enum_in_array_len) * ArraySize)]{};
      std::size_t total_string_length = 0;
      std::size_t valid_count         = 0;
    } ret;

    // there is atleast 1 base 16 hex digit
    constexpr auto skip_if_cast_count = SZC("(enum ") + type_name_len + SZC(")0x0");
    // clang-format off
#if ENCHANTUM_ENABLE_MSVC_SPEEDUP
    using Underlying = std::underlying_type_t<E>;
    constexpr auto skip_work_if_neg = std::is_unsigned_v<Underlying> || sizeof(E) <= 2 ? 0 : 
        std::is_same_v<Underlying,char32_t>  ? sizeof(char32_t)*2-1 : sizeof(std::uint64_t)*2-1;
#endif
    // clang-format on

    for (std::size_t index = 0; index < ArraySize; ++index) {
      if (*str.begin == '(') {
#if ENCHANTUM_ENABLE_MSVC_SPEEDUP
        if constexpr (skip_work_if_neg != 0) {
          const auto i = static_cast<std::underlying_type_t<E>>(ArrayData[index]);
          str.begin += skip_if_cast_count + ((i < 0) * skip_work_if_neg);
        }
        else {
          str.begin += skip_if_cast_count;
        }
#else
        str.begin += skip_if_cast_count;
#endif

        str.begin += str.find_comma() + 1;
      }
      else {
        if constexpr (enum_in_array_len != 0)
          str.begin += enum_in_array_len + SZC("::");

        if constexpr (details::prefix_length_or_zero<E> != 0)
          str.begin += details::prefix_length_or_zero<E>;

        const auto commapos = str.find_comma();

        ret.pairs[ret.valid_count++] = {ArrayData[index], commapos};
        for (std::size_t i = 0; i < commapos; ++i)
          ret.strings[ret.total_string_length++] = str.begin[i];
        ret.total_string_length += ShouldNullTerminate;

        str.begin += commapos + 1;
      }
    }
    return ret;
  }

  template<typename E, typename Pair, bool ShouldNullTerminate>
  constexpr auto reflect() noexcept
  {
    constexpr auto elements = details::get_elements<E, ShouldNullTerminate>();

    constexpr auto strings = [](const auto total_length, const char* const name_data) {
      std::array<char, total_length> ret;
      auto* const                    ret_data = ret.data();
      for (std::size_t i = 0; i < total_length.value; ++i)
        ret_data[i] = name_data[i];
      return ret;
    }(std::integral_constant<std::size_t, elements.total_string_length>{}, elements.strings);

    std::array<Pair, elements.valid_count> ret;
    auto* const                            ret_data = ret.data();
    constexpr const auto*                  str      = static_storage_for<strings>.data();
    for (std::size_t i = 0, string_index = 0; i < elements.valid_count; ++i) {
      const auto& [e, length] = elements.pairs[i];
      auto& [re, rs]          = ret_data[i];
      re                      = e;

      rs = {str + string_index, str + string_index + length};
      string_index += length + ShouldNullTerminate;
    }

    return ret;
  }
} // namespace details


} // namespace enchantum

#undef SZC