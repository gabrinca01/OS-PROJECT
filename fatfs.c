#include "fatfs.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>

int fd,currentDir;
char* data;
myFAT *fat;
myEntry *rootDir;




void initFS(char* diskname){
	
	if((fd=open(diskname,O_CREAT | O_RDWR)) == -1){
		fprintf(stderr,"initFS:open failed -> %s\n",strerror(errno));
		exit(0);
	}
	if(ftruncate(fd,8192+(DATASIZE*BYTES_PER_SECTOR)) == -1){
		fprintf(stderr,"initFS:ftruncate  failed -> %s\n",strerror(errno));
		exit(0);
	}
	
	fat =(myFAT*)mmap(NULL,FATSIZE*BYTES_PER_SECTOR,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);
	
	assert(fat != MAP_FAILED);
	
	rootDir =(myEntry*) mmap(NULL,DIRTABLESIZE*BYTES_PER_SECTOR,PROT_READ | PROT_WRITE,MAP_SHARED,fd,4096);
	
	assert(rootDir != MAP_FAILED);
	
	data =(char*) mmap(NULL,DATASIZE*BYTES_PER_SECTOR,PROT_READ | PROT_WRITE,MAP_SHARED,fd,8192);
	
	assert(data != MAP_FAILED);
	for(int i =0;i < FATSIZE*BYTES_PER_SECTOR/sizeof(myFAT) ;i++){
			fat[i] = UNUSED;	
	}	
	
	for(int i=0;i < DIR_ENTRY_COUNT; i++){
			initEntry(i);
	}
	
	memset(data,0,DATASIZE * BYTES_PER_SECTOR);
	
	initRoot();
	
	if(close(fd)){
		fprintf(stderr,"initFS:close failed ->%s\n",strerror(errno));
		exit(0);
	}
	return;	
}

void initEntry(int i){
		myEntry dir;
		char* name="";
		memcpy(dir.name,name,11);
		dir.create_time= 0;
		dir.modified_time=0;
		dir.size=0;
		dir.start_block = EOC;
		rootDir[i]=dir;
		memcpy(&rootDir[i],&dir,sizeof(myEntry));
}

void initRoot(){
	myEntry rootEntry;
	char* name;
	myFAT fatEntry;
	currentDir =0;
	
	name= "root";
	memcpy(rootEntry.name,name,11);
	rootEntry.create_time = time(0);
	rootEntry.modified_time = time(0);
	rootEntry.start_block=0;
	rootEntry.size = 11;
	rootEntry.isDir = 1;
	
	fatEntry=EOC;
	
	memcpy(&fat[0],&fatEntry,sizeof(myFAT));
	memcpy(rootDir,&rootEntry,sizeof(myEntry));
	memcpy(data,&name,11);
	return;
}

int findfreeFAT(){
	
	for(int i=0;i < FATSIZE*BYTES_PER_SECTOR/sizeof(myFAT);i++){
			if(fat[i] == UNUSED)
				return i;
		
	}
	
	return -1;
}


int findFreeEntry(){
	for(int i=0; i < DIR_ENTRY_COUNT;i++){
		if(strcmp(rootDir[i].name, "") == 0 )
			return i;
	}
	return -1;
}



myFileHandle create_file(char *filename){
	myEntry dirEntry;
	myFAT fatEntry;
	int dirFree,blockFree;
	myFileHandle fd;
	fd.position=-1;
	
	if(findFile(filename) != -1){
		printf("create_file:File already exists\n");
		return fd;
	}
	if((blockFree=findfreeFAT()) == -1){
		printf("create_file:No more blocks available\n");
		return fd;	
	}
	if((dirFree=findFreeEntry()) == -1){
		printf("create_file:no dir entries available\n");
		return fd;
	}
	
	//create a new dir entry
	
	memcpy(dirEntry.name,&filename,11);
	dirEntry.parent_idx=currentDir;
	dirEntry.create_time = time(0);
	dirEntry.modified_time = time(0);
	dirEntry.start_block=blockFree;
	dirEntry.size = 11;
	dirEntry.isDir = 0;
	
	memcpy(&rootDir[dirFree],&dirEntry,sizeof(myEntry));
	
	//fat entry
	fatEntry = EOC;
	memcpy(&fat[blockFree],&fatEntry,sizeof(myFAT));
	
	
	memcpy(&data[blockFree*BYTES_PER_SECTOR],&filename,11);
	
	fd.position=blockFree*BYTES_PER_SECTOR;
	return fd;
}

int findFile(const char* filename){
	for(int i = 0; i < DIR_ENTRY_COUNT;i++){
		if(memcmp(&filename,rootDir[i].name,strlen(filename))== 0)
			return i;	
	}
	return -1;
}

void erase_file(char* filename){
	int index;
	if((index = findFile(filename)) == -1){
			printf("erase_file: file doesnt exists");
			return;
	}
	
	int start = rootDir[index].start_block;
	myFAT next =fat[start];
	while(0){
		memset(&data[start*BYTES_PER_SECTOR],0,BYTES_PER_SECTOR);
		if(next == EOC)
			break;
		fat[start]=UNUSED;
		start=next;
		next=fat[next];
	}
	
	initEntry(index);
	return;	
}
