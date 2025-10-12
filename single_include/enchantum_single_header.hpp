#pragma once

#ifdef ENCHANTUM_CONFIG_FILE
  #include ENCHANTUM_CONFIG_FILE
#endif

#ifndef ENCHANTUM_ALIAS_STRING_VIEW
  #include <string_view>
#endif

namespace enchantum {
#ifdef ENCHANTUM_ALIAS_STRING_VIEW
ENCHANTUM_ALIAS_STRING_VIEW;
#else
using ::std::string_view;
#endif

} // namespace enchantum
#include <array>

namespace enchantum {

namespace details {
#define SZC(x) (sizeof(x) - 1)
  constexpr string_view extract_name_from_type_name(const string_view type_name) noexcept
  {
    if (const auto n = type_name.rfind(':'); n != type_name.npos)
      return type_name.substr(n + 1);
    else
      return type_name;
  }

  template<typename T>
  constexpr auto raw_type_name_func() noexcept
  {
#if defined(__NVCOMPILER)
    constexpr std::size_t prefix = 0;
    constexpr auto s = string_view(__PRETTY_FUNCTION__ + SZC("constexpr auto enchantum::details::raw_type_name_func() noexcept [with T = "),
            SZC(__PRETTY_FUNCTION__) - SZC("constexpr auto enchantum::details::raw_type_name_func() noexcept [with T = ]"));
#elif defined(__clang__)
    constexpr std::size_t prefix = 0;
    constexpr auto s = string_view(__PRETTY_FUNCTION__ + SZC("auto enchantum::details::raw_type_name_func() [_ = "),
                                   SZC(__PRETTY_FUNCTION__) - SZC("auto enchantum::details::raw_type_name_func() [_ = ]"));
#elif defined(_MSC_VER)
    constexpr auto s = string_view(__FUNCSIG__ + SZC("auto __cdecl enchantum::details::raw_type_name_func<"),
                                   SZC(__FUNCSIG__) - SZC("auto __cdecl enchantum::details::raw_type_name_func<") -
                                     SZC(">(void) noexcept"));

    // clang-format off
    constexpr auto prefix = std::is_enum_v<T> ? SZC("enum ") : 
        std::is_class_v<T> ?  SZC("struct ") - (s[0] == 'c') :
        0;
// clang-format on
#elif defined(__GNUG__)
    constexpr std::size_t prefix = 0;
    constexpr auto        s      = string_view(__PRETTY_FUNCTION__ +
                                     SZC("constexpr auto enchantum::details::raw_type_name_func() [with _ = "),
                                   SZC(__PRETTY_FUNCTION__) -
                                     SZC("constexpr auto enchantum::details::raw_type_name_func() [with _ = ]"));
#endif
    std::array<char, 1 + s.size() - prefix> ret{};
    auto* const                             ret_data = ret.data();
    const auto* const                       s_data   = s.data();

    for (std::size_t i = 0; i < ret.size() - 1; ++i)
      ret_data[i] = s_data[i + prefix];
    return ret;
  }

  template<typename T>
  inline constexpr auto raw_type_name_func_var = raw_type_name_func<T>();

  template<typename T>
  constexpr auto type_name_func() noexcept
  {
    static_assert(!std::is_function_v<std::remove_pointer_t<T>> && !std::is_member_function_pointer_v<T>,
                  "enchantum::type_name<T> does not work well with function pointers or functions or member function\n"
                  "pointers");

    constexpr auto& array = raw_type_name_func_var<T>;
    static_assert(array[array.size() - 2] != '>', "enchantum::type_name<T> does not work well with a templated type");

    constexpr auto  s     = details::extract_name_from_type_name(string_view(array.data(), array.size() - 1));
    std::array<char, s.size() + 1> ret{};
    for (std::size_t i = 0; i < s.size(); ++i)
      ret[i] = s[i];
    return ret;
  }

  template<typename T>
  inline constexpr auto type_name_func_var = type_name_func<T>();

#undef SZC

} // namespace details

template<typename T>
inline constexpr auto type_name = string_view(details::type_name_func_var<T>.data(),
                                              details::type_name_func_var<T>.size() - 1);

template<typename T>
inline constexpr auto raw_type_name = string_view(details::raw_type_name_func_var<T>.data(),
                                                  details::raw_type_name_func_var<T>.size() - 1);

} // namespace enchantum

#ifdef ENCHANTUM_CONFIG_FILE
  #include ENCHANTUM_CONFIG_FILE
#endif

#ifndef ENCHANTUM_ALIAS_OPTIONAL
  #include <optional>
#endif

namespace enchantum {
#ifdef ENCHANTUM_ALIAS_OPTIONAL
ENCHANTUM_ALIAS_OPTIONAL;
#else
using ::std::optional;
#endif

} // namespace enchantum

#ifdef ENCHANTUM_CONFIG_FILE
  #include ENCHANTUM_CONFIG_FILE
#endif

#ifndef ENCHANTUM_ALIAS_STRING
  #include <string>
#endif

namespace enchantum {
#ifdef ENCHANTUM_ALIAS_STRING
ENCHANTUM_ALIAS_STRING;
#else
using ::std::string;
#endif

} // namespace enchantum

#ifdef __cpp_concepts
  #include <concepts>
#endif
#include <limits>
#include <string_view>
#include <type_traits>

#ifndef ENCHANTUM_ASSERT
  #include <cassert>
// clang-format off
  #define ENCHANTUM_ASSERT(cond, msg, ...) assert(cond && msg)
// clang-format on
#endif

#ifndef ENCHANTUM_THROW
  // additional info such as local variables are here
  #define ENCHANTUM_THROW(exception, ...) throw exception
#endif

#ifndef ENCHANTUM_MAX_RANGE
  #define ENCHANTUM_MAX_RANGE 256
#endif
#ifndef ENCHANTUM_MIN_RANGE
  #define ENCHANTUM_MIN_RANGE (-ENCHANTUM_MAX_RANGE)
#endif

namespace enchantum {

template<typename T, bool = std::is_enum_v<T>>
inline constexpr bool is_scoped_enum = false;

template<typename E>
inline constexpr bool is_scoped_enum<E, true> = !std::is_convertible_v<E, std::underlying_type_t<E>>;

template<typename E>
inline constexpr bool is_unscoped_enum = std::is_enum_v<E> && !is_scoped_enum<E>;

template<typename E, typename = void>
inline constexpr bool has_fixed_underlying_type = false;

template<typename E>
inline constexpr bool has_fixed_underlying_type<E, decltype(void(E{0}))> = std::is_enum_v<E>;

#ifdef __cpp_concepts

template<typename T>
concept Enum = std::is_enum_v<T>;

template<Enum E>
inline constexpr bool is_bitflag = requires(E e) {
  requires std::same_as<decltype(e & e), bool> || std::same_as<decltype(e & e), E>;
  { ~e } -> std::same_as<E>;
  { e | e } -> std::same_as<E>;
  { e &= e } -> std::same_as<E&>;
  { e |= e } -> std::same_as<E&>;
};

template<typename T>
concept SignedEnum = Enum<T> && std::signed_integral<std::underlying_type_t<T>>;

template<typename T>
concept UnsignedEnum = Enum<T> && !SignedEnum<T>;

template<typename T>
concept ScopedEnum = Enum<T> && (!std::is_convertible_v<T, std::underlying_type_t<T>>);

template<typename T>
concept UnscopedEnum = Enum<T> && !ScopedEnum<T>;

template<typename E, typename Underlying>
concept EnumOfUnderlying = Enum<E> && std::same_as<std::underlying_type_t<E>, Underlying>;

template<typename T>
concept BitFlagEnum = Enum<T> && is_bitflag<T>;

template<typename T>
concept EnumFixedUnderlying = Enum<T> && requires { T{0}; };

#else

template<typename E, typename = void>
inline constexpr bool is_bitflag = false;

// clang-format off
template<typename E>
inline constexpr bool is_bitflag<E, 
    std::void_t<
    decltype(E{} & E{}),
    decltype(~E{}), 
    decltype(E{} | E{}), 
    decltype(std::declval<E&>() &= E{}), 
    decltype(std::declval<E&>() |= E{})
    >> =  std::is_enum_v<E>
    &&    (std::is_same_v<decltype(E{} & E{}),bool>  || std::is_same_v<decltype(E{} & E{}), E>) 
    &&    std::is_same_v<decltype(~E{}), E> 
    &&    std::is_same_v<decltype(E{} | E{}), E>
    &&    std::is_same_v<decltype(std::declval<E&>() &= E{}), E&>
    &&    std::is_same_v<decltype(std::declval<E&>() |= E{}), E&>
    ;
// clang-format on
#endif

namespace details {
  template<typename T, typename U>
  constexpr auto Max(T a, U b)
  {
    return a < b ? b : a;
  }
  template<typename T, typename U>
  constexpr auto Min(T a, U b)
  {
    return a > b ? b : a;
  }
#if !defined(__NVCOMPILER) && defined(__clang__) && __clang_major__ >= 20
  template<typename E, auto V, typename = void>
  inline constexpr bool is_valid_cast = false;

  template<typename E, auto V>
  inline constexpr bool is_valid_cast<E, V, std::void_t<std::integral_constant<E, static_cast<E>(V)>>> = true;

