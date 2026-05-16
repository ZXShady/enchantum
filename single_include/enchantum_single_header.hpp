#pragma once


#include <type_traits>
#include <utility>
#include <cstddef>

#if defined(_MSVC_LANG)
  #define ENCHANTUM_DETAILS_CXX_STD _MSVC_LANG
#else
  #define ENCHANTUM_DETAILS_CXX_STD __cplusplus
#endif
#if ENCHANTUM_DETAILS_CXX_STD >= 201703L
  #include <array>
#endif

#if ENCHANTUM_DETAILS_CXX_STD >= 201703L
  #define ENCHANTUM_DETAILS_INLINE_VAR inline
  #define ENCHANTUM_DETAILS_NODISCARD [[nodiscard]]
#else
  #define ENCHANTUM_DETAILS_INLINE_VAR
  #define ENCHANTUM_DETAILS_NODISCARD
#endif

namespace enchantum {
namespace details {

template<typename...>
using void_t = void;

template<bool B>
using bool_constant = std::integral_constant<bool, B>;

template<typename F, typename... Args>
struct is_invocable {
private:
  template<typename U>
  static auto test(int) -> decltype(std::declval<U>()(std::declval<Args>()...), std::true_type{});

  template<typename>
  static std::false_type test(...);

public:
  static constexpr bool value = decltype(test<F>(0))::value;
};

template<typename T>
struct type_identity {
  using type = T;
};

#if ENCHANTUM_DETAILS_CXX_STD < 201703L
template<typename T, std::size_t N>
struct cxx14_array {
  T elems[N == 0 ? 1 : N]{};

  constexpr T*       data() noexcept { return elems; }
  constexpr const T* data() const noexcept { return elems; }

  constexpr T&       operator[](std::size_t i) noexcept { return elems[i]; }
  constexpr const T& operator[](std::size_t i) const noexcept { return elems[i]; }

  constexpr T&       front() noexcept { return elems[0]; }
  constexpr const T& front() const noexcept { return elems[0]; }

  constexpr T&       back() noexcept { return elems[N == 0 ? 0 : N - 1]; }
  constexpr const T& back() const noexcept { return elems[N == 0 ? 0 : N - 1]; }

  constexpr T*       begin() noexcept { return elems; }
  constexpr const T* begin() const noexcept { return elems; }

  constexpr T*       end() noexcept { return elems + N; }
  constexpr const T* end() const noexcept { return elems + N; }

  constexpr std::size_t size() const noexcept { return N; }
  constexpr bool        empty() const noexcept { return N == 0; }
};
#endif

#if ENCHANTUM_DETAILS_CXX_STD >= 201703L
template<typename T, std::size_t N>
using array = std::array<T, N>;
#else
template<typename T, std::size_t N>
using array = cxx14_array<T, N>;
#endif

} // namespace details
} // namespace enchantum


#ifdef ENCHANTUM_CONFIG_FILE
  #include ENCHANTUM_CONFIG_FILE
#endif

#if !defined(ENCHANTUM_ALIAS_STRING_VIEW) && ENCHANTUM_DETAILS_CXX_STD < 201703L
  #error "C++14 users must define ENCHANTUM_ALIAS_STRING_VIEW to a string_view-compatible alias."
#endif

#ifndef ENCHANTUM_ALIAS_STRING_VIEW
  #include <string_view>
#endif

namespace enchantum {
#ifdef ENCHANTUM_ALIAS_STRING_VIEW
ENCHANTUM_ALIAS_STRING_VIEW;
#else
using ::std ::string_view;
#endif

} // namespace enchantum


#ifdef ENCHANTUM_CONFIG_FILE
  #include ENCHANTUM_CONFIG_FILE
#endif

#if !defined(ENCHANTUM_ALIAS_OPTIONAL) && ENCHANTUM_DETAILS_CXX_STD < 201703L
  #error "C++14 users must define ENCHANTUM_ALIAS_OPTIONAL to an optional-compatible template alias."
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
#include <type_traits>
#include <utility>

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

template<typename T, bool = std::is_enum<T>::value>
ENCHANTUM_DETAILS_INLINE_VAR constexpr bool is_scoped_enum = false;

template<typename E>
ENCHANTUM_DETAILS_INLINE_VAR constexpr bool is_scoped_enum<E, true> = !std::is_convertible<E, std::underlying_type_t<E>>::value;

template<typename E>
ENCHANTUM_DETAILS_INLINE_VAR constexpr bool is_unscoped_enum = std::is_enum<E>::value && !is_scoped_enum<E>;

template<typename E, typename = void>
ENCHANTUM_DETAILS_INLINE_VAR constexpr bool has_fixed_underlying_type = false;

template<typename E>
ENCHANTUM_DETAILS_INLINE_VAR constexpr bool has_fixed_underlying_type<E, decltype(void(E{0}))> = std::is_enum<E>::value;


#ifdef __cpp_concepts

template<typename T>
concept Enum = std::is_enum<T>::value;

template<Enum E>
ENCHANTUM_DETAILS_INLINE_VAR constexpr bool is_bitflag = requires(E e) {
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
concept ScopedEnum = Enum<T> && (!std::is_convertible<T, std::underlying_type_t<T>>::value);

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
ENCHANTUM_DETAILS_INLINE_VAR constexpr bool is_bitflag = false;

// clang-format off
template<typename E>
ENCHANTUM_DETAILS_INLINE_VAR constexpr bool is_bitflag<E,
    enchantum::details::void_t<
    decltype(E{} & E{}),
    decltype(~E{}),
    decltype(E{} | E{}),
    decltype(std::declval<E&>() &= E{}),
    decltype(std::declval<E&>() |= E{})
    >> =  std::is_enum<E>::value
    &&    (std::is_same<decltype(E{} & E{}),bool>::value  || std::is_same<decltype(E{} & E{}), E>::value)
    &&    std::is_same<decltype(~E{}), E>::value
    &&    std::is_same<decltype(E{} | E{}), E>::value
    &&    std::is_same<decltype(std::declval<E&>() &= E{}), E&>::value
    &&    std::is_same<decltype(std::declval<E&>() |= E{}), E&>::value
    ;
// clang-format on
#endif


namespace details {
  template<typename T, typename U>
  constexpr auto Max(T a, U b)
  {
    using R = typename std::common_type<T, U>::type;
    return static_cast<R>(a) < static_cast<R>(b) ? static_cast<R>(b) : static_cast<R>(a);
  }

  template<typename T, typename U>
  constexpr auto Min(T a, U b)
  {
    using R = typename std::common_type<T, U>::type;
    return static_cast<R>(a) > static_cast<R>(b) ? static_cast<R>(b) : static_cast<R>(a);
  }
#if ENCHANTUM_DETAILS_CXX_STD >= 201703L && !defined(__NVCOMPILER) && defined(__clang__) && __clang_major__ >= 20
  template<typename E, auto V, typename = void>
  ENCHANTUM_DETAILS_INLINE_VAR constexpr bool is_valid_cast = false;

  template<typename E, auto V>
  ENCHANTUM_DETAILS_INLINE_VAR constexpr bool is_valid_cast<E, V, enchantum::details::void_t<std::integral_constant<E, static_cast<E>(V)>>> = true;

  template<typename E, std::underlying_type_t<E> range, decltype(range) old_range>
  constexpr auto valid_cast_range_recurse() noexcept;

  template<typename E, std::underlying_type_t<E> range, decltype(range) old_range, bool IsValid>
  struct valid_cast_range_recurse_impl;

  template<typename E, std::underlying_type_t<E> range, decltype(range) old_range>
  struct valid_cast_range_recurse_impl<E, range, old_range, true> {
    static constexpr auto value() noexcept { return valid_cast_range_recurse<E, range * 2, range>(); }
  };

  template<typename E, std::underlying_type_t<E> range, decltype(range) old_range>
  struct valid_cast_range_recurse_impl<E, range, old_range, false> {
    static constexpr auto value() noexcept { return old_range > 0 ? old_range * 2 - 1 : old_range; }
  };

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
    return valid_cast_range_recurse_impl<E, range, old_range, is_valid_cast<E, range>>::value();
  }

  template<typename E, int max_range>
  constexpr auto valid_cast_range_impl(std::integral_constant<int, 0>) noexcept
  {
    using T = std::underlying_type_t<E>;
    return T{0};
  }

  template<typename E, int max_range>
  constexpr auto valid_cast_range_impl(std::integral_constant<int, 1>) noexcept
  {
    using T = std::underlying_type_t<E>;
    using L = std::numeric_limits<T>;
    return is_valid_cast<E, (L::max)()> ? L::max() : details::valid_cast_range_recurse<E, max_range, 0>();
  }

  template<typename E, int max_range>
  constexpr auto valid_cast_range_impl(std::integral_constant<int, -1>) noexcept
  {
    using T = std::underlying_type_t<E>;
    using L = std::numeric_limits<T>;
    return is_valid_cast<E, (L::min)()> ? L::min() : details::valid_cast_range_recurse<E, max_range, 0>();
  }

  template<typename E, int max_range>
  constexpr auto valid_cast_range() noexcept
  {
    return details::valid_cast_range_impl<E, max_range>(std::integral_constant<int, (max_range > 0) - (max_range < 0)>{});
  }

#endif

  template<typename E>
  constexpr auto enum_range_of_bool(const int max_range, std::true_type)
  {
    return max_range > 0;
  }

  template<typename E>
  constexpr auto enum_range_of_signed(const int max_range, std::true_type)
  {
    using T = std::underlying_type_t<E>;
    using L = std::numeric_limits<T>;
#if ENCHANTUM_DETAILS_CXX_STD >= 201703L && !defined(__NVCOMPILER) && defined(__clang__) && __clang_major__ >= 20
    constexpr auto Max = has_fixed_underlying_type<E> ? (L::max)() : details::valid_cast_range<E, 1>();
    constexpr auto Min = has_fixed_underlying_type<E> ? (L::min)() : details::valid_cast_range<E, -1>();
#else
    constexpr auto Max = (L::max)();
    constexpr auto Min = (L::min)();
#endif
    return max_range > 0 ? details::Min(ENCHANTUM_MAX_RANGE, Max) : details::Max(ENCHANTUM_MIN_RANGE, Min);
  }

  template<typename E>
  constexpr auto enum_range_of_signed(const int max_range, std::false_type)
  {
    using T = std::underlying_type_t<E>;
    using L = std::numeric_limits<T>;
#if ENCHANTUM_DETAILS_CXX_STD >= 201703L && !defined(__NVCOMPILER) && defined(__clang__) && __clang_major__ >= 20
    constexpr auto Max = has_fixed_underlying_type<E> ? (L::max)() : details::valid_cast_range<E, 1>();
#else
    constexpr auto Max = (L::max)();
#endif
    return max_range > 0 ? details::Min(static_cast<unsigned int>(ENCHANTUM_MAX_RANGE), Max) : 0;
  }

  template<typename E>
  constexpr auto enum_range_of_bool(const int max_range, std::false_type)
  {
    using T = std::underlying_type_t<E>;
    return enum_range_of_signed<E>(max_range, std::integral_constant<bool, std::is_signed<T>::value>{});
  }

  template<typename E>
  constexpr auto enum_range_of(const int max_range)
  {
    using T = std::underlying_type_t<E>;
    return enum_range_of_bool<E>(max_range, std::is_same<bool, T>{});
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
  ENCHANTUM_DETAILS_INLINE_VAR constexpr bool has_specialized_traits = true;
  template<typename T>
  ENCHANTUM_DETAILS_INLINE_VAR constexpr bool has_specialized_traits<T, typename enum_traits<T>::zxshady_enchantum_is_not_specialized_tag> = false;

} // namespace details

} // namespace enchantum

