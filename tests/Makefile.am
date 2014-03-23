
include ../Makefile.am.inc
LDADD= $(LIBJSON_LA)

LIBJSON_LA=$(top_builddir)/libjson-c.la

check_PROGRAMS = test1 test1Formatted 
check_PROGRAMS += test2 test2Formatted
check_PROGRAMS += test4
check_PROGRAMS += testReplaceExisting
check_PROGRAMS += test_parse_int64
check_PROGRAMS += test_null
check_PROGRAMS += test_cast
check_PROGRAMS += test_parse
check_PROGRAMS += test_locale
check_PROGRAMS += test_charcase

test1Formatted_SOURCES = test1.c parse_flags.c
test1Formatted_CPPFLAGS = -DTEST_FORMATTED

test2Formatted_SOURCES = test2.c parse_flags.c
test2Formatted_CPPFLAGS = -DTEST_FORMATTED

TESTS = test1.test test2.test test4.test testReplaceExisting.test parse_int64.test test_null.test test_cast.test test_parse.test test_locale.test test_charcase.test

TESTS+= test_printbuf.test
check_PROGRAMS+=test_printbuf

TESTS+= test_set_serializer.test
check_PROGRAMS += test_set_serializer

EXTRA_DIST=
EXTRA_DIST += $(TESTS)

testsubdir=testSubDir
TESTS_ENVIRONMENT       = top_builddir=$(top_builddir)

distclean-local:
	-rm -rf $(testsubdir)
