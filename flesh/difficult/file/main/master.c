#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long


struct mystruct
{
	unsigned long long startlba;	//[+0,0x7]:起始lba
	unsigned long long endlba;	//[+0x8,0xf]:末尾lba
	unsigned long long parttype;	//[+0x10,0x17]:分区类型anscii
	unsigned long long partname;	//[0x18,0x1f]:分区名字
	unsigned long long a[4];	//[0x20,0x3f]:undefined
};
//一堆这种东西的结构体的起始地址
static struct mystruct* mytable;
//读缓冲区的地址，缓冲区64k大小
static QWORD readbuffer;




//从磁盘读出来的数据在[+0x400,+0x4400]=0x80个*每个0x80
//[+0,+0xf]:类型guid			raw[0],raw[1]
//[+0x10,+0x1f]:分区guid		raw[2],raw[3]
//[+0x20,+0x27]:起始lba			raw[4]
//[+0x28,+0x2f]:末尾lba			raw[5]
//[+0x30,+0x37]:属性标签		raw[6]
//[+0x38,+0x7f]:名字			raw[7]~raw[0xf]
QWORD explaingpt()
{
	say("gpt disk",0);
	QWORD* raw=(QWORD*)(readbuffer+0x400);
	int i;

	for(i=0;i<0x80;i++)	//0x80 partitions per disk
	{
		//
		mytable[i].startlba=raw[0x10*i+4];
		//
		mytable[i].endlba=raw[0x10*i+5];
		//
		if(raw[0x10*i]==0x4433b9e5ebd0a0a2)
		{
		if(raw[0x10*i+1]==0xc79926b7b668c087)
		{
			mytable[i].parttype=0x7366746e;		//ntfs
			say("fat@",mytable[i].startlba);
		}
		}
		else if(raw[0x10*i]==0x11d2f81fc12a7328)
		{
		if(raw[0x10*i+1]==0x3bc93ec9a0004bba)
		{
			mytable[i].parttype=0x746166;		//fat
			say("fat@",mytable[i].startlba);
		}
		}
		else if(raw[0x10*i]==0x477284830fc63daf)
		{
		if(raw[0x10*i+1]==0xe47d47d8693d798e)
		{
			mytable[i].parttype=0x747865;		//ext
			say("fat@",mytable[i].startlba);
		}
		}
	}
}




//从磁盘读出来的数据在[+0,+0xf]=4个*每个0x10
//[+0]:活动标记
//[+0x1,+0x3]:开始磁头柱面扇区
//[+0x4]:分区类型
//[+0x5,+0x7]:结束磁头柱面扇区
//[+0x8,+0xb]:起始lba
//[+0xc,+0xf]:大小
QWORD explainmbr()
{
	say("mbr disk",0);
	QWORD raw=readbuffer+0x1be;
	int i;

	for(i=0;i<4;i++)
	{
		//
		mytable[i].startlba=*(DWORD*)(raw+0x10*i+8);
		//
		mytable[i].endlba=*(DWORD*)(raw+0x10*i+0xc);
		//fat/ntfs/ext?
		BYTE temp=*(BYTE*)(raw+0x10*i+4);
		if( temp==0x4 | temp==0x6 | temp==0xb )
		{
			mytable[i].parttype=0x746166;
			say("fat@",mytable[i].startlba);
		}
		else if( temp==0x7 )
		{
			mytable[i].parttype=0x7366746e;
			say("fat@",mytable[i].startlba);
		}
		else if( temp==0x83 )
		{
			mytable[i].parttype=0x747865;
			say("fat@",mytable[i].startlba);
		}
	}
}


static int mount(BYTE* addr)
{
	QWORD type=*(DWORD*)addr;
	QWORD offset=0;
	int i;

	for(i=0;i<0x80;i++)
	{
		if( type == mytable[i].parttype )
		{
			offset=mytable[i].startlba;
			break;	
		}
	}
	if(i>=0x80)
	{
		say("partition not found",0);
		return -1;
	}

	if(type == 0x746166) mountfat(offset);
	if(type == 0x7366746e) mountntfs(offset);
	if(type == 0x747865) mountext(offset);

	return 1;
}


void main()
{
	//取出已经申请到的内存地址
	getaddroftable(&mytable);
	getaddrofbuffer(&readbuffer);

	//清理
	BYTE* memory;
	int i;
	memory=(BYTE*)(mytable);
	for(i=0;i<0x10000;i++) memory[i]=0;
	memory=(BYTE*)(readbuffer);
	for(i=0;i<0x100000;i++) memory[i]=0;

	//读出前64个扇区
        read(readbuffer,0,0,64);
        if(*(WORD*)(readbuffer+0x1fe)!=0xAA55)
	{
		say("bad disk",0);
		return;
	}

	//解释分区表
	if(*(QWORD*)(readbuffer+0x200)==0x5452415020494645)
	{
		explaingpt();
	}
	else
	{
		explainmbr();
	}

	//把操作函数的位置放进/bin
	//remember(0x746e756f6d,(QWORD)mount);
}
