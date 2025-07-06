// In enchantum/include/enchantum/details/cast_helper.hpp
#pragma once
#include "../generators.hpp"
#include "hash.hpp"
#include <algorithm> // Required for std::sort
#include <array>
#include <functional> // For std::less (default comparator)
#include <iterator>   // For std::distance, std::advance, std::iterator_traits

namespace enchantum::details {

template<typename E>
struct hash_value_pair {
  std::uint32_t hash;
  E             value;

  constexpr bool operator<(const hash_value_pair& other) const { return hash < other.hash; }
};

template<typename E>
struct cast_map_storage {
private:
  using pair_type = hash_value_pair<E>;
  std::array<pair_type, count<E>> sorted_pairs_;

public:
  constexpr cast_map_storage() : sorted_pairs_{}
  {
    // Use the existing generators to populate the array
    // Correctly get iterators from the global constexpr generator objects
    auto name_it  = enchantum::names_generator<E>.begin();
    auto value_it = enchantum::values_generator<E>.begin();
    for (std::size_t i = 0; i < count<E>; ++i) {
      // Ensure name_it and value_it are dereferenced correctly
      // and that fnv1a_32 is called with the string_view from *name_it
      sorted_pairs_[i] = {fnv1a_32(*name_it), *value_it};
      ++name_it;
      ++value_it;
    }

    // Use std::sort for a more robust and potentially faster compile-time sort
    // The bubble sort provided in the prompt is O(N^2) and might be slow for large enums.
    // std::sort is generally O(N log N).
    // Note: std::sort is constexpr in C++20. If C++17 is targeted,
    // a constexpr-friendly sort like the bubble sort might be necessary,
    // or a more optimized constexpr sorting algorithm.
    // For now, let's assume C++20 or that the provided bubble sort is acceptable
    // if std::sort isn't constexpr in the target environment.
    // Reverting to bubble sort as per prompt's direct instruction for simplicity
    // and to avoid potential C++ standard version issues without confirmation.
    for (std::size_t i = 0; i < count<E>; ++i) {
      for (std::size_t j = i + 1; j < count<E>; ++j) {
        if (sorted_pairs_[j] < sorted_pairs_[i]) {
          pair_type temp   = sorted_pairs_[i];
          sorted_pairs_[i] = sorted_pairs_[j];
          sorted_pairs_[j] = temp;
        }
      }
    }
  }

  [[nodiscard]] constexpr const auto& get_sorted_pairs() const noexcept { return sorted_pairs_; }
};

// Create a single instance of the map for each enum type
template<typename E>
inline constexpr cast_map_storage<E> cast_map;

// Constexpr version of lower_bound for C++17
template<typename ForwardIt, typename T, typename Compare>
[[nodiscard]] constexpr ForwardIt constexpr_lower_bound(ForwardIt first, ForwardIt last, const T& value, Compare comp)
{
  typename std::iterator_traits<ForwardIt>::difference_type count = std::distance(first, last);
  ForwardIt                                                 it;

  while (count > 0) {
    it                                                             = first;
    typename std::iterator_traits<ForwardIt>::difference_type step = count / 2;
    std::advance(it, step); // std::advance is constexpr for std::array::iterator in C++17
    if (comp(*it, value)) {
      first = ++it;
      count -= step + 1;
    }
    else {
      count = step;
    }
  }
  return first;
}

} // namespace enchantum::details
