# Student's Makefile for the CS:APP Performance Lab
TEAM = DogDu
VERSION = 1
HANDINDIR =

CC = gcc
CFLAGS = -Wall -Og -m32 -g
LIBS = -lm

OBJS = kernels.o fcyc.o clock.o

all: driver mytest

driver: $(OBJS) fcyc.h clock.h defs.h config.h driver.c driver.o
	$(CC) $(CFLAGS) $(OBJS) driver.o $(LIBS) -o driver

mytest: mytest.c mytest.o
	$(CC) $(CFLAGS) mytest.o $(LIBS) -o mytest

handin:
	cp kernels.c $(HANDINDIR)/$(TEAM)-$(VERSION)-kernels.c

clean:
	-rm -f $(OBJS) driver core *~ *.o mytest


