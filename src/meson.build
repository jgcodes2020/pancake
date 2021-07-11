exp_headers = include_directories('export')
int_headers = include_directories('private')

buildtype = get_option('buildtype')
platform_dir = host_machine.system()

rsrc_gen = generator(py,
  output: ['pancake_rsrc_@PLAINNAME@.hpp', 'pancake_rsrc_@PLAINNAME@.cpp'],
  arguments: [
    meson.project_source_root()/'buildscript/rsrc_gen.py', 
    '@INPUT@', '-H', '@BUILD_DIR@/pancake_rsrc_@PLAINNAME@.hpp', '-C', '@BUILD_DIR@/pancake_rsrc_@PLAINNAME@.cpp'
  ]
)

deps = [
  dependency('libdwarf'),
  dependency('LIEF'),
  dependency('nlohmann_json')
]

message('Compiling for ' + host_machine.system())

pancake = static_library('pancake', 
  [
    'common/expr/parse.cpp', 
    'common/expr/compile.cpp',
    'common/dwarf.cpp',
    platform_dir/'sm64.cpp', 
    'common/movie.cpp',
    rsrc_gen.process('resources/sm64_macro_defns.json')
  ],
  include_directories: [exp_headers, int_headers],
  dependencies: deps
)