  template<typename E, std::underlying_type_t<E> range, decltype(range) old_range>
  constexpr auto valid_cast_range_recurse() noexcept
  {
    // this tests whether `static_cast`ing range is valid
    // because C style enums stupidly is like a bit field
    // `enum E { a,b,c,d = 3};` is like a bitfield `struct E { int val : 2;}`
    // which means giving E.val a larger than 2 bit value is UB so is it for enums
    // and gcc and msvc ignore this (for good)
    // while clang makes it a subsituation failure which we can check for
    // using std::inegral_constant makes sure this is a constant expression situation
    // for SFINAE to occur
    if constexpr (is_valid_cast<E, range>)
      return valid_cast_range_recurse<E, range * 2, range>();
    else
      return old_range > 0 ? old_range * 2 - 1 : old_range;
  }
  template<typename E, int max_range>
  constexpr auto valid_cast_range() noexcept
  {
    using T = std::underlying_type_t<E>;
    using L = std::numeric_limits<T>;

    if constexpr (max_range == 0)
      return T{0};
    else if constexpr (max_range > 0 && is_valid_cast<E, (L::max)()>)
      return L::max();
    else if constexpr (max_range < 0 && is_valid_cast<E, (L::min)()>)
      return L::min();
    else
      return details::valid_cast_range_recurse<E, max_range, 0>();
  }

#endif

  template<typename E>
  constexpr auto enum_range_of(const int max_range)
  {
    using T = std::underlying_type_t<E>;
    if constexpr (std::is_same_v<bool, T>) {
      return max_range > 0;
    }
    else {
      using L = std::numeric_limits<T>;
#if !defined(__NVCOMPILER) && defined(__clang__) && __clang_major__ >= 20
      constexpr auto Max = has_fixed_underlying_type<E> ? (L::max)() : details::valid_cast_range<E, 1>();
      constexpr auto Min = has_fixed_underlying_type<E>
        ? (L::min)()
        : details::valid_cast_range<E, std::is_signed_v<T> ? -1 : 0>();
#else
      constexpr auto Max = (L::max)();
      constexpr auto Min = (L::min)();
#endif
      (void)Min; // Only used in signed branch
      if constexpr (std::is_signed_v<T>) {
        return max_range > 0 ? details::Min(ENCHANTUM_MAX_RANGE, Max) : details::Max(ENCHANTUM_MIN_RANGE, Min);
      }
      else {
        return max_range > 0 ? details::Min(static_cast<unsigned int>(ENCHANTUM_MAX_RANGE), Max) : 0;
      }
    }
  }
} // namespace details

template<typename E>
struct enum_traits {
private:
  using T = std::underlying_type_t<E>;
public:
  using zxshady_enchantum_is_not_specialized_tag = void;
  static constexpr auto          max = details::enum_range_of<E>(1);
  static constexpr decltype(max) min = details::enum_range_of<E>(-1);
};

namespace details {
  template<typename T,typename = void>
  inline constexpr bool has_specialized_traits = true;
  template<typename T>
  inline constexpr bool has_specialized_traits<T, typename enum_traits<T>::zxshady_enchantum_is_not_specialized_tag> = false;

} // namespace details

} // namespace enchantum

#ifdef __cpp_concepts
  #define ENCHANTUM_DETAILS_ENUM_CONCEPT(Name)         Enum Name
  #define ENCHANTUM_DETAILS_ENUM_BITFLAG_CONCEPT(Name) BitFlagEnum Name
#else
  #define ENCHANTUM_DETAILS_ENUM_CONCEPT(Name)         typename Name, std::enable_if_t<std::is_enum_v<Name>, int> = 0
  #define ENCHANTUM_DETAILS_ENUM_BITFLAG_CONCEPT(Name) typename Name, std::enable_if_t<is_bitflag<Name>, int> = 0
#endif
#include <array>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace enchantum {
namespace details {

  template<typename E, typename = void>
  inline constexpr std::size_t prefix_length_or_zero = 0;

  template<typename E>
  inline constexpr auto prefix_length_or_zero<E, decltype((void)enum_traits<E>::prefix_length)> = std::size_t{
    enum_traits<E>::prefix_length};

  template<typename Underlying, std::size_t ArraySize>
  struct ReflectStringReturnValue {
    Underlying   values[ArraySize]{};
    std::uint8_t string_lengths[ArraySize]{};
    // the sum of all character names must be less than the size of this array
    // no one will likely hit this unless you for some odd reason have extremely long names
    char        strings[1024 * 8]{};
    std::size_t total_string_length = 0;
    std::size_t valid_count         = 0;
  };

} // namespace details
} // namespace enchantum

#if defined(__NVCOMPILER)
  

#include <array>
#include <cassert>
#include <climits>
#include <cstdint>
#include <type_traits>
#include <utility>
namespace enchantum {

namespace details {
  constexpr std::size_t find_semicolon(const char* s)
  {
    for (std::size_t i = 0; true; ++i)
      if (s[i] == ';')
        return i;
  }
  constexpr std::size_t enum_in_array_name_size(const string_view raw_type_name, const bool is_scoped_enum) noexcept
  {
    if (is_scoped_enum)
      return raw_type_name.size();

    if (const auto pos = raw_type_name.rfind(':'); pos != string_view::npos)
      return pos - 1;
    return 0;
  }

#define SZC(x) (sizeof(x) - 1)

  template<auto... V>
  constexpr auto var_name() noexcept
  {
    return __PRETTY_FUNCTION__ + SZC("constexpr auto enchantum::details::var_name() noexcept [with _ *V = (_ *)0; ");
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
      str += SZC("_ *V = ");
      if (str[0] == '(') {
        str += least_length_when_casting;
        while (*str++ != ';')
          /*intentionally empty*/;
        str += SZC(" ");
      }
      else {
        str += least_length_when_value;
        const auto commapos = details::find_semicolon(str);
        if constexpr (IsBitFlag)
          values[valid_count] = index == 0 ? IntType{} : static_cast<IntType>(IntType{1} << (index - 1));
        else
          values[valid_count] = static_cast<IntType>(min + static_cast<IntType>(index));
        string_lengths[valid_count++] = static_cast<std::uint8_t>(commapos);
        __builtin_memcpy(strings + total_string_length, str, commapos);
        total_string_length += commapos + null_terminated;
        str += commapos + SZC("; ");
      }
    }
  }

  template<typename E, bool NullTerminated, auto Min, std::size_t... Is>
  constexpr auto reflect(std::index_sequence<Is...>) noexcept
  {
    using MinT = decltype(Min);
    using T    = std::underlying_type_t<E>;

    constexpr auto elements_local = []() {
      constexpr auto ArraySize = sizeof...(Is) + is_bitflag<E>;
#pragma diag_suppress implicit_return_from_non_void_function
      const auto str = [](auto dependant) {
        constexpr bool always_true = sizeof(dependant) != 0;
        // forces NVCC to shorten the string types
        struct _ {};
        // using a pointer since C++17 only allows pointers to class types not the class types themselves
        constexpr _* A{};
        using Underlying = std::make_unsigned_t<std::conditional_t<std::is_same_v<bool, T>, unsigned char, T>>;
        // dummy 0
        if constexpr (always_true && is_bitflag<E>) // sizeof... to make contest dependant
          return details::var_name<A, static_cast<E>(!always_true), static_cast<E>(Underlying(1) << Is)..., 0>();
        else
          return details::var_name<A, static_cast<E>(static_cast<MinT>(Is) + Min)..., int(!always_true)>();
      }(0);
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
    }();

    using Strings = std::array<char, elements_local.total_string_length>;

    struct {
      decltype(elements_local) elements;
      Strings                  strings{};
    } data = {elements_local};
    __builtin_memcpy(data.strings.data(), elements_local.strings, data.strings.size());
    return data;
  } // namespace details

} // namespace details
} // namespace enchantum
#elif defined(__clang__)
  

// Clang <= 12 outputs "NUMBER" if casting
// Clang > 12 outputs "(E)NUMBER".

#if defined __has_warning
  #if __has_warning("-Wenum-constexpr-conversion")
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wenum-constexpr-conversion"
  #endif
#endif

#include <array>
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

    if (const auto pos = raw_type_name.rfind(':'); pos != string_view::npos)
      return raw_type_name.substr(0, pos - 1);
    return string_view();
  }

