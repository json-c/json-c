# $Id: Makefile,v 1.4 2004/07/22 01:37:44 mclark Exp $

CFLAGS		+= -g -Wall -std=gnu99 -D_GNU_SOURCE -D_REENTRANT
LDFLAGS		+=
LDLIBS		+=

LIB_OBJS 	= debug.o \
		  linkhash.o \
		  printbuf.o \
		  arraylist.o \
		  json_object.o \
		  json_tokener.o

LIB_HDRS	= debug.h \
		  linkhash.h \
		  printbuf.h \
		  arraylist.h \
		  json_object.h \
		  json_tokener.h

TESTS		= test1 test2

all: tests

tests: $(TESTS)
test1: test1.o $(LIB_OBJS)
test2: test2.o $(LIB_OBJS)

clean:
	rm -f *.o *~ $(TESTS)

cex.o: cex.c cex.h
debug.o: debug.c debug.h
linkhash.o: linkhash.c linkhash.h
arraylist.o: arraylist.c arraylist.h
json_object.o: json_object.c $(LIB_HDRS)
json_tokener.o: json_tokener.c $(LIB_HDRS)
test1.o: test1.c $(LIB_HDRS)
test2.o: test2.c $(LIB_HDRS)

