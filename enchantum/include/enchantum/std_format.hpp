#pragma once

#include "enchantum.hpp"
#include <string_view>
#include <format>

template<enchantum::Enum E>
struct std::formatter<E> {
  template<typename ParseContext>
  constexpr auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template<typename FmtContext>
  typename auto format(E e, FmtContext& ctx) const
  {
    return std::format_to(ctx.out(),"{}", enchantum::to_string(e));
  }
};