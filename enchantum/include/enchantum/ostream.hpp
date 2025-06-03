#pragma once

#include "details/format_util.hpp"
#include "enchantum.hpp"
#include <iostream>

namespace enchantum::ostream_operators {
template<typename Traits, Enum E>
std::basic_ostream<char, Traits>& operator<<(std::basic_ostream<char, Traits>& os, const E e)
{
  return os << details::format(e);
}
} // namespace enchantum::ostream_operators