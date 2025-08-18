#include "test_utility.hpp"
#include <algorithm>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <enchantum/enchantum.hpp>
#include <enchantum/generators.hpp>


template<typename T>
auto names_of = []() { static_assert(0 == sizeof(T)); };

template<>
std::array<std::string_view, 5> names_of<Color> = {"Aqua", "Purple", "Green", "Red", "Blue"};
template<>
std::array<std::string_view, 7> names_of<Flags> = {"Flag0", "Flag1", "Flag2", "Flag3", "Flag4", "Flag5", "Flag6"};
template<>
std::array<std::string_view, 5> names_of<UnscopedColor> = {"Aqua", "Purple", "Green", "Red", "Blue"};
template<>
std::array<std::string_view, 3> names_of<BlockType> = {"Dirt", "Stone", "Obsidian"};
template<>
std::array<std::string_view, 7> names_of<StrongFlagsNoOverloadedOperators> =
  {"Flag0", "Flag1", "Flag2", "Flag3", "Flag4", "Flag5", "Flag6"};
template<>
std::array<std::string_view, 10> names_of<ImGuiFreeTypeBuilderFlags> =
  {"NoHinting",
   "NoAutoHint",
   "ForceAutoHint",
   "LightHinting",
   "MonoHinting",
   "Bold",
   "Oblique",
   "Monochrome",
   "LoadColor",
   "Bitmap"};
template<>
std::array<std::string_view, 10> names_of<ContigNonZero> = {"_0", "_1", "_2", "_3", "_4", "_5", "_6", "_7", "_8", "_9"};
template<>
std::array<std::string_view, 10> names_of<ContigNonZeroCStyle> =
  {"ContigNonZeroCStyle_0",
   "ContigNonZeroCStyle_1",
   "ContigNonZeroCStyle_2",
   "ContigNonZeroCStyle_3",
   "ContigNonZeroCStyle_4",
   "ContigNonZeroCStyle_5",
   "ContigNonZeroCStyle_6",
   "ContigNonZeroCStyle_7",
   "ContigNonZeroCStyle_8",
   "ContigNonZeroCStyle_9"};
template<>
std::array<std::string_view, 10> names_of<ContigNonZeroStartWith5CStyle> =
  {"ContigNonZeroStartWith5CStyle_0",
   "ContigNonZeroStartWith5CStyle_1",
   "ContigNonZeroStartWith5CStyle_2",
   "ContigNonZeroStartWith5CStyle_3",
   "ContigNonZeroStartWith5CStyle_4",
   "ContigNonZeroStartWith5CStyle_5",
   "ContigNonZeroStartWith5CStyle_6",
   "ContigNonZeroStartWith5CStyle_7",
   "ContigNonZeroStartWith5CStyle_8",
   "ContigNonZeroStartWith5CStyle_9"};
template<>
std::array<std::string_view, 10> names_of<UnderlyingTypeWChar_t> =
  {"_0", "_1", "_2", "_3", "_4", "_5", "_6", "_7", "_8", "_9"};
#ifdef __cpp_char8_t
template<>
std::array<std::string_view, 10> names_of<UnderlyingTypeChar8_t> =
  {"_0", "_1", "_2", "_3", "_4", "_5", "_6", "_7", "_8", "_9"};
#endif
template<>
std::array<std::string_view, 10> names_of<UnderlyingTypeChar16_t> =
  {"_0", "_1", "_2", "_3", "_4", "_5", "_6", "_7", "_8", "_9"};
template<>
std::array<std::string_view, 10> names_of<UnderlyingTypeChar32_t> =
  {"_0", "_1", "_2", "_3", "_4", "_5", "_6", "_7", "_8", "_9"};
template<>
std::array<std::string_view, 26> names_of<Letters> = {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m",
                                                      "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z"};
