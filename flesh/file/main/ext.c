//块组结构:						[block0]				[block1]			[block2]				[block3]				[block4]
//第一组且每小块扇区数2:		[bootsector]			[superblock]		[groupdescriptor0]		[groupdescriptor1]		[groupdescriptor2]......
//第一组且每block扇区数4,8,16:	[bootsector+superblock]	[groupdescriptor0]	[groupdescriptor1]		[groupdescriptor2]		[groupdescriptor3]
//其他组不管每块扇区数是几		[superblock]			[groupdescriptor0]	[groupdescriptor1]		[groupdescriptor2]		[groupdescriptor3]
#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long

//memory
static QWORD directorybuffer;
static QWORD readbuffer;
static QWORD inodebuffer;

//disk
static QWORD diskaddr;
static QWORD block0;
static QWORD blocksize;
static QWORD groupsize;
static QWORD inodepergroup;
static QWORD inodesize;




//输入值：请求的组号
//返回值：这个组里面inode表的第一个block号
static BYTE blockrecord[512];
static QWORD whichblock(QWORD groupnum)
{
	//group descriptor从哪个扇区开始
	QWORD sector;
	if(blocksize==2) sector=block0+4;
	else sector=block0+blocksize;

	//从这个扇区起往后多少扇区
	sector+=groupnum/(0x200/0x20);

	//肯定在这个扇区里面
	read(blockrecord,sector,diskaddr,1);

	//每0x20描述一个组，一个扇区有16个组的信息
	QWORD addr=(QWORD)blockrecord+8+(groupnum*0x20)%0x200;
	return *(DWORD*)addr;
}
/*
static QWORD getinodeblock(QWORD groupnum)
{
	QWORD sector;
	QWORD addr;

	//group descriptor从哪个扇区开始
	if(blocksize==2) sector=block0+4;
	else sector=block0+blocksize;

	//从这个扇区起往后多少扇区
	sector+=groupnum/(0x200/0x20);

	//肯定在这个扇区里面
	read(blockrecord,sector,diskaddr,1);

	//每0x20描述一个组，一个扇区有16个组的信息
	addr=(QWORD)blockrecord+8+(groupnum*0x20)%0x200;

	//得到这一组inode表的block号
	return *(DWORD*)addr;
}
*/