#ifdef __cpp_concepts
  #define ENCHANTUM_DETAILS_ENUM_CONCEPT(Name)         Enum Name
  #define ENCHANTUM_DETAILS_ENUM_BITFLAG_CONCEPT(Name) BitFlagEnum Name
#else
  #define ENCHANTUM_DETAILS_ENUM_CONCEPT(Name)         typename Name, std::enable_if_t<std::is_enum<Name>::value, int> = 0
  #define ENCHANTUM_DETAILS_ENUM_BITFLAG_CONCEPT(Name) typename Name, std::enable_if_t<is_bitflag<Name>, int> = 0
#endif
#include <cstddef>

namespace enchantum {

namespace details {
#define SZC(x) (sizeof(x) - 1)
  constexpr string_view extract_name_from_type_name(const string_view type_name) noexcept
  {
    const auto n = type_name.rfind(':');
    if (n != type_name.npos)
      return type_name.substr(n + 1);
    else
      return type_name;
  }

  template<typename T>
  constexpr auto raw_type_name_func() noexcept
  {
#if defined(__RESHARPER__)
      constexpr std::size_t prefix =0;
    constexpr auto s = string_view(__rscpp_type_name<T>());
#elif defined(__NVCOMPILER)
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
    constexpr auto prefix = std::is_enum<T>::value ? SZC("enum ") :
        std::is_class<T>::value ?  SZC("struct ") - (s[0] == 'c') :
        0;
// clang-format on
#elif defined(__GNUG__)
    constexpr std::size_t prefix = 0;
    constexpr auto        s      = string_view(__PRETTY_FUNCTION__ +
                                     SZC("constexpr auto enchantum::details::raw_type_name_func() [with _ = "),
                                   SZC(__PRETTY_FUNCTION__) -
                                     SZC("constexpr auto enchantum::details::raw_type_name_func() [with _ = ]"));
#endif
    details::array<char, 1 + s.size() - prefix> ret{};
    auto* const                             ret_data = ret.data();
    const auto* const                       s_data   = s.data();

    for (std::size_t i = 0; i < ret.size() - 1; ++i)
      ret_data[i] = s_data[i + prefix];
    return ret;
  }

  template<typename T>
  ENCHANTUM_DETAILS_INLINE_VAR constexpr auto raw_type_name_func_var = raw_type_name_func<T>();


  template<typename T>
  constexpr auto type_name_func() noexcept
  {
    static_assert(!std::is_function<std::remove_pointer_t<T>>::value && !std::is_member_function_pointer<T>::value,
                  "enchantum::type_name<T> does not work well with function pointers or functions or member function\n"
                  "pointers");

    constexpr auto& array = raw_type_name_func_var<T>;
    static_assert(array[array.size() - 2] != '>', "enchantum::type_name<T> does not work well with a templated type");

    constexpr auto  s     = details::extract_name_from_type_name(string_view(array.data(), array.size() - 1));
    details::array<char, s.size() + 1> ret{};
    for (std::size_t i = 0; i < s.size(); ++i)
      ret[i] = s[i];
    return ret;
  }

  template<typename T>
  ENCHANTUM_DETAILS_INLINE_VAR constexpr auto type_name_func_var = type_name_func<T>();

#undef SZC

} // namespace details

template<typename T>
ENCHANTUM_DETAILS_INLINE_VAR constexpr auto type_name = string_view(details::type_name_func_var<T>.data(),
                                                                    details::type_name_func_var<T>.size() - 1);

template<typename T>
ENCHANTUM_DETAILS_INLINE_VAR constexpr auto raw_type_name = string_view(details::raw_type_name_func_var<T>.data(),
                                                                        details::raw_type_name_func_var<T>.size() - 1);


} // namespace enchantum
#include <array>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace enchantum {
namespace details {


  template<typename E, typename = void>
  ENCHANTUM_DETAILS_INLINE_VAR constexpr std::size_t prefix_length_or_zero = 0;