#define SZC(x) (sizeof(x) - 1)

  template<auto... Vs>
  constexpr auto var_name() noexcept
  {
    // "auto enchantum::details::var_name() [Vs = <(A)0, a, b, c, e, d, (A)6>]"
    return __PRETTY_FUNCTION__ + SZC("auto enchantum::details::var_name() [Vs = <");
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
    constexpr std::size_t skip_after_comma = SZC(", ");
    for (std::size_t index = 0; index < array_size; ++index) {
#if __clang_major__ > 12
      // check if cast (starts with '(')
      if (str[index_check] == '(')
#else
      // check if it is a number or negative sign
      if (str[0] == '-' || (str[0] >= '0' && str[0] <= '9'))
#endif
      {
        str = __builtin_char_memchr(str + least_length_when_casting, ',', UINT8_MAX) + skip_after_comma;
      }
      else {
        str += least_length_when_value;
        const auto commapos = static_cast<std::size_t>(__builtin_char_memchr(str, ',', UINT8_MAX) - str);
        if constexpr (IsBitFlag)
          values[valid_count] = index == 0 ? IntType{} : static_cast<IntType>(IntType{1} << (index - 1));
        else
          values[valid_count] = static_cast<IntType>(min + static_cast<IntType>(index));
        string_lengths[valid_count++] = static_cast<std::uint8_t>(commapos);
        __builtin_memcpy(strings + total_string_length, str, commapos);
        total_string_length += commapos + null_terminated;
        str += commapos + skip_after_comma;
      }
    }
  }
  
  template<typename E, bool NullTerminated, auto Min, std::size_t... Is>
  constexpr auto reflect(std::index_sequence<Is...>) noexcept
  {
    using MinT       = decltype(Min);
    using T          = std::underlying_type_t<E>;
    using Underlying = std::make_unsigned_t<std::conditional_t<std::is_same_v<bool, T>, unsigned char, T>>;

    constexpr auto elements_local = []() {
      constexpr auto ArraySize = sizeof...(Is) + is_bitflag<E>;
      const auto     str       = [](auto dependant) {
        constexpr bool always_true = sizeof(dependant) != 0;
        // dummy 0
        if constexpr (always_true && is_bitflag<E>) // sizeof... to make contest dependant
        {
          return details::var_name<static_cast<E>(!always_true), static_cast<E>(Underlying(1) << Is)..., 0>();
        }
        else {
          return details::var_name<static_cast<E>(static_cast<MinT>(Is) + Min)..., int(!always_true)>();
        }
      }(0);

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
    }();

    using Strings = std::array<char, elements_local.total_string_length>;

    struct {
      decltype(elements_local) elements;
      Strings                  strings{};
    } data = {elements_local};
    __builtin_memcpy(data.strings.data(), elements_local.strings, data.strings.size());
    return data;
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
#elif defined(__GNUC__) || defined(__GNUG__)
  

#include <array>
#include <cassert>
#include <climits>
#include <cstdint>
#include <type_traits>
#include <utility>

#define ENCAHNTUM_DETAILS_GCC_MAJOR __GNUC__
#if __GNUC__ <= 10
// for out of bounds conversions for C style enums
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wconversion"
#endif

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
      return s[0] == '(' ? s.size() - SZC("()0") : s.rfind(':') - 1;
    }
    else {
      if (s[0] == '(') {
        s.remove_prefix(SZC("("));
        s.remove_suffix(SZC(")0"));
      }
      if (const auto pos = s.rfind(':'); pos != s.npos)
        return pos - 1;
      return std::size_t{0};
    }
  }

#if __GNUC__ == 10
  template<auto V>
  constexpr auto gcc10_workaround() noexcept
  {
    using E = decltype(V);
    using T = std::underlying_type_t<E>;
    constexpr auto prefix = SZC("constexpr auto enchantum::details::gcc10_workaround() [with auto V = ");
    constexpr auto begin  = __PRETTY_FUNCTION__ + prefix;
    if constexpr (begin[0] == '(') {
      std::size_t i   = SZC(__PRETTY_FUNCTION__) - prefix - SZC("(");
      const char* end = __PRETTY_FUNCTION__ + SZC(__PRETTY_FUNCTION__) - 1;
      while (*end != ')') {
        --end;
        --i;
      }
      --i;
      return i;
    }
    else if constexpr (static_cast<T>(V) == (std::numeric_limits<T>::max)()) {
      constexpr auto  s      = details::enum_in_array_name_size<E{}>();
      constexpr auto& tyname = raw_type_name<E>;
      if (constexpr auto pos = tyname.rfind("::"); pos != tyname.npos) {
        return s + tyname.substr(pos).size();
      }
      else {
        return s + tyname.size();
      }
    }
    else {
      return details::gcc10_workaround<static_cast<E>(static_cast<T>(V) + 1)>();
    }
  }
#endif

  template<typename Enum>
  constexpr auto length_of_enum_in_template_array_if_casting() noexcept
  {
    if constexpr (is_scoped_enum<Enum>) {
      return details::enum_in_array_name_size<Enum{}>();
    }
    else {
#if __GNUC__ == 10
      return details::gcc10_workaround<static_cast<Enum>((std::numeric_limits<std::underlying_type_t<Enum>>::min)())>();
#else
      constexpr auto  s      = details::enum_in_array_name_size<Enum{}>();
      constexpr auto& tyname = raw_type_name<Enum>;
      if (constexpr auto pos = tyname.rfind("::"); pos != tyname.npos) {
        return s + tyname.substr(pos).size();
      }
      else {
        return s + tyname.size();
      }
#endif
    }
  }

  template<auto... Vs>
  constexpr auto var_name() noexcept
  {
    return __PRETTY_FUNCTION__ + SZC("constexpr auto enchantum::details::var_name() [with auto ...Vs = {");
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
    constexpr std::size_t skip_after_comma = SZC(", ");
    for (std::size_t index = 0; index < array_size; ++index) {
      if (*str == '(') {
        str = std::char_traits<char>::find(str + least_length_when_casting, UINT8_MAX, ',') + skip_after_comma;
      }
      else {
        str += least_length_when_value;
        // although gcc implementation of std::char_traits::find is using a for loop internally
        // copying the code of the function makes it way slower to compile, this was surprising.
        const auto commapos = static_cast<std::size_t>(std::char_traits<char>::find(str, UINT8_MAX, ',') - str);
        if constexpr (IsBitFlag)
          values[valid_count] = index == 0 ? IntType{} : static_cast<IntType>(IntType{1} << (index - 1));
        else
          values[valid_count] = static_cast<IntType>(min + static_cast<IntType>(index));
        string_lengths[valid_count++] = static_cast<std::uint8_t>(commapos);
        for (std::size_t i = 0; i < commapos; ++i)
          strings[total_string_length++] = str[i];
        total_string_length += null_terminated;
        str += commapos + skip_after_comma;
      }
    }
  }

  template<typename E, bool NullTerminated, auto Min, std::size_t... Is>
  constexpr auto reflect(std::index_sequence<Is...>) noexcept
  {

    constexpr auto elements_local = []() {
      constexpr auto ArraySize = sizeof...(Is) + is_bitflag<E>;
      using Under              = std::underlying_type_t<E>;
      using Underlying = std::make_unsigned_t<std::conditional_t<std::is_same_v<bool, Under>, unsigned char, Under>>;

      constexpr auto str = [](const auto dependant) {
#if __GNUC__ <= 10
      // GCC 10 does not have it
  #define CAST(type, value) static_cast<type>(value)
#else
      // __builtin_bit_cast used to silence errors when casting out of unscoped enums range
  #define CAST(type, value) __builtin_bit_cast(type, value)
#endif
        // dummy 0
        if constexpr (sizeof(dependant) && is_bitflag<E>) // sizeof... to make contest dependant
          return details::var_name<E{}, CAST(E, static_cast<Under>(Underlying{1} << Is))..., 0>();
        else
          return details::var_name<CAST(E, static_cast<Under>(static_cast<decltype(Min)>(Is) + Min))..., 0>();
#undef CAST
      }(0);

      constexpr auto enum_in_array_len = details::enum_in_array_name_size<E{}>();
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
    }();
    using Strings = std::array<char, elements_local.total_string_length>;

    struct {
      decltype(elements_local) elements;
      Strings                  strings{};
    } data = {elements_local};
    auto* const data_string = data.strings.data();
    const auto* const src_string = elements_local.strings;
    for (std::size_t i = 0, size = data.strings.size(); i < size; ++i)
      data_string[i] = src_string[i];
    return data;
  }

} // namespace details

} // namespace enchantum

#undef SZC

#if __GNUC__ <= 10
  #pragma GCC diagnostic pop
#endif

#elif defined(_MSC_VER)
  

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
        s.remove_suffix(SZC(")0x0"));
        return s.size();
      }
      return s.substr(0, s.rfind(':') - 1).size();
    }
    else {
      if (s[0] == '(') {
        s.remove_prefix(SZC("(enum "));
        s.remove_suffix(SZC(")0x0"));
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
    constexpr std::size_t skip_comma = SZC(",");
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
        str += i + skip_comma;
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

    auto* const data_string = data.strings.data();
    const auto* const src_string = elements_local.strings;
    for (std::size_t i = 0, size = data.strings.size(); i < size; ++i)
      data_string[i] = src_string[i];
    return data;
  }
} // namespace details
} // namespace enchantum

#undef SZC
#else
  #error unsupported compiler please open an issue for enchantum
#endif

#include <type_traits>
#include <utility>

#ifndef ENCHANTUM_CHECK_OUT_OF_BOUNDS_BY
  #define ENCHANTUM_CHECK_OUT_OF_BOUNDS_BY 2
#endif
#if ENCHANTUM_CHECK_OUT_OF_BOUNDS_BY < 0
  #error ENCHANTUM_CHECK_OUT_OF_BOUNDS_BY must not be a negative number.
#endif
namespace enchantum {

#ifdef __cpp_lib_to_underlying
using ::std::to_underlying;
#else
template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
[[nodiscard]] constexpr auto to_underlying(const E e) noexcept
{
  return static_cast<std::underlying_type_t<E>>(e);
}
#endif

namespace details {

