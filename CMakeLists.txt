cmake_minimum_required(VERSION 3.24)
project(funremez CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(CLI11
    URL https://github.com/CLIUtils/CLI11/archive/refs/tags/v2.7.2.tar.gz
    URL_HASH SHA256=46eef3101da70852ec7af026e09d485ccee81813331c8c6052d39344443b83da
)
FetchContent_MakeAvailable(CLI11)

FetchContent_Declare(PEGTL
    URL https://github.com/taocpp/PEGTL/archive/refs/tags/3.2.9.tar.gz
    URL_HASH SHA256=346aeb9644ce976fde499069403983d25e7b701914c6fac87c021cf04911627d
)
set(PEGTL_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(PEGTL_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(PEGTL)

find_package(PkgConfig REQUIRED)
pkg_check_modules(MPFR REQUIRED IMPORTED_TARGET mpfr)

add_executable(funremez
    src/funremez.cpp
    src/solver.cpp
)

target_compile_definitions(funremez PRIVATE PACKAGE_VERSION="0.7")
target_link_libraries(funremez PRIVATE CLI11::CLI11 taocpp::pegtl PkgConfig::MPFR)
