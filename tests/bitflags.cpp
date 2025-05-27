#include "case_insensitive.hpp"
#include "test_utility.hpp"
#include <array>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cctype>
#include <enchantum/bitflags.hpp>
#include <enchantum/bitwise_operators.hpp>
#include <enchantum/enchantum.hpp>
#include <enchantum/iostream.hpp>
#include <iostream>
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
    STATIC_CHECK(enchantum::cast<EntityStatus>("Active") == EntityStatus::Active);
    STATIC_CHECK(enchantum::cast<EntityStatus>("Inactive") == EntityStatus::Inactive);
    STATIC_CHECK(enchantum::cast<EntityStatus>("pending", case_insensitive) == EntityStatus::Pending);
    STATIC_CHECK(enchantum::cast_bitflag<EntityStatus>("peNdIng|Active", '|', case_insensitive).has_value());
  }

  STATIC_CHECK(enchantum::cast_bitflag<EntityStatus>("Inactive|Active") == (EntityStatus::Inactive | EntityStatus::Active));

  STATIC_CHECK(enchantum::cast<Permission>("Read") == Permission::Read);
  STATIC_CHECK(enchantum::cast<Permission>("Write") == Permission::Write);
  STATIC_CHECK(enchantum::cast<Permission>("Execute") == Permission::Execute);
  STATIC_CHECK(enchantum::cast<Permission>("None").has_value());

  STATIC_CHECK(enchantum::cast<DirectionFlags>("Up") == DirectionFlags::Up);
  STATIC_CHECK(enchantum::cast<DirectionFlags>("Down") == DirectionFlags::Down);
  STATIC_CHECK(enchantum::cast<DirectionFlags>("Right") == DirectionFlags::Right);
  STATIC_CHECK(enchantum::cast<DirectionFlags>("Left") == DirectionFlags::Left);
  STATIC_CHECK(enchantum::cast<DirectionFlags>("NoDirection").has_value());

  STATIC_CHECK(enchantum::cast<Level>("Level1") == Level::Level1);
  STATIC_CHECK(enchantum::cast<Level>("Level2") == Level::Level2);
  STATIC_CHECK(enchantum::cast<Level>("Level3") == Level::Level3);
  STATIC_CHECK(enchantum::cast<Level>("Level4") == Level::Level4);
  STATIC_CHECK(enchantum::cast_bitflag<Level>("Level3|Level1") == (Level::Level3 | Level::Level1));
  STATIC_CHECK(enchantum::cast<Level>("NoLevel").has_value());
}

TEST_CASE("invalid_casts", "[cast][bitflags]")
{
  SECTION("Invalid strings for enum cast")
  {
    STATIC_CHECK_FALSE(enchantum::cast<EntityStatus>("Unknown").has_value());
    STATIC_CHECK_FALSE(enchantum::cast<Permission>("ExecuteAll").has_value());
    STATIC_CHECK_FALSE(enchantum::cast<DirectionFlags>("UPWARD").has_value());
  }

  SECTION("Invalid case without case_insensitive flag")
  {
    STATIC_CHECK_FALSE(enchantum::cast<EntityStatus>("pending").has_value());
    STATIC_CHECK_FALSE(enchantum::cast<Permission>("read").has_value());
  }

  SECTION("Valid mixed-case cast with case_insensitive")
  {
    STATIC_CHECK(enchantum::cast<Permission>("eXecuTe", case_insensitive) == Permission::Execute);
  }

  SECTION("Invalid separator or format in bitflag cast")
  {
    STATIC_CHECK_FALSE(enchantum::cast_bitflag<Level>("Level1,Level2", '|', case_insensitive).has_value());
    STATIC_CHECK_FALSE(enchantum::cast_bitflag<Permission>("|Write|Read", '|', case_insensitive).has_value());
    STATIC_CHECK_FALSE(enchantum::cast_bitflag<Permission>("Write||Read", '|', case_insensitive).has_value());
    STATIC_CHECK_FALSE(enchantum::cast_bitflag<Permission>("Write|Read|", '|', case_insensitive).has_value());
  }

  SECTION("Empty input returns none") { STATIC_CHECK_FALSE(enchantum::cast_bitflag<Permission>("").has_value()); }
}

TEST_CASE("complex_bitflag_combinations", "[bitflags]")
{
  SECTION("Mixed ordering")
  {
    STATIC_CHECK(enchantum::cast_bitflag<Level>("Level4|Level2|Level1") == (Level::Level4 | Level::Level2 | Level::Level1));
  }

  SECTION("Duplicate flags")
  {
    STATIC_CHECK(enchantum::cast_bitflag<Permission>("Read|Read|Write") == (Permission::Read | Permission::Write));
  }
}

