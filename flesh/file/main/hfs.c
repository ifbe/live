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
//
static QWORD block0;
static QWORD blocksize;
static QWORD catalogsector;
static QWORD nodesize;




static void hfs_explain(QWORD number)
{
	say("%llx@%llx\n",number,catalogsector+nodesize*number);
	readdisk(readbuffer,catalogsector+nodesize*number,0,nodesize);	//0x1000
	printmemory(readbuffer,0x200);

	//1.解释节点头
	DWORD rightbrother=BSWAP_32(*(DWORD*)readbuffer);
	DWORD leftbrother=BSWAP_32(*(DWORD*)(readbuffer+4));
	BYTE type=*(BYTE*)(readbuffer+8);
	say("rightbro=%x\nleftbro=%x\ntype:%x\n",rightbrother,leftbrother,type);
	if(type==1|type==2)return;

	//2.解释节点内容，每个index和leaf节点尾巴上有，每个record的位置
	int offset=nodesize*0x200-2;
	while(1)
	{
		//总节点尾巴上找到record在哪儿，指针=0，或者指针>本身位置，就退出
		int temp=BSWAP_16(*(WORD*)(readbuffer+offset));
		if(temp==0)break;
		if(temp>=offset)break;

		//拿到keylength，如果这个位置是最后的空记录，就退出
		int keylen=BSWAP_16(*(WORD*)(readbuffer+temp));
		if(keylen==0)break;

		//打印
		say("%llx@%llx,%4x\n",temp,offset,keylen);

		//
		say("key=");
		int i;
		for(i=0;i<6;i++)
		{
			say("%2x,",*(BYTE*)(readbuffer+temp+2+i));
		}
		DWORD child=BSWAP_32(*(DWORD*)(readbuffer+temp+2+keylen));
		say("child=%.8llx\n",child);

		//
		offset-=2;
	}
}


static void hfs_cd()
{

}


static void hfs_load()
{

}




void printhead()
{
	printmemory(readbuffer+0x400,0x200);
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

	//检查magic值
	if( *(WORD*)(readbuffer+0x400) == 0x2b48 )
	{
		say("hfs+\n");
	}
	else if( *(WORD*)(readbuffer+0x400) == 0x5848 )
	{
		say("hfsx\n");
	}
	printhead();

	//blocksize
	blocksize=BSWAP_32( *(DWORD*)(readbuffer+0x428) )/0x200;
	//catalogsize
	catalogsector=block0+8*BSWAP_32(*(DWORD*)(readbuffer+0x520) );
	//读catalog，得到nodesize
	readdisk(readbuffer,catalogsector,0,0x8);	//0x1000
	nodesize=BSWAP_16( *(WORD*)(readbuffer+0x20) )/0x200;

	say("1block=%llxsector\n",blocksize);
	say("catalogsector:%llx\n",catalogsector);
	say("1node=%llxsector\n",nodesize);

	return 0;
}
