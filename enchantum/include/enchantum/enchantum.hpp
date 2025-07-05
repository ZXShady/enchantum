#pragma once

#include "common.hpp"
#include "details/optional.hpp"
#include "details/string_view.hpp"
#include "entries.hpp" // Assumes this is where static_storage, fnv1a_32, hash_index_pair are.
#include <algorithm>   // For std::lower_bound
#include <bit>
#include <limits> // Required for std::numeric_limits
#include <type_traits>
#include <utility> // For std::pair

namespace enchantum {

// --- Phase 1: Declarations ---
// Forward declarations for detail structs are not strictly necessary if defined before use by API variable templates.
// Declarations of public API free functions
template<Enum E>
[[nodiscard]] constexpr bool contains(const std::underlying_type_t<E> value) noexcept;

template<Enum E>
[[nodiscard]] constexpr bool contains(E value) noexcept;

template<Enum E>
[[nodiscard]] constexpr bool contains(string_view name) noexcept;

template<Enum E, std::predicate<string_view, string_view> BinaryPredicate>
[[nodiscard]] constexpr bool contains(string_view name, BinaryPredicate binary_predicate) noexcept;
// Add other free function declarations if any (e.g., next_value, prev_value if they were not functors)


// --- Phase 2: Full Definitions ---

namespace details {
  // Utility (can be here or above, as long as it's before use)
  constexpr std::pair<std::size_t, std::size_t> minmax_string_size(const string_view* begin, const string_view* const end)
  {
    using T          = std::size_t;
    auto minmax_pair = std::pair<T, T>(std::numeric_limits<T>::max(), T{0});
    for (; begin != end; ++begin) {
      const auto current_size = begin->size();
      minmax_pair.first       = minmax_pair.first < current_size ? minmax_pair.first : current_size;
      minmax_pair.second      = minmax_pair.second > current_size ? minmax_pair.second : current_size;
    }
    return minmax_pair;
  }

  // Full definitions of ALL detail functor structs
  template<typename E>
  struct index_to_enum_functor {
    [[nodiscard]] constexpr optional<E> operator()(const std::size_t idx) const noexcept
    {
      optional<E> res;
      if (idx < enchantum::count<E>)
        res.emplace(enchantum::values<E>[idx]);
      return res;
    }
  };

  struct enum_to_index_functor {
    template<Enum E>
    [[nodiscard]] constexpr optional<std::size_t> operator()(const E e_val) const noexcept
    {
      using T = std::underlying_type_t<E>;
      // The 'contains' called here must be the one declared above and defined below.
      // This implies a potential ordering issue if contains itself depends on enum_to_index.
      // Let's assume for now that 'contains' implementations do not call 'enum_to_index' directly or indirectly in a blocking way.
      if constexpr (is_contiguous<E>) {
        if (enchantum::contains(e_val)) {
          return optional<std::size_t>(static_cast<std::size_t>(static_cast<T>(e_val) - static_cast<T>(min<E>)));
        }
      }
      else if constexpr (is_contiguous_bitflag<E>) {
        if (enchantum::contains(e_val)) {
          constexpr bool has_zero = has_zero_flag<E>;
          if constexpr (has_zero)
            if (static_cast<T>(e_val) == 0)
              return optional<std::size_t>(0);
          using U = std::make_unsigned_t<T>;
          return optional<std::size_t>(has_zero + std::countr_zero(static_cast<U>(e_val)) -
                                       std::countr_zero(static_cast<U>(enchantum::values<E>[has_zero])));
        }
      }
      else {
        constexpr auto&       local_vals = enchantum::values<E>;
        constexpr std::size_t num        = enchantum::count<E>;
        for (std::size_t i = 0; i < num; ++i) {
          if (local_vals[i] == e_val)
            return i;
        }
      }
      return optional<std::size_t>();
    }
  };

  template<typename E>
  struct cast_functor {
    [[nodiscard]] constexpr optional<E> operator()(const std::underlying_type_t<E> val) const noexcept
    {
      // Break circular dependency: Check against known values directly
      constexpr auto& local_values = enchantum::values<E>;
      for (const auto& enum_val : local_values) {
        if (static_cast<std::underlying_type_t<E>>(enum_val) == val) {
          return optional<E>{static_cast<E>(val)};
        }
      }
      return optional<E>{}; // Not found
    }

