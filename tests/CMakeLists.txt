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

file(GLOB SRCS "*.cpp" "*.hpp")


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

if(STD LESS 26)
    list(REMOVE_ITEM SRCS "${CMAKE_CURRENT_SOURCE_DIR}/cxx26_reflection.cpp")
endif()
target_sources(tests PRIVATE ${SRCS})

file(GLOB FAIL_SRCS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/fail/*.cpp"
)

foreach(FAIL_SRC IN LISTS FAIL_SRCS)
    get_filename_component(FAIL_NAME "${FAIL_SRC}" NAME_WE)
    # My God you are a savior my man thanks!
    # Source - https://stackoverflow.com/a/30191576
    # Posted by Fraser
    # Retrieved 2026-09-07, License - CC BY-SA 3.0
    set(FAIL_TEST fail_${FAIL_NAME})
    add_executable(${FAIL_TEST} ${FAIL_SRC})
    set_target_properties(${FAIL_TEST} PROPERTIES
                          EXCLUDE_FROM_ALL TRUE
                          EXCLUDE_FROM_DEFAULT_BUILD TRUE)
    # Provide a PP definition to target the appropriate part of
    # "will_fail.cpp", or provide separate files per test.
    target_link_libraries(${FAIL_TEST} PRIVATE enchantum::enchantum)

    # Add the tests.  These invoke "cmake --build ..." which is a
    # cross-platform way of building the given target.
    add_test(NAME ${FAIL_TEST}
         COMMAND ${CMAKE_COMMAND} --build . --target ${FAIL_TEST} --config $<CONFIGURATION>
         WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
    # Expect these tests to fail (i.e. cmake --build should return
    # a non-zero value)
    set_tests_properties(${FAIL_TEST} PROPERTIES WILL_FAIL TRUE)
endforeach()


add_executable(tests_config)
target_compile_features(tests_config PRIVATE cxx_std_17)
target_compile_definitions(tests_config PRIVATE ENCHANTUM_CONFIG_FILE="config_test/config.hpp")
target_sources(tests_config PRIVATE config_test/config.cpp config_test/config.hpp)
target_include_directories(tests_config PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(tests_config Catch2::Catch2WithMain enchantum::enchantum)

include(CTest)
include(Catch)
catch_discover_tests(tests)
catch_discover_tests(tests_config)



