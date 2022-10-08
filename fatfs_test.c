#include "fatfs.h"
#include <stdio.h>


int main(int argc, char** argv){
	char* diskname="primo_fat_fs";
	initFS(diskname);
	printf("OKAY\n");
	myFileHandle my_file=create_file("ciao");
	printf("OKAY\n");
	erase_file("ciao");
	printf("OKAY\n");
	
	return 0;
}

