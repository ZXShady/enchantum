#pragma once
#include "enchantum.hpp"
#include <iosfwd>
#include <string>
#include <concepts>

namespace enchantum::istream_operators {
template<typename Traits, Enum E>
  requires std::assignable_from<E&,E>
std::basic_istream<char, Traits>& operator>>(std::basic_istream<char, Traits>& is, E& value)
{
  std::basic_string<char, Traits> s;
  is >> s;
  if (!is)
    return is;

  if (const auto v = enchantum::cast<E>(s))
    value = *v;
  else
    is.setstate(std::ios_base::failbit);
  return is;
}

} // namespace enchantum::istream_operators
