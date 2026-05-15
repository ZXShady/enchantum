#pragma once
#include "details/compat.hpp"
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
