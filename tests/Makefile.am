
include ../Makefile.am.inc

#lib_LTLIBRARIES = ../libjson.la

#check_PROGRAMS = test1 test2 test4 test_parse_int64 test_null test_cast test_parse
check_PROGRAMS = test1

#test1_SOURCES = test1.c
#test1_LDADD = $(lib_LTLIBRARIES)
test1_LDADD = $(top_builddir)/libjson.la

#TESTS = test1.test test2.test test4.test parse_int64.test test_null.test test_cast.test test_parse.test
TESTS = test1.test
EXTRA_DIST=
EXTRA_DIST += $(TESTS)

testsubdir=testSubDir
TESTS_ENVIRONMENT       = top_builddir=$(top_builddir)

distclean-local:
	-rm -rf $(testsubdir)
