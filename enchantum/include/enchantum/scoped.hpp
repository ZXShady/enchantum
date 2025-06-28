#pragma once

#include "bitflags.hpp"
#include "details/string.hpp"
#include "details/string_view.hpp"
#include "enchantum.hpp"
#include "type_name.hpp"

namespace enchantum::scoped {
namespace details {


  constexpr string_view remove_scope_or_empty(string_view string, const string_view type_name) noexcept
  {
    if (!string.starts_with(type_name))
      return string_view();
    string.remove_prefix(type_name.size());
    if (!string.starts_with("::"))
      return string_view();
    string.remove_prefix(2);
    return string;
  }
} // namespace details

template<Enum E>
[[nodiscard]] constexpr bool contains(const string_view name) noexcept
{
  const auto n = details::remove_scope_or_empty(name, type_name<E>);
  return !n.empty() && enchantum::contains<E>(n);
}

template<Enum E, std::predicate<string_view, string_view> BinaryPredicate>
[[nodiscard]] constexpr bool contains(const string_view name, const BinaryPredicate binary_predicate) noexcept
{
  const auto n = details::remove_scope_or_empty(name, type_name<E>);
  return !n.empty() && enchantum::contains<E>(n, binary_predicate);
}

namespace details {
  template<Enum E>
  struct scoped_cast_functor {
    [[nodiscard]] constexpr optional<E> operator()(const string_view name) const noexcept
    {
      const auto n = details::remove_scope_or_empty(name, type_name<E>);
      return n.empty() ? optional<E>() : cast<E>(n);
    }

    template<std::predicate<string_view, string_view> BinaryPred>
    [[nodiscard]] constexpr optional<E> operator()(const string_view name, const BinaryPred binary_predicate) const noexcept
    {
      const auto n = details::remove_scope_or_empty(name, type_name<E>);
      return n.empty() ? optional<E>() : cast<E>(n, binary_predicate);
    }
  };

  struct to_scoped_string_functor {
    template<Enum E>
    [[nodiscard]] constexpr string operator()(const E value) const noexcept
    {
      string s;
      if (const auto i = enchantum::enum_to_index(value)) {
        s += type_name<E>;
        s += "::";
        s += names_generator<E>[*i];
        return s;
      }
      return s;
    }
  };
} // namespace details


inline constexpr details::to_scoped_string_functor to_string;

template<Enum E>
inline constexpr details::scoped_cast_functor<E> cast;

template<BitFlagEnum E, std::predicate<string_view, string_view> BinaryPred>
[[nodiscard]] constexpr bool contains_bitflag(const string_view s, const char sep, const BinaryPred binary_pred) noexcept
{
  std::size_t pos = 0;
  for (std::size_t i = s.find(sep); i != s.npos; i = s.find(sep, pos)) {
    if (!enchantum::scoped::contains<E>(s.substr(pos, i - pos), binary_pred))
      return false;
    pos = i + 1;
  }
  return enchantum::scoped::contains<E>(s.substr(pos), binary_pred);
}

template<BitFlagEnum E>
[[nodiscard]] constexpr bool contains_bitflag(const string_view s, const char sep = '|') noexcept
{
  std::size_t pos = 0;
  for (std::size_t i = s.find(sep); i != s.npos; i = s.find(sep, pos)) {
    if (!enchantum::scoped::contains<E>(s.substr(pos, i - pos)))
      return false;
    pos = i + 1;
  }
  return enchantum::scoped::contains<E>(s.substr(pos));
}


template<BitFlagEnum E>
[[nodiscard]] constexpr string to_string_bitflag(const E value, const char sep = '|')
{
  using T = std::underlying_type_t<E>;
  if constexpr (has_zero_flag<E>)
    if (static_cast<T>(value) == 0)
      return enchantum::scoped::to_string(value);

  string         name;
  T              check_value = 0;
  constexpr auto scope_name  = type_name<E>;
  for (auto i = std::size_t{has_zero_flag<E>}; i < count<E>; ++i) {
    const auto v = static_cast<T>(values_generator<E>[i]);
    if (v == (static_cast<T>(value) & v)) {
      if (!name.empty())
        name.append(1, sep); // append separator if not the first value
      name.append(scope_name.data(), scope_name.size());
      name.append("::", 2);
      const auto s = names_generator<E>[i];
      name.append(s.data(), s.size()); // not using operator += since this may not be std::string_view always
      check_value |= v;
    }
  }
  if (check_value == static_cast<T>(value))
    return name;
  return string();
}


template<BitFlagEnum E, std::predicate<string_view, string_view> BinaryPred>
[[nodiscard]] constexpr optional<E> cast_bitflag(const string_view s, const char sep, const BinaryPred binary_pred) noexcept
{
  using T = std::underlying_type_t<E>;
  T           check_value{};
  std::size_t pos = 0;
  for (std::size_t i = s.find(sep); i != s.npos; i = s.find(sep, pos)) {
    if (const auto v = enchantum::scoped::cast<E>(s.substr(pos, i - pos), binary_pred))
      check_value |= static_cast<T>(*v);
    else
      return optional<E>();
    pos = i + 1;
  }

  if (const auto v = enchantum::scoped::cast<E>(s.substr(pos), binary_pred))
    return optional<E>(static_cast<E>(check_value | static_cast<T>(*v)));
  return optional<E>();
}

template<BitFlagEnum E>
[[nodiscard]] constexpr optional<E> cast_bitflag(const string_view s, const char sep = '|') noexcept
{
  return enchantum::scoped::cast_bitflag<E>(s, sep, [](const auto& a, const auto& b) { return a == b; });
}

} // namespace enchantum::scoped
