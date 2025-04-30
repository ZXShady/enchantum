#include "test_utility.hpp"
#include <catch2/catch_test_macros.hpp>
#include <enchantum/array.hpp>

enum class Country : std::int16_t {
  Brandon = -6,
  London = -3,
  French,
  Earth,
  Asia,
  Europe     = 8,
  LeInternet = 12,
  Atlantis,
};


TEST_CASE("enum array", "[containers][array]")
{
  enchantum::array<Country, std::string> array;

  array[Country::London]     = "Capital, not a country";
  array[Country::French]     = "Le French";
  array[Country::Brandon]    = "Wait who's brandon?";
  array[Country::Earth]      = "Not in the UN yet";
  array[Country::Asia]       = "Anime";
  array[Country::Europe]     = "The great country of Europe";
  array[Country::LeInternet] = "Online-only country";
  array[Country::Atlantis]   = "Underwater with Posidon";
  CHECK(array[Country::London] == "Capital, not a country");
  CHECK(array[Country::French] == "Le French");
  CHECK(array[Country::Brandon] == "Wait who's brandon?");
  CHECK(array[Country::Earth] == "Not in the UN yet");
  CHECK(array[Country::Asia] == "Anime");
  CHECK(array[Country::Europe] == "The great country of Europe");
  CHECK(array[Country::LeInternet] == "Online-only country");
  CHECK(array[Country::Atlantis] == "Underwater with Posidon");
  CHECK(array.size() == 8);
  CHECK(array.size() == enchantum::count<decltype(array)::index_type>);
  
  CHECK_THROWS_AS(array.at(static_cast<Country>(0xdead / 2)), std::out_of_range);


  const enchantum::array<Country, std::string> array2 = {{
    "Wait who's brandon?",
    "Capital, not a country",
    "Le French",
    "Not in the UN yet",
    "Anime",
    "The great country of Europe",
    "Online-only country",
    "Underwater with Posidon",
  }};
  CHECK(array == array2);
  CHECK_FALSE(array != array2);
}