#pragma once

#include "../bitflags.hpp"
#include "../enchantum.hpp"
#include <string>

namespace enchantum {
namespace details {
  template<typename E>
  std::string format(E e) noexcept
  {
    if constexpr (is_bitflag<E>) {
      if (const auto name = enchantum::to_string_bitflag(e); !name.empty()) {
        if constexpr (std::is_same_v<std::string, string>) {
          return name;
        }
        else {
          return std::string(name.data(), name.size());
        }
      }
    }
    else {
      if (const auto name = enchantum::to_string(e); !name.empty())
        return std::string(name.data(), name.size());
    }
    return std::to_string(+enchantum::to_underlying(e)); // promote using + to select int overload if to underlying returns char
  }
} // namespace details
} // namespace enchantum