  template<typename E>
  ENCHANTUM_DETAILS_INLINE_VAR constexpr auto prefix_length_or_zero<E, decltype((void)enum_traits<E>::prefix_length)> = std::size_t{
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
#if defined(__RESHARPER__)

#include <cstdint>
#include <initializer_list>
#include <type_traits>
#include <utility>

namespace enchantum {
namespace details {

  // WORKAROUND
  // resharper seems to not copy values of arrays correctly in constexpr contexts.
  // it copies the last element of the array to the WHOLE array
  // giving the array a default value other than default-init fixes the issue
  // as for why 'Count' is explicitly taken although it is equal to sizeof...(Is)
  // is to workaround another bug, which seems to think sizeof...(Is) is 0
  template<std::size_t Count, typename Value, std::size_t... Is>
  constexpr auto rscpp_make_defaulted_array_of(const Value value, std::index_sequence<Is...>)
  {
    return details::array<Value, Count>{(Is, void(), value)...};
  }


  template<typename E, bool NullTerminated, typename MinT, MinT Min, std::size_t... Is>
  constexpr auto reflect_elements(std::index_sequence<Is...>) noexcept
  {
    using T    = std::underlying_type_t<E>;
    using U    = std::make_unsigned_t<std::conditional_t<std::is_same<bool, T>::value, unsigned char, T>>;
    constexpr bool        IsBitFlag    = is_bitflag<E>;
    constexpr std::size_t max_elements = sizeof...(Is) + IsBitFlag;

    const char* names[max_elements]{};
    T           values[max_elements]{};
    std::size_t count = 0;

    if (IsBitFlag) {
      const auto* zero_name = __rscpp_enumerator_name(E(0));
      if (zero_name) {
        names[count]    = zero_name;
        values[count++] = 0;
      }

      for (std::size_t i : {Is...}) {
        const auto val  = T(U(1) << i);
        const auto name = __rscpp_enumerator_name(E(val));
        if (name) {
          names[count]    = name;
          values[count++] = val;
        }
      }
    }
    else {
      for (std::size_t i = 0; i < max_elements; ++i) {
        const auto val  = T(MinT(i) + Min);
        const auto name = __rscpp_enumerator_name(E(val));
        if (name) {
          names[count]    = name;
          values[count++] = val;
        }
      }
    }

    auto ret = ReflectStringReturnValue<T, max_elements>{};
    for (std::size_t i = 0; i < count; ++i) {
      const auto        str = names[i] + prefix_length_or_zero<E>;
      const std::size_t len = __builtin_strlen(str);
      ret.values[i]         = values[i];
      ret.string_lengths[i] = len;
      for (std::size_t j = 0; j < len; ++j)
        ret.strings[ret.total_string_length + j] = str[j];
      ret.total_string_length += len + (NullTerminated ? 1 : 0);
    }
    ret.valid_count = count;
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

    const auto size = data.strings.size();
    const auto str  = data.strings.data();
    for (std::size_t i = 0; i < size; ++i)
      str[i] = elements_local.strings[i];
    return data;
  }

} // namespace details
} // namespace enchantum
#elif defined(__NVCOMPILER)
#include <cassert>
#include <climits>
#include <cstdint>
#include <type_traits>
#include <utility>
namespace enchantum {

namespace details {
#define SZC(x) (sizeof(x) - 1)

  constexpr std::size_t find_semicolon(const char* s)
  {
    for (std::size_t i = 0; true; ++i)
      if (s[i] == ';')
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
#elif defined(__clang__)

// Clang <= 12 outputs "NUMBER" if casting
// Clang > 12 outputs "(E)NUMBER".

#if defined __has_warning
  #if __has_warning("-Wenum-constexpr-conversion")
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wenum-constexpr-conversion"
  #endif
#endif

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

#if ENCHANTUM_DETAILS_CXX_STD < 201703L
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
#else
  template<auto... Vs>
  constexpr auto var_name() noexcept
  {
    // "auto enchantum::details::var_name() [Vs = <(A)0, a, b, c, e, d, (A)6>]"
    return __PRETTY_FUNCTION__ + SZC("auto enchantum::details::var_name() [Vs = <");
  }
#endif


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
#if ENCHANTUM_DETAILS_CXX_STD >= 201703L
    return details::var_name<static_cast<E>(false), static_cast<E>(Underlying(1) << Is)..., 0>();
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
#if ENCHANTUM_DETAILS_CXX_STD >= 201703L
    const auto     str       = details::var_name<static_cast<E>(static_cast<MinT>(Is) + Min)..., 0>();
#else
    const auto     str       = details::var_name<E, static_cast<E>(static_cast<MinT>(Is) + Min)..., E{}>();
#endif

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
#elif defined(__GNUC__) || defined(__GNUG__)
#include <cassert>
#include <climits>
#include <cstdint>
#include <type_traits>
#include <utility>

#if defined(__has_include) && __has_include(<bits/char_traits.h>)
  #include <bits/char_traits.h>
#endif

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
#if ENCHANTUM_DETAILS_CXX_STD >= 201703L
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
    if constexpr (!std::is_convertible<E, std::underlying_type_t<E>>::value) {
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
#else
  template<typename E, E Enum>
  constexpr auto enum_in_array_name_size() noexcept
  {
    // constexpr auto f() [with auto _ = (
    //constexpr auto f() [with auto _ = (Scoped)0]
    auto s  = string_view(__PRETTY_FUNCTION__ +
                            SZC("constexpr auto enchantum::details::enum_in_array_name_size() [with E = "),
                          SZC(__PRETTY_FUNCTION__) -
                            SZC("constexpr auto enchantum::details::enum_in_array_name_size() [with E = ]"));
    std::size_t enum_pos = 0;
    while (s[enum_pos] != ';')
      ++enum_pos;
    s.remove_prefix(enum_pos + SZC("; E Enum = "));
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
#endif

#if __GNUC__ == 10
  #if ENCHANTUM_DETAILS_CXX_STD >= 201703L
  template<auto V>
  constexpr auto gcc10_workaround() noexcept
  {
    using E               = decltype(V);
    using T               = std::underlying_type_t<E>;
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
  #else
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
#endif

#if ENCHANTUM_DETAILS_CXX_STD >= 201703L
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
#else
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
#endif

#if ENCHANTUM_DETAILS_CXX_STD < 201703L
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
#else
  template<auto... Vs>
  constexpr auto var_name() noexcept
  {
    return __PRETTY_FUNCTION__ + SZC("constexpr auto enchantum::details::var_name() [with auto ...Vs = {");
  }
#endif


  constexpr bool is_out_of_range_parse(const char* str, const std::size_t least_length_when_casting, const std::size_t array_size)
  {
    for (std::size_t index = 0; index < array_size; ++index) {
      if (*str == '(') {
#if ENCHANTUM_DETAILS_CXX_STD >= 201703L
        const auto comma = std::char_traits<char>::find(str + least_length_when_casting, UINT8_MAX, ',');
        if (comma == nullptr || *comma == '\0')
          return false;
        str = comma + SZC(", ");
#else
        const auto comma = details::find_char(str + least_length_when_casting, UINT8_MAX, ',');
        if (comma == nullptr || *comma == '\0')
          return false;
        str = comma + SZC(", ");
#endif
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
#if ENCHANTUM_DETAILS_CXX_STD >= 201703L
        const auto comma = std::char_traits<char>::find(str + least_length_when_casting, UINT8_MAX, ',');
        if (comma == nullptr || *comma == '\0')
          return;
        str = comma + SZC(", ");
#else
        const auto comma = details::find_char(str + least_length_when_casting, UINT8_MAX, ',');
        if (comma == nullptr || *comma == '\0')
          return;
        str = comma + SZC(", ");
#endif
      }
      else {
        str += least_length_when_value;
#if ENCHANTUM_DETAILS_CXX_STD >= 201703L
        const auto comma = std::char_traits<char>::find(str, UINT8_MAX, ',');
        if (comma == nullptr)
          return;
        const auto commapos = static_cast<std::size_t>(comma - str);
#else
        const auto comma = details::find_char(str, UINT8_MAX, ',');
        if (comma == nullptr)
          return;
        const auto commapos = static_cast<std::size_t>(comma - str);
#endif
        if (IsBitFlag)
          values[valid_count] = index == 0 ? IntType{} : static_cast<IntType>(IntType{1} << (index - 1));
        else
          values[valid_count] = static_cast<IntType>(min + static_cast<IntType>(index));
        string_lengths[valid_count++] = static_cast<std::uint8_t>(commapos);
        for (std::size_t i = 0; i < commapos; ++i)
          strings[total_string_length++] = str[i];
        total_string_length += null_terminated;
#if ENCHANTUM_DETAILS_CXX_STD >= 201703L
        str += commapos + SZC(", ");
#else
        if (*comma == '\0')
          return;
        str = comma + SZC(", ");
#endif
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
#if ENCHANTUM_DETAILS_CXX_STD >= 201703L
    return details::var_name<E{}, CAST(E, static_cast<typename std::underlying_type<E>::type>(Underlying{1} << Is))..., 0>();
#else
    return details::var_name<E, E{}, CAST(E, static_cast<typename std::underlying_type<E>::type>(Underlying{1} << Is))..., E{}>();
#endif
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
#if ENCHANTUM_DETAILS_CXX_STD >= 201703L
    return details::var_name<CAST(E, static_cast<typename std::underlying_type<E>::type>(static_cast<MinT>(Is) + Min))..., 0>();
#else
    return details::var_name<E, CAST(E, static_cast<typename std::underlying_type<E>::type>(static_cast<MinT>(Is) + Min))..., E{}>();
#endif
#undef CAST
  }

  template<typename E, bool NullTerminated, typename MinT, MinT Min, std::size_t... Is>
  constexpr auto reflect_elements(std::index_sequence<Is...>) noexcept
  {
    constexpr auto ArraySize = sizeof...(Is) + is_bitflag<E>;
    using Under              = std::underlying_type_t<E>;
    using Underlying = std::make_unsigned_t<std::conditional_t<std::is_same<bool, Under>::value, unsigned char, Under>>;

    constexpr auto str = details::reflect_var_name<E, MinT, Min, Underlying, Is...>(std::integral_constant<bool, is_bitflag<E>>{});

#if ENCHANTUM_DETAILS_CXX_STD >= 201703L
    constexpr auto enum_in_array_len = details::enum_in_array_name_size<E{}>();
#else
    constexpr auto enum_in_array_len = details::enum_in_array_name_size<E, E{}>();
#endif
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
#if ENCHANTUM_DETAILS_CXX_STD >= 201703L
    constexpr auto str = details::var_name<CAST(E, static_cast<Under>(static_cast<MinT>(Is) + Min))..., 0>();
#else
    constexpr auto str = details::var_name<E, CAST(E, static_cast<Under>(static_cast<MinT>(Is) + Min))..., E{}>();
#endif
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
#elif defined(_MSC_VER)
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
#else
  #error unsupported compiler please open an issue for enchantum
#endif

#include <climits>
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
ENCHANTUM_DETAILS_NODISCARD constexpr auto to_underlying(const E e) noexcept
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
    details::array<E, Size> values{};
    // +1 for easier iteration on on last string
    details::array<StringLengthType, Size + 1> string_indices{};
  };

  template<typename E,
           bool NullTerminated,
           typename T = typename std::underlying_type<E>::type,
           T Min = static_cast<T>(enum_traits<E>::min),
           T Max = static_cast<T>(enum_traits<E>::max)>
  ENCHANTUM_DETAILS_INLINE_VAR constexpr auto reflection_data_impl = details::reflect<E, NullTerminated, T, Min>(
    std::make_index_sequence<details::get_index_sequence_max(is_bitflag<E>,
                                                              has_fixed_underlying_type<E>,
                                                              sizeof(E),
                                                              Min,
                                                              Max,
                                                               std::is_signed<T>::value)>{});


  template<typename E,
           typename T = typename std::underlying_type<E>::type,
           T Min = static_cast<T>(enum_traits<E>::min),
           T Max = static_cast<T>(enum_traits<E>::max)>
  ENCHANTUM_DETAILS_INLINE_VAR constexpr bool has_a_value_in = details::is_out_of_range<E, T, Min>(
    std::make_index_sequence<
      details::get_index_sequence_max(false, has_fixed_underlying_type<E>, sizeof(E), Min, Max, std::is_signed<T>::value)>{});


  // Thanks https://en.cppreference.com/w/cpp/utility/intcmp.html
  template<typename T, typename U>
  constexpr bool cmp_less_impl(const T t, const U u, std::true_type, std::false_type) noexcept
  {
    return t < u;
  }

  template<typename T, typename U>
  constexpr bool cmp_less_impl(const T t, const U u, std::false_type, std::true_type) noexcept
  {
    return t < 0 || std::make_unsigned_t<T>(t) < u;
  }

  template<typename T, typename U>
  constexpr bool cmp_less_impl(const T t, const U u, std::false_type, std::false_type) noexcept
  {
    return u >= 0 && t < std::make_unsigned_t<U>(u);
  }

  template<typename T, typename U>
  constexpr bool cmp_less(const T t, const U u) noexcept
  {
    return details::cmp_less_impl(t,
                                  u,
                                  std::integral_constant<bool, std::is_signed<T>::value == std::is_signed<U>::value>{},
                                  std::integral_constant<bool, std::is_signed<T>::value>{});
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

  constexpr bool cmp_less(const bool t, const bool u) noexcept { return int(t) < int(u); }

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

  template<typename E, bool NullTerminated, typename T, T Min, T ScaleMin, bool MinIsNegative>
  struct lower_out_of_bounds_checker {
    static constexpr void check() noexcept
    {
      static_assert(!has_a_value_in<E, T, ScaleMin, Min - 1>,
                    "enchantum has detected that this enum is not fully reflected. Please look at "
                    "https://github.com/ZXShady/enchantum/blob/main/docs/"
                    "features.md#enchantum_check_out_of_bounds_by "
                    "for more information");
    }
  };

  template<typename E, bool NullTerminated, typename T, T Min, T ScaleMin>
  struct lower_out_of_bounds_checker<E, NullTerminated, T, Min, ScaleMin, false> {
    static constexpr void check() noexcept
    {
      static_assert(!has_a_value_in<E, T, Min + 1, ScaleMin>,
                    "enchantum has detected that this enum is not fully reflected. Please look at "
                    "https://github.com/ZXShady/enchantum/blob/main/docs/"
                    "features.md#enchantum_check_out_of_bounds_by "
                    "for more information");
    }
  };

  template<typename E, bool NullTerminated, typename T, T Min, bool CanCheckLower>
  struct maybe_lower_out_of_bounds_checker {
    static constexpr void check() noexcept {}
  };

  template<typename E, bool NullTerminated, typename T, T Min>
  struct maybe_lower_out_of_bounds_checker<E, NullTerminated, T, Min, true> {
    static constexpr void check() noexcept
    {
      constexpr auto scale = ENCHANTUM_CHECK_OUT_OF_BOUNDS_BY;
      lower_out_of_bounds_checker<E, NullTerminated, T, Min, Min * scale, (Min < 0)>::check();
    }
  };

  template<typename E, bool NullTerminated, typename T, T Max, bool CanCheckUpper>
  struct upper_out_of_bounds_checker {
    static constexpr void check() noexcept {}
  };

  template<typename E, bool NullTerminated, typename T, T Max>
  struct upper_out_of_bounds_checker<E, NullTerminated, T, Max, true> {
    static constexpr void check() noexcept
    {
      constexpr auto scale           = ENCHANTUM_CHECK_OUT_OF_BOUNDS_BY;
      constexpr bool upper_has_value = has_a_value_in<E, T, Max + 1, Max * scale>;
      static_assert(!upper_has_value,
                    "enchantum has detected that this enum is not fully reflected. Please look at "
                    "https://github.com/ZXShady/enchantum/blob/main/docs/"
                    "features.md#enchantum_check_out_of_bounds_by "
                    "for more information");
      constexpr auto min             = static_cast<T>(enum_traits<E>::min);
      constexpr auto tmin            = std::numeric_limits<T>::min();
      constexpr bool can_check_lower = !upper_has_value && min > tmin && min >= tmin / scale;
      maybe_lower_out_of_bounds_checker<E, NullTerminated, T, min, can_check_lower>::check();
    }
  };

  template<typename E, bool NullTerminated, std::size_t ValidCount, bool ShouldCheck>
  struct out_of_bounds_checker {
    static constexpr void check() noexcept {}
  };

  template<typename E, bool NullTerminated, std::size_t ValidCount>
  struct out_of_bounds_checker<E, NullTerminated, ValidCount, true> {
    static constexpr void check() noexcept
    {
#if defined(__NVCOMPILER) || defined(__RESHARPER__)
      static_assert(ValidCount == reflection_data_impl<E, NullTerminated, typename std::underlying_type<E>::type,
        details::ClampToRange<typename std::underlying_type<E>::type>(enum_traits<E>::min * ENCHANTUM_CHECK_OUT_OF_BOUNDS_BY),
        details::ClampToRange<typename std::underlying_type<E>::type>(enum_traits<E>::max * ENCHANTUM_CHECK_OUT_OF_BOUNDS_BY)
      >.elements.valid_count,
          "enchantum has detected that this enum is not fully reflected. Please look at "
          "https://github.com/ZXShady/enchantum/blob/main/docs/"
          "features.md#enchantum_check_out_of_bounds_by "
          "for more information");
#else
      using T                        = std::underlying_type_t<E>;
      constexpr auto max             = static_cast<T>(enum_traits<E>::max);
      constexpr auto scale           = ENCHANTUM_CHECK_OUT_OF_BOUNDS_BY;
      constexpr auto tmax            = std::numeric_limits<T>::max();
      constexpr bool can_check_upper = max < tmax && max <= tmax / scale;
      upper_out_of_bounds_checker<E, NullTerminated, T, max, can_check_upper>::check();
#endif
    }
  };

  template<typename E, bool NullTerminated>
  constexpr auto get_reflection_data() noexcept
  {
    constexpr auto elements = reflection_data_impl<E, NullTerminated>.elements;
    using StringLengthType = std::conditional_t<(elements.total_string_length < UINT8_MAX), std::uint8_t, std::uint16_t>;
#if ENCHANTUM_CHECK_OUT_OF_BOUNDS_BY >= 2
    details::out_of_bounds_checker<E,
                                   NullTerminated,
                                   elements.valid_count,
#if __clang_major__ >= 20
                                   has_fixed_underlying_type<E> &&
#endif
                                     !details::has_specialized_traits<E> && !is_bitflag<E> &&
                                     !std::is_same<std::underlying_type_t<E>, bool>::value>::check();
#endif
    FinalReflectionResult<E, StringLengthType, elements.valid_count> ret;
    std::size_t                                                      i            = 0;
    StringLengthType                                                 string_index = 0;
    for (; i < elements.valid_count; ++i) {
      ret.values[i] = static_cast<E>(elements.values[i]);
      // "aabc"

      ret.string_indices[i] = string_index;
#if defined(ENCHANTUM_DETAILS_GCC_MAJOR) && ENCHANTUM_DETAILS_GCC_MAJOR <= 10
      // false positives from T += T
      // it does not make sense.
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wconversion"
#endif
      string_index += static_cast<StringLengthType>(elements.string_lengths[i] + NullTerminated);
#if defined(ENCHANTUM_DETAILS_GCC_MAJOR) && ENCHANTUM_DETAILS_GCC_MAJOR <= 10
  #pragma GCC diagnostic pop
#endif
    }
    ret.string_indices[i] = string_index;
    return ret;
  }


  template<typename E, bool NullTerminated>
  ENCHANTUM_DETAILS_INLINE_VAR constexpr auto reflection_data_string_storage = details::reflection_data_impl<E, NullTerminated>.strings;

  template<typename E, bool NullTerminated>
  ENCHANTUM_DETAILS_INLINE_VAR constexpr auto reflection_data = details::get_reflection_data<E, NullTerminated>();

  template<typename E, bool NullTerminated>
  ENCHANTUM_DETAILS_INLINE_VAR constexpr auto reflection_string_indices = reflection_data<E, NullTerminated>.string_indices;

  template<typename Pair, typename = void>
  struct has_first_second : std::false_type {
  };

  template<typename Pair>
  struct has_first_second<Pair, void_t<decltype(std::declval<Pair&>().first), decltype(std::declval<Pair&>().second)>>
    : std::true_type {
  };

  template<typename Pair, typename E>
  constexpr void assign_entry_impl(Pair& entry, E value, const char* string, std::size_t size, std::true_type)
  {
    using StringView = typename std::remove_cv<typename std::remove_reference<decltype(entry.second)>::type>::type;
    entry.first      = value;
    entry.second     = StringView(string, size);
  }

  template<typename Pair, typename E>
  constexpr void assign_entry_impl(Pair& entry, E value, const char* string, std::size_t size, std::false_type)
  {
    entry = Pair{value, string_view(string, size)};
  }

  template<typename Pair, typename E>
  constexpr void assign_entry(Pair& entry, E value, const char* string, std::size_t size)
  {
    assign_entry_impl(entry, value, string, size, has_first_second<Pair>{});
  }

  template<typename E, typename Pair, bool NullTerminated, typename Reflected = int>
  constexpr auto get_entries()
  {
#if defined(__NVCOMPILER)
    // nvc++ had issues with that and did not allow it. it just did not work after testing in godbolt and I don't know why
    const auto reflected = details::reflection_data<E, NullTerminated>;
    const auto strings   = details::reflection_data_string_storage<E, NullTerminated>.data();
#else
    constexpr auto reflected = details::reflection_data<std::remove_cv_t<E>, NullTerminated>;
    constexpr auto strings   = details::reflection_data_string_storage<std::remove_cv_t<E>, NullTerminated>.data();
#endif
    constexpr auto size = reflected.values.size();
    static_assert(size != 0,
                  "enchantum failed to reflect this enum.\n"
                  "Please read https://github.com/ZXShady/enchantum/blob/main/docs/limitations.md before opening an "
                  "issue\n"
                  "with your enum type with all its namespace/classes it is defined inside to help the creator debug "
                  "the "
                  "issues.");

    const auto& indices = reflected.string_indices;
#if defined(__RESHARPER__)
    auto ret = details::rscpp_make_defaulted_array_of<size>(Pair{reflected.values[0],
                                                                 string_view(strings + indices[0],
                                                                             indices[1] - indices[0] - NullTerminated)},
                                                            std::make_index_sequence<size>{});
#else
    details::array<Pair, size> ret{};
#endif
    auto* const ret_data = ret.data();
    for (std::size_t i = 0; i < size; ++i) {
      assign_entry(ret_data[i], reflected.values[i], strings + indices[i], indices[i + 1] - indices[i] - NullTerminated);
    }
    return ret;
  }
} // namespace details

#ifdef __cpp_concepts
template<Enum E, typename Pair = std::pair<E, enchantum::string_view>, bool NullTerminated = true>
#else
template<typename E,
         typename Pair                            = std::pair<E, enchantum::string_view>,
         bool NullTerminated                      = true,
          std::enable_if_t<std::is_enum<E>::value, int> = 0>
#endif
ENCHANTUM_DETAILS_INLINE_VAR constexpr auto entries = enchantum::details::get_entries<E, Pair, NullTerminated>();

namespace details {
  template<typename E>
  constexpr auto get_values() noexcept
  {
    constexpr auto              enums = entries<E>;
    details::array<E, enums.size()> ret{};
    const auto* const           enums_data = enums.data();
    for (std::size_t i = 0; i < ret.size(); ++i)
      ret[i] = enums_data[i].first;
    return ret;
  }

  template<typename E, typename String, bool NullTerminated>
  constexpr auto get_names() noexcept
  {
    constexpr auto                   enums = entries<E, std::pair<E, String>, NullTerminated>;
    details::array<String, enums.size()> ret{};
    const auto* const                enums_data = enums.data();
    for (std::size_t i = 0; i < ret.size(); ++i)
      ret[i] = enums_data[i].second;
    return ret;
  }

} // namespace details

template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
ENCHANTUM_DETAILS_INLINE_VAR constexpr auto values = details::get_values<E>();

#ifdef __cpp_concepts
template<Enum E, typename String = string_view, bool NullTerminated = true>
#else
template<typename E, typename String = string_view, bool NullTerminated = true, std::enable_if_t<std::is_enum<E>::value, int> = 0>
#endif
ENCHANTUM_DETAILS_INLINE_VAR constexpr auto names = details::get_names<E, String, NullTerminated>();


#define ENCHANTUM_DECLARE_EMPTY(ENUM)                                                                                         \
  template<>                                                                                                                  \
  ENCHANTUM_DETAILS_INLINE_VAR constexpr auto enchantum::entries<ENUM> = ::enchantum::details::array<std::pair<ENUM, ::enchantum::string_view>, 0> \
  {                                                                                                                           \
  }

template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
ENCHANTUM_DETAILS_INLINE_VAR constexpr auto min = entries<E>.front().first;

template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
ENCHANTUM_DETAILS_INLINE_VAR constexpr auto max = entries<E>.back().first;

template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
ENCHANTUM_DETAILS_INLINE_VAR constexpr std::size_t count = entries<E>.size();

namespace details {
  template<typename E, bool IsBitflag>
  struct has_zero_flag_impl {
    static constexpr bool value() noexcept { return false; }
  };

  template<typename E>
  struct has_zero_flag_impl<E, true> {
    static constexpr bool value() noexcept
    {
      constexpr auto& vals = values<E>;
      for (std::size_t i = 0; i < vals.size(); ++i)
        if (static_cast<typename std::underlying_type<E>::type>(vals[i]) == 0)
          return true;
      return false;
    }
  };

  template<typename E, bool IsBitflag>
  struct is_contiguous_bitflag_impl {
    static constexpr bool value() noexcept { return false; }
  };

  template<typename E>
  struct is_contiguous_bitflag_impl<E, true> {
    static constexpr bool value() noexcept
    {
      constexpr auto& enums = entries<E>;
      using T               = typename std::underlying_type<E>::type;
      std::size_t i = static_cast<std::size_t>(has_zero_flag_impl<E, true>::value());
      for (; i < enums.size() - 1; ++i)
        if (T(enums[i].first) << 1 != T(enums[i + 1].first))
          return false;
      return true;
    }
  };

  template<typename E, bool Empty>
  struct is_contiguous_impl {
    static constexpr bool value() noexcept
    {
      return static_cast<std::size_t>(enchantum::to_underlying(max<E>) - enchantum::to_underlying(min<E>)) + 1 == count<E>;
    }
  };

  template<typename E>
  struct is_contiguous_impl<E, true> {
    static constexpr bool value() noexcept { return false; }
  };
} // namespace details

#if ENCHANTUM_DETAILS_CXX_STD >= 201703L
template<typename E>
ENCHANTUM_DETAILS_INLINE_VAR constexpr bool has_zero_flag = [](const auto is_bitflag) {
  if constexpr (is_bitflag.value) {
    for (const auto v : values<E>)
      if (static_cast<std::underlying_type_t<E>>(v) == 0)
        return true;
  }
  return false;
}(std::integral_constant<bool, is_bitflag<E>>{});

template<typename E>
ENCHANTUM_DETAILS_INLINE_VAR constexpr bool is_contiguous = []() {
  if constexpr (count<E> == 0)
    return false;
  else
    return static_cast<std::size_t>(enchantum::to_underlying(max<E>) - enchantum::to_underlying(min<E>)) + 1 == count<E>;
}();

template<typename E>
ENCHANTUM_DETAILS_INLINE_VAR constexpr bool is_contiguous_bitflag = [](const auto is_bitflag) {
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
}(std::integral_constant<bool, is_bitflag<E>>{});
#else
template<typename E>
ENCHANTUM_DETAILS_INLINE_VAR constexpr bool has_zero_flag = details::has_zero_flag_impl<E, is_bitflag<E>>::value();

template<typename E>
ENCHANTUM_DETAILS_INLINE_VAR constexpr bool is_contiguous = details::is_contiguous_impl<E, count<E> == 0>::value();


template<typename E>
ENCHANTUM_DETAILS_INLINE_VAR constexpr bool is_contiguous_bitflag = details::is_contiguous_bitflag_impl<E, is_bitflag<E>>::value();
#endif

#ifdef __cpp_concepts
template<typename E>
concept ContiguousEnum = Enum<E> && is_contiguous<E>;
template<typename E>
concept ContiguousBitFlagEnum = BitFlagEnum<E> && is_contiguous_bitflag<E>;
#endif

} // namespace enchantum

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
#ifdef __cpp_impl_three_way_comparison
  #include <compare>
#endif
#include <cstddef>
#include <cstdint>
#include <utility>

#if defined(ENCHANTUM_DETAILS_GCC_MAJOR) && ENCHANTUM_DETAILS_GCC_MAJOR <= 10
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

    ENCHANTUM_DETAILS_NODISCARD constexpr CRTP operator++(int) & noexcept
    {
      auto copy = static_cast<CRTP&>(*this);
      ++*this;
      return copy;
    }
    ENCHANTUM_DETAILS_NODISCARD constexpr CRTP operator--(int) & noexcept
    {
      auto copy = static_cast<CRTP&>(*this);
      --*this;
      return copy;
    }

    ENCHANTUM_DETAILS_NODISCARD constexpr friend CRTP operator+(CRTP it, const std::ptrdiff_t offset) noexcept
    {
      it += offset;
      return it;
    }

    ENCHANTUM_DETAILS_NODISCARD constexpr friend CRTP operator+(const std::ptrdiff_t offset, CRTP it) noexcept
    {
      it += offset;
      return it;
    }

    ENCHANTUM_DETAILS_NODISCARD constexpr friend CRTP operator-(CRTP it, const std::ptrdiff_t offset) noexcept
    {
      it -= offset;
      return it;
    }

    ENCHANTUM_DETAILS_NODISCARD constexpr std::ptrdiff_t operator-(const sized_iterator that) const noexcept
    {
      return index - that.index;
    }

    ENCHANTUM_DETAILS_NODISCARD constexpr std::ptrdiff_t        operator-(senitiel) const noexcept { return index - Size; }
    ENCHANTUM_DETAILS_NODISCARD friend constexpr std::ptrdiff_t operator-(senitiel, sized_iterator it) noexcept
    {
      return Size - it.index;
    }

    ENCHANTUM_DETAILS_NODISCARD constexpr bool operator==(const sized_iterator that) const noexcept { return that.index == index; };
    ENCHANTUM_DETAILS_NODISCARD constexpr bool operator==(senitiel) const noexcept { return Size == index; }

#ifdef __cpp_impl_three_way_comparison
    ENCHANTUM_DETAILS_NODISCARD constexpr auto operator<=>(const sized_iterator that) const noexcept { return index <=> that.index; };
    ENCHANTUM_DETAILS_NODISCARD constexpr auto operator<=>(senitiel) const noexcept { return index <=> Size; }
#else

    ENCHANTUM_DETAILS_NODISCARD constexpr bool operator!=(const sized_iterator that) const noexcept { return that.index != index; };
    ENCHANTUM_DETAILS_NODISCARD constexpr bool operator!=(senitiel) const noexcept { return Size != index; }

    ENCHANTUM_DETAILS_NODISCARD friend constexpr bool operator==(senitiel, const sized_iterator it) noexcept
    {
      return Size == it.index;
    }


    ENCHANTUM_DETAILS_NODISCARD friend constexpr bool operator!=(senitiel, const sized_iterator it) noexcept
    {
      return Size != it.index;
    }


    ENCHANTUM_DETAILS_NODISCARD constexpr bool operator<(const sized_iterator that) const noexcept { return index < that.index; };
    ENCHANTUM_DETAILS_NODISCARD constexpr bool operator>(const sized_iterator that) const noexcept { return index > that.index; };
    ENCHANTUM_DETAILS_NODISCARD constexpr bool operator<=(const sized_iterator that) const noexcept { return index <= that.index; };
    ENCHANTUM_DETAILS_NODISCARD constexpr bool operator>=(const sized_iterator that) const noexcept { return index >= that.index; };

    ENCHANTUM_DETAILS_NODISCARD constexpr bool operator<(senitiel) const noexcept { return index < Size; };
    ENCHANTUM_DETAILS_NODISCARD constexpr bool operator>(senitiel) const noexcept { return index > Size; };
    ENCHANTUM_DETAILS_NODISCARD constexpr bool operator<=(senitiel) const noexcept { return index <= Size; };
    ENCHANTUM_DETAILS_NODISCARD constexpr bool operator>=(senitiel) const noexcept { return index >= Size; };

    ENCHANTUM_DETAILS_NODISCARD friend constexpr bool operator<(senitiel, const sized_iterator it) noexcept
    {
      return Size < it.index;
    };
    ENCHANTUM_DETAILS_NODISCARD friend constexpr bool operator>(senitiel, const sized_iterator it) noexcept
    {
      return Size > it.index;
    };
    ENCHANTUM_DETAILS_NODISCARD friend constexpr bool operator<=(senitiel, const sized_iterator it) noexcept
    {
      return Size <= it.index;
    };
    ENCHANTUM_DETAILS_NODISCARD friend constexpr bool operator>=(senitiel, const sized_iterator it) noexcept
    {
      return Size >= it.index;
    };

#endif
  };

  template<typename E, typename String = string_view, bool NullTerminated = true>
  struct names_generator_t {
    ENCHANTUM_DETAILS_NODISCARD static constexpr std::size_t size() noexcept { return count<E>; }

    struct iterator : sized_iterator<iterator, static_cast<std::ptrdiff_t>(size())> {
      using base       = sized_iterator<iterator, static_cast<std::ptrdiff_t>(size())>;
      using value_type = String;
      using base::operator+=;
      ENCHANTUM_DETAILS_NODISCARD constexpr String operator*() const noexcept
      {
        const auto* const p       = details::reflection_string_indices<E, NullTerminated>.data();
        const auto* const strings = details::reflection_data_string_storage<E, NullTerminated>.data();
        return String(strings + p[this->index], p[this->index + 1] - p[this->index] - NullTerminated);
      }

      ENCHANTUM_DETAILS_NODISCARD constexpr String operator[](const std::ptrdiff_t i) const noexcept
      {
        auto it = *this;
        it += i;
        return *it;
      }
    };

    ENCHANTUM_DETAILS_NODISCARD static constexpr auto begin() { return iterator{}; }
    ENCHANTUM_DETAILS_NODISCARD static constexpr auto end() { return senitiel{}; }

    ENCHANTUM_DETAILS_NODISCARD constexpr auto operator[](const std::size_t i) const noexcept
    {
      auto it = begin();
      it += static_cast<std::ptrdiff_t>(i);
      return *it;
    }
  };

  template<typename E>
  struct values_generator_t {
    ENCHANTUM_DETAILS_NODISCARD static constexpr std::size_t size() noexcept { return count<E>; }

    struct iterator : sized_iterator<iterator, static_cast<std::ptrdiff_t>(size())> {
      using base       = sized_iterator<iterator, static_cast<std::ptrdiff_t>(size())>;
      using value_type = E;
      using base::operator+=;
#if ENCHANTUM_DETAILS_CXX_STD < 201703L
      ENCHANTUM_DETAILS_NODISCARD constexpr E dereference(std::true_type, std::false_type) const noexcept
      {
        using T = typename std::underlying_type<E>::type;
        return static_cast<E>(static_cast<T>(min<E>) + static_cast<T>(this->index));
      }

      ENCHANTUM_DETAILS_NODISCARD constexpr E dereference(std::false_type, std::true_type) const noexcept
      {
        using T                        = typename std::underlying_type<E>::type;
        using UT                       = typename std::make_unsigned<T>::type;
        constexpr auto real_min_offset = details::countr_zero(static_cast<UT>(values<E>[has_zero_flag<E>]));
        if (has_zero_flag<E> ? this->index == 0 : false)
          return E{};
        return static_cast<E>(UT{1} << (real_min_offset + static_cast<UT>(this->index - has_zero_flag<E>)));
      }

      ENCHANTUM_DETAILS_NODISCARD constexpr E dereference(std::false_type, std::false_type) const noexcept
      {
        return values<E>[static_cast<std::size_t>(this->index)];
      }
#endif

      ENCHANTUM_DETAILS_NODISCARD constexpr E operator*() const noexcept
      {
#if ENCHANTUM_DETAILS_CXX_STD >= 201703L
        if constexpr (is_contiguous<E>) {
          using T = typename std::underlying_type<E>::type;
          return static_cast<E>(static_cast<T>(min<E>) + static_cast<T>(this->index));
        }
        else if constexpr (is_contiguous_bitflag<E>) {
          using T                        = typename std::underlying_type<E>::type;
          using UT                       = typename std::make_unsigned<T>::type;
          constexpr auto real_min_offset = details::countr_zero(static_cast<UT>(values<E>[has_zero_flag<E>]));
          if (has_zero_flag<E> ? this->index == 0 : false)
            return E{};
          return static_cast<E>(UT{1} << (real_min_offset + static_cast<UT>(this->index - has_zero_flag<E>)));
        }
        else {
          return values<E>[static_cast<std::size_t>(this->index)];
        }
#else
        return dereference(std::integral_constant<bool, is_contiguous<E>>{},
                           std::integral_constant<bool, is_contiguous_bitflag<E>>{});
#endif
      }
      ENCHANTUM_DETAILS_NODISCARD constexpr E operator[](const std::ptrdiff_t i) const noexcept
      {
        auto it = *this;
        it += i;
        return *it;
      }
    };

    ENCHANTUM_DETAILS_NODISCARD static constexpr auto begin() { return iterator{}; }
    ENCHANTUM_DETAILS_NODISCARD static constexpr auto end() { return senitiel{}; }

    ENCHANTUM_DETAILS_NODISCARD constexpr auto operator[](const std::size_t i) const noexcept
    {
      auto it = begin();
      it += static_cast<std::ptrdiff_t>(i);
      return *it;
    }
  };

  template<typename E, typename Pair = std::pair<E, string_view>, bool NullTerminated = true>
  struct entries_generator_t {
    ENCHANTUM_DETAILS_NODISCARD static constexpr std::size_t size() noexcept { return count<E>; }

    struct iterator : sized_iterator<iterator, static_cast<std::ptrdiff_t>(size())> {
      using base       = sized_iterator<iterator, static_cast<std::ptrdiff_t>(size())>;
      using value_type = Pair;
      using base::operator+=;
      ENCHANTUM_DETAILS_NODISCARD constexpr Pair operator*() const noexcept
      {
        return Pair{
          values_generator_t<E>{}[static_cast<std::size_t>(this->index)],
          names_generator_t<E, string_view, NullTerminated>{}[static_cast<std::size_t>(this->index)],
        };
      }
      ENCHANTUM_DETAILS_NODISCARD constexpr Pair operator[](const std::ptrdiff_t i) const noexcept
      {
        auto it = *this;
        it += i;
        return *it;
      }
    };

    ENCHANTUM_DETAILS_NODISCARD static constexpr auto begin() { return iterator{}; }
    ENCHANTUM_DETAILS_NODISCARD static constexpr auto end() { return senitiel{}; }

    ENCHANTUM_DETAILS_NODISCARD constexpr auto operator[](const std::size_t i) const noexcept
    {
      auto it = begin();
      it += static_cast<std::ptrdiff_t>(i);
      return *it;
    }
  };

} // namespace details

template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
ENCHANTUM_DETAILS_INLINE_VAR constexpr details::values_generator_t<E> values_generator{};

#ifdef __cpp_concepts
template<Enum E, typename StringView = string_view, bool NullTerminated = true>
ENCHANTUM_DETAILS_INLINE_VAR constexpr details::names_generator_t<E, StringView, NullTerminated> names_generator{};

template<Enum E, typename Pair = std::pair<E, string_view>, bool NullTerminated = true>
ENCHANTUM_DETAILS_INLINE_VAR constexpr details::entries_generator_t<E, Pair, NullTerminated> entries_generator{};

#else
template<typename E, typename StringView = string_view, bool NullTerminated = true, std::enable_if_t<std::is_enum<E>::value, int> = 0>
ENCHANTUM_DETAILS_INLINE_VAR constexpr details::names_generator_t<E, StringView, NullTerminated> names_generator{};

template<typename E, typename Pair = std::pair<E, string_view>, bool NullTerminated = true, std::enable_if_t<std::is_enum<E>::value, int> = 0>
ENCHANTUM_DETAILS_INLINE_VAR constexpr details::entries_generator_t<E, Pair, NullTerminated> entries_generator{};

#endif

} // namespace enchantum

#if defined(ENCHANTUM_DETAILS_GCC_MAJOR) && ENCHANTUM_DETAILS_GCC_MAJOR <= 10
  #pragma GCC diagnostic pop
#endif

// IWYU pragma: begin_exports
// IWYU pragma: end_exports

#include <type_traits>
#include <utility>

#if defined(ENCHANTUM_DETAILS_GCC_MAJOR) && ENCHANTUM_DETAILS_GCC_MAJOR <= 10
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wconversion"
  #pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif


namespace enchantum {

namespace details {
  template<typename BinaryPredicate>
  constexpr bool call_predicate_impl(const BinaryPredicate binary_pred, const string_view a, const string_view b, std::true_type)
  {
    const auto a_size = a.size();
    if (a_size != b.size())
      return false;
    const auto a_data = a.data();
    const auto b_data = b.data();

    for (std::size_t i = 0; i < a_size; ++i)
      if (!binary_pred(a_data[i], b_data[i]))
        return false;
    return true;
  }

  template<typename BinaryPredicate>
  constexpr bool call_predicate_impl(const BinaryPredicate binary_pred, const string_view a, const string_view b, std::false_type)
  {
    static_assert(enchantum::details::is_invocable<const BinaryPredicate&, const string_view&, const string_view&>::value,
                  "BinaryPredicate must be callable with either two chars or two string_views");
    return binary_pred(a, b);
  }

  template<typename BinaryPredicate>
  constexpr bool call_predicate(const BinaryPredicate binary_pred, const string_view a, const string_view b)
  {
    return call_predicate_impl(binary_pred,
                               a,
                               b,
                               enchantum::details::bool_constant<
                                 enchantum::details::is_invocable<const BinaryPredicate&, const char&, const char&>::value>{});
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


namespace details {
  template<typename E, bool Empty>
  struct contains_bounds {
    static constexpr bool outside(typename std::underlying_type<E>::type value) noexcept
    {
      using T = typename std::underlying_type<E>::type;
      return value < T(min<E>) || value > T(max<E>);
    }
  };

  template<typename E>
  struct contains_bounds<E, true> {
    static constexpr bool outside(typename std::underlying_type<E>::type) noexcept { return false; }
  };

  template<typename E>
  constexpr bool contains_impl(typename std::underlying_type<E>::type value, std::true_type, std::false_type) noexcept
  {
    using T = typename std::underlying_type<E>::type;
    if (has_zero_flag<E> ? value == 0 : false)
      return true;
    const auto u = static_cast<typename std::make_unsigned<T>::type>(value);
    return u != 0 && (u & (u - 1)) == 0;
  }

  template<typename E>
  constexpr bool contains_impl(typename std::underlying_type<E>::type, std::false_type, std::true_type) noexcept
  {
    return true;
  }

  template<typename E>
  constexpr bool contains_impl(typename std::underlying_type<E>::type value, std::false_type, std::false_type) noexcept
  {
    using T = typename std::underlying_type<E>::type;
    for (std::size_t i = 0; i < count<E>; ++i)
      if (static_cast<T>(values_generator<E>[i]) == value)
        return true;
    return false;
  }

  template<typename E>
  constexpr bool contains_value(typename std::underlying_type<E>::type value) noexcept
  {
    if (details::contains_bounds<E, count<E> == 0>::outside(value))
      return false;

    return details::contains_impl<E>(value,
                                     std::integral_constant<bool, is_contiguous_bitflag<E>>{},
                                     std::integral_constant<bool, is_contiguous<E>>{});
  }

  template<typename E>
  constexpr optional<std::size_t> enum_to_index_impl(const E e, std::true_type, std::false_type) noexcept
  {
    using T = typename std::underlying_type<E>::type;
    if (details::contains_value<E>(static_cast<T>(e)))
      return optional<std::size_t>(std::size_t(T(e) - T(min<E>)));
    return optional<std::size_t>();
  }

  template<typename E>
  constexpr optional<std::size_t> enum_to_index_impl(const E e, std::false_type, std::true_type) noexcept
  {
    using T = typename std::underlying_type<E>::type;
    if (!details::contains_value<E>(static_cast<T>(e)))
      return optional<std::size_t>();

    const bool has_zero = has_zero_flag<E>;
    if (has_zero ? static_cast<T>(e) == 0 : false)
      return optional<std::size_t>(0); // assumes 0 is the index of value `0`

    using U = typename std::make_unsigned<T>::type;
    const auto value_offset = static_cast<std::size_t>(details::countr_zero(static_cast<U>(e)));
    const auto base_offset =
      static_cast<std::size_t>(details::countr_zero(static_cast<U>(values_generator<E>[static_cast<std::size_t>(has_zero)])));
    return optional<std::size_t>(std::size_t(has_zero) + value_offset - base_offset);
  }

  template<typename E>
  constexpr optional<std::size_t> enum_to_index_impl(const E e, std::false_type, std::false_type) noexcept
  {
    for (std::size_t i = 0; i < count<E>; ++i) {
      if (values_generator<E>[i] == e)
        return optional<std::size_t>(i);
    }
    return optional<std::size_t>();
  }
} // namespace details


template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
ENCHANTUM_DETAILS_NODISCARD constexpr bool contains(const std::underlying_type_t<E> value) noexcept
{
  return details::contains_value<E>(value);
}

template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
ENCHANTUM_DETAILS_NODISCARD constexpr bool contains(const E value) noexcept
{
  return enchantum::contains<E>(static_cast<std::underlying_type_t<E>>(value));
}

template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
ENCHANTUM_DETAILS_NODISCARD constexpr bool contains(const string_view name) noexcept
{
  constexpr auto minmax = details::minmax_string_size(names<E>.data(), names<E>.data() + names<E>.size());
  const auto size = name.size();
  if (size < minmax.first || size > minmax.second)
    return false;

  for (std::size_t i = 0; i < count<E>; ++i) {
    const auto s = names_generator<E>[i];
    if (s == name)
      return true;
  }
  return false;
}


template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E), typename BinaryPred>
ENCHANTUM_DETAILS_NODISCARD constexpr bool contains(const string_view name, const BinaryPred binary_pred) noexcept
{
  for (std::size_t i = 0; i < count<E>; ++i) {
    const auto s = names_generator<E>[i];
    if (details::call_predicate(binary_pred, name, s))
      return true;
  }
  return false;
}


namespace details {
  template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
  struct index_to_enum_functor {
    ENCHANTUM_DETAILS_NODISCARD constexpr optional<E> operator()(const std::size_t index) const noexcept
    {
      if (index < count<E>)
        return optional<E>(values_generator<E>[index]);
      return optional<E>();
    }
  };

  struct enum_to_index_functor {
    template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
    ENCHANTUM_DETAILS_NODISCARD constexpr optional<std::size_t> operator()(const E e) const noexcept
    {
      return details::enum_to_index_impl<E>(e,
                                            std::integral_constant<bool, is_contiguous<E> && count<E> != 0>{},
                                            std::integral_constant<bool, is_contiguous_bitflag<E>>{});
    }
  };


  template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
  struct cast_functor {
    ENCHANTUM_DETAILS_NODISCARD constexpr optional<E> operator()(const std::underlying_type_t<E> value) const noexcept
    {
      if (!enchantum::contains<E>(value))
        return optional<E>();
      return optional<E>(static_cast<E>(value));
    }

    ENCHANTUM_DETAILS_NODISCARD constexpr optional<E> operator()(const string_view name) const noexcept
    {
      constexpr auto minmax = details::minmax_string_size(names<E>.data(), names<E>.data() + names<E>.size());
      const auto size = name.size();
      if (size < minmax.first || size > minmax.second)
        return optional<E>(); // nullopt

      for (std::size_t i = 0; i < count<E>; ++i) {
        if (names_generator<E>[i] == name) {
          return optional<E>(values_generator<E>[i]);
        }
      }
      return optional<E>(); // nullopt
    }

    template<typename BinaryPred>
    ENCHANTUM_DETAILS_NODISCARD constexpr optional<E> operator()(const string_view name, const BinaryPred binary_pred) const noexcept
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
ENCHANTUM_DETAILS_INLINE_VAR constexpr details::index_to_enum_functor<E> index_to_enum{};

ENCHANTUM_DETAILS_INLINE_VAR constexpr details::enum_to_index_functor enum_to_index{};

template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
ENCHANTUM_DETAILS_INLINE_VAR constexpr details::cast_functor<E> cast{};


namespace details {
  struct to_string_functor {
    template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
    ENCHANTUM_DETAILS_NODISCARD constexpr string_view operator()(const E value) const noexcept
    {
      if (const auto i = enchantum::enum_to_index(value))
        return names_generator<E>[*i];
      return string_view();
    }
  };

} // namespace details
ENCHANTUM_DETAILS_INLINE_VAR constexpr details::to_string_functor to_string{};


} // namespace enchantum


#if defined(ENCHANTUM_DETAILS_GCC_MAJOR) && ENCHANTUM_DETAILS_GCC_MAJOR <= 10
  #pragma GCC diagnostic pop
#endif

#include <cstddef>

#if defined(ENCHANTUM_DETAILS_GCC_MAJOR) && ENCHANTUM_DETAILS_GCC_MAJOR <= 10
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wconversion"
  #pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif


namespace enchantum {

namespace details {
  struct equal_to_string_view {
    constexpr bool operator()(const string_view a, const string_view b) const noexcept { return a == b; }
  };

  template<typename E>
  constexpr E value_ors_impl()
  {
    static_assert(is_bitflag<E>, "");
    using T = std::underlying_type_t<E>;
    T ret{};
    for (std::size_t i = 0; i < count<E>; ++i)
      ret |= static_cast<T>(values_generator<E>[i]);
    return static_cast<E>(ret);
  }
} // namespace details

template<typename E>
ENCHANTUM_DETAILS_INLINE_VAR constexpr E value_ors = details::value_ors_impl<E>();


template<ENCHANTUM_DETAILS_ENUM_BITFLAG_CONCEPT(E)>
ENCHANTUM_DETAILS_NODISCARD constexpr bool contains_bitflag(const std::underlying_type_t<E> value) noexcept
{
  if (!has_zero_flag<E>)
    if (value == 0)
      return false;

  return value == (static_cast<std::underlying_type_t<E>>(value_ors<E>) & value);
}

template<ENCHANTUM_DETAILS_ENUM_BITFLAG_CONCEPT(E)>
ENCHANTUM_DETAILS_NODISCARD constexpr bool contains_bitflag(const E value) noexcept
{
  return enchantum::contains_bitflag<E>(static_cast<std::underlying_type_t<E>>(value));
}

template<ENCHANTUM_DETAILS_ENUM_BITFLAG_CONCEPT(E), typename BinaryPred>
ENCHANTUM_DETAILS_NODISCARD constexpr bool contains_bitflag(const string_view s, const char sep, const BinaryPred binary_pred) noexcept
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
ENCHANTUM_DETAILS_NODISCARD constexpr bool contains_bitflag(const string_view s, const char sep = '|') noexcept
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
ENCHANTUM_DETAILS_NODISCARD constexpr String to_string_bitflag(const E value, const char sep = '|')
{
  using T = std::underlying_type_t<E>;
  if (has_zero_flag<E>)
    if (static_cast<T>(value) == 0)
      return String(names_generator<E>[0].data(), names_generator<E>[0].size());

  String name;
  T      check_value = 0;
  for (auto i = static_cast<std::size_t>(has_zero_flag<E>); i < count<E>; ++i) {
    const auto v = static_cast<T>(values_generator<E>[i]);
    if (v == (static_cast<T>(value) & v)) {
      const auto s = names_generator<E>[i];
      if (!name.empty())
        name.append(1, sep);           // append separator if not the first value
      name.append(s.data(), s.size());
      check_value |= v;
    }
  }
  if (check_value == static_cast<T>(value))
    return name;
  return String();
}

template<ENCHANTUM_DETAILS_ENUM_BITFLAG_CONCEPT(E), typename BinaryPred>
ENCHANTUM_DETAILS_NODISCARD constexpr optional<E> cast_bitflag(const string_view s, const char sep, const BinaryPred binary_pred) noexcept
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
ENCHANTUM_DETAILS_NODISCARD constexpr optional<E> cast_bitflag(const string_view s, const char sep = '|') noexcept
{
  return enchantum::cast_bitflag<E>(s, sep, details::equal_to_string_view{});
}

template<ENCHANTUM_DETAILS_ENUM_BITFLAG_CONCEPT(E)>
ENCHANTUM_DETAILS_NODISCARD constexpr optional<E> cast_bitflag(const std::underlying_type_t<E> value) noexcept
{
  return enchantum::contains_bitflag<E>(value) ? optional<E>(static_cast<E>(value)) : optional<E>();
}

} // namespace enchantum

#if defined(ENCHANTUM_DETAILS_GCC_MAJOR) && ENCHANTUM_DETAILS_GCC_MAJOR <= 10
  #pragma GCC diagnostic pop
#endif

#include <string>

namespace enchantum {
namespace details {
  template<typename String>
  std::string string_to_std_string(const String& name, std::true_type)
  {
    return name;
  }

  template<typename String>
  std::string string_to_std_string(const String& name, std::false_type)
  {
    return std::string(name.data(), name.size());
  }

  template<typename E>
  std::string format_impl(E e, std::true_type) noexcept
  {
    const auto name = enchantum::to_string_bitflag(e);
    if (!name.empty())
      return details::string_to_std_string(name, std::integral_constant<bool, std::is_same<std::string, string>::value>{});
    return std::to_string(+enchantum::to_underlying(e)); // promote using + to select int overload if to underlying returns char
  }

  template<typename E>
  std::string format_impl(E e, std::false_type) noexcept
  {
    const auto name = enchantum::to_string(e);
    if (!name.empty())
      return std::string(name.data(), name.size());
    return std::to_string(+enchantum::to_underlying(e)); // promote using + to select int overload if to underlying returns char
  }

  template<typename E>
  std::string format(E e) noexcept
  {
    return format_impl(e, std::integral_constant<bool, is_bitflag<E>>{});
  }
} // namespace details
} // namespace enchantum

#include <utility>
#include <cstddef>

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
noexcept(noexcept(std::declval<Func>()(std::declval<E>())))
{
  using Ret = decltype(func(e));
  

  return [&]<std::size_t... Idx>(std::index_sequence<Idx...>) {
    if ((values<Enums>[Idx] == enums))
      (func(std::integral_constant<E, values<E>[Idx]> {}), ...);
  }(std::make_index_sequence<count<E>>());
}
template<Enum... Enums, std::invocable<Enums...> Func>
constexpr auto visit(Func func, Enums... enums) noexcept(noexcept(std::declval<Func>()(std::declval<Enums>()...)))
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
  constexpr void for_each(Func& f, std::index_sequence<I...>)
  {
#if ENCHANTUM_DETAILS_CXX_STD >= 201703L
    ((void)f(std::integral_constant<E, values<E>[I]>{}), ...);
#else
    using expander = int[];
    (void)expander{0, ((void)f(std::integral_constant<E, values<E>[I]> {}), 0)...};
#endif
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
#include <type_traits>

namespace enchantum {

template<typename E, typename V, typename Container = std::array<V, count<E>>>
class array : public Container {
  static_assert(std::is_enum<E>::value, "enchantum::array requires an enum type");
public:
  using container_type = Container;
  using index_type     = E;
  using typename Container::const_reference;
  using typename Container::reference;

  using Container::at;
  using Container::operator[];

  ENCHANTUM_DETAILS_NODISCARD constexpr reference at(const E index)
  {
    if (const auto i = enchantum::enum_to_index(index))
      return operator[](*i);
    ENCHANTUM_THROW(std::out_of_range("enchantum::array::at index out of range"), index);
  }

  ENCHANTUM_DETAILS_NODISCARD constexpr const_reference at(const E index) const
  {
    if (const auto i = enchantum::enum_to_index(index))
      return operator[](*i);
    ENCHANTUM_THROW(std::out_of_range("enchantum::array::at: index out of range"), index);
  }

  ENCHANTUM_DETAILS_NODISCARD constexpr reference operator[](const E index) noexcept
  {
    return operator[](*enchantum::enum_to_index(index));
  }

  ENCHANTUM_DETAILS_NODISCARD constexpr const_reference operator[](const E index) const noexcept
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
  static_assert(std::is_enum<E>::value, "enchantum::bitset requires an enum type");
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

  constexpr bitset() = default;

  ENCHANTUM_DETAILS_NODISCARD string to_string(const char sep = '|') const
  {
    string name;
    for (std::size_t i = 0; i < enchantum::count<E>; ++i) {
      if (test(i)) {
        const auto s = enchantum::names_generator<E>[i];
        if (!name.empty())
          name += sep;
        name.append(s.data(), s.size());
      }
    }
    return name;
  }

  ENCHANTUM_DETAILS_NODISCARD constexpr auto to_string(const char zero, const char one) const
  {
    return Container::to_string(zero, one);
  }

  constexpr bitset(const std::initializer_list<E> values) noexcept
  {
    for (auto value : values) {
      set(value, true);
    }
  }

  ENCHANTUM_DETAILS_NODISCARD constexpr reference operator[](const E index) noexcept
  {
    return operator[](*enchantum::enum_to_index(index));
  }

  ENCHANTUM_DETAILS_NODISCARD constexpr bool operator[](const E index) const noexcept
  {
    return operator[](*enchantum::enum_to_index(index));
  }

  constexpr bool test(const E pos) const
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
  ENCHANTUM_DETAILS_NODISCARD constexpr E operator~(E e) noexcept
  {
    return static_cast<E>(~static_cast<std::underlying_type_t<E>>(e));
  }

  template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
  ENCHANTUM_DETAILS_NODISCARD constexpr E operator|(E a, E b) noexcept
  {
    using T = std::underlying_type_t<E>;
    return static_cast<E>(static_cast<T>(a) | static_cast<T>(b));
  }

  template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
  ENCHANTUM_DETAILS_NODISCARD constexpr E operator&(E a, E b) noexcept
  {
    using T = std::underlying_type_t<E>;
    return static_cast<E>(static_cast<T>(a) & static_cast<T>(b));
  }

  template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
  ENCHANTUM_DETAILS_NODISCARD constexpr E operator^(E a, E b) noexcept
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
  ENCHANTUM_DETAILS_NODISCARD constexpr Enum operator&(Enum a, Enum b) noexcept                         \
  {                                                                                       \
    using T = std::underlying_type_t<Enum>;                                               \
    return static_cast<Enum>(static_cast<T>(a) & static_cast<T>(b));                      \
  }                                                                                       \
  ENCHANTUM_DETAILS_NODISCARD constexpr Enum operator|(Enum a, Enum b) noexcept                         \
  {                                                                                       \
    using T = std::underlying_type_t<Enum>;                                               \
    return static_cast<Enum>(static_cast<T>(a) | static_cast<T>(b));                      \
  }                                                                                       \
  ENCHANTUM_DETAILS_NODISCARD constexpr Enum operator^(Enum a, Enum b) noexcept                         \
  {                                                                                       \
    using T = std::underlying_type_t<Enum>;                                               \
    return static_cast<Enum>(static_cast<T>(a) ^ static_cast<T>(b));                      \
  }                                                                                       \
  constexpr Enum&              operator&=(Enum& a, Enum b) noexcept { return a = a & b; } \
  constexpr Enum&              operator|=(Enum& a, Enum b) noexcept { return a = a | b; } \
  constexpr Enum&              operator^=(Enum& a, Enum b) noexcept { return a = a ^ b; } \
  ENCHANTUM_DETAILS_NODISCARD constexpr Enum operator~(Enum a) noexcept                                 \
  {                                                                                       \
    return static_cast<Enum>(~static_cast<std::underlying_type_t<Enum>>(a));              \
  }



#if defined(ENCHANTUM_DETAILS_GCC_MAJOR) && ENCHANTUM_DETAILS_GCC_MAJOR <= 10
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wconversion"
  #pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif

namespace enchantum {
namespace scoped {
  namespace details {


    constexpr string_view remove_scope_or_empty(string_view string, const string_view type_name) noexcept
    {
      if (string.substr(0, type_name.size()) != type_name)
        return string_view();
      string.remove_prefix(type_name.size());
      if (string.substr(0, 2) != string_view("::", 2))
        return string_view();
      string.remove_prefix(2);
      return string;
    }
  } // namespace details

  template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
  ENCHANTUM_DETAILS_NODISCARD constexpr bool contains(const string_view name) noexcept
  {
    const auto n = details::remove_scope_or_empty(name, type_name<E>);
    return !n.empty() && enchantum::contains<E>(n);
  }

  template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E), typename BinaryPredicate>
  ENCHANTUM_DETAILS_NODISCARD constexpr bool contains(const string_view name, const BinaryPredicate binary_predicate) noexcept
  {
    const auto n = details::remove_scope_or_empty(name, type_name<E>);
    return !n.empty() && enchantum::contains<E>(n, binary_predicate);
  }

  namespace details {
    template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
    struct scoped_cast_functor {
      ENCHANTUM_DETAILS_NODISCARD constexpr optional<E> operator()(const string_view name) const noexcept
      {
        const auto n = details::remove_scope_or_empty(name, type_name<E>);
        return n.empty() ? optional<E>() : enchantum::cast<E>(n);
      }

      template<typename BinaryPred>
      ENCHANTUM_DETAILS_NODISCARD constexpr optional<E> operator()(const string_view name, const BinaryPred binary_predicate) const noexcept
      {
        const auto n = details::remove_scope_or_empty(name, type_name<E>);
        return n.empty() ? optional<E>() : enchantum::cast<E>(n, binary_predicate);
      }
    };

    struct to_scoped_string_functor {
      // hacky workaround about string not being a literal type.
      template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E), typename String = string>
      ENCHANTUM_DETAILS_NODISCARD constexpr String operator()(const E value) const noexcept
      {
        String s;
        if (const auto i = enchantum::enum_to_index(value)) {
          const auto scope_name = type_name<E>;
          s.append(scope_name.data(), scope_name.size());
          s.append("::", 2);
          const auto name = names_generator<E>[*i];
          s.append(name.data(), name.size());
          return s;
        }
        return s;
      }
    };
  } // namespace details


  ENCHANTUM_DETAILS_INLINE_VAR constexpr details::to_scoped_string_functor to_string;

  template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
  ENCHANTUM_DETAILS_INLINE_VAR constexpr details::scoped_cast_functor<E> cast;

  template<ENCHANTUM_DETAILS_ENUM_BITFLAG_CONCEPT(E), typename BinaryPred>
  ENCHANTUM_DETAILS_NODISCARD constexpr bool contains_bitflag(const string_view s, const char sep, const BinaryPred binary_pred) noexcept
  {
    std::size_t pos = 0;
    for (std::size_t i = s.find(sep); i != s.npos; i = s.find(sep, pos)) {
      if (!enchantum::scoped::contains<E>(s.substr(pos, i - pos), binary_pred))
        return false;
      pos = i + 1;
    }
    return enchantum::scoped::contains<E>(s.substr(pos), binary_pred);
  }

  template<ENCHANTUM_DETAILS_ENUM_BITFLAG_CONCEPT(E)>
  ENCHANTUM_DETAILS_NODISCARD constexpr bool contains_bitflag(const string_view s, const char sep = '|') noexcept
  {
    std::size_t pos = 0;
    for (std::size_t i = s.find(sep); i != s.npos; i = s.find(sep, pos)) {
      if (!enchantum::scoped::contains<E>(s.substr(pos, i - pos)))
        return false;
      pos = i + 1;
    }
    return enchantum::scoped::contains<E>(s.substr(pos));
  }


  template<typename String = string, ENCHANTUM_DETAILS_ENUM_BITFLAG_CONCEPT(E)>
  ENCHANTUM_DETAILS_NODISCARD constexpr String to_string_bitflag(const E value, const char sep = '|')
  {
    using T = std::underlying_type_t<E>;
    if (has_zero_flag<E>)
      if (static_cast<T>(value) == 0)
        return enchantum::scoped::to_string(value);

    String         name;
    T              check_value = 0;
    constexpr auto scope_name  = type_name<E>;
    for (auto i = static_cast<std::size_t>(has_zero_flag<E>); i < count<E>; ++i) {
      const auto v = static_cast<T>(values_generator<E>[i]);
      if (v == (static_cast<T>(value) & v)) {
        if (!name.empty())
          name.append(1, sep); // append separator if not the first value
        name.append(scope_name.data(), scope_name.size());
        name.append("::", 2);
        const auto s = names_generator<E>[i];
        name.append(s.data(), s.size());
        check_value |= v;
      }
    }
    if (check_value == static_cast<T>(value))
      return name;
    return string();
  }


  template<ENCHANTUM_DETAILS_ENUM_BITFLAG_CONCEPT(E), typename BinaryPred>
  ENCHANTUM_DETAILS_NODISCARD constexpr optional<E> cast_bitflag(const string_view s, const char sep, const BinaryPred binary_pred) noexcept
  {
    using T = std::underlying_type_t<E>;
    T           check_value{};
    std::size_t pos = 0;
    for (std::size_t i = s.find(sep); i != s.npos; i = s.find(sep, pos)) {
      if (const auto v = enchantum::scoped::cast<E>(s.substr(pos, i - pos), binary_pred))
        check_value |= static_cast<T>(*v);
      else
        return optional<E>();
      pos = i + 1;
    }

    if (const auto v = enchantum::scoped::cast<E>(s.substr(pos), binary_pred))
      return optional<E>(static_cast<E>(check_value | static_cast<T>(*v)));
    return optional<E>();
  }

  template<ENCHANTUM_DETAILS_ENUM_BITFLAG_CONCEPT(E)>
ENCHANTUM_DETAILS_NODISCARD constexpr optional<E> cast_bitflag(const string_view s, const char sep = '|') noexcept
{
  return enchantum::scoped::cast_bitflag<E>(s, sep, enchantum::details::equal_to_string_view{});
}
} // namespace scoped
} // namespace enchantum

#if defined(ENCHANTUM_DETAILS_GCC_MAJOR) && ENCHANTUM_DETAILS_GCC_MAJOR <= 10
  #pragma GCC diagnostic pop
#endif

#include <iostream>
#include <string>

namespace enchantum {
namespace iostream_operators {
  template<typename String, typename E>
  bool extract_enum_from_string(const String& s, E& value, std::true_type)
  {
    if (const auto v = enchantum::cast_bitflag<E>(string_view(s.data(), s.size()))) {
      value = *v;
      return true;
    }
    return false;
  }

  template<typename String, typename E>
  bool extract_enum_from_string(const String& s, E& value, std::false_type)
  {
    if (const auto v = enchantum::cast<E>(string_view(s.data(), s.size()))) {
      value = *v;
      return true;
    }
    return false;
  }

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

    if (!extract_enum_from_string(s, value, std::integral_constant<bool, is_bitflag<E>>{}))
      is.setstate(std::ios_base::failbit);
    return is;
  }
} // namespace iostream_operators
} // namespace enchantum

#include <cstddef>

#if defined(ENCHANTUM_DETAILS_GCC_MAJOR) && ENCHANTUM_DETAILS_GCC_MAJOR <= 10
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif

namespace enchantum {
namespace details {
  template<std::ptrdiff_t N>
  struct next_value_functor {
    template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
    ENCHANTUM_DETAILS_NODISCARD constexpr optional<E> operator()(const E value, const std::ptrdiff_t n = 1) const noexcept
    {
      if (!enchantum::contains(value))
        return optional<E>();

      const auto index = static_cast<std::ptrdiff_t>(*enchantum::enum_to_index(value)) + (n * N);
      if (index >= 0 && index < static_cast<std::ptrdiff_t>(count<E>))
        return optional<E>(values_generator<E>[static_cast<std::size_t>(index)]);
      return optional<E>();
    }
  };

  template<std::ptrdiff_t N>
  struct next_value_circular_functor {
    template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
    ENCHANTUM_DETAILS_NODISCARD constexpr E operator()(const E value, const std::ptrdiff_t n = 1) const noexcept
    {
      ENCHANTUM_ASSERT(enchantum::contains(value), "next/prev_value_circular requires 'value' to be a valid enum member", value);
      const auto     i     = static_cast<std::ptrdiff_t>(*enchantum::enum_to_index(value));
      constexpr auto count = static_cast<std::ptrdiff_t>(enchantum::count<E>);
      return values_generator<E>[static_cast<std::size_t>(((i + (n * N)) % count + count) % count)]; // handles wrap around and negative n
    }
  };
} // namespace details


ENCHANTUM_DETAILS_INLINE_VAR constexpr details::next_value_functor<1>           next_value{};
ENCHANTUM_DETAILS_INLINE_VAR constexpr details::next_value_functor<-1>          prev_value{};
ENCHANTUM_DETAILS_INLINE_VAR constexpr details::next_value_circular_functor<1>  next_value_circular{};
ENCHANTUM_DETAILS_INLINE_VAR constexpr details::next_value_circular_functor<-1> prev_value_circular{};

} // namespace enchantum


#if defined(ENCHANTUM_DETAILS_GCC_MAJOR) && ENCHANTUM_DETAILS_GCC_MAJOR <= 10
  #pragma GCC diagnostic pop
#endif
#if __has_include(<fmt/format.h>)

#include <fmt/format.h>

#ifdef __cpp_concepts
template<enchantum::Enum E>
struct fmt::formatter<E>
#else
template<typename E>
struct fmt::formatter<E, char, std::enable_if_t<std::is_enum<E>::value>>
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
