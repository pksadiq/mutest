sources = [
  'mutest-expect.c',
  'mutest-expect-funcs.c',
  'mutest-expect-types.c',
  'mutest-main.c',
  'mutest-spec.c',
  'mutest-suite.c',
  'mutest-utils.c',
]

test_headers = [
  'sys/ioctl.h',
  'sys/types.h',
  'unistd.h',
]

foreach h: test_headers
  if cc.has_header(h)
    define = 'HAVE_' + h.underscorify().to_upper()
    config_h.set(define, 1)
  endif
endforeach

test_functions = [
  [ 'isatty', 'unistd.h' ]
]

foreach f: test_functions
  if cc.has_function(f[0], prefix: '#include <@0@>'.format(f[1]))
    define = 'HAVE_' + f[0].underscorify().to_upper()
    config_h.set(define, 1)
  endif
endforeach

configure_file(output: 'config.h', configuration: config_h)

test_link_args = []
if host_machine.system() == 'linux' and cc.get_id() == 'gcc'
  test_link_args += [
  ]
endif
if host_machine.system() == 'darwin'
  test_link_args += [
    '-compatibility_version 1',
    '-current_version 1.0',
  ]
endif

mutest_link_args = cc.get_supported_link_arguments(test_link_args)

if static
  mutest_lib = static_library(
    mutest_api_path,
    sources: sources,
    install: installable,
    c_args: common_flags + [
      '-DMUTEST_COMPILATION',
    ],
    dependencies: cc.find_library('m', required: false),
    include_directories: headers_inc,
  )
else
  mutest_lib = library(
    mutest_api_path,
    sources: sources,
    version: mutest_soversion,
    c_args: common_flags + [
      '-DMUTEST_COMPILATION',
    ],
    dependencies: cc.find_library('m', required: false),
    include_directories: headers_inc,
    link_args: mutest_link_args,
  )
endif

mutest_dep = declare_dependency(
  link_with: mutest_lib,
  include_directories: headers_inc,
)

if installable
  pkgconf = import('pkgconfig')
  pkgconf.generate(
    name: 'mutest',
    description: 'Simple C unit testing library',
    libraries: mutest_lib,
    subdirs: mutest_api_path,
    filebase: mutest_api_path,
  )
endif
