
include ../Makefile.am.inc

lib_LTLIBRARIES = $(top_builddir)/libjson.la

check_PROGRAMS = test1 test2 test4 test_parse_int64 test_null test_cast test_parse

test1_LDADD = $(lib_LTLIBRARIES)

test2_LDADD = $(lib_LTLIBRARIES)

test4_LDADD = $(lib_LTLIBRARIES)

test_parse_int64_LDADD = $(lib_LTLIBRARIES)

test_null_LDADD = $(lib_LTLIBRARIES)

test_cast_LDADD = $(lib_LTLIBRARIES)

test_parse_LDADD = $(lib_LTLIBRARIES)

TESTS = test1.test test2.test test4.test parse_int64.test test_null.test test_cast.test test_parse.test
EXTRA_DIST=
EXTRA_DIST += $(TESTS)

testsubdir=testSubDir
TESTS_ENVIRONMENT       = top_builddir=$(top_builddir)

distclean-local:
	-rm -rf $(testsubdir)
