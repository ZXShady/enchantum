#pragma once

#include "enchantum.hpp"
#include <utility>
#include <cstddef>

namespace enchantum {

#if 0
namespace details {

  template<std::size_t range, std::size_t sets>
  constexpr auto cartesian_product()
  {
    constexpr auto size = []() {
      std::size_t x = range;
      std::size_t n = sets;
      while (--n != 0)
        x *= range;
      return x;
    }();

    std::array<std::array<std::size_t, sets>, size> products{};
    std::array<std::size_t, sets>                   counter{};

    for (auto& product : products) {
      product = counter;

      ++counter.back();
      for (std::size_t i = counter.size() - 1; i != 0; i--) {
        if (counter[i] != range)
          break;

        counter[i] = 0;
        ++counter[i - 1];
      }
    }
    return products;
  }

} // namespace details
#endif

#if 0
template<Enum E, std::invocable<E> Func>
constexpr auto visit(Func func, E e) 
noexcept(noexcept(std::declval<Func>()(std::declval<E>())))
{
  using Ret = decltype(func(e));
  

  return [&]<std::size_t... Idx>(std::index_sequence<Idx...>) {
    if ((values<Enums>[Idx] == enums))
      (func(std::integral_constant<E, values<E>[Idx]> {}), ...);
  }(std::make_index_sequence<count<E>>());
}
template<Enum... Enums, std::invocable<Enums...> Func>
constexpr auto visit(Func func, Enums... enums) noexcept(noexcept(std::declval<Func>()(std::declval<Enums>()...)))
{
  using Ret = decltype(func(enums...));
  return [&]<std::size_t... Idx>(std::index_sequence<Idx...>) {
    if ((values<Enums>[Idx] == enums) && ...)
      (func(std::integral_constant<E, values<E>[Idx]> {}), ...);
  }(std::make_index_sequence<count<Enums>>()...);
}
#endif
namespace details {

  template<typename E, typename Func, std::size_t... I>
  constexpr void for_each(Func& f, std::index_sequence<I...>)
  {
    using expander = int[];
    (void)expander{0, ((void)f(std::integral_constant<E, values<E>[I]> {}), 0)...};
  }

} // namespace details

template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E), typename Func>
constexpr void for_each(Func f) // intentional not const
{
  details::for_each<E>(f, std::make_index_sequence<count<E>>{});
}
} // namespace enchantum
