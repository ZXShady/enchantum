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

#include <concepts>
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

template<typename T>
concept Enum = std::is_enum_v<T>;

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

template<Enum E>
inline constexpr bool is_bitflag = requires(E e) {
  requires std::same_as<decltype(e & e), bool> || std::same_as<decltype(e & e), E>;
  { ~e } -> std::same_as<E>;
  { e | e } -> std::same_as<E>;
  { e &= e } -> std::same_as<E&>;
  { e |= e } -> std::same_as<E&>;
};

template<typename T>
concept BitFlagEnum = Enum<T> && is_bitflag<T>;

template<typename T>
concept EnumFixedUnderlying = Enum<T> && requires { T{0}; };

template<typename T>
struct enum_traits;

template<SignedEnum E>
struct enum_traits<E> {
private:
  using U = std::underlying_type_t<E>;
  using L = std::numeric_limits<U>;
public:
  static constexpr std::size_t prefix_length = 0;

  static constexpr auto min = (L::min)() > ENCHANTUM_MIN_RANGE ? (L::min)() : ENCHANTUM_MIN_RANGE;
  static constexpr auto max = (L::max)() < ENCHANTUM_MAX_RANGE ? (L::max)() : ENCHANTUM_MAX_RANGE;
};

template<UnsignedEnum E>
struct enum_traits<E> {
private:
  using T = std::underlying_type_t<E>;
  using L = std::numeric_limits<T>;
public:
  static constexpr std::size_t prefix_length = 0;

  static constexpr auto min = []() {
    if constexpr (std::is_same_v<T, bool>)
      return false;
    else
      return (ENCHANTUM_MIN_RANGE) < 0 ? 0 : (ENCHANTUM_MIN_RANGE);
  }();
  static constexpr auto max = []() {
    if constexpr (std::is_same_v<T, bool>)
      return true;
    else
      return (L::max)() < (ENCHANTUM_MAX_RANGE) ? (L::max)() : (ENCHANTUM_MAX_RANGE);
  }();
};

} // namespace enchantum
#include <array>
#include <climits>
#include <cstddef>
#include <type_traits>

namespace enchantum::details {
template<typename T>
constexpr T Max(T a, T b)
{
  return a < b ? b : a;
}
template<typename T>
constexpr T Min(T a, T b)
{
  return a > b ? b : a;
}

template<typename E, typename = void>
inline constexpr std::size_t prefix_length_or_zero = 0;

template<typename E>
inline constexpr auto prefix_length_or_zero<E, decltype((void)enum_traits<E>::prefix_length)> = std::size_t{
  enum_traits<E>::prefix_length};

template<typename Enum, auto Min, decltype(Min) Max>
constexpr auto generate_arrays()
{
  if constexpr (BitFlagEnum<Enum>) {
    constexpr std::size_t                             bits = sizeof(Enum) * CHAR_BIT;
    std::array<Enum, bits + 1 + 1 /*1 here to make*/> ret{}; // 0 value reflected
    for (std::size_t i = 0; i < bits; ++i)
      ret[i + 1] = static_cast<Enum>(static_cast<std::make_unsigned_t<std::underlying_type_t<Enum>>>(1) << i);

    return ret;
  }
  else {
    static_assert(Min < Max, "enum_traits::min must be less than enum_traits::max");
    std::array<Enum, (Max - Min) + 1 + 1 /*+ 1 here to make */> array{};
    auto* const                                                 array_data = array.data();
    for (std::size_t i = 0, size = Max - Min + 1; i < size; ++i)
      array_data[i] = static_cast<Enum>(static_cast<decltype(Min)>(i) + Min);
    return array;
  }
}

} // namespace enchantum::details

#if defined(__NVCOMPILER)
  #include <array>
#include <cassert>
#include <climits>
#include <cstdint>
#include <type_traits>
#include <utility>

namespace enchantum {

namespace details {
  constexpr std::size_t find_semicolon(const char* const s)
  {
    for (std::size_t i = 0; true; ++i)
      if (s[i] == ';')
        return i;
  }
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
    constexpr auto funcsig_off = SZC("constexpr auto enchantum::details::var_name() noexcept [with _ Vs = _{}; ");
    return string_view(__PRETTY_FUNCTION__ + funcsig_off, SZC(__PRETTY_FUNCTION__) - funcsig_off - SZC(" _ Vs = 0]"));
  }

