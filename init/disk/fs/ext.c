#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long
#define inodebuffer 0x140000
#define dirbuffer 0x180000
#define rawbuffer 0x1c0000


static QWORD block0;
static QWORD blocksize;
static QWORD inode1;


void ext_cd(QWORD name)
{
	//传进来的名字处理一下
	blank2zero(&name);
	if(name == '/')
	{
		//read inode
	        read(inodebuffer,block0+blocksize*inode1,getdisk(),0x200);

		DWORD* rsi=(DWORD*)(inodebuffer+0x80+0x28);
		QWORD rdi=dirbuffer;
		int i;
		for(i=0;i<8;i++)
		{
			//f30a
			if(rsi[i] != 0)
			{
				QWORD temp=block0+blocksize*rsi[i];
				say("@",&rsi[i]);
				say("",temp);
			        read(rdi,temp,getdisk(),blocksize);
				rdi+=0x200*blocksize;
			}
		}
	}
}


void ext_load(QWORD name)
{
	//处理名字
	blank2zero(&name);

}


int mountext(QWORD sector)
{
	block0=sector;
	say("ext sector:",sector);

	//读分区前8扇区，总共0x1000字节
        read(0x130000,sector,getdisk(),0x8);	//0x1000

	//检查magic值
	if( *(WORD*)0x130438 != 0xef53 ) return -1;
	say("yes!ext!",0);

	//变量们
	blocksize=*(DWORD*)0x130418;	//就不另开个temp变量了
	blocksize=( 1<<(blocksize+10) )/0x200;
	say("sectors/block=",blocksize);

	//读block descriptor
	if(blocksize==2)
	{
        read(0x138000,sector+4,getdisk(),0x8);		//在block2
	}
	else
	{
        read(0x138000,sector+blocksize,getdisk(),0x8);	//在block1
	}

	inode1=*(DWORD*)0x138008;
	say("inode1:",inode1);

	//cd /
	ext_cd('/');

	//保存函数地址
	say("",0);
	finishext();
	return 0;
}
