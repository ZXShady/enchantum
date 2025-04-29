#pragma once

#ifndef ENCHANTUM_ALIAS_STRING_VIEW
  #include <string_view>
#endif


namespace enchantum {
#ifdef ENCHANTUM_ALIAS_STRING_VIEW
ENCHANTUM_ALIAS_STRING_VIEW;
#else
using ::std::basic_string_view;
using ::std::string_view;
#endif

} // namespace enchantum