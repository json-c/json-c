This list was created with:

```
PREV=2024-09-15
NOW=2026-06-27
curl "https://api.github.com/search/issues?q=repo%3Ajson-c%2Fjson-c+closed%3A>${PREV}+created%3A<${NOW}&sort=created&order=asc&per_page=100&page=1" > issues1.out
jq -r '.items[] | "[" + .title + "](" + .url + ")" | tostring' issues?.out > issues.md
sed -e's,^\[ *\(.*\)\](https://api.github.com/.*/\([0-9].*\)),* [Issue #\2](https://github.com/json-c/json-c/issues/\2) - \1,' -i issues.md
cat issues.md >> issues_closed_for_0.19.md
```

* [Issue #825](https://github.com/json-c/json-c/issues/825) - Add support for WASI
* [Issue #847](https://github.com/json-c/json-c/issues/847) - Add support for Commodore Amiga
* [Issue #867](https://github.com/json-c/json-c/issues/867) - Parsing of control characters
* [Issue #872](https://github.com/json-c/json-c/issues/872) - fix linkhash breaking -std=c89
* [Issue #873](https://github.com/json-c/json-c/issues/873) - Correct year in changelog for 0.18 release
* [Issue #874](https://github.com/json-c/json-c/issues/874) - comparison with infinity
* [Issue #875](https://github.com/json-c/json-c/issues/875) - Unicode characters seem to be rejected with JSON_TOKENER_STRICT
* [Issue #876](https://github.com/json-c/json-c/issues/876) - json_object_from_fd(_ex) doesn't work with a socket descriptor
* [Issue #877](https://github.com/json-c/json-c/issues/877) - Fix underline bug in README
* [Issue #878](https://github.com/json-c/json-c/issues/878) - Fix underline bug in README
* [Issue #879](https://github.com/json-c/json-c/issues/879) - Handle NULL gracefully in json_tokener_free
* [Issue #880](https://github.com/json-c/json-c/issues/880) - Undefined behavior when coercing NaN value to int
* [Issue #881](https://github.com/json-c/json-c/issues/881) - Tokener with depth=0 causes segmentation fault
* [Issue #882](https://github.com/json-c/json-c/issues/882) - Explicitly handle NaN values when converting to integer
* [Issue #883](https://github.com/json-c/json-c/issues/883) - Add fuzz tests for json_object/point/array apis
* [Issue #884](https://github.com/json-c/json-c/issues/884) - fix macro WIN32_LEAN_AND_MEAN redefined
* [Issue #885](https://github.com/json-c/json-c/issues/885) - A core dump occurs when executing json_object_put.
* [Issue #886](https://github.com/json-c/json-c/issues/886) - Linking to json-c using cmake's find_package error
* [Issue #887](https://github.com/json-c/json-c/issues/887) - Can json-c accept invalid JSON strings?
* [Issue #888](https://github.com/json-c/json-c/issues/888) - Allow build with cmake 4.0.0
* [Issue #891](https://github.com/json-c/json-c/issues/891) - Cmake cant build only static library
* [Issue #892](https://github.com/json-c/json-c/issues/892) - JSON_TOKENER_STRICT rejects U+00BA : MASCULINE ORDINAL INDICATOR
* [Issue #893](https://github.com/json-c/json-c/issues/893) - Fix bug involving supplemental code points that look like high surrogates
* [Issue #894](https://github.com/json-c/json-c/issues/894) - json_object_to_json_string_* trims string to 32767 characters
* [Issue #895](https://github.com/json-c/json-c/issues/895) - CMake: sync minimum version
* [Issue #897](https://github.com/json-c/json-c/issues/897) - error: ‘calloc’ sizes specified with ‘sizeof’ in the earlier argument and not in the later argument
* [Issue #898](https://github.com/json-c/json-c/issues/898) - bug: Fixing Cmake build when using clang-cl
* [Issue #899](https://github.com/json-c/json-c/issues/899) - Meson support
* [Issue #900](https://github.com/json-c/json-c/issues/900) - meson cleanups
* [Issue #901](https://github.com/json-c/json-c/issues/901) - miscellanious gitignore updates
* [Issue #902](https://github.com/json-c/json-c/issues/902) - json_object_get_int() doesn't support INT32_MIN or INT32_MAX.
* [Issue #903](https://github.com/json-c/json-c/issues/903) - meson: use override_dependency
* [Issue #904](https://github.com/json-c/json-c/issues/904) - Include getopt.h header file if the platform is not AIX.
* [Issue #905](https://github.com/json-c/json-c/issues/905) - Fix typo
* [Issue #906](https://github.com/json-c/json-c/issues/906) - json-c: fix calloc argument order to avoid GCC14 -Wcalloc-transposed-args in arraylist.c and json_object.c
* [Issue #907](https://github.com/json-c/json-c/issues/907) - tests/CMakeLists: Allow build with CMake 4
* [Issue #908](https://github.com/json-c/json-c/issues/908) - [json-c-0.18] tests/CMakeLists: Allow build with CMake 4
* [Issue #909](https://github.com/json-c/json-c/issues/909) - compilation error with MSVC in packages that use GNU Autoconf
* [Issue #910](https://github.com/json-c/json-c/issues/910) - Support MSVC in packages that use GNU Autoconf.
* [Issue #911](https://github.com/json-c/json-c/issues/911) - Building libjson-c DLL for Windows under Cygwin: why it's named cygjson-c?
* [Issue #912](https://github.com/json-c/json-c/issues/912) - libjson-c.dll is missing VERSIONINFO
* [Issue #913](https://github.com/json-c/json-c/issues/913) - Add VERSIONINFO to libjson-c.dll
* [Issue #914](https://github.com/json-c/json-c/issues/914) - Memory usage regression due to newlocale() on macOS
* [Issue #915](https://github.com/json-c/json-c/issues/915) - Fix memory leak on macOS by auto-enabling NEWLOCALE_NEEDS_FREELOCALE
* [Issue #916](https://github.com/json-c/json-c/issues/916) - OOM via large array index in json_pointer_set
* [Issue #917](https://github.com/json-c/json-c/issues/917) - Fix: OOM vulnerability cause by is_valid_index
* [Issue #918](https://github.com/json-c/json-c/issues/918) - Assertion failed has occurred in function json_object_object_add_ex at json_object.c
* [Issue #919](https://github.com/json-c/json-c/issues/919) - Compiling json-c in Windows for a non x86 target fails
* [Issue #920](https://github.com/json-c/json-c/issues/920) - Broken link at wiki/List-of-json-c-tutorials
* [Issue #922](https://github.com/json-c/json-c/issues/922) - ninja install target does not install all headers needed
* [Issue #923](https://github.com/json-c/json-c/issues/923) - json-c seems to be stack recursive, which can cause stack overflows
* [Issue #924](https://github.com/json-c/json-c/issues/924) - avoid recursion during json_object_put
* [Issue #925](https://github.com/json-c/json-c/issues/925) - Fix build with clang-21
* [Issue #927](https://github.com/json-c/json-c/issues/927) - Coordinated Vulnerability Disclosure - json-c 0.18.99 Use-After-Free via json_object_iterator Concurrent Modification
* [Issue #928](https://github.com/json-c/json-c/issues/928) - Build error from current top of tree: d1018cf
* [Issue #929](https://github.com/json-c/json-c/issues/929) - Coordinated Vulnerability Disclosure - json-c 0.18.99
* [Issue #930](https://github.com/json-c/json-c/issues/930) - fix locale-dependent strtod in json_object_get_double
* [Issue #931](https://github.com/json-c/json-c/issues/931) - deep copy values in json_patch copy op to avoid aliasing and cycles
