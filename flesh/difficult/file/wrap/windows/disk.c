#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define QWORD unsigned long long
#define DWORD unsigned int
#define WORD unsigned short
#define BYTE unsigned char
static BYTE diskname[20]={'\\','\\','.','\\','P','h','y','s','i','c','a','l','D','r','i','v','e','0','\0','\0'};
HANDLE hDev;


__attribute__((constructor)) void initdisk()
{
	//disk暂时根本不管是什么，默认就是当前第一个硬盘
	DWORD i=0;
	int status[10];
	for(i=0;i<10;i++)
	{
		diskname[17]=0x30+i;
		hDev=CreateFile(diskname,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,0,0);
		if(hDev != INVALID_HANDLE_VALUE)
		{
			printf("%d    \\\\.\\PHYSICALDRIVE%d\n",i,i);
			status[i]=1;
			CloseHandle(hDev);
		}
		else
		{
			status[i]=0;
			//printf("physicaldrive%d,GetLastError()=:%d\n",i,GetLastError());
		}
	}
	printf("choose disk(give me the number):");
	scanf("%d",&i);
	diskname[17]=0x30+i;
	hDev=CreateFile(diskname,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,0,0);
}
__attribute__((destructor)) void destorydisk()
{
	CloseHandle(hDev);
}




//mem地址，file名字，文件内偏移，总字节数
int mem2file(char* memaddr,char* filename,QWORD offset,QWORD count)
{
    HANDLE hFile;//文件句柄
    hFile=CreateFile(
        filename,//创建或打开的文件或设备的名称(这里是txt文件)。
        GENERIC_WRITE,// 文件访问权限,写
        0,//共享模式,这里设置0防止其他进程打开文件或设备
        NULL,//SECURITY_ATTRIBUTES结构，安全描述，这里NULL代表默认安全级别
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,//设置文件的属性，里面有高速缓存的选项
        NULL);

    //这里失败不会返回NULL，而是INVALID_HANDLE_VALUE
    if(hFile==INVALID_HANDLE_VALUE)
    {
        printf("hFile error\n");
        return -1;
    }

	LARGE_INTEGER li;
	li.QuadPart = offset;
	SetFilePointer (hFile,li.LowPart,&li.HighPart,FILE_BEGIN);

	unsigned long dwBytesWritten = 0;
	WriteFile(hFile,memaddr,count,&dwBytesWritten,NULL);

	CloseHandle(hFile);
}
//内存地址，第一扇区，请无视，总字节数
void read(QWORD buf,QWORD startsector,QWORD disk,DWORD count)
{
	LARGE_INTEGER li;
	li.QuadPart = startsector*512;
	SetFilePointer (hDev,li.LowPart,&li.HighPart,FILE_BEGIN);

	unsigned long dwret = 0;
	ReadFile(hDev,(unsigned char*)buf,count*512,&dwret,0);
	if(dwret!=count*512)printf("read %d bytes,GetLastError()=%d\n",dwret,GetLastError());
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
