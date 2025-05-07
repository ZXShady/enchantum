#pragma once

#include "enchantum.hpp"
#include <string_view>
#include <format>

template<enchantum::Enum E>
struct std::formatter<E> {
  template<typename ParseContext>
  static constexpr auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template<typename FmtContext>
  static auto format(const E e, FmtContext& ctx)
  {
    return std::format_to(ctx.out(),"{}", enchantum::to_string(e));
  }
};