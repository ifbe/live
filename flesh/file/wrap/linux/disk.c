#include<stdio.h>
#include<stdlib.h>
#define QWORD unsigned long long
#define DWORD unsigned int
static char diskname[10]={'/','d','e','v','/','s','d','a','\0','\0'};
static char status[10];
FILE* fd;

/*
static void enumeratedisk()
{
	printf(diskname);

	FILE* tempfd;
	char i=0;
	for(i=0;i<10;i++)
	{
		diskname[7]=i+'a';
		printf(diskname);
		fd = fopen(diskname, "r");
		if(fd != NULL)
		{
			fclose(tempfd);
			status[i]=1;
			printf("%d    %s\n",i,diskname);
		}
		else
		{
			status[i]=0;
		}
	}
}
*/
__attribute__((constructor)) void initdisk()
{
	fd = fopen(diskname, "r");
}
__attribute__((destructor)) void destorydisk()
{
	fclose(fd);
}




void disk(QWORD addr)
{
	//如果是空的，那就只扫描一遍有哪些物理磁盘然后打印一遍就返回不往下走
	if(*(DWORD*)addr == 0xffffffff)
	{
		//enumeratedisk();
		return;
	}

	//收到的地址里面究竟是些什么
	QWORD i=*(DWORD*)addr;

	//如果是小于10的数字
	if(i<10)
	{
	}
	else
	{
	}
}
void read(QWORD buf,QWORD sector,QWORD disk,DWORD count)
{
	//disk暂时根本不管是什么，默认就是当前第一个硬盘
	lseek(fd,0,sector*512);
	fread((void*)buf,1,count*512,fd);
}
int mem2file(char* memaddr,char* filename,QWORD offset,QWORD count)
{

}
