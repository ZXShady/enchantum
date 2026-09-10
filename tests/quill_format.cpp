
#include <enchantum/bitflags.hpp>
#include "test_utility.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#if defined(__clang_major__) && (__clang_major__ == 9 || __clang_major__ == 11)
#include <enchantum/details/format_util.hpp>

template <typename E>
static std::string quill_fmt(E e)
{
  return enchantum::details::format(e);
}
#else
#if defined(__GNUC__) || defined(__clang__)
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wsign-conversion"
  #pragma GCC diagnostic ignored "-Wshadow"
#elif defined(_MSC_VER)
  #pragma warning(push)
#endif
#include <enchantum/quill_format.hpp>
#include <quill/Logger.h>
#include <quill/SimpleSetup.h>
#include <quill/LogMacros.h>

namespace {
  struct StringSink : quill::Sink {
    static void Init() 
    {
        // if I did not do this it would hang
  static char init_once=(quill::Backend::start(),char());
  static_cast<void>(init_once);
    }
    void flush_sink() override {}
    void write_log(quill::MacroMetadata const* /* log_metadata */,std::uint64_t /* log_timestamp */, std::string_view /* thread_id */,
                                     std::string_view /* thread_name */, std::string const& /* process_id */,
                                     std::string_view /* logger_name */, quill::LogLevel /* log_level */,
                                     std::string_view /* log_level_description */,
                                     std::string_view /* log_level_short_code */,
                                     std::vector<std::pair<std::string, std::string>> const* /* named_args */,
                                     std::string_view /* log_message */, std::string_view log_statement) override
    {
      str += log_statement;
    }
    std::string str;
  }; 
}

template <typename E>
static std::string quill_fmt(E e)
{
    StringSink::Init();
    auto sink = quill::Frontend::create_or_get_sink<StringSink>("temp");
    auto logger = quill::Frontend::create_or_get_logger("temp", sink,quill::PatternFormatterOptions{"%(message)"});


    auto& strsink = static_cast<StringSink&>(*sink);
    strsink.str.clear();
    LOG_INFO(logger, "{}", e);
    logger->flush_log(0);
    // quill appends a newline, so removei t
    if (!strsink.str.empty() && strsink.str.back() == '\n')
        strsink.str.pop_back();
    return strsink.str;
}

#if defined(__GNUC__) || defined(__clang__)
#    pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#    pragma warning(pop)
#endif
#endif

TEMPLATE_LIST_TEST_CASE("quill::format", "[stringify][quill_format]", AllEnumsTestTypes)
{
  for (const auto& [value, string] : enchantum::entries<TestType>)
    CHECK(quill_fmt(value) == string);
  using T = std::underlying_type_t<TestType>;
  if constexpr (!enchantum::is_bitflag<TestType> && !std::is_same_v<bool, std::underlying_type_t<TestType>>)
    if constexpr (123 <= T(enchantum::max<TestType>) && !enchantum::contains<TestType>(123))
      CHECK("123" == quill_fmt(TestType(123)));
}


TEST_CASE("Color enum quill::format", "[stringify][quill_format]")
{
  CHECK(quill_fmt(Color::Green) == "Green");
  CHECK(quill_fmt(Color::Red) == "Red");
  CHECK(quill_fmt(Color::Blue) == "Blue");
  CHECK(quill_fmt(Color::Purple) == "Purple");
  CHECK(quill_fmt(Color::Aqua) == "Aqua");
}

TEST_CASE("UnscopedColor enum quill::format", "[stringify][quill_format]")
{
  CHECK(quill_fmt(UnscopedColor::Green) == "Green");
  CHECK(quill_fmt(UnscopedColor::Red) == "Red");
  CHECK(quill_fmt(UnscopedColor::Blue) == "Blue");
  CHECK(quill_fmt(UnscopedColor::Purple) == "Purple");
  CHECK(quill_fmt(UnscopedColor::Aqua) == "Aqua");
}

TEST_CASE("Flags enum quill::format", "[stringify][fmt_format]")
{
  SECTION("Normal quill::format")
  {
    CHECK(quill_fmt( Flags::Flag0) == "Flag0");
    CHECK(quill_fmt( Flags::Flag1) == "Flag1");
    CHECK(quill_fmt( Flags::Flag2) == "Flag2");
    CHECK(quill_fmt( Flags::Flag3) == "Flag3");
    CHECK(quill_fmt( Flags::Flag4) == "Flag4");
  }
  SECTION("quill::format with enchantum::to_string_bitflag")
  {
    CHECK(quill_fmt( Flags::Flag0 | Flags::Flag4) == "Flag0|Flag4");
    CHECK(quill_fmt( Flags::Flag0 | Flags::Flag4 | Flags(200)) == "217");
    CHECK(quill_fmt( enchantum::value_ors<Flags>) == "Flag0|Flag1|Flag2|Flag3|Flag4|Flag5|Flag6");
  }
}
