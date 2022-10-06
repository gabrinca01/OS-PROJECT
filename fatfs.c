#include "fatfs.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>

int fd,currentDir;
char* data;
myFAT *fat;
myEntry *dirtable;




void initFS(char* diskname){
	
	if((fd=open(diskname,O_RDWR)) == -1){
		fprintf(stderr,"initFS:open failed -> %s\n",strerror(errno));
		exit(0);
	}
	if(ftruncate(fd,8192+DATASIZE) == -1){
		fprintf(stderr,"initFS:ftruncate  failed -> %s\n",strerror(errno));
		exit(0);
	}
	
	fat =(myFAT*)mmap(NULL,FATSIZE,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);
	
	assert(fat != MAP_FAILED);
	
	dirtable =(myEntry*) mmap(NULL,DIRTABLESIZE,PROT_READ | PROT_WRITE,MAP_SHARED,fd,4096);
	
	assert(dirtable != MAP_FAILED);
	
	data =(char*) mmap(NULL,DATASIZE,PROT_READ | PROT_WRITE,MAP_SHARED,fd,8192);
	
	assert(data != MAP_FAILED);
	for(int i =0;i < FATSIZE/sizeof(myFAT) ;i++){
			fat[i] = UNUSED;	
	}	
	
	for(int i=0;i < DIRTABLESIZE/sizeof(myEntry); i++){
			initEntry(i);
	}
	
	initRoot();
	
	if(close(fd)){
		fprintf(stderr,"initFS:close failed ->%s\n",strerror(errno));
		exit(0);
	}
	return;	
}

void initEntry(int i){
		myEntry dir;
		dir.name = "";
		dir.create_time= 0;
		dir.modified_time=0;
		dir.size=0;
		dir.firstBlockIdx = EOC;
		dirtable[i]=dir;
		memcpy(&dirtable[i],&dir,sizeof(myEntry));
}

void initRoot(){
	myFAT fat_root;
	myEntry rootEntry;
	char name;
	
	currentDir =0;
	
	fat[0]=EOC;
	name= '.';
	
	rootEntry.name = "/";
	rootEntry.create_time = time(0);
	rootEntry.modified_time = time(0);
	rootEntry.firstBlockIdx=0;
	rootEntry.size = 0;
	
	memcpy(&fat[0],&fat_root,sizeof(myFAT));
	memcpy(dirtable,&rootEntry,sizeof(myEntry));
	memcpy(data,&name,1);
	
	
}