template<>
std::array<std::string_view, 5> names_of<NonContigFlagsWithNoneCStyle> = {"None", "Flag0", "Flag1", "Flag2", "Flag6"};
template<>
std::array<std::string_view, 9> names_of<FlagsWithNone> =
  {"None", "Flag0", "Flag1", "Flag2", "Flag3", "Flag4", "Flag5", "Flag6", "Flag7"};
template<>
std::array<std::string_view, 5> names_of<Direction2D> = {"None", "Left", "Up", "Right", "Down"};
template<>
std::array<std::string_view, 7> names_of<Direction3D> = {"None", "Left", "Up", "Right", "Down", "Front", "Back"};
template<>
std::array<std::string_view, 7> names_of<CStyleFromA_To_G> = {"a", "b", "c", "e", "d", "f", "g"};
template<>
std::array<std::string_view, 2> names_of<MinMaxValues> = {"min", "max"};
template<>
std::array<std::string_view, 2> names_of<MinMaxValuesCStyle> = {"MinMaxValuesCStyle_min", "MinMaxValuesCStyle_max"};

template<>
std::array<std::string_view, 7> names_of<Outer::Inner::Anon> = {"_0", "_1", "_2", "_3", "_4", "_5", "_6"};
template<>
std::array<std::string_view, 7> names_of<Outer::Inner::CStyleAnon> =
  {"CStyleAnon_0", "CStyleAnon_1", "CStyleAnon_2", "CStyleAnon_3", "CStyleAnon_4", "CStyleAnon_5", "CStyleAnon_6"};
template<>
std::array<std::string_view, 5> names_of<UnscopedCStyle> =
  {"Unscoped_CStyle_Val0",
   "Unscoped_CStyle_Value1",
   "Unscoped_CStyle_Value4",
   "Unscoped_CStyle_Value3",
   "Unscoped_CStyle_Value2"};
template<>
std::array<std::string_view, 2> names_of<BoolEnum> = {"False", "True"};
template<>
std::array<std::string_view, 2> names_of<BoolEnumCStyle> = {"BoolEnumCStyle_False", "BoolEnumCStyle_True"};


template<typename T>
auto values_of = []() { static_assert(0 == sizeof(T)); };

template<>
auto values_of<Color> = std::array{Color::Aqua, Color::Purple, Color::Green, Color::Red, Color::Blue};
template<>
auto values_of<Flags> = std::array{Flags::Flag0, Flags::Flag1, Flags::Flag2, Flags::Flag3, Flags::Flag4, Flags::Flag5, Flags::Flag6};
template<>
auto values_of<UnscopedColor> = std::array{UnscopedColor::Aqua,
                                           UnscopedColor::Purple,
                                           UnscopedColor::Green,
                                           UnscopedColor::Red,
                                           UnscopedColor::Blue};
template<>
auto values_of<BlockType> = std::array{BlockType::Dirt, BlockType::Stone, BlockType::Obsidian};
template<>
auto values_of<StrongFlagsNoOverloadedOperators> = std::array{StrongFlagsNoOverloadedOperators::Flag0,
                                                              StrongFlagsNoOverloadedOperators::Flag1,
                                                              StrongFlagsNoOverloadedOperators::Flag2,
                                                              StrongFlagsNoOverloadedOperators::Flag3,
                                                              StrongFlagsNoOverloadedOperators::Flag4,
                                                              StrongFlagsNoOverloadedOperators::Flag5,
                                                              StrongFlagsNoOverloadedOperators::Flag6};
template<>
auto values_of<ImGuiFreeTypeBuilderFlags> = std::array{ImGuiFreeTypeBuilderFlags_NoHinting,
                                                       ImGuiFreeTypeBuilderFlags_NoAutoHint,
                                                       ImGuiFreeTypeBuilderFlags_ForceAutoHint,
                                                       ImGuiFreeTypeBuilderFlags_LightHinting,
                                                       ImGuiFreeTypeBuilderFlags_MonoHinting,
                                                       ImGuiFreeTypeBuilderFlags_Bold,
                                                       ImGuiFreeTypeBuilderFlags_Oblique,
                                                       ImGuiFreeTypeBuilderFlags_Monochrome,
                                                       ImGuiFreeTypeBuilderFlags_LoadColor,
                                                       ImGuiFreeTypeBuilderFlags_Bitmap};

