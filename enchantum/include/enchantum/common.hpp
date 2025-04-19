#pragma once
#include <concepts>
#include <limits>
#include <string_view>

#ifndef ENCHANTUM_MAX_RANGE
  #define ENCHANTUM_MAX_RANGE 256
#endif
#ifndef ENCHANTUM_MIN_RANGE
  #define ENCHANTUM_MIN_RANGE (-ENCHANTUM_MAX_RANGE)
#endif
#ifndef ENCHANTUM_SEARCH_PER_ITERATION
  #define ENCHANTUM_SEARCH_PER_ITERATION 32
#endif

#if ENCHANTUM_SEARCH_PER_ITERATION < 16
#error It is recommended to use a higher ENCHANTUM_SEARCH_PER_ITERATION since decreasing it can lead to inaccurate reflection results. 
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

template<typename E,typename Underlying>
concept EnumOfUnderlying = Enum<E> && std::same_as<std::underlying_type_t<E>,Underlying>;

template <Enum E>
inline constexpr bool is_bitflag =  
(
    requires(E e) { { e & e } -> std::same_as<E>; } ||
    requires(E e) { { e & e } -> std::same_as<bool>; }
) &&
requires(E e) {
    { ~e } -> std::same_as<E>;
    { e | e } -> std::same_as<E>;
    { e &= e } -> std::same_as<E&>;
    { e |= e } -> std::same_as<E&>;
};

template<typename T>
concept BitFlagEnum = Enum<T> && is_bitflag<T>;

template<typename T>
struct enum_traits;

template<SignedEnum E> struct enum_traits<E> {
private:
  using U = std::underlying_type_t<E>;
  using L = std::numeric_limits<U>;
public:
  static constexpr auto min        = (L::min)() > ENCHANTUM_MIN_RANGE ? (L::min)() : ENCHANTUM_MIN_RANGE;
  static constexpr auto max        = (L::max)() < ENCHANTUM_MAX_RANGE ? (L::max)() : ENCHANTUM_MAX_RANGE;
  static constexpr bool is_bitflag = false;
};

template<UnsignedEnum E>
struct enum_traits<E> {
private:
  using U = std::underlying_type_t<E>;
  using L = std::numeric_limits<U>;
public:
  static constexpr auto min        = ENCHANTUM_MIN_RANGE < 0 ? 0 : ENCHANTUM_MIN_RANGE;
  static constexpr auto max        = (L::max)() < ENCHANTUM_MAX_RANGE ? (L::max)() : ENCHANTUM_MAX_RANGE;
  static constexpr bool is_bitflag = false;
};


} // namespace enchantum