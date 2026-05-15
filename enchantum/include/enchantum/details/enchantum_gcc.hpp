#include "../common.hpp"
#include "../type_name.hpp"
#include "shared.hpp"
#include <cassert>
#include <climits>
#include <cstdint>
#include <type_traits>
#include <utility>

#if defined(__has_include) && __has_include(<bits/char_traits.h>)
  #include <bits/char_traits.h>
#endif
#include "string_view.hpp"

#define ENCHANTUM_DETAILS_GCC_MAJOR __GNUC__
#if __GNUC__ <= 10
// for out of bounds conversions for C style enums
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wconversion"
#endif

namespace enchantum {
namespace details {
#define SZC(x) (sizeof(x) - 1)


  // this is needed since gcc transforms "{anonymous}" into "<unnamed>" for values
  template<typename E, E Enum>
  constexpr auto enum_in_array_name_size() noexcept
  {
    // constexpr auto f() [with auto _ = (
    //constexpr auto f() [with auto _ = (Scoped)0]
    auto s  = string_view(__PRETTY_FUNCTION__ +
                            SZC("constexpr auto enchantum::details::enum_in_array_name_size() [with E = "),
                          SZC(__PRETTY_FUNCTION__) -
                            SZC("constexpr auto enchantum::details::enum_in_array_name_size() [with E = ]"));
    s.remove_prefix(s.find(';') + SZC(" E Enum = "));
    // if scoped
    if (!std::is_convertible<E, std::underlying_type_t<E>>::value) {
      return s[0] == '(' ? s.size() - SZC("()0") : s.rfind(':') - 1;
    }
    else {
      if (s[0] == '(') {
        s.remove_prefix(SZC("("));
        s.remove_suffix(SZC(")0"));
      }
      const auto pos = s.rfind(':');
      if (pos != s.npos)
        return pos - 1;
      return std::size_t{0};
    }
  }

#if __GNUC__ == 10
  template<typename E, E V>
  constexpr auto gcc10_workaround() noexcept
  {
    using T               = std::underlying_type_t<E>;
    constexpr auto prefix = SZC("constexpr auto enchantum::details::gcc10_workaround() [with E = ");
    constexpr auto begin  = __PRETTY_FUNCTION__ + prefix;
    if (begin[0] == '(') {
      std::size_t i   = SZC(__PRETTY_FUNCTION__) - prefix - SZC("(");
      const char* end = __PRETTY_FUNCTION__ + SZC(__PRETTY_FUNCTION__) - 1;
      while (*end != ')') {
        --end;
        --i;
      }
      --i;
      return i;
    }
    else if (static_cast<T>(V) == (std::numeric_limits<T>::max)()) {
      constexpr auto  s      = details::enum_in_array_name_size<E, E{}>();
      constexpr auto& tyname = raw_type_name<E>;
      const auto pos = tyname.rfind("::");
      if (pos != tyname.npos) {
        return s + tyname.substr(pos).size();
      }
      else {
        return s + tyname.size();
      }
    }
    else {
      return details::gcc10_workaround<E, static_cast<E>(static_cast<T>(V) + 1)>();
    }
  }
#endif

  template<typename Enum>
  constexpr auto length_of_enum_in_template_array_if_casting_impl(std::true_type) noexcept
  {
    return details::enum_in_array_name_size<Enum, Enum{}>();
  }

  template<typename Enum>
  constexpr auto length_of_enum_in_template_array_if_casting_impl(std::false_type) noexcept
  {
#if __GNUC__ == 10
    return details::gcc10_workaround<Enum, static_cast<Enum>((std::numeric_limits<std::underlying_type_t<Enum>>::min)())>();
#else
    constexpr auto  s      = details::enum_in_array_name_size<Enum, Enum{}>();
    constexpr auto& tyname = raw_type_name<Enum>;
    const auto pos = tyname.rfind("::");
    if (pos != tyname.npos)
      return s + tyname.substr(pos).size();
    return s + tyname.size();
#endif
  }

  template<typename Enum>
  constexpr auto length_of_enum_in_template_array_if_casting() noexcept
  {
    return details::length_of_enum_in_template_array_if_casting_impl<Enum>(std::integral_constant<bool, is_scoped_enum<Enum>>{});
  }

  constexpr const char* find_gcc_values_pack(const char* s) noexcept
  {
    for (std::size_t i = 0; true; ++i) {
      if (s[i] == 'V' && s[i + 1] == 's' && s[i + 2] == ' ' && s[i + 3] == '=' && s[i + 4] == ' ' && s[i + 5] == '{')
        return s + i + SZC("Vs = {");
    }
  }

  constexpr const char* find_char(const char* s, const std::size_t count, const char c) noexcept
  {
    for (std::size_t i = 0; i < count; ++i) {
      if (s[i] == c || s[i] == '\0')
        return s + i;
    }
    return nullptr;
  }

