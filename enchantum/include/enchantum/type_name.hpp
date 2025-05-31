#pragma once
#include "details/string_view.hpp"
#include <array>

namespace enchantum {

namespace details {
#define SZC(x) (sizeof(x) - 1)
  template<typename T>
  constexpr auto type_name_func() noexcept
  {
#if defined(__clang__)
    constexpr std::size_t prefix = 0;
    constexpr auto        s = string_view(__PRETTY_FUNCTION__ + SZC("auto enchantum::details::type_name_func() [_ = "),
                                   SZC(__PRETTY_FUNCTION__) - SZC("auto enchantum::details::type_name_func() [_ = ]"));
#elif defined(_MSC_VER)
    constexpr auto s = string_view(__FUNCSIG__ + SZC("auto __cdecl enchantum::details::type_name_func<"),
                                   SZC(__FUNCSIG__) - SZC("auto __cdecl enchantum::details::type_name_func<") -
                                     SZC(">(void) noexcept"));

    // clang-format off
    constexpr auto prefix = std::is_enum_v<T> ? SZC("enum ") : 
        std::is_class_v<T> ?  SZC("struct ") - (s[0] == 'c') :
        0;
// clang-format on
#elif defined(__GNUG__)
    constexpr std::size_t prefix = 0;
    constexpr auto s = string_view(__PRETTY_FUNCTION__ + SZC("constexpr auto enchantum::details::type_name_func() [with _ = "),
                                   SZC(__PRETTY_FUNCTION__) -
                                     SZC("constexpr auto enchantum::details::type_name_func() [with _ = ]"));
#endif
    std::array<char, 1 + s.size() - prefix> ret{};
    auto* const                             ret_data = ret.data();
    const auto* const                       s_data   = s.data();

    for (std::size_t i = 0; i < ret.size() - 1; ++i)
      ret_data[i] = s_data[i + prefix];
    return ret;
  }

  template<typename T>
  inline constexpr auto type_name_func_var = type_name_func<T>();
#undef SZC

} // namespace details

template<typename T>
inline constexpr auto type_name = string_view(details::type_name_func_var<T>.data(),
                                              details::type_name_func_var<T>.size() - 1);

} // namespace enchantum