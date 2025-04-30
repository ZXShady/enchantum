include(CheckCXXCompilerFlag)
include(CheckCXXSourceCompiles)
include(FetchContent)

# Check for std::format support
set(CMAKE_REQUIRED_FLAGS "-std=c++20")
check_cxx_source_compiles("
#include <format>
int main() {
  auto s = std::format(\"{}\", 42);
  return 0;
}
" HAS_STD_FORMAT)

if(NOT HAS_STD_FORMAT)
  message(STATUS "This compiler does not support <format> header not running std_format.cpp tests")
endif()

FetchContent_Declare( 
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG devel
)
FetchContent_MakeAvailable(Catch2)

add_executable(tests)

target_include_directories(tests PRIVATE "third_party")

target_link_libraries(tests enchantum::enchantum Catch2::Catch2 Catch2::Catch2WithMain)

if(ENCHANTUM_RUNTIME_TESTS)
  message(STATUS "enchantum tests are being ran at runtime")
  target_compile_definitions(tests PRIVATE CATCH_CONFIG_RUNTIME_STATIC_REQUIRE)
endif()

if(MSVC)
  target_compile_options(tests PRIVATE /Za /permissive-)
  target_compile_options(tests PRIVATE /W4)
else()
  target_compile_options(tests PRIVATE -Wall -Wextra -Wconversion -Wpedantic)
endif()

file(GLOB_RECURSE SRCS "*.cpp" "*.hpp")

if(NOT HAS_STD_FORMAT)
	list(REMOVE_ITEM SRCS "${CMAKE_CURRENT_SOURCE_DIR}/std_format.cpp")
endif()

target_sources(tests PRIVATE ${SRCS})

include(CTest)
include(Catch)
catch_discover_tests(tests)
