
project('json-c', 'c', version: '0.18.99',
  meson_version: '>=0.54.0',
  license: 'MIT',
  default_options: ['buildtype=release', 'warning_level=2'])

cc = meson.get_compiler('c')

# Configuration header generation
conf_data = configuration_data()
jconf_data = configuration_data()
conf_data.set('VERSION', meson.project_version())

has_std_lib = cc.has_header('stdlib.h')
has_std_arg = cc.has_header('stdarg.h')
has_string = cc.has_header('string.h')
has_float = cc.has_header('float.h')

if has_std_lib and has_std_arg and has_string and has_float
  conf_data.set('STDC_HEADERS', 1, description : 'Define to 1 if you have the ANSI C header files.')
endif

bsd_dep = dependency('libbsd', required: false)

headers = {
  'bsd/stdlib.h': bsd_dep,
  'dlfcn.h': [],
  'endian.h': [],
  'fcntl.h': [],
  'inttypes.h': [],
  'limits.h': [],
  'locale.h': [],
  'memory.h': [],
  'stdarg.h': [],
  'stdint.h': [],
  'stdlib.h': [],
  'string.h': [],
  'strings.h': [],
  'syslog.h': [],
  'sys/cdefs.h': [],
  'sys/param.h': [],
  'sys/random.h': [],
  'sys/resource.h': [],
  'sys/stat.h': [],
  'sys/types.h': [],
  'unistd.h': [],
  'xlocale.h': [],
}

foreach h, d : headers
  if cc.has_header(h, dependencies: d)
    conf_data.set('HAVE_@0@'.format(h.underscorify().to_upper()), 1, description :  'Define to 1 if you have the <@0@> header file'.format(h))
  endif
endforeach

if cc.has_header('inttypes.h')
  conf_data.set('JSON_C_HAVE_INTTYPES_H', 1, description : 'Define to 1 if you have the <inttypes.h> header file.')
  jconf_data.set('JSON_C_HAVE_INTTYPES_H', 1, description : 'Define to 1 if you have the <inttypes.h> header file.')
endif

if cc.has_header('stdint.h')
  conf_data.set('JSON_C_HAVE_STDINT_H', 1, description : 'Define to 1 if you have the <stdint.h> header file.')
  jconf_data.set('JSON_C_HAVE_STDINT_H', 1, description : 'Define to 1 if you have the <stdint.h> header file.')
endif

funcs = [
  'open',
  'realloc',
  'setlocale',
  'strdup',
  'strerror',
  'uselocale',
  'duplocale',
  'vsyslog',
  'getrandom',
  'getrusage',
  'strtoll',
  'strtoull',
  'arc4random',
  'vasprintf',
]

if conf_data.has('HAVE_STRINGS_H')
  funcs += ['strcasecmp', 'strncasecmp']
endif

foreach f : funcs
  if cc.has_function(f)
    conf_data.set('HAVE_@0@'.format(f.to_upper()), 1, description : 'Define to 1 if you have the `@0@` function.'.format(f))
  endif
endforeach

foreach f : ['snprintf', 'vsnprintf', 'vprintf']
  if cc.has_header_symbol('stdio.h', f)
    conf_data.set('HAVE_@0@'.format(f.to_upper()), 1, description : 'Define to 1 if you have the `@0@` function.'.format(f))
  endif
endforeach

if not conf_data.has('HAVE_VPRINTF') and cc.has_function('_doprnt')
  conf_data.set('HAVE_DOPRNT', 1, description : 'Define to 1 if you have _doprnt but not vprintf.')
endif

if conf_data.has('HAVE_STRTOLL')
  conf_data.set('json_c_strtoll', 'strtoll')
elif cc.has_function('_strtoi64', prefix : '#include <stdlib.h>')
  conf_data.set('json_c_strtoll', '_strtoi64')
endif

if conf_data.has('HAVE_STRTOULL')
  conf_data.set('json_c_strtoull', 'strtoull')
elif cc.has_function('_strtoui64', prefix : '#include <stdlib.h>')
  conf_data.set('json_c_strtoull', '_strtoui64')
endif

decls = {
  'INFINITY': 'math.h',
  'isinf': 'math.h',
  'isnan': 'math.h',
  'NAN': 'math.h',
  '_finite': 'float.h',
  '_isnan': 'float.h',
}

foreach d, h : decls
  if cc.has_header_symbol(h, d)
    conf_data.set('HAVE_DECL_@0@'.format(d.to_upper()), 1, description : 'Define to 1 if you have the declaration of `@0@`'.format(d))
  endif
endforeach

check_thread = cc.compiles('__thread int x;', name: 'Check for __thread support')

if check_thread
  conf_data.set('HAVE___THREAD', 1)
  conf_data.set('SPEC___THREAD', '__thread')
elif cc.get_argument_syntax() == 'msvc'
  conf_data.set('SPEC___THREAD', '__declspec(thread)')
endif


gnu_warning_section_support = cc.compiles('''
    extern void json_object_get();
    __asm__(".section .gnu.json_object_get\n\t.ascii \"Please link against libjson-c instead of libjson\"\n\t.text");
    int main(int c, char *v) { return 0; }
  ''', name: 'Check for GNU warning section support')