  template<auto Copy>
  inline constexpr auto static_storage_for = Copy;

  template<typename E, std::size_t Length, std::size_t StringLength>
  struct ReflectRetVal {
    E values[Length]{};

    // We are making an assumption that no sane user will use an enum member name longer than 256 characters
    // if you are not sane then I don't know what to do
    std::uint8_t string_lengths[Length]{};

    char        strings[StringLength]{};
    std::size_t total_string_length = 0;
    std::size_t valid_count         = 0;
  };

  template<typename E, bool NullTerminated>
  constexpr auto reflect() noexcept
  {
    constexpr auto Min      = enum_traits<E>::min;
    constexpr auto Max      = enum_traits<E>::max;
    constexpr auto bits     = (sizeof(E) * CHAR_BIT) - std::is_signed_v<E>;
    constexpr auto elements = []() {
      using Under      = std::underlying_type_t<E>;
      using Underlying = std::make_unsigned_t<std::conditional_t<std::is_same_v<bool, Under>, unsigned char, Under>>;
      constexpr auto ArraySize = is_bitflag<E> ? 1 + bits : (Max - Min) + 1;

// NVCC seems to not consider else branches of if constexpr as always returning so I have to
// disable this warning
#pragma diag_suppress implicit_return_from_non_void_function
      constexpr auto ConstStr = []<std::size_t... Idx>(std::index_sequence<Idx...>) -> std::string_view {
        // dummy 0
        constexpr struct _ {
        } A;                                           // forces NVCC to shorten the string types
        if constexpr (sizeof...(Idx) && is_bitflag<E>) // sizeof... to make contest dependant
          return details::var_name<A, static_cast<E>(0), static_cast<E>(Underlying(1) << Idx)..., 0>();
        else
          return details::var_name<A, static_cast<E>(static_cast<decltype(Min)>(Idx) + Min)..., 0>();
      }(std::make_index_sequence<is_bitflag<E> ? bits : ArraySize>());
#pragma diag_default implicit_return_from_non_void_function

      const auto* str = ConstStr.data();

      constexpr auto enum_in_array_name = details::enum_in_array_name(raw_type_name<E>, ScopedEnum<E>);
      constexpr auto enum_in_array_len  = enum_in_array_name.size();
      ReflectRetVal<E, ArraySize, ConstStr.size() + (NullTerminated * ArraySize)> ret;

      for (std::size_t index = 0; index < ArraySize; ++index) {
        str += SZC("_ Vs = ");
        // check if cast (starts with '(')
        if (*str == '(') {
          str += SZC("(") + enum_in_array_len + SZC(")0"); // there is atleast 1 base 10 digit
          str += details::find_semicolon(str) + SZC("; ");
        }
        else {
          if constexpr (enum_in_array_len != 0)
            str += enum_in_array_len + SZC("::");

          if constexpr (details::prefix_length_or_zero<E> != 0)
            str += details::prefix_length_or_zero<E>;

          const auto name_size = details::find_semicolon(str);
          {
            if constexpr (is_bitflag<E>)
              ret.values[ret.valid_count] = {index == 0 ? E() : E(Underlying{1} << (index - 1))};
            else
              ret.values[ret.valid_count] = {E(Min + static_cast<decltype(Min)>(index))};

            ret.string_lengths[ret.valid_count++] = name_size;
            
            for(std::size_t i =0;i<name_size;++i)
              ret.strings[i+ret.total_string_length] = str[i];
            // __builtin_memcpy(ret.strings + ret.total_string_length, str, name_size);
            ret.total_string_length += name_size + NullTerminated;
          }
          str += name_size + SZC("; ");
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
    for(std::size_t i=0;i<elements.valid_count;++i)
      ret.values[i] = elements.values[i];

    // __builtin_memcpy(ret.values.data(), elements.values, sizeof(ret.values));

    constexpr auto strings = [](const auto total_length, const char* data) {
      std::array<char, total_length.value> strings{};
      for(std::size_t i = 0;i<total_length.value;++i)
        strings[i] = data[i];
        // __builtin_memcpy(strings.data(), data, total_length.value);
      return strings;
    }(std::integral_constant<std::size_t, elements.total_string_length>{}, elements.strings);
    ret.strings = static_storage_for<strings>.data();

    auto* const      string_indices_data = ret.string_indices.data();
    std::size_t      i                   = 0;
    StringLengthType string_index        = 0;
    for (; i < elements.valid_count; ++i) {
      string_indices_data[i] = string_index;
      string_index += elements.string_lengths[i] + NullTerminated;
    }
    ret.string_indices[i] = string_index;
    return ret;
  }

} // namespace details
} // namespace enchantum
#elif defined(__clang__)
  

// Clang <= 12 outputs "NUMBER" if casting
// Clang > 12 outputs "(E)NUMBER".

#if defined __has_warning && __has_warning("-Wenum-constexpr-conversion")
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wenum-constexpr-conversion"
#endif

#include <array>
#include <cassert>
#include <climits>
#include <cstdint>
#include <type_traits>
#include <utility>

namespace enchantum {

#if __clang_major__ >= 20
namespace details {

  template<typename T, auto V, typename = void>
  inline constexpr bool is_valid_cast = false;

  template<typename T, auto V>
  inline constexpr bool is_valid_cast<T, V, std::void_t<std::integral_constant<T, static_cast<T>(V)>>> = true;

  template<typename T, std::underlying_type_t<T> range, decltype(range) old_range>
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
    if constexpr (is_valid_cast<T, range>)
      return valid_cast_range_recurse<T, range * 2, range>();
    else
      return old_range > 0 ? old_range * 2 - 1 : old_range;
  }
  template<typename T, std::underlying_type_t<T> max_range = 1>
  constexpr auto valid_cast_range()
  {
    using L = std::numeric_limits<decltype(max_range)>;
    if constexpr (max_range > 0 && is_valid_cast<T, (L::max())>)
      return L::max();
    else if constexpr (max_range < 0 && is_valid_cast<T, (L::min())>)
      return L::min();
    else
      return valid_cast_range_recurse<T, max_range, 0>();
  }

} // namespace details

template<UnscopedEnum E>
  requires SignedEnum<E> && (!EnumFixedUnderlying<E>)
struct enum_traits<E> {
private:
  using T = std::underlying_type_t<E>;
public:
  static constexpr auto max = details::Min(details::valid_cast_range<E>(), static_cast<T>(ENCHANTUM_MAX_RANGE));
  static constexpr decltype(max) min = details::Max(details::valid_cast_range<E, -1>(), static_cast<T>(ENCHANTUM_MIN_RANGE));
};

template<UnscopedEnum E>
  requires UnsignedEnum<E> && (!EnumFixedUnderlying<E>)
struct enum_traits<E> {
  static constexpr auto          max = details::Min(details::valid_cast_range<E>(),
                                           static_cast<std::underlying_type_t<E>>(ENCHANTUM_MAX_RANGE));
  static constexpr decltype(max) min = 0;
};
#endif

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
        auto        v = valid_cast_range<E>();
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
          str += static_cast<std::size_t>(__builtin_char_memchr(str, ',', UINT8_MAX) - str) + SZC(", ");
        }
        else {
          if constexpr (enum_in_array_len != 0)
            str += enum_in_array_len + SZC("::");

          if constexpr (details::prefix_length_or_zero<E> != 0)
            str += details::prefix_length_or_zero<E>;

          const auto name_size = static_cast<std::uint8_t>(__builtin_char_memchr(str, ',', UINT8_MAX) - str);
          {
            if constexpr (is_bitflag<E>)
              ret.values[ret.valid_count] = {index == 0 ? E() : E(Underlying{1} << (index - 1))};
            else
              ret.values[ret.valid_count] = {E(Min + static_cast<decltype(Min)>(index))};

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

#if defined __has_warning && __has_warning("-Wenum-constexpr-conversion")
  #pragma clang diagnostic pop
#endif
#undef SZC
#elif defined(__GNUC__) || defined(__GNUG__)
  

#include <array>
#include <cassert>
#include <climits>
#include <cstdint>
#include <type_traits>
#include <utility>

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
      using Under      = std::underlying_type_t<E>;
      using Underlying = std::make_unsigned_t<std::conditional_t<std::is_same_v<bool, Under>, unsigned char, Under>>;

      constexpr auto ConstStr = []<std::size_t... Idx>(std::index_sequence<Idx...>) {
        // __builtin_bit_cast used to silence errors when casting out of unscoped enums range
        // dummy 0
        if constexpr (sizeof...(Idx) && is_bitflag<E>) // sizeof... to make contest dependant
          return details::var_name<E{}, __builtin_bit_cast(E, static_cast<Under>(Underlying(1) << Idx))...,0>();
        else
          return details::var_name<__builtin_bit_cast(E, static_cast<Under>(static_cast<decltype(Min)>(Idx) + Min))...,0>();
      }(std::make_index_sequence<ArraySize - is_bitflag<E>>());
      struct RetVal {
        E            values[ArraySize];
        std::uint8_t string_lengths[ArraySize];
        char         strings[ConstStr.size()]{};
        std::size_t  total_string_length = 0;
        std::size_t  valid_count         = 0;
      } ret;
      constexpr auto enum_in_array_len = details::enum_in_array_name_size<E{}>();
      const auto* str = ConstStr.data();
      for (std::size_t index = 0; index < ArraySize; ++index) {
        if (*str == '(') {
          str += SZC("(") + length_of_enum_in_template_array_casting + SZC(")0"); // there is atleast 1 base 10 digit
          //if(!str.empty())
          //	std::cout << "after str \"" << str << '"' << '\n';

          str += static_cast<std::size_t>(std::char_traits<char>::find(str,UINT8_MAX,',') -str) + SZC(", ");

          //std::cout << "strsize \"" << str.size() << '"' << '\n';
        }
        else {
          if constexpr (enum_in_array_len != 0)
            str += enum_in_array_len + SZC("::");
          if constexpr (details::prefix_length_or_zero<E> != 0)
            str += details::prefix_length_or_zero<E>;

          // although gcc implementation of std::char_traits::find is using a for loop internally
          // copying the code of the function makes it way slower to compile, this was surprising.
          const auto commapos = static_cast<std::size_t>(std::char_traits<char>::find(str,UINT8_MAX,',') - str);
          const auto name_size = static_cast<std::uint8_t>(commapos);
          if constexpr (is_bitflag<E>)
            ret.values[ret.valid_count] = index == 0 ? E() : E(Underlying{1} << (index - 1));
          else
            ret.values[ret.valid_count] = E(Min + static_cast<decltype(Min)>(index));
          ret.string_lengths[ret.valid_count++] = name_size;
          for (std::size_t i = 0; i < name_size; ++i)
            ret.strings[ret.total_string_length++] = str[i];
          ret.total_string_length += NullTerminated;
          str += commapos + SZC(", ");
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
    auto s = string_view{__FUNCSIG__ + SZC("auto __cdecl enchantum::details::enum_in_array_name_size<"),
                         SZC(__FUNCSIG__) - SZC("auto __cdecl enchantum::details::enum_in_array_name_size<>(void) noexcept")};

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

  template<typename E, bool NullTerminated>
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

        const auto commapos = str.find_comma(); // never fails

        ret.values[ret.valid_count]           = ArrayData[index];
        ret.string_lengths[ret.valid_count++] = static_cast<std::uint8_t>(commapos);
        for (std::size_t i = 0; i < commapos; ++i)
          ret.strings[ret.total_string_length++] = str.begin[i];
        ret.total_string_length += NullTerminated;
        str.begin += commapos + 1;
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
      auto* const                    ret_data = ret.data();
      for (std::size_t i = 0; i < total_length.value; ++i)
        ret_data[i] = name_data[i];
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
#else
  #error unsupported compiler please open an issue for enchantum
#endif

#include <type_traits>
#include <utility>

namespace enchantum {

#ifdef __cpp_lib_to_underlying
using ::std::to_underlying;
#else
template<Enum E>
[[nodiscard]] constexpr auto to_underlying(const E e) noexcept
{
  return static_cast<std::underlying_type_t<E>>(e);
}
#endif

namespace details {
  template<typename E, bool NullTerminated = true>
  inline constexpr auto reflection_data = reflect<E, NullTerminated>();

  template<typename E, bool NullTerminated = true>
  inline constexpr auto reflection_string_indices = reflection_data<E, NullTerminated>.string_indices;
} // namespace details

template<Enum E, typename Pair = std::pair<E, string_view>, bool NullTerminated = true>
inline constexpr auto entries = []() {
  
#if defined(__NVCOMPILER) 
  // nvc++ had issues with that and did not allow it. it just did not work after testing in godbolt and I don't know why
  const auto             reflected = details::reflection_data<E, NullTerminated>;
#else
  const auto             reflected = details::reflection_data<std::remove_cv_t<E>, NullTerminated>;
#endif
  constexpr auto         size      = sizeof(reflected.values) / sizeof(reflected.values[0]);
  static_assert(size != 0,
    "enchantum failed to reflect this enum.\n"
    "Please read https://github.com/ZXShady/enchantum/blob/main/docs/limitations.md before opening an issue\n"
    "with your enum type with all its namespace/classes it is defined inside to help the creator debug the issues.");
  std::array<Pair, size> ret{};
  auto* const            ret_data = ret.data();

  for (std::size_t i = 0; i < size; ++i) {
    auto& [e, s]     = ret_data[i];
    e                = reflected.values[i];
    using StringView = std::remove_cvref_t<decltype(s)>;
    s                = StringView(reflected.strings + reflected.string_indices[i],
                   reflected.strings + reflected.string_indices[i + 1] - NullTerminated);
  }
  return ret;
}();

template<Enum E>
inline constexpr auto values = []() {
  constexpr auto&             enums = entries<E>;
  std::array<E, enums.size()> ret{};
  const auto* const           enums_data = enums.data();
  for (std::size_t i = 0; i < ret.size(); ++i)
    ret[i] = enums_data[i].first;
  return ret;
}();

template<Enum E, typename String = string_view, bool NullTerminated = true>
inline constexpr auto names = []() {
  constexpr auto&                  enums = entries<E, std::pair<E, String>, NullTerminated>;
  std::array<String, enums.size()> ret{};
  const auto* const                enums_data = enums.data();
  for (std::size_t i = 0; i < ret.size(); ++i)
    ret[i] = enums_data[i].second;
  return ret;
}();

template<Enum E>
inline constexpr auto min = entries<E>.front().first;

template<Enum E>
inline constexpr auto max = entries<E>.back().first;

template<Enum E>
inline constexpr std::size_t count = entries<E>.size();

template<typename>
inline constexpr bool has_zero_flag = false;

template<BitFlagEnum E>
inline constexpr bool has_zero_flag<E> = []() {
  for (const auto v : values<E>)
    if (static_cast<std::underlying_type_t<E>>(v) == 0)
      return true;
  return false;
}();

template<typename>
inline constexpr bool is_contiguous = false;

template<Enum E>
inline constexpr bool is_contiguous<E> = static_cast<std::size_t>(
                                           enchantum::to_underlying(max<E>) - enchantum::to_underlying(min<E>)) +
    1 ==
  count<E>;

template<typename E>
concept ContiguousEnum = Enum<E> && is_contiguous<E>;

template<typename>
inline constexpr bool is_contiguous_bitflag = false;

template<BitFlagEnum E>
inline constexpr bool is_contiguous_bitflag<E> = []() {
  constexpr auto& enums = entries<E>;
  using T               = std::underlying_type_t<E>;
  for (auto i = std::size_t{has_zero_flag<E>}; i < enums.size() - 1; ++i)
    if (T(enums[i].first) << 1 != T(enums[i + 1].first))
      return false;
  return true;
}();

template<typename E>
concept ContiguousBitFlagEnum = BitFlagEnum<E> && is_contiguous_bitflag<E>;

} // namespace enchantum

#include <bit>
#include <compare>
#include <cstddef>
#include <cstdint>
#include <utility>

namespace enchantum {
namespace details {

  struct senitiel {};

  template<typename CRTP, std::size_t Size>
  struct sized_iterator {
  private:
    using IndexType = std::conditional_t<(Size < INT16_MAX), std::int8_t, std::int16_t>;
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

    [[nodiscard]] constexpr friend CRTP operator-(const std::ptrdiff_t offset, CRTP it) noexcept
    {
      it -= offset;
      return it;
    }

    [[nodiscard]] constexpr friend std::ptrdiff_t operator-(const CRTP a, const CRTP b) noexcept
    {
      return a.index - b.index;
    }

    [[nodiscard]] constexpr bool operator==(const CRTP that) const noexcept { return that.index == index; };
    [[nodiscard]] constexpr auto operator<=>(const CRTP that) const noexcept { return index <=> that.index; };
    [[nodiscard]] constexpr bool operator==(senitiel) const noexcept { return Size == index; }
    [[nodiscard]] constexpr auto operator<=>(senitiel) const noexcept { return index <=> Size; }
  };

  template<typename E, typename String = string_view, bool NullTerminated = true>
  struct names_generator_t {
    [[nodiscard]] static constexpr auto size() noexcept { return count<E>; }

    static_assert(size() < INT16_MAX, "Too many enum entries");

    struct iterator : sized_iterator<iterator, size()> {
      [[nodiscard]] constexpr String operator*() const noexcept
      {
        const auto* const p       = reflection_string_indices<E, NullTerminated>.data();
        const auto* const strings = reflection_data<E, NullTerminated>.strings;
        return String(strings + p[this->index], strings + p[this->index + 1] - NullTerminated);
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
    [[nodiscard]] static constexpr auto size() noexcept { return count<E>; }

    static_assert(size() < INT16_MAX, "Too many enum entries");

    struct iterator : sized_iterator<iterator, size()> {
    public:
      [[nodiscard]] constexpr E operator*() const noexcept
      {
        using T = std::underlying_type_t<E>;

        if constexpr (is_contiguous<E>) {
          return static_cast<E>(static_cast<T>(min<E>) + static_cast<T>(this->index));
        }
        else if constexpr (is_contiguous_bitflag<E>) {
          using UT                       = std::make_unsigned_t<T>;
          constexpr auto real_min_offset = std::countr_zero(static_cast<UT>(values<E>[has_zero_flag<E>]));

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
    [[nodiscard]] static constexpr auto size() noexcept { return count<E>; }

    static_assert(size() < INT16_MAX, "Too many enum entries");

    struct iterator : sized_iterator<iterator, size()> {
    public:
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

template<Enum E, typename StringView = string_view, bool NullTerminated = true>
inline constexpr details::names_generator_t<E, StringView, NullTerminated> names_generator{};

template<Enum E>
inline constexpr details::values_generator_t<E> values_generator{};

template<Enum E, typename Pair = std::pair<E, string_view>, bool NullTerminated = true>
inline constexpr details::entries_generator_t<E, Pair, NullTerminated> entries_generator{};

} // namespace enchantum
#include <bit>
#include <type_traits>
#include <utility>

namespace enchantum {

namespace details {
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

template<Enum E>
[[nodiscard]] constexpr bool contains(const std::underlying_type_t<E> value) noexcept
{
  using T = std::underlying_type_t<E>;

  if (value < T(min<E>) || value > T(max<E>))
    return false;

  if constexpr (is_contiguous_bitflag<E>) {
    if constexpr (has_zero_flag<E>)
      if (value == 0)
        return true;

    return std::popcount(static_cast<std::make_unsigned_t<T>>(value)) == 1;
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

template<Enum E>
[[nodiscard]] constexpr bool contains(const E value) noexcept
{
  return enchantum::contains<E>(static_cast<std::underlying_type_t<E>>(value));
}

template<Enum E>
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

template<Enum E, std::predicate<string_view, string_view> BinaryPredicate>
[[nodiscard]] constexpr bool contains(const string_view name, const BinaryPredicate binary_predicate) noexcept
{
  for (const auto s : names_generator<E>)
    if (binary_predicate(name, s))
      return true;
  return false;
}

namespace details {
  template<typename E>
  struct index_to_enum_functor {
    [[nodiscard]] constexpr optional<E> operator()(const std::size_t index) const noexcept
    {
      optional<E> ret;
      if (index < count<E>)
        ret.emplace(values_generator<E>[index]);
      return ret;
    }
  };

  struct enum_to_index_functor {
    template<Enum E>
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
          return has_zero + std::countr_zero(static_cast<U>(e)) - std::countr_zero(static_cast<U>(values_generator<E>[has_zero]));
        }
      }
      else {
        for (std::size_t i = 0; i < count<E>; ++i) {
          if (values_generator<E>[i] == e)
            return i;
        }
      }
      return optional<std::size_t>();
    }
  };

  template<Enum E>
  struct cast_functor {
    [[nodiscard]] constexpr optional<E> operator()(const std::underlying_type_t<E> value) const noexcept
    {
      optional<E> a; // rvo not that it really matters
      if (!enchantum::contains<E>(value))
        return a;
      a.emplace(static_cast<E>(value));
      return a;
    }

    [[nodiscard]] constexpr optional<E> operator()(const string_view name) const noexcept
    {
      optional<E> a; // rvo not that it really matters

      constexpr auto minmax = details::minmax_string_size(names<E>.data(), names<E>.data() + names<E>.size());
      if (const auto size = name.size(); size < minmax.first || size > minmax.second)
        return a; // nullopt

      for (std::size_t i = 0; i < count<E>; ++i) {
        if (names_generator<E>[i] == name) {
          a.emplace(values_generator<E>[i]);
          return a;
        }
      }
      return a; // nullopt
    }

    template<std::predicate<string_view, string_view> BinaryPred>
    [[nodiscard]] constexpr optional<E> operator()(const string_view name, const BinaryPred binary_predicate) const noexcept
    {
      optional<E> a; // rvo not that it really matters
      for (std::size_t i = 0; i < count<E>; ++i) {
        if (binary_predicate(name, names_generator<E>[i])) {
          a.emplace(values_generator<E>[i]);
          return a;
        }
      }
      return a;
    }
  };

} // namespace details

template<Enum E>
inline constexpr details::index_to_enum_functor<E> index_to_enum{};

inline constexpr details::enum_to_index_functor enum_to_index{};

template<Enum E>
inline constexpr details::cast_functor<E> cast{};

namespace details {
  struct to_string_functor {
    template<Enum E>
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

namespace enchantum {

template<BitFlagEnum E>
inline constexpr E value_ors = [] {
  using T = std::underlying_type_t<E>;
  T ret{};
  for (const auto val : values_generator<E>)
    ret |= static_cast<T>(val);
  return static_cast<E>(ret);
}();

template<BitFlagEnum E>
[[nodiscard]] constexpr bool contains_bitflag(const std::underlying_type_t<E> value) noexcept
{
  using T = std::underlying_type_t<E>;
  if constexpr (is_contiguous_bitflag<E>) {
    return value >= static_cast<T>(min<E>) && value <= static_cast<T>(value_ors<E>);
  }
  else {
    if (value == 0)
      return has_zero_flag<E>;
    T valid_bits = 0;

    for (auto i = std::size_t{has_zero_flag<E>}; i < count<E>; ++i) {
      const auto v = static_cast<T>(values_generator<E>[i]);
      if ((value & v) == v)
        valid_bits |= v;
    }
    return valid_bits == value;
  }
}

template<BitFlagEnum E>
[[nodiscard]] constexpr bool contains_bitflag(const E value) noexcept
{
  return enchantum::contains_bitflag<E>(static_cast<std::underlying_type_t<E>>(value));
}

template<BitFlagEnum E, std::predicate<string_view, string_view> BinaryPred>
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

template<BitFlagEnum E>
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

template<typename String = string, BitFlagEnum E>
[[nodiscard]] constexpr String to_string_bitflag(const E value, const char sep = '|')
{
  using T = std::underlying_type_t<E>;
  if constexpr (has_zero_flag<E>)
    if (static_cast<T>(value) == 0)
      return String(names_generator<E>[0]);

  String name;
  T      check_value = 0;
  for (auto i = std::size_t{has_zero_flag<E>}; i < count<E>; ++i) {
    const auto v = static_cast<T>(values<E>[i]);
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

template<BitFlagEnum E, std::predicate<string_view, string_view> BinaryPred>
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

template<BitFlagEnum E>
[[nodiscard]] constexpr optional<E> cast_bitflag(const string_view s, const char sep = '|') noexcept
{
  return enchantum::cast_bitflag<E>(s, sep, [](const auto& a, const auto& b) { return a == b; });
}

template<BitFlagEnum E>
[[nodiscard]] constexpr optional<E> cast_bitflag(const std::underlying_type_t<E> value) noexcept
{
  return enchantum::contains_bitflag<E>(value) ? optional<E>(static_cast<E>(value)) : optional<E>();
}

} // namespace enchantum

#include <string>

namespace enchantum {
namespace details {
  template<Enum E>
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

#include <concepts>
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

template<Enum E, typename Func>
constexpr void for_each(Func f) // intentional not const
{
  details::for_each<E>(f, std::make_index_sequence<count<E>>{});
}
} // namespace enchantum

#include <array>
#include <stdexcept>

namespace enchantum {

template<Enum E, typename V>
class array : public std::array<V, count<E>> {
private:
  using base = std::array<V, count<E>>;
public:
  using index_type = E;
  using typename base::reference;
  using typename base::const_reference;

  using base::at;
  using base::operator[];

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

template<Enum E>
class bitset : public details::bitset<count<E>> {
private:
  using base = details::bitset<count<E>>;
public:
  using typename base::reference;

  using base::operator[];
  using base::flip;
  using base::reset;
  using base::set;
  using base::test;

  using base::base;
  using base::operator=;

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

  [[nodiscard]] constexpr auto to_string(const char zero,const char one) const
  {
    return base::to_string(zero,one);
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

namespace enchantum::bitwise_operators {
template<Enum E>
[[nodiscard]] constexpr E operator~(E e) noexcept
{
  return static_cast<E>(~static_cast<std::underlying_type_t<E>>(e));
}

template<Enum E>
[[nodiscard]] constexpr E operator|(E a, E b) noexcept
{
  using T = std::underlying_type_t<E>;
  return static_cast<E>(static_cast<T>(a) | static_cast<T>(b));
}

template<Enum E>
[[nodiscard]] constexpr E operator&(E a, E b) noexcept
{
  using T = std::underlying_type_t<E>;
  return static_cast<E>(static_cast<T>(a) & static_cast<T>(b));
}

template<Enum E>
[[nodiscard]] constexpr E operator^(E a, E b) noexcept
{
  using T = std::underlying_type_t<E>;
  return static_cast<E>(static_cast<T>(a) ^ static_cast<T>(b));
}

template<Enum E>
constexpr E& operator|=(E& a, E b) noexcept
{
  using T  = std::underlying_type_t<E>;
  return a = static_cast<E>(static_cast<T>(a) | static_cast<T>(b));
}

template<Enum E>
constexpr E& operator&=(E& a, E b) noexcept
{
  using T  = std::underlying_type_t<E>;
  return a = static_cast<E>(static_cast<T>(a) & static_cast<T>(b));
}

template<Enum E>
constexpr E& operator^=(E& a, E b) noexcept
{
  using T  = std::underlying_type_t<E>;
  return a = static_cast<E>(static_cast<T>(a) ^ static_cast<T>(b));
}

} // namespace enchantum::bitwise_operators

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

#include <concepts>
#include <iostream>
#include <string>

namespace enchantum::iostream_operators {
template<typename Traits, Enum E>
std::basic_ostream<char, Traits>& operator<<(std::basic_ostream<char, Traits>& os, const E e)
{
  return os << details::format(e);
}

template<typename Traits, Enum E>
  requires std::assignable_from<E&, E>
std::basic_istream<char, Traits>& operator>>(std::basic_istream<char, Traits>& is, E& value)
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
} // namespace enchantum::iostream_operators

#include <cstddef>

namespace enchantum {
namespace details {
  template<std::ptrdiff_t N>
  struct next_value_functor {
    template<Enum E>
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
    template<Enum E>
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

#if __has_include(<fmt/format.h>)
  

#include <fmt/format.h>

template<enchantum::Enum E>
struct fmt::formatter<E> : fmt::formatter<string_view> {
  template<typename FmtContext>
  constexpr auto format(const E e, FmtContext& ctx) const
  {
    return fmt::formatter<string_view>::format(enchantum::details::format(e), ctx);
  }
};
#elif __has_include(<format>)
  

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