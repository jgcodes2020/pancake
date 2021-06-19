project('pancake', 'cpp', 
  default_options: [
    'cpp_std=c++17',
    'b_vscrt=static_from_buildtype'
  ],
  version: '0.1-WIP'
)

cxx = meson.get_compiler('cpp')
py = find_program('python3', 'python')

# Enforce correct compilers
if (host_machine.system() == 'windows')
  assert(cxx.get_id() == 'msvc', 'You need to use Visual C++ on Windows.')
endif

meson.add_postconf_script(py, meson.project_source_root()/'buildscript/postconf.py')

# Compile static library
subdir('src')
subdir('test')