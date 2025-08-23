#pragma once

// Policy macros for string→enum cast optimization
#ifndef ENCHANTUM_ENUM_POLICY_AUTO
#define ENCHANTUM_ENUM_POLICY_AUTO 0
#endif
#ifndef ENCHANTUM_ENUM_POLICY_NAIVE
#define ENCHANTUM_ENUM_POLICY_NAIVE 1
#endif
#ifndef ENCHANTUM_ENUM_POLICY_LEN_FIRST
#define ENCHANTUM_ENUM_POLICY_LEN_FIRST 2
#endif

#ifndef ENCHANTUM_ENUM_TINY_THRESHOLD
#define ENCHANTUM_ENUM_TINY_THRESHOLD 3
#endif
#ifndef ENCHANTUM_ENUM_SMALL_THRESHOLD
#define ENCHANTUM_ENUM_SMALL_THRESHOLD 12
#endif
#ifndef ENCHANTUM_ENUM_BUCKET_HASH_THRESHOLD
#define ENCHANTUM_ENUM_BUCKET_HASH_THRESHOLD 8
#endif

#ifndef ENCHANTUM_ENUM_DISPATCH_POLICY
#define ENCHANTUM_ENUM_DISPATCH_POLICY ENCHANTUM_ENUM_POLICY_AUTO
#endif

#include "common.hpp"
#include "details/optional.hpp"
#include "details/string_view.hpp"
#include "entries.hpp"
#include "generators.hpp"
#include <type_traits>
#include <utility>
#include <array>
#include <cstdint>

#if defined(ENCAHNTUM_DETAILS_GCC_MAJOR) && ENCAHNTUM_DETAILS_GCC_MAJOR <= 10
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wconversion"
  #pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif


