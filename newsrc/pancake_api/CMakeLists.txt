cmake_minimum_required(VERSION 3.15)
project(pancake_api
  DESCRIPTION "Core API for Pancake"
)

add_library(pancake.api
  "src/movie.cpp"
  "src/sm64.cpp"
)

target_include_directories(pancake.api
  PUBLIC include
)

set_target_properties(pancake.api PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED on
)
