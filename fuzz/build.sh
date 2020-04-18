#!/bin/bash -eu
# Copyright 2018 Google Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
################################################################################
# this is expected to be run only by oss-fuzz. It will run from $SRC (above json-c)
cp $SRC/json-c/fuzz/*.dict "$OUT/"

BUILD="$SRC/json-c/build"

zip -j "$SRC/corpus.zip" "$SRC/go-fuzz-corpus/json/corpus"

mkdir "$BUILD"
cd "$BUILD"
cmake -DCMAKE_C_FLAGS="$CFLAGS" -DCMAKE_CXX_FLAGS="$CXXFLAGS" -DBUILD_SHARED_LIBS=OFF ..
make -j$(nproc)
cp fuzz/*_fuzzer "$OUT/"

fuzzerFiles=$(find fuzz/ -name "*_fuzzer")

for F in $fuzzerFiles; do
	FN=$(basename -- $F)
	cp "$SRC/corpus.zip" "$OUT/${FN}_seed_corpus.zip"
done
