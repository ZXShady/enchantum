include(FetchContent)
FetchContent_Declare( 
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG devel
)
FetchContent_MakeAvailable(Catch2)

# FetchContent_Declare for fmt removed
# set(FMT_HEADER_ONLY OFF) # Not needed with ExternalProject
# FetchContent_MakeAvailable(fmt) # Removed FetchContent for fmt

# ===================================================================
# START: Correct {fmt} Dependency Handling
# ===================================================================

# This ensures the FetchContent module is available.
include(FetchContent)

# 1. Declare the dependency. Using a specific GIT_TAG is best practice.
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG 10.2.1
)

# 2. Configure {fmt} to build its sources.
#    This MUST be set BEFORE FetchContent_MakeAvailable. This tells the
#    {fmt} project itself to build as a compiled library.
set(FMT_HEADER_ONLY OFF CACHE BOOL "Force fmt to build as a compiled library")

# 3. Make the dependency available. CMake will now download and configure
#    the {fmt} project, creating the compiled `fmt::fmt` target.
FetchContent_MakeAvailable(fmt)

target_compile_features(Catch2 PRIVATE cxx_std_17)
target_compile_definitions(Catch2 PUBLIC CATCH_CONFIG_ENABLE_OPTIONAL_STRINGMAKER)
add_executable(tests)

# Add precompiled header support to accelerate test builds.
# This instructs the compiler to process pch.hpp once and reuse the result
# for every source file in the 'tests' target, avoiding redundant parsing
# of large headers like Catch2. fmt is no longer in PCH.
# This is guarded to only apply to compilers known to support PCH.
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC|GNU|Clang")
    target_precompile_headers(tests PRIVATE pch.hpp)
    # Exclude single_header.cpp from PCH because it includes the single-header version of the library,
    # which would conflict with the PCH version of the library.
    set_source_files_properties(single_header.cpp PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
    # Exclude single_header.cpp from Unity builds for the same reason.
    set_source_files_properties(single_header.cpp PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON)
endif()

# Optionally enable Unity builds for the test suite if the user opts in.
# This combines source files to reduce build system overhead and further
# minimizes redundant header parsing, offering a significant speedup for
# full rebuilds at the cost of potentially hiding missing include errors.
if(ENCHANTUM_ENABLE_UNITY_BUILD)
    set_target_properties(tests PROPERTIES UNITY_BUILD ON)
    message(STATUS "Unity build enabled for tests.")
endif()
target_include_directories(tests PRIVATE "third_party") # fmt include dirs will come from fmt::fmt target
target_compile_features(tests PRIVATE cxx_std_20)


# 4. Link your tests against the compiled `fmt::fmt` library target.
#    This provides the compiled function definitions to the linker.
target_link_libraries(tests PRIVATE enchantum::enchantum Catch2::Catch2WithMain fmt::fmt)

# 5. !! CRITICAL STEP !!
#    Define FMT_HEADER_ONLY=0 for your OWN target. This tells the compiler
#    that when it processes `#include <fmt/format.h>` inside your test files,
#    it should expect the declarations for a compiled library. This synchronizes
#    your target with the library you are linking against.
target_compile_definitions(tests PRIVATE FMT_HEADER_ONLY=0)

# ===================================================================
# END: Correct {fmt} Dependency Handling
# ===================================================================

if(ENCHANTUM_RUNTIME_TESTS)
  message(STATUS "enchantum tests are being ran at runtime")
  target_compile_definitions(tests PRIVATE CATCH_CONFIG_RUNTIME_STATIC_REQUIRE)
endif()

if(MSVC)
  target_compile_options(tests PRIVATE /Za /permissive-)
  target_compile_options(tests PRIVATE /WX /W4)
else()
  target_compile_options(tests PRIVATE -Wall -Wextra -Wshadow -Wconversion -Wpedantic) # Removed -Werror
endif()

file(GLOB_RECURSE SRCS "*.cpp" "*.hpp")


if(MSVC)
set(CMAKE_REQUIRED_FLAGS "/std:c++20")
else()
set(CMAKE_REQUIRED_FLAGS "-std=c++20")
endif()
check_cxx_source_compiles("
    #include <format>
    int main() {
        auto s = std::format(\"{}\",42);
        (void)s;
        return 0;
    }" 
    HAS_STD_FORMAT
)

if(NOT HAS_STD_FORMAT)
    message(STATUS "This compiler does not support <format> header not running std_format.cpp tests")
	list(REMOVE_ITEM SRCS "${CMAKE_CURRENT_SOURCE_DIR}/std_format.cpp")
endif()

target_sources(tests PRIVATE ${SRCS})

include(CTest)
include(Catch)
catch_discover_tests(tests)
