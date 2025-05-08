#include "test_utility.hpp"
#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <enchantum/bitwise_operators.hpp>
#include <enchantum/enchantum.hpp>


enum class ImGuiFreeTypeBuilderFlags_Classy {
  NoHinting     = 1 << 0,
  NoAutoHint    = 1 << 1,
  ForceAutoHint = 1 << 2,
  LightHinting  = 1 << 3,
  MonoHinting   = 1 << 4,
  Bold          = 1 << 5,
  Oblique       = 1 << 6,
  Monochrome    = 1 << 7,
  LoadColor     = 1 << 8,
  Bitmap        = 1 << 9
};
ENCHANTUM_DEFINE_BITWISE_FOR(ImGuiFreeTypeBuilderFlags_Classy)


TEST_CASE("prefix_length", "[count]")
{
  constexpr auto count = enchantum::count<ImGuiFreeTypeBuilderFlags>;
  STATIC_CHECK(count == enchantum::count<ImGuiFreeTypeBuilderFlags_Classy>);
  for (std::size_t i = 0; i < count; ++i) {
    CHECK(enchantum::names<ImGuiFreeTypeBuilderFlags>[i] == enchantum::names<ImGuiFreeTypeBuilderFlags_Classy>[i]);
  }
}