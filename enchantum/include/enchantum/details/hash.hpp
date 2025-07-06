// In enchantum/include/enchantum/details/hash.hpp
#pragma once
#include "string_view.hpp" // Use the library's own string_view (corrected path)
#include <cstdint>

namespace enchantum::details {
constexpr std::uint32_t fnv1a_32(const char* str, std::size_t len) noexcept
{
  std::uint32_t hash = 0x811c9dc5; // FNV_offset_basis
  for (std::size_t i = 0; i < len; ++i) {
    hash ^= static_cast<std::uint8_t>(str[i]);
    hash *= 0x01000193; // FNV_prime
  }
  return hash;
}

constexpr std::uint32_t fnv1a_32(string_view sv) noexcept { return fnv1a_32(sv.data(), sv.size()); }
} // namespace enchantum::details
