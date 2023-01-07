# An admittedly primitive Makefile
# To compile, type "make" or make "all"
# To remove files, type "make clean"

CC = gcc
CFLAGS = -Wall
OBJS = wserver.o wclient.o request.o io_helper.o buffer.o 

.SUFFIXES: .c .o 

all: wserver wclient spin.cgi test.cgi

wserver: wserver.o request.o io_helper.o buffer.o common.h common_threads.h
	$(CC) $(CFLAGS) -o wserver wserver.o request.o io_helper.o buffer.o 

wclient: wclient.o io_helper.o
	$(CC) $(CFLAGS) -o wclient wclient.o io_helper.o

spin.cgi: spin.c
	$(CC) $(CFLAGS) -o spin.cgi spin.c

test.cgi: test.c 
	$(CC) $(CFLAGS) -o test.cgi test.c

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	-rm -f $(OBJS) wserver wclient spin.cgi test.cgi 
