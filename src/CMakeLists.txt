# Resource generator
function(pancake_rsrc_gen var_outputs src_name)
  set(abs_path "${CMAKE_CURRENT_SOURCE_DIR}/${src_name}")
  set(file_name)
  get_filename_component(file_name ${src_name} NAME)
  set(gen_src "${CMAKE_CURRENT_BINARY_DIR}/pancake_rsrc_${file_name}.cpp")
  set(gen_inc "${CMAKE_CURRENT_BINARY_DIR}/_genhdrs/pancake_rsrc_${file_name}.hpp")
  
  set(${var_outputs} ${${var_outputs}} ${gen_src} PARENT_SCOPE)
  message(STATUS "Generated filename is: ${gen_src}")
  add_custom_command(
    OUTPUT ${gen_src} ${gen_inc}
    MAIN_DEPENDENCY ${abs_path}
    COMMAND python3 "${PROJECT_SOURCE_DIR}/scripts/rsrc_gen.py" -C "${gen_src}" -H "${gen_inc}" "${abs_path}"
  )
endfunction()

# Sources
set(pancake_sources 
  "common/movie.cpp" 
  "common/dwarf.cpp" 
  "common/expr/compile.cpp" 
  "common/expr/parse.cpp" 
)
if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  list(APPEND pancake_sources
    "windows/sm64.cpp"
  )
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
  list(APPEND pancake_sources 
    "linux/sm64.cpp"
  )
else()
  message(FATAL_ERROR "This library can only be compiled for Windows and Linux. I don't have a Mac, so I can't build Apple software.")
endif()

# Main header
set(pancake_export "${CMAKE_CURRENT_SOURCE_DIR}/export")
set(pancake_private "${CMAKE_CURRENT_SOURCE_DIR}/private")
set(pancake_genhdrs "${CMAKE_CURRENT_BINARY_DIR}/_genhdrs")

# Generated sources
set(macro_defns_src)
pancake_rsrc_gen(macro_defns_src "resources/sm64_macro_defns.json")
list(APPEND pancake_sources ${macro_defns_src})

message(STATUS "Final source list: ${pancake_sources}")

add_library(pancake STATIC ${pancake_sources})
set_target_properties(pancake PROPERTIES
  # Require C++17
  CXX_STANDARD 17 
  CXX_STANDARD_REQUIRED on 
)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
endif()

# Include both public/private headers
target_include_directories(pancake 
  PUBLIC ${pancake_export} 
  PRIVATE ${pancake_private}
  PRIVATE ${pancake_genhdrs}
)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  target_link_libraries(pancake 
    PUBLIC LIEF::dep libdwarf::dep nlohmann_json::dep
  )
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
  target_link_libraries(pancake 
    PUBLIC LIEF::dep libdwarf::dep nlohmann_json::dep ${CMAKE_DL_LIBS}
  )
else()
  message(FATAL_ERROR "This library can only be compiled for Windows and Linux. I don't have a Mac, so I can't build Apple software.")
endif()
