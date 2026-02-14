#This hack is needed for Windows tests
test_lib = static_library(
  'jsonctest',
  objects: libjson.extract_all_objects(
    recursive: false,
  ),
  install: false,
)

test_deps = declare_dependency(
  include_directories: '..',
  link_with: test_lib,
)

# List of test sources and expected output files
test_cases = [
  ['test1', 'test1.expected'],
  ['test2', 'test2.expected'],
  ['test4', 'test4.expected'],
  ['testReplaceExisting', 'testReplaceExisting.expected'],
  ['test_cast', 'test_cast.expected'],
  ['test_charcase', 'test_charcase.expected'],
  ['test_compare', 'test_compare.expected'],
  ['test_deep_copy', 'test_deep_copy.expected'],
  ['test_double_serializer', 'test_double_serializer.expected'],
  ['test_float', 'test_float.expected'],
  ['test_int_add', 'test_int_add.expected'],
  ['test_int_get', 'test_int_get.expected'],
  ['test_locale', 'test_locale.expected'],
  ['test_null', 'test_null.expected'],
  ['test_parse', 'test_parse.expected'],
  ['test_parse_int64', 'test_parse_int64.expected'],
  ['test_printbuf', 'test_printbuf.expected'],
  ['test_set_serializer', 'test_set_serializer.expected'],
  ['test_set_value', 'test_set_value.expected'],
  ['test_strerror', 'test_strerror.expected'],
  ['test_util_file', 'test_util_file.expected'],
  ['test_visit', 'test_visit.expected'],
  ['test_object_iterator', 'test_object_iterator.expected'],
  ['test_json_pointer', 'test_json_pointer.expected'],
  ['test_safe_json_pointer_set', 'test_safe_json_pointer_set.expected'],
  ['test_json_patch', 'test_json_patch.expected'],
]

# Copy expected files and test data
expected_files = []
foreach t : test_cases
  expected_files += t[1]
endforeach

foreach f : expected_files + ['valid.json', 'valid_nested.json', 'json_patch_spec_tests.json', 'json_patch_tests.json']
  configure_file(input: f, output: f, copy: true)
endforeach

# Build and register tests
special_args = {
  'test_json_patch': ['.'],
  'test_util_file': ['.'],
}

testdir = meson.current_build_dir()
message('Test data directory: ' + testdir)

foreach t : test_cases
  name = t[0]
  expected = t[1]
  exe = executable(name, name + '.c',
    dependencies: test_deps
  )

  test(name, exe,
    args: special_args.get(name, []),
    env: ['EXPECTED_FILE=' + meson.current_build_dir() / expected],
    workdir: testdir
  )
endforeach
