CC=gcc
CFLAGS =-std=c99
RM=rm

all:clean serv

serv:fatfs.c fatfs.h
	$(CC) $(CFLAGS) -o serv fatfs.c fatfs.h
		
