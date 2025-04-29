cmake_minimum_required(VERSION 3.22.0)
project(enchantum LANGUAGES CXX)

if(POLICY CMP0128)
    cmake_policy(SET CMP0128 NEW)
endif()
  
set(INCDIR "enchantum/include")
file(GLOB_RECURSE INCS "${INCDIR}/*.hpp")

add_library(enchantum INTERFACE ${INCS})
add_library(enchantum::enchantum ALIAS enchantum)
target_compile_features(enchantum INTERFACE cxx_std_20)

target_include_directories(enchantum INTERFACE
    ${INCDIR}
)


option(ENCHANTUM_BUILD_TESTS "Enable tests for this `enchantum` library" OFF)
option(ENCHANTUM_BUILD_BENCHMARKS "Enable compile time benchmarks `enchantum` library" OFF)

if(ENCHANTUM_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

if(ENCHANTUM_BUILD_BENCHMARKS)
  add_subdirectory(benchmarks)
endif()
