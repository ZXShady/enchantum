#pragma once

#include "enchantum.hpp"
#include <concepts>

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
noexcept(std::is_nothrow_invocable_v<Func, E>)
{
  using Ret = decltype(func(e));
  

  return [&]<std::size_t... Idx>(std::index_sequence<Idx...>) {
    if ((values<Enums>[Idx] == enums))
      (func(std::integral_constant<E, values<E>[Idx]> {}), ...);
  }(std::make_index_sequence<count<E>>());
}
template<Enum... Enums, std::invocable<Enums...> Func>
constexpr auto visit(Func func, Enums... enums) noexcept(std::is_nothrow_invocable_v<Func, Enums...>)
{
  using Ret = decltype(func(enums...));
  return [&]<std::size_t... Idx>(std::index_sequence<Idx...>) {
    if ((values<Enums>[Idx] == enums) && ...)
      (func(std::integral_constant<E, values<E>[Idx]> {}), ...);
  }(std::make_index_sequence<count<Enums>>()...);
}
#endif
#if 0
namespace details {
  template<enchantum::Enum E, typename Func, std::size_t... Idx>
  constexpr void for_each(Func& func, std::index_sequence<Idx...>) noexcept(
    noexcept((func(std::integral_constant<E, values<E>[Idx]> {}), ...)))
  {
    (void)(func(std::integral_constant<E, values<E>[Idx]> {}), ...);
  }

} // namespace details

template<Enum E, E Func>
constexpr void for_each(Func func) noexcept(noexcept(details::for_each<E>(func, std::make_index_sequence<count<E>>{})))
{
  details::for_each<E>(func, std::make_index_sequence<count<E>>());
}
#endif
} // namespace enchantum