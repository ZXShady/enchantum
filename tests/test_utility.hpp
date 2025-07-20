#pragma once
#include "case_insensitive.hpp"
#include <catch2/catch_tostring.hpp>
#include <cstdint>
#include <enchantum/bitwise_operators.hpp>
#include <enchantum/common.hpp>

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

  enum BlockType_ {
    Dirt     = -42,
    Stone    = 21,
    Obsidian = 124,
  };
  using BlockType = decltype(BlockType_::Dirt);
};

using UglyType = Really_Unreadable_Class_Name<int, long, int***, TypeWithCommas<int, long[3], TypeWithCommas<long, int>>>;
using BlockType = UglyType::BlockType;

// Clang seems to have weird behavior with enum in templates
// It does not display them in pretty function names unless atleast 1 member of the enum was
// used.
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

enum class StrongFlagsNoOverloadedOperators : std::uint8_t {
  Flag0 = 1 << 0,
  Flag1 = 1 << 1,
  Flag2 = 1 << 2,
  Flag3 = 1 << 3,
  Flag4 = 1 << 4,
  Flag5 = 1 << 5,
  Flag6 = 1 << 6,
};

template<>
inline constexpr bool enchantum::is_bitflag<StrongFlagsNoOverloadedOperators> = true;

// taken from ImGui.
// https://github.com/ocornut/imgui/blob/46235e91f602b663f9b0f1f1a300177b61b193f5/misc/freetype/imgui_freetype.h#L26

enum ImGuiFreeTypeBuilderFlags {
  ImGuiFreeTypeBuilderFlags_NoHinting     = 1 << 0,
  ImGuiFreeTypeBuilderFlags_NoAutoHint    = 1 << 1,
  ImGuiFreeTypeBuilderFlags_ForceAutoHint = 1 << 2,
  ImGuiFreeTypeBuilderFlags_LightHinting  = 1 << 3,
  ImGuiFreeTypeBuilderFlags_MonoHinting   = 1 << 4,
  ImGuiFreeTypeBuilderFlags_Bold          = 1 << 5,
  ImGuiFreeTypeBuilderFlags_Oblique       = 1 << 6,
  ImGuiFreeTypeBuilderFlags_Monochrome    = 1 << 7,
  ImGuiFreeTypeBuilderFlags_LoadColor     = 1 << 8,
  ImGuiFreeTypeBuilderFlags_Bitmap        = 1 << 9
};

template<>
struct enchantum::enum_traits<ImGuiFreeTypeBuilderFlags> {
  static constexpr std::size_t prefix_length = sizeof("ImGuiFreeTypeBuilderFlags_") - 1;
  static constexpr auto        min           = 0;
  static constexpr auto        max           = ENCHANTUM_MAX_RANGE;
};
template<>
inline constexpr bool enchantum::is_bitflag<ImGuiFreeTypeBuilderFlags> = true;

