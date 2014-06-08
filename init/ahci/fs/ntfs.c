#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long


static QWORD sector0;
static QWORD clustersize;
static QWORD indexsize;
static QWORD mftcluster;
static QWORD mftsize;


void mftcache()
{
        read(0x140000,sector0+mftcluster*clustersize,getdisk(),0x200); //pbr
}


void explaina0(BYTE* mft,int offset)
{
	QWORD rdi=0x180000;
	QWORD datarun=offset+(QWORD)mft[offset+0x20];

	BYTE data=mft[datarun];
	say("[a0]data:",data);
	BYTE low4bit=0xf & data;
	int count=0;
	while(low4bit)
	{
		count=count*256+mft[datarun+low4bit];
		low4bit--;
	}
	say("[a0]cluster count:",count);

	low4bit=0xf & data;
	BYTE high4bit=data >>4;
	int number=0;
	while(high4bit)
	{
		number=number*256+mft[datarun+low4bit+high4bit];
		high4bit--;
	}
	say("[a0]cluster number:",number);

	read(rdi,sector0+number*clustersize,getdisk(),clustersize*count);
	rdi+=clustersize*count*0x200;
}


void explainindex()
{
	QWORD index=0x180018+ ( *(DWORD*)0x180018 ) ;	//now index=first

	if( *(DWORD*)0x180000 !=0x58444e49 )
	{
		say("index wrong");
		return;
	}
	QWORD indexsize=*(DWORD*)0x18001c;
	say("index size:",indexsize);

	BYTE* rdi=(BYTE*)0x1c0000;
	while(index<0x180000+indexsize)
	{
		int i;
		for(i=0;i<0x10;i++) rdi[i]=0;
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



void explainmft(QWORD data)
{
	BYTE* mft=(BYTE*)data;
	if( *(DWORD*)mft !=0x454c4946 )
	{
		say("[a0]mft wrong");
		return;
	}

	int offset= *(WORD*)(&mft[0x14]);
	while(offset<0x400)
	{
		switch(mft[offset])
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
				break;
			}
			case 0x90:{
				say("[90]index root:",offset);
				break;
			}
			case 0xa0:
			{
				say("[a0]index allocation:",offset);
				explaina0(mft,offset);
				explainindex();
				break;
			}
			case 0xb0:{
				say("[b0]bitmap:",offset);
				break;
			}
			case 0:{
				break;
			}
			default:{
				say("[??]unknown",mft[offset]);
				break;
			}
		}

		if(mft[offset] == 0) break;
		offset+=( *(DWORD*)(&mft[offset+4]) );
	}
}


void ntfs_cd(QWORD name)
{
	blank2zero(&name);

	QWORD* addr=(QWORD*)0x1c0000;
	int i;
	for(i=0;i<0x100;i+=4)
	{
		if(addr[i] ==name )
		{
			say("live:",addr[i+2]);
			explainmft(0x140000+0x400*addr[i+2]);
			return;
		}
	}
	if(i==0x100)
	{
		say("directory not found",0);
		return;
	}
}


void ntfs_load()
{

}


void ntfs(QWORD sector)
{
	clustersize=(QWORD)( *(BYTE*)0x13000d );
	say("clustersize:",clustersize);
	indexsize=(QWORD)( *(BYTE*)0x130044 );
	say("indexsize:",indexsize);
	mftsize=(QWORD)( *(BYTE*)0x130040 );
	say("mftsize:",mftsize);
	mftcluster= *(QWORD*)0x130030;
	say("mftcluster:",mftcluster);

	mftcache();
	explainmft(0x141400);

	finishntfs();
}


int mountntfs(QWORD sector)
{
	sector0=sector;

        read(0x130000,sector,getdisk(),1); //pbr
	say("ntfs sector:",sector);

	if( *(DWORD*)0x130003 != 0x5346544e ) return -1;

	ntfs(sector);
	return 0;
}
