project(
  'xml_puny_dom',
  'cpp',
  version: '26.3.0',
  meson_version: '>=1.10.0',
  default_options: [
    'cpp_std=c++23',
    'warning_level=3',
    'werror=true'
  ]
)

subproject('xml_parser')
xml_parser_dependency = dependency('xml_parser')

xml_puny_dom_library_include_directories = include_directories('include')

xml_puny_dom_library = library(
  'xml_puny_dom',
  sources: ['source/xml_puny_dom.cpp'],
  include_directories: xml_puny_dom_library_include_directories,
  dependencies: xml_parser_dependency,
)

xml_puny_dom_library_dependency = declare_dependency(
  include_directories: xml_puny_dom_library_include_directories,
  link_with: xml_puny_dom_library,
  dependencies: xml_parser_dependency
)
meson.override_dependency('xml_puny_dom', xml_puny_dom_library_dependency)

test(
  'test_suite',
  executable(
    'test_suite',
    build_by_default: false,
    sources: 'testing/test_suite.cpp',
    dependencies: xml_puny_dom_library_dependency,
  )
)