namespace {

enum MinMaxValuesCStyle {
  MinMaxValuesCStyle_min = ENCHANTUM_MIN_RANGE,
  MinMaxValuesCStyle_max = ENCHANTUM_MAX_RANGE,
};

enum class MinMaxValues {
  min = ENCHANTUM_MIN_RANGE,
  max = ENCHANTUM_MAX_RANGE,
};

enum class ContigNonZero {
  _0 = -10,
  _1,
  _2,
  _3,
  _4,
  _5,
  _6,
  _7,
  _8,
  _9,
};

enum ContigNonZeroCStyle {
  ContigNonZeroCStyle_0 = -1,
  ContigNonZeroCStyle_1,
  ContigNonZeroCStyle_2,
  ContigNonZeroCStyle_3,
  ContigNonZeroCStyle_4,
  ContigNonZeroCStyle_5,
  ContigNonZeroCStyle_6,
  ContigNonZeroCStyle_7,
  ContigNonZeroCStyle_8,
  ContigNonZeroCStyle_9,
};


enum ContigNonZeroStartWith5CStyle {
  ContigNonZeroStartWith5CStyle_0 = 5,
  ContigNonZeroStartWith5CStyle_1,
  ContigNonZeroStartWith5CStyle_2,
  ContigNonZeroStartWith5CStyle_3,
  ContigNonZeroStartWith5CStyle_4,
  ContigNonZeroStartWith5CStyle_5,
  ContigNonZeroStartWith5CStyle_6,
  ContigNonZeroStartWith5CStyle_7,
  ContigNonZeroStartWith5CStyle_8,
  ContigNonZeroStartWith5CStyle_9,
};

enum class UnderlyingTypeWChar_t : wchar_t {
  _0,
  _1,
  _2,
  _3,
  _4,
  _5,
  _6,
  _7,
  _8,
  _9,
};
enum class UnderlyingTypeChar16_t : char16_t {
  _0,
  _1,
  _2,
  _3,
  _4,
  _5,
  _6,
  _7,
  _8,
  _9,
};
enum class UnderlyingTypeChar32_t : char32_t {
  _0,
  _1,
  _2,
  _3,
  _4,
  _5,
  _6,
  _7,
  _8,
  _9,
};
#ifdef __cpp_char8_t
enum class UnderlyingTypeChar8_t : char8_t {
  _0,
  _1,
  _2,
  _3,
  _4,
  _5,
  _6,
  _7,
  _8,
  _9,
};
#endif
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

enum NonContigFlagsWithNoneCStyle : unsigned char {
  None  = 0,
  Flag0 = 1 << 0,
  Flag1 = 1 << 1,
  Flag2 = 1 << 2,
  Flag6 = 1 << 6,
};
ENCHANTUM_DEFINE_BITWISE_FOR(NonContigFlagsWithNoneCStyle)

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


enum CStyleFromA_To_G {
  a,
  b,
  c,
  e,
  d,
  f,
  g
};

namespace {
namespace Outer {
  namespace {
    namespace Inner {
      enum Anon {
        _0,
        _1,
        _2,
        _3,
        _4,
        _5,
        _6
      };
      enum CStyleAnon {
        CStyleAnon_0,
        CStyleAnon_1,
        CStyleAnon_2,
        CStyleAnon_3,
        CStyleAnon_4,
        CStyleAnon_5,
        CStyleAnon_6
      };
    } // namespace Inner
  } // namespace
} // namespace Outer
} // namespace


enum Unscoped : int {
};

enum UnscopedCStyle {
  Unscoped_CStyle_Val0   = -11,
  Unscoped_CStyle_Value1 = 0,
  Unscoped_CStyle_Value2 = 55,
  Unscoped_CStyle_Value3 = 32,
  Unscoped_CStyle_Value4 = 11,
};


enum class BoolEnum : bool {
  False,
  True
};

enum BoolEnumCStyle : bool {
  BoolEnumCStyle_False,
  BoolEnumCStyle_True
};


using namespace LongNamespaced::Namespace2;
template<typename...>
struct type_list {}; // not wanting to include tuple to detect if I am missing a header in tests

template<typename... Ts,typename... Us>
type_list<Ts..., Us...> concatter_func(type_list<Ts...>,type_list<Us...>);

template<typename A, typename B>
using concat = decltype(::concatter_func(A{},B{}));

using AllFlagsTestTypes = type_list<StrongFlagsNoOverloadedOperators, ImGuiFreeTypeBuilderFlags, NonContigFlagsWithNoneCStyle, FlagsWithNone, Flags>;
using AllEnumsTestTypes = concat<
  AllFlagsTestTypes,
  type_list<
#ifdef __cpp_char8_t
    UnderlyingTypeChar8_t,
#endif
    UnderlyingTypeChar16_t,
    UnderlyingTypeChar32_t,
    UnderlyingTypeWChar_t,

    MinMaxValuesCStyle,
    BlockType,
    ContigNonZeroStartWith5CStyle,
    BoolEnumCStyle,
    ContigNonZeroCStyle,
    ContigNonZero,
    MinMaxValues,
    Color,
    UnscopedColor,
    UnscopedCStyle,
    BoolEnum,
    Direction2D,
    Direction3D,
    Letters,
    CStyleFromA_To_G,
    Outer::Inner::Anon,
    Outer::Inner::CStyleAnon>>;

template<enchantum::Enum E>
struct Catch::StringMaker<E> {
  static std::string convert(E e)
  {
    return '"' + std::to_string(static_cast<std::underlying_type_t<E>>(e)) + '"';
    //if constexpr (enchantum::BitFlagEnum<E>)
    //  return "\"" + enchantum::to_string_bitflag(e) + "\" (" +
    //    std::to_string(static_cast<std::underlying_type_t<E>>(e)) + ")";
    //else
    //  return "\"" + std::string(enchantum::to_string(e)) + "\" (" +
    //    std::to_string(static_cast<std::underlying_type_t<E>>(e)) + ")";
  }
};
