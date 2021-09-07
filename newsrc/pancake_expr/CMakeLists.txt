project(pancake_expr
  DESCRIPTION "Expression parser for Pancake"
)

# Sources
# =======

add_library(pancake.expr
  "src/compile.cpp"
  "src/parse.cpp"
)

# Properties
# ==========

set_target_properties(pancake.expr
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED on
)

target_include_directories(pancake.expr
  PUBLIC include
)

target_link_libraries(pancake.expr )
