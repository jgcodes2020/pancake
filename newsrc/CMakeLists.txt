add_subdirectory(pancake_dl)
add_subdirectory(pancake_dwarf)
add_subdirectory(pancake_expr)
add_subdirectory(pancake_rsrc)
add_subdirectory(pancake_stx)

target_link_libraries(pancake.dwarf 
  PRIVATE pancake.stx
)
target_link_libraries(pancake.expr 
  PRIVATE pancake.stx
  PUBLIC pancake.dwarf
)