  template<typename Int>
  constexpr std::size_t get_index_sequence_max(
    const bool        is_bitflag,
    const bool        has_fixed_underlying,
    const std::size_t sizeof_enum,
    const Int         min,
    const Int         max,
    const bool        is_signed)
  {
    (void)has_fixed_underlying;
    if (!is_bitflag)
      return static_cast<std::size_t>(max - min + 1);

#if __clang_major__ >= 20
    if (!has_fixed_underlying) {
      auto        v = max;
      std::size_t r = 1;
      while (v >>= 1)
        r++;
      return r;
    }
#endif
    return (sizeof_enum * CHAR_BIT) - is_signed;
  }

  template<typename E, typename StringLengthType, std::size_t Size>
  struct FinalReflectionResult {
    std::array<E, Size> values{};
    // +1 for easier iteration on on last string
    std::array<StringLengthType, Size + 1> string_indices{};
  };

  template<typename E, bool NullTerminated, auto Min = enum_traits<E>::min, decltype(Min) Max = enum_traits<E>::max>
  inline constexpr auto reflection_data_impl = details::reflect<E, NullTerminated, Min>(
    std::make_index_sequence<details::get_index_sequence_max(is_bitflag<E>,
                                                             has_fixed_underlying_type<E>,
                                                             sizeof(E),
                                                             Min,
                                                             Max,
                                                             std::is_signed_v<std::underlying_type_t<E>>)>{});

  // Thanks https://en.cppreference.com/w/cpp/utility/intcmp.html
  template<typename T, typename U>
  constexpr bool cmp_less(const T t, const U u) noexcept
  {
    if constexpr (std::is_signed_v<T> == std::is_signed_v<U>)
      return t < u;
    else if constexpr (std::is_signed_v<T>)
      return t < 0 || std::make_unsigned_t<T>(t) < u;
    else
      return u >= 0 && t < std::make_unsigned_t<U>(u);
  }

  template<typename U>
  constexpr bool cmp_less(const bool t, const U u) noexcept
  {
    return details::cmp_less(int(t), u);
  }

  template<typename T>
  constexpr bool cmp_less(const T t, const bool u) noexcept
  {
    return details::cmp_less(t, int(u));
  }

  constexpr bool cmp_less(const bool t, const bool u) noexcept
  {
    return int(t) < int(u);
  }

  template<typename T, typename U>
  constexpr T ClampToRange(U u)
  {
    using L = std::numeric_limits<T>;
    if (details::cmp_less((L::max)(), u))
      return (L::max)();
    if (details::cmp_less(u, (L::min)()))
      return (L::min)();
    return T(u);
  }
  template<typename E, bool NullTerminated>
  constexpr auto get_reflection_data() noexcept
  {
    constexpr auto elements = reflection_data_impl<E, NullTerminated>.elements;
    using StringLengthType = std::conditional_t<(elements.total_string_length < UINT8_MAX), std::uint8_t, std::uint16_t>;

#if ENCHANTUM_CHECK_OUT_OF_BOUNDS_BY >= 2
    if constexpr (
  #if __clang_major__ >= 20
      has_fixed_underlying_type<E> &&
  #endif
      !details::has_specialized_traits<E>) {
      static_assert(elements.valid_count == reflection_data_impl<E, NullTerminated,
        details::ClampToRange<std::underlying_type_t<E>>(enum_traits<E>::min * ENCHANTUM_CHECK_OUT_OF_BOUNDS_BY),
        details::ClampToRange<std::underlying_type_t<E>>(enum_traits<E>::max * ENCHANTUM_CHECK_OUT_OF_BOUNDS_BY)
    >.elements.valid_count,
          "enchantum has detected that this enum is not fully reflected. Please look at https://github.com/ZXShady/enchantum/blob/main/docs/features.md#enchantum_check_out_of_bounds_by for more information");
    }
#endif
    FinalReflectionResult<E, StringLengthType, elements.valid_count> ret;
    std::size_t                                                      i            = 0;
    StringLengthType                                                 string_index = 0;
    for (; i < elements.valid_count; ++i) {
      ret.values[i] = static_cast<E>(elements.values[i]);
      // "aabc"

      ret.string_indices[i] = string_index;
#if defined(ENCAHNTUM_DETAILS_GCC_MAJOR) && ENCAHNTUM_DETAILS_GCC_MAJOR <= 10
  // false positives from T += T
  // it does not make sense.
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wconversion"
#endif
      string_index += static_cast<StringLengthType>(elements.string_lengths[i] + NullTerminated);
#if defined(ENCAHNTUM_DETAILS_GCC_MAJOR) && ENCAHNTUM_DETAILS_GCC_MAJOR <= 10
  #pragma GCC diagnostic pop
#endif
    }
    ret.string_indices[i] = string_index;
    return ret;
  }

  template<typename E, bool NullTerminated>
  inline constexpr auto reflection_data_string_storage = details::reflection_data_impl<E, NullTerminated>.strings;

  template<typename E, bool NullTerminated>
  inline constexpr auto reflection_data = details::get_reflection_data<E, NullTerminated>();

  template<typename E, bool NullTerminated>
  inline constexpr auto reflection_string_indices = reflection_data<E, NullTerminated>.string_indices;
} // namespace details

#ifdef __cpp_concepts
template<Enum E, typename Pair = std::pair<E, string_view>, bool NullTerminated = true>
#else
template<typename E, typename Pair = std::pair<E, string_view>, bool NullTerminated = true, std::enable_if_t<std::is_enum_v<E>, int> = 0>
#endif
inline constexpr auto entries = []() {

#if defined(__NVCOMPILER)
  // nvc++ had issues with that and did not allow it. it just did not work after testing in godbolt and I don't know why
  const auto reflected = details::reflection_data<E, NullTerminated>;
  const auto strings   = details::reflection_data_string_storage<E, NullTerminated>.data();
#else
  const auto reflected = details::reflection_data<std::remove_cv_t<E>, NullTerminated>;
  const auto strings   = details::reflection_data_string_storage<std::remove_cv_t<E>, NullTerminated>.data();
#endif
  using Pairs = std::array<Pair, sizeof(reflected.values) / sizeof(reflected.values[0])>;
  Pairs          ret{};
  constexpr auto size = ret.size();
  static_assert(size != 0,
                "enchantum failed to reflect this enum.\n"
                "Please read https://github.com/ZXShady/enchantum/blob/main/docs/limitations.md before opening an "
                "issue\n"
                "with your enum type with all its namespace/classes it is defined inside to help the creator debug the "
                "issues.");
  auto* const ret_data = ret.data();

  for (std::size_t i = 0; i < size; ++i) {
    auto& [e, s]     = ret_data[i];
    e                = reflected.values[i];
    using StringView = std::remove_cv_t<std::remove_reference_t<decltype(s)>>;
    s                = StringView(strings + reflected.string_indices[i],
                   reflected.string_indices[i + 1] - reflected.string_indices[i] - NullTerminated);
  }
  return ret;
}();

namespace details {
  template<typename E>
  constexpr auto get_values() noexcept
  {
    constexpr auto              enums = entries<E>;
    std::array<E, enums.size()> ret{};
    const auto* const           enums_data = enums.data();
    for (std::size_t i = 0; i < ret.size(); ++i)
      ret[i] = enums_data[i].first;
    return ret;
  }

  template<typename E, typename String, bool NullTerminated>
  constexpr auto get_names() noexcept
  {
    constexpr auto                   enums = entries<E, std::pair<E, String>, NullTerminated>;
    std::array<String, enums.size()> ret{};
    const auto* const                enums_data = enums.data();
    for (std::size_t i = 0; i < ret.size(); ++i)
      ret[i] = enums_data[i].second;
    return ret;
  }

} // namespace details

template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
inline constexpr auto values = details::get_values<E>();

#ifdef __cpp_concepts
template<Enum E, typename String = string_view, bool NullTerminated = true>
#else
template<typename E, typename String = string_view, bool NullTerminated = true, std::enable_if_t<std::is_enum_v<E>, int> = 0>
#endif
inline constexpr auto names = details::get_names<E, String, NullTerminated>();

template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
inline constexpr auto min = entries<E>.front().first;

template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
inline constexpr auto max = entries<E>.back().first;

template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
inline constexpr std::size_t count = entries<E>.size();

template<typename E>
inline constexpr bool has_zero_flag = [](const auto is_bitflag) {
  if constexpr (is_bitflag.value) {
    for (const auto v : values<E>)
      if (static_cast<std::underlying_type_t<E>>(v) == 0)
        return true;
  }
  return false;
}(std::bool_constant<is_bitflag<E>>{});

template<typename E>
inline constexpr bool is_contiguous = static_cast<std::size_t>(
                                        enchantum::to_underlying(max<E>) - enchantum::to_underlying(min<E>)) +
    1 ==
  count<E>;

template<typename E>
inline constexpr bool is_contiguous_bitflag = [](const auto is_bitflag) {
  if constexpr (is_bitflag.value) {
    constexpr auto& enums = entries<E>;
    using T               = std::underlying_type_t<E>;
    for (auto i = std::size_t{has_zero_flag<E>}; i < enums.size() - 1; ++i)
      if (T(enums[i].first) << 1 != T(enums[i + 1].first))
        return false;
    return true;
  }
  else {
    return false;
  }
}(std::bool_constant<is_bitflag<E>>{});

#ifdef __cpp_concepts
template<typename E>
concept ContiguousEnum = Enum<E> && is_contiguous<E>;
template<typename E>
concept ContiguousBitFlagEnum = BitFlagEnum<E> && is_contiguous_bitflag<E>;
#endif

} // namespace enchantum