if gnu_warning_section_support
  conf_data.set('HAS_GNU_WARNING_LONG', 1, description : 'Define to 1 if the compiler supports .gnu.warning sections.')
endif

atomic_builtin_support = cc.compiles('''
    int main() {
      int x = 0;
      int i = __sync_add_and_fetch(&x, 1);
      return x;
    }
  ''',
  name: 'Check for atomic builtins')
if atomic_builtin_support
  conf_data.set('HAVE_ATOMIC_BUILTINS', 1, description : 'Define to 1 if the compiler supports atomic builtins.')
endif

if get_option('enable_rdrand')
  conf_data.set('ENABLE_RDRAND', 1)
endif
if get_option('override_get_random_seed')
  conf_data.set('OVERRIDE_GET_RANDOM_SEED', get_option('override_get_random_seed'))
endif
if get_option('enable_threading')
  conf_data.set('ENABLE_THREADING', 1)
endif
if get_option('newlocale_needs_freelocale')
  conf_data.set('NEWLOCALE_NEEDS_FREELOCALE', 1)
endif

conf_data.set('SIZEOF_INT', cc.sizeof('int'))
conf_data.set('SIZEOF_INT64_T', cc.sizeof('int64_t', prefix : '#include <stdint.h>'))
conf_data.set('SIZEOF_LONG', cc.sizeof('long'))
conf_data.set('SIZEOF_LONG_LONG', cc.sizeof('long long'))
conf_data.set('SIZEOF_SIZE_T', cc.sizeof('size_t', prefix : '#include <stddef.h>'))
if cc.get_argument_syntax() == 'msvc'
  conf_data.set('SIZEOF_SSIZE_T', cc.sizeof('SSIZE_T', prefix : '#include <BaseTsd.h>\n#include <stddef.h>'))
else
  conf_data.set('SIZEOF_SSIZE_T', cc.sizeof('ssize_t', prefix : '#include <sys/types.h>'))
endif

conf_data.set('PACKAGE_VERSION', meson.project_version())
conf_data.set('PROJECT_NAME', meson.project_name())

configure_header = configure_file(
  output: 'config.h',
  configuration: conf_data
)

json_configure_header = configure_file(
  output: 'json_config.h',
  configuration: jconf_data
)

jhconf_data = configuration_data()

jhconf_data.set('JSON_H_JSON_PATCH',
  get_option('disable_json_patch') ? '' : '#include "json_patch.h"'
)

jhconf_data.set('JSON_H_JSON_POINTER',
  get_option('disable_json_pointer') ? '' : '#include "json_pointer.h"'
)

json_header = configure_file(
  input: 'json.h.cmakein',
  output: 'json.h',
  configuration: jhconf_data
)

# Platform-specific flags
add_project_arguments('-D_GNU_SOURCE', language: 'c')

if host_machine.system() == 'windows'
  add_project_arguments('-DWIN32', language: 'c')
endif

# Compiler flags
message('target is ' + host_machine.system())
if host_machine.system() == 'windows'
  # Cover any compiler on Windows attempting to use MSVC's standard library
  add_project_arguments('-D_CRT_NONSTDC_NO_DEPRECATE', '-D_CRT_SECURE_NO_WARNINGS', language: 'c')
endif

add_project_arguments(cc.get_supported_arguments('-Wno-unused-parameter'), language : 'c')

sym = cc.get_supported_link_arguments(
  '-Wl,--version-script,@0@/json-c.sym'.format(
    meson.current_source_dir(),
  ),
)

# Source files
sources = files(
  'arraylist.c', 'debug.c', 'json_c_version.c', 'json_object.c',
  'json_object_iterator.c', 'json_tokener.c', 'json_util.c',
  'json_visit.c', 'linkhash.c', 'printbuf.c', 'random_seed.c',
  'strerror_override.c'
)

if not get_option('disable_json_pointer')
  sources += files('json_pointer.c')
  if not get_option('disable_json_patch')
    sources += files('json_patch.c')
  endif
endif

# Include directories
inc = include_directories('.')

# Build library
libjson = library('json-c',
  sources,
  include_directories: inc,
  dependencies: bsd_dep,
  install: true,
  link_args: sym,
  version: '5.4.0',
  soversion: '5',
)

jsonc_dep = declare_dependency(link_with: libjson, include_directories: inc)
meson.override_dependency('json-c', jsonc_dep)

# Install headers
install_headers(
  'arraylist.h', 'debug.h', 'json_c_version.h', 'json_inttypes.h',
  'json_object.h', 'json_object_iterator.h', 'json_tokener.h',
  'json_types.h', 'json_util.h', 'json_visit.h', 'linkhash.h',
  'printbuf.h', json_configure_header, json_header
)

# Optional apps
if get_option('build_apps') and host_machine.system() != 'windows'
  subdir('apps')
endif

# Optional tests
if get_option('buildtype') == 'debug'
  subdir('tests')
endif
