#pragma once

#include "../common.hpp"         // For Enum concept
#include "../details/optional.hpp" // For enchantum::optional
#include "../entries.hpp"        // For enchantum::names, enchantum::values, enchantum::count
#include "string_view.hpp"       // For enchantum::string_view (located in this 'details' directory)
#include "hash.hpp"              // For fnv1a_32

#include <cstddef> // For std::size_t
#include <utility> // For std::index_sequence, std::get

namespace enchantum::details {

// Compile-time string to enum conversion using C++17 fold expression
template <typename E, std::size_t... I>
[[nodiscard]] constexpr auto string_to_enum_impl(::enchantum::string_view name, std::index_sequence<I...>) noexcept -> ::enchantum::optional<E> {
    static_assert(::enchantum::Enum<E>, "Type E must be an enumeration.");

    ::enchantum::optional<E> result{}; // Initialize to empty

    if constexpr (sizeof...(I) == 0) { // Handle empty enum
        return result;
    }

    const auto input_hash = ::enchantum::details::fnv1a_32(name);

    // Use a fold expression with logical OR to find the first match.
    // The structure (condition && (side_effect_producing_true_for_found_state)) || ...
    // allows short-circuiting once a match is found.
    // The fold expression itself will perform the assignment to 'result' upon finding a match.
    // The boolean result of the fold isn't strictly needed here since we check 'result'.
    (void)( // Cast to void to suppress potential unused result warning for the fold itself
        (
            // Check 1: Hash matches for the current enum name from the pack
            (input_hash == ::enchantum::details::fnv1a_32(std::get<I>(::enchantum::names<E>)) &&
             // Check 2: Full string comparison to handle collisions
             name == std::get<I>(::enchantum::names<E>)) &&
            // If both checks pass, assign the enum value to result and yield true to stop the fold
            (result = std::get<I>(::enchantum::values<E>), true) // Assign and make this part true
        ) || ... // Logical OR: if current check is false, proceed to the next in the pack
    );
    // If no match was found after checking all items, 'result' remains empty.

    return result;
}

} // namespace enchantum::details
