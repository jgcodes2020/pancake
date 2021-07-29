# nlohmann json dep
# ===========================
set(NLOHMANN_JSON_PREFIX      "${CMAKE_CURRENT_BINARY_DIR}/nlohmann_json")
set(NLOHMANN_JSON_INSTALL_DIR "${NLOHMANN_JSON_PREFIX}")
set(NLOHMANN_JSON_INCLUDE_DIR "${NLOHMANN_JSON_PREFIX}/include")
  
# Create include dir
file(MAKE_DIRECTORY ${NLOHMANN_JSON_INCLUDE_DIR})

# Git URL (using GitHub)
set(NLOHMANN_JSON_GIT_URL "https://github.com/nlohmann/json.git")

# Use version 3.9.1
set(NLOHMANN_JSON_VERSION v3.9.1)

# nlohmann_json compilation config
set(NLOHMANN_JSON_CMAKE_ARGS
  -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
  -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
  -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
  -DBUILD_TESTING=off
)

# External project
ExternalProject_Add(NLOHMANN_JSON_extproj
  PREFIX            "${NLOHMANN_JSON_PREFIX}"
  GIT_REPOSITORY    ${NLOHMANN_JSON_GIT_URL}
  GIT_TAG           ${NLOHMANN_JSON_VERSION}
  INSTALL_DIR       ${NLOHMANN_JSON_INSTALL_DIR}
  CMAKE_ARGS        ${NLOHMANN_JSON_CMAKE_ARGS}
)

# Dependency
add_library(nlohmann_json::dep INTERFACE IMPORTED)
set_target_properties(nlohmann_json::dep PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${NLOHMANN_JSON_INCLUDE_DIR}
)
add_dependencies(nlohmann_json::dep NLOHMANN_JSON_extproj)