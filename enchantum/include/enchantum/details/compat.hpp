#pragma once

#include <type_traits>
#include <utility>
#include <cstddef>

#if defined(_MSVC_LANG)
  #define ENCHANTUM_DETAILS_CXX_STD _MSVC_LANG
#else
  #define ENCHANTUM_DETAILS_CXX_STD __cplusplus
#endif

#if ENCHANTUM_DETAILS_CXX_STD >= 201703L
  #include <array>
#endif

#if ENCHANTUM_DETAILS_CXX_STD >= 201703L
  #define ENCHANTUM_DETAILS_INLINE_VAR inline
#else
  #define ENCHANTUM_DETAILS_INLINE_VAR
#endif

namespace enchantum {
namespace details {

template<typename...>
using void_t = void;

template<bool B>
using bool_constant = std::integral_constant<bool, B>;

template<typename F, typename... Args>
struct is_invocable {
private:
  template<typename U>
  static auto test(int) -> decltype(std::declval<U>()(std::declval<Args>()...), std::true_type{});

  template<typename>
  static std::false_type test(...);

public:
  static constexpr bool value = decltype(test<F>(0))::value;
};

template<typename T>
struct type_identity {
  using type = T;
};

template<typename T, std::size_t N>
struct cxx14_array {
  T elems[N == 0 ? 1 : N]{};

  constexpr T*       data() noexcept { return elems; }
  constexpr const T* data() const noexcept { return elems; }

  constexpr T&       operator[](std::size_t i) noexcept { return elems[i]; }
  constexpr const T& operator[](std::size_t i) const noexcept { return elems[i]; }

  constexpr T&       front() noexcept { return elems[0]; }
  constexpr const T& front() const noexcept { return elems[0]; }

  constexpr T&       back() noexcept { return elems[N == 0 ? 0 : N - 1]; }
  constexpr const T& back() const noexcept { return elems[N == 0 ? 0 : N - 1]; }

  constexpr T*       begin() noexcept { return elems; }
  constexpr const T* begin() const noexcept { return elems; }

  constexpr T*       end() noexcept { return elems + N; }
  constexpr const T* end() const noexcept { return elems + N; }

  constexpr std::size_t size() const noexcept { return N; }
};

#if ENCHANTUM_DETAILS_CXX_STD >= 201703L
template<typename T, std::size_t N>
using array = std::array<T, N>;
#else
template<typename T, std::size_t N>
using array = cxx14_array<T, N>;
#endif

} // namespace details
} // namespace enchantum
