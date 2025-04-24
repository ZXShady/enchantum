cmake_minimum_required(VERSION 3.15.0)
include(CheckCXXCompilerFlag)
include(FetchContent)
 
FetchContent_Declare( 
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG devel
)
FetchContent_MakeAvailable(Catch2)

add_executable(tests)

target_link_libraries(tests enchantum::enchantum Catch2::Catch2 Catch2::Catch2WithMain)

if(ENCHANTUM_RUNTIME_TESTS)
	target_compile_definitions(tests PRIVATE CATCH_CONFIG_RUNTIME_STATIC_REQUIRE)
endif()

if(MSVC)
  target_compile_options(tests PRIVATE /Za /permissive-)
endif()

if(MSVC)
  target_compile_options(tests PRIVATE /W4)
else()
  target_compile_options(tests PRIVATE -Wall -Wextra -Wconversion -Wpedantic)
endif()

# Gather source files
set(SRCS 
  ""
)
file(GLOB_RECURSE SRCS "*.cpp" "*.hpp")

target_sources(tests PRIVATE
  ${SRCS}
)

include(CTest)
include(Catch)
catch_discover_tests(tests)
