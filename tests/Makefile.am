
include ../Makefile.am.inc

LIBJSON_LA=$(top_builddir)/libjson.la

check_PROGRAMS = test1 test2 test4 test_parse_int64 test_null test_cast test_parse

test1_LDADD = $(LIBJSON_LA)

test2_LDADD = $(LIBJSON_LA)

test4_LDADD = $(LIBJSON_LA)

test_parse_int64_LDADD = $(LIBJSON_LA)

test_null_LDADD = $(LIBJSON_LA)

test_cast_LDADD = $(LIBJSON_LA)

test_parse_LDADD = $(LIBJSON_LA)

TESTS = test1.test test2.test test4.test parse_int64.test test_null.test test_cast.test test_parse.test

TESTS+= test_printbuf.test
check_PROGRAMS+=test_printbuf
test_printbuf_LDADD = $(LIBJSON_LA)

EXTRA_DIST=
EXTRA_DIST += $(TESTS)

testsubdir=testSubDir
TESTS_ENVIRONMENT       = top_builddir=$(top_builddir)

distclean-local:
	-rm -rf $(testsubdir)
