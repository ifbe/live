#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define QWORD unsigned long long
#define DWORD unsigned int
#define WORD unsigned short
#define BYTE unsigned char
HANDLE hDev;


__attribute__((constructor)) void initdisk()
{
	//disk暂时根本不管是什么，默认就是当前第一个硬盘
	hDev=CreateFile("\\\\.\\PHYSICALDRIVE1",GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,0,0);
	if(hDev == INVALID_HANDLE_VALUE)
	{
		printf("error:%d,you can try administrator\n",GetLastError() );
		return;
	}
}
__attribute__((destructor)) void destorydisk()
{
	CloseHandle(hDev);
}





void read(QWORD buf,QWORD startsector,QWORD disk,DWORD count)
{
	LARGE_INTEGER li;
	li.QuadPart = startsector*512;
	SetFilePointer (hDev,li.LowPart,&li.HighPart,FILE_BEGIN);

	unsigned long dwRet = 0;
	ReadFile(hDev,(unsigned char*)buf,count*512,&dwRet,0);
	//printf("read %d bytes\n",dwRet);
/*
	int i,j;
	for(i=0;i<dwRet;i+=16)
	{
		QWORD skip=*(QWORD*)(buf+i);
		if(skip==0)continue;

		printf("[%4x,%4x]",i,i+15);

		for(j=0;j<16;j++)
		{
			BYTE temp=*(BYTE*)(buf+i+j);
			printf("%4x",temp);
		}
		printf("\n",i,i+15);
	}
*/
	//return dwRet;
}
