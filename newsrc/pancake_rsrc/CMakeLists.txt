cmake_minimum_required(VERSION 3.15)
project(pancake_rsrc 
  LANGUAGES C
  DESCRIPTION "Resources and utilities thereof for Pancake"
)

# Sources
# =======
function(pancake_rsrc_gen target src_name)
  message(STATUS "Target: ${target}")
  message(STATUS "Source file: ${src_name}")

  set(abs_path "${CMAKE_CURRENT_SOURCE_DIR}/${src_name}")
  set(file_name)
  get_filename_component(file_name ${src_name} NAME)
  set(gen_obj "${CMAKE_CURRENT_BINARY_DIR}/pancake_rsrc_${file_name}.o")
  set(gen_inc "${CMAKE_CURRENT_BINARY_DIR}/_genhdrs/pancake_rsrc_${file_name}.h")
  
  message(STATUS "Generated filename is: ${gen_obj}")
  add_custom_command(
    OUTPUT ${gen_obj} ${gen_inc}
    MAIN_DEPENDENCY ${abs_path}
    COMMENT "Generating linkable object for ${src_name}"
    COMMAND python3 "${Pancake_SOURCE_DIR}/scripts/rsrc_gen_asm.py" -O "${gen_obj}" -H "${gen_inc}" "${abs_path}"
  )
  
  
  target_sources(${target}
    PRIVATE ${gen_obj}
    PUBLIC ${gen_inc}
  )
  set_source_files_properties(gen_obj PROPERTIES
    EXTERNAL_OBJECT on
    GENERATED on
  )
endfunction()


add_library(pancake.rsrc STATIC)

pancake_rsrc_gen(pancake.rsrc "resources/sm64_macro_defns.json")

# Properties
# ==========

set_target_properties(pancake.rsrc PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED on
  LINKER_LANGUAGE C
)

target_include_directories(pancake.rsrc
  PUBLIC include "${CMAKE_CURRENT_BINARY_DIR}/_genhdrs"
)

target_link_libraries(pancake.rsrc
  PUBLIC CONAN_PKG::ms-gsl CONAN_PKG::nlohmann_json
)