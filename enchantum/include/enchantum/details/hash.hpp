#pragma once
#include "string_view.hpp" // Located in the same 'details' directory
#include <cstdint>
#include <cstddef> // For std::size_t

namespace enchantum::details {

// FNV-1a 32-bit hash function
[[nodiscard]] constexpr std::uint32_t fnv1a_32(const char* str, std::size_t len) noexcept {
    std::uint32_t hash = 0x811c9dc5; // FNV_offset_basis
    for (std::size_t i = 0; i < len; ++i) {
        hash ^= static_cast<std::uint8_t>(str[i]);
        hash *= 0x01000193; // FNV_prime
    }
    return hash;
}

[[nodiscard]] constexpr std::uint32_t fnv1a_32(::enchantum::string_view sv) noexcept {
    return fnv1a_32(sv.data(), sv.size());
}

} // namespace enchantum::details
