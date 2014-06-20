#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long


static QWORD sector0;
static QWORD cacheblock;
static QWORD cachecurrent;
static QWORD clustersize;
static QWORD mftcluster;
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
	say("runcount:",temp);
	*count=temp;

	i=low4bit+high4bit;
	if(run[i] < 0x80) temp=(long long)run[i];
	else temp=(long long)(char)run[i];

	for(i=low4bit+high4bit-1;i>low4bit;i--)
	{
		temp=(temp<<8)+(long long)run[i];
	}
	say("runlcn:",temp);
	*lcn=temp;
}


void mftpart(QWORD runaddr,QWORD mftnum)	//datarun地址，mft号
{
	//变量们
	QWORD rdi=0x140000;
	QWORD mftaddr = sector0;	//=cluster0
	BYTE data;
	long long lcn;
	long long count;

	QWORD wishstart=2 * ( mftnum&0xffffffffffffff00 );
	QWORD wishend=wishstart+0x200;
	QWORD runstart;
	QWORD runend;

	//第一个run
	say("[run]addr0:",runaddr);
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
	say("[mft]addr0:",mftaddr);
	count *= clustersize;
	say("[mft]count0:",count);

	data= *(BYTE*)runaddr;
	runaddr= runaddr + 1 + (QWORD)(data & 0xf) + (QWORD)(data >> 4);
	runstart=0;
	runend=count;

//判断是否是我们要的，是就读进内存
//				[--------wishstart,wishend---------]
//	[--------------------------------------------------------------------->]
//1:	[runstart,runend]
//2:	[------------runstart,runend------------]
//3:	[---------------------runstart,runend----------------------------]

	if(runend > wishstart)	//读[wishstart,?]
	{
		if(runend>=wishend)  //读[wishstart,wishend],结束
		{
			read(rdi,mftaddr+wishstart,getdisk(),256);
			say("[cached]:",mftaddr+wishstart);
			return;
		}
		else			//读[wishstart,runend],没完
		{
			read(rdi,mftaddr+wishstart,getdisk(),runend-wishstart);
			say("[cacheing]:",mftaddr+wishstart);
			rdi+=count*0x200;
			wishstart=runend;
		}
	}
	//else 一个扇区都还没得到

	while( *(BYTE*)runaddr != 0 )
	{
		//剩下的run
		say("[run]addr?:",runaddr);
		explainrun(runaddr,&lcn,&count);

		mftaddr += lcn*clustersize;
		say("[mft]addr?:",mftaddr);
		count *= clustersize;
		say("[mft]count?:",count);

		data= *(BYTE*)runaddr;
		runaddr= runaddr + 1 + (QWORD)(data & 0xf) + (QWORD)(data >> 4);
		runstart=runend;
		runend+=count;

//判断是否是我们要的，是就读进内存
//				[--------wishstart,wishend--------]
//	[--------------------------------------------------------------------->]
//1:	[runstart,runend]
//2:	[----------------runstart,runend----------------]
//3:	[------------------------runstart,runend------------------------]
//4:				[runstart,runend]
//5:				[----------------runstart,runend---------------]
//6(不可能):				[--------runstart,runend--------]

		if(runend > wishstart)	//读[wishstart,?]
		{
			if(runend>=wishend)  //读[wishstart,wishend],结束
			{
				read(rdi,mftaddr+wishstart,getdisk(),256);
				say("[cached]:",mftaddr+wishstart);
				return;
			}
			else			//读[wishstart,runend],没完
			{
				read(rdi,mftaddr+wishstart,getdisk(),runend-wishstart);
				say("[cacheing]:",mftaddr+wishstart);
				rdi+=count*0x200;
				wishstart=runend;
			}
		}
		//else 一个扇区都不读

	}
}


