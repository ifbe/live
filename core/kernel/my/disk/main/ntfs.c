#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long

#define diskhome 0x400000
#define programhome 0x2000000

#define pbrbuffer diskhome+0x30000
#define mftbuffer diskhome+0x40000
#define indexbuffer diskhome+0x80000
#define rawbuffer diskhome+0xc0000


static QWORD diskaddr;
static QWORD ntfssector;
static QWORD cacheblock;
static QWORD cachecurrent;
static QWORD clustersize;
static QWORD mftcluster;
static QWORD rawpointer;

static QWORD pwd[10];
static int ntfspwd;


void explainrun(QWORD runaddr,long long* offset,long long* count)
{
	//变量
	BYTE* run=(char*)runaddr;
	BYTE data= run[0];
	BYTE low4bit=0xf & data;
	BYTE high4bit=data >>4;
	int i;
	long long temp;

	//簇数
	temp=0;
	for(i=low4bit;i>0;i--)
	{
		temp=(temp<<8)+(QWORD)run[i];
	}
	//say("runcount:",temp);
	*count=temp*clustersize;

	//簇号
	i=low4bit+high4bit;
	if( (run[i] < 0x80) | (*offset==0) )	//正数 或者 传进来的是第一个run
	{
		temp=(long long)run[i];		//0x68变成0x0000000000000068
	}
	else					//负数 且 不是第一个run
	{
		temp=(long long)(char)run[i];	//0x98变成0xffffffffffffff98
	}

	for(i=low4bit+high4bit-1;i>low4bit;i--)
	{
		temp=(temp<<8)+(long long)run[i];
	}

	//从pbr开始偏移的扇区数
	*offset=*offset + temp*clustersize;

}


void mftpart(QWORD runaddr,QWORD mftnum)	//datarun地址，mft号
{
	say("mftrun@",runaddr);
	say("{",0);

	//变量们
	QWORD rdi=mftbuffer;
	BYTE data;
	long long offset=0;
	long long count;

	QWORD wishstart=2 * ( mftnum&0xffffffffffffff00 );
	QWORD wishend=wishstart+0x200;
	QWORD start=0;
	QWORD end=0;

	//每个run走一轮，需要的部分读进内存
	while(1)
	{
		data= *(BYTE*)runaddr;
		if(data == 0) break;

		explainrun(runaddr,&offset,&count);
		say("    sector:",ntfssector+offset);
		say("    size:",count);

		runaddr= runaddr + 1 + (QWORD)(data & 0xf) + (QWORD)(data >> 4);
		start=end;
		end += count;

		//判断是否是我们要的，是就读进内存
		//			[----wishstart,wishend----]
		//[------------------------------------------------------]
		//1:[--start,end--]
		//2:[----------start,end------------]
		//3:[-------------------start,end------------------------]
		//4:			[-----start,end------]
		//5:			[-----------start,end------------]
		if(end > wishstart)	//读[wishstart,?]
		{
			//即将被读的开始扇区号
			QWORD temp1;
			//本run地址=ntfssector+offset
			temp1=ntfssector+offset;
			//本run中再偏移=(end-start)-(end-wishstart)
			temp1=temp1+(end-start)-(end-wishstart);

			if(end>=wishend)	//读满0x40000字节
			{
				//即将被读的扇区数
				QWORD temp2;
				//本run结束-期待的块开始
				temp2=(mftbuffer+0x40000-rdi)/0x200;

				read(rdi,temp1,diskaddr,temp2);

				say("    lastpart:",temp1);
				say("}",0);
				say("",0);
				return;
			}
			else			//从wishstart读到本run结束
			{
				//即将被读的扇区数
				QWORD temp2;
				//本run结束-期待的块开始
				temp2=end-wishstart;

				read(rdi,temp1,diskaddr,temp2);
				rdi+=temp2*0x200;
				wishstart=end;
			}
		}

	}
}


void checkmftcache(QWORD mftnum)
{
	//是否需要重新载入某块mft
	cacheblock=mftnum & 0xffffffffffffff00;
	if(cachecurrent == cacheblock) return;

	//开始找那一块地址并且读取
	QWORD offset=pbrbuffer+0x8000 + ( *(WORD*)(pbrbuffer+0x8014) );
	DWORD property;
	while(1)
	{
		if(offset > pbrbuffer+0x8400) break;
		property= *(DWORD*)offset;

		if(property == 0xffffffff)
		{
			//结束了，mft没有80属性
			say("wrong mft@138000",0);
			return;
		}
		if(property==0x80)
		{
			//从mft的dararun中读取我们要的部分mft
			mftpart(offset+(*(QWORD*)(offset+0x20)),mftnum);
			cachecurrent = cacheblock;
			return;
		}

		//offset=下一个property地址
		offset += *(DWORD*)(offset+4);
	}
}


void datarun(QWORD rdi,QWORD runaddr)
{
	say("run@",runaddr);
	say("{",0);

	//变量们
	BYTE data;
	long long offset=0;
	long long count;

	//每个run来一次
	while(1)
	{
		data= *(BYTE*)runaddr;
		if(data == 0) break;

		explainrun(runaddr,&offset,&count);
		say("    sector:",ntfssector+offset);
		say("    size:",count);

		runaddr= runaddr + 1 + (QWORD)(data & 0xf) + (QWORD)(data >> 4);

		//读进内存
		read(rdi,ntfssector+offset,diskaddr,count);
		rdi+=count*0x200;
	}

	say("}",0);
}


