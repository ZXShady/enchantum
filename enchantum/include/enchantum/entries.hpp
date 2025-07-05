#pragma once

#include "details/hash.hpp" // Added for FNV-1a hash
#include "details/string_view.hpp"
#include <algorithm> // Required for std::sort (or a constexpr alternative)
#include <array>     // Required for std::array

#if defined(__clang__)
  #include "details/enchantum_clang.hpp"
#elif defined(__GNUC__) || defined(__GNUG__)
  #include "details/enchantum_gcc.hpp"
#elif defined(_MSC_VER)
  #include "details/enchantum_msvc.hpp"
#endif

#include "common.hpp"
#include <type_traits>
#include <utility>

namespace enchantum {

#ifdef __cpp_lib_to_underlying
using ::std::to_underlying;
#else
template<Enum E>
[[nodiscard]] constexpr auto to_underlying(const E e) noexcept
{
  return static_cast<std::underlying_type_t<E>>(e);
}
#endif


namespace details {
  // Helper struct for hash map
  struct hash_index_pair {
    std::uint32_t hash;
    std::size_t   index;

    constexpr bool operator<(const hash_index_pair& other) const { return hash < other.hash; }
  };

  // Constexpr bubble sort (simple enough for small N)
  template<typename T, std::size_t N>
  constexpr std::array<T, N> bubble_sort(std::array<T, N> arr)
  {
    bool swapped;
    for (std::size_t i = 0; i < N - 1; ++i) {
      swapped = false;
      for (std::size_t j = 0; j < N - i - 1; ++j) {
        if (arr[j + 1] < arr[j]) {
          T temp     = arr[j];
          arr[j]     = arr[j + 1];
          arr[j + 1] = temp;
          swapped    = true;
        }
      }
      if (!swapped) {
        break;
      }
    }
    return arr;
  }

  template<typename E, bool NullTerminated = true>
  struct static_storage {
    using enum_type                            = E;
    static constexpr auto        reflected_raw = details::reflect<std::remove_cv_t<E>, NullTerminated>();
    static constexpr std::size_t count         = sizeof(reflected_raw.values) / sizeof(reflected_raw.values[0]);

    std::array<E, count>               values_arr;
    std::array<string_view, count>     names_arr;
    std::array<hash_index_pair, count> hashes_arr; // New hashes array

    constexpr static_storage() : values_arr{}, names_arr{}, hashes_arr{}
    {
      std::array<hash_index_pair, count> temp_hashes{};
      for (std::size_t i = 0; i < count; ++i) {
        values_arr[i]  = reflected_raw.values[i];
        names_arr[i]   = string_view(reflected_raw.strings + reflected_raw.string_indices[i],
                                   reflected_raw.strings + reflected_raw.string_indices[i + 1] - NullTerminated);
        temp_hashes[i] = {fnv1a_32(names_arr[i]), i};
      }
      hashes_arr = bubble_sort(temp_hashes); // Sort hashes
    }
  };

  template<typename E, bool NullTerminated = true>
  inline constexpr static_storage<E, NullTerminated> storage_v{};

} // namespace details


template<Enum E, typename Pair = std::pair<E, string_view>, bool NullTerminated = true>
inline constexpr auto entries = []() {
  constexpr auto&                 storage = details::storage_v<std::remove_cv_t<E>, NullTerminated>;
  std::array<Pair, storage.count> ret;

  for (std::size_t i = 0; i < storage.count; ++i) {
    ret[i] = {storage.values_arr[i], storage.names_arr[i]};
  }
  return ret;
}();

template<Enum E>
inline constexpr auto values = details::storage_v<std::remove_cv_t<E>>.values_arr;

template<Enum E, typename String = string_view, bool NullTerminated = true>
inline constexpr auto names = []() {
  // This needs to be a lambda to handle potential String type conversion if different from string_view
  constexpr auto& storage_names = details::storage_v<std::remove_cv_t<E>, NullTerminated>.names_arr;
  if constexpr (std::is_same_v<String, string_view>) {
    return storage_names;
  }
  else {
    std::array<String, storage_names.size()> ret;
    for (std::size_t i = 0; i < storage_names.size(); ++i) {
      ret[i] = String(storage_names[i].data(), storage_names[i].size());
    }
    return ret;
  }
}();

// New variable template for accessing hashes
template<Enum E, bool NullTerminated = true>
inline constexpr auto hashes = details::storage_v<std::remove_cv_t<E>, NullTerminated>.hashes_arr;

template<Enum E>
inline constexpr auto min = details::storage_v<std::remove_cv_t<E>>.values_arr[0]; // Assuming sorted by value or direct access

template<Enum E>
inline constexpr auto
  max = details::storage_v<std::remove_cv_t<E>>.values_arr[details::storage_v<std::remove_cv_t<E>>.count - 1]; // Assuming sorted or direct access

template<Enum E>
inline constexpr std::size_t count = details::storage_v<std::remove_cv_t<E>>.count;


template<typename>
inline constexpr bool has_zero_flag = false;

template<BitFlagEnum E>
inline constexpr bool has_zero_flag<E> = []() {
  for (const auto v : values<E>) // Uses the new values
    if (static_cast<std::underlying_type_t<E>>(v) == 0)
      return true;
  return false;
}();

template<typename>
inline constexpr bool is_contiguous = false;

template<Enum E>
inline constexpr bool is_contiguous<E> = static_cast<std::size_t>(enchantum::to_underlying(max<E>) -
                                                                  enchantum::to_underlying(min<E>)) + // Uses new min/max
    1 ==
  count<E>; // Uses new count

template<typename E>
concept ContiguousEnum = Enum<E> && is_contiguous<E>;

template<typename>
inline constexpr bool is_contiguous_bitflag = false;

template<BitFlagEnum E>
inline constexpr bool is_contiguous_bitflag<E> = []() {
  constexpr auto resolved_values = values<E>; // Use new values
  using T                        = std::underlying_type_t<E>;
  for (auto i = std::size_t{has_zero_flag<E>}; i < resolved_values.size() - 1; ++i)
    if (T(resolved_values[i]) << 1 != T(resolved_values[i + 1]))
      return false;
  return true; // Corrected: should return true if loop completes
}();

// Removed duplicate definitions of names, min, max, count, has_zero_flag, is_contiguous, ContiguousEnum, is_contiguous_bitflag
// The definitions above using details::storage_v are now the sole definitions.

template<typename E>
concept ContiguousBitFlagEnum = BitFlagEnum<E> && is_contiguous_bitflag<E>;

} // namespace enchantum