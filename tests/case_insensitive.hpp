#pragma once
#include <cstddef>
#include <string_view>

struct CaseInsenitive {

  constexpr bool operator()(const char a, const char b) const
  {
    constexpr auto tolower = [](const char c) { return (c >= 'A' && c <= 'Z') ? c + ('a' - 'A') : c; };
    return tolower(a) == tolower(b);
  }
};

inline constexpr CaseInsenitive case_insensitive;

struct CaseInsenitiveByStrings {
  constexpr bool operator()(const std::string_view a, const std::string_view b) const
  {
    if (a.size() != b.size())
      return false;

    for (std::size_t i = 0; i < a.size(); ++i)
      if (!case_insensitive(a[i], b[i]))
        return false;
    return true;
  }
};

inline constexpr CaseInsenitiveByStrings case_insensitive_by_strings;


struct CaseInsenitiveBoth {
  constexpr bool operator()(char a, char b) const { return case_insensitive(a, b); }
  template<typename String>
  constexpr bool operator()(const String& a, const String& b) const
  {
    static_assert(sizeof(String) == 0, "Not chosen");
    return case_insensitive_by_strings(a, b);
  }
};

inline constexpr CaseInsenitiveBoth case_insensitive_both;
