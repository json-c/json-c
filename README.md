`json-c`
========

1. [Overview and Build Status](#overview)
2. [Building on Unix](#buildunix)
3. [Install Prerequisites](#installprereq)
4. [Building with partial threading support](#buildthreaded)
5. [Building with CMake](#CMake)
6. [Linking to libjson-c](#linking)
7. [Using json-c](#using)

JSON-C - A JSON implementation in C <a name="overview"></a>
-----------------------------------

Build Status
* [AppVeyor Build](https://ci.appveyor.com/project/hawicz/json-c) ![AppVeyor Build Status](https://ci.appveyor.com/api/projects/status/github/json-c/json-c?branch=master&svg=true)
* [Travis Build](https://travis-ci.org/json-c/json-c) ![Travis Build Status](https://travis-ci.org/json-c/json-c.svg?branch=master)

JSON-C implements a reference counting object model that allows you to easily
construct JSON objects in C, output them as JSON formatted strings and parse
JSON formatted strings back into the C representation of JSON objects.
It aims to conform to [RFC 7159](https://tools.ietf.org/html/rfc7159).

Building on Unix and Windows with `vcpkg`, `gcc`/`g++`, `curl`, `unzip`, and `tar`
--------------------------------------------------

You can download and install JSON-C using the [vcpkg](https://github.com/Microsoft/vcpkg/) dependency manager:

    git clone https://github.com/Microsoft/vcpkg.git
    cd vcpkg
    ./bootstrap-vcpkg.sh
    ./vcpkg integrate install
    vcpkg install json-c

The JSON-C port in vcpkg is kept up to date by Microsoft team members and community contributors. If the version is out of date, please [create an issue or pull request](https://github.com/Microsoft/vcpkg) on the vcpkg repository.

Building on Unix with `git`, `gcc` and `autotools` <a name="buildunix"></a>
--------------------------------------------------

Home page for json-c: https://github.com/json-c/json-c/wiki

### Prerequisites:

See also the "Installing prerequisites" section below.

 - `gcc`, `clang`, or another C compiler
 - `libtool>=2.2.6b`

If you're not using a release tarball, you'll also need:

 - `autoconf>=2.64` (`autoreconf`)
 - `automake>=1.13`

Make sure you have a complete `libtool` install, including `libtoolize`.

To generate docs (e.g. as part of make distcheck) you'll also need:
 - `doxygen>=1.8.13`

### Build instructions:

`json-c` GitHub repo: https://github.com/json-c/json-c

```sh
$ git clone https://github.com/json-c/json-c.git
$ cd json-c
$ sh autogen.sh
```

followed by

```sh
$ ./configure  # --enable-threading
$ make
$ make install
```

To build and run the test programs:

```sh
$ make check
$ make USE_VALGRIND=0 check   # optionally skip using valgrind
```

Install prerequisites <a name="installprereq"></a>
-----------------------

If you are on a relatively modern system, you'll likely be able to install
the prerequisites using your OS's packaging system.

### Install using apt (e.g. Ubuntu 16.04.2 LTS)
```sh
sudo apt install git
sudo apt install autoconf automake libtool
sudo apt install valgrind # optional
```

Then start from the "git clone" command, above.

### Manually install and build autoconf, automake and libtool

For older OS's that don't have up-to-date versions of the packages will
require a bit more work. For example, CentOS release 5.11, etc...

```sh
curl -O http://ftp.gnu.org/gnu/autoconf/autoconf-2.69.tar.gz
curl -O http://ftp.gnu.org/gnu/automake/automake-1.15.tar.gz
curl -O http://ftp.gnu.org/gnu/libtool/libtool-2.2.6b.tar.gz

tar xzf autoconf-2.69.tar.gz
tar xzf automake-1.15.tar.gz
tar xzf libtool-2.2.6b.tar.gz

export PATH=${HOME}/ac_install/bin:$PATH

(cd autoconf-2.69 && \
  ./configure --prefix ${HOME}/ac_install && \
  make && \
  make install)

(cd automake-1.15 && \
  ./configure --prefix ${HOME}/ac_install && \
  make && \
  make install)

(cd libtool-2.2.6b && \
  ./configure --prefix ${HOME}/ac_install && \
  make && \
  make install)
```


Building with partial threading support <a name="buildthreaded"></a>
----------------------------------------

Although json-c does not support fully multi-threaded access to
object trees, it has some code to help make its use in threaded programs
a bit safer.  Currently, this is limited to using atomic operations for
json_object_get() and json_object_put().

Since this may have a performance impact, of at least 3x slower
according to https://stackoverflow.com/a/11609063, it is disabled by
default.  You may turn it on by adjusting your configure command with:
   --enable-threading

Separately, the default hash function used for object field keys,
lh_char_hash, uses a compare-and-swap operation to ensure the random
seed is only generated once.  Because this is a one-time operation, it
is always compiled in when the compare-and-swap operation is available.

Building with CMake <a name="CMake"></a>
--------------------

To use [CMake](https://cmake.org/cmake-tutorial/), build it like:

```sh
mkdir build
cd build
cmake ../
make
```

CMake can take a few options.

Variable             | Type   | Description
---------------------|--------|--------------
CMAKE_INSTALL_PREFIX | String | The install location.
BUILD_SHARED_LIBS    | Bool   | The default build generates a dynamic (dll/so) library.  Set this to OFF to create a static library instead.
ENABLE_RDRAND        | Bool   | Enable RDRAND Hardware RNG Hash Seed
ENABLE_THREADING     | Bool   | Enable partial threading support

Pass these options as `-D` on CMake's command-line.

```sh
cmake -DBUILD_SHARED_LIBS=OFF ...
```

Testing with cmake:

By default, if valgrind is available running tests uses it.
That can slow the tests down considerably, so to disable it use:
```sh
export USE_VALGRIND=0
```

To run tests:
```sh
mkdir build-test
cd build-test
# VALGRIND=1 causes -DVALGRIND=1 to be included when building
VALGRIND=1 cmake ..
make

make test
# By default, if valgrind is available running tests uses it.
make USE_VALGRIND=0 test   # optionally skip using valgrind
```

If a test fails, check `Testing/Temporary/LastTest.log`, 
`tests/testSubDir/${testname}/${testname}.vg.out`, and other similar files.
If there is insufficient output try:
```sh
VERBOSE=1 make test
```
or
```sh
JSONC_TEST_TRACE=1 make test
```
and check the log files again.



Linking to `libjson-c` <a name="linking">
----------------------

If your system has `pkgconfig`,
then you can just add this to your `makefile`:

```make
CFLAGS += $(shell pkg-config --cflags json-c)
LDFLAGS += $(shell pkg-config --libs json-c)
```

Without `pkgconfig`, you would do something like this:

```make
JSON_C_DIR=/path/to/json_c/install
CFLAGS += -I$(JSON_C_DIR)/include/json-c
LDFLAGS+= -L$(JSON_C_DIR)/lib -ljson-c
```


Using json-c <a name="using">
------------

To use json-c you can either include json.h, or preferrably, one of the
following more specific header files:

* json_object.h  - Core types and methods.
* json_tokener.h - Methods for parsing and serializing json-c object trees.
* json_pointer.h - JSON Pointer (RFC 6901) implementation for retrieving
                   objects from a json-c object tree.
* json_object_iterator.h - Methods for iterating over single json_object instances.
* json_visit.h   - Methods for walking a tree of json-c objects.
* json_util.h    - Miscelleanous utility functions.

For a full list of headers see [files.html](http://json-c.github.io/json-c/json-c-0.13.1/doc/html/files.html)