    [[nodiscard]] constexpr auto operator()(string_view name_sv) const noexcept -> optional<E>
    {
      constexpr auto& hashes_arr = enchantum::hashes<E>;
      constexpr auto& names_arr  = enchantum::names<E>;
      constexpr auto& values_arr = enchantum::values<E>;

      if (name_sv.empty() && names_arr.empty())
        return optional<E>();
      if (hashes_arr.empty())
        return optional<E>();

      const auto name_hash = details::fnv1a_32(name_sv);

      const auto it = std::lower_bound(hashes_arr.begin(),
                                       hashes_arr.end(),
                                       details::hash_index_pair{name_hash, 0},
                                       [](const details::hash_index_pair& pair_elem,
                                          const details::hash_index_pair& val_to_find) {
                                         return pair_elem.hash < val_to_find.hash;
                                       });

      if (it != hashes_arr.end() && it->hash == name_hash) {
        auto current_it = it;
        while (current_it != hashes_arr.end() && current_it->hash == name_hash) {
          if (names_arr[current_it->index] == name_sv) {
            return optional<E>{values_arr[current_it->index]};
          }
          ++current_it;
        }
      }
      return optional<E>{};
    }

    template<std::predicate<string_view, string_view> BinaryPred>
    [[nodiscard]] constexpr optional<E> operator()(string_view name_sv, const BinaryPred binary_pred) const noexcept
    {
      optional<E>           res;
      constexpr auto&       local_names_arr  = enchantum::names<E>;
      constexpr auto&       local_values_arr = enchantum::values<E>;
      constexpr std::size_t num              = enchantum::count<E>;
      for (std::size_t i = 0; i < num; ++i) {
        if (binary_pred(name_sv, local_names_arr[i])) {
          res.emplace(local_values_arr[i]);
          return res;
        }
      }
      return res;
    }
  };

  struct to_string_functor {
    template<Enum E>
    [[nodiscard]] constexpr string_view operator()(const E val) const noexcept
    {
      // Call the enum_to_index_functor directly to avoid potential circular dependency with API variable templates
      if (const auto i = details::enum_to_index_functor{}(val)) {
        return enchantum::names<E>[*i]; // enchantum::names is fine as it resolves to constexpr data
      }
      return string_view();
    }
  };
} // namespace details

// Definitions of API variable templates (these instantiate the functors)
// These MUST come AFTER the full definition of their respective detail functor structs.
template<Enum E>
inline constexpr details::index_to_enum_functor<E> index_to_enum{};

inline constexpr details::enum_to_index_functor enum_to_index{};

template<typename E>
inline constexpr details::cast_functor<E> cast{};

inline constexpr details::to_string_functor to_string{};


// Full definitions of public API functions
template<Enum E>
[[nodiscard]] constexpr bool contains(const std::underlying_type_t<E> value_val) noexcept
{
  using T = std::underlying_type_t<E>;
  if (value_val < static_cast<T>(min<E>) || value_val > static_cast<T>(max<E>))
    return false;
  if constexpr (is_contiguous_bitflag<E>) {
    if constexpr (has_zero_flag<E>)
      if (value_val == 0)
        return true;
    return std::popcount(static_cast<std::make_unsigned_t<T>>(value_val)) == 1;
  }
  else if constexpr (is_contiguous<E>) {
    return true;
  }
  else {
    // This now calls API variable template enchantum::values
    for (const auto v_enum : enchantum::values<E>)
      if (static_cast<T>(v_enum) == value_val)
        return true;
    return false;
  }
}

template<Enum E>
[[nodiscard]] constexpr bool contains(const E value_val) noexcept
{
  return enchantum::contains<E>(static_cast<std::underlying_type_t<E>>(value_val));
}

template<Enum E>
[[nodiscard]] constexpr bool contains(string_view name_sv) noexcept
{
  // This now calls API variable template enchantum::names and enchantum::count
  constexpr auto minmax_val = details::minmax_string_size(enchantum::names<E>.data(),
                                                          enchantum::names<E>.data() + enchantum::count<E>);
  if (const auto current_size = name_sv.size(); current_size < minmax_val.first || current_size > minmax_val.second)
    return false;
  // This now calls API variable template enchantum::cast
  return static_cast<bool>(enchantum::cast<E>(name_sv));
}

template<Enum E, std::predicate<string_view, string_view> BinaryPredicate>
[[nodiscard]] constexpr bool contains(string_view name_sv, BinaryPredicate binary_pred) noexcept
{
  constexpr auto& local_names_arr = enchantum::names<E>; // Calls API variable template
  for (const auto& s_view : local_names_arr) {
    if (binary_pred(name_sv, s_view))
      return true;
  }
  return false;
}

} // namespace enchantum