#ifdef __cpp_impl_three_way_comparison
  #include <compare>
#endif

#if (__cplusplus >= 202002L || (defined(_MSVC_LANG) && _MSVC_LANG >= 202002L)) && __has_include(<bit>)
#include <bit>
namespace enchantum{
  namespace details 
  {
    using ::std::countr_zero;
  }
}
#else
namespace enchantum{
  namespace details 
  {
    template <typename T>
    constexpr int countr_zero(T x) {
    if (x == 0) 
      return sizeof(T) * 8;

    int count = 0;
    while ((x & 1) == 0) {
        x = static_cast<T>(x >> 1);
        ++count;
    }
    return count;
  }
  }
}
#endif
#include <cstddef>
#include <cstdint>
#include <utility>
#if defined(ENCAHNTUM_DETAILS_GCC_MAJOR) && ENCAHNTUM_DETAILS_GCC_MAJOR <= 10
  // false positives from T += T
  // it does not make sense.
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wconversion"
#endif

namespace enchantum {
namespace details {

  struct senitiel {};

  template<typename CRTP, std::ptrdiff_t Size>
  struct sized_iterator {
    static_assert(Size < INT16_MAX, "Too many enum entries");
  private:
    using IndexType = std::conditional_t<(Size <= INT8_MAX), std::int8_t, std::int16_t>;
  public:
    IndexType       index{};
    constexpr CRTP& operator+=(const std::ptrdiff_t offset) & noexcept
    {
      index += static_cast<IndexType>(offset);
      return static_cast<CRTP&>(*this);
    }
    constexpr CRTP& operator-=(const std::ptrdiff_t offset) & noexcept
    {
      index -= static_cast<IndexType>(offset);
      return static_cast<CRTP&>(*this);
    }

    constexpr CRTP& operator++() & noexcept
    {
      ++index;
      return static_cast<CRTP&>(*this);
    }
    constexpr CRTP& operator--() & noexcept
    {
      --index;
      return static_cast<CRTP&>(*this);
    }

    [[nodiscard]] constexpr CRTP operator++(int) & noexcept
    {
      auto copy = static_cast<CRTP&>(*this);
      ++*this;
      return copy;
    }
    [[nodiscard]] constexpr CRTP operator--(int) & noexcept
    {
      auto copy = static_cast<CRTP&>(*this);
      --*this;
      return copy;
    }

    [[nodiscard]] constexpr friend CRTP operator+(CRTP it, const std::ptrdiff_t offset) noexcept
    {
      it += offset;
      return it;
    }

    [[nodiscard]] constexpr friend CRTP operator+(const std::ptrdiff_t offset, CRTP it) noexcept
    {
      it += offset;
      return it;
    }

    [[nodiscard]] constexpr friend CRTP operator-(CRTP it, const std::ptrdiff_t offset) noexcept
    {
      it -= offset;
      return it;
    }

    [[nodiscard]] constexpr std::ptrdiff_t operator-(const sized_iterator that) const noexcept
    {
      return index - that.index;
    }

    [[nodiscard]] constexpr std::ptrdiff_t        operator-(senitiel) const noexcept { return index - Size; }
    [[nodiscard]] friend constexpr std::ptrdiff_t operator-(senitiel, sized_iterator it) noexcept
    {
      return Size - it.index;
    }

    [[nodiscard]] constexpr bool operator==(const sized_iterator that) const noexcept { return that.index == index; };
    [[nodiscard]] constexpr bool operator==(senitiel) const noexcept { return Size == index; }

#ifdef __cpp_impl_three_way_comparison
    [[nodiscard]] constexpr auto operator<=>(const sized_iterator that) const noexcept { return index <=> that.index; };
    [[nodiscard]] constexpr auto operator<=>(senitiel) const noexcept { return index <=> Size; }
#else

    [[nodiscard]] constexpr bool operator!=(const sized_iterator that) const noexcept { return that.index != index; };
    [[nodiscard]] constexpr bool operator!=(senitiel) const noexcept { return Size != index; }

    [[nodiscard]] friend constexpr bool operator==(senitiel, const sized_iterator it) noexcept
    {
      return Size == it.index;
    }

    [[nodiscard]] friend constexpr bool operator!=(senitiel, const sized_iterator it) noexcept
    {
      return Size != it.index;
    }

    [[nodiscard]] constexpr bool operator<(const sized_iterator that) const noexcept { return index < that.index; };
    [[nodiscard]] constexpr bool operator>(const sized_iterator that) const noexcept { return index > that.index; };
    [[nodiscard]] constexpr bool operator<=(const sized_iterator that) const noexcept { return index <= that.index; };
    [[nodiscard]] constexpr bool operator>=(const sized_iterator that) const noexcept { return index >= that.index; };

    [[nodiscard]] constexpr bool operator<(senitiel) const noexcept { return index < Size; };
    [[nodiscard]] constexpr bool operator>(senitiel) const noexcept { return index > Size; };
    [[nodiscard]] constexpr bool operator<=(senitiel) const noexcept { return index <= Size; };
    [[nodiscard]] constexpr bool operator>=(senitiel) const noexcept { return index >= Size; };

    [[nodiscard]] friend constexpr bool operator<(senitiel, const sized_iterator it) noexcept
    {
      return Size < it.index;
    };
    [[nodiscard]] friend constexpr bool operator>(senitiel, const sized_iterator it) noexcept
    {
      return Size > it.index;
    };
    [[nodiscard]] friend constexpr bool operator<=(senitiel, const sized_iterator it) noexcept
    {
      return Size <= it.index;
    };
    [[nodiscard]] friend constexpr bool operator>=(senitiel, const sized_iterator it) noexcept
    {
      return Size >= it.index;
    };

#endif
  };

  template<typename E, typename String = string_view, bool NullTerminated = true>
  struct names_generator_t {
    [[nodiscard]] static constexpr std::size_t size() noexcept { return count<E>; }

    struct iterator : sized_iterator<iterator, static_cast<std::ptrdiff_t>(size())> {
      using value_type = String;
      [[nodiscard]] constexpr String operator*() const noexcept
      {
        const auto* const p       = details::reflection_string_indices<E, NullTerminated>.data();
        const auto* const strings = details::reflection_data_string_storage<E, NullTerminated>.data();
        return String(strings + p[this->index], p[this->index + 1] - p[this->index] - NullTerminated);
      }

      [[nodiscard]] constexpr String operator[](const std::ptrdiff_t i) const noexcept { return *(*this + i); }
    };

    [[nodiscard]] static constexpr auto begin() { return iterator{}; }
    [[nodiscard]] static constexpr auto end() { return senitiel{}; }

    [[nodiscard]] constexpr auto operator[](const std::size_t i) const noexcept
    {
      return *(begin() + static_cast<std::ptrdiff_t>(i));
    }
  };

  template<typename E>
  struct values_generator_t {
    [[nodiscard]] static constexpr std::size_t size() noexcept { return count<E>; }

    struct iterator : sized_iterator<iterator, static_cast<std::ptrdiff_t>(size())> {
      using value_type = E;
      [[nodiscard]] constexpr E operator*() const noexcept
      {
        using T = std::underlying_type_t<E>;

        if constexpr (is_contiguous<E>) {
          return static_cast<E>(static_cast<T>(min<E>) + static_cast<T>(this->index));
        }
        else if constexpr (is_contiguous_bitflag<E>) {
          using UT                       = std::make_unsigned_t<T>;
          constexpr auto real_min_offset = details::countr_zero(static_cast<UT>(values<E>[has_zero_flag<E>]));

          if constexpr (has_zero_flag<E>)
            if (this->index == 0)
              return E{};
          return static_cast<E>(UT{1} << (real_min_offset + static_cast<UT>(this->index - has_zero_flag<E>)));
        }
        else {
          return values<E>[static_cast<std::size_t>(this->index)];
        }
      }
      [[nodiscard]] constexpr E operator[](const std::ptrdiff_t i) const noexcept { return *(*this + i); }
    };

    [[nodiscard]] static constexpr auto begin() { return iterator{}; }
    [[nodiscard]] static constexpr auto end() { return senitiel{}; }

    [[nodiscard]] constexpr auto operator[](const std::size_t i) const noexcept
    {
      return *(begin() + static_cast<std::ptrdiff_t>(i));
    }
  };

  template<typename E, typename Pair = std::pair<E, string_view>, bool NullTerminated = true>
  struct entries_generator_t {
    [[nodiscard]] static constexpr std::size_t size() noexcept { return count<E>; }

    struct iterator : sized_iterator<iterator, static_cast<std::ptrdiff_t>(size())> {
      using value_type = Pair;
      [[nodiscard]] constexpr Pair operator*() const noexcept
      {
        return Pair{
          values_generator_t<E>{}[static_cast<std::size_t>(this->index)],
          names_generator_t<E, string_view, NullTerminated>{}[static_cast<std::size_t>(this->index)],
        };
      }
      [[nodiscard]] constexpr Pair operator[](const std::ptrdiff_t i) const noexcept { return *(*this + i); }
    };

