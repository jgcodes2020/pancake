# libdwarf dependency
# ===========================
set(LIBDWARF_PREFIX       "${CMAKE_CURRENT_BINARY_DIR}/libdwarf")
set(LIBDWARF_INSTALL_DIR  "${LIBDWARF_PREFIX}")
set(LIBDWARF_INCLUDE_DIRS "${LIBDWARF_PREFIX}/include")

# libdwarf static library
set(LIBDWARF_LIBFILE
  "${LIBDWARF_PREFIX}/lib64/${CMAKE_STATIC_LIBRARY_PREFIX}dwarf${CMAKE_STATIC_LIBRARY_SUFFIX}")
  
# Create include dir
file(MAKE_DIRECTORY ${LIBDWARF_INCLUDE_DIRS})

# Git URL (using GitHub)
set(LIBDWARF_GIT_URL "https://github.com/davea42/libdwarf-code.git")

# Use commit from July 15, 2021 (v0.1.1)
set(LIBDWARF_VERSION c703612)

# libdwarf compilation config
set(LIBDWARF_CMAKE_ARGS
  -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
  -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
  -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
)

# Specify MSVCRT on MSVC
if(MSVC)
  list(APPEND ${LIBDWARF_CMAKE_ARGS} -DLIBDWARF_CRT=MT)
endif()

# External project
# libdwarf simply checks if zlib is linked
ExternalProject_Add(LIBDWARF_extproj
  PREFIX           "${LIBDWARF_PREFIX}"
  GIT_REPOSITORY   ${LIBDWARF_GIT_URL}
  GIT_TAG          ${LIBDWARF_VERSION}
  PATCH_COMMAND    git apply "${CMAKE_CURRENT_LIST_DIR}/patches/libdwarf_no_zlib.patch"
  INSTALL_DIR      ${LIBDWARF_INSTALL_DIR}
  CMAKE_ARGS       ${LIBDWARF_CMAKE_ARGS}
  BUILD_BYPRODUCTS ${LIBDWARF_LIBFILE}
)

# Define target for easier inclusion
add_library(libdwarf::dep STATIC IMPORTED)
set_target_properties(libdwarf::dep PROPERTIES
  IMPORTED_LOCATION ${LIBDWARF_LIBFILE}
  INTERFACE_INCLUDE_DIRECTORIES ${LIBDWARF_INCLUDE_DIRS}
)
add_dependencies(libdwarf::dep LIBDWARF_extproj)