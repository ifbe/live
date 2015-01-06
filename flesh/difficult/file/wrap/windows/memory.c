﻿#include<stdlib.h>
unsigned char* tablebuf;		//分区概览
unsigned char* readbuf;		//读出来的数据存放的地址
unsigned char* dirbuf;		//存储翻译之后的(目录名,索引号)
unsigned char* fsbuf;		//文件系统读取代码需要的内部缓冲
__attribute__((constructor)) void initmemory()
{
	tablebuf=(unsigned char*)malloc(0x10000);
	readbuf=(unsigned char*)malloc(0x100000);
	dirbuf=(unsigned char*)malloc(0x100000);
	fsbuf=(unsigned char*)malloc(0x100000);
}
__attribute__((destructor)) void destorymemory()
{
	free(tablebuf);
	free(readbuf);
	free(dirbuf);
	free(fsbuf);
}




void getaddroftable(unsigned long long* p)
{
	*p=(unsigned long long)tablebuf;
}
void getaddrofbuffer(unsigned long long* p)
{
	*p=(unsigned long long)readbuf;
}
void getaddrofdir(unsigned long long* p)
{
	*p=(unsigned long long)fsbuf;
}
void getaddroffs(unsigned long long* p)
{
	*p=(unsigned long long)fsbuf;
}