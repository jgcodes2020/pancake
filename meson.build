project('pancake', 'cpp', 
  default_options: [
    'cpp_std=c++17',
    'b_vscrt=static_from_buildtype'
  ],
  version: '0.1-WIP'
)

cxx = meson.get_compiler('cpp')
py = find_program('python3', 'python')

# Compile static library
subdir('src')
