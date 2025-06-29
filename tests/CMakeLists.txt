# In file: tests/CMakeLists.txt

# Use FetchContent to manage dependencies for Catch2 and {fmt}
include(FetchContent)

# --- Catch2 Dependency ---
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.6.0 # Pin to a specific, stable version
)
FetchContent_MakeAvailable(Catch2)
# Include Catch.cmake to make catch_discover_tests available
include(${catch2_SOURCE_DIR}/extras/Catch.cmake)
target_compile_features(Catch2 PRIVATE cxx_std_17)
target_compile_definitions(Catch2 PUBLIC CATCH_CONFIG_ENABLE_OPTIONAL_STRINGMAKER)

# --- {fmt} Dependency ---
# 1. Declare the {fmt} dependency, pinning to a specific version for reproducibility.
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG 10.2.1 # Pin to a specific, stable version
)

# 2. Configure {fmt} to be built as a compiled (not header-only) library.
#    This MUST be done BEFORE FetchContent_MakeAvailable.
set(FMT_HEADER_ONLY OFF CACHE BOOL "Force fmt to build as a compiled library")

# 3. Make the dependency available. This will download {fmt} and prepare the `fmt::fmt` target.
FetchContent_MakeAvailable(fmt)

# --- Precompiled Header (PCH) Configuration ---
# Create a dedicated INTERFACE library to manage the PCH. This is a modern CMake pattern.
add_library(tests_pch INTERFACE)

# Define the PCH file for this target.
# Targets that link to tests_pch will precompile pch.hpp.
target_precompile_headers(tests_pch INTERFACE "pch.hpp")

# --- Main Test Executable ---
add_executable(tests)
target_compile_features(tests PRIVATE cxx_std_20)

# Link the test executable against all its dependencies:
# 1. enchantum: The library being tested.
# 2. Catch2::Catch2WithMain: The testing framework and its main function.
# 3. fmt::fmt: The COMPILED {fmt} library target.
# 4. tests_pch: The interface library that provides the PCH configuration.
target_link_libraries(tests PRIVATE
    enchantum::enchantum
    Catch2::Catch2WithMain
    fmt::fmt
    tests_pch
)

# Register tests with CTest using catch_discover_tests
catch_discover_tests(tests)

# IMPORTANT: Synchronize your target with the compiled fmt library.
# This ensures that when your test files #include <fmt/format.h>, the headers
# are in the correct "compiled mode", matching how you are linking.
target_compile_definitions(tests PRIVATE FMT_HEADER_ONLY=0)

# --- Compiler Flags and Options ---
if(ENCHANTUM_RUNTIME_TESTS)
  message(STATUS "enchantum tests are being ran at runtime")
  target_compile_definitions(tests PRIVATE CATCH_CONFIG_RUNTIME_STATIC_REQUIRE)
endif()

if(MSVC)
  target_compile_options(tests PRIVATE /Za /permissive-)
  target_compile_options(tests PRIVATE /WX /W4)
else()
  # The -Werror flag is removed as per the original PR's intent
  target_compile_options(tests PRIVATE -Wall -Wextra -Wshadow -Wconversion -Wpedantic)
endif()

# --- Source Files ---
# Gather all test source files. The PCH header itself is not included here.
file(GLOB_RECURSE TEST_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp")
target_sources(tests PRIVATE ${TEST_SOURCES})

# Exclude single_header.cpp from PCH because it tests the amalgamated header
set_source_files_properties(
    single_header.cpp
    PROPERTIES SKIP_PRECOMPILE_HEADERS ON
)

# --- Optional Unity Build ---
# This logic is added at the end, controlled by an option in the root CMakeLists.txt
if(ENCHANTUM_ENABLE_UNITY_BUILD)
    set_target_properties(tests PROPERTIES UNITY_BUILD ON)
    message(STATUS "Unity build enabled for tests.")
endif()
