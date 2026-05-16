#pragma once
#include <cstddef>
#include <enchantum/details/string_view.hpp>

struct CaseInsenitive {
  static constexpr char tolower(const char c) { return (c >= 'A' && c <= 'Z') ? static_cast<char>(c + ('a' - 'A')) : c; }

  constexpr bool operator()(const char a, const char b) const
  {
    return tolower(a) == tolower(b);
  }
};

constexpr CaseInsenitive case_insensitive{};

struct CaseInsenitiveByStrings {
  constexpr bool operator()(const enchantum::string_view a, const enchantum::string_view b) const
  {
    if (a.size() != b.size())
      return false;

    for (std::size_t i = 0; i < a.size(); ++i)
      if (!case_insensitive(a[i], b[i]))
        return false;
    return true;
  }
};

constexpr CaseInsenitiveByStrings case_insensitive_by_strings{};


struct CaseInsenitiveBoth {
  constexpr bool operator()(char a, char b) const { return case_insensitive(a, b); }
  template<typename String>
  constexpr bool operator()(const String& a, const String& b) const
  {
    static_assert(sizeof(String) == 0, "Not chosen");
    return case_insensitive_by_strings(a, b);
  }
};

constexpr CaseInsenitiveBoth case_insensitive_both{};