void explain80(QWORD addr)	//file data
{
	say("80@",addr);

	if( *(BYTE*)(addr+8) == 0 )
	{
		say("resident80",0);
		DWORD length = *(DWORD*)(addr+0x10);
		BYTE* rsi=(BYTE*)(addr + (QWORD)(*(DWORD*)(addr+0x14)) );
		BYTE* rdi=(BYTE*)programhome;
		int i;
		for(i=0;i<length;i++) rdi[i]=rsi[i];

		return;
	}
	else
	{
		say("non resident80",0);
		datarun(programhome , addr + (*(QWORD*)(addr+0x20)) );
	}
}


void index2raw(QWORD start,QWORD end)
{
	//say("    index2raw:",rawpointer);

	BYTE* buffer=(BYTE*)rawpointer;
	QWORD index=start;
	int i;

	while(index<end)
	{
		if( *(DWORD*)(index+8) <= 0x18 ) break;

		for(i=0;i<*(BYTE*)(index+0x50);i++)
		{
			if(i>=0x10) break;
			buffer[i]= *(BYTE*)(index+0x52+i*2);
		}
		*(QWORD*)(buffer+0x10)= (*(QWORD*)index)&0xffffffffffff;

		index+= *(WORD*)(index+0x8);
		rawpointer+=0x20;
		buffer+=0x20;
	}
}


void explain90(QWORD addr)	//index root
{
	say("90@",addr);

	addr += *(DWORD*)(addr+0x14);	//现在addr=属性体地址=索引根地址

	addr+=0x10;			//现在addr=索引头地址
	QWORD size=(QWORD)( *(DWORD*)(addr+4) );

	addr+=0x10;			//现在addr=第一个索引项地址

	//剩下的事(这块以后要改，排序什么的)
	if( *(BYTE*)(addr+0xc) ==0 )	//是小索引
	{
		index2raw(addr,addr+size);
	}
	else				//是大索引
	{
		index2raw(addr,addr+size);
	}
}


void explainindex(QWORD addr)
{
	if( *(DWORD*)addr !=0x58444e49 )
	{
		say("wrong index",0);
		return;
	}

	say("    vcn:",*(QWORD*)(addr+0x10));
	QWORD start=addr + 0x18 + ( *(DWORD*)(addr+0x18) );
	QWORD end=addr + ( *(DWORD*)(addr+0x1c) );
	index2raw(start,end);
}


void explaina0(QWORD addr)	//index allocation
{
	say("a0@",addr);
	datarun(indexbuffer , addr + (*(QWORD*)(addr+0x20)) );

	//*(QWORD*)rawpointer= 0x2e;
	//*(QWORD*)(rawpointer+0x10)= (*(QWORD*)(indexbuffer+0x10))&0xffffffffffff;
	//rawpointer+=0x20;

	say("INDX@",indexbuffer);
	say("{",0);

	QWORD p=indexbuffer;
	while( *(DWORD*)p ==0x58444e49 )
	{
		explainindex(p);
		p+=0x1000;
	}

	say("}",0);
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
	say("",0);
}


static int ntfs_cd(BYTE* addr)
{
	//变量们
	QWORD name;
	QWORD mftnumber;
	QWORD* memory=(QWORD*)(rawbuffer);
	int i;

	//传进来的名字处理一下
	str2data(addr,&name);
	blank2zero(&name);

	//几种特殊的名字
	switch(name)
	{
		case 0x2e:{	//	.
			return 0;
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
				if(memory[i] ==name )
				{
					mftnumber=memory[i+2];
					say("directory:",mftnumber);
					if(ntfspwd<10) ntfspwd++;
					pwd[ntfspwd]=mftnumber;
					break;
				}
			}
			if(i==0x100)
			{
				say("directory not found",0);
				return -1;
			}
		}
	}

	//清理indexbuffer,rawbuffer
	memory=(QWORD*)(indexbuffer);
	for(i=0;i<0x800;i++) memory[i]=0;	//clear [180000,1bfff8]
	memory=(QWORD*)(rawbuffer);
	for(i=0;i<0x800;i++) memory[i]=0;	//clear [1c0000,1ffff8]

	//当前位置
	rawpointer=rawbuffer;

	//开始change directory
	explainmft(mftnumber);

	return 1;
}


static void ntfs_load(BYTE* addr)
{
	//变量们
	QWORD name=0;
	QWORD mftnumber;
	QWORD* memory=(QWORD*)(rawbuffer);
	int i;

	//处理名字
	str2data(addr,&name);
	blank2zero(&name);

	//搜索
	for(i=0;i<0x100;i+=4)
	{
		if(memory[i] ==name )
		{
			mftnumber=memory[i+2];
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
	diskaddr=*(QWORD*)(0x200000+8);

	//读PBR，失败就返回
        read(pbrbuffer,sector,diskaddr,1);
	if( *(DWORD*)(pbrbuffer+3) != 0x5346544e ) return -1;

	//记下PBR地址
	ntfssector=sector;
	say("ntfs sector:",sector);

	//变量
	clustersize=(QWORD)( *(BYTE*)(pbrbuffer+0xd) );
	say("clustersize:",clustersize);
	mftcluster= *(QWORD*)(pbrbuffer+0x30);
	say("mftcluster:",mftcluster);
	QWORD indexsize=clustersize * (QWORD)( *(BYTE*)(pbrbuffer+0x44) );
	say("indexsize:",indexsize);
	say("",0);

	//保存开头几个mft
        read(pbrbuffer+0x8000,ntfssector+mftcluster*clustersize,diskaddr,0x20);
	cachecurrent=0xffff;		//no mft cache yet

	//cd /
	pwd[0]=5;
	ntfspwd=0;
	ntfs_cd("/");

	//保存函数地址
        remember(0x6463,(QWORD)ntfs_cd);
        remember(0x64616f6c,(QWORD)ntfs_load);
	return 0;
}