    [[nodiscard]] static constexpr auto begin() { return iterator{}; }
    [[nodiscard]] static constexpr auto end() { return senitiel{}; }

    [[nodiscard]] constexpr auto operator[](const std::size_t i) const noexcept
    {
      return *(begin() + static_cast<std::ptrdiff_t>(i));
    }
  };

} // namespace details

template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
inline constexpr details::values_generator_t<E> values_generator{};

#ifdef __cpp_concepts
template<Enum E, typename StringView = string_view, bool NullTerminated = true>
inline constexpr details::names_generator_t<E, StringView, NullTerminated> names_generator{};

template<Enum E, typename Pair = std::pair<E, string_view>, bool NullTerminated = true>
inline constexpr details::entries_generator_t<E, Pair, NullTerminated> entries_generator{};

#else
template<typename E, typename StringView = string_view, bool NullTerminated = true, std::enable_if_t<std::is_enum_v<E>, int> = 0>
inline constexpr details::names_generator_t<E, StringView, NullTerminated> names_generator{};

template<typename E, typename Pair = std::pair<E, string_view>, bool NullTerminated = true, std::enable_if_t<std::is_enum_v<E>, int> = 0>
inline constexpr details::entries_generator_t<E, Pair, NullTerminated> entries_generator{};

#endif

} // namespace enchantum

#if defined(ENCAHNTUM_DETAILS_GCC_MAJOR) && ENCAHNTUM_DETAILS_GCC_MAJOR <= 10
  #pragma GCC diagnostic pop
#endif

#include <type_traits>
#include <utility>

#if defined(ENCAHNTUM_DETAILS_GCC_MAJOR) && ENCAHNTUM_DETAILS_GCC_MAJOR <= 10
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wconversion"
  #pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif

namespace enchantum {

namespace details {
  template<typename BinaryPredicate>
  constexpr bool call_predicate(const BinaryPredicate binary_pred, const string_view a, const string_view b)
  {
    if constexpr (std::is_invocable_v<const BinaryPredicate&, const char&, const char&>) {
      const auto a_size = a.size();
      if (a_size != b.size())
        return false;
      const auto a_data = a.data();
      const auto b_data = b.data();

      for (std::size_t i = 0; i < a_size; ++i)
        if (!binary_pred(a_data[i],b_data[i]))
          return false;
      return true;
    }
    else {
      static_assert(std::is_invocable_v<const BinaryPredicate&, const string_view&, const string_view&>,
                    "BinaryPredicate must be callable with atleast 2 char or 2 string_views");
      return binary_pred(a, b);
    }
  }

  constexpr std::pair<std::size_t, std::size_t> minmax_string_size(const string_view* begin, const string_view* const end)
  {
    using T     = std::size_t;
    auto minmax = std::pair<T, T>(std::numeric_limits<T>::max(), 0);

    for (; begin != end; ++begin) {
      const auto size = begin->size();
      minmax.first    = minmax.first < size ? minmax.first : size;
      minmax.second   = minmax.second > size ? minmax.second : size;
    }
    return minmax;
  }

} // namespace details

template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
[[nodiscard]] constexpr bool contains(const std::underlying_type_t<E> value) noexcept
{
  using T = std::underlying_type_t<E>;

  if (value < T(min<E>) || value > T(max<E>))
    return false;

  if constexpr (is_contiguous_bitflag<E>) {
    if constexpr (has_zero_flag<E>)
      if (value == 0)
        return true;
    const auto u = static_cast<std::make_unsigned_t<T>>(value);

    // std::has_single_bit
    return u != 0 && (u & (u - 1)) == 0;
  }
  else if constexpr (is_contiguous<E>) {
    return true;
  }
  else {
    for (const auto v : values_generator<E>)
      if (static_cast<T>(v) == value)
        return true;
    return false;
  }
}

template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
[[nodiscard]] constexpr bool contains(const E value) noexcept
{
  return enchantum::contains<E>(static_cast<std::underlying_type_t<E>>(value));
}

template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
[[nodiscard]] constexpr bool contains(const string_view name) noexcept
{
  constexpr auto minmax = details::minmax_string_size(names<E>.data(), names<E>.data() + names<E>.size());
  if (const auto size = name.size(); size < minmax.first || size > minmax.second)
    return false;

  for (const auto s : names_generator<E>)
    if (s == name)
      return true;
  return false;
}

template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E), typename BinaryPred>
[[nodiscard]] constexpr bool contains(const string_view name, const BinaryPred binary_pred) noexcept
{
  for (const auto s : names_generator<E>)
    if (details::call_predicate(binary_pred, name, s))
      return true;
  return false;
}

namespace details {
  template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
  struct index_to_enum_functor {
    [[nodiscard]] constexpr optional<E> operator()(const std::size_t index) const noexcept
    {
      if (index < count<E>)
        return optional<E>(values_generator<E>[index]);
      return optional<E>();
    }
  };

  struct enum_to_index_functor {
    template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
    [[nodiscard]] constexpr optional<std::size_t> operator()(const E e) const noexcept
    {
      using T = std::underlying_type_t<E>;

      if constexpr (is_contiguous<E>) {
        if (enchantum::contains(e)) {
          return optional<std::size_t>(std::size_t(T(e) - T(min<E>)));
        }
      }
      else if constexpr (is_contiguous_bitflag<E>) {
        if (enchantum::contains(e)) {
          constexpr bool has_zero = has_zero_flag<E>;
          if constexpr (has_zero)
            if (static_cast<T>(e) == 0)
              return optional<std::size_t>(0); // assumes 0 is the index of value `0`

          using U = std::make_unsigned_t<T>;
          return has_zero + details::countr_zero(static_cast<U>(e)) -
            details::countr_zero(static_cast<U>(values_generator<E>[has_zero]));
        }
      }
      else {
        for (std::size_t i = 0; i < count<E>; ++i) {
          if (values_generator<E>[i] == e)
            return optional<std::size_t>(i);
        }
      }
      return optional<std::size_t>();
    }
  };

  template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
  struct cast_functor {
    [[nodiscard]] constexpr optional<E> operator()(const std::underlying_type_t<E> value) const noexcept
    {
      if (!enchantum::contains<E>(value))
        return optional<E>();
      return optional<E>(static_cast<E>(value));
    }

    [[nodiscard]] constexpr optional<E> operator()(const string_view name) const noexcept
    {
      constexpr auto minmax = details::minmax_string_size(names<E>.data(), names<E>.data() + names<E>.size());
      if (const auto size = name.size(); size < minmax.first || size > minmax.second)
        return optional<E>(); // nullopt

      for (std::size_t i = 0; i < count<E>; ++i) {
        if (names_generator<E>[i] == name) {
          return optional<E>(values_generator<E>[i]);
        }
      }
      return optional<E>(); // nullopt
    }

    template<typename BinaryPred>
    [[nodiscard]] constexpr optional<E> operator()(const string_view name, const BinaryPred binary_pred) const noexcept
    {

      for (std::size_t i = 0; i < count<E>; ++i) {
        if (details::call_predicate(binary_pred, name, names_generator<E>[i])) {
          return optional<E>(values_generator<E>[i]);
        }
      }
      return optional<E>();
    }
  };

} // namespace details

template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
inline constexpr details::index_to_enum_functor<E> index_to_enum{};

inline constexpr details::enum_to_index_functor enum_to_index{};

template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
inline constexpr details::cast_functor<E> cast{};

namespace details {
  struct to_string_functor {
    template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
    [[nodiscard]] constexpr string_view operator()(const E value) const noexcept
    {
      if (const auto i = enchantum::enum_to_index(value))
        return names_generator<E>[*i];
      return string_view();
    }
  };

} // namespace details
inline constexpr details::to_string_functor to_string{};

} // namespace enchantum

#if defined(ENCAHNTUM_DETAILS_GCC_MAJOR) && ENCAHNTUM_DETAILS_GCC_MAJOR <= 10
  #pragma GCC diagnostic pop
#endif

#if defined(ENCAHNTUM_DETAILS_GCC_MAJOR) && ENCAHNTUM_DETAILS_GCC_MAJOR <= 10
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wconversion"
  #pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif

