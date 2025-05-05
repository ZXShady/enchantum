#pragma once

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