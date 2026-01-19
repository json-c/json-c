This list was created with:

```
PREV=2024-09-15
NOW=2026-01-19
curl "https://api.github.com/search/issues?q=repo%3Ajson-c%2Fjson-c+closed%3A>${PREV}+created%3A<${NOW}&sort=created&order=asc&per_page=100&page=1" > issues1.out
jq -r '.items[] | "[" + .title + "](" + .url + ")" | tostring' issues?.out > issues.md
sed -e's,^\[ *\(.*\)\](https://api.github.com/.*/\([0-9].*\)),* [Issue #\2](https://github.com/json-c/json-c/issues/\2) - \1,' -i issues.md
cat issues.md >> issues_closed_for_0.19.md
```

* [Issue #826](https://github.com/json-c/json-c/issues/826) - CMake: remove needless policy setting
* [Issue #827](https://github.com/json-c/json-c/issues/827) - Resolve build issue in tokener_parse_ex_fuzzer.c
* [Issue #828](https://github.com/json-c/json-c/issues/828) - Put arc4random check under DISABLE_EXTRA_LIBS=OFF in CMakeLists.txt.
* [Issue #829](https://github.com/json-c/json-c/issues/829) - Cannot build with clang-cl
* [Issue #830](https://github.com/json-c/json-c/issues/830) - I am looking whether this CVE - CVE-2021-32292 is impacted by json-c version v0.13.1or not ?
* [Issue #831](https://github.com/json-c/json-c/issues/831) - rename WIN32 to _WIN32
* [Issue #833](https://github.com/json-c/json-c/issues/833) - memory leak issue in 0.13
* [Issue #834](https://github.com/json-c/json-c/issues/834) - fix error des of 'error_memory'
* [Issue #835](https://github.com/json-c/json-c/issues/835) - fix error des of 'error_memory'
* [Issue #836](https://github.com/json-c/json-c/issues/836) - Null pointer dereference in tokener_parse_ex_fuzzer.cc
* [Issue #837](https://github.com/json-c/json-c/issues/837) - Add type checking for parsed objects in tokener_parse_ex_fuzzer
* [Issue #838](https://github.com/json-c/json-c/issues/838) - Cannot link properly using cmake
* [Issue #839](https://github.com/json-c/json-c/issues/839) - json_pointer.c: initialize idx
* [Issue #840](https://github.com/json-c/json-c/issues/840) - Memory leak when enable HAVE_SETLOCALE and HAVE_USELOCALE
* [Issue #841](https://github.com/json-c/json-c/issues/841) - Random crash in json_tokener_parse
* [Issue #842](https://github.com/json-c/json-c/issues/842) - Assignment bug in json_pointer.c
* [Issue #843](https://github.com/json-c/json-c/issues/843) - _WIN32 should be used, not WIN32 in source code ifdefs.
* [Issue #844](https://github.com/json-c/json-c/issues/844) - Doxygen: mis-wording in `json_object_put`
* [Issue #845](https://github.com/json-c/json-c/issues/845) - [CMP0153](https://cmake.org/cmake/help/latest/policy/CMP0153.html)
* [Issue #846](https://github.com/json-c/json-c/issues/846) - CMP0153
* [Issue #848](https://github.com/json-c/json-c/issues/848) - json_object_from_file caused calloc SIGSEGV
* [Issue #849](https://github.com/json-c/json-c/issues/849) - random_seed.c: add a Coverity Scan suppression
* [Issue #850](https://github.com/json-c/json-c/issues/850) - Missing return description
* [Issue #851](https://github.com/json-c/json-c/issues/851) - Allow NULL in json_object_new_string() and json_object_new_string_len()
* [Issue #852](https://github.com/json-c/json-c/issues/852) - docs: micro typo in json_object_put doc
* [Issue #853](https://github.com/json-c/json-c/issues/853) - json_tokener_parse_verbose does not set the error indicator when it fails to allocate the tokener
* [Issue #854](https://github.com/json-c/json-c/issues/854) - json_tokener_parse_verbose sets the error indicator to a wrong value when there is a memory allocation failure
* [Issue #856](https://github.com/json-c/json-c/issues/856) - Using libjson-c.so or compiling from source seg faults on json_tokener_parse_ex in alpine docker container
* [Issue #857](https://github.com/json-c/json-c/issues/857) - json_tokener_parse_verbose sets the error indicator to a wrong value when there is a memory allocation failure
* [Issue #858](https://github.com/json-c/json-c/issues/858) - json_tokener.h: json_tokener_parse_verbose: fix typo
* [Issue #859](https://github.com/json-c/json-c/issues/859) - Develop
* [Issue #860](https://github.com/json-c/json-c/issues/860) - Develop
* [Issue #861](https://github.com/json-c/json-c/issues/861) - Handle yet another out-of-memory condition.
* [Issue #862](https://github.com/json-c/json-c/issues/862) - Missing `json-c-targets.cmake` during build cmake build step
* [Issue #863](https://github.com/json-c/json-c/issues/863) - build ios simulator error
* [Issue #864](https://github.com/json-c/json-c/issues/864) - Build pkg-config for msvc as well
* [Issue #865](https://github.com/json-c/json-c/issues/865) - Assertion usage
* [Issue #866](https://github.com/json-c/json-c/issues/866) - Added github actions
* [Issue #868](https://github.com/json-c/json-c/issues/868) - I meet compile error in harmony  
* [Issue #869](https://github.com/json-c/json-c/issues/869) - At a high level how are jsons parses? Are they terminated by the new line?
* [Issue #870](https://github.com/json-c/json-c/issues/870) - Non-Compliant features should be optional (and disabled)
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
