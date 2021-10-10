add_subdirectory(pancake_api)
add_subdirectory(pancake_cosa)
add_subdirectory(pancake_dl)
add_subdirectory(pancake_dwarf)
add_subdirectory(pancake_expr)
add_subdirectory(pancake_rsrc)
add_subdirectory(pancake_stx)

target_link_libraries(pancake.dwarf 
  PRIVATE pancake.stx
)
target_link_libraries(pancake.expr 
  PRIVATE pancake.rsrc
  PUBLIC pancake.dwarf pancake.stx
)
target_link_libraries(pancake.api
  PRIVATE pancake.expr
  PUBLIC pancake.dwarf pancake.dl
)
target_link_libraries(pancake.cosa
  PRIVATE pancake.api
)