namespace enchantum {

namespace details {
  template<typename BinaryPredicate>
  constexpr bool call_predicate(const BinaryPredicate binary_pred, const string_view a, const string_view b)
  {
    if constexpr (std::is_invocable_v<const BinaryPredicate&, const char&, const char&>) {
      const auto a_size = a.size();
      if (a_size != b.size())
        return false;
      const auto a_data = a.data();
      const auto b_data = b.data();

      for (std::size_t i = 0; i < a_size; ++i)
        if (!binary_pred(a_data[i],b_data[i]))
          return false;
      return true;
    }
    else {
      static_assert(std::is_invocable_v<const BinaryPredicate&, const string_view&, const string_view&>,
                    "BinaryPredicate must be callable with atleast 2 char or 2 string_views");
      return binary_pred(a, b);
    }
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

// constexpr FNV-1a (32-bit) for optional in-bucket hashing
constexpr uint32_t fnv1a32_sv(string_view sv) noexcept {
  uint32_t h = 2166136261u;
  const char* p = sv.data();
  for (size_t i = 0; i < sv.size(); ++i) {
    h ^= static_cast<unsigned char>(p[i]);
    h *= 16777619u;
  }
  return h;
}

template<typename E>
struct len_first_index {
  static constexpr std::size_t N = count<E>;
  static_assert(N <= 0xFFFF, "Too many enum entries; widen indices if needed.");

  static constexpr std::array<std::uint8_t, N> build_lengths() {
    std::array<std::uint8_t, N> a{};
    for (std::size_t i = 0; i < N; ++i) {
      const auto s = names_generator<E>[i];
      a[i] = static_cast<std::uint8_t>(s.size()); // reflection stores uint8 lengths
    }
    return a;
  }

  static constexpr std::array<unsigned char, N> build_firstchars() {
    std::array<unsigned char, N> a{};
    for (std::size_t i = 0; i < N; ++i) {
      const auto s = names_generator<E>[i];
      a[i] = s.size() ? static_cast<unsigned char>(s[0]) : 0;
    }
    return a;
  }

  static constexpr auto lens = build_lengths();
  static constexpr auto fcs  = build_firstchars();

  // Counting sort by length, then by first char within each length bucket
  static constexpr std::array<std::uint16_t, N> build_sorted_indices() {
    std::array<std::uint16_t, N> by_len{};
    std::array<std::uint16_t, N> out{};
    std::array<std::uint16_t, 257> cnt_len{};
    std::array<std::uint16_t, 257> start_len{};

    for (std::size_t i = 0; i < N; ++i) ++cnt_len[lens[i]];

    start_len[0] = 0;
    for (std::size_t L = 1; L <= 256; ++L)
      start_len[L] = static_cast<std::uint16_t>(start_len[L-1] + cnt_len[L-1]);

    auto pos_len = start_len;
    for (std::uint16_t i = 0; i < N; ++i)
      by_len[pos_len[lens[i]]++] = i;

    for (std::size_t L = 0; L <= 255; ++L) {
      const std::uint16_t begin = start_len[L];
      const std::uint16_t end   = static_cast<std::uint16_t>(start_len[L] + cnt_len[L]);
      const std::uint16_t bucket = end - begin;
      if (!bucket) continue;

      std::array<std::uint16_t, 257> cnt_fc{};
      std::array<std::uint16_t, 257> start_fc{};

      for (std::uint16_t k = 0; k < bucket; ++k) {
        const auto idx = by_len[begin + k];
        ++cnt_fc[fcs[idx]];
      }

      start_fc[0] = begin;
      for (std::size_t c = 1; c <= 256; ++c)
        start_fc[c] = static_cast<std::uint16_t>(start_fc[c-1] + cnt_fc[c-1]);

      auto pos_fc = start_fc;
      for (std::uint16_t k = 0; k < bucket; ++k) {
        const auto idx = by_len[begin + k];
        out[pos_fc[fcs[idx]]++] = idx;
      }
    }
    return out;
  }

  static constexpr auto sorted_indices = build_sorted_indices();

  // length_starts[L] = first pos with length >= L
  // exact range for L is [length_starts[L], length_starts[L+1])
  static constexpr std::array<std::uint16_t, 257> build_length_starts() {
    std::array<std::uint16_t, 257> starts{};
    std::size_t pos = 0;
    for (std::size_t L = 0; L <= 256; ++L) {
      while (pos < N) {
        const auto i = sorted_indices[pos];
        if (static_cast<std::size_t>(lens[i]) < L) ++pos; else break;
      }
      starts[L] = static_cast<std::uint16_t>(pos);
    }
    return starts;
  }

  static constexpr auto length_starts = build_length_starts();

  static constexpr unsigned char first_char_at_sorted_pos(std::size_t sorted_pos) {
    const auto i = sorted_indices[sorted_pos];
    return fcs[i];
  }

  // Precomputed per-name hashes aligned to sorted order (used only for large buckets)
  static constexpr std::array<uint32_t, N> build_hashes_sorted() {
    std::array<uint32_t, N> hs{};
    for (std::size_t pos = 0; pos < N; ++pos) {
      const auto i = sorted_indices[pos];
      hs[pos] = fnv1a32_sv(names_generator<E>[i]);
    }
    return hs;
  }

  static constexpr auto hashes_sorted = build_hashes_sorted();
};

template<typename E>
constexpr std::size_t lower_bound_fc(std::size_t lo, std::size_t hi, unsigned char target) {
  while (lo < hi) {
    const std::size_t mid = (lo + hi) / 2;
    if (len_first_index<E>::first_char_at_sorted_pos(mid) < target) lo = mid + 1; else hi = mid;
  }
  return lo;
}

template<typename E>
constexpr std::size_t upper_bound_fc(std::size_t lo, std::size_t hi, unsigned char target) {
  while (lo < hi) {
    const std::size_t mid = (lo + hi) / 2;
    if (len_first_index<E>::first_char_at_sorted_pos(mid) <= target) lo = mid + 1; else hi = mid;
  }
  return lo;
}

// Tiny unrolled path
template<typename E>
[[nodiscard]] constexpr optional<E> cast_unrolled(string_view name) noexcept {
  if constexpr (count<E> >= 1) { if (names_generator<E>[0] == name) return optional<E>(values_generator<E>[0]); }
  if constexpr (count<E> >= 2) { if (names_generator<E>[1] == name) return optional<E>(values_generator<E>[1]); }
  if constexpr (count<E> >= 3) { if (names_generator<E>[2] == name) return optional<E>(values_generator<E>[2]); }
  return optional<E>();
}

// Main fast path for medium/large enums
template<typename E>
[[nodiscard]] constexpr optional<E> cast_len_first(string_view name) noexcept {
  constexpr std::size_t N = count<E>;
  if constexpr (N == 0) return optional<E>();
  const std::size_t len = name.size();
  if (len > 255) return optional<E>();

  const auto &starts = len_first_index<E>::length_starts;
  const std::size_t begin = starts[len];
  const std::size_t end   = starts[len + 1];
  if (begin == end) return optional<E>();

  const unsigned char fc = (len ? static_cast<unsigned char>(name[0]) : 0);
  const std::size_t first = lower_bound_fc<E>(begin, end, fc);
  const std::size_t last  = upper_bound_fc<E>(begin, end, fc);
  if (first == last) return optional<E>();

  const std::size_t bucket = last - first;

  if (bucket <= ENCHANTUM_ENUM_BUCKET_HASH_THRESHOLD) {
    for (std::size_t pos = first; pos < last; ++pos) {
      const auto idx = len_first_index<E>::sorted_indices[pos];
      if (names_generator<E>[idx] == name)
        return optional<E>(values_generator<E>[idx]);
    }
    return optional<E>();
  }

  const uint32_t h = fnv1a32_sv(name);
  for (std::size_t pos = first; pos < last; ++pos) {
    if (len_first_index<E>::hashes_sorted[pos] == h) {
      const auto idx = len_first_index<E>::sorted_indices[pos];
      if (names_generator<E>[idx] == name)
        return optional<E>(values_generator<E>[idx]);
    }
  }
  return optional<E>();
}

template<typename E>
[[nodiscard]] constexpr bool contains_len_first(string_view name) noexcept {
  return cast_len_first<E>(name).has_value();
}

} // namespace details


template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
[[nodiscard]] constexpr bool contains(const std::underlying_type_t<E> value) noexcept
{
  using T = std::underlying_type_t<E>;

  if (value < T(min<E>) || value > T(max<E>))
    return false;

  if constexpr (is_contiguous_bitflag<E>) {
    if constexpr (has_zero_flag<E>)
      if (value == 0)
        return true;
    const auto u = static_cast<std::make_unsigned_t<T>>(value);

    // std::has_single_bit
    return u != 0 && (u & (u - 1)) == 0;
  }
  else if constexpr (is_contiguous<E>) {
    return true;
  }
  else {
    for (const auto v : values_generator<E>)
      if (static_cast<T>(v) == value)
        return true;
    return false;
  }
}

template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
[[nodiscard]] constexpr bool contains(const E value) noexcept
{
  return enchantum::contains<E>(static_cast<std::underlying_type_t<E>>(value));
}

template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
[[nodiscard]] constexpr bool contains(const string_view name) noexcept
{
#if ENCHANTUM_ENUM_DISPATCH_POLICY == ENCHANTUM_ENUM_POLICY_NAIVE
  constexpr auto minmax = details::minmax_string_size(names<E>.data(), names<E>.data() + names<E>.size());
  if (const auto size = name.size(); size < minmax.first || size > minmax.second)
    return false;

  for (const auto s : names_generator<E>)
    if (s == name)
      return true;
  return false;
#elif ENCHANTUM_ENUM_DISPATCH_POLICY == ENCHANTUM_ENUM_POLICY_LEN_FIRST
  return details::contains_len_first<E>(name);
#else // AUTO
  if constexpr (count<E> <= ENCHANTUM_ENUM_TINY_THRESHOLD) {
    if constexpr (count<E> >= 1) { if (names_generator<E>[0] == name) return true; }
    if constexpr (count<E> >= 2) { if (names_generator<E>[1] == name) return true; }
    if constexpr (count<E> >= 3) { if (names_generator<E>[2] == name) return true; }
    return false;
  } else if constexpr (count<E> <= ENCHANTUM_ENUM_SMALL_THRESHOLD) {
    constexpr auto minmax = details::minmax_string_size(names<E>.data(), names<E>.data() + names<E>.size());
    if (const auto size = name.size(); size < minmax.first || size > minmax.second)
      return false;

    for (const auto s : names_generator<E>)
      if (s == name)
        return true;
    return false;
  } else {
    return details::contains_len_first<E>(name);
  }
#endif
}


template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E), typename BinaryPred>
[[nodiscard]] constexpr bool contains(const string_view name, const BinaryPred binary_pred) noexcept
{
  for (const auto s : names_generator<E>)
    if (details::call_predicate(binary_pred, name, s))
      return true;
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
          return has_zero + details::countr_zero(static_cast<U>(e)) -
            details::countr_zero(static_cast<U>(values_generator<E>[has_zero]));
        }
      }
      else {
        for (std::size_t i = 0; i < count<E>; ++i) {
          if (values_generator<E>[i] == e)
            return optional<std::size_t>(i);
        }
      }
      return optional<std::size_t>();
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
#if ENCHANTUM_ENUM_DISPATCH_POLICY == ENCHANTUM_ENUM_POLICY_NAIVE
      constexpr auto minmax = details::minmax_string_size(names<E>.data(), names<E>.data() + names<E>.size());
      const auto sz = name.size();
      if (sz < minmax.first || sz > minmax.second) return optional<E>();
      for (std::size_t i = 0; i < count<E>; ++i)
        if (names_generator<E>[i] == name)
          return optional<E>(values_generator<E>[i]);
      return optional<E>();