template<>
auto values_of<ContigNonZero> = std::array{ContigNonZero::_0,
                                           ContigNonZero::_1,
                                           ContigNonZero::_2,
                                           ContigNonZero::_3,
                                           ContigNonZero::_4,
                                           ContigNonZero::_5,
                                           ContigNonZero::_6,
                                           ContigNonZero::_7,
                                           ContigNonZero::_8,
                                           ContigNonZero::_9};
template<>
auto values_of<ContigNonZeroCStyle> = std::array{ContigNonZeroCStyle_0,
                                                 ContigNonZeroCStyle_1,
                                                 ContigNonZeroCStyle_2,
                                                 ContigNonZeroCStyle_3,
                                                 ContigNonZeroCStyle_4,
                                                 ContigNonZeroCStyle_5,
                                                 ContigNonZeroCStyle_6,
                                                 ContigNonZeroCStyle_7,
                                                 ContigNonZeroCStyle_8,
                                                 ContigNonZeroCStyle_9};
template<>
auto values_of<ContigNonZeroStartWith5CStyle> = std::array{ContigNonZeroStartWith5CStyle_0,
                                                           ContigNonZeroStartWith5CStyle_1,
                                                           ContigNonZeroStartWith5CStyle_2,
                                                           ContigNonZeroStartWith5CStyle_3,
                                                           ContigNonZeroStartWith5CStyle_4,
                                                           ContigNonZeroStartWith5CStyle_5,
                                                           ContigNonZeroStartWith5CStyle_6,
                                                           ContigNonZeroStartWith5CStyle_7,
                                                           ContigNonZeroStartWith5CStyle_8,
                                                           ContigNonZeroStartWith5CStyle_9};
template<>
auto values_of<UnderlyingTypeWChar_t> = std::array{UnderlyingTypeWChar_t::_0,
                                                   UnderlyingTypeWChar_t::_1,
                                                   UnderlyingTypeWChar_t::_2,
                                                   UnderlyingTypeWChar_t::_3,
                                                   UnderlyingTypeWChar_t::_4,
                                                   UnderlyingTypeWChar_t::_5,
                                                   UnderlyingTypeWChar_t::_6,
                                                   UnderlyingTypeWChar_t::_7,
                                                   UnderlyingTypeWChar_t::_8,
                                                   UnderlyingTypeWChar_t::_9};
#ifdef __cpp_char8_t
template<>
auto values_of<UnderlyingTypeChar8_t> = std::array{UnderlyingTypeChar8_t::_0,
                                                   UnderlyingTypeChar8_t::_1,
                                                   UnderlyingTypeChar8_t::_2,
                                                   UnderlyingTypeChar8_t::_3,
                                                   UnderlyingTypeChar8_t::_4,
                                                   UnderlyingTypeChar8_t::_5,
                                                   UnderlyingTypeChar8_t::_6,
                                                   UnderlyingTypeChar8_t::_7,
                                                   UnderlyingTypeChar8_t::_8,
                                                   UnderlyingTypeChar8_t::_9};

#endif
template<>
auto values_of<UnderlyingTypeChar16_t> = std::array{UnderlyingTypeChar16_t::_0,
                                                    UnderlyingTypeChar16_t::_1,
                                                    UnderlyingTypeChar16_t::_2,
                                                    UnderlyingTypeChar16_t::_3,
                                                    UnderlyingTypeChar16_t::_4,
                                                    UnderlyingTypeChar16_t::_5,
                                                    UnderlyingTypeChar16_t::_6,
                                                    UnderlyingTypeChar16_t::_7,
                                                    UnderlyingTypeChar16_t::_8,
                                                    UnderlyingTypeChar16_t::_9};
