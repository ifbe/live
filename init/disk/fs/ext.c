#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long
#define inodebuffer 0x140000
#define tablebuffer 0x180000
#define rawbuffer 0x1c0000


static QWORD block0;
static QWORD blocksize;
static QWORD groupsize;
static QWORD inodepergroup;
static QWORD inodesize;
static QWORD cachecurrent;
static QWORD cacheblock;


QWORD getinodeblock(QWORD groupnum)
{
	QWORD sector;
	QWORD addr;

	//group descriptor从哪个扇区开始
	if(blocksize==2) sector=block0+4;
	else sector=block0+blocksize;

	//从这个扇区往后多少扇区
	sector+=groupnum/(0x200/0x20);

	//肯定在这个扇区里面
	read(0x138000,sector,getdisk(),1);

	//每0x20描述一个组，一个扇区里面偏移多少
	addr=0x138000+8+(groupnum*0x20)%0x200;

	//得到这一组inode表的block号
	return *(DWORD*)addr;

}


void checkinodecache(QWORD inode)
{
	QWORD rdi=inodebuffer;

	//内存里是这一块的话就不重新载入了
	cacheblock=(inode-1)*inodesize;
	cacheblock&=0xfffffffffffc0000;
	if(cachecurrent == cacheblock) return;

	say("inode:",inode);
	say("cacheblock:",cacheblock);
	say("{",0);


	//那一块第一个inode的号
	inode=1+cacheblock/inodesize;

	while(1)
	{
		if(rdi>=inodebuffer+0x40000) break;

		say("i:",inode);

		//此inode在第几块组
		QWORD groupnum=(inode-1)/inodepergroup;
		say("group number:",groupnum);

		//组内偏移（多少扇区）
		QWORD groupoffset=((inode-1) % inodepergroup)/(0x200/inodesize);
		say("group offset:",groupoffset);

		//得到这一组的块编号
		QWORD inodeblock=getinodeblock(groupnum);
		say("inode block:",inodeblock);

		//计算从哪一块开始
		QWORD where=block0+inodeblock*blocksize+groupoffset;
		say("sector:",where);

		//计算读多少块
		QWORD count;
		QWORD temp;

		//每扇区inode数量=0x200/inodesize
		//inode表里，每组多少扇区=inodepergroup/（0x200/inodesize）
		temp=inodepergroup/(0x200/inodesize);

		//计算这一次读多少个扇区
		if(groupoffset+0x200<=temp)
		{
			count=0x200;
		}
		else
		{
			count=temp-groupoffset;
			if(rdi-inodebuffer<0x200*count)
			{
				count=(inodebuffer+0x40000-rdi)/0x200;
			}
		}
		say("count:",count);

		//read inode table
	        read(rdi,where,getdisk(),count);

		rdi+=count*0x200;
		inode+=0x200/inodesize*count;
	}

	cachecurrent=cacheblock;
	say("}",0);
	say("",0);
}


void explaininode(QWORD rdi,QWORD inode)
{
	QWORD rsi;
	int i;

	checkinodecache(inode);

	say("inode:",inode);

	//(inode-1)%(0x40000/inodesize)
	rsi=inodebuffer+((inode-1)*inodesize)%0x40000;

	//检查是不是软链接
	WORD temp=(*(WORD*)rsi)&0xf000;
	if(temp == 0xa000){
		say("softlink:",0);
		say((char*)(rsi+0x28),0);
		say("",0);
		return;
	}

	rsi+=0x28;

	if(*(WORD*)rsi == 0xf30a)
	{
		QWORD numbers;

		say("extend@",rsi);
		numbers=*(WORD*)(rsi+2);
		say("numbers:",numbers);

		say("{",0);
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

		say("}",0);
		say("",0);
	}
	else
	{
	say("old@",rsi);
	say("{",0);
/*
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
*/
	say("}",0);
	say("",0);
	}
}


void table2raw(QWORD rsi)
{
	QWORD rdi=rawbuffer;
	int i;

	while(*(DWORD*)rsi != 0)
	{
		//先是名字
		i=0;
		for(i=0;i<*(BYTE*)(rsi+6);i++)
		{
			*(BYTE*)(rdi+i)=*(BYTE*)(rsi+8+i);
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
	QWORD* table=(QWORD*)rawbuffer;
	int i;
	QWORD number=0;

	//传进来的名字处理一下
	blank2zero(&name);

	//找inode
	switch(name)
	{
		case '/':
		{
			number=2;
			break;
		}
		default:
		{
			for(i=0;i<0x200;i+=4)
			{
				if(table[i] == name)
				{
					number=table[i+2];
					break;
				}
			}
			if(number == 0)
			{
				say("not found",0);
				return;
			}
		}
	}

	//清理buffer
	table=(QWORD*)tablebuffer;
	for(i=0;i<0x8000;i++) table[i]=0;
	table=(QWORD*)rawbuffer;
	for(i=0;i<0x8000;i++) table[i]=0;

	//开搞
	explaininode(tablebuffer,number);
	table2raw(tablebuffer);
}


void ext_load(QWORD name)
{
	QWORD* table=(QWORD*)rawbuffer;
	int i;
	QWORD number=0;

	//处理名字
	blank2zero(&name);

	for(i=0;i<0x200;i+=4)
	{
		if(table[i] == name)
		{
			number=table[i+2];
			break;
		}
	}
	if(number == 0)
	{
		say("not found",0);
		return;
	}

	explaininode(0x400000,number);
}


int mountext(QWORD sector)
{
	block0=sector;
	say("ext sector:",sector);

	//读分区前8扇区，总共0x1000字节
        read(0x130000,block0,getdisk(),0x8);	//0x1000

	//检查magic值
	if( *(WORD*)0x130438 != 0xef53 ) return;

	//变量们
	blocksize=*(DWORD*)0x130418;	//就不另开个temp变量了
	blocksize=( 1<<(blocksize+10) )/0x200;
	say("sectorperblock:",blocksize);
	groupsize=( *(DWORD*)0x130420 )*blocksize;
	say("sectorpergroup:",groupsize);
	inodepergroup=*(DWORD*)0x130428;
	say("inodepergroup:",inodepergroup);
	inodesize=*(WORD*)0x130458;
	say("byteperinode:",inodesize);

	//inode table缓存
	cachecurrent=0xffffffff;
	cacheblock=0;
	say("",0);

	//cd /
	ext_cd('/');

	//保存函数地址
	finishext();
	return 0;
}
