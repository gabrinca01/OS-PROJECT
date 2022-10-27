#include "fatfs.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>

//GLOBAL VARIABLES
int fd,currentDir;
char* data;
myFAT *fat;
myEntry *rootDir;




void initFS(char* diskname)
{
	
	//OPENING FILE TO  MMAP THE BUFFER
	if((fd=open(diskname,O_CREAT | O_RDWR)) == -1)
	{
		fprintf(stderr,"initFS:open failed -> %s\n",strerror(errno));
		exit(0);
	}
	
	
	//GIVING SOME SPACE TO FILE
	if(ftruncate(fd,8192+(DATASIZE*BYTES_PER_SECTOR)) == -1)
	{
		fprintf(stderr,"initFS:ftruncate  failed -> %s\n",strerror(errno));
		exit(0);
	}
	
	
	//MMPPING 3 DIFFERENT REGIONS OF DISK(FAT-TABLE,ROOT DIRECTORY,DATA )
	fat =(myFAT*)mmap(NULL,FATSIZE*BYTES_PER_SECTOR,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);
	
	assert(fat != MAP_FAILED);
	
	rootDir =(myEntry*) mmap(NULL,DIRTABLESIZE*BYTES_PER_SECTOR,PROT_READ | PROT_WRITE,MAP_SHARED,fd,4096);
	
	assert(rootDir != MAP_FAILED);
	
	data =(char*) mmap(NULL,DATASIZE*BYTES_PER_SECTOR,PROT_READ | PROT_WRITE,MAP_SHARED,fd,8192);
	
	assert(data != MAP_FAILED);
	
	
	//INIT FAT
	for(int i =0;i < FATSIZE*BYTES_PER_SECTOR/sizeof(myFAT) ;i++)
	{
			fat[i] = UNUSED;	
	}	
	//INIT ENTRIES
	for(int i=0;i < DIR_ENTRY_COUNT; i++)
	{
			initEntry(i);
	}
	//INIT DATA
	memset(data,0,DATASIZE * BYTES_PER_SECTOR);
	
	//INIT ROOT
	initRoot();
	
	if(close(fd))
	{
		fprintf(stderr,"initFS:close failed ->%s\n",strerror(errno));
		exit(0);
	}
	
	return;	
}

void initEntry(int i)
{
		myEntry dir;
		
		char* name="";
		
		memcpy(dir.name,name,1);
		
		dir.create_time= 0;
		
		dir.modified_time=0;
		
		dir.size=0;
		
		dir.start_block = EOC;
		
		rootDir[i]=dir;
		
		memcpy(&rootDir[i],&dir,sizeof(myEntry));
		
		return;
}

void initRoot()
{
	myEntry rootEntry;
	
	char* name;
	
	currentDir =0;
	
	name= "root";
	
	memcpy(rootEntry.name,name,MAXNAME);
	
	rootEntry.parent_idx=-1;
	
	rootEntry.create_time = time(0);
	
	rootEntry.modified_time = time(0);
	
	rootEntry.start_block=0;
	
	rootEntry.size = 0;
	
	rootEntry.isDir = 1;
	
	fat[0]=EOC;
	
	memcpy(rootDir,&rootEntry,sizeof(myEntry));
	
	return;
	
}

int findfreeFAT()
{
	
	for(int i=0;i < FATSIZE*BYTES_PER_SECTOR/sizeof(myFAT);i++)
	{
			if(fat[i] == UNUSED)
				
				return i;
		
	}
	
	return -1;
}


int findFreeEntry()
{
	for(int i=0; i < DIR_ENTRY_COUNT;i++)
	{
		if(strcmp(rootDir[i].name, "") == 0 )
			return i;
	}
	
	return -1;

}



myFileHandle create_file(char *filename)
{
	myEntry dirEntry;
	
	int dirFree,blockFree;
	
	myFileHandle fd;
	
	fd.position=-1;
	
	if(findFile(filename) != -1)
	{
		printf("create_file:File already exists\n");
	
		return fd;
	}
	
	if((blockFree=findfreeFAT()) == -1)
	{
		printf("create_file:No more blocks available\n");
		return fd;	
	}
	
	if((dirFree=findFreeEntry()) == -1)
	{
		printf("create_file:no dir entries available\n");
		return fd;
	}
	//create a new dir entry
	
	memcpy(dirEntry.name,&filename,MAXNAME);
	
	dirEntry.parent_idx = currentDir;
	
	dirEntry.create_time = time(0);
	
	dirEntry.modified_time = time(0);
	
	dirEntry.start_block = blockFree;
	
	dirEntry.size = 0;
	
	dirEntry.isDir = 0;
	
	memcpy(&rootDir[dirFree],&dirEntry,sizeof(myEntry));
	
	//fat entry
	fat[blockFree]=EOC;
	
	fd.position=blockFree*BYTES_PER_SECTOR-1;
	
	fd.dir_idx=dirFree;
	
	return fd;
	
}

