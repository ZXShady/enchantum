cmake_minimum_required(VERSION 3.22.0)
project(consumer)

add_executable(consumer main.cpp)

find_package(enchantum REQUIRED CONFIG)

target_link_libraries(consumer enchantum::enchantum)