  template<typename E, E... Vs>
  constexpr auto var_name() noexcept
  {
    return details::find_gcc_values_pack(__PRETTY_FUNCTION__);
  }


  constexpr bool is_out_of_range_parse(const char* str, const std::size_t least_length_when_casting, const std::size_t array_size)
  {
    for (std::size_t index = 0; index < array_size; ++index) {
      if (*str == '(') {
        const auto comma = details::find_char(str + least_length_when_casting, UINT8_MAX, ',');
        if (comma == nullptr || *comma == '\0')
          return false;
        str = comma + SZC(", ");
      }
      else
        return true;
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
    (void)min; // not always used
    for (std::size_t index = 0; index < array_size; ++index) {
      if (*str == '(') {
        const auto comma = details::find_char(str + least_length_when_casting, UINT8_MAX, ',');
        if (comma == nullptr || *comma == '\0')
          return;
        str = comma + SZC(", ");
      }
      else {
        str += least_length_when_value;
        const auto comma = details::find_char(str, UINT8_MAX, ',');
        if (comma == nullptr)
          return;
        const auto commapos = static_cast<std::size_t>(comma - str);
        if (IsBitFlag)
          values[valid_count] = index == 0 ? IntType{} : static_cast<IntType>(IntType{1} << (index - 1));
        else
          values[valid_count] = static_cast<IntType>(min + static_cast<IntType>(index));
        string_lengths[valid_count++] = static_cast<std::uint8_t>(commapos);
        for (std::size_t i = 0; i < commapos; ++i)
          strings[total_string_length++] = str[i];
        total_string_length += null_terminated;
        if (*comma == '\0')
          return;
        str = comma + SZC(", ");
      }
    }
  }


  template<typename E, typename MinT, MinT Min, typename Underlying, std::size_t... Is>
  constexpr const char* reflect_var_name(std::true_type) noexcept
  {
#if __GNUC__ <= 10
  #define CAST(type, value) static_cast<type>(value)
#else
  #define CAST(type, value) __builtin_bit_cast(type, value)
#endif
    return details::var_name<E, E{}, CAST(E, static_cast<typename std::underlying_type<E>::type>(Underlying{1} << Is))..., E{}>();
#undef CAST
  }

  template<typename E, typename MinT, MinT Min, typename Underlying, std::size_t... Is>
  constexpr const char* reflect_var_name(std::false_type) noexcept
  {
#if __GNUC__ <= 10
  #define CAST(type, value) static_cast<type>(value)
#else
  #define CAST(type, value) __builtin_bit_cast(type, value)
#endif
    return details::var_name<E, CAST(E, static_cast<typename std::underlying_type<E>::type>(static_cast<MinT>(Is) + Min))..., E{}>();
#undef CAST
  }

  template<typename E, bool NullTerminated, typename MinT, MinT Min, std::size_t... Is>
  constexpr auto reflect_elements(std::index_sequence<Is...>) noexcept
  {
    constexpr auto ArraySize = sizeof...(Is) + is_bitflag<E>;
    using Under              = std::underlying_type_t<E>;
    using Underlying = std::make_unsigned_t<std::conditional_t<std::is_same<bool, Under>::value, unsigned char, Under>>;

    constexpr auto str = details::reflect_var_name<E, MinT, Min, Underlying, Is...>(std::integral_constant<bool, is_bitflag<E>>{});

    constexpr auto enum_in_array_len = details::enum_in_array_name_size<E, E{}>();
    constexpr auto length_of_enum_in_template_array_casting = details::length_of_enum_in_template_array_if_casting<E>();

    ReflectStringReturnValue<std::underlying_type_t<E>, ArraySize> ret;
    details::parse_string<is_bitflag<E>>(
      /*str = */ str,
      /*least_length_when_casting=*/SZC("(") + length_of_enum_in_template_array_casting + SZC(")0"),
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
    } data                  = {elements_local};
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

#if __GNUC__ <= 10
    // GCC 10 does not have it
  #define CAST(type, value) static_cast<type>(value)
#else
    // __builtin_bit_cast used to silence errors when casting out of unscoped enums range
  #define CAST(type, value) __builtin_bit_cast(type, value)
#endif
    constexpr auto str = details::var_name<E, CAST(E, static_cast<Under>(static_cast<MinT>(Is) + Min))..., E{}>();
#undef CAST

    constexpr auto length_of_enum_in_template_array_casting = details::length_of_enum_in_template_array_if_casting<E>();

    return details::is_out_of_range_parse(
      /*str = */ str,
      /*least_length_when_casting=*/SZC("(") + length_of_enum_in_template_array_casting + SZC(")0"),
      /*array_size = */ ArraySize);
  }


} // namespace details

} // namespace enchantum

#undef SZC

#if __GNUC__ <= 10
  #pragma GCC diagnostic pop
#endif
