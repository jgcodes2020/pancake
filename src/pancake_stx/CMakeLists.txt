project(pancake_stx
  DESCRIPTION "Extensions of the C++ STL for Pancake"
)

# Sources
# =======

add_library(pancake.stx INTERFACE)

# Properties
# ==========

set_target_properties(pancake.stx PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED on
)

target_include_directories(pancake.stx
  INTERFACE include
)

# Installation
# ============
install(DIRECTORY "include"
  DESTINATION "."
)