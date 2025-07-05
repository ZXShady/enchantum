#pragma once


#ifdef ENCHANTUM_CONFIG_FILE
  #include ENCHANTUM_CONFIG_FILE
#endif

#ifndef ENCHANTUM_ALIAS_STRING
  #include <string>
#endif


namespace enchantum {
#ifdef ENCHANTUM_ALIAS_STRING
ENCHANTUM_ALIAS_STRING;
#else
using ::std::string;
#endif

} // namespace enchantum

#include "string_view.hpp" // For enchantum::string_view

namespace enchantum::details {

// Helper to append a character to enchantum::string
inline void append_char(enchantum::string& str, char ch) {
    str += ch;
}

// Helper to append a string_view to enchantum::string
inline void append_sv(enchantum::string& str, enchantum::string_view sv) {
    str.append(sv.data(), sv.size());
}

} // namespace enchantum::details