TEST_CASE("alias_and_zero_behavior", "[bitflags][aliases]")
{
  SECTION("Zero flag handling")
  {
    STATIC_CHECK(enchantum::cast<Permission>("None") == Permission::None);
    STATIC_CHECK(enchantum::contains_bitflag(Permission::None));
  }
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
    STATIC_CHECK(enchantum::contains_bitflag(DirectionFlags::NoDirection));
    STATIC_CHECK(enchantum::contains_bitflag(DirectionFlags::Up | DirectionFlags::Down));
    STATIC_CHECK_FALSE(enchantum::contains_bitflag(DirectionFlags(0xdeadbeef)));
  }

  SECTION("Level")
  {
    STATIC_CHECK(enchantum::contains_bitflag(Level::NoLevel));
    STATIC_CHECK(enchantum::contains_bitflag(Level::Level1 | Level::Level3));
  }
}

TEST_CASE("contains_bitflag_with_invalid_bits", "[bitflags]")
{
  SECTION("Contains invalid bits in DirectionFlags")
  {
    STATIC_CHECK_FALSE(enchantum::contains_bitflag(DirectionFlags(1 << 5))); // Not defined
    STATIC_CHECK_FALSE(enchantum::contains_bitflag(DirectionFlags(0xFFFFFFFFFFFFFFFF)));
  }

  SECTION("Valid combinations still pass")
  {
    STATIC_CHECK(enchantum::contains_bitflag(DirectionFlags::Left | DirectionFlags::Right));
  }
}

TEMPLATE_LIST_TEST_CASE("bitflags", "[bitflags]", AllFlagsTestTypes)
{
  constexpr auto  count  = enchantum::count<TestType>;
  constexpr auto& values = enchantum::values<TestType>;
  using T                = std::underlying_type_t<TestType>;
  std::vector<TestType> combinations;
  combinations.reserve(1u << count);
  constexpr auto total = std::uint64_t{1} << (count - enchantum::has_zero_flag<TestType>);
  for (std::uint64_t mask = 1; mask < total; ++mask) {
    TestType value{};
    for (auto bit = std::size_t{enchantum::has_zero_flag<TestType>}; bit < count; ++bit) {
      if (mask & static_cast<std::uint64_t>(values[bit])) {
        value |= values[bit];
      }
    }
    if (static_cast<T>(value) != 0)
      combinations.push_back(value);
  }
  // test for 0
  if (enchantum::has_zero_flag<TestType>)
    combinations.emplace_back();

  SECTION("cast_bitflag(to_string_bitflag(enum)) == enum")
  {
    for (const auto comb : combinations) {
      CHECK(comb == enchantum::cast_bitflag<TestType>(enchantum::to_string_bitflag(comb)));
    }
  }

  SECTION("contains_bitflag(to_string_bitflag(enum))")
  {
    for (const auto comb : combinations) {
      CHECK(enchantum::contains_bitflag<TestType>(enchantum::to_string_bitflag(comb)));
    }
  }

  SECTION("contains_bitflag(enum)")
  {
    for (const auto comb : combinations) {
      CHECK(enchantum::contains_bitflag(comb));
    }
  }

  SECTION("contains_bitflag<E>(std::underlying_type_t<E>(enum))")
  {
    for (const auto comb : combinations) {
      CHECK(enchantum::contains_bitflag<TestType>(static_cast<T>(comb)));
    }
  }

  SECTION("Value ORs")
  {
    const auto s = []() {
      std::string ret;
      for (const auto& [e, s] : enchantum::entries<TestType>) {
        if (T(e) != 0) {
          ret += s;
          if (e != enchantum::max<TestType>)
            ret += '|';
        }
      }
      return ret;
    }();
    CHECK(enchantum::to_string_bitflag(enchantum::value_ors<TestType>) == s);

  }
}

TEMPLATE_LIST_TEST_CASE("contains_bitflag returns false for invalid combinations", "[bitflags]", AllFlagsTestTypes)
{
  using Underlying = std::underlying_type_t<TestType>;

  std::vector<TestType> invalid_combinations;

  for (std::size_t bit = 0; bit < sizeof(Underlying) * CHAR_BIT; ++bit) {
    const auto test_bit = static_cast<TestType>(Underlying{1} << bit);

    if (!static_cast<bool>(enchantum::value_ors<TestType> & test_bit)) {
      // This bit is not part of the valid mask, so it's invalid
      invalid_combinations.push_back(test_bit);
    }
  }

  if constexpr (enchantum::has_zero_flag<TestType>)
    CHECK(enchantum::contains_bitflag(TestType{}));

  for (const auto comb : invalid_combinations) {
    CHECK_FALSE(enchantum::contains_bitflag(comb));
  }
}