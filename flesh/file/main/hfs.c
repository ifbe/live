#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long
#define	BSWAP_8(x)	((x) & 0xff)
#define	BSWAP_16(x)	((BSWAP_8(x) << 8) | BSWAP_8((x) >> 8))
#define	BSWAP_32(x)	((BSWAP_16(x) << 16) | BSWAP_16((x) >> 16))
#define	BSWAP_64(x)	((BSWAP_32(x) << 32) | BSWAP_32((x) >> 32))


//系统或者各种东西提供好的memory，这几个变量仅仅记录了位置
static QWORD readbuffer;
static QWORD directorybuffer;
static QWORD catalogbuffer;
//分区开始扇区，每块多少扇区
static QWORD block0;
static QWORD blocksize;
//catalog扇区位置，每个节点多少扇区
static QWORD catalogsector;
static QWORD nodesize;
//记下几个节点号
static QWORD rootnode;
static QWORD firstleafnode;




//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//（只用来调试）解释index节点
void explainindexnode()
{
	//每个index和leaf节点尾巴上有，每个record的位置
	int temp=nodesize*0x200;
	while(1)
	{
		//每次进来-2
		temp-=2;

		//从节点尾巴上找到record在哪儿，指针=0，或者指针>本身位置，就退出
		int offset=BSWAP_16(*(WORD*)(readbuffer+temp));
		if(offset==0)break;
		if(offset>=temp)break;

		//拿到keylength，如果这个位置是最后的空记录，就退出
		int keylen=BSWAP_16(*(WORD*)(readbuffer+offset));
		if(keylen==0)break;

		//打印
		say("@%x,len=%x,key=",offset,keylen);

		//
		int i;
		for(i=0;i<8;i++)
		{
			say("%.2x ",*(BYTE*)(readbuffer+offset+2+i));
		}
		DWORD child=BSWAP_32(*(DWORD*)(readbuffer+offset+2+keylen));
		say(",child=%x\n",child);

	}
}
//（只用来调试）解释叶节点的record的data部分
explainrecorddata(QWORD offset)
{
	switch( *(BYTE*)(readbuffer+offset+1) )
	{
		case 1:			//folder
		{
			say("folder,");
			say("cnid=%x,",BSWAP_32(*(DWORD*)(readbuffer+offset+8)));
			break;
		}
		case 2:			//file
		{
			say("file,");
			say("cnid=%x,",BSWAP_32(*(DWORD*)(readbuffer+offset+8)));
			break;
		}
		case 3:			//folderthread
		{
			say("folderthread,");
			break;
		}
		case 4:			//filethread
		{
			say("filethread,");
			break;
		}
	}
	say("\n");
}
//（只用来调试）解释叶节点
explainleafnode()
{
	//每个index和leaf节点尾巴上有，每个record的位置	//每个index和leaf节点尾巴上有，每个record的位置
	int temp=nodesize*0x200;
	while(1)
	{
		//每次进来-2
		temp-=2;

		//从节点尾巴上找到record在哪儿，指针=0，或者指针>本身位置，就退出
		int offset=BSWAP_16(*(WORD*)(readbuffer+temp));
		if(offset==0)break;
		if(offset>=temp)break;

		//拿到keylength，如果这个位置是最后的空记录，就退出
		int keylen=BSWAP_16(*(WORD*)(readbuffer+offset));
		if(keylen==0)break;

		//key第一部分，father
		DWORD father=BSWAP_32(*(DWORD*)(readbuffer+offset+2));
		say("@%x,len=%x,father=%x,data:",offset,keylen,father);

		//key后面的data
		int i;
		for(i=0;i<8;i++)
		{
			say("%.2x ",*(BYTE*)(readbuffer+offset+2+keylen+i));
		}
		say("\n");

		//目录和文件
		explainrecorddata(offset+2+keylen);

	}//while结束

}//函数结束
//（只用来调试）解释某个节点
static void hfs_explain(QWORD number)
{
	say("%llx@%llx\n",number,catalogsector+nodesize*number);
	readdisk(readbuffer,catalogsector+nodesize*number,0,nodesize);	//0x1000
	printmemory(readbuffer,0x1000);

	//1.解释节点头
	DWORD rightbrother=BSWAP_32(*(DWORD*)readbuffer);
	DWORD leftbrother=BSWAP_32(*(DWORD*)(readbuffer+4));
	BYTE type=*(BYTE*)(readbuffer+8);
	say("rightbro=%x,leftbro=%x,type:%x\n",rightbrother,leftbrother,type);

	//
	if(type==1)
	{
		//这是头节点
	}
	else if(type==0)
	{
		//index节点
		explainindexnode();
	}
	else if(type==0xff)
	{
		//叶节点
		explainleafnode();
	}
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------





//搜索b树，在节点里找到第一个大于等于cnid的key
//返回找到的节点内偏移，找不到就返回-1
int searchnodeforcnid(QWORD* in1,QWORD* in2)
{
	QWORD nodenum=*in1;
	DWORD want=*in2;
	say("enter node:%llx\n",nodenum);

	//把指定节点读到内存,顺便看看这节点是啥类型
	readdisk(readbuffer,catalogsector+nodenum*nodesize,0,nodesize);
	BYTE type=*(BYTE*)(readbuffer+8);

	//节点内每一个record找一遍，找本节点内第一个的偏移
	int temp=nodesize*0x200;
	int candidateoffset=-1;
	QWORD candidatekey=0;
	QWORD candidatechild=0;
	while(1)
	{
		//每次进来减少2，下一个指针
		temp-=2;


		//从节点尾巴上找到record在哪儿，不可能在0，也不可能大于尾巴上的指针的位置
		int offset=BSWAP_16(*(WORD*)(readbuffer+temp));
		if(offset==0)break;
		if(offset>=temp)break;


		//拿到keylength，如果这个位置是最后的那一个空记录，就退出
		int keylen=BSWAP_16(*(WORD*)(readbuffer+offset));
		if(keylen==0)break;


		//看这一个record的key
		DWORD key=BSWAP_32(*(DWORD*)(readbuffer+offset+2));


		//每次碰到key<want的，要把它设置为新的候选者，然后看下一个record继续等着
		//注意这里key==want也要设为候选者（这种情况，进入了两个if里面）
		if(key<=want)
		{
			//比如节点内是1，2，3，64，88，1270，3355，而want=4
			//那么最开始1是候选者，又被更好的2取代，又被更好的3取代
			candidateoffset=offset;
			candidatekey=key;
			candidatechild=BSWAP_32(*(DWORD*)(readbuffer+offset+2+keylen));
		}

		//碰到want<key的，后面就更不可能want<key了，就用这个候选者吧
		//注意这里want==key的完美选择，后面的同样也不需要看了（这种情况，进入了两个if里面）
		if(want<=key)
		{
			//检查一遍候选者
			if(candidateoffset==-1)
			{
				//want=0所以它会比所有的都小，但是cnid不可能=0，所以返回失败
				*in2=0;
				return;
			}
			/*
			if(candidateoffset==offset&&)		//有毛病吧！！！！！！！！！！！！！！
			{
				//选了当前这个record而不是前一个record，并且本node内最后一个record就是它
				//所以有可能，右兄弟节点里面有更好的候选者
				*in1=0;
				*in2=0;
				return;
			}*/


			//其他正常候选record，看节点类型分别处理（头节点和位图节点不可能被传进来吧？）
			if(type==0xff)
			{
				//要是叶子节点，因为指定要那一号
				//所以必须相等才能返回正常位置，不相等就返回失败
				if(want==key)
				{
					*in1=nodenum;
					*in2=candidateoffset;
				}
				else
				{
					*in2=0;
				}
				return;
			}
			if(type==0)
			{
				//要是index节点，就找它儿子麻烦
				*in1=candidatechild;
				*in2=want;
				searchnodeforcnid(in1,in2);
				return;
			}
		}//大概找到那一个key之后的处理

	}//大while(1)循环

}//函数结束




static void explaindirectory(QWORD temp1,QWORD wantcnid)
{
	QWORD rdi=directorybuffer;
	int i;

	//清理内存
	for(i=0;i<0x100000;i++)
	{
		*(BYTE*)(rdi+i)=0;
	}

	//第一个是当前目录信息（也就是.）
	*(QWORD*)rdi='.';		//1.name
	*(QWORD*)(rdi+0x10)=wantcnid;		//2.cnid
	rdi+=0x40;


	//然后是后面的记录
	int temp=nodesize*0x200;
	while(1)
	{
		//每次进来直接减2
		temp-=2;

		//这个节点结束了
		WORD offset=BSWAP_16(*(WORD*)(readbuffer+temp));
		if(offset==0)break;
		if(offset>=temp)break;

		//出毛病了，到指针最后一个都没找着
		int keylen=BSWAP_16(*(WORD*)(readbuffer+offset));
		if(keylen==0)return;

		//拿key，要是key>wantcnid就结束了,要是小于就越过这个直接找下一位
		DWORD key=BSWAP_32(*(DWORD*)(readbuffer+offset+2));
		if(key>wantcnid)break;
		if(wantcnid<key)continue;

		//既然到这儿，那么key==wantcnid
		//所以把这个record，翻译成这种格式就行：名字，id，种类，大小
		WORD namelen=BSWAP_16(*(WORD*)(readbuffer+offset+6));
		//say("%x@%x\n",namelen,offset);
		if(namelen>=0xf)namelen=0xf;
		i=0;
		for(i=0;i<namelen;i++)	//namelength=*(byte*)(rsi+6)
		{
			*(BYTE*)(rdi+i)=*(BYTE*)(readbuffer+offset+9+i*2);
			//say("%c",*(BYTE*)(readbuffer+offset+9+i*2));
		}
		//2.cnid号
		*(QWORD*)(rdi+0x10)=BSWAP_32(*(DWORD*)(readbuffer+offset+2+keylen+0x8));
		//3.type
		QWORD filetype=BSWAP_16(*(WORD*)(readbuffer+offset+2+keylen));
		*(QWORD*)(rdi+0x20)=filetype;
		//4.size
		if(filetype==2)
		{
			*(QWORD*)(rdi+0x30)=BSWAP_64(*(QWORD*)(readbuffer+offset+2+keylen+0x58));
		}

		if(*(DWORD*)rdi == 0)*(DWORD*)rdi=0x3f3f3f3f;
		rdi+=0x40;

	}//大while(1)循环


}//函数结束
//所谓cd，就是把fathercnid=want的那些记录，翻译成容易看懂的格式：名字，id，种类，大小
static int hfs_cd(BYTE* addr)
{
	//1.检查文件夹名字来寻找cnid，如果没有这个文件夹那么不管了返回
	QWORD* table=(QWORD*)(directorybuffer);	//一定要括号
	QWORD wantcnid=0;
	int i;
	if(addr[0]=='/')
	{
		//1，2这种肯定在第一个叶节点（吧）
		wantcnid=2;
	}
	else
	{
		for(i=0;i<0x200;i+=4)
		{
			if( compare(&table[i],addr) == 0 )
			{
				wantcnid=table[i+2];
				break;
			}
		}
	}
	if(wantcnid==0)
	{
		say("not found:%s\n",addr);
		return -1;
	}




	//2.已经知道了目录的cnid号，那么需要从b树里面找到节点号和节点内偏移
	QWORD temp1;	//传走的时候是最初的节点号，传回来的时候是找到的节点号
	QWORD temp2;	//传走的时候是请求的cnid，传回来的时候是找到的节点内偏移（0就出错）
	if(wantcnid==2)
	{
		//根肯定在最开始的地方，相当于稍微优化一下
		temp1=firstleafnode;
		temp2=2;
	}
	else
	{
		temp1=rootnode;
		temp2=wantcnid;
	}
	searchnodeforcnid(&temp1,&temp2);
	if( temp2 < 0xe )		//offset值不可能小于e
	{
		say("this cnid isn't in btree\n");
		return -2;
	}
	say("wantcnid:%llx,@node:%llx,offset:%llx\n",wantcnid,temp1,temp2);




	//3.既然上面找到了，那么就逐个翻译吧
	//（temp2那个返回值是为了省事给hfs_load函数准备的，但是hfs_cd只用它来判断搜索成功失败）
	explaindirectory(temp1,wantcnid);
}




void explainfile(QWORD temp1,QWORD wantcnid,QWORD fileoffset)
{
	
	QWORD rdi=readbuffer;
	int i;

	//清理内存
	for(i=0;i<0x100000;i++)
	{
		*(BYTE*)(rdi+i)=0;
	}

	//然后是后面的记录
	int temp=nodesize*0x200;
	while(1)
	{
		//每次进来直接减2
		temp-=2;

		//这个节点结束了
		WORD offset=BSWAP_16(*(WORD*)(readbuffer+temp));
		if(offset==0)break;
		if(offset>=temp)break;

		//出毛病了，到指针最后一个都没找着
		int keylen=BSWAP_16(*(WORD*)(readbuffer+offset));
		if(keylen==0)return;

		//拿key，要是key>wantcnid就结束了,要是小于就越过这个直接找下一位
		DWORD key=BSWAP_32(*(DWORD*)(readbuffer+offset+2));
		if(key>wantcnid)break;
		if(wantcnid<key)continue;

		//既然到这儿，那么key==wantcnid
		//所以把这个record，翻译成这种格式就行：名字，id，种类，大小
		WORD namelen=BSWAP_16(*(WORD*)(readbuffer+offset+6));
		//say("%x@%x\n",namelen,offset);
		if(namelen>=0xf)namelen=0xf;
		i=0;
		for(i=0;i<namelen;i++)	//namelength=*(byte*)(rsi+6)
		{
			*(BYTE*)(rdi+i)=*(BYTE*)(readbuffer+offset+9+i*2);
			//say("%c",*(BYTE*)(readbuffer+offset+9+i*2));
		}
		//2.cnid号
		*(QWORD*)(rdi+0x10)=BSWAP_32(*(DWORD*)(readbuffer+offset+2+keylen+0x8));
		//3.type
		QWORD filetype=BSWAP_16(*(WORD*)(readbuffer+offset+2+keylen));
		*(QWORD*)(rdi+0x20)=filetype;
		//4.size
		if(filetype==2)
		{
			*(QWORD*)(rdi+0x30)=BSWAP_64(*(QWORD*)(readbuffer+offset+2+keylen+0x58));
		}

		if(*(DWORD*)rdi == 0)*(DWORD*)rdi=0x3f3f3f3f;
		rdi+=0x40;

	}//大while(1)循环

}
static void hfs_load(BYTE* addr,QWORD fileoffset)
{
	//1.在自己的表里面搜索到这个文件名对应的cnid号
	QWORD* table=(QWORD*)(directorybuffer);	//一定要括号
	QWORD wantcnid=0;
	int i;
	for(i=0;i<0x200;i+=4)
	{
		if( compare(&table[i],addr) == 0 )
		{
			wantcnid=table[i+2];
			break;
		}
	}
	if(wantcnid==0)
	{
		say("not found:%s\n",addr);
		return -1;
	}




	//2.搜索b树搜索key，只能找它爸，没办法直接找到它！
	QWORD temp1=rootnode;
	QWORD temp2=*(DWORD*)(directorybuffer+0x10);	//
	searchnodeforcnid(&temp1,&temp2);
	if(temp2 < 0xe)
	{
		say("not found\n");
		return;
	}
	say("wantcnid:%llx,@node:%llx,offset:%llx\n",wantcnid,temp1,temp2);




	//3.从这个节点开始，record，的data部分，的fork信息里面，找到东西
	explainfile(temp1,wantcnid,fileoffset);
}




void printhead()
{
	QWORD sector;
	say("allocation\n");
	say("    size:%llx\n",BSWAP_64(*(QWORD*)(readbuffer+0x470) ) );
	say("    clumpsize:%llx\n",BSWAP_32(*(DWORD*)(readbuffer+0x478) ) );
	say("    totalblocks:%llx\n",BSWAP_32(*(DWORD*)(readbuffer+0x47c) ) );
	sector=block0+8*BSWAP_32(*(DWORD*)(readbuffer+0x480) );
	say("    sector:%llx,%lld\n",sector,sector);
	say("    count:%llx\n",blocksize*BSWAP_32(*(DWORD*)(readbuffer+0x484) ) );
	say("extents overflow\n");
	say("    size:%llx\n",BSWAP_64(*(QWORD*)(readbuffer+0x4c0) ) );
	say("    clumpsize:%llx\n",BSWAP_32(*(DWORD*)(readbuffer+0x4c8) ) );
	say("    totalblocks:%llx\n",BSWAP_32(*(DWORD*)(readbuffer+0x4cc) ) );
	sector=block0+8*BSWAP_32(*(DWORD*)(readbuffer+0x4d0) );
	say("    sector:%llx,%lld\n",sector,sector);
	say("    count:%llx\n",blocksize*BSWAP_32(*(DWORD*)(readbuffer+0x4d4) ) );
	say("catalog\n");
	say("    size:%llx\n",BSWAP_64(*(QWORD*)(readbuffer+0x510) ) );
	say("    clumpsize:%llx\n",BSWAP_32(*(DWORD*)(readbuffer+0x518) ) );
	say("    totalblocks:%llx\n",BSWAP_32(*(DWORD*)(readbuffer+0x51c) ) );
	sector=block0+8*BSWAP_32(*(DWORD*)(readbuffer+0x520) );
	say("    sector:%llx,%lld\n",sector,sector);
	say("    count:%llx\n",blocksize*BSWAP_32(*(DWORD*)(readbuffer+0x524) ) );
	say("attribute\n");
	say("    size:%llx\n",BSWAP_64(*(QWORD*)(readbuffer+0x560) ) );
	say("    clumpsize:%llx\n",BSWAP_32(*(DWORD*)(readbuffer+0x568) ) );
	say("    totalblocks:%llx\n",BSWAP_32(*(DWORD*)(readbuffer+0x56c) ) );
	sector=block0+8*BSWAP_32(*(DWORD*)(readbuffer+0x570) );
	say("    sector:%llx,%lld\n",sector,sector);
	say("    count:%llx\n",blocksize*BSWAP_32(*(DWORD*)(readbuffer+0x574) ) );
}
int mounthfs(QWORD sector,QWORD* explainfunc,QWORD* cdfunc,QWORD* loadfunc)
{
	block0=sector;

	//返回cd和load函数的地址
	*explainfunc=(QWORD)hfs_explain;
	*cdfunc=(QWORD)hfs_cd;
	*loadfunc=(QWORD)hfs_load;

	//准备好可用的内存地址
	getaddrofbuffer(&readbuffer);
	getaddrofdir(&directorybuffer);
	getaddroffs(&catalogbuffer);

	//读分区前8扇区，总共0x1000字节(其实只要分区内2号和3号扇区)
	readdisk(readbuffer,sector,0,0x8);	//0x1000
	printmemory(readbuffer+0x400,0x200);
	printhead();
	if( *(WORD*)(readbuffer+0x400) == 0x2b48 )say("hfs+\n");
	else if( *(WORD*)(readbuffer+0x400) == 0x5848 )say("hfsx\n");
	blocksize=BSWAP_32( *(DWORD*)(readbuffer+0x428) )/0x200;
	catalogsector=block0+8*BSWAP_32(*(DWORD*)(readbuffer+0x520) );
	say("blocksize:%x\n",blocksize);
	say("catalogsector:%llx\n",catalogsector);

	//读catalog的第0个node，得到nodesize
	readdisk(readbuffer,catalogsector,0,0x8);	//0x1000
	nodesize=BSWAP_16( *(WORD*)(readbuffer+0x20) )/0x200;
	rootnode=BSWAP_32(*(DWORD*)(readbuffer+0x10) );
	firstleafnode=BSWAP_32(*(DWORD*)(readbuffer+0x18) );
	say("nodesize:%x\n",nodesize);
	say("rootnode:%x\n",rootnode);
	say("firstleafnode:%x\n",firstleafnode);

	return 0;
}
