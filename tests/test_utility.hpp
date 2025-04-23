#pragma once
#include <cstdint>

// Tests whether string parsing will break if commas occur in typename
namespace LongNamespaced::Namespace2 {
template<typename... IAmHereForCommasInTypeName>
struct Really_Unreadable_Class_Name {

  enum class Color {
    Aqua   = -42,
    Purple = 21,
    Green = 124,
    Red   = 213,
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
    Flag7 = 1 << 7
  };

  
  
  
  
};
} // namespace LongNamespaced::Namespace2

namespace {
enum class Letters  {
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


enum Unscoped : int {};

template<typename... Commas>
struct TypeWithCommas;

using Color = LongNamespaced::Namespace2::
  Really_Unreadable_Class_Name<int, long, int***, TypeWithCommas<int, long[3], TypeWithCommas<long, int>>>::Color;

using Flags = LongNamespaced::Namespace2::
  Really_Unreadable_Class_Name<int, long, int***, TypeWithCommas<int, long[3], TypeWithCommas<long, int>>>::Flags;


Flags operator~(Flags);
bool   operator&(Flags, Flags);
Flags  operator|(Flags, Flags);
Flags& operator|=(Flags&, Flags);
Flags& operator&=(Flags&, Flags);

enum class BoolEnum : bool { False,True };
