#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long


//每0x40个字节是一个磁盘分区记录，里面包含起止扇区分区类型分区名字等信息
struct mystruct
{
	unsigned long long startlba;	//[+0,0x7]:起始lba
	unsigned long long endlba;	//[+0x8,0xf]:末尾lba
	unsigned long long parttype;	//[+0x10,0x17]:分区类型anscii
	unsigned long long partname;	//[0x18,0x1f]:分区名字
	unsigned long long a[4];	//[0x20,0x3f]:unused
};
static struct mystruct mytable[128];		//0x40*0x80=0x2000
//每0x40个字节是一个当前目录表
struct dirbuffer
{
	unsigned long long name;	//[+0,0x7]:起始lba
	unsigned long long unused;
	unsigned long long type;	//[+0x10,0x1f]:末尾lba
	unsigned long long unused1;
	unsigned long long specialid;	//[+0x20,0x2f]:分区类型anscii
	unsigned long long unused2;
	unsigned long long size;	//[0x30,0x3f]:分区名字
	unsigned long long unused3;
};
static struct dirbuffer* dir;	//dir=“目录名缓冲区”的内存地址（dir[0],dir[1],dir[2]是这个内存地址里面的第0，1，2字节快）
//读缓冲区的地址，缓冲区64k大小
static QWORD readbuffer;
//键盘输入专用
static unsigned char buffer[128];
//调用的cd和load函数所在的内存地址
static QWORD cdfunc,loadfunc;




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
	getaddrofbuffer(&readbuffer);
	getaddrofdir(&dir);

	//清理
	BYTE* memory;
	int i;
	memory=(BYTE*)(mytable);
	for(i=0;i<0x2000;i++) memory[i]=0;
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


static int mount(QWORD i)
{
	if(i>10)
	{
		say("impossible partition:%llx\n",i);
		return;
	}
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
	//分区表转换到容易理解的表里
	init();

	while(1)
	{
		//等输入
		QWORD first,second;
		listen(buffer);
		buf2arg(buffer,&first,&second);
		//say("%s,%s\n",(char*)&first,(char*)&second);
		//say("%llx,%llx\n",first,second);

		//判断
		if(first==0x74697865)break;		//exit
		else if(first==0x746e756f6d)			//mount ?
		{
			QWORD temp;
			anscii2dec(&second,&temp);
			//say("temp:%llx\n",temp);
			mount(temp);
		}
		else if(first==0x6463)				//cd
		{
			((int (*)())(cdfunc))(&second);
		}
		else if(first==0x64616f6c)				//load
		{
			//寻找这个文件名，主要为了得到size
			int i;
			for(i=0;i<0x40;i++)
			{
				
			}
			
			//现在才是分段读取保存
			((int (*)())(loadfunc))(&second,i);
		}
		else if(first==0x736c)				//ls
		{
			int i;
			say("name                special id          type                size\n");
			for(i=0;i<0x40;i++)
			{
				if(dir[i].name==0)break;
				say("%-16.16s    %-16llx    %-16llx    %-16llx\n",(char*)(&dir[i]),dir[i].type,dir[i].specialid,dir[i].size);
			}
			say("\n");
		}
	}
}