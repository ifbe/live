#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long

//memory
static QWORD directorybuffer;
static QWORD readbuffer;
static QWORD fatbuffer;
static QWORD indexbuffer;

//disk
static QWORD diskaddr;
static QWORD fat0;
static QWORD fatsize;
static QWORD clustersize;
static QWORD cluster0;
static QWORD cachecurrent;
static QWORD cacheblock;

void checkfatcache()	//put cache in +0x40000
{
	if(cachecurrent == cacheblock) return;


	QWORD i;
	for(i=0;i<0x40;i++){
		read(fatbuffer+i*0x1000,fat0+cacheblock*0x200+8*i,diskaddr,8);
	}
	say("cacheblock:%x\n",cacheblock);
	cachecurrent=cacheblock;
}


void dir2raw()
{
	BYTE* rsi=(BYTE*)(indexbuffer);
	BYTE* rdi=(BYTE*)(directorybuffer);
	int i;

	for(i=0;i<0x4000;i++) rdi[i]=0;

	while(rsi<(BYTE*)(indexbuffer+0x40000) )
	{
		if( rsi[0xb] !=0xf ){		//fat ignore
		if( rsi[0] !=0xe5 ){		//not deleted
		if( *(QWORD*)rsi !=0 ){		//have name
			*(QWORD*)rdi=*(QWORD*)rsi;

			QWORD temp=((QWORD)(*(WORD*)(rsi+0x14)))<<16; //high
			temp+=(QWORD)(*(WORD*)(rsi+0x1a));  //low
			*(QWORD*)(rdi+10)=temp;

			rdi+=0x20;
		}
		}
		}
		rsi+=0x20;
	}
}


void fat16_data(QWORD dest,QWORD source)	//destine,clusternum
{
	say("cluster:%x\n",source);

	QWORD rdi=dest;
	while(rdi<dest+0x80000)
	{
		read(rdi,cluster0+clustersize*source,diskaddr,clustersize);
		rdi+=clustersize*0x200;

		source=(QWORD)(*(WORD*)(fatbuffer+2*source));

		if(source<2){say("impossible cluster,bye!%x\n",source);return;}
		if(source==0xfff7){say("bad cluster,bye!%x\n",source);return;}
		if(source>=0xfff8) break;
	}

	say("count:%x\n",rdi-dest);
	say("\n");
}


void fat16_root()
{
	BYTE* memory=(BYTE*)(indexbuffer);
	int i;
	for(i=0;i<0x80000;i++) memory[i]=0;
	say("cd %x\n",fat0+fatsize*2);
	read(indexbuffer,fat0+fatsize*2,diskaddr,32);
	dir2raw();
	say("\n");
}


static int fat16_cd(BYTE* addr)
{
	QWORD name=0;
	QWORD directory=0;
	QWORD p=indexbuffer;
	int i;

	str2data(addr,&name);
	zero2blank(&name);
	small2capital(&name);
	say("name:%x\n",name);

	if( (name&0xff) == 0x2f)
	{
		fat16_root();
		return 0;
	}
	else
	{
		for(;p<indexbuffer+0x800;p+=0x20)
		{
			if( *(QWORD*)p == name )
			{
				if( *(BYTE*)(p+0xb)&0x10) break;
			}
		}
		if(p==indexbuffer+0x800)
		{
			say("directory not found\n");
			return -1;
		}
		directory=(QWORD)(*(WORD*)(p+0x1a)); //fat16,only 16bit
	}

	//清理
	BYTE* memory=(BYTE*)(indexbuffer);
	for(i=0;i<0x80000;i++) memory[i]=0;

	//读取
	fat16_data(indexbuffer,directory);

	//转换
	dir2raw();

	return 1;
}


static void fat16_load(BYTE* addr)
{
	QWORD name=0;
	str2data(addr,&name);
	zero2blank(&name);
	small2capital(&name);

	QWORD p=indexbuffer;
	for(;p<(indexbuffer+0x40000);p+=0x20)
	{
		if( *(QWORD*)p==name ) break;
	}
	if(p==indexbuffer+0x40000)
	{
		say("file not found,bye!\n");
		return;
	}

	QWORD file=(QWORD)(*(WORD*)(p+0x1a));	//fat16,only 16bit
	fat16_data(readbuffer,file);
}


void fat16(QWORD fatsector)
{
	//QWORD fatsector=(QWORD)( *(DWORD*)(readbuffer+0x1c) );
	//注意现在readbuffer里面还是pbr
	fatsize=(QWORD)( *(WORD*)(readbuffer+0x16) );
	say("fatsize:%x\n",fatsize);
	fat0=fatsector + (QWORD)( *(WORD*)(readbuffer+0xe) );
	say("fat0:%x\n",fat0);
	clustersize=(QWORD)( *(BYTE*)(readbuffer+0xd) );
	say("clustersize:%x\n",clustersize);
	cluster0=fat0+fatsize*2+32-clustersize*2;
	say("cluster0:%x\n",cluster0);
	say("\n");

	checkfatcache();
	fat16_cd("/");

	//保存函数地址
	//remember(0x6463,(QWORD)fat16_cd);
	//remember(0x64616f6c,(QWORD)fat16_load);

}


