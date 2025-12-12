This list was created with:

```
PREV=2023-08-12
NOW=2024-09-15
curl "https://api.github.com/search/issues?q=repo%3Ajson-c%2Fjson-c+closed%3A>${PREV}+created%3A<${NOW}&sort=created&order=asc&per_page=100&page=1" > issues1.out
jq -r '.items[] | "[" + .title + "](" + .url + ")" | tostring' issues?.out > issues.md
sed -e's,^\[ *\(.*\)\](https://api.github.com/.*/\([0-9].*\)),* [Issue #\2](https://github.com/json-c/json-c/issues/\2) - \1,' -i issues.md
cat issues.md >> issues_closed_for_0.18.md
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