//文件->人，inode->户口本里的记录，这个函数->管理员
//这个函数作用：每次指名要一个记录，管理员就从户口本里翻到那儿([0x400n+1号,0x400n+1号])再指给请求者看
//比如:0x3352号:[0x3001,0x3400]，0x182304号->[0x182001,182400]
//注意1:不管inodesize是120(0x400*120)还是128(0x400*128)还是256(0x400*256)，算出的值都能被0x200(一个扇区)整除
//注意2:每个组的inode数量一般为8192，是0x400的倍数
//注意3:inode首个的编号是1不是0
static QWORD firstinodeincache;
static QWORD checkcacheforinode(QWORD wanted)
{
	//内存里已经是这几个的话就直接返回
	QWORD inodeoffset=(wanted-1)%0x400;
	if(wanted-inodeoffset == firstinodeincache)
	{
		return inodebuffer+inodeoffset*inodesize;
	}


	//不是就麻烦了
	QWORD rdi=inodebuffer;
	QWORD this=wanted-inodeoffset;
	while(1)
	{
		//inode分散在各个group里面，可能需要从不同地方收集
		QWORD groupnum=(this-1)/inodepergroup;		//算出此inode在第几块组
		QWORD groupoffset=(this-1)%inodepergroup;			//在这一组内的偏移（几个）

		//算：哪个扇区,几个扇区
		QWORD where;
		where=block0+blocksize*whichblock(groupnum);	//这一组第一个inode所在扇区，不会出问题
		where+=groupoffset*inodesize/0x200;		//注意！！！！inodepergroup奇葩时这里出问题
		QWORD count;
		count=inodepergroup-groupoffset;	//这一group内剩余多少个
		if(count>0x400)count=0x400;			//这一组里剩余的太多的话，多余的不要

		//read inode table
		say("inode:%x@%x\n",this,where);
	    read(rdi,where,diskaddr,count*inodesize/0x200);	//注意！！！！inodepergroup奇葩时这里出问题

		//读满0x400个inode就走人
		rdi+=count*inodesize;				//注意！！！！inodepergroup奇葩时这里出问题
		this+=count;
		if(rdi-inodebuffer>=0x40000) break;
	}
	//mem2file(inodebuffer,"after.bin",0,0x40000);
	firstinodeincache=wanted-inodeoffset;
	return inodebuffer+inodeoffset*inodesize;
}
/*
static QWORD checkcacheforinode(QWORD wanted)
{
	//内存里已经是这几个的话就直接返回
	QWORD offset=(wanted-1)%0x400;
	if(wanted-offset == firstinodeincache)
	{
		return inodebuffer+offset*inodesize;
	}


	//不是就麻烦了
	QWORD rdi=inodebuffer;
	QWORD this=wanted-offset;
	while(1)
	{
		//读满0x400个inode就走人
		if(this-wanted>=0x400) break;
		say("reading inode:%x\n",this);

		//inode分散在各个group里面，可能需要从不同地方收集
		QWORD groupnum=(this-1)/inodepergroup;		//算出此inode在第几块组
		QWORD groupoffset=((this-1) % inodepergroup)/(0x200/inodesize);		//在这一组内的偏移（多少扇区）
		QWORD inodeblock=getinodeblock(groupnum);		//得到这一组的块编号
		QWORD where=block0+inodeblock*blocksize+groupoffset;		//计算从哪一块开始
		say("group number:%x\n",groupnum);
		say("group offset:%x\n",groupoffset);
		say("inode block:%x\n",inodeblock);
		say("sector:%x\n",where);

		//在这一group里面读多少块
		//每扇区inode数量=0x200/inodesize
		//inode表里，每组多少扇区=inodepergroup/（0x200/inodesize）
		QWORD count;
		QWORD temp=inodepergroup/(0x200/inodesize);

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
		say("count:%x\n",count);

		//read inode table
	    read(rdi,where,diskaddr,count);

		rdi+=count*0x200;
		this+=0x200/inodesize*count;
	}
	//mem2file(inodebuffer,"after.bin",0,0x40000);
	firstinodeincache=wanted-offset;
	return inodebuffer+offset*inodesize;
}
*/


//输入值：目的内存地址，要搞的inode号
//返回值：<=0就是失败
static int explaininode(QWORD inode)
{
	say("inode:%x{\n",inode);
	QWORD rsi=checkcacheforinode(inode);

	//检查是不是软链接
	WORD temp=(*(WORD*)rsi)&0xf000;
	//say("%x\n",temp);
	if(temp == 0xa000){
		say((char*)(rsi+0x28));
		say("(soft link)\n");
		return -1;
	}

	rsi+=0x28;
	if(*(WORD*)rsi == 0xf30a)		//ext4用新方式
	{
		QWORD numbers=*(WORD*)(rsi+2);
		say("extend@%x\n",rsi);
		say("numbers:%x\n",numbers);

		QWORD rdi;
		for(rdi=readbuffer;rdi<readbuffer+0x40000;rdi++)
		{
			*(BYTE*)rdi=0;
		}
		rdi=readbuffer;

		int i;
		rsi+=12;
		for(i=0;i<numbers;i++)
		{
			QWORD temp1;
			QWORD temp2;
			say("    @%x\n",rsi);

			temp1=*(DWORD*)(rsi+8);
			temp1+= *(WORD*)(rsi+6);
			temp1*=blocksize;
			temp1+=block0;
			say("    sector:%x\n",temp1);

			temp2=*(WORD*)(rsi+4);
			temp2*=blocksize;
			say("    count:%x\n",temp2);

			rsi+=12;

		    read(rdi,temp1,diskaddr,temp2);
			rdi+=0x200*blocksize;
		}

		say("}\n");
		return 1;
	}
	else				//ext2，3用老方式
	{
	say("old@%x\n",rsi);
	say("{\n");
/*
	for(i=0;i<8;i++)
	{
		if(*(DWORD*)rsi != 0)
		{
			QWORD temp;
			say("    pointer@%x\n",rsi);

			temp=block0+(*(DWORD*)rsi)*blocksize;
			say("sector:%x\n",temp);

		        read(rdi,temp,diskaddr,blocksize);
			rdi+=0x200*blocksize;
		}

		rsi+=4;
	}
*/
	say("}\n");
	return 2;
	}
}


