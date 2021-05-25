project('pancake', 'cpp', 
  default_options: ['cpp_std=c++17'],
  version: '0.1-WIP'
)

cxx = meson.get_compiler('cpp')

# download conan packages
message('Loading Conan packages')
run_command('conan', 'install', '-b=missing', meson.global_source_root(), check: true)
# load conan include dirs
conan_incs = include_directories(run_command('python', meson.global_source_root() / 'buildscript/includes.py', meson.global_source_root()).stdout().split('\n'))
  
# load conan static lib dirs
conan_libs = []
foreach line : run_command('python', meson.global_source_root() / 'buildscript/libs.py', meson.global_source_root()).stdout().split('\n')
  tokens = line.split('\t')
  if tokens.length() < 2
    break
  endif
  message('processing lib ' + tokens[1])
  conan_libs += cxx.find_library(tokens[1], dirs: tokens[0])
endforeach

conan_dep = declare_dependency(
  include_directories: conan_incs,
  dependencies: conan_libs,
  version: '0.1-dev'
)

# Compile static library
subdir('src')
