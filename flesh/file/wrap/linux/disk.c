#include<stdio.h>
#define QWORD unsigned long long
#define DWORD unsigned int
FILE* fd;


__attribute__((constructor)) void initdisk()
{
        //fd = open("/dev/sda", O_RDWR);
        fd = fopen("/dev/sda", "r");
        if(fd == NULL)
        {
          printf("error,you can try sudo\n");
          return;
        }
}
__attribute__((destructor)) void destorydisk()
{
	fclose(fd);
}




void read(QWORD buf,QWORD sector,QWORD disk,DWORD count)
{
	//disk暂时根本不管是什么，默认就是当前第一个硬盘
	lseek(fd,0,sector*512);
	fread((void*)buf,1,count*512,fd);
}
