CC=gcc
CCOPTS=--std=gnu99 -g -Wall 
AR=ar


BINS=fatfs_test

OBJS=fatfs.c

HEADERS=fatfs.h

LIBS=

%.o:	%.c $(HEADERS)
	$(CC) $(CCOPTS) -c -o $@  $<

.phony: clean all


all:	$(BINS) $(LIBS)

fatfs_test: fatfs_test.c $(OBJS)
	$(CC) $(CCOPTS) -o $@ $^

clean:
	rm -rf *.o *~ $(LIBS) $(BINS)

		
