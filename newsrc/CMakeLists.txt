add_subdirectory(pancake_dl)
add_subdirectory(pancake_dwarf)
add_subdirectory(pancake_stx)

target_link_libraries(pancake.dwarf pancake.stx)