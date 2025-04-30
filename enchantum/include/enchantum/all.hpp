#pragma once

#include "enchantum.hpp"
#include "algorithms.hpp"
#include "cast.hpp"
#include "next_value.hpp"
#if __has_include(<fmt/format.h>)
#include "fmt_format.hpp"
#elif __has_include(<format>)
  #include "std_format.hpp"
#endif
#include "bitflags.hpp"
#include "iostream.hpp"
#include "bitwise_operators.hpp"