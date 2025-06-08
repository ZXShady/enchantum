cmake_minimum_required(VERSION 3.22.0)
project(enchantum LANGUAGES CXX)

if(POLICY CMP0128)
    cmake_policy(SET CMP0128 NEW)
endif()

set(INCDIR "${CMAKE_CURRENT_SOURCE_DIR}/enchantum/include")
file(GLOB_RECURSE INCS "${INCDIR}/*.hpp")

add_library(enchantum INTERFACE ${INCS})
add_library(enchantum::enchantum ALIAS enchantum)
target_compile_features(enchantum INTERFACE cxx_std_20)
 
target_include_directories(enchantum INTERFACE
    $<BUILD_INTERFACE:${INCDIR}>
    $<INSTALL_INTERFACE:include>
)

option(ENCHANTUM_BUILD_TESTS "Enable tests for this `enchantum` library" OFF)
option(ENCHANTUM_ENABLE_MSVC_SPEEDUP "Enable faster but not 100% accurate MSVC enum reflection." ON)
option(ENCHANTUM_BUILD_BENCHMARKS "Enable compile time benchmarks `enchantum` library" OFF)

if(ENCHANTUM_ENABLE_MSVC_SPEEDUP)
    target_compile_definitions(enchantum INTERFACE ENCHANTUM_ENABLE_MSVC_SPEEDUP=1)
endif()

if(ENCHANTUM_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

if(ENCHANTUM_BUILD_BENCHMARKS)
  add_subdirectory(benchmarks)
endif()


install(TARGETS enchantum
    EXPORT enchantumTargets
)

install(DIRECTORY ${INCDIR}/enchantum/
    DESTINATION include/enchantum
    FILES_MATCHING PATTERN "*.hpp"
)

install(EXPORT enchantumTargets
    NAMESPACE enchantum::
    DESTINATION cmake
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/enchantumConfigVersion.cmake"
  VERSION 0.1.0
  COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/enchantumConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/enchantumConfig.cmake"
    INSTALL_DESTINATION cmake
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/enchantumConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/enchantumConfigVersion.cmake"
    DESTINATION cmake
)