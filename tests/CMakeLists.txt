include(FetchContent)
FetchContent_Declare( 
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG devel
)
FetchContent_MakeAvailable(Catch2)
 
target_compile_features(Catch2 PRIVATE cxx_std_17)
target_compile_definitions(Catch2 PUBLIC CATCH_CONFIG_ENABLE_OPTIONAL_STRINGMAKER)
add_executable(tests)
target_include_directories(tests PRIVATE "third_party")
target_compile_features(tests PRIVATE cxx_std_17)


target_link_libraries(tests PRIVATE enchantum::enchantum Catch2::Catch2WithMain)

if(ENCHANTUM_RUNTIME_TESTS)
  message(STATUS "enchantum tests are being ran at runtime")
  target_compile_definitions(tests PRIVATE CATCH_CONFIG_RUNTIME_STATIC_REQUIRE)
endif()

if(MSVC)
  target_compile_options(tests PRIVATE /Za /permissive-)
  target_compile_options(tests PRIVATE /WX /W4)
else()
  target_compile_options(tests PRIVATE -Werror -Wall -Wextra -Wshadow -Wconversion -Wpedantic)
endif()

file(GLOB_RECURSE SRCS "*.cpp" "*.hpp")


get_target_property(STD tests CXX_STANDARD)

if(MSVC)
set(CMAKE_REQUIRED_FLAGS "/std:c++${STD}")
else()
set(CMAKE_REQUIRED_FLAGS "-std=c++${STD}")
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

if(STD LESS 20)
    list(REMOVE_ITEM SRCS "${CMAKE_CURRENT_SOURCE_DIR}/concepts.cpp")
    list(REMOVE_ITEM SRCS "${CMAKE_CURRENT_SOURCE_DIR}/double_include.cpp")
    list(REMOVE_ITEM SRCS "${CMAKE_CURRENT_SOURCE_DIR}/std_format.cpp")
    list(REMOVE_ITEM SRCS "${CMAKE_CURRENT_SOURCE_DIR}/single_header.cpp")
endif()
target_sources(tests PRIVATE ${SRCS})

include(CTest)
include(Catch)
catch_discover_tests(tests)
