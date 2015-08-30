#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long

#define idehome 0x100000
#define ahcihome 0x100000
#define xhcihome 0x200000
#define usbhome 0x300000

#define diskhome 0x400000
	#define partitionhome diskhome+0x10000
	#define mbrbuffer diskhome+0x20000
#define fshome 0x500000
#define dirhome 0x600000
#define datahome 0x700000




void diary(char*,QWORD);

void mountfat(QWORD);
void mountext(QWORD);
void mountntfs(QWORD);

int use(QWORD,char*);
void remember(QWORD,QWORD);

void string2data(BYTE* str,QWORD* data);

int read(QWORD,QWORD,QWORD,QWORD);
void identify(QWORD);

void hostcommand(DWORD,DWORD,DWORD,DWORD);
void usbcommand(BYTE bmrequesttype,BYTE brequest,WORD wvalue,WORD windex,WORD wlength,QWORD buffer);




static void directidentify()
{
	identify(datahome);
}
static void directread(QWORD sector)
{
	int result;
	diary("reading sector:%x\n",0);
	result=read(datahome,0,0,1);

	//result=read(datahome,sector,0,8);
	//if(result>=0) diary("read sector:%x",sector);
	//else diary("something wrong:%x",sector);
}
static void directhostcmd(char* arg0,char* arg1,char* arg2,char* arg3)
{
	QWORD dword0,dword1,dword2,dword3;
	string2data(arg0,&dword0);
	string2data(arg1,&dword1);
	string2data(arg2,&dword2);
	string2data(arg3,&dword3);
	hostcommand(dword0,dword1,dword2,dword3);
}
static void directusbcmd(char* arg0,char* arg1,char* arg2,char* arg3,char* arg4,char* arg5)
{
	QWORD bmrequesttype;
	QWORD brequest;
	QWORD wvalue;
	QWORD windex;
	QWORD wlength;
	QWORD buffer;

	//检查错误

	//发给选中的设备
	string2data(arg0,&bmrequesttype);
	string2data(arg1,&brequest);
	string2data(arg2,&wvalue);
	string2data(arg3,&windex);
	string2data(arg4,&wlength);
	string2data(arg5,&buffer);
	usbcommand(bmrequesttype, brequest, wvalue, windex, wlength, buffer);
}