int findFile(const char* filename)
{
	for(int i = 0; i < DIR_ENTRY_COUNT;i++)
	{
		if(memcmp(&filename,rootDir[i].name,strlen(filename))== 0)
			return i;	
	}
	
	return -1;
	
}

void erase_file(char* filename)
{
	
	int index;
	
	if((index = findFile(filename)) == -1)
	{
			printf("erase_file: file doesnt exists");
			return;
	}
	
	int start = rootDir[index].start_block;
	
	myFAT next =fat[start];
	
	while(0)
	{
		memset(&data[start*BYTES_PER_SECTOR],0,BYTES_PER_SECTOR);
		
		fat[start]=UNUSED;
		
		if(next == EOC)
			break;
		
		start=next;
		
		next=fat[next];
	
	}
	
	initEntry(index);
	
	return;	

}


size_t my_write(myFileHandle *fd,const char* buf,size_t count){
	
	int i=0,blockFree;
	
	myEntry dir=rootDir[fd->dir_idx];
	
	uint32_t pos=fd->position;
	
	myFAT next=fat[dir.start_block];
	
	while(i < count)
	{
		
			
			if((pos+1)%BYTES_PER_SECTOR < BYTES_PER_SECTOR-1)
			{ 
				data[pos]=buf[i];
				pos++;
				i++;
			}
			
			
			else
			{
				if(next == EOC)
				{
					if((blockFree=findfreeFAT()) == -1){
						printf("create_file:No more blocks available\n");
						rootDir[fd->dir_idx].size += i-1;
						return i-1;	
					}
				
				fat[(pos+1)/BYTES_PER_SECTOR]=blockFree;
					
				fat[blockFree]=EOC;
				
				pos=blockFree * BYTES_PER_SECTOR-1;
					
				}
				else
				{
					pos=next*BYTES_PER_SECTOR-1;
					
					next=fat[next];
					
				}
				
				
			}
							
	}
	
	
	rootDir[fd->dir_idx].size += count;
	
	rootDir[fd->dir_idx].modified_time=time(0);
	
	return count;

}

size_t my_read(myFileHandle *fd, char* buf,size_t count){
		
		int i=0;
		
		myEntry dir=rootDir[fd->dir_idx];
		
		uint32_t pos=fd->position;
		
		myFAT next=fat[dir.start_block];
		
		while( i < count)
		{
			if(pos%BYTES_PER_SECTOR <= BYTES_PER_SECTOR)
			{ 
				buf[i]=data[pos];
			
				pos++;
			
				i++;
			
			}
			
			
			else
			{
				if(next == EOC)
					return i-1;
					
				else
				{
					pos=next*BYTES_PER_SECTOR;
				
					next=fat[next];
					
				}
				
				
			}
			
		}
		
		
		return count;
		
}

