#pragma once
#include "common.hpp"
#include "details/string.hpp" // Already included, good.
#include "details/string_view.hpp"
#include "enchantum.hpp" // Provides 'names', 'values', 'count', 'cast' etc.
// #include "generators.hpp" // Should not be needed anymore by this file.

namespace enchantum {

template<BitFlagEnum E>
inline constexpr E value_ors = [] {
  using T = std::underlying_type_t<E>;
  T ret{};
  for (const auto val : enchantum::values<E>) // Use enchantum::values
    ret |= static_cast<T>(val);
  return static_cast<E>(ret);
}();


template<BitFlagEnum E>
[[nodiscard]] constexpr bool contains_bitflag(const std::underlying_type_t<E> value) noexcept
{
  using T = std::underlying_type_t<E>;
  if constexpr (is_contiguous_bitflag<E>) {
    return value >= static_cast<T>(min<E>) && value <= static_cast<T>(value_ors<E>);
  }
  else {
    if (value == 0)
      return has_zero_flag<E>;
    T valid_bits = 0;

    // Iterate using enchantum::values<E>
    constexpr auto&       enum_values = enchantum::values<E>;
    constexpr std::size_t num_entries = enchantum::count<E>;
    for (auto i = std::size_t{has_zero_flag<E>}; i < num_entries; ++i) {
      const auto v = static_cast<T>(enum_values[i]);
      if ((value & v) == v) // Check if the bit 'v' is set in 'value'
        valid_bits |= v;    // Accumulate the valid bits found in 'value'
    }
    // Ensure all bits in 'value' were valid enum flags
    return valid_bits == value;
  }
}

template<BitFlagEnum E>
[[nodiscard]] constexpr bool contains_bitflag(const E value) noexcept
{
  return enchantum::contains_bitflag<E>(static_cast<std::underlying_type_t<E>>(value));
}

template<BitFlagEnum E, std::predicate<string_view, string_view> BinaryPred>
[[nodiscard]] constexpr bool contains_bitflag(const string_view s, const char sep, const BinaryPred binary_pred) noexcept
{
  std::size_t pos = 0;
  for (std::size_t i = s.find(sep); i != s.npos; i = s.find(sep, pos)) {
    if (!enchantum::contains<E>(s.substr(pos, i - pos), binary_pred))
      return false;
    pos = i + 1;
  }
  return enchantum::contains<E>(s.substr(pos), binary_pred);
}


template<BitFlagEnum E>
[[nodiscard]] constexpr bool contains_bitflag(const string_view s, const char sep = '|') noexcept
{
  std::size_t pos = 0;
  for (std::size_t i = s.find(sep); i != s.npos; i = s.find(sep, pos)) {
    if (!enchantum::contains<E>(s.substr(pos, i - pos)))
      return false;
    pos = i + 1;
  }
  return enchantum::contains<E>(s.substr(pos));
}


template<typename String = string, BitFlagEnum E>
[[nodiscard]] constexpr String to_string_bitflag(const E value, const char sep = '|')
{
  using T                            = std::underlying_type_t<E>;
  constexpr auto&       local_names  = enchantum::names<E, string_view>; // Use string_view for direct access
  constexpr auto&       local_values = enchantum::values<E>;
  constexpr std::size_t num_entries  = enchantum::count<E>;

  if constexpr (has_zero_flag<E>) {
    if (static_cast<T>(value) == 0) {
      // Find the name of the zero value
      for (size_t i = 0; i < num_entries; ++i) {
        if (static_cast<T>(local_values[i]) == 0) {
          return String(local_names[i].data(), local_names[i].size());
        }
      }
      // Fallback if has_zero_flag is true but no explicit 0-valued member found (should be rare)
      if constexpr (std::is_constructible_v<String, const char*>)
        return String("0");
      return String();
    }
  }
  else {
    if (static_cast<T>(value) == 0) { // No zero flag, and value is 0
      if constexpr (std::is_constructible_v<String, const char*>)
        return String("0"); // Or String() for empty
      return String();
    }
  }

  String result_name_str; // Use a temporary std::string for accumulation
  T      accumulated_value = 0;
  bool   first             = true;

  for (std::size_t i = 0; i < num_entries; ++i) {
    const T current_flag_value = static_cast<T>(local_values[i]);
    if (current_flag_value == 0 && has_zero_flag<E>)
      continue; // Skip zero value in combinations

    if ((static_cast<T>(value) & current_flag_value) == current_flag_value) {
      if (!first) {
        details::append_char(result_name_str, sep);
      }
      const auto s_view = local_names[i];
      details::append_sv(result_name_str, s_view);
      accumulated_value |= current_flag_value;
      first = false;
    }
  }

  if (accumulated_value == static_cast<T>(value)) {
    if constexpr (std::is_same_v<String, std::string> || std::is_same_v<String, enchantum::string>)
      return result_name_str;
    else
      return String(result_name_str.data(), result_name_str.length());
  }

  // Value is not a perfect combination of known flags or is non-zero and has_zero_flag is false but value is 0 (covered above)
  return String();
}

template<BitFlagEnum E, std::predicate<string_view, string_view> BinaryPred>
[[nodiscard]] constexpr optional<E> cast_bitflag(const string_view s, const char sep, const BinaryPred binary_pred) noexcept
{
  using T = std::underlying_type_t<E>;
  T           check_value{};
  std::size_t pos = 0;
  for (std::size_t i = s.find(sep); i != s.npos; i = s.find(sep, pos)) {
    if (const auto v = enchantum::cast<E>(s.substr(pos, i - pos), binary_pred))
      check_value |= static_cast<T>(*v);
    else
      return optional<E>();
    pos = i + 1;
  }

  if (const auto v = enchantum::cast<E>(s.substr(pos), binary_pred))
    return optional<E>(static_cast<E>(check_value | static_cast<T>(*v)));
  return optional<E>();
}

template<BitFlagEnum E>
[[nodiscard]] constexpr optional<E> cast_bitflag(const string_view s, const char sep = '|') noexcept
{
  return enchantum::cast_bitflag<E>(s, sep, [](const auto& a, const auto& b) { return a == b; });
}

template<BitFlagEnum E>
[[nodiscard]] constexpr optional<E> cast_bitflag(const std::underlying_type_t<E> value) noexcept
{
  return enchantum::contains_bitflag<E>(value) ? optional<E>(static_cast<E>(value)) : optional<E>();
}

} // namespace enchantum