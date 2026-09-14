#include "test_utility.hpp"
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
#include <enchantum/nlohmann_json.hpp>
#include <nlohmann/json.hpp>
enum class Error {
  OutOfRange,InvalidValue,NullPointerAccess
};

enum FileFlags {
    File_Read = 1 << 0,
    File_Write = 1 << 1,
    File_Execute = 1 << 2,
    File_All = File_Read | File_Write | File_Execute,
};

template<>
constexpr bool enchantum::is_bitflag<FileFlags> = true;

TEST_CASE("nlohmann json")
{
    {
        nlohmann::json j = Error::InvalidValue;
        CHECK(Error::InvalidValue == j.get<Error>());
    }
    {
        nlohmann::json j = "hi";
        CHECK_THROWS(j.get<Error>());
    }
    {
        nlohmann::json j = 0x42;
        CHECK_THROWS(j.get<Error>());
    }

    {
        nlohmann::json j = File_All;
        CHECK(File_All == j.get<FileFlags>());
    }
    {
        nlohmann::json j = "File_Read|File_Write|File_Execute";
        CHECK(File_All == j.get<FileFlags>());
    }
    {
        nlohmann::json j = "hi";
        CHECK_THROWS(j.get<FileFlags>());
    }
    {
        nlohmann::json j = 0x42;
        CHECK_THROWS(j.get<FileFlags>());
    }
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
