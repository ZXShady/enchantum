#pragma once

#include "bitflags.hpp"
#include "enchantum.hpp"

#include <format>
#include <string_view>

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
    if constexpr (enchantum::BitFlagEnum<E>)
      return std::format_to(ctx.out(), "{}", enchantum::to_string_bitflag(e));
    else
      return std::format_to(ctx.out(), "{}", enchantum::to_string(e));
  }
};