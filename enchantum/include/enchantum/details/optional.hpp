#pragma once

#include "compat.hpp"

#ifdef ENCHANTUM_CONFIG_FILE
  #include ENCHANTUM_CONFIG_FILE
#endif

#if !defined(ENCHANTUM_ALIAS_OPTIONAL) && ENCHANTUM_DETAILS_CXX_STD < 201703L
  #error "C++14 users must define ENCHANTUM_ALIAS_OPTIONAL to an optional-compatible template alias."
#endif

#ifndef ENCHANTUM_ALIAS_OPTIONAL
  #include <optional>
#endif

namespace enchantum {
#ifdef ENCHANTUM_ALIAS_OPTIONAL
ENCHANTUM_ALIAS_OPTIONAL;
#else
using ::std::optional;
#endif

} // namespace enchantum
