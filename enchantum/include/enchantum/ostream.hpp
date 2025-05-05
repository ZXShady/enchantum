#pragma once

#include "enchantum.hpp"
#include <iosfwd>
#include <string_view>

namespace enchantum::ostream_operators {
template<typename Traits, Enum E>
std::basic_ostream<char, Traits>& operator<<(std::basic_ostream<char, Traits>& os, E value)
{
  return os << enchantum::to_string(value);
}
} // namespace enchantum::ostream_operators