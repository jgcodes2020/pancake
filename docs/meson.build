doc_cmd = ['make', 'html']
if build_machine.system() == 'windows'
  doc_cmd = ['cmd', '/c', ]
endif

src_folder = meson.project_source_root() / 'src'

custom_target('docs',
  build_by_default: false,
  depends_files: [
    'index.rst', 
    # API
    'api/index.rst',
      'api/sm64.rst',
        src_folder/'export/sm64.hpp'
  ]
  depends: exp_headers,
  command: 
)