static void explaindirectory()
{
	QWORD rdi=directorybuffer;
	QWORD rsi=readbuffer;
	int i;

	for(i=0;i<0x10000;i++)
	{
		*(BYTE*)(rdi+i)=0;
	}
	while(1)
	{
		//全零就结束
		if(*(DWORD*)rsi == 0)break;

		//1.名字
		i=0;
		for(i=0;i<*(BYTE*)(rsi+6);i++)	//namelength=*(byte*)(rsi+6)
		{
			*(BYTE*)(rdi+i)=*(BYTE*)(rsi+8+i);
		}
		//2.inode号
		*(QWORD*)(rdi+0x10)=*(DWORD*)rsi;
		//3.type
		*(QWORD*)(rdi+0x20)=*(BYTE*)(rsi+7);
		//4.size，ext的目录表里面没有文件大小，需要到inode表里面寻找
		//最后指向下一个
		rsi+=*(WORD*)(rsi+4);
		rdi+=0x40;
	}
}


static int ext_cd(BYTE* addr)
{
	//QWORD name;
	QWORD* table=(QWORD*)(directorybuffer);	//一定要括号
	int i;

	QWORD number=0;
	if(addr[0]=='/')
	{
		number=2;
	}
	else
	{
		for(i=0;i<0x200;i+=4)
		{
			if( compare(&table[i],addr) == 0 )
			{
				number=table[i+2];
				//say("number:%x",number);
				break;
			}
		}
	}
	if(number == 0)
	{
		say("not found:%s\n",addr);
		return -1;
	}

	//开搞
	int result=explaininode(number);
	if(result>0)explaindirectory();
	return 1;
}


static void ext_load(BYTE* addr)
{
	QWORD name;
	QWORD* table=(QWORD*)(directorybuffer);	//一定要括号
	int i;
	QWORD number=0;

	//处理名字
	str2data(addr,&name);
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
		say("not found\n");
		return;
	}

	explaininode(number);
}


int mountext(QWORD sector,QWORD* cdfunc,QWORD* loadfunc)
{
	//返回cd和load函数的地址
	*cdfunc=(QWORD)ext_cd;
	*loadfunc=(QWORD)ext_load;

	//准备好可用的内存地址
	getaddrofbuffer(&readbuffer);
	getaddrofdir(&directorybuffer);
	getaddroffs(&inodebuffer);
	block0=sector;				//say("ext sector:%x\n",sector);

	//读分区前8扇区，总共0x1000字节
	read(readbuffer,block0,diskaddr,0x8);	//0x1000

	//检查magic值
	if( *(WORD*)(readbuffer+0x438) != 0xef53 ) return;

	//变量们
	blocksize=*(DWORD*)(readbuffer+0x418);
	blocksize=( 1<<(blocksize+10) )/0x200;		//每块多少扇区
	say("sectorperblock:%x\n",blocksize);
	groupsize=( *(DWORD*)(readbuffer+0x420) )*blocksize;	//每组多少扇区
	say("sectorpergroup:%x\n",groupsize);
	inodepergroup=*(DWORD*)(readbuffer+0x428);		//每组多少个inode
	say("inodepergroup:%x\n",inodepergroup);
	inodesize=*(WORD*)(readbuffer+0x458);		//每inode多少字节
	say("byteperinode:%x\n",inodesize);

	//inode table缓存
	firstinodeincache=0xffffffff;

	//cd /
	ext_cd("/");

	return 0;
}