template<>
auto values_of<UnderlyingTypeChar32_t> = std::array{UnderlyingTypeChar32_t::_0,
                                                    UnderlyingTypeChar32_t::_1,
                                                    UnderlyingTypeChar32_t::_2,
                                                    UnderlyingTypeChar32_t::_3,
                                                    UnderlyingTypeChar32_t::_4,
                                                    UnderlyingTypeChar32_t::_5,
                                                    UnderlyingTypeChar32_t::_6,
                                                    UnderlyingTypeChar32_t::_7,
                                                    UnderlyingTypeChar32_t::_8,
                                                    UnderlyingTypeChar32_t::_9};

template<>
auto values_of<Letters> = std::array{Letters::a, Letters::b, Letters::c, Letters::d, Letters::e, Letters::f, Letters::g,
                                     Letters::h, Letters::i, Letters::j, Letters::k, Letters::l, Letters::m, Letters::n,
                                     Letters::o, Letters::p, Letters::q, Letters::r, Letters::s, Letters::t, Letters::u,
                                     Letters::v, Letters::w, Letters::x, Letters::y, Letters::z};
template<>
auto values_of<NonContigFlagsWithNoneCStyle> = std::array{NonContigFlagsWithNoneCStyle::None,
                                                          NonContigFlagsWithNoneCStyle::Flag0,
                                                          NonContigFlagsWithNoneCStyle::Flag1,
                                                          NonContigFlagsWithNoneCStyle::Flag2,
                                                          NonContigFlagsWithNoneCStyle::Flag6};
template<>
auto values_of<FlagsWithNone> = std::array{FlagsWithNone::None,
                                           FlagsWithNone::Flag0,
                                           FlagsWithNone::Flag1,
                                           FlagsWithNone::Flag2,
                                           FlagsWithNone::Flag3,
                                           FlagsWithNone::Flag4,
                                           FlagsWithNone::Flag5,
                                           FlagsWithNone::Flag6,
                                           FlagsWithNone::Flag7};
template<>
auto values_of<Direction2D> = std::array{Direction2D::None, Direction2D::Left, Direction2D::Up, Direction2D::Right, Direction2D::Down};
template<>
auto values_of<Direction3D> = std::array{Direction3D::None,
                                         Direction3D::Left,
                                         Direction3D::Up,
                                         Direction3D::Right,
                                         Direction3D::Down,
                                         Direction3D::Front,
                                         Direction3D::Back};
template<>
auto values_of<CStyleFromA_To_G> = std::array{CStyleFromA_To_G::a,
                                              CStyleFromA_To_G::b,
                                              CStyleFromA_To_G::c,
                                              CStyleFromA_To_G::e,
                                              CStyleFromA_To_G::d,
                                              CStyleFromA_To_G::f,
                                              CStyleFromA_To_G::g};
template<>
auto values_of<MinMaxValues> = std::array{MinMaxValues::min, MinMaxValues::max};
template<>
auto values_of<MinMaxValuesCStyle> = std::array{MinMaxValuesCStyle_min, MinMaxValuesCStyle_max};

template<>
auto values_of<Outer::Inner::Anon> = std::array{Outer::Inner::Anon::_0,
                                                Outer::Inner::Anon::_1,
                                                Outer::Inner::Anon::_2,
                                                Outer::Inner::Anon::_3,
                                                Outer::Inner::Anon::_4,
                                                Outer::Inner::Anon::_5,
                                                Outer::Inner::Anon::_6};
template<>
auto values_of<Outer::Inner::CStyleAnon> = std::array{Outer::Inner::CStyleAnon_0,
                                                      Outer::Inner::CStyleAnon_1,
                                                      Outer::Inner::CStyleAnon_2,
                                                      Outer::Inner::CStyleAnon_3,
                                                      Outer::Inner::CStyleAnon_4,
                                                      Outer::Inner::CStyleAnon_5,
                                                      Outer::Inner::CStyleAnon_6};
