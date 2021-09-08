project(pancake_rsrc
  DESCRIPTION "Resources and utilities thereof for Pancake"
)

# Sources
# =======

add_library(pancake.rsrc
  "src/spanbuf.cpp"
)

# Properties
# ==========

set_target_properties(pancake.rsrc PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED on
)

target_include_directories(pancake.rsrc
  PUBLIC include
)

target_link_libraries(pancake.rsrc
  PUBLIC CONAN_PKG::ms-gsl
)