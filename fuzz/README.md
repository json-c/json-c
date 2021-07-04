# Fuzzers

This directory contains fuzzers that
target [llvm's LibFuzzer](https://llvm.org/docs/LibFuzzer.html). They are built
and run automatically by
Google's [OSS-Fuzz](https://github.com/google/oss-fuzz/) infrastructure.

## How do I test or run the fuzzers like oss-fuzz?

```
git clone https://github.com/google/oss-fuzz.git
cd oss-fuzz
python infra/helper.py build_image json-c
python infra/helper.py build_fuzzers --sanitizer address --engine libfuzzer --architecture x86_64 json-c
python infra/helper.py run_fuzzer json-c tokener_parse_ex_fuzzer
```

## How do I add new unit or regression tests for the fuzzer?

The tests directory contains json files that can be used to either test the fuzzer itself or be used as regression tests. As long as the files end in `.json`, cmake will pick them up and generate a Ctest test case. If/when oss-fuzz finds a bug with a fuzzer, simply pull that test case into the `./tests` directory and it will serve as a regression test.

Note - the fuzzers are not being run with sanitizers in this repository's CI at the moment; we're strictly building them here to ensure that they function.

## How do I reproduce a failure form a fuzzer?

Use [the steps detailed on OSS-fuzz](https://google.github.io/oss-fuzz/advanced-topics/reproducing/).
