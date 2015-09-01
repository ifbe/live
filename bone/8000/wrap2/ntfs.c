#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long

#define fshome 0x900000			//[2m,3m)
#define pbrbuffer fshome+0x30000
#define mftbuffer fshome+0x40000
#define indexbuffer fshome+0x80000

#define rawbuffer 0xa00000			//[3m,4m)

#define programhome 0xb00000		//[4m,?)




void remember(QWORD first,QWORD second);
void diary(char* , ...);
void read(QWORD first,QWORD second,QWORD third,QWORD fourth);
void string2data(BYTE* str,QWORD* data);
void blank2zero(QWORD* name);




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
	//diary("runcount:%x",temp);
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
	diary("mftrun@%x{",runaddr);

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
		diary("	sector:%x",ntfssector+offset);
		diary("	size:%x",count);

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

				diary("	lastpart:%x",temp1);
				diary("}");
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
			diary("wrong mft");
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
	diary("run@%x{",runaddr);

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
		diary("	sector:%x",ntfssector+offset);
		diary("	size:%x",count);

		runaddr= runaddr + 1 + (QWORD)(data & 0xf) + (QWORD)(data >> 4);

		//读进内存
		read(rdi,ntfssector+offset,diskaddr,count);
		rdi+=count*0x200;
	}

	diary("}",0);
}


void explain80(QWORD addr)	//file data
{
	diary("80@%x",addr);

	if( *(BYTE*)(addr+8) == 0 )
	{
		diary("resident80");
		DWORD length = *(DWORD*)(addr+0x10);
		BYTE* rsi=(BYTE*)(addr + (QWORD)(*(DWORD*)(addr+0x14)) );
		BYTE* rdi=(BYTE*)programhome;
		int i;
		for(i=0;i<length;i++) rdi[i]=rsi[i];

		return;
	}
	else
	{
		diary("non resident80");
		datarun(programhome , addr + (*(QWORD*)(addr+0x20)) );
	}
}


void index2raw(QWORD start,QWORD end)
{
	//diary("    index2raw:%x",rawpointer);

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
	diary("90@%x",addr);

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
		diary("wrong index");
		return;
	}

	diary("	vcn:%x",*(QWORD*)(addr+0x10));
	QWORD start=addr + 0x18 + ( *(DWORD*)(addr+0x18) );
	QWORD end=addr + ( *(DWORD*)(addr+0x1c) );
	index2raw(start,end);
}


void explaina0(QWORD addr)	//index allocation
{
	diary("a0@%x",addr);
	datarun(indexbuffer , addr + (*(QWORD*)(addr+0x20)) );

	//*(QWORD*)rawpointer= 0x2e;
	//*(QWORD*)(rawpointer+0x10)= (*(QWORD*)(indexbuffer+0x10))&0xffffffffffff;
	//rawpointer+=0x20;

	diary("INDX@%x{",indexbuffer);

	QWORD p=indexbuffer;
	while( *(DWORD*)p ==0x58444e49 )
	{
		explainindex(p);
		p+=0x1000;
	}

	diary("}",0);
}


void explainmft(QWORD data)
{
	checkmftcache(data);		//reload cache
	QWORD mft=mftbuffer+0x400*(data % 0x100);    //0x40000/0x400=0x100个
	if( *(DWORD*)mft !=0x454c4946 )
	{
		diary("[mft]wrong:%x",data);
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
				diary("10@%x",offset);
				break;
			}
			case 0x20:{	//property list
				diary("20@%x",offset);
				break;
			}
			case 0x30:{	//unicode name
				diary("30@%x",offset);
				break;
			}
			case 0x40:{	//old volumn
				diary("40@%x",offset);
				break;
			}
			case 0x50:{	//object id
				diary("50@%x",offset);
				break;
			}
			case 0x60:{	//security??
				diary("60@%x",offset);
				break;
			}
			case 0x70:{	//volumn name
				diary("70@%x",offset);
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
				diary("b0@%x",offset);
				break;
			}
			case 0xc0:{	//symbolic link
				diary("c0@%x",offset);
				break;
			}
			case 0xd0:{	//ea information
				diary("d0@%x",offset);
				break;
			}
			case 0xe0:{	//ea
				diary("e0@%x",offset);
				break;
			}
			case 0xf0:{	//property set
				diary("f0@%x",offset);
				break;
			}
			case 0x100:{	//logged utility stream
				diary("100@%x",offset);
				break;
			}
			default:{
				diary("unknown@%x",offset);
				break;
			}
		}
		offset += *(DWORD*)(offset+4);
	}
	diary("");
}


static int ntfs_cd(BYTE* addr)
{
	//变量们
	QWORD name;
	QWORD mftnumber;
	QWORD* memory=(QWORD*)(rawbuffer);
	int i;

	//传进来的名字处理一下
	string2data(addr,&name);
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
					diary("directory:%x",mftnumber);
					if(ntfspwd<10) ntfspwd++;
					pwd[ntfspwd]=mftnumber;
					break;
				}
			}
			if(i==0x100)
			{
				diary("directory not found");
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
	string2data(addr,&name);
	blank2zero(&name);

	//搜索
	for(i=0;i<0x100;i+=4)
	{
		if(memory[i] ==name )
		{
			mftnumber=memory[i+2];
			diary("file:%x",mftnumber);
			break;
		}
	}
	if(i==0x100)
	{
		diary("directory not found");
		return;
	}

	//开始load
	explainmft(mftnumber);
}


int mountntfs(QWORD sector)
{
	diary("ntfs sector:%x",sector);

	//读PBR，失败就返回
	read(pbrbuffer,sector,0,1);
	if( *(DWORD*)(pbrbuffer+3) != 0x5346544e ) return -1;

	//记下PBR地址
	ntfssector=sector;
	diary("ntfs sector:%x",sector);

	//变量
	clustersize=(QWORD)( *(BYTE*)(pbrbuffer+0xd) );
	diary("clustersize:%x",clustersize);
	mftcluster= *(QWORD*)(pbrbuffer+0x30);
	diary("mftcluster:%x",mftcluster);
	QWORD indexsize=clustersize * (QWORD)( *(BYTE*)(pbrbuffer+0x44) );
	diary("indexsize:%x",indexsize);

	//保存开头几个mft
	read(pbrbuffer+0x8000,ntfssector+mftcluster*clustersize,0,0x20);
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
