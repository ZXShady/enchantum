#pragma once

#include "common.hpp"
#include "details/optional.hpp"
#include "details/string_view.hpp"
#include "entries.hpp"
#include <algorithm> // Added for std::lower_bound
#include <iterator>  // Added for std::distance
#include <type_traits>
#include <utility>

namespace enchantum {

namespace details {
// Helper function implementations (moved here for correct declaration order)
template<BitFlagEnum E>
constexpr bool compute_has_zero_flag_impl() {
    // values<E> is from enchantum::entries.hpp
    for (const auto v : values<E>)
        if (static_cast<std::underlying_type_t<E>>(v) == 0)
            return true;
    return false;
}

template<Enum E>
constexpr bool compute_is_contiguous_impl() {
    // entries<E> from enchantum::entries.hpp
    constexpr auto& enums = entries<E>;
    if (enums.size() <= 1) {
        return true;
    }
    using T = std::underlying_type_t<E>;
    if constexpr (std::is_same_v<T, bool>) {
         return true;
    }
    for (std::size_t i = 0; i < enums.size() - 1; ++i) {
        if (static_cast<T>(enums[i].first) + 1 != static_cast<T>(enums[i + 1].first)) {
            return false;
        }
    }
    return true;
}

template<BitFlagEnum E>
constexpr bool compute_is_contiguous_bitflag_impl() {
    constexpr auto& enums = entries<E>;
    if (enums.empty()) { // Explicitly handle empty case
        return true;
    }
    // Use details::compute_has_zero_flag_impl<E>() to avoid circular dependency
    // on the public ::enchantum::has_zero_flag<E> variable during its own initialization.
    constexpr std::size_t start_offset = details::compute_has_zero_flag_impl<E>() ? 1 : 0;

    // If, after potentially skipping a zero flag, there's 0 or 1 element left, it's contiguous.
    if (enums.size() <= start_offset + 1) {
        return true;
    }

    // Now, there are at least two elements in the sequence part to check.
    using T = std::underlying_type_t<E>;
    for (std::size_t i = start_offset; i < enums.size() - 1; ++i) {
        if (static_cast<T>(enums[i].first) << 1 != static_cast<T>(enums[i + 1].first)) {
            return false;
        }
    }
    return true;
}
// End of helper function implementations
} // namespace details

// General templates (defaults)
template<typename>
inline constexpr bool has_zero_flag = false;

template<typename>
inline constexpr bool is_contiguous = false;

template<typename>
inline constexpr bool is_contiguous_bitflag = false;

// Specializations using helper functions via IILE
template<BitFlagEnum E>
inline constexpr bool has_zero_flag<E> = []() {
    return details::compute_has_zero_flag_impl<E>();
}();

template<Enum E>
inline constexpr bool is_contiguous<E> = []() {
    return details::compute_is_contiguous_impl<E>();
}();

template<BitFlagEnum E>
inline constexpr bool is_contiguous_bitflag<E> = []() {
    return details::compute_is_contiguous_bitflag_impl<E>();
}();

template<typename E>
concept ContiguousEnum = Enum<E> && is_contiguous<E>;

template<typename E>
concept ContiguousBitFlagEnum = BitFlagEnum<E> && is_contiguous_bitflag<E>;

template<Enum E>
[[nodiscard]] constexpr bool contains(const E value) noexcept
{
  // values<E> is sorted, so we can use binary_search.
  return std::binary_search(values<E>.begin(), values<E>.end(), value);
}

template<Enum E>
[[nodiscard]] constexpr bool contains(const std::underlying_type_t<E> value) noexcept
{
  return enchantum::contains(static_cast<E>(value));
}


template<Enum E>
[[nodiscard]] constexpr bool contains(const string_view name) noexcept
{
  // Use sorted_names_entries for efficient lookup
  const auto& sorted_entries = enchantum::sorted_names_entries<E>;
  const auto it = std::lower_bound(sorted_entries.begin(), sorted_entries.end(), name,
                                   [](const auto& entry, const string_view sv) {
                                     return entry.name < sv;
                                   });
  return (it != sorted_entries.end() && it->name == name);
}


template<Enum E, std::predicate<string_view, string_view> BinaryPredicate>
[[nodiscard]] constexpr bool contains(const string_view name, const BinaryPredicate binary_predicate) noexcept
{
  for (const auto& s : names<E>)
    if (binary_predicate(name, s))
      return true;
  return false;
}

template<ContiguousEnum E>
[[nodiscard]] constexpr bool contains(const E value) noexcept
{
  using T = std::underlying_type_t<E>;
  return T(value) <= T(max<E>) && T(value) >= T(min<E>);
}

namespace details {

// Helper functions were moved to the top of namespace enchantum.
// This details namespace now only contains the functors etc.

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
      if constexpr (ContiguousEnum<E>) {
        using T = std::underlying_type_t<E>;
        if (enchantum::contains(e))
          return optional<std::size_t>(std::size_t(T(e) - T(min<E>)));
      }
      else {
        const auto& v = values<E>;
        const auto it = std::lower_bound(v.begin(), v.end(), e);
        if (it != v.end() && *it == e) {
          return optional<std::size_t>(std::distance(v.begin(), it));
        }
      }
      return optional<std::size_t>();
    }
  };


  template<Enum E>
  struct cast_functor {
    [[nodiscard]] constexpr optional<E> operator()(const std::underlying_type_t<E> value) const noexcept
    {
      optional<E> a;
      if (!enchantum::contains<E>(value))
        return a;
      a.emplace(static_cast<E>(value));
      return a;
    }

    [[nodiscard]] constexpr optional<E> operator()(const string_view name) const noexcept
    {
      // Use sorted_names_entries for efficient lookup
      const auto& sorted_entries = enchantum::sorted_names_entries<E>;
      const auto it = std::lower_bound(sorted_entries.begin(), sorted_entries.end(), name,
                                     [](const auto& entry, const string_view sv) {
                                       return entry.name < sv;
                                     });

      if (it != sorted_entries.end() && it->name == name) {
        return optional<E>(it->value);
      }
      return optional<E>();
    }

    template<std::predicate<string_view, string_view> BinaryPred>
    [[nodiscard]] constexpr optional<E> operator()(const string_view name, const BinaryPred binary_predicate) const noexcept
    {
      optional<E> a;
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