//从磁盘读出来的数据在[+0x400,+0x4400]=0x80个*每个0x80
//[+0,+0xf]:类型guid			raw[0],raw[1]
//[+0x10,+0x1f]:分区guid		raw[2],raw[3]
//[+0x20,+0x27]:起始lba			raw[4]
//[+0x28,+0x2f]:末尾lba			raw[5]
//[+0x30,+0x37]:属性标签		raw[6]
//[+0x38,+0x7f]:名字			raw[7]~raw[0xf]
//翻译之后的在[diskhome,diskhome+0x800]
//[+0,0x7]:起始lba			our[0]
//[+0x8,0xf]:末尾lba			our[1]
//[+0x10,0x17]:分区类型anscii		our[2]
//[0x18,0x3f]:空
QWORD explaingpt()
{
	diary("gpt disk",0);
	QWORD* our=(QWORD*)(partitionhome);
	QWORD* raw=(QWORD*)(mbrbuffer+0x400);
	int i;
	for(i=0;i<0x200;i++) our[i]=0;

	for(i=0;i<0x80;i++)
	{
		our[8*i]=raw[0x10*i+4];
		our[8*i+1]=raw[0x10*i+5];
		if(raw[0x10*i]==0x4433b9e5ebd0a0a2)
		{
			if(raw[0x10*i+1]==0xc79926b7b668c087)
				our[8*i+2]=0x7366746e;
		}
		else if(raw[0x10*i]==0x11d2f81fc12a7328)
		{
			if(raw[0x10*i+1]==0x3bc93ec9a0004bba)
				our[8*i+2]=0x746166;
		}
		else if(raw[0x10*i]==0x477284830fc63daf)
		{
			if(raw[0x10*i+1]==0xe47d47d8693d798e)
				our[8*i+2]=0x747865;
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
//翻译之后的在[diskhome,diskhome+0x800]
//[+0,0x7]:起始lba			our[0]
//[+0x8,0xf]:末尾lba			our[1]
//[+0x10,0x17]:分区类型anscii		our[2]
//[0x18,0x3f]:空
QWORD explainmbr()
{
	diary("mbr disk",0);
	QWORD raw=mbrbuffer+0x1be;
	BYTE* our=(BYTE*)(partitionhome);
	int i;
	for(i=0;i<0x1000;i++) our[i]=0;

	for(i=0;i<4;i++)
	{
		*(QWORD*)(our+0x40*i)=*(DWORD*)(raw+0x10*i+8);
		*(QWORD*)(our+0x40*i+8)=*(DWORD*)(raw+0x10*i+0xc);


		BYTE temp=*(BYTE*)(raw+0x10*i+4);
		if( temp==0x4 | temp==0x6 | temp==0xb )
		{
			*(QWORD*)(our+0x40*i+0x10)=0x746166;
		}
		else if( temp==0x7 )
		{
			*(QWORD*)(our+0x40*i+0x10)=0x7366746e;
		}
		else if( temp==0x83 )
		{
			*(QWORD*)(our+0x40*i+0x10)=0x747865;
		}
	}
}

static int mount(BYTE* addr)
{
	QWORD name=*(DWORD*)addr;

	QWORD* memory=(QWORD*)(partitionhome);
	QWORD offset=0;
	int i;
	for(i=0;i<0x80;i+=8)
	{
		if( name == memory[i+2] )
		{
			offset=memory[i];
			break;	
		}
	}
	if(i>=0x80)
	{
		diary("partition not found",0);
		return -1;
	}

	if(name == 0x746166) mountfat(offset);
	if(name == 0x7366746e) mountntfs(offset);
	if(name == 0x747865) mountext(offset);

	return 1;
}








void master()
{
	QWORD from,to,temp;

	//一.首先找到已经能用的硬盘
	to=diskhome;
	for(;to<diskhome+0x100000;to+=4)	//清理内存
	{
		*(DWORD*)to=0;
	}

	from=ahcihome;
	to=diskhome;
	for(; from<(ahcihome+0x1000); from+=0x40)
	{
		temp=*(QWORD*)from;
		if(temp != 0)
		{
			//把所有已经发现的"硬盘设备"放在diskhome里面
			*(QWORD*)(to+0) = *(QWORD*)(from+0);
			*(QWORD*)(to+8) = *(QWORD*)(from+8);
			*(QWORD*)(to+0x10) = *(QWORD*)(from+0x10);
			to+=0x40;
		}
	}




	//二.把操作函数的位置放进/bin以便在终端里直接调
	remember(0x746e756f6d,		(QWORD)mount);
	remember(0x796669746e656469,		(QWORD)directidentify);
	remember(0x6b73696464616572,		(QWORD)directread);
	remember(0x646d6374736f68,		(QWORD)directhostcmd);
	remember(0x646d63627375,		(QWORD)directusbcmd);




	//三.然后读出前64个扇区,再解释分区表
	if(*(DWORD*)(diskhome) == 0x656469)return;

	read(mbrbuffer,0,0,64);
	if(*(WORD*)(mbrbuffer+0x1fe)!=0xAA55)
	{
		diary("bad disk",0);
		return;
	}

	if(*(QWORD*)(mbrbuffer+0x200)==0x5452415020494645)
	{
		explaingpt();
	}
	else
	{
		explainmbr();
	}




	//自动尝试(脑袋已经在内存里,身体暂时还在硬盘里......)
	//int result;

	//mount("fat");		//try fat
	//result=use(0x6463,"live");	//cd live
	//if(result>=0) return;		//成功，滚


	//mount("ntfs");		//try ntfs
	//result=use(0x6463,"live");	//cd live
	//if(result>=0) return;		//成功，滚


	//mount("ext");		//try ext
	//result=use(0x6463,"live");	//cd live
					//不管了，直接滚
}
