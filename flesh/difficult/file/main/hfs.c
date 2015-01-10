#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long


//系统或者各种东西提供好的memory，这几个变量仅仅记录了位置
static QWORD directorybuffer;
static QWORD readbuffer;
static QWORD mftbuffer;
static QWORD indexbuffer;


void checkcacheforwhat()
{
	
}


void hfs_cd()
{

}


void hfs_load()
{

}

int mounthfs(QWORD sector,QWORD* cdfunc,QWORD* loadfunc)
{
	//返回cd和load函数的地址
	*cdfunc=(QWORD)hfs_cd;
	*loadfunc=(QWORD)hfs_load;

	return 0;
}