off_t my_seek(myFileHandle *fd, off_t offset, int whence)
{

	myEntry dir=rootDir[fd->dir_idx];

	uint32_t pos=fd->position;

	myFAT next=fat[dir.start_block];
	
	off_t os=offset;

	switch(whence)
	{
		case SEEK_SET:
			
			if(offset < 0)
					return 0;
			else
			{
				pos=dir.start_block*BYTES_PER_SECTOR-1;
	
				while(1)
				{
				
					if(os  <= BYTES_PER_SECTOR)
					{
						
						fd->position =pos+os;
			
						return offset;
			
					}
				
					else if(next == EOC)
					{
						
						fd->position = pos+BYTES_PER_SECTOR;
			
						os -= BYTES_PER_SECTOR;
			
						return offset-os;
			
					}
				
					else
					{
						os -= BYTES_PER_SECTOR;
			
						pos=next*BYTES_PER_SECTOR-1;
			
						next=fat[next];
					 
					}
					
				}
			}
			break;
			
		case SEEK_CUR:
			
			if(offset <= 0)
			{
				int block_index= (pos+1)/BYTES_PER_SECTOR;
			
				int count=1;
			
				int num_blocks = block_index - (pos+offset)/BYTES_PER_SECTOR;
			
				if(num_blocks == 0)
				{
					fd->position -= offset;
				
					return offset;
				
				}
				
				while(next != block_index && next != EOC)
				{
				
					next=fat[next];
				
					count++;
				
				}
				
				if(num_blocks > count)
				{
					
					fd->position = dir.start_block*BYTES_PER_SECTOR;
					
					return pos%BYTES_PER_SECTOR +count*BYTES_PER_SECTOR;
				
				}
				
				else if(count-num_blocks==0)
				{
					fd->position =dir.start_block*BYTES_PER_SECTOR + BYTES_PER_SECTOR-(-offset-(pos%BYTES_PER_SECTOR)-(num_blocks-1)*BYTES_PER_SECTOR);
				
					return offset;
				
				}
				
				else
				{
					next=fat[dir.start_block];
				
					num_blocks++;
				
					while(count-num_blocks >0)
					{
						next=fat[next];
					
						count--;
					
					}
					
					fd->position = next*BYTES_PER_SECTOR + BYTES_PER_SECTOR - (offset - (pos%BYTES_PER_SECTOR)-(num_blocks-2)*BYTES_PER_SECTOR);
					
					return offset;
				
				}
					
			}
			else
			{
				while(1)
				{
				
					if(pos%BYTES_PER_SECTOR + os  <= BYTES_PER_SECTOR)
					{
						fd->position =pos+os;
				
						return offset;
				
					}
				
					else if(next == EOC)
					{
						fd->position = pos+BYTES_PER_SECTOR;
				
						os -= BYTES_PER_SECTOR -pos%BYTES_PER_SECTOR;
				
						return offset-os;
				
					}
				
					else
					{
						os -= BYTES_PER_SECTOR-pos%BYTES_PER_SECTOR;
				
						pos=next*BYTES_PER_SECTOR-1;
				
						next=fat[next];
					 
					}
					
				}
			
					
			}
			
			break;	
				
			
		
		case SEEK_END:
			if(offset > 0)
			
				return 0;
			while(next != EOC)
			{
				pos=next*BYTES_PER_SECTOR-1;
			
				next = fat[next];
			
			}
			pos += dir.size%BYTES_PER_SECTOR-1;
			
			next=fat[dir.start_block];
			
			int block_index= (pos+1)/BYTES_PER_SECTOR;
				
			int count=1;
	
			int num_blocks = block_index - (pos+offset)/BYTES_PER_SECTOR;
				
			if(num_blocks == 0)
			{
				fd->position -= offset;
				
				return offset;
			
			}
			while(next != block_index && next != EOC)
			{
				next=fat[next];
		
				count++;
			}
			if(num_blocks > count)
			{
					
				fd->position = dir.start_block*BYTES_PER_SECTOR;
					
				return pos%BYTES_PER_SECTOR +count*BYTES_PER_SECTOR;
			}
			else if(count-num_blocks==0)
			{
				fd->position =dir.start_block*BYTES_PER_SECTOR + BYTES_PER_SECTOR-(-offset-(pos%BYTES_PER_SECTOR)-(num_blocks-1)*BYTES_PER_SECTOR);
				
				return offset;
			
			}
			else
			{
				next=fat[dir.start_block];
				
				num_blocks++;
				while(count-num_blocks >0)
				{
					next=fat[next];
				
					count--;
				
				}
				fd->position = next*BYTES_PER_SECTOR + BYTES_PER_SECTOR - (offset - (pos%BYTES_PER_SECTOR)-(num_blocks-2)*BYTES_PER_SECTOR);
				
				return offset;
			}
				
		
			default:
				return -1;
		
		
	}
	
	return -1;
	
}


void create_dir(char* dirname){
	
	myEntry dirEntry;
	
	int dirFree,blockFree;
	
	
	if(findFile(dirname) != -1)
	{
		printf("create_file:Directory already exists\n");
	
		return;
	
	}
	
	if((blockFree=findfreeFAT()) == -1)
	{
		printf("create_file:No more blocks available\n");
	
		return ;	
	
	}
	
	if((dirFree=findFreeEntry()) == -1)
	{
		printf("create_file:no dir entries available\n");
	
		return ;
	
	}
	
	//create a new dir entry
	
	memcpy(dirEntry.name,&dirname,MAXNAME);
	
	dirEntry.parent_idx = currentDir;
	
	dirEntry.create_time = time(0);
	
	dirEntry.modified_time = time(0);
	
	dirEntry.start_block=blockFree;
	
	dirEntry.size = 0;
	
	dirEntry.isDir = 1;
	
	memcpy(&rootDir[dirFree],&dirEntry,sizeof(myEntry));
	
	//fat entry
	fat[blockFree]=EOC;
	
	return;
	
}

void erase_dir(char* dirname)
{
	int index;
	
	if((index = findFile(dirname)) == -1)
	{
		printf("erase_file: Directory doesnt exists");
		
		return;
	}
	
	int start = rootDir[index].start_block;
	
	fat[start]=UNUSED;
	
	initEntry(index);
	
	return;		
}


void change_dir(char* dirname)
{
	int index;
	
	if((index = findFile(dirname)) == -1)
	{
		printf("erase_file: Directory doesnt exists");
	
		return;
	}
	if(rootDir[index].parent_idx == currentDir)
		
		currentDir=rootDir[index].start_block;
	
	else
	{
		printf("Can't find directory");
		
		return;
	}
	
	return;	
}



void list_dir(char* dirname)
{
	int index;
	if((index=findFile(dirname)) == -1)
	{
		printf("list_dir: Directory doesnt exists");
		
		return;
	
	}
	for(int i=0;i < DIR_ENTRY_COUNT;++i)
	{
		if(rootDir[i].parent_idx == index)
		{
			if(rootDir[i].isDir)
				
				printf("DIR--->%s\n",rootDir[i].name);
			
			else
				
				printf("FILE--->%s\n",rootDir[i].name);
			
		}
		
	}
	
	printf("\n");
	
	return;
}
