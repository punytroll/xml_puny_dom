project(
  'xml_puny_dom',
  'cpp',
  version: '1.0.0',
  meson_version: '>=0.63.0',
  default_options: [
    'cpp_std=c++20',
    'warning_level=3',
    'werror=true'
  ]
)

subproject('xml_parser')

xml_parser_dependency = dependency('xml_parser')

xml_puny_dom_library = library(
  'xml_puny_dom',
  sources: ['source/xml_puny_dom.cpp'],
  include_directories: [include_directories('include/xml_puny_dom')],
  dependencies: [xml_parser_dependency]
)

xml_puny_dom_library_dependency = declare_dependency(
  include_directories: [include_directories('include')],
  link_with: [xml_puny_dom_library],
  dependencies: [xml_parser_dependency]
)
