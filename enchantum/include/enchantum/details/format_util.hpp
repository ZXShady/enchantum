#pragma once

#include "../bitflags.hpp"
#include "../enchantum.hpp"
#include <string>

namespace enchantum {
namespace details {
  template<typename String>
  std::string string_to_std_string(const String& name, std::true_type)
  {
    return name;
  }

  template<typename String>
  std::string string_to_std_string(const String& name, std::false_type)
  {
    return std::string(name.data(), name.size());
  }

  template<typename E>
  std::string format_impl(E e, std::true_type) noexcept
  {
    const auto name = enchantum::to_string_bitflag(e);
    if (!name.empty())
      return details::string_to_std_string(name, std::integral_constant<bool, std::is_same<std::string, string>::value>{});
    return std::to_string(+enchantum::to_underlying(e)); // promote using + to select int overload if to underlying returns char
  }

  template<typename E>
  std::string format_impl(E e, std::false_type) noexcept
  {
    const auto name = enchantum::to_string(e);
    if (!name.empty())
      return std::string(name.data(), name.size());
    return std::to_string(+enchantum::to_underlying(e)); // promote using + to select int overload if to underlying returns char
  }

  template<typename E>
  std::string format(E e) noexcept
  {
    return format_impl(e, std::integral_constant<bool, is_bitflag<E>>{});
  }
} // namespace details
} // namespace enchantum
