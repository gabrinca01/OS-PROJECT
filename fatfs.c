#include "fatfs.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

int fd,currentDir;
struct myFAT *fat;
struct myEntry *dirtable;
char* data;
//map file drive
void initFS(char* diskname){
	if((fd=open(diskname,O_CREAT | O_RDWR)) == -1){
		fprintf(stderr,"initFS:open failed -> %s\n",strerror(errno));
		exit(0);
	}
	if((fat = mmap(0,FATSIZE * sizeof(struct myFAT),PROT_READ | PROT_WRITE,MAP_SHARED,fd,0)) == MAP_FAILED) {
		fprintf(stderr,"initFS:mmap fat failed -> %s\n",strerror(errno));
		exit(0);
	}
	if(dirtable = mmap(0,DIRTABLESIZE * sizeof(struct myEntry),PROT_WRITE | PROT_READ,MAP_SHARED,fd,BLOCKSIZE*ROOTIDX)) == MAP_FAILED) {
		fprintf(stderr,"initFS:mmap dir table failed -> %s\n",strerror(errno));
		exit(0);
	}
	if((data = mmap(0,DATASIZE,PROT_READ | PROT_WRITE,MAP_SHARED,fd,8192)) == MAP_FAILED){
	fprintf(stderr,"initFS:mmap data failed -> %s\n",strerror(errno));
	exit(0);	
	}
	
	for(int i =0;i < FATSIZE;i++){
			fat[i].next = 0x0000;
	}
	for(int i=0;i < DIRTABLESIZE; i++){
			initEntry(i);
	}
	for(int i=0; i < DATASIZE; i++){
			data[i]=0;
	}
	initRoot();
	return;	
}

void initEntry(int i){
		struct myEntry dir;
		dir.name = "";
		dir.create_time= 0;
		dir.modified_time=0;
		dir.size=0;
		dir.start_blocks = 0xFFFF;
		
		memcpy(&dirtable[index],&dir,sizeof(struct myEntry));
}

void initRoot(){
	struct myFAT fat_root;
	struct myEntry rootEntry;
	char name;
	
	currentDir =0;
	
	fat_root.next=0xFFFF;
	name= '.';
	
	rootEntry.name = "/";
	rootEntry.create_time = time(0);
	rootEntry.modified_time = time(0);
	rootEntry.start_blocks=DATAIDX;
	rootEntry.size = 0;
	
	memcpy(&fat[DATAIDX],&fat_root,sizeof(struct myFAT));
	memcpy(&dirtable,&rootEntry,sizeof(struct myEntry));
	memcpy(&data[0],&name,sizeof(char));
	
}
