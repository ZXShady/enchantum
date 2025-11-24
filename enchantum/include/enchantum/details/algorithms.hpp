#pragma once
#include <cstddef>
#include <type_traits>
#include "../common.hpp" // for to_underlying if needed, but we can just use < comparison

namespace enchantum {
namespace details {

  template<typename Container, typename T>
  constexpr bool binary_search(const Container& container, const T& value) noexcept
  {
    std::size_t left  = 0;
    std::size_t right = container.size();

    while (left < right) {
      const std::size_t mid = left + (right - left) / 2;
      const auto&       mid_val = container[mid];

      if (mid_val == value)
        return true;

      if (mid_val < value)
        left = mid + 1;
      else
        right = mid;
    }
    return false;
  }

  // Returns index if found, -1 if not (or distinct sentinel)
  // Returns std::size_t, using -1 (SIZE_MAX) as invalid
  template<typename Container, typename T>
  constexpr std::size_t binary_find_index(const Container& container, const T& value) noexcept
  {
    std::size_t left  = 0;
    std::size_t right = container.size();

    while (left < right) {
      const std::size_t mid = left + (right - left) / 2;
      const auto&       mid_val = container[mid];

      if (mid_val == value)
        return mid;

      if (mid_val < value)
        left = mid + 1;
      else
        right = mid;
    }
    return static_cast<std::size_t>(-1);
  }

} // namespace details
} // namespace enchantum
