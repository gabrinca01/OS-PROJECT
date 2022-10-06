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


#define FATIDX 0
#define ENTRIESIDX FATSIZE
#define DATAIDX  FATSIZE+DIRTABLESIZE
#define BLOCKSIZE 256
#define FATSIZE  BLOCKSNUMBER*sizeof(myFAT)
#define DATASIZE BLOCKSNUMBER*BLOCKSIZE
#define BLOCKSNUMBER  256
#define DIRTABLESIZE 32 * sizeof(myEntry)
#define UNUSED 0x0000
#define EOC    0xFFFF

typedef  uint16_t myFAT;



typedef struct myEntry{
	char* name;
	uint8_t numfiles;
	uint16_t modified_time;
	uint16_t create_time;
	uint16_t firstBlockIdx;
	uint32_t size;
}myEntry;

typedef struct myFileHandle {
	char* name;
	uint32_t position;
	myEntry* dir;
	uint32_t start;
	uint32_t size;
}myFileHandle;





void initFS(char* diskname);
void formatFS();
void initRoot();
void initEntry(int i);
myFileHandle* create_file();
void erase_file();
uint32_t my_write();
uint32_t my_read();
void my_seek();
void create_dir();
void erase_dir();
void change_dir();
void list_dir();


	


#endif