namespace enchantum {

template<typename E>
inline constexpr E value_ors = [] {
  static_assert(is_bitflag<E>, "");
  using T = std::underlying_type_t<E>;
  T ret{};
  for (const auto val : values_generator<E>)
    ret |= static_cast<T>(val);
  return static_cast<E>(ret);
}();

template<ENCHANTUM_DETAILS_ENUM_BITFLAG_CONCEPT(E)>
[[nodiscard]] constexpr bool contains_bitflag(const std::underlying_type_t<E> value) noexcept
{
  if constexpr (!has_zero_flag<E>)
    if (value == 0)
      return false;

  return value == (static_cast<std::underlying_type_t<E>>(value_ors<E>) & value);
}

template<ENCHANTUM_DETAILS_ENUM_BITFLAG_CONCEPT(E)>
[[nodiscard]] constexpr bool contains_bitflag(const E value) noexcept
{
  return enchantum::contains_bitflag<E>(static_cast<std::underlying_type_t<E>>(value));
}

template<ENCHANTUM_DETAILS_ENUM_BITFLAG_CONCEPT(E), typename BinaryPred>
[[nodiscard]] constexpr bool contains_bitflag(const string_view s, const char sep, const BinaryPred binary_pred) noexcept
{
  std::size_t pos = 0;
  for (std::size_t i = s.find(sep); i != s.npos; i = s.find(sep, pos)) {
    if (!enchantum::contains<E>(s.substr(pos, i - pos), binary_pred))
      return false;
    pos = i + 1;
  }
  return enchantum::contains<E>(s.substr(pos), binary_pred);
}

template<ENCHANTUM_DETAILS_ENUM_BITFLAG_CONCEPT(E)>
[[nodiscard]] constexpr bool contains_bitflag(const string_view s, const char sep = '|') noexcept
{
  std::size_t pos = 0;
  for (std::size_t i = s.find(sep); i != s.npos; i = s.find(sep, pos)) {
    if (!enchantum::contains<E>(s.substr(pos, i - pos)))
      return false;
    pos = i + 1;
  }
  return enchantum::contains<E>(s.substr(pos));
}

template<typename String = string, ENCHANTUM_DETAILS_ENUM_BITFLAG_CONCEPT(E)>
[[nodiscard]] constexpr String to_string_bitflag(const E value, const char sep = '|')
{
  using T = std::underlying_type_t<E>;
  if constexpr (has_zero_flag<E>)
    if (static_cast<T>(value) == 0)
      return String(names_generator<E>[0]);

  String name;
  T      check_value = 0;
  for (auto i = std::size_t{has_zero_flag<E>}; i < count<E>; ++i) {
    const auto v = static_cast<T>(values_generator<E>[i]);
    if (v == (static_cast<T>(value) & v)) {
      const auto s = names_generator<E>[i];
      if (!name.empty())
        name.append(1, sep);           // append separator if not the first value
      name.append(s.data(), s.size()); // not using operator += since this may not be std::string_view always
      check_value |= v;
    }
  }
  if (check_value == static_cast<T>(value))
    return name;
  return String();
}

template<ENCHANTUM_DETAILS_ENUM_BITFLAG_CONCEPT(E), typename BinaryPred>
[[nodiscard]] constexpr optional<E> cast_bitflag(const string_view s, const char sep, const BinaryPred binary_pred) noexcept
{
  using T = std::underlying_type_t<E>;
  T           check_value{};
  std::size_t pos = 0;
  for (std::size_t i = s.find(sep); i != s.npos; i = s.find(sep, pos)) {
    if (const auto v = enchantum::cast<E>(s.substr(pos, i - pos), binary_pred))
      check_value |= static_cast<T>(*v);
    else
      return optional<E>();
    pos = i + 1;
  }

  if (const auto v = enchantum::cast<E>(s.substr(pos), binary_pred))
    return optional<E>(static_cast<E>(check_value | static_cast<T>(*v)));
  return optional<E>();
}

template<ENCHANTUM_DETAILS_ENUM_BITFLAG_CONCEPT(E)>
[[nodiscard]] constexpr optional<E> cast_bitflag(const string_view s, const char sep = '|') noexcept
{
  return enchantum::cast_bitflag<E>(s, sep, [](const auto& a, const auto& b) { return a == b; });
}

template<ENCHANTUM_DETAILS_ENUM_BITFLAG_CONCEPT(E)>
[[nodiscard]] constexpr optional<E> cast_bitflag(const std::underlying_type_t<E> value) noexcept
{
  return enchantum::contains_bitflag<E>(value) ? optional<E>(static_cast<E>(value)) : optional<E>();
}

} // namespace enchantum

#if defined(ENCAHNTUM_DETAILS_GCC_MAJOR) && ENCAHNTUM_DETAILS_GCC_MAJOR <= 10
  #pragma GCC diagnostic pop
#endif

#include <string>

namespace enchantum {
namespace details {
  template<typename E>
  std::string format(E e) noexcept
  {
    if constexpr (is_bitflag<E>) {
      if (const auto name = enchantum::to_string_bitflag(e); !name.empty()) {
        if constexpr (std::is_same_v<std::string, string>) {
          return name;
        }
        else {
          return std::string(name.data(), name.size());
        }
      }
    }
    else {
      if (const auto name = enchantum::to_string(e); !name.empty())
        return std::string(name.data(), name.size());
    }
    return std::to_string(+enchantum::to_underlying(e)); // promote using + to select int overload if to underlying returns char
  }
} // namespace details
} // namespace enchantum

#include <utility>

namespace enchantum {

#if 0
namespace details {

  template<std::size_t range, std::size_t sets>
  constexpr auto cartesian_product()
  {
    constexpr auto size = []() {
      std::size_t x = range;
      std::size_t n = sets;
      while (--n != 0)
        x *= range;
      return x;
    }();

    std::array<std::array<std::size_t, sets>, size> products{};
    std::array<std::size_t, sets>                   counter{};

    for (auto& product : products) {
      product = counter;

      ++counter.back();
      for (std::size_t i = counter.size() - 1; i != 0; i--) {
        if (counter[i] != range)
          break;

        counter[i] = 0;
        ++counter[i - 1];
      }
    }
    return products;
  }

} // namespace details
#endif

#if 0
template<Enum E, std::invocable<E> Func>
constexpr auto visit(Func func, E e) 
noexcept(std::is_nothrow_invocable_v<Func, E>)
{
  using Ret = decltype(func(e));
  

  return [&]<std::size_t... Idx>(std::index_sequence<Idx...>) {
    if ((values<Enums>[Idx] == enums))
      (func(std::integral_constant<E, values<E>[Idx]> {}), ...);
  }(std::make_index_sequence<count<E>>());
}
template<Enum... Enums, std::invocable<Enums...> Func>
constexpr auto visit(Func func, Enums... enums) noexcept(std::is_nothrow_invocable_v<Func, Enums...>)
{
  using Ret = decltype(func(enums...));
  return [&]<std::size_t... Idx>(std::index_sequence<Idx...>) {
    if ((values<Enums>[Idx] == enums) && ...)
      (func(std::integral_constant<E, values<E>[Idx]> {}), ...);
  }(std::make_index_sequence<count<Enums>>()...);
}
#endif
namespace details {

  template<typename E, typename Func, std::size_t... I>
  constexpr auto for_each(Func& f, std::index_sequence<I...>)
  {
    // Clang 13 to 15 says ths syntax is invalid if I dont put more `()`
    (void)((f(std::integral_constant<E, values<E>[I]> {}), ...));
  }

} // namespace details

template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E), typename Func>
constexpr void for_each(Func f) // intentional not const
{
  details::for_each<E>(f, std::make_index_sequence<count<E>>{});
}
} // namespace enchantum

#include <array>
#include <stdexcept>

namespace enchantum {

template<typename E, typename V, typename Container = std::array<V, count<E>>>
class array : public Container {
  static_assert(std::is_enum_v<E>);
public:
  using container_type = Container;
  using index_type     = E;
  using typename Container::const_reference;
  using typename Container::reference;

  using Container::at;
  using Container::operator[];

  [[nodiscard]] constexpr reference at(const E index)
  {
    if (const auto i = enchantum::enum_to_index(index))
      return operator[](*i);
    ENCHANTUM_THROW(std::out_of_range("enchantum::array::at index out of range"), index);
  }

  [[nodiscard]] constexpr const_reference at(const E index) const
  {
    if (const auto i = enchantum::enum_to_index(index))
      return operator[](*i);
    ENCHANTUM_THROW(std::out_of_range("enchantum::array::at: index out of range"), index);
  }

  [[nodiscard]] constexpr reference operator[](const E index) noexcept
  {
    return operator[](*enchantum::enum_to_index(index));
  }

  [[nodiscard]] constexpr const_reference operator[](const E index) const noexcept
  {
    return operator[](*enchantum::enum_to_index(index));
  }
};

} // namespace enchantum

#ifndef ENCHANTUM_ALIAS_BITSET
  #include <bitset>
#endif

#include <stdexcept>

namespace enchantum {

namespace details {
#ifndef ENCHANTUM_ALIAS_BITSET
  using ::std::bitset;
#else
  ENCHANTUM_ALIAS_BITSET;
#endif
} // namespace details

template<typename E, typename Container = details::bitset<count<E>>>
class bitset : public Container {
  static_assert(std::is_enum_v<E>);
public:

  using container_type = Container;
  using typename Container::reference;

  using Container::operator[];
  using Container::flip;
  using Container::reset;
  using Container::set;
  using Container::test;

  using Container::Container;
  using Container::operator=;

  [[nodiscard]] string to_string(const char sep = '|') const
  {
    string name;
    for (std::size_t i = 0; i < enchantum::count<E>; ++i) {
      if (test(i)) {
        const auto s = enchantum::names_generator<E>[i];
        if (!name.empty())
          name += sep;
        name.append(s.data(), s.size()); // not using operator += since this may not be std::string_view always
      }
    }
    return name;
  }

  [[nodiscard]] constexpr auto to_string(const char zero, const char one) const
  {
    return Container::to_string(zero, one);
  }

  constexpr bitset(const std::initializer_list<E> values) noexcept
  {
    for (auto value : values) {
      set(value, true);
    }
  }

