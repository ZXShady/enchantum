#include <enchantum/enchantum.hpp>

#include <catch2/catch_test_macros.hpp>



namespace {

    enum class Keywords {
  _constexpr,
  _consteval,
  _constinit,
  _const
};
} // namespace

template<>
struct enchantum::enum_traits<Keywords> {
    constexpr static int min           = 0;
    constexpr static int max           = 4;
    constexpr static int prefix_length = 1;
};
TEST_CASE("Config")
{
  tests::my_optional<Keywords> keyword = enchantum::cast<Keywords>(enchantum::to_string(Keywords::_constexpr));
  CHECK(*keyword == Keywords::_constexpr);
}