void fat32_data(QWORD dest,QWORD source)		//destine,clusternum
{
	say("cluster:%x\n",source);

	QWORD rdi=dest;
	while(rdi<dest+0x80000)
	{
		read(rdi,cluster0+clustersize*source,diskaddr,clustersize);
		rdi+=clustersize*0x200;

		cacheblock=source/0x10000;
		checkfatcache();

		source=(QWORD)(*(DWORD*)(fatbuffer+4*(source%0x10000)));

		if(source<2){say("impossible cluster,bye!%x\n",source);return;}
		if(source==0x0ffffff7){say("bad cluster,bye!%x\n",source);return;}
		if(source>=0x0ffffff8) break;
	}
	say("count:%x\n",rdi-dest);
	say("\n");
}


void fat32_root()
{
	BYTE* memory=(BYTE*)(indexbuffer);
	int i;
	for(i=0;i<0x80000;i++) memory[i]=0;

	say("cd root:%x\n",cluster0+clustersize*2);
	read(indexbuffer,cluster0+clustersize*2,diskaddr,32);
	dir2raw();
	say("\n");
}


static int fat32_cd(BYTE* addr)
{
	QWORD name=0;
	QWORD directory=0;
	QWORD p=indexbuffer;
	int i;

	str2data(addr,&name);
	zero2blank(&name);
	small2capital(&name);

	//搜索
	if((name&0xff) != 0x2f)
	{
		for(;p<indexbuffer+clustersize*0x200;p+=0x20)
		{
		    if( *(QWORD*)p== name)
		    {
			if( *(BYTE*)(p+0xb)&0x10) break;
		    }
		}
		if(p==indexbuffer+clustersize*0x200)
		{
			say("directory not found!%x\n",p);
			return -1;
		}
		directory=((QWORD)(*(WORD*)(p+0x14)))<<16; //high 16bit
		directory+=(QWORD)(*(WORD*)(p+0x1a));  //low 16bit
	}

	if(directory==0)
	{
		fat32_root();
		return 0;
	}

	//清理
	BYTE* memory=(BYTE*)(indexbuffer);
	for(i=0;i<0x80000;i++) memory[i]=0;

	//读取
	fat32_data(indexbuffer,directory);

	//转换
	dir2raw();

	return 1;
}


static void fat32_load(BYTE* addr)
{
	QWORD name=0;
	str2data(addr,&name);
	zero2blank(&name);
	small2capital(&name);

	QWORD p=indexbuffer;
	for(;p<indexbuffer+clustersize*0x200;p+=0x20)
	{
		if( *(QWORD*)p== name) break;
	}
	if(p==indexbuffer+clustersize*0x200)
	{
		say("file not found,bye!\n");
		return;
	}

	QWORD file=((QWORD)(*(WORD*)(p+0x14)))<<16; //high 16bit
	file+=(QWORD)(*(WORD*)(p+0x1a));  //low 16bit

	fat32_data(readbuffer,file);
}


void fat32(QWORD fatsector)
{
	//QWORD fatsector=(QWORD)( *(DWORD*)(readbuffer+0x1c) );
	//注意现在readbuffer里面还是pbr
	fatsize=(QWORD)( *(DWORD*)(readbuffer+0x24) );
	say("fatsize:%x\n",fatsize);
	fat0=fatsector + (QWORD)( *(WORD*)(readbuffer+0xe) );
	say("fat0:%x\n",fat0);
	clustersize=(QWORD)( *(BYTE*)(readbuffer+0xd) );
	say("clustersize:%x\n",clustersize);
	cluster0=fat0+fatsize*2-clustersize*2;
	say("cluster0:%x\n",cluster0);
	say("\n");

	checkfatcache();
	fat32_cd("/");

	//保存函数地址
	//remember(0x6463,(QWORD)fat32_cd);
	//remember(0x64616f6c,(QWORD)fat32_load);

}


int mountfat(QWORD sector,QWORD* cdfunc,QWORD* loadfunc)
{
	//准备好可用的内存地址
	getaddrofbuffer(&readbuffer);
	getaddrofdir(&directorybuffer);
	getaddroffs(&fatbuffer);
	indexbuffer=fatbuffer+0x40000;

	//fat版本
	//diskaddr=*(QWORD*)(0x200000+8);
	say("partition sector:%x\n",sector);

	read(readbuffer,sector,diskaddr,1); //pbr

	if( *(WORD*)(readbuffer+0xb) !=0x200)
	{
		say("not 512B per sector,bye!\n");
		return -1;
	}
	if( *(BYTE*)(readbuffer+0x10) != 2)
	{
		say("not 2 fat,bye!\n");
		return -2;
	}

	int similarity=50;

	if( *(WORD*)(readbuffer+0x11) == 0) similarity++;         //fat32为0
	else similarity--;
	if( *(WORD*)(readbuffer+0x16) ==0) similarity++;         //fat32为0
	else similarity--;

	cachecurrent=0xffffffff;
	cacheblock=0;

	if(similarity==48)
	{
		//返回cd和load函数的地址
		*cdfunc=(QWORD)fat16_cd;
		*loadfunc=(QWORD)fat16_load;

		say("fat16\n");
		fat16(sector);
	}
	else if(similarity==52)
	{
		//返回cd和load函数的地址
		*cdfunc=(QWORD)fat32_cd;
		*loadfunc=(QWORD)fat32_load;

		say("fat32\n");
		fat32(sector);
	}
	else
	{
		say("seem not fatxx,bye!\n");
		return -3;
	}

	return 0;
}

