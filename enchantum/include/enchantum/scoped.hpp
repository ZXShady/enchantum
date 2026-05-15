#pragma once

#include "bitflags.hpp"
#include "common.hpp"
#include "details/string.hpp"
#include "details/string_view.hpp"
#include "enchantum.hpp"
#include "type_name.hpp"


#if defined(ENCHANTUM_DETAILS_GCC_MAJOR) && ENCHANTUM_DETAILS_GCC_MAJOR <= 10
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wconversion"
  #pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif

namespace enchantum {
namespace scoped {
  namespace details {


    constexpr string_view remove_scope_or_empty(string_view string, const string_view type_name) noexcept
    {
      if (string.substr(0, type_name.size()) != type_name)
        return string_view();
      string.remove_prefix(type_name.size());
      if (string.substr(0, 2) != string_view("::", 2))
        return string_view();
      string.remove_prefix(2);
      return string;
    }
  } // namespace details

  template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
  [[nodiscard]] constexpr bool contains(const string_view name) noexcept
  {
    const auto n = details::remove_scope_or_empty(name, type_name<E>);
    return !n.empty() && enchantum::contains<E>(n);
  }

  template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E), typename BinaryPredicate>
  [[nodiscard]] constexpr bool contains(const string_view name, const BinaryPredicate binary_predicate) noexcept
  {
    const auto n = details::remove_scope_or_empty(name, type_name<E>);
    return !n.empty() && enchantum::contains<E>(n, binary_predicate);
  }

  namespace details {
    template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
    struct scoped_cast_functor {
      [[nodiscard]] constexpr optional<E> operator()(const string_view name) const noexcept
      {
        const auto n = details::remove_scope_or_empty(name, type_name<E>);
        return n.empty() ? optional<E>() : enchantum::cast<E>(n);
      }

      template<typename BinaryPred>
      [[nodiscard]] constexpr optional<E> operator()(const string_view name, const BinaryPred binary_predicate) const noexcept
      {
        const auto n = details::remove_scope_or_empty(name, type_name<E>);
        return n.empty() ? optional<E>() : enchantum::cast<E>(n, binary_predicate);
      }
    };

    struct to_scoped_string_functor {
      // hacky workaround about string not being a literal type.
      template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E), typename String = string>
      [[nodiscard]] constexpr String operator()(const E value) const noexcept
      {
        String s;
        if (const auto i = enchantum::enum_to_index(value)) {
          const auto scope_name = type_name<E>;
          s.append(scope_name.data(), scope_name.size());
          s.append("::", 2);
          const auto name = names_generator<E>[*i];
          s.append(name.data(), name.size());
          return s;
        }
        return s;
      }
    };
  } // namespace details


  ENCHANTUM_DETAILS_INLINE_VAR constexpr details::to_scoped_string_functor to_string;

  template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
  ENCHANTUM_DETAILS_INLINE_VAR constexpr details::scoped_cast_functor<E> cast;

  template<ENCHANTUM_DETAILS_ENUM_BITFLAG_CONCEPT(E), typename BinaryPred>
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

  template<ENCHANTUM_DETAILS_ENUM_BITFLAG_CONCEPT(E)>
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


  template<typename String = string, ENCHANTUM_DETAILS_ENUM_BITFLAG_CONCEPT(E)>
  [[nodiscard]] constexpr String to_string_bitflag(const E value, const char sep = '|')
  {
    using T = std::underlying_type_t<E>;
    if (has_zero_flag<E>)
      if (static_cast<T>(value) == 0)
        return enchantum::scoped::to_string(value);

    String         name;
    T              check_value = 0;
    constexpr auto scope_name  = type_name<E>;
    for (auto i = static_cast<std::size_t>(has_zero_flag<E>); i < count<E>; ++i) {
      const auto v = static_cast<T>(values_generator<E>[i]);
      if (v == (static_cast<T>(value) & v)) {
        if (!name.empty())
          name.append(1, sep); // append separator if not the first value
        name.append(scope_name.data(), scope_name.size());
        name.append("::", 2);
        const auto s = names_generator<E>[i];
        name.append(s.data(), s.size());
        check_value |= v;
      }
    }
    if (check_value == static_cast<T>(value))
      return name;
    return string();
  }


  template<ENCHANTUM_DETAILS_ENUM_BITFLAG_CONCEPT(E), typename BinaryPred>
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

  template<ENCHANTUM_DETAILS_ENUM_BITFLAG_CONCEPT(E)>
  [[nodiscard]] constexpr optional<E> cast_bitflag(const string_view s, const char sep = '|') noexcept
  {
    return enchantum::scoped::cast_bitflag<E>(s, sep, enchantum::details::equal_to_string_view{});
  }
} // namespace scoped
} // namespace enchantum

#if defined(ENCHANTUM_DETAILS_GCC_MAJOR) && ENCHANTUM_DETAILS_GCC_MAJOR <= 10
  #pragma GCC diagnostic pop
#endif
