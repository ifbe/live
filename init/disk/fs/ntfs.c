#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long
#define mftbuffer 0x140000
#define indexbuffer 0x180000
#define rawbuffer 0x1c0000


static QWORD ntfssector;
static QWORD cacheblock;
static QWORD cachecurrent;
static QWORD clustersize;
static QWORD mftcluster;
static QWORD rawpointer;

static QWORD pwd[10];
static int ntfspwd;


void explainrun(QWORD runaddr,long long* lcn,long long* count)
{
	BYTE* run=(char*)runaddr;
	BYTE data= run[0];
	BYTE low4bit=0xf & data;
	BYTE high4bit=data >>4;
	int i;
	long long temp;

	temp=0;
	for(i=low4bit;i>0;i--)
	{
		temp=(temp<<8)+(QWORD)run[i];
	}
	//say("runcount:",temp);
	*count=temp;

	i=low4bit+high4bit;
	if(run[i] < 0x80) temp=(long long)run[i];
	else temp=(long long)(char)run[i];

	for(i=low4bit+high4bit-1;i>low4bit;i--)
	{
		temp=(temp<<8)+(long long)run[i];
	}
	//say("runlcn:",temp);
	*lcn=temp;
}


void mftpart(QWORD runaddr,QWORD mftnum)	//datarun地址，mft号
{
	say("    run@",runaddr);
	say("    {",0);

	//变量们
	QWORD rdi=mftbuffer;
	QWORD mftaddr = ntfssector;	//=cluster0
	BYTE data;
	long long lcn;
	long long count;

	QWORD wishstart=2 * ( mftnum&0xffffffffffffff00 );
	QWORD wishend=wishstart+0x200;
	QWORD runstart;
	QWORD runend;

	//第一个run
	explainrun(runaddr,&lcn,&count);

	QWORD temp=0;
	data=( *(BYTE*)runaddr ) >> 4;
	while(data)
	{
		temp=(temp<<8) + 0xff;
		data--;
	}
	lcn=lcn & temp;
	mftaddr += lcn*clustersize;
	say("    sector:",mftaddr);
	count *= clustersize;
	say("    size:",count);

	data= *(BYTE*)runaddr;
	runaddr= runaddr + 1 + (QWORD)(data & 0xf) + (QWORD)(data >> 4);
	runstart=0;
	runend=count;

	//判断是否是我们要的，是就读进内存
	//				[------wishstart,wishend-------]
	//	[------------------------------------------------------------>]
	//1:	[runstart,runend]
	//2:	[------------runstart,runend------------]
	//3:	[---------------------runstart,runend-----------------------]

	if(runend > wishstart)	//读[wishstart,?]
	{
		if(runend>=wishend)  //读[wishstart,wishend],结束
		{
			read(rdi,mftaddr+wishstart,getdisk(),256);
			say("    }",0);
			return;
		}
		else			//读[wishstart,runend],没完
		{
			read(rdi,mftaddr+wishstart,getdisk(),runend-wishstart);
			rdi+=count*0x200;
			wishstart=runend;
		}
	}
	//else 一个扇区都还没得到

	while( *(BYTE*)runaddr != 0 )
	{
		//剩下的run
		explainrun(runaddr,&lcn,&count);

		mftaddr += lcn*clustersize;
		say("    sector:",mftaddr);
		count *= clustersize;
		say("    size:",count);

		data= *(BYTE*)runaddr;
		runaddr= runaddr + 1 + (QWORD)(data & 0xf) + (QWORD)(data >> 4);
		runstart=runend;
		runend+=count;

	//判断是否是我们要的，是就读进内存
	//			[--------wishstart,wishend--------]
	//[---------------------------------------------------------------->]
	//1:[runstart,runend]
	//2:[----------runstart,runend------------]
	//3:[-------------------runstart,runend--------------------------]
	//4:			[-----runstart,runend------]
	//5:			[-----------runstart,runend-------------]
	//6(不可能):			[--------runstart,runend--------]

		if(runend > wishstart)	//读[wishstart,?]
		{
			if(runend>=wishend)  //读[wishstart,wishend],结束
			{
				read(rdi,mftaddr+wishstart,getdisk(),256);
				say("    }",0);
				return;
			}
			else			//读[wishstart,runend],没完
			{
				read(rdi,mftaddr+wishstart,getdisk(),runend-wishstart);
				rdi+=count*0x200;
				wishstart=runend;
			}
		}
		//else 一个扇区都不读

	}
}


