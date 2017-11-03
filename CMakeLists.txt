cmake_minimum_required(VERSION 2.8)

project(TouchDesktopCOmpositor)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "--std=c++17 -g ${CMAKE_CXX_FLAGS}")

find_package(SDL2 REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS})
include_directories(include/)

add_executable(start src/main.cc)

target_link_libraries(start ${SDL2_LIBRARIES})
