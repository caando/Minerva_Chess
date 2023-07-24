cmake_minimum_required(VERSION 3.2)

project(Minerva_Chess)

include(FetchContent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -O3 -Wall -Wno-write-strings -flto -funroll-loops -finline-functions -static-libstdc++")

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})

add_subdirectory(src)
add_subdirectory(test)
