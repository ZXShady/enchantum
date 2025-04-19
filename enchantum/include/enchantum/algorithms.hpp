#pragma once

#include "enchantum.hpp"
#include <concepts>

namespace enchantum {

template<Enum E, std::invocable<E> Func>
constexpr auto for_each(Func func) noexcept(std::is_nothrow_invocable_v<Func, E>)
{
  [&func]<std::size_t... Idx>(std::index_sequence<Idx...>) { 
	  (func(std::integral_constant<E, values<E>[Idx]>{}),...);
  }(std::make_index_sequence<count<E>>());
}

} // namespace enchantum