template<>
auto values_of<UnscopedCStyle> = std::array{Unscoped_CStyle_Val0,
                                            Unscoped_CStyle_Value1,
                                            Unscoped_CStyle_Value4,
                                            Unscoped_CStyle_Value3,
                                            Unscoped_CStyle_Value2};
template<>
auto values_of<BoolEnum> = std::array{BoolEnum::False, BoolEnum::True};
template<>
auto values_of<BoolEnumCStyle> = std::array{BoolEnumCStyle_False, BoolEnumCStyle_True};


TEMPLATE_LIST_TEST_CASE("array size checks", "[constants]", AllEnumsTestTypes)
{
  constexpr auto count = enchantum::count<TestType>;
  //constexpr auto  min     = enchantum::min<TestType>;
  //constexpr auto  max     = enchantum::max<TestType>;
  constexpr auto& names   = enchantum::names<TestType>;
  constexpr auto& values  = enchantum::values<TestType>;
  constexpr auto& entries = enchantum::entries<TestType>;


  STATIC_CHECK(count == entries.size());
  STATIC_CHECK(names.size() == entries.size());
  STATIC_CHECK(values.size() == entries.size());

  SECTION("names<E> and values<E> equal to entries<E>")
  {
    for (std::size_t i = 0; i < count; ++i) {
      CHECK(values[i] == entries[i].first);
      CHECK(names[i] == entries[i].second);
    }
  }

  SECTION("entries_generator<E> and entries<E> are equal")
  {
    for (std::size_t i = 0; i < count; ++i) {
      CHECK(enchantum::entries_generator<TestType>[i] == entries[i]);
    }
  }

  SECTION("names_generator<E> and names<E> are equal")
  {
    for (std::size_t i = 0; i < count; ++i) {
      CHECK(enchantum::names_generator<TestType>[i] == names[i]);
    }
  }

  SECTION("values_generator<E> and values<E> are equal")
  {
    for (std::size_t i = 0; i < count; ++i) {
      CHECK(enchantum::values_generator<TestType>[i] == values[i]);
    }
  }

  SECTION("enum_to_index identities")
  {
    for (std::size_t i = 0; i < count; ++i) {
      CHECK(i == enchantum::enum_to_index(values[i]));
    }
  }

  SECTION("index_to_enum identities")
  {
    for (std::size_t i = 0; i < count; ++i) {
      CHECK(enchantum::index_to_enum<TestType>(i) == values[i]);
      CHECK_FALSE(enchantum::index_to_enum<TestType>(i + count).has_value());
    }
  }

  SECTION("cast(to_string()) identities")
  {
    for (std::size_t i = 0; i < count; ++i) {
      CHECK(values[i] == enchantum::cast<TestType>(enchantum::to_string(values[i])));
    }
  }

  SECTION("expected values")
  {
    auto comp = [](auto x, auto y) {
      CAPTURE(enchantum::raw_type_name<decltype(x)>);
      CAPTURE(enchantum::raw_type_name<decltype(y)>);
      CAPTURE(enchantum::raw_type_name<TestType>);


      CHECK(typeid(x[0]) == typeid(x[1]));

      STATIC_CHECK(std::is_same_v<std::decay_t<decltype(x[0])>, std::decay_t<decltype(y[0])>>);


      REQUIRE(x.size() == y.size());

      for (std::size_t i = 0; i < x.size(); ++i)
        CHECK(x[i] == y[i]);
    };
    comp(values_of<TestType>, enchantum::values<TestType>);
    comp(values_of<TestType>, enchantum::values_generator<TestType>);
  }

  SECTION("expected names")
  {
    auto comp = [](auto x, auto y) {
      CAPTURE(enchantum::raw_type_name<decltype(x)>);
      CAPTURE(enchantum::raw_type_name<decltype(y)>);
      CAPTURE(enchantum::raw_type_name<TestType>);

      REQUIRE(x.size() == y.size());

      STATIC_CHECK(std::is_same_v<std::decay_t<decltype(x[0])>, std::decay_t<decltype(y[0])>>);

      for (std::size_t i = 0; i < x.size(); ++i)
        CHECK(x[i] == y[i]);
    };
    comp(names_of<TestType>, enchantum::names<TestType>);
    comp(names_of<TestType>, enchantum::names_generator<TestType>);
  }
}

