#pragma once

#include "enchantum.hpp"
#include <fmt/format.h>
#include <string_view>

template<enchantum::Enum E>
struct fmt::formatter<E> {
  template<typename ParseContext>
  constexpr auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template<typename FmtContext>
  typename auto format(E e, FmtContext& ctx) const
  {
    return fmt::format_to(ctx.out(), "{}", enchantum::to_string(e));
  }
};