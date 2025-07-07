#pragma once

#include "details/string_view.hpp"

#if defined(__clang__)
  #include "details/enchantum_clang.hpp"
#elif defined(__GNUC__) || defined(__GNUG__)
  #include "details/enchantum_gcc.hpp"
#elif defined(_MSC_VER)
  #include "details/enchantum_msvc.hpp"
#endif

#include "common.hpp"
#include <type_traits>
#include <utility>

namespace enchantum {

#ifdef __cpp_lib_to_underlying
using ::std::to_underlying;
#else
template<Enum E>
[[nodiscard]] constexpr auto to_underlying(const E e) noexcept
{
  return static_cast<std::underlying_type_t<E>>(e);
}
#endif


namespace details {
  template<typename E, bool NullTerminated = true>
  inline constexpr auto reflection_data = reflect<E, NullTerminated>();

  template<typename E, bool NullTerminated = true>
  inline constexpr auto reflection_string_indices = reflection_data<E, NullTerminated>.string_indices;
} // namespace details


template<Enum E, typename Pair = std::pair<E, string_view>, bool NullTerminated = true>
inline constexpr auto entries = []() {
  const auto             reflected = details::reflection_data<std::remove_cv_t<E>, NullTerminated>;
  constexpr auto         size      = sizeof(reflected.values) / sizeof(reflected.values[0]);
  std::array<Pair, size> ret;
  auto* const            ret_data = ret.data();


  for (std::size_t i = 0; i < size; ++i) {
    auto& [e, s]     = ret_data[i];
    e                = reflected.values[i];
    using StringView = std::remove_cvref_t<decltype(s)>;
    s                = StringView(reflected.strings + reflected.string_indices[i],
                   reflected.strings + reflected.string_indices[i + 1] - NullTerminated);
  }
  return ret;
}();

template<Enum E>
inline constexpr auto values = []() {
  constexpr auto&             enums = entries<E>;
  std::array<E, enums.size()> ret;
  const auto* const           enums_data = enums.data();
  for (std::size_t i = 0; i < ret.size(); ++i)
    ret[i] = enums_data[i].first;
  return ret;
}();

template<Enum E, typename String = string_view, bool NullTerminated = true>
inline constexpr auto names = []() {
  constexpr auto&                  enums = entries<E, std::pair<E, String>, NullTerminated>;
  std::array<String, enums.size()> ret;
  const auto* const                enums_data = enums.data();
  for (std::size_t i = 0; i < ret.size(); ++i)
    ret[i] = enums_data[i].second;
  return ret;
}();

template<Enum E>
inline constexpr auto min = entries<E>.front().first;

template<Enum E>
inline constexpr auto max = entries<E>.back().first;

template<Enum E>
inline constexpr std::size_t count = entries<E>.size();


template<typename>
inline constexpr bool has_zero_flag = false;

template<BitFlagEnum E>
inline constexpr bool has_zero_flag<E> = []() {
  for (const auto v : values<E>)
    if (static_cast<std::underlying_type_t<E>>(v) == 0)
      return true;
  return false;
}();

template<typename>
inline constexpr bool is_contiguous = false;

template<Enum E>
inline constexpr bool is_contiguous<E> = static_cast<std::size_t>(
                                           enchantum::to_underlying(max<E>) - enchantum::to_underlying(min<E>)) +
    1 ==
  count<E>;

template<typename E>
concept ContiguousEnum = Enum<E> && is_contiguous<E>;

template<typename>
inline constexpr bool is_contiguous_bitflag = false;

template<BitFlagEnum E>
inline constexpr bool is_contiguous_bitflag<E> = []() {
  constexpr auto& enums = entries<E>;
  using T               = std::underlying_type_t<E>;
  for (auto i = std::size_t{has_zero_flag<E>}; i < enums.size() - 1; ++i)
    if (T(enums[i].first) << 1 != T(enums[i + 1].first))
      return false;
  return true;
}();

template<typename E>
concept ContiguousBitFlagEnum = BitFlagEnum<E> && is_contiguous_bitflag<E>;

} // namespace enchantum