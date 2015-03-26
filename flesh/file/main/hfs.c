#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long


//系统或者各种东西提供好的memory，这几个变量仅仅记录了位置
static QWORD readbuffer;
static QWORD directorybuffer;
static QWORD catalogbuffer;
void printmemory(QWORD addr,QWORD size);





static void hfs_explain()
{
	
}


static void hfs_cd()
{

}


static void hfs_load()
{

}

int mounthfs(QWORD sector,QWORD* explainfunc,QWORD* cdfunc,QWORD* loadfunc)
{
	//返回cd和load函数的地址
	*explainfunc=(QWORD)hfs_explain;
	*cdfunc=(QWORD)hfs_cd;
	*loadfunc=(QWORD)hfs_load;

	//准备好可用的内存地址
	getaddrofbuffer(&readbuffer);
	getaddrofdir(&directorybuffer);
	getaddroffs(&catalogbuffer);

	//读分区前8扇区，总共0x1000字节(其实只要分区内2号和3号扇区)
	read(readbuffer,sector,0,0x8);	//0x1000
	printmemory(readbuffer+0x400,0x200);

	//检查magic值
	if( *(WORD*)(readbuffer+0x400) == 0x2b48 )
	{
		say("hfs+\n");
	}
	else if( *(WORD*)(readbuffer+0x400) == 0x5848 )
	{
		say("hfsx\n");
	}

	return 0;
}
