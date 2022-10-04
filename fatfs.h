#ifndef FATFS_H
#define FATFS_H

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>



#define MAXNAME 64
#define FATIDX 0
#define ROOTIDX 8
#define DATAIDX 10
#define BLOCKSIZE 512
#define FATSIZE  BLOCKSIZE *sizeof(uint16_t)*2
#define DATASIZE FATSIZE*BLOCKSIZE
#define DIRTABLESIZE 32


struct myFAT{
	uint16_t next;
}

struct myEntry{
	char* name;
	uint16_t modified_time;
	uint16_t create_time;
	uint16_t start_blocks;
	uint32_t size;
}

struct myFilehandle {
	char* name;
	uint32_t position;
}



void initFS(char* diskname);
void formatFS();
void initRoot();
void initEntry(int i);
myFileHandle* create_file();
void erase_file();
uint32_t my_write();
uint32_t my_read();
void my_seek;
void create_dir();
void erase_dir();
void change_dir();
void list_dir();


	


#endif
