xpmt_headers = include_directories('include')
experiment = executable('experiment', 'cpp/experiment.cpp',
  include_directories: [exp_headers, int_headers, xpmt_headers],
  link_with: pancake,
  dependencies: deps
)