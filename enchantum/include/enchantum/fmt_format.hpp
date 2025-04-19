#pragma once

#include "enchantum.hpp"
#include <fmt/format.h>
#include <string_view>

template<enchantum::Enum E>
struct fmt::formatter<T> {
  constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.end(); }
  template<typename FormatContext>
  static std::string_view format(const E value, FormatContext& ctx)
  {
    return enchantum::to_string(value);
  }
};