TEST_CASE("Color enum to_string", "[to_string]")
{
  STATIC_CHECK(enchantum::to_string(Color::Green) == "Green");
  STATIC_CHECK(enchantum::to_string(Color::Red) == "Red");
  STATIC_CHECK(enchantum::to_string(Color::Blue) == "Blue");
  STATIC_CHECK(enchantum::to_string(Color::Purple) == "Purple");
  STATIC_CHECK(enchantum::to_string(Color::Aqua) == "Aqua");
}

TEST_CASE("Color enum cast", "[cast]")
{
  STATIC_CHECK(enchantum::cast<Color>("Green") == Color::Green);
  STATIC_CHECK(enchantum::cast<Color>("Red") == Color::Red);
  STATIC_CHECK(enchantum::cast<Color>("Blue") == Color::Blue);
  STATIC_CHECK(enchantum::cast<Color>("Purple") == Color::Purple);
  STATIC_CHECK(enchantum::cast<Color>("Aqua") == Color::Aqua);

  STATIC_CHECK_FALSE(enchantum::cast<Color>("ZXShady"));
  STATIC_CHECK_FALSE(enchantum::cast<Color>("red"));   // case-sensitive
  STATIC_CHECK_FALSE(enchantum::cast<Color>("GREEN")); // all caps
}

TEST_CASE("Color enum min/max", "[range][min_max]")
{
  STATIC_CHECK(enchantum::min<Color> == Color::Aqua);
  STATIC_CHECK(enchantum::max<Color> == Color::Blue);

  STATIC_CHECK(static_cast<std::underlying_type_t<Color>>(enchantum::min<Color>) == -42);
  STATIC_CHECK(static_cast<std::underlying_type_t<Color>>(enchantum::max<Color>) == 214);
}

TEST_CASE("Color enum cast from underlying type", "[cast]")
{
  using enchantum::cast;
  using T = std::underlying_type_t<Color>;
  STATIC_CHECK(enchantum::cast<Color>(T(Color::Green)) == Color::Green);
  STATIC_CHECK(enchantum::cast<Color>(T(Color::Red)) == Color::Red);
  STATIC_CHECK(enchantum::cast<Color>(T(Color::Blue)) == Color::Blue);
  STATIC_CHECK(enchantum::cast<Color>(T(Color::Purple)) == Color::Purple);
  STATIC_CHECK(enchantum::cast<Color>(T(Color::Aqua)) == Color::Aqua);
  STATIC_CHECK_FALSE(enchantum::cast<Color>(T(2138)));
}

TEST_CASE("NonContigFlagsWithNoneCStyle contains", "[contains]")
{
  STATIC_CHECK_FALSE(enchantum::contains(NonContigFlagsWithNoneCStyle(1 << 3)));
  STATIC_CHECK_FALSE(enchantum::contains(NonContigFlagsWithNoneCStyle(1 << 4)));
  STATIC_CHECK_FALSE(enchantum::contains(NonContigFlagsWithNoneCStyle(1 << 5)));
  STATIC_CHECK_FALSE(enchantum::contains(NonContigFlagsWithNoneCStyle(1 << 7)));
}


TEST_CASE("Color enum cast from string_view", "[cast]")
{
  STATIC_CHECK(enchantum::cast<Color>("Green") == Color::Green);
  STATIC_CHECK(enchantum::cast<Color>("Red") == Color::Red);
  STATIC_CHECK(enchantum::cast<Color>("Blue") == Color::Blue);
  STATIC_CHECK(enchantum::cast<Color>("Purple") == Color::Purple);
  STATIC_CHECK(enchantum::cast<Color>("Aqua") == Color::Aqua);

  STATIC_CHECK_FALSE(enchantum::cast<Color>("Chartreuse"));
  STATIC_CHECK_FALSE(enchantum::cast<Color>("BLUE"));
}


