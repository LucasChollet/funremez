cmake_minimum_required(VERSION 3.16)
project(lolremez CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(CLI11
    URL https://github.com/CLIUtils/CLI11/archive/refs/tags/v2.7.2.tar.gz
    URL_HASH SHA256=46eef3101da70852ec7af026e09d485ccee81813331c8c6052d39344443b83da
)
FetchContent_MakeAvailable(CLI11)

FetchContent_Declare(PEGTL
    URL https://github.com/taocpp/PEGTL/archive/refs/tags/3.0.0.tar.gz
    URL_HASH SHA256=b47af025450f5c12251cd831f90deab666e18a8029f3dbffb32672b0450af902
)
set(PEGTL_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(PEGTL_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(PEGTL)

add_executable(lolremez
    src/lolremez.cpp
    src/solver.cpp
)

target_include_directories(lolremez PRIVATE lol/include)
target_compile_definitions(lolremez PRIVATE PACKAGE_VERSION="0.7")
target_link_libraries(lolremez PRIVATE CLI11::CLI11 taocpp::pegtl)
