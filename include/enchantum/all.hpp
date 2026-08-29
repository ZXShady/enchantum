#pragma once

// IWYU pragma: begin_exports
#include "type_name.hpp" // IWYU pragma: export:
#include "details/optional.hpp" // IWYU pragma: export:
#include "details/string.hpp" // IWYU pragma: export:
#include "details/string_view.hpp" // IWYU pragma: export:
#include "details/shared.hpp" // IWYU pragma: export:
#include "details/format_util.hpp" // IWYU pragma: export:
#include "generators.hpp" // IWYU pragma: export:
#include "algorithms.hpp" // IWYU pragma: export:
#include "array.hpp" // IWYU pragma: export:
#include "bitset.hpp" // IWYU pragma: export:
#include "bitflags.hpp" // IWYU pragma: export:
#include "bitwise_operators.hpp" // IWYU pragma: export:
#include "enchantum.hpp" // IWYU pragma: export:
#include "entries.hpp" // IWYU pragma: export:
#include "iostream.hpp" // IWYU pragma: export:
#include "next_value.hpp" // IWYU pragma: export:

#if __has_include(<fmt/format.h>)
  #include "fmt_format.hpp" // IWYU pragma: export:
#elif (__cplusplus >= 202002 || (defined(_MSVC_LANG) && _MSVC_LANG >= 202002)) && __has_include(<format>)
  #include "std_format.hpp" // IWYU pragma: export:
#endif
// IWYU pragma: end_exports