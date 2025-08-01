#pragma once
#include "enchantum.hpp"
#ifndef ENCHANTUM_ALIAS_BITSET
  #include <bitset>
#endif
#include "bitflags.hpp"
#include "generators.hpp"
#include <stdexcept>

namespace enchantum {

namespace details {
#ifndef ENCHANTUM_ALIAS_BITSET
  using ::std::bitset;
#else
  ENCHANTUM_ALIAS_BITSET;
#endif
} // namespace details

template<typename E, typename Container = details::bitset<count<E>>>
class bitset : public Container {
  static_assert(std::is_enum_v<E>);
public:

  using container_type = Container;
  using typename Container::reference;

  using Container::operator[];
  using Container::flip;
  using Container::reset;
  using Container::set;
  using Container::test;

  using Container::Container;
  using Container::operator=;

  [[nodiscard]] string to_string(const char sep = '|') const
  {
    string name;
    for (std::size_t i = 0; i < enchantum::count<E>; ++i) {
      if (test(i)) {
        const auto s = enchantum::names_generator<E>[i];
        if (!name.empty())
          name += sep;
        name.append(s.data(), s.size()); // not using operator += since this may not be std::string_view always
      }
    }
    return name;
  }

  [[nodiscard]] constexpr auto to_string(const char zero, const char one) const
  {
    return Container::to_string(zero, one);
  }

  constexpr bitset(const std::initializer_list<E> values) noexcept
  {
    for (auto value : values) {
      set(value, true);
    }
  }

  [[nodiscard]] constexpr reference operator[](const E index) noexcept
  {
    return operator[](*enchantum::enum_to_index(index));
  }

  [[nodiscard]] constexpr bool operator[](const E index) const noexcept
  {
    return operator[](*enchantum::enum_to_index(index));
  }

  constexpr bool test(const E pos)
  {

    if (const auto i = enchantum::enum_to_index(pos))
      return test(*i);
    ENCHANTUM_THROW(std::out_of_range("enchantum::bitset::test(E pos,bool value) out of range exception"), pos);
  }

  constexpr bitset& set(const E pos, bool value = true)
  {

    if (const auto i = enchantum::enum_to_index(pos))
      return static_cast<bitset&>(set(*i, value));
    ENCHANTUM_THROW(std::out_of_range("enchantum::bitset::set(E pos,bool value) out of range exception"), pos);
  }


  constexpr bitset& reset(const E pos)
  {
    if (const auto i = enchantum::enum_to_index(pos))
      return static_cast<bitset&>(reset(*i));
    ENCHANTUM_THROW(std::out_of_range("enchantum::bitset::reset(E pos) out of range exception"), pos);
  }

  constexpr bitset& flip(const E pos)
  {
    if (const auto i = enchantum::enum_to_index(pos))
      return static_cast<bitset&>(flip(*i));
    ENCHANTUM_THROW(std::out_of_range("enchantum::bitset::flip(E pos) out of range exception"), pos);
  }
};

} // namespace enchantum


template<typename E>
struct std::hash<enchantum::bitset<E>> : std::hash<enchantum::details::bitset<enchantum::count<E>>> {
  using std::hash<enchantum::details::bitset<enchantum::count<E>>>::operator();
};
