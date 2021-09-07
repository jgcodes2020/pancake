project(pancake_dl
  DESCRIPTION "Dynamic library loader for Pancake"
)

# Sources
# =======

add_library(pancake.dl)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  target_sources(pancake.dl
    PRIVATE
      "src/win32.cpp"
  )
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
  target_sources(pancake.dl
    PRIVATE
      "src/linux.cpp"
  )
else()
  message(FATAL_ERROR "I can't test Mac. Sorry. Add it yourself, I guess...")
endif()

# Properties
# ==========

set_target_properties(pancake.dl PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED on
)

target_include_directories(pancake.dl
  PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include"
)

target_link_libraries(pancake.dl CONAN_PKG::LIEF)
