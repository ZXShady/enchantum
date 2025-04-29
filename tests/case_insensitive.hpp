#pragma once
#include <cstddef>
struct CaseInsenitive {
  template<typename String>
  constexpr bool operator()(const String& lhs, const String& rhs) const
  {
    if (lhs.size() != rhs.size())
      return false;

    constexpr auto tolower = [](const char c) { return (c >= 'A' && c <= 'Z') ? c + ('a' - 'A') : c; };
    for (std::size_t i = 0; i < lhs.size(); ++i)
      if (tolower(lhs[i]) != tolower(rhs[i]))
        return false;
    return true;
  }
};

inline constexpr CaseInsenitive case_insensitive;
