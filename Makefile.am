include Makefile.am.inc

EXTRA_DIST = README.html README-WIN32.html config.h.win32 doc json-c.vcproj
SUBDIRS = . tests

lib_LTLIBRARIES = libjson.la

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = json.pc

libjsonincludedir = $(includedir)/json
libjsoninclude_HEADERS = \
	arraylist.h \
	bits.h \
	debug.h \
	json.h \
	json_config.h \
	json_inttypes.h \
	json_object.h \
	json_object_private.h \
	json_tokener.h \
	json_util.h \
	linkhash.h \
	printbuf.h

#libjsonx_includedir = $(libdir)/json-c-@VERSION@
#
#libjsonx_include_HEADERS = \
#	json_config.h

libjson_la_LDFLAGS = -version-info 1:0:1 -no-undefined

libjson_la_SOURCES = \
	arraylist.c \
	debug.c \
	json_object.c \
	json_tokener.c \
	json_util.c \
	linkhash.c \
	printbuf.c


distclean-local:
	-rm -rf $(testsubdir)
	-rm -rf config.h.in~ Makefile.in aclocal.m4 autom4te.cache/ config.guess config.sub configure depcomp install-sh ltmain.sh missing

