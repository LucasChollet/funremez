cmake_minimum_required(VERSION 3.16)
project(lolremez CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(lolremez
    src/lolremez.cpp
    src/solver.cpp
)

target_include_directories(lolremez PRIVATE lol/include)
target_compile_definitions(lolremez PRIVATE PACKAGE_VERSION="0.7")