TEST_CASE("Color enum cast with custom binary predicate (case insensitive)", "[cast]")
{
  STATIC_CHECK(enchantum::cast<Color>("green", case_insensitive) == Color::Green);
  STATIC_CHECK(enchantum::cast<Color>("RED", case_insensitive) == Color::Red);
  STATIC_CHECK(enchantum::cast<Color>("bLuE", case_insensitive) == Color::Blue);
  STATIC_CHECK(enchantum::cast<Color>("purple", case_insensitive) == Color::Purple);
  STATIC_CHECK(enchantum::cast<Color>("AQUA", case_insensitive) == Color::Aqua);
  STATIC_CHECK_FALSE(enchantum::cast<Color>("zxSHADY", case_insensitive));
}

TEST_CASE("Color enum index_to_enum", "[index_to_enum]")
{
  STATIC_CHECK(enchantum::index_to_enum<Color>(0) == Color::Aqua);
  STATIC_CHECK(enchantum::index_to_enum<Color>(1) == Color::Purple);
  STATIC_CHECK(enchantum::index_to_enum<Color>(2) == Color::Green);
  STATIC_CHECK(enchantum::index_to_enum<Color>(3) == Color::Red);
  STATIC_CHECK(enchantum::index_to_enum<Color>(4) == Color::Blue);
}

TEST_CASE("count", "[count]")
{
  STATIC_CHECK(enchantum::count<StrongFlagsNoOverloadedOperators> == 7);
  STATIC_CHECK(enchantum::count<ImGuiFreeTypeBuilderFlags> == 10);
  STATIC_CHECK(enchantum::count<NonContigFlagsWithNoneCStyle> == 5);
  STATIC_CHECK(enchantum::count<FlagsWithNone> == 9);
  STATIC_CHECK(enchantum::count<Flags> == 7);
  STATIC_CHECK(enchantum::count<UnderlyingTypeChar8_t> == 10);
  STATIC_CHECK(enchantum::count<UnderlyingTypeChar16_t> == 10);
  STATIC_CHECK(enchantum::count<UnderlyingTypeChar32_t> == 10);
  STATIC_CHECK(enchantum::count<UnderlyingTypeWChar_t> == 10);

  STATIC_CHECK(enchantum::count<MinMaxValuesCStyle> == 2);

  STATIC_CHECK(enchantum::count<BlockType> == 3);

  STATIC_CHECK(enchantum::count<ContigNonZeroStartWith5CStyle> == 10);

  STATIC_CHECK(enchantum::count<BoolEnumCStyle> == 2);
  STATIC_CHECK(enchantum::count<BoolEnum> == 2);

  STATIC_CHECK(enchantum::count<ContigNonZeroCStyle> == 10);

  STATIC_CHECK(enchantum::count<ContigNonZero> == 10);

  STATIC_CHECK(enchantum::count<MinMaxValues> == 2);
  STATIC_CHECK(enchantum::count<Color> == 5);
  STATIC_CHECK(enchantum::count<UnscopedColor> == 5);

  STATIC_CHECK(enchantum::count<UnscopedCStyle> == 5);

  STATIC_CHECK(enchantum::count<Outer::Inner::Anon> == 7);
  STATIC_CHECK(enchantum::count<Outer::Inner::CStyleAnon> == 7);

  STATIC_CHECK(enchantum::count<Letters> == 26);
  STATIC_CHECK(enchantum::count<CStyleFromA_To_G> == 7);
  STATIC_CHECK(enchantum::count<Direction2D> == 5);
  STATIC_CHECK(enchantum::count<Direction3D> == 7);
}
