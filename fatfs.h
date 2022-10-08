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


#define FAT_IDX 0
#define ROOT_IDX 8
#define DATA_IDX  11
#define DIR_ENTRY_COUNT 32
#define BYTES_PER_SECTOR 512
#define FATSIZE  8
#define DATASIZE (BLOCKSNUMBER-FATSIZE-DIRTABLESIZE)
#define BLOCKSNUMBER  256
#define DIRTABLESIZE  (DIR_ENTRY_COUNT * sizeof(myEntry)+BYTES_PER_SECTOR-1)/BYTES_PER_SECTOR
#define UNUSED 0x0000
#define EOC    0xFFFF
#define MAXNAME 11


typedef  uint16_t myFAT;

typedef uint8_t bool;

typedef struct {
	
	char name[MAXNAME];
	uint16_t modified_time;
	uint16_t create_time;
	uint16_t start_block;
	uint32_t size;
	bool isDir;
	uint32_t parent_idx;
	
}myEntry;

typedef struct {
	uint32_t position;
}myFileHandle;





void initFS(char* diskname);
void formatFS();
void initRoot();
void initEntry(int i);
int findFreeBlock();
int findFreeFAT();
int findFreeEntry();
myFileHandle create_file(char* filename);
int findFile(const char* filename);
void erase_file(char* filename);
uint32_t my_write();
uint32_t my_read();
void my_seek();
void create_dir();
void erase_dir();
void change_dir();
void list_dir();


	


#endif
