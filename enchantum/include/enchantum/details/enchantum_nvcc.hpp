#include "../common.hpp"
#include "../type_name.hpp"
#include "shared.hpp"
#include <cassert>
#include <climits>
#include <cstdint>
#include <type_traits>
#include <utility>
namespace enchantum {

namespace details {
#define SZC(x) (sizeof(x) - 1)

  constexpr std::size_t find_comma(const char* s)
  {
    for (std::size_t i = 0; true; ++i)
      if (s[i] == ',')
        return i;
  }

  constexpr const char* find_nvcc_values_pack(const char* s) noexcept
  {
    for (std::size_t i = 0; true; ++i) {
      if (s[i] == 'V' && s[i + 1] == ' ' && s[i + 2] == '=' && s[i + 3] == ' ' && s[i + 4] == '{')
        return s + i + SZC("V = {");
    }
  }

  constexpr std::size_t enum_in_array_name_size(const string_view raw_type_name, const bool is_scoped_enum) noexcept
  {
    if (is_scoped_enum)
      return raw_type_name.size();

    const auto pos = raw_type_name.rfind(':');
    if (pos != string_view::npos)
      return pos - 1;
    return 0;
  }

  template<typename E, E... V>
  constexpr auto var_name() noexcept
  {
    return details::find_nvcc_values_pack(__PRETTY_FUNCTION__);
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
    for (std::size_t index = 0; index < array_size; ++index) {
      // check if cast (starts with '(')
      if (str[0] == '(') {
        str += least_length_when_casting;
        while (*str++ != ',')
          /*intentionally empty*/;
        str += SZC(" ");
      }
      else {
        str += least_length_when_value;
        const auto commapos = details::find_comma(str);
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
    using T = std::underlying_type_t<E>;
    using Underlying = std::make_unsigned_t<std::conditional_t<std::is_same<bool, T>::value, unsigned char, T>>;
    constexpr auto ArraySize = sizeof...(Is) + is_bitflag<E>;
#pragma diag_suppress implicit_return_from_non_void_function
    const auto str = details::reflect_var_name<E, MinT, Min, Underlying, Is...>(std::integral_constant<bool, is_bitflag<E>>{});
#pragma diag_default implicit_return_from_non_void_function

    constexpr auto enum_in_array_len = details::enum_in_array_name_size(raw_type_name<E>, is_scoped_enum<E>);
    // Ubuntu Clang 20 complains about using local constexpr variables in a local struct
    ReflectStringReturnValue<std::underlying_type_t<E>, ArraySize> ret;

    // ((anonymous namespace)::A)0
    // (anonymous namespace)::a
    // this is needed to determine whether the above are cast expression if 2 braces are
    // next to eachother then it is a cast but only for anonymoused namespaced enums

    details::parse_string<is_bitflag<E>>(
      /*str = */ str,
      /*least_length_when_casting=*/SZC("(") + enum_in_array_len + SZC(")0"),
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
  } // namespace details

} // namespace details
} // namespace enchantum