void datarun(QWORD rdi,QWORD runaddr)
{
	//变量们
	BYTE data;
	QWORD dataaddr = sector0;
	long long lcn;
	long long count;

	//第一个run
	say("[run]addr0:",runaddr);
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
	say("[data]addr0:",dataaddr);
	count *= clustersize;
	say("[data]count0:",count);

	data= *(BYTE*)runaddr;
	runaddr= runaddr + 1 + (QWORD)(data & 0xf) + (QWORD)(data >> 4);

	//读进内存
	read(rdi,dataaddr,getdisk(),count);
	rdi+=count*0x200;

	while( *(BYTE*)runaddr != 0 )		//不准确
	{
		//剩下的run
		say("[run]addr0:",runaddr);
		explainrun(runaddr,&lcn,&count);

		dataaddr += lcn*clustersize;
		say("[data]addr?:",dataaddr);
		count *= clustersize;
		say("[data]count?:",count);

		data= *(BYTE*)runaddr;
		runaddr= runaddr + 1 + (QWORD)(data & 0xf) + (QWORD)(data >> 4);

		//读进内存
		read(rdi,dataaddr,getdisk(),count);
		rdi+=count*0x200;
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


void explain80(QWORD addr)			//addr=property 90
{
	say("[80]file data:",addr);

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


void explainindex(QWORD start,QWORD end)
{
	QWORD index=start;
	say("index:",index);

	BYTE* rdi=(BYTE*)0x1c0000;
	int i;
	for(i=0;i<0x800;i++) rdi[i]=0;		//clear [1c0000,1ffff8]

	*(QWORD*)rdi= 0x2e;
	*(QWORD*)(rdi+0x10)= (*(QWORD*)(index+0x10))&0xffffffffffff;
	rdi+=0x20;

	while(index<end)
	{
		for(i=0;i<*(BYTE*)(index+0x50);i++)
		{
			if(i>=0x10) break;
			rdi[i]= *(BYTE*)(index+0x52+i*2);
		}
		*(QWORD*)(rdi+0x10)= (*(QWORD*)index)&0xffffffffffff;

		index+= *(WORD*)(index+0x8);
		rdi+=0x20;
	}
}


void explain90(QWORD addr)			//addr=property 90
{
	say("[90]index root:",addr);

	addr += *(DWORD*)(addr+0x14);		//addr=property body
	if( *(BYTE*)(addr+0x1c) !=0 )
	{
		say("[90]ignored",0);
		return;
	}

	say("[90]explaining",0);
	explainindex(addr+0x10,addr+0x10+ (QWORD)(*(DWORD*)(addr+4)) );
}


void explaina0(QWORD addr)
{
	say("[a0]index allocation:",addr);
	datarun(0x180000 , addr + (*(QWORD*)(addr+0x20)) );

	if( *(DWORD*)0x180000 !=0x58444e49 )
	{
		say("data run fail",0);
		return;
	}

	QWORD start=0x180018+(*(DWORD*)0x180018);
	QWORD end=0x180000 + (*(DWORD*)0x18001c);

	explainindex(start,end);
}


void explainmft(QWORD data)
{
	checkmftcache(data);		//reload cache
	QWORD mft=0x140000+0x400*(data % 0x100);    //0x40000/0x400=0x100个
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
			case 0x10:{
				say("[10]standard infomation:",offset);
				break;
			}
			case 0x20:{
				say("[20]property list:",offset);
				break;
			}
			case 0x30:{
				say("[30]unicode name:",offset);
				break;
			}
			case 0x40:{
				say("[40]old volum:",offset);
				break;
			}
			case 0x50:{
				say("[50]object id:",offset);
				break;
			}
			case 0x60:{
				say("[60]security??:",offset);
				break;
			}
			case 0x70:{
				say("[70]volumn name:",offset);
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
			case 0xb0:{
				say("[b0]bitmap:",offset);
				break;
			}
			case 0xc0:{
				say("[c0]symbolic link:",offset);
				break;
			}
			case 0xd0:{
				say("[d0]ea information:",offset);
				break;
			}
			case 0xe0:{
				say("[e0]ea:",offset);
				break;
			}
			case 0xf0:{
				say("[f0]property set:",offset);
				break;
			}
			case 0x100:{
				say("[100]logged utility stream:",offset);
				break;
			}
			default:{
				say("unknown lastone:",offset);
				break;
			}
		}
		offset += *(DWORD*)(offset+4);
	}
}


void ntfs_cd(QWORD name)
{
	blank2zero(&name);

	if(name==0x2e) return;
	if(name==0x2e2e)
	{
		if(ntfspwd>0) ntfspwd--;
		explainmft(pwd[ntfspwd]);
		return;
	}

	QWORD* addr=(QWORD*)0x1c0000;
	int i;
	for(i=0;i<0x100;i+=4)
	{
		if(addr[i] ==name )
		{
			say("directory:",addr[i+2]);
			explainmft(addr[i+2]);
			if(ntfspwd<10) ntfspwd++;
			pwd[ntfspwd]=addr[i+2];

			return;
		}
	}
	if(i==0x100)
	{
		say("directory not found",0);
		return;
	}
}


void ntfs_load(QWORD name)
{
	blank2zero(&name);

	QWORD* addr=(QWORD*)0x1c0000;
	int i;
	for(i=0;i<0x100;i+=4)
	{
		if(addr[i] ==name )
		{
			say("file:",addr[i+2]);
			explainmft(addr[i+2]);
			return;
		}
	}
	if(i==0x100)
	{
		say("directory not found",0);
		return;
	}
}


int mountntfs(QWORD sector)
{
	sector0=sector;

        read(0x130000,sector,getdisk(),1); //pbr
	say("ntfs sector:",sector);

	if( *(DWORD*)0x130003 != 0x5346544e ) return -1;

	pwd[0]=5;
	ntfspwd=0;

	clustersize=(QWORD)( *(BYTE*)0x13000d );
	say("clustersize:",clustersize);
	mftcluster= *(QWORD*)0x130030;
	say("mftcluster:",mftcluster);
	QWORD indexsize=clustersize * (QWORD)( *(BYTE*)0x130044 );
	say("indexsize:",indexsize);

        read(0x138000,sector0+mftcluster*clustersize,getdisk(),0x20);
	cachecurrent=0xffff;		//no mft cache yet

	explainmft(5);			//cd /
	finishntfs();

	return 0;
}
