#pragma once

#include <cstddef> // Required for std::size_t
#include <cstdint>
#include <string_view>

namespace enchantum::details {

/**
 * @brief Computes the 32-bit FNV-1a hash of a string at compile time.
 * @param str Pointer to the start of the string.
 * @param len The length of the string.
 * @return The 32-bit FNV-1a hash.
 */
constexpr std::uint32_t fnv1a_32(const char* str, std::size_t len) noexcept
{
  std::uint32_t hash = 0x811c9dc5; // FNV_offset_basis
  for (std::size_t i = 0; i < len; ++i) {
    hash ^= static_cast<std::uint8_t>(str[i]);
    hash *= 0x01000193; // FNV_prime
  }
  return hash;
}

/**
 * @brief Computes the 32-bit FNV-1a hash of a string_view at compile time.
 * @param sv The string_view to hash.
 * @return The 32-bit FNV-1a hash.
 */
constexpr std::uint32_t fnv1a_32(std::string_view sv) noexcept { return fnv1a_32(sv.data(), sv.size()); }

} // namespace enchantum::details
