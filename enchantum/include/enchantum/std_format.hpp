#pragma once

#include "enchantum.hpp"
#include <string_view>
#include <format>

template<enchantum::Enum E>
struct std::formatter<E> {
  constexpr auto parse(std::format_parse_context& ctx) { return ctx.end(); }
  template<typename FormatContext>
  static std::string_view format(const E value, FormatContext& ctx)
  {
    return enchantum::to_string(value);
  }
};