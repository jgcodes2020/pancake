project(pancake_dwarf
  DESCRIPTION "Smart-pointer-based wrapper of libdwarf"
)

# Sources
# =======

add_library(pancake.dwarf
  "src/error.cpp"
  "src/functions.cpp"
  "src/type_lookup.cpp"
)

# Properties
# ==========

set_target_properties(pancake.dwarf PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED on
)

target_include_directories(pancake.dwarf
  PUBLIC include
)

target_link_libraries(pancake.dwarf 
  PUBLIC CONAN_PKG::libdwarf
)