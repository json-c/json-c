
appconf_data = configuration_data()
# Check for json_tokener_get_parse_end symbol
appconf_data.set('HAVE_JSON_TOKENER_GET_PARSE_END',
  cc.has_function('json_tokener_get_parse_end', prefix: '#include <json_tokener.h>') ? 1 : 0
)

# Check for getrusage if sys/resource.h is available
appconf_data.set('HAVE_SYS_RESOURCE_H', cc.has_header('sys/resource.h') ? 1 : 0, description: 'Define to 1 if you have the <sys/resource.h> header file.')
if appconf_data.get('HAVE_SYS_RESOURCE_H') == 1
  appconf_data.set('HAVE_GETRUSAGE',
    cc.has_function('getrusage', prefix: '#include <sys/resource.h>') ? 1 : 0, description: 'Define if you have the `getrusage` function. ')
else 
  appconf_data.set('HAVE_GETRUSAGE', 0, description: 'Define if you have the `getrusage` function. ')
endif

# Generate apps_config.h
configure_file(
  output: 'apps_config.h',
  configuration: appconf_data
)

# Build json_parse executable
executable('json_parse', 'json_parse.c',
  dependencies: [jsonc_dep],
  install: false
)
