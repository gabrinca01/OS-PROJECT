#include "fatfs.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern myEntry* rootDir;
extern char* data;
extern myFAT* fat;

int main(int argc, char** argv){
	/*char *welcome_msg = "Welcome to the FAT file system :\).You can choose among the following options:\n"
		"createFile + filename (MAX LENGTH = 11)\n"
		"eraseFile + filename \n"
		"write + filename + string you wanna write\n"
		"read + filename to see whats written in the file\n"
		"seek + filename + offset + (SEEK_SET | SEEK_CUR | SEEK_END)\n"
		"createDir + dirname\n"
		"eraseDir + dirname\n"
		"cd + dirname\n"
		"ls + dirname\n"
		"quit\n";
		
	printf("%s",welcome_msg);
	while(0)
	{
		
		printf("$--");
		char* command = (char*) malloc(40);
		char* input;
		
		fgets(command,40,stdin);
		input = strtok(command," ");
		switch(input)
		{
			case "createFile":
				input=strtok(NULL," ");
				myFileHandle my_file=create_file(input);
			break;
			
			case "eraseFile":
				input = strtok(NULL," ");
				
				erase_file(input);
				
			break;
			
			case "write":
				input=strtok(NULL," ");
				myFileHandle my_file;
				int dir_idx=findFile(input);
				my_file.position=rootDir[dir_idx].start_block *BYTES_PER_SECTOR-1;
				my_file.dir_idx=dir_idx;
				
				input=strtok(NULL;" ");
				size_t written_bytes=my_write(&my_file,input,strlen(input));
				printf("OKAY written bytes=%ld\n",written_bytes);
				
			break;
			
			case "read":
				input=strtok(NULL," ");
				myFileHandle my_file;
				int dir_idx=findFile(input);
				my_file.position=rootDir[dir_idx].start_block *BYTES_PER_SECTOR-1;
				my_file.dir_idx=dir_idx;
				int size = rootDir[dir_idx].size;
				
				char *buf=(char*)malloc(size);
				my_read(&my_file,buf,size);
				printf("%s\n",buf);
				free(buf);
				
				
			
			break;
			
			case "seek":
				input=strtok(NULL," ");
				myFileHandle my_file;
				int dir_idx=findFile(input);
				my_file.position=rootDir[dir_idx].start_block *BYTES_PER_SECTOR-1;
				my_file.dir_idx=dir_idx;
				input = strtok(NULL," ");
				int offset=atoi(input);
				input = strtok(NULL," ");
				switch(input){
						case "SEEK_SET":
							off_t os=my_seek(&my_file,(off_t)offset,SEEK_SET);
							printf("OKAY1\nfd = %d\n%ld\n",my_file.position,os);
						break;
						case "SEEK_CUR":
							off_t os=my_seek(&my_file,(off_t)offset,SEEK_CUR);
							printf("OKAY1\nfd = %d\n%ld\n",my_file.position,os);
						break;
						case "SEEK_END":
							off_t os=my_seek(&my_file,(off_t)offset,SEEK_END);
							printf("OKAY1\nfd = %d\n%ld\n",my_file.position,os);
						
						break;
						
						default: break;
					
				}
			
			break;
			
			case "createDir":
				input = strtok(NULL," ");
				create_dir(input);
			
			break;
			
			case "cd":
				input = strtok(NULL," ");
				change_dir(input);
			
			break;
			
			case "ls":
				input=strtok(NULL," ");
				
				list_dir(input);
				
			break;
			
			case "eraseDir":
				input=strtok(NULL," ");
				
				erase_dir(input);
				
				break;
			
			case "quit":
					exit(EXIT_SUCCESS);
			
			
			default: 
			printf("You must have mistaken something");
			break;
		
		}
		free(command);
		
		
	}
	
	*/
	
	
	char* diskname="FAT_FS";
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

