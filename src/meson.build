exp_headers = include_directories('export')
int_headers = include_directories('private')

buildtype = get_option('buildtype')
platform_dir = 'cpp' / host_machine.system()

deps = [
  subproject('libdwarf').get_variable(buildtype),
  subproject('LIEF').get_variable(buildtype)
]

message('Compiling for ' + host_machine.system())

pancake = static_library('pancake', 'cpp/exparser.cpp', 'cpp/dwarf.cpp',
  include_directories: [exp_headers, int_headers],
  dependencies: deps
)
