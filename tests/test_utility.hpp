#pragma once
#include "case_insensitive.hpp"
#include <cstdint>
#include <enchantum/bitwise_operators.hpp>
#include <type_traits>

template<typename... Commas>
struct TypeWithCommas;

// Tests whether string parsing will break if commas occur in typename
namespace LongNamespaced::Namespace2::inline InlineNamespace {
template<typename... IAmHereForCommasInTypeName>
struct Really_Unreadable_Class_Name {

  enum class Color : std::int16_t {
    Aqua   = -42,
    Purple = 21,
    Green  = 124,
    Red    = 213,
    Blue,
  };


  enum class Flags : std::uint8_t {
    Flag0 = 1 << 0,
    Flag1 = 1 << 1,
    Flag2 = 1 << 2,
    Flag3 = 1 << 3,
    Flag4 = 1 << 4,
    Flag5 = 1 << 5,
    Flag6 = 1 << 6,
  };

  enum UnscopedColor : std::int64_t {
    Aqua   = -42,
    Purple = 21,
    Green  = 124,
    Red    = 213,
    Blue,
  };
};

//template<typename... Ts>
//[[nodiscard]] constexpr auto operator~(typename Really_Unreadable_Class_Name<Ts...>::Flags a) noexcept
//{
//  using T = std::underlying_type_t<decltype(a)>;
//  return static_cast<decltype(a)>(~static_cast<T>(a));
//}
//
//template<typename... Ts>
//[[nodiscard]] constexpr auto operator|(typename Really_Unreadable_Class_Name<Ts...>::Flags a,
//                                        typename Really_Unreadable_Class_Name<Ts...>::Flags b) noexcept
//{
//  using T = std::underlying_type_t<decltype(a)>;
//  return static_cast<decltype(a)>(static_cast<T>(a) | static_cast<T>(b));
//}
//
//template<typename... Ts>
//[[nodiscard]] constexpr auto operator&(typename Really_Unreadable_Class_Name<Ts...>::Flags a,
//                                        typename Really_Unreadable_Class_Name<Ts...>::Flags b) noexcept
//{
//  using T = std::underlying_type_t<decltype(a)>;
//  return static_cast<decltype(a)>(static_cast<T>(a) & static_cast<T>(b));
//}
//
//template<typename... Ts>
//constexpr auto& operator|=(typename Really_Unreadable_Class_Name<Ts...>::Flags& a,
//                           typename Really_Unreadable_Class_Name<Ts...>::Flags  b) noexcept
//{
//  return a = a | b;
//}

//template<typename... Ts>
//constexpr auto& operator&=(typename Really_Unreadable_Class_Name<Ts...>::Flags& a,
//                           typename Really_Unreadable_Class_Name<Ts...>::Flags b) noexcept
//{
//  return a = a & b;
//}

using UglyType = Really_Unreadable_Class_Name<int, long, int***, TypeWithCommas<int, long[3], TypeWithCommas<long, int>>>;
#ifdef __clang__
using Color         = decltype(UglyType::Color::Aqua);
using Flags         = decltype(UglyType::Flags::Flag0);
using UnscopedColor = decltype(UglyType::UnscopedColor::Aqua);
#else
using Color         = UglyType::Color;
using Flags         = UglyType::Flags;
using UnscopedColor = UglyType::UnscopedColor;
#endif

ENCHANTUM_DEFINE_BITWISE_FOR(Flags)

} // namespace LongNamespaced::Namespace2::inline InlineNamespace

namespace {
enum class Letters {
  a,
  b,
  c,
  d,
  e,
  f,
  g,
  h,
  i,
  j,
  k,
  l,
  m,
  n,
  o,
  p,
  q,
  r,
  s,
  t,
  u,
  v,
  w,
  x,
  y,
  z
};
} // namespace

enum class FlagsWithNone : unsigned char {
  None  = 0,
  Flag0 = 1 << 0,
  Flag1 = 1 << 1,
  Flag2 = 1 << 2,
  Flag3 = 1 << 3,
  Flag4 = 1 << 4,
  Flag5 = 1 << 5,
  Flag6 = 1 << 6,
  Flag7 = 1 << 7,
};
ENCHANTUM_DEFINE_BITWISE_FOR(FlagsWithNone)

enum class Direction2D : std::uint8_t {
  None  = 0,
  Left  = 1,
  Right = 3,
  Up    = 2,
  Down  = 4,

  East  = Right,
  West  = Left,
  North = Up,
  South = Down
};

enum class Direction3D : std::int16_t {
  None  = 0,
  Left  = 1,
  Right = 3,
  Up    = 2,
  Down  = 4,
  Front = 5,
  Back  = 7,

  Top = Up,
};


enum Unscoped : int {
};

enum UnscopedCStyle {
  Unscoped_CStyle_Val0   = -11,
  Unscoped_CStyle_Value1 = 0,
  Unscoped_CStyle_Value2 = 55,
  Unscoped_CStyle_Value3 = 32,
  Unscoped_CStyle_Value4 = 11,
};


// Clang seems to have weird behavior with enum in templates
// It does not display them in pretty function names unless atleast 1 member of the enum was
// used.


enum class BoolEnum : bool {
  False,
  True
};

using namespace LongNamespaced::Namespace2;
  template<typename...>
struct type_list {}; // not wanting to include tuple to detect if I am missing a header in tests

template<typename A, typename B>
using concat = std::remove_pointer_t<decltype([]<class... Ts, class... Us>(type_list<Ts...>, type_list<Us...>) {
  return type_list<Ts..., Us...>{};
}(A{}, B{}))>;

using AllFlagsTestTypes = type_list<FlagsWithNone, Flags>;
using AllEnumsTestTypes = concat<AllFlagsTestTypes,
                                 type_list<Color, UnscopedColor, UnscopedCStyle, BoolEnum, Direction2D, Direction3D, Letters>>;
