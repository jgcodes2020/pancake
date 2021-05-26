exp_headers = include_directories('export')

static_library('pancake', 'cpp/sm64-win.cpp', 
  include_directories: exp_headers,
  dependencies: conan_dep
)

executable('experiment', 'experiment/experiment.cpp',
  include_directories: exp_headers,
  dependencies: conan_dep
)