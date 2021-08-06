add_executable(experiment "cpp/m64_test.cpp")

set_target_properties(experiment PROPERTIES
  # Require C++17
  CXX_STANDARD 17 
  CXX_STANDARD_REQUIRED on 
)

target_include_directories(experiment
  PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(experiment pancake backward::dep)