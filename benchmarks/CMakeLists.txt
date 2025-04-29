include(CheckCXXCompilerFlag)

include(FetchContent)
FetchContent_Declare(magic_enum
	GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
	GIT_SHALLOW ON
	GIT_TAG master)
FetchContent_MakeAvailable(magic_enum)

add_executable(benchmarks)
target_include_directories(benchmarks PRIVATE "${magic_enum_SOURCE_DIR}/include")
target_link_libraries(benchmarks enchantum::enchantum Catch2::Catch2 Catch2::Catch2WithMain)

if(MSVC)
  target_compile_options(benchmarks PRIVATE /Za /permissive-)
  target_compile_options(benchmarks PRIVATE /W4)
else()
  target_compile_options(benchmarks PRIVATE -Wall -Wextra -Wconversion -Wpedantic)
endif()



# Gather source files
file(GLOB_RECURSE SRCS "*.cpp" "*.hpp")

target_sources(benchmarks PRIVATE
  ${SRCS}
)
