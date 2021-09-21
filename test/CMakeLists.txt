add_executable(experiment "cpp/m64_test.cpp")

set_target_properties(experiment PROPERTIES
  # Require C++17
  CXX_STANDARD 17 
  CXX_STANDARD_REQUIRED on 
)

target_include_directories(experiment
  PRIVATE include
)

target_link_libraries(experiment pancake.api pancake.expr)