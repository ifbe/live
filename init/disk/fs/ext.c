#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long
#define mftbuffer 0x140000
#define indexbuffer 0x180000
#define rawbuffer 0x1c0000


void ext_cd(QWORD name)
{
	//传进来的名字处理一下
	blank2zero(&name);

}


void ext_load(QWORD name)
{
	//处理名字
	blank2zero(&name);

}


int mountext(QWORD sector)
{
	//读PBR，失败就返回
        read(0x130000,sector,getdisk(),1);
	if( *(DWORD*)0x130003 != 0x5346544e ) return -1;

	//变量
	say("",0);

	//保存函数地址
	finishext();
	return 0;
}
