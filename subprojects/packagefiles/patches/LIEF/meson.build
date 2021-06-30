project('LIEF', 'cpp',
  default_options: [
    'default_library=static'
  ]
)

cxx = meson.get_compiler('cpp')

dep = declare_dependency(
  include_directories: include_directories('include'),
  dependencies: cxx.find_library('LIEF', dirs: meson.current_source_dir()/'lib')
)