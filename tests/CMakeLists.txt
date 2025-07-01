# In file: tests/CMakeLists.txt

# ------------------------------------------------------------------
# PHASE 1: DEPENDENCY MANAGEMENT
# ------------------------------------------------------------------
include(FetchContent)

# --- Catch2 Dependency ---
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.6.0
)
FetchContent_MakeAvailable(Catch2)

# --- {fmt} Dependency ---
# The vendored copy has been removed. We now fetch {fmt} cleanly.
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG 10.2.1
)
# Configure {fmt} to be built as a compiled (not header-only) library.
set(FMT_HEADER_ONLY OFF CACHE BOOL "Force fmt to build as a compiled library")
FetchContent_MakeAvailable(fmt)

# ------------------------------------------------------------------
# PHASE 2: PCH and UNITY BUILD SETUP
# ------------------------------------------------------------------
# Create a dedicated INTERFACE library to manage the PCH. This is a modern pattern.
add_library(tests_pch INTERFACE)
target_precompile_headers(tests_pch INTERFACE "pch.hpp")

# The logic to enable Unity builds is here.
# The option itself should be in the root CMakeLists.txt
set(ENCHANTUM_UNITY_BUILD_PROPERTY "") # Initialize to empty
if(ENCHANTUM_ENABLE_UNITY_BUILD)
    set(ENCHANTUM_UNITY_BUILD_PROPERTY UNITY_BUILD ON)
    message(STATUS "Unity build enabled for tests.")
endif()

# ------------------------------------------------------------------
# PHASE 3: TEST EXECUTABLE TARGET
# ------------------------------------------------------------------
add_executable(tests)
if(ENCHANTUM_ENABLE_UNITY_BUILD)
  set_target_properties(tests PROPERTIES ${ENCHANTUM_UNITY_BUILD_PROPERTY})
endif()
target_compile_features(tests PRIVATE cxx_std_20)

# --- Link Dependencies ---
# Link against the library, testing frameworks, and our PCH helper.
target_link_libraries(tests PRIVATE
    enchantum::enchantum
    Catch2::Catch2WithMain
    fmt::fmt # Link the compiled {fmt} library
    tests_pch
)

# --- Synchronize Compile Definitions ---
# This ensures our code and the {fmt} headers agree on the compiled mode.
target_compile_definitions(tests PRIVATE FMT_HEADER_ONLY=0)

if(ENCHANTUM_RUNTIME_TESTS)
  target_compile_definitions(tests PRIVATE CATCH_CONFIG_RUNTIME_STATIC_REQUIRE)
endif()

# --- Set Compiler Flags ---
if(MSVC)
  target_compile_options(tests PRIVATE /Za /permissive- /WX /W4)
else()
  target_compile_options(tests PRIVATE -Wall -Wextra -Wshadow -Wconversion -Wpedantic)
endif()

# ------------------------------------------------------------------
# PHASE 4: SOURCE FILE MANAGEMENT (NO GLOB)
# ------------------------------------------------------------------
# Explicitly list all test sources. This replaces the unreliable file(GLOB).
target_sources(tests PRIVATE
    algorithms.cpp
    array.cpp
    bitflags.cpp
    bitset.cpp
    concepts.cpp
    different_entries_types.cpp
    double_include.cpp
    enchantum.cpp
    # This file will be created in the next phase.
    # `fmt_format.cpp` and `std_format.cpp` are removed in favor of it.
    format_test.cpp
    functors.cpp
    istream.cpp
    next_value.cpp
    null_terminated.cpp
    ostream.cpp
    prefix_length.cpp
    scoped.cpp
    single_header.cpp
    type_name.cpp
    unscoped_test.cpp
)

# Exclude single_header.cpp from PCH as it tests the single header include
set_source_files_properties(single_header.cpp PROPERTIES SKIP_PRECOMPILE_HEADERS ON)

# Add the test executable to CTest
add_test(NAME enchantum_tests COMMAND tests)
