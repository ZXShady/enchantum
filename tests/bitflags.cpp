#include "case_insensitive.hpp"
#include <array>
#include <catch2/catch_test_macros.hpp>
#include <cctype>
#include <enchantum/bitflags.hpp>
#include <enchantum/bitwise_operators.hpp>
#include <enchantum/enchantum.hpp>
#include <enchantum/iostream.hpp>
#include <sstream>
#include <string_view>

namespace {
enum class EntityStatus {
  Active   = 1,
  Inactive = 2,
  Pending  = 4
};
ENCHANTUM_DEFINE_BITWISE_FOR(EntityStatus)

} // namespace

enum class Permission : int {
  None    = 0,
  Read    = 1 << 1,
  Write   = 1 << 2,
  Execute = 1 << 3,
};

struct DirectionFlags_Struct {
  enum Type : std::uint64_t {
    NoDirection = 0,
    Left        = std::uint64_t{1} << 10,
    Right       = std::uint64_t{1} << 20,
    Up          = std::uint64_t{1} << 31,
    Down        = std::uint64_t{1} << 63,
  };
};
using DirectionFlags = DirectionFlags_Struct::Type;

enum Level : int {
  NoLevel = 0,
  Level1  = 1 << 1,
  Level2  = 1 << 2,
  Level3  = 1 << 3,
  Level4  = 1 << 4,

  Alias1 = Level1,
  Alias2 = Level2,
  Alias3 = Level3,
  Alias4 = Level4
};
ENCHANTUM_DEFINE_BITWISE_FOR(Permission)
ENCHANTUM_DEFINE_BITWISE_FOR(DirectionFlags)
ENCHANTUM_DEFINE_BITWISE_FOR(Level)

TEST_CASE("cast cast_bitflags", "[casts][bitflags]")
{
  SECTION("from strings")
  {
    STATIC_REQUIRE(enchantum::cast<EntityStatus>("Active") == EntityStatus::Active);
    STATIC_REQUIRE(enchantum::cast<EntityStatus>("Inactive") == EntityStatus::Inactive);
    STATIC_REQUIRE(enchantum::cast<EntityStatus>("pending", case_insensitive) == EntityStatus::Pending);
    STATIC_REQUIRE_FALSE(enchantum::cast_bitflag<EntityStatus>("pending|Active", '|', case_insensitive).has_value());
  }

  STATIC_REQUIRE(
    enchantum::cast_bitflag<EntityStatus>("Inactive|Active") == (EntityStatus::Inactive | EntityStatus::Active));

  STATIC_REQUIRE(enchantum::cast<Permission>("Read") == Permission::Read);
  STATIC_REQUIRE(enchantum::cast<Permission>("Write") == Permission::Write);
  STATIC_REQUIRE(enchantum::cast<Permission>("Execute") == Permission::Execute);
  STATIC_REQUIRE_FALSE(enchantum::cast<Permission>("None").has_value());

  STATIC_REQUIRE(enchantum::cast<DirectionFlags>("Up") == DirectionFlags::Up);
  STATIC_REQUIRE(enchantum::cast<DirectionFlags>("Down") == DirectionFlags::Down);
  STATIC_REQUIRE(enchantum::cast<DirectionFlags>("Right") == DirectionFlags::Right);
  STATIC_REQUIRE(enchantum::cast<DirectionFlags>("Left") == DirectionFlags::Left);
  STATIC_REQUIRE_FALSE(enchantum::cast<DirectionFlags>("NoDirection").has_value());

  STATIC_REQUIRE(enchantum::cast<Level>("Level1") == Level::Level1);
  STATIC_REQUIRE(enchantum::cast<Level>("Level2") == Level::Level2);
  STATIC_REQUIRE(enchantum::cast<Level>("Level3") == Level::Level3);
  STATIC_REQUIRE(enchantum::cast<Level>("Level4") == Level::Level4);
  STATIC_REQUIRE(enchantum::cast_bitflag<Level>("Level3|Level1") == (Level::Level3 | Level::Level1));
  STATIC_REQUIRE_FALSE(enchantum::cast<Level>("NoLevel").has_value());
}

TEST_CASE("contains_bitflag", "[contains][bitflags]")
{
  {
    // shut down warnings about unneeded internal declarations
    EntityStatus e{};
    EntityStatus e2{};
    (void)(e & e);
    (void)(e &= e2);
    (void)(e |= e2);
    (void)(e | e);
    (void)(e ^ e);
    (void)(e ^= e2);
    (void)(~e);
  }
  SECTION("DirectionFlags")
  {
    STATIC_REQUIRE_FALSE(enchantum::contains_bitflag(DirectionFlags::NoDirection));
    STATIC_REQUIRE(enchantum::contains_bitflag(DirectionFlags::Up | DirectionFlags::Down));
    STATIC_REQUIRE_FALSE(enchantum::contains_bitflag(DirectionFlags(0xdeadbeef)));
  }
  SECTION("Level")
  {
    STATIC_REQUIRE_FALSE(enchantum::contains_bitflag(Level::NoLevel));
    STATIC_REQUIRE(enchantum::contains_bitflag(Level::Level1 | Level::Level3));
  }
}