void checkmftcache(QWORD mftnum)
{
	cacheblock=mftnum & 0xffffffffffffff00;
	if(cachecurrent == cacheblock) return;

	QWORD offset=0x138000 + ( *(WORD*)0x138014 );
	DWORD property;
	while(1)
	{
		if(offset > 0x138400) break;

		property= *(DWORD*)offset;
		if(property == 0xffffffff) break;

		//care only about 0x80,ignore all other properties
		if(property==0x80)
		{
			mftpart(offset+(*(QWORD*)(offset+0x20)),mftnum);
			cachecurrent = cacheblock;
			return;
		}

		offset += *(DWORD*)(offset+4);
	}
	say("mft at 0x138000 wrong",0);
}


void datarun(QWORD rdi,QWORD runaddr)
{
	say("    run@",runaddr);
	say("    {",0);

	//变量们
	BYTE data;
	QWORD dataaddr = ntfssector;
	long long lcn;
	long long count;

	//第一个run
	explainrun(runaddr,&lcn,&count);

	QWORD temp=0;
	data=( *(BYTE*)runaddr ) >> 4;
	while(data)
	{
		temp=(temp<<8) + 0xff;
		data--;
	}
	lcn=lcn & temp;
	dataaddr += lcn*clustersize;
	say("    sector:",dataaddr);
	count *= clustersize;
	say("    size:",count);

	data= *(BYTE*)runaddr;
	runaddr= runaddr + 1 + (QWORD)(data & 0xf) + (QWORD)(data >> 4);

	//读进内存
	read(rdi,dataaddr,getdisk(),count);
	rdi+=count*0x200;

	while( *(BYTE*)runaddr != 0 )		//不准确
	{
		//剩下的run
		explainrun(runaddr,&lcn,&count);

		dataaddr += lcn*clustersize;
		say("    sector:",dataaddr);
		count *= clustersize;
		say("    size:",count);

		data= *(BYTE*)runaddr;
		runaddr= runaddr + 1 + (QWORD)(data & 0xf) + (QWORD)(data >> 4);

		//读进内存
		read(rdi,dataaddr,getdisk(),count);
		rdi+=count*0x200;
	}

	say("    }",0);
}


void explain80(QWORD addr)	//file data
{
	say("80@",addr);

	if( *(BYTE*)(addr+8) == 0 )
	{
		say("resident80",0);
		DWORD length = *(DWORD*)(addr+0x10);
		BYTE* rsi=(BYTE*)(addr + (QWORD)(*(DWORD*)(addr+0x14)) );
		BYTE* rdi=(BYTE*)0x200000;
		int i;
		for(i=0;i<length;i++) rdi[i]=rsi[i];

		return;
	}
	else
	{
		say("non resident80",0);
		datarun(0x200000 , addr + (*(QWORD*)(addr+0x20)) );
	}
}


void index2raw(QWORD start,QWORD end)
{
	BYTE* buffer=(BYTE*)rawbuffer;
	QWORD index=start;
	int i;

	*(QWORD*)buffer= 0x2e;
	*(QWORD*)(buffer+0x10)= (*(QWORD*)(index+0x10))&0xffffffffffff;
	buffer+=0x20;

	while(index<end)
	{
		for(i=0;i<*(BYTE*)(index+0x50);i++)
		{
			if(i>=0x10) break;
			buffer[i]= *(BYTE*)(index+0x52+i*2);
		}
		*(QWORD*)(buffer+0x10)= (*(QWORD*)index)&0xffffffffffff;

		index+= *(WORD*)(index+0x8);
		buffer+=0x20;
	}
}


void explain90(QWORD addr)	//index root
{
	say("90@",addr);

	addr += *(DWORD*)(addr+0x14);		//addr=property body
	if( *(BYTE*)(addr+0x1c) !=0 )
	{
		return;
	}

	index2raw(addr+0x10,addr+0x10+ (QWORD)(*(DWORD*)(addr+4)) );
}


void explaina0(QWORD addr)	//index allocation
{
	say("a0@",addr);
	datarun(indexbuffer , addr + (*(QWORD*)(addr+0x20)) );

	if( *(DWORD*)indexbuffer !=0x58444e49 )
	{
		say("data run fail",0);
		return;
	}

	say("    INDX@",indexbuffer);
	QWORD start=indexbuffer + 0x18 + ( *(DWORD*)(indexbuffer+0x18) );
	QWORD end=indexbuffer + ( *(DWORD*)(indexbuffer+0x1c) );
	index2raw(start,end);
}


