#pragma once
#include "enchantum.hpp"
#include <array>
#include <stdexcept>

namespace enchantum {

template<Enum E, typename V>
class array : public std::array<V, count<E>> {
private:
  using base = std::array<V, count<E>>;
public:
  using index_type = E;
  using typename base::reference;
  using typename base::const_reference;

  using base::at;
  using base::operator[];

  [[nodiscard]] constexpr reference at(const E index)
  {
    if (const auto i = enchantum::enum_to_index(index))
      return operator[](*i);
    ENCHANTUM_THROW(std::out_of_range("enchantum::array::at index out of range"), index);
  }

  [[nodiscard]] constexpr const_reference at(const E index) const
  {
    if (const auto i = enchantum::enum_to_index(index))
      return operator[](*i);
    ENCHANTUM_THROW(std::out_of_range("enchantum::array::at: index out of range"), index);
  }

  [[nodiscard]] constexpr reference operator[](const E index) noexcept
  {
    return operator[](*enchantum::enum_to_index(index));
  }

  [[nodiscard]] constexpr const_reference operator[](const E index) const noexcept
  {
    return operator[](*enchantum::enum_to_index(index));
  }
};

} // namespace enchantum
