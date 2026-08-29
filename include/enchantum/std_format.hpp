#pragma once

#include <format>
#include "details/string_view.hpp"
#include "details/format_util.hpp"
#include "common.hpp"

template<enchantum::Enum E>
struct std::formatter<E> : std::formatter< ::enchantum::string_view> {
  template<typename FmtContext>
  constexpr auto format(const E e, FmtContext& ctx) const
  {
    return std::formatter< ::enchantum::string_view>::format(enchantum::details::format(e), ctx);
  }
};