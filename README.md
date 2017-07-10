`json-c`                       {#mainpage}
========

JSON-C - A JSON implementation in C
-----------------------------------

Build Status
* [![AppVeyor Build Status](https://ci.appveyor.com/api/projects/status/github/json-c/json-c?branch=master&svg=true)](https://ci.appveyor.com/project/hawicz/json-c)
* [![Travis Build Status](https://travis-ci.org/json-c/json-c.svg?branch=master)](https://travis-ci.org/json-c/json-c)

JSON-C implements a reference counting object model that allows you to easily 
construct JSON objects in C, output them as JSON formatted strings and parse 
JSON formatted strings back into the C representation of JSON objects.
It aims to conform to [RFC 7159](https://tools.ietf.org/html/rfc7159).

Building on Unix with `git`, `gcc` and `autotools`
--------------------------------------------------

Home page for json-c: https://github.com/json-c/json-c/wiki

### Prerequisites:

See also the "Installing prerequisites" section below.

 - `gcc`, `clang`, or another C compiler
 - `libtool>=2.2.6b`

If you're not using a release tarball, you'll also need:

 - `autoconf>=2.64` (`autoreconf`)
 - `automake>=1.10.3`

Make sure you have a complete `libtool` install, including `libtoolize`.

### Build instructions:

`json-c` GitHub repo: https://github.com/json-c/json-c

```sh
$ git clone https://github.com/json-c/json-c.git
$ cd json-c
$ sh autogen.sh
```

followed by

```sh
$ ./configure
$ make
$ make install
```

To build and run the test programs:

```sh
$ make check
```

Linking to `libjson-c`
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


Install prerequisites
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

For older OS's that don't have up-to-date version of the packages will
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

