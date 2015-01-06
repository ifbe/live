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
static QWORD cdfunc,loadfunc;
static unsigned char buffer[128];



//从磁盘读出来的数据在[+0x400,+0x4400]=0x80个*每个0x80
//[+0,+0xf]:类型guid			raw[0],raw[1]
//[+0x10,+0x1f]:分区guid		raw[2],raw[3]
//[+0x20,+0x27]:起始lba			raw[4]
//[+0x28,+0x2f]:末尾lba			raw[5]
//[+0x30,+0x37]:属性标签		raw[6]
//[+0x38,+0x7f]:名字			raw[7]~raw[0xf]
QWORD explaingpt()
{
	say("gpt disk\n");
	QWORD* raw=(QWORD*)(readbuffer+0x400);

	int src=0,dst=0;
	for(src=0;src<0x80;src++)	//0x80 partitions per disk
	{
		QWORD firsthalf=raw[0x10*src];
		QWORD secondhalf=raw[0x10*src+1];
		if(firsthalf==0)continue;

		say("%2d    ",dst);
		//startsector
		mytable[dst].startlba=raw[0x10*src+4];

		//endsector
		mytable[dst].endlba=raw[0x10*src+5];

		//parttype
		say("[%8x,%8x]    ",mytable[dst].startlba,mytable[dst].endlba);
		if((firsthalf==0x4433b9e5ebd0a0a2)&&(secondhalf==0xc79926b7b668c087))
		{
			mytable[dst].parttype=0x7366746e;		//ntfs
			say("ntfs\n");
		}
		else if((firsthalf==0x11d2f81fc12a7328)&&(secondhalf==0x3bc93ec9a0004bba))
		{
			mytable[dst].parttype=0x746166;		//fat
			say("fat\n");
		}
		else if((firsthalf==0x477284830fc63daf)&&(secondhalf==0xe47d47d8693d798e))
		{
			mytable[dst].parttype=0x747865;		//ext
			say("ext\n");
		}
		else say("unknown\n");

		//partname

		//pointer++
		dst++;
	}
}


//从磁盘读出来的数据在[+0x1be,+0x1fd]每个0x10总共4个
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

	int src=0,dst=0;
	for(src=0;src<4;src++)
	{
		BYTE temp=*(BYTE*)(raw+0x10*src+4);
		if(temp==0)continue;

		say("%2d    ",dst);

		//startsector
		mytable[dst].startlba=*(DWORD*)(raw+0x10*src+8);

		//endsector
		mytable[dst].endlba=*(DWORD*)(raw+0x10*src+0xc);

		//parttype
		say("[%8x,%8x]    ",mytable[dst].startlba);
		if( temp==0x4 | temp==0x6 | temp==0xb )
		{
			mytable[dst].parttype=0x746166;
			say("fat\n");
		}
		else if( temp==0x7 )
		{
			mytable[dst].parttype=0x7366746e;
			say("ntfs\n",mytable[dst].startlba);
		}
		else if( temp==0x83 )
		{
			mytable[dst].parttype=0x747865;
			say("ext\n",mytable[dst].startlba);
		}
		else say("unknown\n");
		
		//partname

		//dst
		dst++;
	}
}


void init()
{
	//取出已经申请到的内存地址，看不惯就手动malloc吧
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
		say("bad disk\n");
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
}


static int mount(int i)
{
	QWORD start=mytable[i].startlba;
	QWORD end=mytable[i].endlba;
	QWORD type=mytable[i].parttype;
	say("startlba:%x,endlba:%x,type:%s\n",start,end,(char*)&type);

	//看分区种类调用函数，顺便把分区offset给它
	if(type == 0x746166)
	{
		mountfat(start,&cdfunc,&loadfunc);
	}
	else if(type == 0x7366746e)
	{
		mountntfs(start,&cdfunc,&loadfunc);
	}
	else if(type == 0x747865)
	{
		mountext(start,&cdfunc,&loadfunc);
	}
	return 1;
}



void main()
{
	init();

	//把操作函数的位置放进/bin
	//remember(0x746e756f6d,(QWORD)mount);

	QWORD first,second;
	int i;
	QWORD temp;
	QWORD** p;
	getaddrofdir(&p);
	say("type exit to exit,type mount ? to mount,type ls to list,type load ? to load\n");
	while(1)
	{
		//取一段输入
		listen(buffer);
		buf2arg(buffer,&first,&second);
		//say("%s,%s\n",(char*)&first,(char*)&second);
		say("%llx,%llx\n",first,second);

		//判断
		if(first==0x74697865)break;		//exit
		else if(first==0x746e756f6d)			//mount ?
		{
			anscii2dec(&second,&temp);
			//say("temp:%d\n",temp);
			mount(temp);
		}
		else if(first==0x6463)				//cd
		{
			((int (*)())(cdfunc))(&second);
		}
		else if(first==64616f6c)				//load
		{
			
		}
		else if(first==0x736c)				//ls
		{
			for(i=0;i<0x200;i+=0x10)
			{
				if(p[i]==0)break;
				say("%-16.16s    ",(char*)(&p[i]));
				//say("%s,%llx    ",(char*)(&p[i]),p[i+2]);
				if(p[i+4]==0)break;
				say("%-16.16s    ",(char*)(&p[i+4]));
				//say("%s,%llx    ",(char*)(&p[i+4]),p[i+6]);
				if(p[i+8]==0)break;
				say("%-16.16s    ",(char*)(&p[i+8]));
				//say("%s,%llx    ",(char*)(&p[i+8]),p[i+0x10]);
				if(p[i+0xc]==0)break;
				say("%-16.16s\n",(char*)(&p[i+12]));
				//say("%s,%llx\n",(char*)(&p[i+0xc]),p[i+0xe]);
			}
			say("\n");
		}
	}
}