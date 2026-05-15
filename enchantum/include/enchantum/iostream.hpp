#pragma once

#include "bitflags.hpp"
#include "details/format_util.hpp"
#include "enchantum.hpp"
#include <iostream>
#include <string>

namespace enchantum {
namespace iostream_operators {
  template<typename E>
  bool extract_enum_from_string(const std::string& s, E& value, std::true_type)
  {
    if (const auto v = enchantum::cast_bitflag<E>(s)) {
      value = *v;
      return true;
    }
    return false;
  }

  template<typename E>
  bool extract_enum_from_string(const std::string& s, E& value, std::false_type)
  {
    if (const auto v = enchantum::cast<E>(s)) {
      value = *v;
      return true;
    }
    return false;
  }

  template<typename Traits, ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
  std::basic_ostream<char, Traits>& operator<<(std::basic_ostream<char, Traits>& os, const E e)
  {
    return os << details::format(e);
  }

  template<typename Traits, ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
  auto operator>>(std::basic_istream<char, Traits>& is, E& value) -> decltype((value = E{}, is))
  // sfinae to check whether value is assignable
  {
    std::basic_string<char, Traits> s;
    is >> s;
    if (!is)
      return is;

    if (!extract_enum_from_string(s, value, std::integral_constant<bool, is_bitflag<E>>{}))
      is.setstate(std::ios_base::failbit);
    return is;
  }
} // namespace iostream_operators
} // namespace enchantum