void explainmft(QWORD data)
{
	checkmftcache(data);		//reload cache
	QWORD mft=mftbuffer+0x400*(data % 0x100);    //0x40000/0x400=0x100个
	if( *(DWORD*)mft !=0x454c4946 )
	{
		say("[mft]wrong:",data);
		return;
	}


	QWORD offset=mft + ( *(WORD*)(mft+0x14) );
	DWORD property;
	while(1)
	{
		if(offset > mft + 0x400) break;

		property= *(DWORD*)offset;
		if(property == 0xffffffff) break;

		switch(property)
		{
			case 0x10:{	//standard information
				say("10@",offset);
				break;
			}
			case 0x20:{	//property list
				say("20@",offset);
				break;
			}
			case 0x30:{	//unicode name
				say("30@",offset);
				break;
			}
			case 0x40:{	//old volumn
				say("40@",offset);
				break;
			}
			case 0x50:{	//object id
				say("50@",offset);
				break;
			}
			case 0x60:{	//security??
				say("60@",offset);
				break;
			}
			case 0x70:{	//volumn name
				say("70@",offset);
				break;
			}
			case 0x80:{
				explain80(offset);
				break;
			}
			case 0x90:{
				explain90(offset);
				break;
			}
			case 0xa0:
			{
				explaina0(offset);
				break;
			}
			case 0xb0:{	//bitmap
				say("b0@",offset);
				break;
			}
			case 0xc0:{	//symbolic link
				say("c0@",offset);
				break;
			}
			case 0xd0:{	//ea information
				say("d0@",offset);
				break;
			}
			case 0xe0:{	//ea
				say("e0@",offset);
				break;
			}
			case 0xf0:{	//property set
				say("f0@",offset);
				break;
			}
			case 0x100:{	//logged utility stream
				say("100@",offset);
				break;
			}
			default:{
				say("unknown@",offset);
				break;
			}
		}
		offset += *(DWORD*)(offset+4);
	}
}


void ntfs_cd(QWORD name)
{
	//变量们
	QWORD mftnumber;
	QWORD* addr=(QWORD*)rawbuffer;
	int i;

	//传进来的名字处理一下
	blank2zero(&name);

	//几种特殊的名字
	switch(name)
	{
		case 0x2e:{	//	.
			return;
		}
		case 0x2e2e:{	//	..
			if(ntfspwd>0) ntfspwd--;
			mftnumber=pwd[ntfspwd];
			break;
		}
		case 0x2f:{	//	/
			mftnumber=5;
			break;
		}
		default:{	//	其他名字
			for(i=0;i<0x100;i+=4)
			{
				if(addr[i] ==name )
				{
					mftnumber=addr[i+2];
					say("directory:",mftnumber);
					if(ntfspwd<10) ntfspwd++;
					pwd[ntfspwd]=mftnumber;
					break;
				}
			}
			if(i==0x100)
			{
				say("directory not found",0);
				return;
			}
		}
	}

	//清理rawbuffer
	for(i=0;i<0x800;i++) addr[i]=0;		//clear [1c0000,1ffff8]
	rawpointer=rawbuffer;

	//开始change directory
	explainmft(mftnumber);
}


void ntfs_load(QWORD name)
{
	//变量们
	QWORD mftnumber;
	QWORD* addr=(QWORD*)rawbuffer;
	int i;

	//处理名字
	blank2zero(&name);

	//搜索
	for(i=0;i<0x100;i+=4)
	{
		if(addr[i] ==name )
		{
			mftnumber=addr[i+2];
			say("file:",mftnumber);
			break;
		}
	}
	if(i==0x100)
	{
		say("directory not found",0);
		return;
	}

	//开始load
	explainmft(mftnumber);
}


int mountntfs(QWORD sector)
{
	//读PBR，失败就返回
        read(0x130000,sector,getdisk(),1);
	if( *(DWORD*)0x130003 != 0x5346544e ) return -1;

	//记下PBR地址
	ntfssector=sector;
	say("ntfs sector:",sector);

	//变量
	clustersize=(QWORD)( *(BYTE*)0x13000d );
	say("clustersize:",clustersize);
	mftcluster= *(QWORD*)0x130030;
	say("mftcluster:",mftcluster);
	QWORD indexsize=clustersize * (QWORD)( *(BYTE*)0x130044 );
	say("indexsize:",indexsize);

	//保存开头几个mft
        read(0x138000,ntfssector+mftcluster*clustersize,getdisk(),0x20);
	cachecurrent=0xffff;		//no mft cache yet

	//cd /
	pwd[0]=5;
	ntfspwd=0;
	ntfs_cd('/');

	//保存函数地址
	finishntfs();
	return 0;
}
