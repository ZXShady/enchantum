#pragma once

#include "compat.hpp"

#ifdef ENCHANTUM_CONFIG_FILE
  #include ENCHANTUM_CONFIG_FILE
#endif

#if !defined(ENCHANTUM_ALIAS_STRING_VIEW) && ENCHANTUM_DETAILS_CXX_STD < 201703L
  #error "C++14 users must define ENCHANTUM_ALIAS_STRING_VIEW to a string_view-compatible alias."
#endif

#ifndef ENCHANTUM_ALIAS_STRING_VIEW
  #include <string_view>
#endif

namespace enchantum {
#ifdef ENCHANTUM_ALIAS_STRING_VIEW
ENCHANTUM_ALIAS_STRING_VIEW;
#else
using ::std::string_view;
#endif

} // namespace enchantum
