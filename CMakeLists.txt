cmake_minimum_required(VERSION 3.22.0)
project(enchantum LANGUAGES CXX)

if(POLICY CMP0128)
    cmake_policy(SET CMP0128 NEW)
endif()
  
set(INCDIR "enchantum/include")
set(INCS "")
file(GLOB_RECURSE INCS "${INCDIR}/*.hpp")

add_library(enchantum INTERFACE ${INCS})
add_library(enchantum::enchantum ALIAS enchantum)
target_compile_features(enchantum INTERFACE cxx_std_20)

if (MSVC)
    target_compile_options(enchantum INTERFACE /Zc:__cplusplus)
endif()

target_include_directories(enchantum INTERFACE
    ${INCDIR}
)


option(ENCHANTUM_BUILD_TESTS "Tests for this `enum reflection` library" OFF)
option(ENCHANTUM_BUILD_BENCHMARKS "Benchmarks for this `enum reflection` library" OFF)

if(ENCHANTUM_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

if(ENCHANTUM_BUILD_BENCHMARKS)
  add_subdirectory(benchmarks)
endif()
