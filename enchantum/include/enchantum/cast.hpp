#pragma once
#include "enchantum.hpp"
#include <concepts>
#include <string_view>

#ifndef ENCHANTUM_ALIAS_OPTIONAL
  #include <optional>
#endif
namespace enchantum {

#ifdef ENCHANTUM_ALIAS_OPTIONAL
ENCHANTUM_ALIAS_OPTIONAL;
#else
using std::optional;
#endif
} // namespace enchantum


namespace enchantum {

template<Enum E>
constexpr optional<E> cast(std::underlying_type_t<E> e) noexcept
{
  optional<E> a; // rvo not that it really matters
  if (enchantum::contains(E(e)))
    a.emplace(E(e));
  return a;
}

template<Enum E>
constexpr optional<E> cast(std::string_view name) noexcept
{
  optional<E> a; // rvo not that it really matters
  for (const auto& [e, s] : entries<E>) {
    if (s == name) {
      a.emplace(e);
      break;
    }
  }
  return a;
}

template<Enum E, std::regular_invocable<std::string_view, std::string_view> BinaryPred>
constexpr optional<E> cast(std::string_view name, BinaryPred binary_predicate) noexcept
{
  optional<E> a; // rvo not that it really matters
  for (const auto& [e, s] : entries<E>) {
    if (binary_predicate(name, s)) {
      a.emplace(e);
      break;
    }
  }
  return a;
}


} // namespace enchantum