  [[nodiscard]] constexpr reference operator[](const E index) noexcept
  {
    return operator[](*enchantum::enum_to_index(index));
  }

  [[nodiscard]] constexpr bool operator[](const E index) const noexcept
  {
    return operator[](*enchantum::enum_to_index(index));
  }

  constexpr bool test(const E pos)
  {

    if (const auto i = enchantum::enum_to_index(pos))
      return test(*i);
    ENCHANTUM_THROW(std::out_of_range("enchantum::bitset::test(E pos,bool value) out of range exception"), pos);
  }

  constexpr bitset& set(const E pos, bool value = true)
  {

    if (const auto i = enchantum::enum_to_index(pos))
      return static_cast<bitset&>(set(*i, value));
    ENCHANTUM_THROW(std::out_of_range("enchantum::bitset::set(E pos,bool value) out of range exception"), pos);
  }

  constexpr bitset& reset(const E pos)
  {
    if (const auto i = enchantum::enum_to_index(pos))
      return static_cast<bitset&>(reset(*i));
    ENCHANTUM_THROW(std::out_of_range("enchantum::bitset::reset(E pos) out of range exception"), pos);
  }

  constexpr bitset& flip(const E pos)
  {
    if (const auto i = enchantum::enum_to_index(pos))
      return static_cast<bitset&>(flip(*i));
    ENCHANTUM_THROW(std::out_of_range("enchantum::bitset::flip(E pos) out of range exception"), pos);
  }
};

} // namespace enchantum

template<typename E>
struct std::hash<enchantum::bitset<E>> : std::hash<enchantum::details::bitset<enchantum::count<E>>> {
  using std::hash<enchantum::details::bitset<enchantum::count<E>>>::operator();
};

#include <type_traits>
/*
Note this header is an extremely easy way to cause ODR issues.

class Flags { F1 = 1 << 0,F2 = 1<< 1};
// **note I did not define any operators**

enchantum::contains(Flags::F1); // considered a classical `Enum` concept 

using namespace enchantum::bitwise_operators;

enchantum::contains(Flags::F1); // considered `BitFlagEnum` concept woops! ODR! 

*/

namespace enchantum {
namespace bitwise_operators {

  template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
  [[nodiscard]] constexpr E operator~(E e) noexcept
  {
    return static_cast<E>(~static_cast<std::underlying_type_t<E>>(e));
  }

  template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
  [[nodiscard]] constexpr E operator|(E a, E b) noexcept
  {
    using T = std::underlying_type_t<E>;
    return static_cast<E>(static_cast<T>(a) | static_cast<T>(b));
  }

  template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
  [[nodiscard]] constexpr E operator&(E a, E b) noexcept
  {
    using T = std::underlying_type_t<E>;
    return static_cast<E>(static_cast<T>(a) & static_cast<T>(b));
  }

  template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
  [[nodiscard]] constexpr E operator^(E a, E b) noexcept
  {
    using T = std::underlying_type_t<E>;
    return static_cast<E>(static_cast<T>(a) ^ static_cast<T>(b));
  }

  template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
  constexpr E& operator|=(E& a, E b) noexcept
  {
    using T  = std::underlying_type_t<E>;
    return a = static_cast<E>(static_cast<T>(a) | static_cast<T>(b));
  }

  template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
  constexpr E& operator&=(E& a, E b) noexcept
  {
    using T  = std::underlying_type_t<E>;
    return a = static_cast<E>(static_cast<T>(a) & static_cast<T>(b));
  }

  template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
  constexpr E& operator^=(E& a, E b) noexcept
  {
    using T  = std::underlying_type_t<E>;
    return a = static_cast<E>(static_cast<T>(a) ^ static_cast<T>(b));
  }

} // namespace bitwise_operators
} // namespace enchantum

#define ENCHANTUM_DEFINE_BITWISE_FOR(Enum)                                                \
  [[nodiscard]] constexpr Enum operator&(Enum a, Enum b) noexcept                         \
  {                                                                                       \
    using T = std::underlying_type_t<Enum>;                                               \
    return static_cast<Enum>(static_cast<T>(a) & static_cast<T>(b));                      \
  }                                                                                       \
  [[nodiscard]] constexpr Enum operator|(Enum a, Enum b) noexcept                         \
  {                                                                                       \
    using T = std::underlying_type_t<Enum>;                                               \
    return static_cast<Enum>(static_cast<T>(a) | static_cast<T>(b));                      \
  }                                                                                       \
  [[nodiscard]] constexpr Enum operator^(Enum a, Enum b) noexcept                         \
  {                                                                                       \
    using T = std::underlying_type_t<Enum>;                                               \
    return static_cast<Enum>(static_cast<T>(a) ^ static_cast<T>(b));                      \
  }                                                                                       \
  constexpr Enum&              operator&=(Enum& a, Enum b) noexcept { return a = a & b; } \
  constexpr Enum&              operator|=(Enum& a, Enum b) noexcept { return a = a | b; } \
  constexpr Enum&              operator^=(Enum& a, Enum b) noexcept { return a = a ^ b; } \
  [[nodiscard]] constexpr Enum operator~(Enum a) noexcept                                 \
  {                                                                                       \
    return static_cast<Enum>(~static_cast<std::underlying_type_t<Enum>>(a));              \
  }

#include <iostream>
#include <string>

namespace enchantum {
namespace iostream_operators {
  template<typename Traits, ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
  std::basic_ostream<char, Traits>& operator<<(std::basic_ostream<char, Traits>& os, const E e)
  {
    return os << details::format(e);
  }

  template<typename Traits, ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
  auto operator>>(std::basic_istream<char, Traits>& is, E& value) -> decltype((value = E{}, is))
  // sfinae to check whether value is assignable
  {
    std::basic_string<char, Traits> s;
    is >> s;
    if (!is)
      return is;

    if constexpr (is_bitflag<E>) {
      if (const auto v = enchantum::cast_bitflag<E>(s))
        value = *v;
      else
        is.setstate(std::ios_base::failbit);
    }
    else {
      if (const auto v = enchantum::cast<E>(s))
        value = *v;
      else
        is.setstate(std::ios_base::failbit);
    }
    return is;
  }
} // namespace iostream_operators
} // namespace enchantum

#include <cstddef>

#if defined(ENCAHNTUM_DETAILS_GCC_MAJOR) && ENCAHNTUM_DETAILS_GCC_MAJOR <= 10
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif

namespace enchantum {
namespace details {
  template<std::ptrdiff_t N>
  struct next_value_functor {
    template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
    [[nodiscard]] constexpr optional<E> operator()(const E value, const std::ptrdiff_t n = 1) const noexcept
    {
      if (!enchantum::contains(value))
        return optional<E>{};

      const auto index = static_cast<std::ptrdiff_t>(*enchantum::enum_to_index(value)) + (n * N);
      if (index >= 0 && index < static_cast<std::ptrdiff_t>(count<E>))
        return optional<E>{values_generator<E>[static_cast<std::size_t>(index)]};
      return optional<E>{};
    }
  };

  template<std::ptrdiff_t N>
  struct next_value_circular_functor {
    template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
    [[nodiscard]] constexpr E operator()(const E value, const std::ptrdiff_t n = 1) const noexcept
    {
      ENCHANTUM_ASSERT(enchantum::contains(value), "next/prev_value_circular requires 'value' to be a valid enum member", value);
      const auto     i     = static_cast<std::ptrdiff_t>(*enchantum::enum_to_index(value));
      constexpr auto count = static_cast<std::ptrdiff_t>(enchantum::count<E>);
      return values_generator<E>[static_cast<std::size_t>(((i + (n * N)) % count + count) % count)]; // handles wrap around and negative n
    }
  };
} // namespace details

inline constexpr details::next_value_functor<1>           next_value{};
inline constexpr details::next_value_functor<-1>          prev_value{};
inline constexpr details::next_value_circular_functor<1>  next_value_circular{};
inline constexpr details::next_value_circular_functor<-1> prev_value_circular{};

} // namespace enchantum

#if defined(ENCAHNTUM_DETAILS_GCC_MAJOR) && ENCAHNTUM_DETAILS_GCC_MAJOR <= 10
  #pragma GCC diagnostic pop
#endif

#if __has_include(<fmt/format.h>)
  

#include <fmt/format.h>

#ifdef __cpp_concepts
template<enchantum::Enum E>
struct fmt::formatter<E>
#else
template<typename E>
struct fmt::formatter<E, char, std::enable_if_t<std::is_enum_v<E>>>
#endif
: fmt::formatter<string_view> {
  template<typename FmtContext>
  constexpr auto format(const E e, FmtContext& ctx) const
  {
    return fmt::formatter<string_view>::format(enchantum::details::format(e), ctx);
  }
};
#elif (__cplusplus >= 202002 || (defined(_MSVC_LANG) && _MSVC_LANG >= 202002)) && __has_include(<format>)
  

#include <format>
#include <string_view>

template<enchantum::Enum E>
struct std::formatter<E> : std::formatter<string_view> {
  template<typename FmtContext>
  constexpr auto format(const E e, FmtContext& ctx) const
  {
    return std::formatter<string_view>::format(enchantum::details::format(e), ctx);
  }
};
#endif