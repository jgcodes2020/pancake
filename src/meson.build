exp_headers = include_directories('export')

deps = [
  subproject('libdwarf').get_variable('dwarf'),
  subproject('lief').get_variable('lief')
]

static_library('pancake', 'cpp/sm64-win.cpp', 
  include_directories: exp_headers,
  dependencies: deps
)

executable('experiment', 'experiment/experiment.cpp',
  include_directories: exp_headers,
  dependencies: deps
)