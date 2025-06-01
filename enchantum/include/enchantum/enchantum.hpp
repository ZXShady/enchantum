#pragma once

#include "common.hpp"
#include "details/optional.hpp"
#include "details/string_view.hpp"
#include "entries.hpp"
#include <bit>
#include <type_traits>
#include <utility>
namespace enchantum {

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
inline constexpr bool is_contiguous<E> = static_cast<std::size_t>(to_underlying(max<E>) - to_underlying(min<E>)) + 1 ==
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


template<Enum E>
[[nodiscard]] constexpr bool contains(const std::underlying_type_t<E> value) noexcept
{
  using T = std::underlying_type_t<E>;
  if (value < T(min<E>) || value > T(max<E>))
    return false;

  if constexpr (is_bitflag<E>) {
    if constexpr (has_zero_flag<E>)
      if (value == 0)
        return true;
    return 1 == std::popcount(static_cast<std::make_unsigned_t<T>>(value));
  }
  else {
    for (const auto v : values<E>)
      if (static_cast<T>(v) == value)
        return true;
    return is_contiguous<E>;
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
  for (const auto& s : names<E>)
    if (s == name)
      return true;
  return false;
}


template<Enum E, std::predicate<string_view, string_view> BinaryPredicate>
[[nodiscard]] constexpr bool contains(const string_view name, const BinaryPredicate binary_predicate) noexcept
{
  for (const auto& s : names<E>)
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
      if (index < values<E>.size())
        ret.emplace(values<E>[index]);
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
          return has_zero + std::countr_zero(static_cast<U>(e)) - std::countr_zero(static_cast<U>(values<E>[has_zero]));
        }
      }
      else {
        for (std::size_t i = 0; i < values<E>.size(); ++i) {
          if (values<E>[i] == e)
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
      for (const auto& [e, s] : entries<E>) {
        if (s == name) {
          a.emplace(e);
          return a;
        }
      }
      return a; // nullopt
    }

    template<std::predicate<string_view, string_view> BinaryPred>
    [[nodiscard]] constexpr optional<E> operator()(const string_view name, const BinaryPred binary_predicate) const noexcept
    {
      optional<E> a; // rvo not that it really matters
      for (const auto& [e, s] : entries<E>) {
        if (binary_predicate(name, s)) {
          a.emplace(e);
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
        return names<E>[*i];
      return string_view();
    }
  };

} // namespace details
inline constexpr details::to_string_functor to_string{};


} // namespace enchantum