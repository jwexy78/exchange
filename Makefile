CC=g++
CFLAGS=-std=c++1y -lcurses

SHAREDLIBSROOT=../sharedlibs

include $(SHAREDLIBSROOT)/catch/rules.mk

all: main.out test.out

main.out: FORCE
	$(CC) $(CFLAGS) $(IFLAGS) main.cpp -o $@

test.out: FORCE
	$(CC) $(CFLAGS) $(IFLAGS) test.cpp -o $@

FORCE: