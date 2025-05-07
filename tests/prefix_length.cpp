#include "test_utility.hpp"
#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <enchantum/bitwise_operators.hpp>
#include <enchantum/enchantum.hpp>

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
ENCHANTUM_DEFINE_BITWISE_FOR(ImGuiFreeTypeBuilderFlags)

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

template<>
struct enchantum::enum_traits<ImGuiFreeTypeBuilderFlags> {
  static constexpr std::size_t prefix_length = sizeof("ImGuiFreeTypeBuilderFlags_") - 1;
  static constexpr auto        min           = ENCHANTUM_MIN_RANGE;
  static constexpr auto        max           = ENCHANTUM_MAX_RANGE;
};
TEST_CASE("prefix_length", "[count]")
{
  constexpr auto count = enchantum::count<ImGuiFreeTypeBuilderFlags>;
  STATIC_CHECK(count == enchantum::count<ImGuiFreeTypeBuilderFlags_Classy>);
  for (std::size_t i = 0; i < count; ++i) {
    CHECK(enchantum::names<ImGuiFreeTypeBuilderFlags>[i] == enchantum::names<ImGuiFreeTypeBuilderFlags_Classy>[i]);
  }
}