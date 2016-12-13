`json-c`                       {#mainpage}
========

JSON-C - A JSON implementation in C
-----------------------------------

JSON-C implements a reference counting object model that allows you to easily 
construct JSON objects in C, output them as JSON formatted strings and parse 
JSON formatted strings back into the C representation of JSON objects.
It aims to conform to [RFC 7159](https://tools.ietf.org/html/rfc7159).

Building on Unix with `git`, `gcc` and `autotools`
--------------------------------------------------

Home page for json-c: https://github.com/json-c/json-c/wiki

Prerequisites:

 - `gcc`, `clang`, or another C compiler
 - `libtool>=2.2.6b`

If you're not using a release tarball, you'll also need:

 - `autoconf>=2.64` (`autoreconf`)
 - `automake>=1.10.3`

Make sure you have a complete `libtool` install, including `libtoolize`.

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
