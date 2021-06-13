exp_headers = include_directories('export')
int_headers = include_directories('private')

buildtype = get_option('buildtype')

deps = [
  subproject('libdwarf').get_variable('lib'),
  subproject('LIEF').get_variable(buildtype)
]

includer = generator(py, 
  arguments: ['../script/always_include.py', '@INPUT@', '-o', '@OUTPUT@', '-H', 'ciso646'],
  output: '@BASENAME@.gen.cpp',
)

#static_library('pancake', 'cpp/sm64-win.cpp', 
#  include_directories: [exp_headers, int_headers],
#  dependencies: deps
#)

executable('experiment', 'experiment/experiment.cpp',
  include_directories: [exp_headers, int_headers],
  dependencies: deps
)