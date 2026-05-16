#pragma once

#include <cstddef>
#include <string>

#if defined(_MSVC_LANG)
  #define TESTS_CPP14_CXX_STD _MSVC_LANG
#else
  #define TESTS_CPP14_CXX_STD __cplusplus
#endif

namespace tests_cpp14 {

template<typename T>
struct optional {
  constexpr optional() = default;
  constexpr optional(T v) : value_(v), has_value_(true) {}

  constexpr explicit operator bool() const noexcept { return has_value_; }
  constexpr bool has_value() const noexcept { return has_value_; }
  constexpr const T& value() const noexcept { return value_; }
  constexpr T& value() noexcept { return value_; }
  constexpr const T& operator*() const noexcept { return value_; }
  constexpr T& operator*() noexcept { return value_; }

  T    value_{};
  bool has_value_ = false;
};

template<typename T>
constexpr bool operator==(const optional<T>& a, const T& b) noexcept
{
  return a.has_value() && *a == b;
}

template<typename T>
constexpr bool operator==(const T& a, const optional<T>& b) noexcept
{
  return b == a;
}

template<typename T>
constexpr bool operator!=(const optional<T>& a, const T& b) noexcept
{
  return !(a == b);
}

template<typename T>
constexpr bool operator!=(const T& a, const optional<T>& b) noexcept
{
  return !(a == b);
}

template<typename T>
constexpr bool operator==(const optional<T>& a, const optional<T>& b) noexcept
{
  return a.has_value() == b.has_value() && (!a.has_value() || *a == *b);
}

template<typename T>
constexpr bool operator!=(const optional<T>& a, const optional<T>& b) noexcept
{
  return !(a == b);
}

struct string_view {
  static constexpr std::size_t npos = static_cast<std::size_t>(-1);

  constexpr string_view() = default;
  constexpr string_view(const char* s) : begin_(s), end_(s + length(s)) {}
  constexpr string_view(const char* s, std::size_t size) : begin_(s), end_(s + size) {}
  constexpr string_view(const string_view&) = default;
  constexpr string_view& operator=(const string_view&) = default;
  string_view(const std::string& s) : begin_(s.data()), end_(s.data() + s.size()) {}

  static constexpr std::size_t length(const char* s) noexcept
  {
    std::size_t count = 0;
    while (s[count] != '\0')
      ++count;
    return count;
  }

  constexpr const char* data() const noexcept { return begin_; }
  constexpr std::size_t size() const noexcept { return static_cast<std::size_t>(end_ - begin_); }
  constexpr bool        empty() const noexcept { return begin_ == end_; }

  constexpr const char* begin() const noexcept { return begin_; }
  constexpr const char* end() const noexcept { return end_; }

  constexpr char operator[](std::size_t i) const noexcept { return begin_[i]; }

  operator std::string() const { return std::string(data(), size()); }

  constexpr void remove_prefix(std::size_t amount) noexcept { begin_ += amount; }
  constexpr void remove_suffix(std::size_t amount) noexcept { end_ -= amount; }

  constexpr string_view substr(std::size_t offset) const noexcept
  {
    return offset > size() ? string_view(end_, 0) : string_view(begin_ + offset, size() - offset);
  }

  constexpr string_view substr(std::size_t offset, std::size_t count) const noexcept
  {
    return offset > size() ? string_view(end_, 0) : string_view(begin_ + offset, count > size() - offset ? size() - offset : count);
  }

  constexpr std::size_t find(char c, std::size_t pos = 0) const noexcept
  {
    for (std::size_t i = pos; i < size(); ++i)
      if (begin_[i] == c)
        return i;
    return npos;
  }

  constexpr std::size_t rfind(char c) const noexcept
  {
    for (std::size_t i = size(); i != 0; --i)
      if (begin_[i - 1] == c)
        return i - 1;
    return npos;
  }

  constexpr std::size_t rfind(string_view needle) const noexcept
  {
    if (needle.empty())
      return size();
    if (needle.size() > size())
      return npos;

    for (std::size_t pos = size() - needle.size() + 1; pos != 0; --pos) {
      const std::size_t offset = pos - 1;
      bool              match  = true;
      for (std::size_t i = 0; i < needle.size(); ++i) {
        if (begin_[offset + i] != needle[i]) {
          match = false;
          break;
        }
      }
      if (match)
        return offset;
    }
    return npos;
  }

  constexpr std::size_t rfind(const char* s) const noexcept { return rfind(string_view(s)); }

  const char* begin_ = nullptr;
  const char* end_   = nullptr;
};

constexpr bool operator==(string_view a, string_view b) noexcept
{
  if (a.size() != b.size())
    return false;
  for (std::size_t i = 0; i < a.size(); ++i)
    if (a[i] != b[i])
      return false;
  return true;
}

constexpr bool operator!=(string_view a, string_view b) noexcept { return !(a == b); }

} // namespace tests_cpp14

#define ENCHANTUM_ALIAS_STRING_VIEW using string_view = tests_cpp14::string_view;
#define ENCHANTUM_ALIAS_OPTIONAL \
  template<typename T>             \
  using optional = tests_cpp14::optional<T>;
