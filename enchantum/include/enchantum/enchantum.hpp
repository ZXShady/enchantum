#pragma once

#include "common.hpp"
#include "details/cast_helper.hpp" // My new file
#include "details/optional.hpp"
#include "details/string_view.hpp"
#include "entries.hpp"
#include "generators.hpp"
#include <bit>
#include <limits> // For std::numeric_limits
#include <type_traits>
#include <utility>

namespace enchantum {

// Phase A: Forward Declarations
namespace details {
  constexpr std::pair<std::size_t, std::size_t> minmax_string_size(const string_view* begin, const string_view* const end);
  template<Enum E>
  struct index_to_enum_functor;
  struct enum_to_index_functor;
  struct to_string_functor;
  template<Enum E>
  struct cast_functor;
} // namespace details

template<Enum E>
[[nodiscard]] constexpr bool contains(const std::underlying_type_t<E> value) noexcept;
template<Enum E>
[[nodiscard]] constexpr bool contains(const E value) noexcept;
template<Enum E>
[[nodiscard]] constexpr bool contains(const string_view name) noexcept;
template<Enum E, std::predicate<string_view, string_view> BinaryPredicate>
[[nodiscard]] constexpr bool contains(const string_view name, const BinaryPredicate binary_predicate) noexcept;
// End of Phase A

// Phase B: Full definitions of `details` structs and `details` helpers.
namespace details {

  constexpr std::pair<std::size_t, std::size_t> minmax_string_size(const string_view* begin, const string_view* const end)
  {
    using T     = std::size_t;
    auto minmax = std::pair<T, T>(std::numeric_limits<T>::max(), T{});
    for (; begin != end; ++begin) {
      const auto size = begin->size();
      minmax.first    = minmax.first < size ? minmax.first : size;
      minmax.second   = minmax.second > size ? minmax.second : size;
    }
    return minmax;
  }

  template<Enum E>
  struct index_to_enum_functor {
    [[nodiscard]] constexpr optional<E> operator()(const std::size_t index) const noexcept
    {
      optional<E> ret;
      if (index < count<E>)
        ret.emplace(::enchantum::values_generator<E>[index]);
      return ret;
    }
  };

  struct enum_to_index_functor {
    template<Enum E_ETI>
    [[nodiscard]] constexpr optional<std::size_t> operator()(const E_ETI e) const noexcept
    {
      using T = std::underlying_type_t<E_ETI>;
      if constexpr (is_contiguous<E_ETI>) {
        if (::enchantum::contains(e)) { // Calls fwd-declared ::enchantum::contains
          return optional<std::size_t>(std::size_t(T(e) - T(min<E_ETI>)));
        }
      }
      else if constexpr (is_contiguous_bitflag<E_ETI>) {
        if (::enchantum::contains(e)) { // Calls fwd-declared ::enchantum::contains
          constexpr bool has_zero = has_zero_flag<E_ETI>;
          if constexpr (has_zero)
            if (static_cast<T>(e) == 0)
              return optional<std::size_t>(0);
          using U = std::make_unsigned_t<T>;
          return has_zero + std::countr_zero(static_cast<U>(e)) -
            std::countr_zero(static_cast<U>(::enchantum::values_generator<E_ETI>[has_zero]));
        }
      }
      else {
        for (std::size_t i = 0; i < count<E_ETI>; ++i) {
          if (::enchantum::values_generator<E_ETI>[i] == e)
            return optional<std::size_t>(i);
        }
      }
      return optional<std::size_t>();
    }
  };

  struct to_string_functor {
    template<Enum E_TSF>
    [[nodiscard]] constexpr string_view operator()(const E_TSF value) const noexcept
    {
      constexpr details::enum_to_index_functor get_index_func;
      if (const auto i = get_index_func(value))
        return ::enchantum::names_generator<E_TSF>[*i];
      return string_view();
    }
  };

  template<Enum E>
  struct cast_functor {
    [[nodiscard]] constexpr optional<E> operator()(const std::underlying_type_t<E> value) const noexcept
    {
      optional<E> a;
      if (!::enchantum::contains<E>(value)) // Calls fwd-declared ::enchantum::contains
        return a;
      a.emplace(static_cast<E>(value));
      return a;
    }

    [[nodiscard]] constexpr auto operator()(string_view name) const noexcept -> optional<E>
    {
      const auto            name_hash = details::fnv1a_32(name);
      constexpr const auto& pairs     = details::cast_map<E>.get_sorted_pairs();
      const auto            it        = details::constexpr_lower_bound(pairs.begin(),
                                                     pairs.end(),
                                                     name_hash,
                                                     [](const details::hash_value_pair<E>& pair, std::uint32_t h) {
                                                       return pair.hash < h;
                                                     });
      if (it != pairs.end() && it->hash == name_hash) {
        auto current_it = it;
        while (current_it != pairs.end() && current_it->hash == name_hash) {
          constexpr details::to_string_functor get_name_func;
          if (get_name_func(current_it->value) == name) {
            return optional<E>{current_it->value};
          }
          ++current_it;
        }
      }
      return optional<E>{};
    }

    template<std::predicate<string_view, string_view> BinaryPred>
    [[nodiscard]] constexpr optional<E> operator()(const string_view name, const BinaryPred binary_predicate) const noexcept
    {
      optional<E> a;
      for (std::size_t i = 0; i < count<E>; ++i) {
        if (binary_predicate(name, ::enchantum::names_generator<E>[i])) {
          a.emplace(::enchantum::values_generator<E>[i]);
          return a;
        }
      }
      return a;
    }
  };
} // namespace details

// Phase C: Full definitions of global `enchantum` objects.
template<Enum E>
inline constexpr details::index_to_enum_functor<E> index_to_enum{};
inline constexpr details::enum_to_index_functor    enum_to_index{};
inline constexpr details::to_string_functor        to_string{};
template<Enum E>
inline constexpr details::cast_functor<E> cast{};

// Phase D: Full definitions of `enchantum::contains` functions.
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
    for (const auto v : ::enchantum::values_generator<E>)
      if (static_cast<T>(v) == value)
        return true;
    return false;
  }
}

template<Enum E>
[[nodiscard]] constexpr bool contains(const E value) noexcept
{
  return ::enchantum::contains<E>(static_cast<std::underlying_type_t<E>>(value));
}

template<Enum E>
[[nodiscard]] constexpr bool contains(const string_view name) noexcept
{
  constexpr auto minmax_s = details::minmax_string_size(::enchantum::names<E>.data(),
                                                        ::enchantum::names<E>.data() + ::enchantum::names<E>.size());
  if (const auto size = name.size(); size < minmax_s.first || size > minmax_s.second)
    return false;
  for (const auto s : ::enchantum::names_generator<E>)
    if (s == name)
      return true;
  return false;
}

template<Enum E, std::predicate<string_view, string_view> BinaryPredicate>
[[nodiscard]] constexpr bool contains(const string_view name, const BinaryPredicate binary_predicate) noexcept
{
  for (const auto s : ::enchantum::names_generator<E>)
    if (binary_predicate(name, s))
      return true;
  return false;
}

} // namespace enchantum