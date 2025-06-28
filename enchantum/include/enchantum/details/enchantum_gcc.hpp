#include "../common.hpp"
#include "../type_name.hpp"
#include "generate_arrays.hpp"
#include <array>
#include <cassert>
#include <climits>
#include <cstdint>
#include <type_traits>
#include <utility>

#include "string_view.hpp"

namespace enchantum {
namespace details {
#define SZC(x) (sizeof(x) - 1)
  // this is needed since gcc transforms "{anonymous}" into "<unnamed>" for values
  template<auto Enum>
  constexpr auto enum_in_array_name_size() noexcept
  {
    // constexpr auto f() [with auto _ = (
    //constexpr auto f() [with auto _ = (Scoped)0]
    auto s  = string_view(__PRETTY_FUNCTION__ +
                           SZC("constexpr auto enchantum::details::enum_in_array_name_size() [with auto Enum = "),
                         SZC(__PRETTY_FUNCTION__) -
                           SZC("constexpr auto enchantum::details::enum_in_array_name_size() [with auto Enum = ]"));
    using E = decltype(Enum);
    // if scoped
    if constexpr (!std::is_convertible_v<E, std::underlying_type_t<E>>) {
      return s.front() == '(' ? s.size() - SZC("()0") : s.rfind(':') - 1;
    }
    else {
      if (s.front() == '(') {
        s.remove_prefix(SZC("("));
        s.remove_suffix(SZC(")0"));
      }
      if (const auto pos = s.rfind(':'); pos != s.npos)
        return pos - 1;
      return std::size_t{0};
    }
  }

  template<typename Enum>
  constexpr auto length_of_enum_in_template_array_if_casting() noexcept
  {
    if constexpr (ScopedEnum<Enum>) {
      return details::enum_in_array_name_size<Enum{}>();
    }
    else {
      constexpr auto  s      = details::enum_in_array_name_size<Enum{}>();
      constexpr auto& tyname = raw_type_name<Enum>;
      if (constexpr auto pos = tyname.rfind("::"); pos != tyname.npos) {
        return s + tyname.substr(pos).size();
      }
      else {
        return s + tyname.size();
      }
    }
  }

  template<auto... Vs>
  constexpr auto var_name() noexcept
  {
    //constexpr auto f() [with auto _ = std::array<E, 6>{std::__array_traits<E, 6>::_Type{a, b, c, e, d, (E)6}}]
    constexpr std::size_t funcsig_off = SZC("constexpr auto enchantum::details::var_name() [with auto ...Vs = {");
    return std::string_view(__PRETTY_FUNCTION__ + funcsig_off, SZC(__PRETTY_FUNCTION__) - funcsig_off - SZC("}]"));
  }


  template<auto Copy>
  inline constexpr auto static_storage_for = Copy;

  template<typename E, bool NullTerminated>
  constexpr auto reflect() noexcept
  {
    constexpr auto Min = enum_traits<E>::min;
    constexpr auto Max = enum_traits<E>::max;

    constexpr auto elements = []() {
      constexpr auto length_of_enum_in_template_array_casting = details::length_of_enum_in_template_array_if_casting<E>();
      constexpr auto ArraySize = 1 + std::size_t{is_bitflag<E> ? (sizeof(E) * CHAR_BIT - std::is_signed_v<E>) : Max - Min};
      //constexpr auto Array    = details::generate_arrays<E, Min, Max>();
      using Under      = std::underlying_type_t<E>;
      using Underlying = std::make_unsigned_t<std::conditional_t<std::is_same_v<bool, Under>, unsigned char, Under>>;


      constexpr auto ConstStr = []<std::size_t... Idx>(std::index_sequence<Idx...>) {
        if constexpr (sizeof...(Idx) && is_bitflag<E>) // sizeof... to make contest dependant
          return details::var_name<E{}, __builtin_bit_cast(E, static_cast<Under>(Underlying(1) << Idx))...>();
        else
          return details::var_name<__builtin_bit_cast(E, static_cast<Under>(static_cast<decltype(Min)>(Idx) + Min))...>();
      }(std::make_index_sequence<ArraySize - is_bitflag<E>>());
      auto str = ConstStr;
      struct RetVal {
        E            values[ArraySize];
        std::uint8_t string_lengths[ArraySize];
        char         strings[ConstStr.size()]{};
        std::size_t  total_string_length = 0;
        std::size_t  valid_count         = 0;
      } ret;
      constexpr auto enum_in_array_len = details::enum_in_array_name_size<E{}>();
      for (std::size_t index = 0; index < ArraySize; ++index) {
        if (str.front() == '(') {
          str.remove_prefix(SZC("(") + length_of_enum_in_template_array_casting + SZC(")0")); // there is atleast 1 base 10 digit
          //if(!str.empty())
          //	std::cout << "after str \"" << str << '"' << '\n';

          if (const auto commapos = str.find(','); commapos != str.npos)
            str.remove_prefix(commapos + 2);

          //std::cout << "strsize \"" << str.size() << '"' << '\n';
        }
        else {
          if constexpr (enum_in_array_len != 0)
            str.remove_prefix(enum_in_array_len + SZC("::"));
          if constexpr (details::prefix_length_or_zero<E> != 0)
            str.remove_prefix(details::prefix_length_or_zero<E>);

          const auto commapos = str.find(',');


          {
            const auto        name      = str.substr(0, commapos);
            const auto        name_size = static_cast<std::uint8_t>(name.size());
            const auto* const name_data = name.data();

            if constexpr (is_bitflag<E>)
              ret.values[ret.valid_count] = index == 0 ? E() : E(Underlying{1} << (index - 1));
            else
              ret.values[ret.valid_count] = E(Min + static_cast<decltype(Min)>(index));
            ret.string_lengths[ret.valid_count++] = name_size;
            for (std::size_t i = 0; i < name_size; ++i)
              ret.strings[ret.total_string_length++] = name_data[i];
            ret.total_string_length += NullTerminated;

            if (commapos != str.npos)
              str.remove_prefix(commapos + 2);
          }
        }
      }
      return ret;
    }();

    constexpr auto strings = [](const auto total_length, const char* data) {
      std::array<char, total_length.value> ret;
      auto* const                          ret_data = ret.data();
      for (std::size_t i = 0; i < total_length.value; ++i)
        ret_data[i] = data[i];
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

    std::size_t      i            = 0;
    StringLengthType string_index = 0;
    for (; i < elements.valid_count; ++i) {
      ret.values[i] = elements.values[i];
      // "aabc"

      ret.string_indices[i] = string_index;
      string_index += static_cast<StringLengthType>(elements.string_lengths[i] + NullTerminated);
    }
    ret.string_indices[i] = string_index;
    return ret;
  }

} // namespace details

} // namespace enchantum

#undef SZC