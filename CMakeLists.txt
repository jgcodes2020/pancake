cmake_minimum_required(VERSION 3.15)
project(Pancake
  VERSION 0.1.1
  DESCRIPTION "A library to interface with libsm64"
  HOMEPAGE_URL "https://github.com/jgcodes2020/pancake"
  LANGUAGES C CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)

include(ExternalProject)

include("cmake/dep_LIEF.cmake")
include("cmake/dep_libdwarf.cmake")
include("cmake/dep_nlohmann_json.cmake")
include("cmake/dep_backward.cmake")

configure_file("pancake_metadata.json.in" "pancake_metadata.json")

add_subdirectory(src)
add_subdirectory(test)