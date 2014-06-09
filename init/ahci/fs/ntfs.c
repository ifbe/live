#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long


static QWORD sector0;
static QWORD clustersize;
static QWORD indexsize;
static QWORD mftcluster;
static QWORD mftsize;
static QWORD pwd[10];
static int ntfspwd;


void mftcache()
{
        read(0x140000,sector0+mftcluster*clustersize,getdisk(),0x200); //pbr
}


void datarun(QWORD rdi,QWORD addr)
{
	BYTE data= *(BYTE*)addr;
	say("[a0]data:",data);

	BYTE low4bit=0xf & data;
	int count=0;
	while(low4bit)
	{
		count=count*256+(int)(*(BYTE*)(addr+low4bit));
		low4bit--;
	}
	say("[a0]cluster count:",count);

	low4bit=0xf & data;
	BYTE high4bit=data >>4;
	int number=0;
	while(high4bit)
	{
		number=number*256+(int)(*(BYTE*)(addr+low4bit+high4bit));
		high4bit--;
	}
	say("[a0]cluster number:",number);

	read(rdi,sector0+number*clustersize,getdisk(),clustersize*count);

	rdi+=clustersize*count*0x200;
	addr= addr + (QWORD)(data & 0xf) + (QWORD)(data >> 4);
}


void explain80(QWORD addr)			//addr=property 90
{
	if( *(BYTE*)(addr+8) == 0 )
	{
		say("resident",0);
		DWORD length = *(DWORD*)(addr+0x10);
		BYTE* rsi=(BYTE*)(addr + (QWORD)(*(DWORD*)(addr+0x14)) );
		BYTE* rdi=(BYTE*)0x200000;
		int i;
		for(i=0;i<length;i++) rdi[i]=rsi[i];

		return;
	}
	else
	{
		say("non resident",0);
		datarun(0x200000 , addr + (*(QWORD*)(addr+0x20)) );
	}
}


void explainindex(QWORD start,QWORD end)
{
	BYTE* rdi=(BYTE*)0x1c0000;
	QWORD index=start;
	int i;
	for(i=0;i<0x800;i++) rdi[i]=0;		//clear [1c0000,1ffff8]

	*(QWORD*)rdi= 0x2e;
	say("index:",index);
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
	addr += *(DWORD*)(addr+0x14);		//addr=property body
	if( *(BYTE*)(addr+0x1c) !=0 )
	{
		say("[90]ignored for a0",0);
		return;
	}

	explainindex(addr+0x10,addr+0x10+ (QWORD)(*(DWORD*)(addr+4)) );
}


void explaina0(QWORD addr)
{
	datarun(0x180000 , addr + (*(QWORD*)(addr+0x20)) );

	if( *(DWORD*)0x180000 !=0x58444e49 )
	{
		say("data run fail",0);
		return;
	}

	QWORD start=0x180018+(*(DWORD*)0x180018);
	//say("index start:",start);
	QWORD end=0x180000 + (*(DWORD*)0x18001c);
	//say("index end:",end);
	explainindex(start,end);
}


void explainmft(QWORD data)
{
	QWORD mft=0x140000+0x400*data;
	if( *(DWORD*)mft !=0x454c4946 )
	{
		say("[mft]wrong:",data);
		return;
	}

	QWORD offset=mft + ( *(WORD*)(mft+0x14) );
	while(offset<mft + 0x400)
	{
		switch( *(WORD*)offset )
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
				say("[80]file data:",offset);
				explain80(offset);
				break;
			}
			case 0x90:{
				say("[90]index root:",offset);
				explain90(offset);
				break;
			}
			case 0xa0:
			{
				say("[a0]index allocation:",offset);
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
	indexsize=(QWORD)( *(BYTE*)0x130044 );
	say("indexsize:",indexsize);
	mftsize=(QWORD)( *(BYTE*)0x130040 );
	say("mftsize:",mftsize);
	mftcluster= *(QWORD*)0x130030;
	say("mftcluster:",mftcluster);

	mftcache();
	explainmft(5);

	finishntfs();

	return 0;
}
