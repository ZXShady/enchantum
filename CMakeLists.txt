cmake_minimum_required(VERSION 3.22.0)
project(enchantum
VERSION 0.4.0
HOMEPAGE_URL "https://github.com/ZXShady/enchantum"
DESCRIPTION  "Fast macro-free enum reflection C++ library"
LANGUAGES CXX)

if(POLICY CMP0128)
    cmake_policy(SET CMP0128 NEW)
endif()

set(INCDIR "${CMAKE_CURRENT_SOURCE_DIR}/enchantum/include")
file(GLOB_RECURSE INCS "${INCDIR}/*.hpp")

add_library(enchantum INTERFACE ${INCS})
add_library(enchantum::enchantum ALIAS enchantum)
target_compile_features(enchantum INTERFACE cxx_std_17)

add_library(enchantum_cxx14 INTERFACE ${INCS})
add_library(enchantum::enchantum-cxx14 ALIAS enchantum_cxx14)
set_target_properties(enchantum_cxx14 PROPERTIES EXPORT_NAME enchantum-cxx14)
target_compile_features(enchantum_cxx14 INTERFACE cxx_std_14)

foreach(enchantum_target enchantum enchantum_cxx14)
target_include_directories(${enchantum_target} INTERFACE
    $<BUILD_INTERFACE:${INCDIR}>
    $<INSTALL_INTERFACE:include>
)
endforeach()

option(ENCHANTUM_BUILD_TESTS "Enable tests for this `enchantum` library" OFF)
option(ENCHANTUM_ENABLE_MSVC_SPEEDUP "Enable faster but not 100% accurate MSVC enum reflection." ON)
option(ENCHANTUM_BUILD_BENCHMARKS "Enable compile time benchmarks `enchantum` library" OFF)

foreach(enchantum_target enchantum enchantum_cxx14)
target_compile_definitions(${enchantum_target} INTERFACE ENCHANTUM_ENABLE_MSVC_SPEEDUP=$<BOOL:${ENCHANTUM_ENABLE_MSVC_SPEEDUP}>)
endforeach()

if(ENCHANTUM_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

if(ENCHANTUM_BUILD_BENCHMARKS)
  add_subdirectory(benchmarks)
endif()


install(TARGETS enchantum enchantum_cxx14
    EXPORT enchantumTargets
    COMPONENT enchantum
)

include(GNUInstallDirs)

install(DIRECTORY ${INCDIR}/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT enchantum
    FILES_MATCHING PATTERN "*.hpp"
)

install(EXPORT enchantumTargets
    NAMESPACE enchantum::
    DESTINATION ${CMAKE_INSTALL_DATADIR}/enchantum/cmake
    COMPONENT enchantum
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/enchantumConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
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
    COMPONENT enchantum
)

export(
    TARGETS enchantum enchantum_cxx14
    NAMESPACE enchantum::
    FILE "${CMAKE_CURRENT_BINARY_DIR}/enchantumTargets.cmake"
)
