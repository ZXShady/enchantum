#pragma once

#include "details/format_util.hpp"
#include "details/string_view.hpp"
#include "enchantum.hpp"
#include <quill/bundled/fmt/format.h>

#ifdef __cpp_concepts
template<enchantum::Enum E>
struct fmtquill::formatter<E>
#else
template<typename E>
struct fmtquill::formatter<E, char, std::enable_if_t<std::is_enum_v<E>>>
#endif
: fmtquill::formatter< ::enchantum::string_view> {
  template<typename FmtContext>
  constexpr auto format(const E e, FmtContext& ctx) const
  {
    return fmtquill::formatter< ::enchantum::string_view>::format(enchantum::details::format(e), ctx);
  }
};
