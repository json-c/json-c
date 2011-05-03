AM_CFLAGS = -Wall -Wwrite-strings -Werror -std=gnu99 -D_GNU_SOURCE -D_REENTRANT

EXTRA_DIST = README.html README-WIN32.html config.h.win32 doc json-c.vcproj

lib_LTLIBRARIES = libjson.la

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = json.pc

libjsonincludedir = $(includedir)/json
libjsoninclude_HEADERS = \
	arraylist.h \
	bits.h \
	debug.h \
	json.h \
	json_inttypes.h \
	json_object.h \
	json_object_private.h \
	json_tokener.h \
	json_util.h \
	linkhash.h \
	printbuf.h

libjson_la_LDFLAGS = -version-info 1:0:1

libjson_la_SOURCES = \
	arraylist.c \
	debug.c \
	json_object.c \
	json_tokener.c \
	json_util.c \
	linkhash.c \
	printbuf.c

check_PROGRAMS = test1 test2 test4 test_parse_int64 test_null test_cast

test1_SOURCES = test1.c
test1_LDADD = $(lib_LTLIBRARIES)

test2_SOURCES = test2.c
test2_LDADD = $(lib_LTLIBRARIES)

test4_SOURCES = test4.c
test4_LDADD = $(lib_LTLIBRARIES)

test_parse_int64_SOURCES = test_parse_int64.c
test_parse_int64_LDADD = $(lib_LTLIBRARIES)

test_null_SOURCES = test_null.c
test_null_LDADD = $(lib_LTLIBRARIES)

test_cast_SOURCES = test_cast.c
test_cast_LDADD = $(lib_LTLIBRARIES)

TESTS = test1.test test2.test test4.test parse_int64.test test_null.test test_cast.test 
EXTRA_DIST += $(TESTS)
testsubdir=testSubDir
TESTS_ENVIRONMENT       = top_builddir=$(top_builddir)

distclean-local:
	-rm -rf $(testsubdir)