#elif ENCHANTUM_ENUM_DISPATCH_POLICY == ENCHANTUM_ENUM_POLICY_LEN_FIRST
      if constexpr (count<E> <= ENCHANTUM_ENUM_TINY_THRESHOLD)
        return details::cast_unrolled<E>(name);
      return details::cast_len_first<E>(name);
#else // AUTO
      if constexpr (count<E> <= ENCHANTUM_ENUM_TINY_THRESHOLD) {
        return details::cast_unrolled<E>(name);
      } else if constexpr (count<E> <= ENCHANTUM_ENUM_SMALL_THRESHOLD) {
        constexpr auto minmax = details::minmax_string_size(names<E>.data(), names<E>.data() + names<E>.size());
        const auto sz = name.size();
        if (sz < minmax.first || sz > minmax.second) return optional<E>();
        for (std::size_t i = 0; i < count<E>; ++i)
          if (names_generator<E>[i] == name)
            return optional<E>(values_generator<E>[i]);
        return optional<E>();
      } else {
        return details::cast_len_first<E>(name);
      }
#endif
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
inline constexpr details::index_to_enum_functor<E> index_to_enum{};

inline constexpr details::enum_to_index_functor enum_to_index{};

template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
inline constexpr details::cast_functor<E> cast{};


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
inline constexpr details::to_string_functor to_string{};


} // namespace enchantum


#if defined(ENCAHNTUM_DETAILS_GCC_MAJOR) && ENCAHNTUM_DETAILS_GCC_MAJOR <= 10
  #pragma GCC diagnostic pop
#endif