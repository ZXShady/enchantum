#pragma once

// IWYU pragma: begin_exports
#include "common.hpp"              // IWYU pragma: export
#include "details/optional.hpp"    // IWYU pragma: export
#include "details/string_view.hpp" // IWYU pragma: export
#include "entries.hpp"             // IWYU pragma: export
#include "generators.hpp"          // IWYU pragma: export
#include "type_name.hpp"           // IWYU pragma: export
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
[[nodiscard]] constexpr bool contains(const std::underlying_type_t<E> value) noexcept
{
  return details::contains_value<E>(value);
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
[[nodiscard]] constexpr bool contains(const string_view name, const BinaryPred binary_pred) noexcept
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
      return details::enum_to_index_impl<E>(e,
                                            std::integral_constant<bool, is_contiguous<E> && count<E> != 0>{},
                                            std::integral_constant<bool, is_contiguous_bitflag<E>>{});
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
ENCHANTUM_DETAILS_INLINE_VAR constexpr details::index_to_enum_functor<E> index_to_enum{};

ENCHANTUM_DETAILS_INLINE_VAR constexpr details::enum_to_index_functor enum_to_index{};

template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
ENCHANTUM_DETAILS_INLINE_VAR constexpr details::cast_functor<E> cast{};


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
ENCHANTUM_DETAILS_INLINE_VAR constexpr details::to_string_functor to_string{};


} // namespace enchantum


#if defined(ENCHANTUM_DETAILS_GCC_MAJOR) && ENCHANTUM_DETAILS_GCC_MAJOR <= 10
  #pragma GCC diagnostic pop
#endif
