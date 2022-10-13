#include "fatfs.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern myEntry* rootDir;
extern char* data;
extern myFAT* fat;

int main(int argc, char** argv){
	char* diskname="primo_fat_fs";
	initFS(diskname);
	printf("INIT FS:OKAY\n");
	myFileHandle my_file=create_file("ciao");
	printf("CREATION OF FILE:OKAY\n");
	
	char *buf=(char*)malloc(1034);
	for(int i=0;i < 1034;i++){
			buf[i]='a';
	}
	
	size_t written_bytes=my_write(&my_file,buf,strlen(buf));
	printf("OKAY written bytes=%ld\n",written_bytes);
	printf("FILE SIZE = %d\n",rootDir[my_file.dir_idx].size);
	char *buf2=(char*)malloc(strlen(buf));
	my_read(&my_file,buf2,strlen(buf));
	printf("my_read\n%s\nfd=%d\n",buf,my_file.position);
	
	off_t os=my_seek(&my_file,(off_t)824,SEEK_SET);
	printf("OKAY1\nfd = %d\n%ld\n",my_file.position,os);
	os=my_seek(&my_file,-500,SEEK_CUR);
	printf("OKAY2\nfd = %d\n%ld\n",my_file.position,os);
	os=my_seek(&my_file,-1000,SEEK_END);
	printf("OKAY1\nfd = %d\n%ld\n",my_file.position,os);
	create_dir("my_first_dir");
	printf("DIR CREATED\n");
	return 0;
}

