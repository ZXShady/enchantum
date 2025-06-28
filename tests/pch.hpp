// Path: tests/pch.hpp

#pragma once

/**
 * @file pch.hpp
 * @brief Precompiled header for the enchantum test suite.
 *
 * This file includes large and stable third-party headers (Catch2, {fmt})
 * and core standard library components that are used across the majority of
 * test files. Using this as a precompiled header significantly accelerates
 * the compilation of the 'tests' target by avoiding redundant parsing.
 */

// Third-party dependencies that are large and slow to parse
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>
// Define FMT_UNICODE before including fmt/format.h to avoid redefinition errors.
#define FMT_UNICODE 0
// #include <fmt/format.h> // Removed as per instructions for compiled fmt
// #include <fmt/ranges.h> // Removed as per instructions for compiled fmt

// Frequently used standard library components
#include <string>
#include <string_view>
#include <vector>
#include <array>
#include <algorithm>
#include <sstream>
#include <optional>
#include <utility>

// Project's own test utilities, which includes enchantum itself
#include "test_utility.hpp"
