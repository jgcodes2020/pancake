# nlohmann json dep
# ===========================
set(BACKWARD_PREFIX      "${CMAKE_CURRENT_BINARY_DIR}/backward")
set(BACKWARD_INSTALL_DIR "${BACKWARD_PREFIX}")
set(BACKWARD_INCLUDE_DIR "${BACKWARD_PREFIX}/include")
  
# Create include dir
file(MAKE_DIRECTORY ${BACKWARD_INCLUDE_DIR})

# Git URL (using GitHub)
set(BACKWARD_GIT_URL "https://github.com/bombela/backward-cpp.git")

# Use version 3.9.1
set(BACKWARD_VERSION 0916dec)

# libdwarf compilation config
set(BACKWARD_CMAKE_ARGS
  -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
  -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
  -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
  -DBACKWARD_TESTING=off
)

# External project
ExternalProject_Add(BACKWARD_extproj
  PREFIX            "${BACKWARD_PREFIX}"
  GIT_REPOSITORY    ${BACKWARD_GIT_URL}
  GIT_TAG           ${BACKWARD_VERSION}
  INSTALL_DIR       ${BACKWARD_INSTALL_DIR}
  PATCH_COMMAND     git apply "${CMAKE_CURRENT_LIST_DIR}/patches/backward.patch" || echo "Patch is already applied...?"
  CMAKE_ARGS        ${BACKWARD_CMAKE_ARGS}
  USES_TERMINAL_DOWNLOAD on
  USES_TERMINAL_UPDATE on
  USES_TERMINAL_CONFIGURE on
  USES_TERMINAL_BUILD on
  USES_TERMINAL_INSTALL on
)

# Dependency
add_library(backward::dep INTERFACE IMPORTED)
set_target_properties(backward::dep PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${BACKWARD_INCLUDE_DIR}
)
add_dependencies(backward::dep BACKWARD_extproj)