cmake_minimum_required(VERSION 3.15)
project(Pancake
  VERSION 0.3.0
  DESCRIPTION "A library to interface with libsm64"
  HOMEPAGE_URL "https://github.com/jgcodes2020/pancake"
  LANGUAGES C CXX
)

include("${PROJECT_SOURCE_DIR}/cmake/err_handling.cmake")

# Automatic dependencies using Conan
# ----------------------------------

# Install homemade Conan packages
message(STATUS "Installing homemade dependency packages...")
execute_process(
  COMMAND python3 "${PROJECT_SOURCE_DIR}/conan_recipes/setup_conan.py"
  RESULT_VARIABLE PANCAKE_LAST_CMD_EXIT
)
pancake_check_exit_code("Conan package creation failed. This shouldn't happen.")

# Install conan.cmake
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
  message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan...")
  file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/v0.16.1/conan.cmake"
    "${CMAKE_BINARY_DIR}/conan.cmake"
    EXPECTED_HASH SHA256=396e16d0f5eabdc6a14afddbcfff62a54a7ee75c6da23f32f7a31bc85db23484
    TLS_VERIFY ON
  )
endif()
include("${PROJECT_BINARY_DIR}/conan.cmake")

# Install packages
if(NOT CONAN_IN_LOCAL_CACHE)
  if(${GENERATOR_IS_MULTI_CONFIG})
    foreach(TYPE ${CMAKE_CONFIGURATION_TYPES})
      conan_cmake_autodetect(CONAN_CMAKE_SETTINGS BUILD_TYPE ${TYPE})
      conan_cmake_install(
        PATH_OR_REFERENCE ${PROJECT_SOURCE_DIR}
        BUILD missing mbedtls
        SETTINGS ${CONAN_CMAKE_SETTINGS}
      )
    endforeach()
  else()
    # Set a default build type
    include("${PROJECT_SOURCE_DIR}/cmake/default_build_type.cmake")
    conan_cmake_autodetect(CONAN_CMAKE_SETTINGS)
    conan_cmake_install(
      PATH_OR_REFERENCE ${PROJECT_SOURCE_DIR}
      BUILD missing
      SETTINGS ${CONAN_CMAKE_SETTINGS}
    )
  endif()
endif()

# Include Conan build info
include("${PROJECT_BINARY_DIR}/conanbuildinfo.cmake")
conan_basic_setup(TARGETS)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)

# Automatic dependencies using CMake ExternalProject
# --------------------------------------------------
# This is actually the old dependency script, I'm keeping it around in case I need it.

# include(ExternalProject)
# include("cmake/deps/LIEF.cmake")
# include("cmake/deps/libdwarf.cmake")
# include("cmake/deps/nlohmann_json.cmake")

# Config files (unused ATM)
# ------------

configure_file("pancake_metadata.json.in" "pancake_metadata.json")

add_subdirectory(src)
if (PANCAKE_BUILD_TESTS)
  add_subdirectory(test)
endif()

install (SCRIPT "cmake/install_helper.cmake")