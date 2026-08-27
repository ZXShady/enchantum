#pragma once

namespace enchantum {
template<typename E>
struct enum_traits;

#if defined(__cpp_impl_reflection) && __cpp_impl_reflection >= 202506L

struct ignore_t {};
constexpr inline ignore_t ignore{};

#endif

} // namespace enchantum

