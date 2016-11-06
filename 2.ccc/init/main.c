#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long
//libboot
#define journalhome 0x100000
//libhard
#define idehome 0x400000
#define ahcihome 0x400000
#define xhcihome 0x600000
#define usbhome 0x700000
//libsoft
#define diskhome 0x800000
	#define partitionhome diskhome+0x10000
	#define mbrbuffer diskhome+0x20000
#define fshome 0x900000
#define dirhome 0xa00000
#define datahome 0xb00000
//libui




void mountfat(QWORD);
void mountext(QWORD);
void mountntfs(QWORD);

void explainmbr(QWORD source,QWORD dest);		//不废话,直接传地址
void explaingpt(QWORD source,QWORD dest);

int use(QWORD,char*);
void remember(QWORD,QWORD);

void string2data(BYTE* str,QWORD* data);

int read(QWORD,QWORD,QWORD,QWORD);
void identify(QWORD);

void hostcommand(DWORD,DWORD,DWORD,DWORD);
void usbcommand(BYTE bmrequesttype,BYTE brequest,WORD wvalue,WORD windex,WORD wlength,QWORD buffer);
void initxhci();
void initahci();
void initide();

void say(char*,...);
void diary(char*,...);




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
static void directidentify()
{
	char* p=(char*)datahome;
	QWORD temp;
	int i;
	identify(datahome);

	//SN，调转字节序
	for(i=0x14;i<0x60;i+=2)
	{
		temp=p[i];
		p[i]=p[i+1];
		p[i+1]=temp;
	}
	say((char*)(datahome+20));

	//size
	temp=*(QWORD*)(datahome+200);
	say("%dGB(%xsectors)",temp>>21,temp);
}
static void directread(QWORD sector)
{
	int result;
	say("reading sector:%x\n",0);
	result=read(datahome,0,0,1);

	//result=read(datahome,sector,0,8);
	//if(result>=0) say("read sector:%x",sector);
	//else say("something wrong:%x",sector);
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
static void directinitxhci()
{
	initxhci();
}
static void directinitahci()
{
	initahci();
}
static void directsay(char* first,char* second,char* third,char* fourth,char* fifth,char* sixth)
{
	say(first);
	say(second);
	say(third);
	say(fourth);
	say(fifth);
	say(sixth);
}








void something()
{
	//二.把操作函数的位置放进/bin以便在终端里直接调
	remember(0x746e756f6d,		(QWORD)mount);
	remember(0x796669746e656469,		(QWORD)directidentify);
	remember(0x6b73696464616572,		(QWORD)directread);
	remember(0x646d6374736f68,		(QWORD)directhostcmd);
	remember(0x646d63627375,		(QWORD)directusbcmd);
	remember(0x6963686174696e69,		(QWORD)directinitahci);
	remember(0x6963687874696e69,		(QWORD)directinitxhci);
	remember(0x796173,		(QWORD)directsay);
}
void initsoftware()
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
		explaingpt(mbrbuffer+0x400,partitionhome);
	}
	else
	{
		explainmbr(mbrbuffer+0x1be,partitionhome);
	}
}
/*
void autotry()
{
	//自动尝试(脑袋已经在内存里,身体暂时还在硬盘里......)
	int result;

	mount("fat");		//try fat
	result=use(0x6463,"live");	//cd live
	if(result>=0) return;		//成功，滚


	mount("ntfs");		//try ntfs
	result=use(0x6463,"live");	//cd live
	if(result>=0) return;		//成功，滚


	mount("ext");		//try ext
	result=use(0x6463,"live");	//cd live
					//不管了，直接滚
}
*/





void main()
{
	//2.初始化软件
	initsoftware();

	//3.初始化界面

	//4.
	//autotry()
}