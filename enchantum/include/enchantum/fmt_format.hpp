#pragma once

#include "enchantum.hpp"
#include <fmt/format.h>
#include <string_view>

template<enchantum::Enum E>
struct fmt::formatter<E> {
  template<typename ParseContext>
  static constexpr auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template<typename FmtContext>
  static auto format(const E e, FmtContext& ctx)
  {
    return fmt::format_to(ctx.out(), "{}", enchantum::to_string(e));
  }
};