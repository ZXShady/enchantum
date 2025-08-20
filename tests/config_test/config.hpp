#pragma once
#include <cstddef>
#include <cstdlib>

namespace tests {

template<typename T>
struct my_optional {
  my_optional()                   = default;
  my_optional(const my_optional&) = default;
  template<typename U>
  void operator=(const U&) = delete;

  my_optional(T t) : value(t), has_value(true) {}
  explicit operator bool() const { return has_value; }
  T        operator*() const
  {
    if (!has_value)
      std::abort();
    return value;
  }
  T    value{};
  bool has_value{};
};

struct my_string_view {
  enum : std::size_t {
    npos = std::size_t(-1)
  };
  static std::size_t string_length(const char* s)
  { 
      std::size_t count = 0;
      while (*s++)
      ++count;
      return count; 
  }

  constexpr my_string_view() = default;
  my_string_view(const char* s) : my_string_view(s, string_length(s)) {}
  constexpr my_string_view(const char* begin, std::size_t size) : mbegin(begin), mend(begin + size) {}
  constexpr const char* data() const { return mbegin; };
  constexpr auto        size() const { return std::size_t(mend - mbegin); };
  constexpr std::size_t rfind(const char c) const
  {
    for (auto i = static_cast<std::ptrdiff_t>(size()) - 1; i >= 0; --i)
      if (mbegin[i] == c)
        return i;
    return npos;
  }
  constexpr my_string_view substr(std::size_t offset) const
  {
    if (offset > size())
      offset = size();

    auto c = *this;
    c.mbegin += offset;
    return c;
  }
  constexpr void remove_prefix(std::size_t amount) { mbegin += amount; }

  constexpr void           remove_suffix(std::size_t amount) { mend -= amount; }
  constexpr my_string_view substr(std::size_t offset, std::size_t end) const
  {
    auto c = substr(offset);
    c.mend = c.mbegin + end;
    return c;
  }
  constexpr char operator[](const std::size_t i) const noexcept
  {
    if (i > size())
      std::abort();
    return mbegin[i];
  }
  friend bool operator==(my_string_view a, my_string_view b)
  {
    if (a.size() != b.size())
      return false;

    for (std::size_t i = 0; i < a.size(); ++i)
      if (a[i] != b[i])
        return false;
    return true;
  }
  auto begin() const { return mbegin; }
  auto end() const { return mend; }

  const char* mbegin{};
  const char* mend{};
};

} // namespace tests

#define ENCHANTUM_ALIAS_STRING_VIEW using string_view = tests::my_string_view;
#define ENCHANTUM_ALIAS_OPTIONAL \
  template<typename T>           \
  using optional = tests::my_optional<T>;
