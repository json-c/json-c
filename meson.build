
project('json-c', 'c', version: '0.18.99',
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

if cc.has_header('dlfcn.h')
  conf_data.set('HAVE_DLFCN_H', 1, description : 'Define to 1 if you have the <dlfcn.h> header file.')
endif

if cc.has_header('endian.h')
  conf_data.set('HAVE_ENDIAN_H', 1, description : 'Define to 1 if you have the <endian.h> header file')
endif 

if cc.has_header('fcntl.h')
  conf_data.set('HAVE_FCNTL_H', 1, description : 'Define to 1 if you have the <fcntl.h> header file.')
endif

if cc.has_header('inttypes.h')
  conf_data.set('HAVE_INTTYPES_H', 1, description : 'Define to 1 if you have the <inttypes.h> header file.')
  conf_data.set('JSON_C_HAVE_INTTYPES_H', 1, description : 'Define to 1 if you have the <inttypes.h> header file.')
  jconf_data.set('JSON_C_HAVE_INTTYPES_H', 1, description : 'Define to 1 if you have the <inttypes.h> header file.')
endif 

if cc.has_header('limits.h')
  conf_data.set('HAVE_LIMITS_H', 1, description : 'Define to 1 if you have the <limits.h> header file.')
endif

if cc.has_header('locale.h')
  conf_data.set('HAVE_LOCALE_H', 1, description : 'Define to 1 if you have the <locale.h> header file.')
endif
if cc.has_header('memory.h')
  conf_data.set('HAVE_MEMORY_H', 1, description : 'Define to 1 if you have the <memory.h> header file.')
endif
if has_std_arg
  conf_data.set('HAVE_STDARG_H', 1, description : 'Define to 1 if you have the <stdarg.h> header file.')
endif
if cc.has_header('stdint.h')
  conf_data.set('HAVE_STDINT_H', 1, description : 'Define to 1 if you have the <stdint.h> header file.')
  conf_data.set('JSON_C_HAVE_STDINT_H', 1, description : 'Define to 1 if you have the <stdint.h> header file.')
  jconf_data.set('JSON_C_HAVE_STDINT_H', 1, description : 'Define to 1 if you have the <stdint.h> header file.')
endif
if has_std_lib
  conf_data.set('HAVE_STDLIB_H', 1, description : 'Define to 1 if you have the <stdlib.h> header file.')
endif
if cc.has_header('strings.h')
  conf_data.set('HAVE_STRINGS_H', 1, description : 'Define to 1 if you have the <strings.h> header file.')
  if cc.has_function('strcasecmp', prefix : '#include <strings.h>')
    conf_data.set('HAVE_STRCASECMP', 1, description : 'Define to 1 if you have the `strcasecmp` function.')
  endif
  if cc.has_function('strncasecmp', prefix : '#include <strings.h>')
    conf_data.set('HAVE_STRNCASECMP', 1, description : 'Define to 1 if you have the `strncasecmp` function.')
  endif
endif
if has_string
  conf_data.set('HAVE_STRING_H', 1, description : 'Define to 1 if you have the <string.h> header file.')
endif
if cc.has_header('syslog.h')
  conf_data.set('HAVE_SYSLOG_H', 1, description : 'Define to 1 if you have the <syslog.h> header file.')
endif
if cc.has_header('sys/cdefs.h')
  conf_data.set('HAVE_SYS_CDEFS_H', 1, description : 'Define to 1 if you have the <sys/cdefs.h> header file.')
endif
if cc.has_header('sys/param.h')
  conf_data.set('HAVE_SYS_PARAM_H', 1, description : 'Define to 1 if you have the <sys/param.h> header file.')
endif
if cc.has_header('sys/random.h')
  conf_data.set('HAVE_SYS_RANDOM_H', 1, description : 'Define to 1 if you have the <sys/random.h> header file.')
endif
if cc.has_header('sys/resource.h')
  conf_data.set('HAVE_SYS_RESOURCE_H', 1, description : 'Define to 1 if you have the <sys/resource.h> header file.')
endif
if cc.has_header('sys/stat.h')
  conf_data.set('HAVE_SYS_STAT_H', 1, description : 'Define to 1 if you have the <sys/stat.h> header file.')
endif
if cc.has_header('sys/types.h')
  conf_data.set('HAVE_SYS_TYPES_H', 1, description : 'Define to 1 if you have the <sys/types.h> header file.')
endif
if cc.has_header('unistd.h')
  conf_data.set('HAVE_UNISTD_H', 1, description : 'Define to 1 if you have the <unistd.h> header file.')
endif
if cc.has_header('xlocale.h')
  conf_data.set('HAVE_XLOCALE_H', 1, description : 'Define to 1 if you have the <xlocale.h> header file.')
endif
has_bsd_stdlib = cc.has_header('bsd/stdlib.h')
if has_bsd_stdlib
  conf_data.set('HAVE_BSD_STDLIB_H', 1, description : 'Define to 1 if you have the <bsd/stdlib.h> header file.')
endif
if cc.has_function('vprintf', prefix : '#include <stdarg.h>\n#include <stdio.h>')
  conf_data.set('HAVE_VPRINTF', 1, description : 'Define to 1 if you have the `vprintf` function.')
elif cc.has_function('_doprnt')
  conf_data.set('HAVE_DOPRNT', 1, description : 'Define to 1 if you have _doprnt but not vprintf.') 
endif
if cc.has_define('INFINITY', prefix : '#include <float.h>\n#include <math.h>')
  conf_data.set('HAVE_DECL_INFINITY', 1, description : 'Define to 1 if you have the declaration of `INFINITY`')
endif
if cc.has_define('isinf', prefix : '#include <float.h>\n#include <math.h>') or cc.has_function('isinf', prefix : '#include <float.h>\n#include <math.h>')
  conf_data.set('HAVE_DECL_ISINF', 1, description : 'Define to 1 if you have the declaration of `isinf`')
endif
if cc.has_define('isnan', prefix : '#include <float.h>\n#include <math.h>') or cc.has_function('isnan', prefix : '#include <float.h>\n#include <math.h>')
  conf_data.set('HAVE_DECL_ISNAN', 1, description : 'Define to 1 if you have the declaration of `isnan`')
endif
if cc.has_define('NAN', prefix : '#include <float.h>\n#include <math.h>')
  conf_data.set('HAVE_DECL_NAN', 1, description : 'Define to 1 if you have the declaration of `NAN`')
endif
if cc.has_define('_finite', prefix : '#include <float.h>\n#include <math.h>') or cc.has_function('_finite', prefix : '#include <float.h>\n#include <math.h>')
  conf_data.set('HAVE_DECL__FINITE', 1, description : 'Define to 1 if you have the declaration of `_finite`')
endif
if cc.has_define('_isnan', prefix : '#include <float.h>\n#include <math.h>') or cc.has_function('_isnan', prefix : '#include <float.h>\n#include <math.h>')
  conf_data.set('HAVE_DECL__ISNAN', 1, description : 'Define to 1 if you have the declaration of `_isnan`')
endif

if cc.has_function('open', prefix : '#include <fcntl.h>')
  conf_data.set('HAVE_OPEN', 1, description : 'Define to 1 if you have the `open` function.')
endif
if cc.has_function('realloc', prefix : '#include <stdlib.h>')
  conf_data.set('HAVE_REALLOC', 1, description : 'Define to 1 if you have the `realloc` function.')
endif
if cc.has_function('setlocale', prefix : '#include <locale.h>')
  conf_data.set('HAVE_SETLOCALE', 1, description : 'Define to 1 if you have the `setlocale` function.')
endif
if cc.has_function('snprintf', prefix : '#include <stdio.h>')
  conf_data.set('HAVE_SNPRINTF', 1, description : 'Define to 1 if you have the `snprintf` function.')
endif
if cc.has_function('strdup')
  conf_data.set('HAVE_STRDUP', 1, description : 'Define to 1 if you have the `strdup` function.')
endif
if cc.has_function('strerror')
  conf_data.set('HAVE_STRERROR', 1, description : 'Define to 1 if you have the `strerror` function.')
endif

if cc.has_function('uselocale', prefix : '#include <locale.h>')
  conf_data.set('HAVE_USELOCALE', 1, description : 'Define to 1 if you have the `uselocale` function.')
endif
if cc.has_function('duplocale', prefix : '#include <locale.h>')
  conf_data.set('HAVE_DUPLOCALE', 1, description : 'Define to 1 if you have the `duplocale` function.')
endif
if cc.has_function('vasprintf', prefix : '#define _GNU_SOURCE\n#include <stdarg.h>\n#include <stdio.h>')
  conf_data.set('HAVE_VASPRINTF', 1, description : 'Define to 1 if you have the `vasprintf` function.')
endif
if cc.has_function('vsnprintf', prefix : '#include <stdarg.h>\n#include <stdio.h>')
  conf_data.set('HAVE_VSNPRINTF', 1, description : 'Define to 1 if you have the `vsnprintf` function.')
endif
if cc.has_function('vsyslog')
  conf_data.set('HAVE_VSYSLOG', 1, description : 'Define to 1 if you have the `vsyslog` function.')
endif
if cc.has_function('getrandom')
  conf_data.set('HAVE_GETRANDOM', 1, description : 'Define to 1 if you have the `getrandom` function.')
endif
if cc.has_function('getrusage', prefix : '#include <sys/resource.h>')
  conf_data.set('HAVE_GETRUSAGE', 1, description : 'Define to 1 if you have the `getrusage` function.')
endif

have_strtoll = cc.has_function('strtoll')
have_strtoull = cc.has_function('strtoull')

if have_strtoll
  conf_data.set('HAVE_STRTOLL', 1)
  conf_data.set('json_c_strtoll', 'strtoll')
elif cc.has_function('_strtoi64', prefix : '#include <stdlib.h>')
  conf_data.set('json_c_strtoll', '_strtoi64')
endif

if have_strtoull
  conf_data.set('HAVE_STRTOULL', 1)
  conf_data.set('json_c_strtoull', 'strtoull')
elif cc.has_function('_strtoui64', prefix : '#include <stdlib.h>')
  conf_data.set('json_c_strtoull', '_strtoui64')
endif

check_thread = cc.compiles('''
    __thread int x = 0;
    int main() { return x; }
  ''',
  name: 'Check for __thread support')

if check_thread
  conf_data.set('HAVE___THREAD', 1)
  conf_data.set('SPEC___THREAD', '__thread')
elif cc.get_id().contains('msvc')
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

if has_bsd_stdlib
  if cc.has_function('arc4random', prefix: '#include <bsd/stdlib.h>')
    conf_data.set('HAVE_ARC4RANDOM', 1)
  endif
else
  if cc.has_function('arc4random', prefix: '#include <stdlib.h>')
    conf_data.set('HAVE_ARC4RANDOM', 1)
  endif
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
conf_data.set('SIZEOF_SIZE_T', cc.sizeof('size_t'))
if target_machine.system() == 'windows'
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
message('target is ' + target_machine.system())
if target_machine.system() == 'windows'
  # Cover any compiler on Windows attempting to use MSVC's standard library
  add_project_arguments(['-D_CRT_NONSTDC_NO_DEPRECATE', '-D_CRT_SECURE_NO_WARNINGS'], language: 'c')
endif 

if cc.get_id().contains('gcc') or cc.get_id().contains('clang')
  add_project_arguments(cc.get_supported_arguments(['-Wno-unused-parameter']), language : 'c')
endif

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
  install: true,
  version: '5.4.0',
  soversion: '5',
)

jsonc_dep = declare_dependency(link_with: libjson, include_directories: inc)

# Install headers
install_headers(
  'arraylist.h', 'debug.h', 'json_c_version.h', 'json_inttypes.h',
  'json_object.h', 'json_object_iterator.h', 'json_tokener.h',
  'json_types.h', 'json_util.h', 'json_visit.h', 'linkhash.h',
  'printbuf.h', json_configure_header, json_header
)

# Optional apps
if get_option('build_apps') and target_machine.system() != 'windows'
  subdir('apps')
endif

# Optional tests
if get_option('buildtype') == 'debug'
  subdir('tests')
endif
