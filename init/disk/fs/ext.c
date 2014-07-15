#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long
#define inodebuffer 0x140000
#define tablebuffer 0x180000
#define rawbuffer 0x1c0000


static QWORD block0;
static QWORD blocksize;
static QWORD inode1;
static QWORD inodesize;


void checkinodecache()
{
	//read inodes
        read(inodebuffer,block0+blocksize*inode1,getdisk(),0x200);
}


void explaininode(QWORD rdi,QWORD inode)
{
	checkinodecache();

	QWORD rsi=inodebuffer+(inode-1)*inodesize+0x28;
	int i;

	if(*(WORD*)rsi == 0xf30a)
	{
		QWORD numbers;

		say("    extend@",rsi);
		numbers=*(WORD*)(rsi+2);
		say("    {",0);
		say("    numbers:",numbers);

		rsi+=12;
		for(i=0;i<numbers;i++)
		{
			QWORD temp1;
			QWORD temp2;
			say("    @",rsi);

			temp1=*(DWORD*)(rsi+8);
			temp1+= *(WORD*)(rsi+6);
			temp1*=blocksize;
			temp1+=block0;
			say("    sector:",temp1);

			temp2=*(WORD*)(rsi+4);
			temp2*=blocksize;
			say("    count:",temp2);

			rsi+=12;

		        read(rdi,temp1,getdisk(),temp2);
			rdi+=0x200*blocksize;
		}

		say("    }",0);
	}
	else
	{
	say("    old@",rsi);
	say("    {",0);
	for(i=0;i<8;i++)
	{
		if(*(DWORD*)rsi != 0)
		{
			QWORD temp;
			say("    pointer@",rsi);

			temp=block0+(*(DWORD*)rsi)*blocksize;
			say("sector:",temp);

		        read(rdi,temp,getdisk(),blocksize);
			rdi+=0x200*blocksize;
		}

		rsi+=4;
	}
	say("    }",0);
	}
}


void table2raw(QWORD rsi)
{
	QWORD rdi=rawbuffer;
	int i;

	while(*(DWORD*)rsi != 0)
	{
		//先是名字
		if( *(BYTE*)(rsi+6) < 16)
		{
			i=0;
			while(*(BYTE*)(rsi+8+i) != 0)
			{
				*(BYTE*)(rdi+i)=*(BYTE*)(rsi+8+i);
				i++;
			}
		}
		else
		{
			*(QWORD*)rdi=*(QWORD*)(rsi+8);
			*(QWORD*)(rdi+8)=*(QWORD*)(rsi+16);
		}

		//再是inode号
		*(QWORD*)(rdi+0x10)=*(DWORD*)rsi;

		//剩下的是。。。
		rsi+=*(WORD*)(rsi+4);
		rdi+=0x20;
	}
}


void ext_cd(QWORD name)
{
	//传进来的名字处理一下
	blank2zero(&name);
	if(name == '/')
	{
		explaininode(tablebuffer,2);
		table2raw(tablebuffer);
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
	say("sector/block=",blocksize);
	inodesize=*(WORD*)0x130458;
	say("byte/inode:",inodesize);

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
