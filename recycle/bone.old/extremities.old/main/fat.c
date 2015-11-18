#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long

#define diskhome 0x400000
#define programhome 0x2000000

#define pbrbuffer diskhome+0x30000
#define fatbuffer diskhome+0x40000
#define indexbuffer diskhome+0x80000
#define rawbuffer diskhome+0xc0000




void remember(QWORD,QWORD);
void say(char* first,QWORD second);
void read(QWORD,QWORD,QWORD,QWORD);
void str2data(BYTE* str,QWORD* data);
void zero2blank(QWORD* name);
void small2capital(QWORD* name);




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
	say("cacheblock:",cacheblock);
	cachecurrent=cacheblock;
}


void dir2raw()
{
	BYTE* rsi=(BYTE*)(indexbuffer);
	BYTE* rdi=(BYTE*)(rawbuffer);
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
	say("cluster:",source);

	QWORD rdi=dest;
	while(rdi<dest+0x80000)
	{
		read(rdi,cluster0+clustersize*source,diskaddr,clustersize);
		rdi+=clustersize*0x200;

		source=(QWORD)(*(WORD*)(fatbuffer+2*source));

		if(source<2){say("impossible cluster,bye!",source);return;}
		if(source==0xfff7){say("bad cluster,bye!",source);return;}
		if(source>=0xfff8) break;
	}

	say("count:",rdi-dest);
	say("",0);
}


void fat16_root()
{
	BYTE* memory=(BYTE*)(indexbuffer);
	int i;
	for(i=0;i<0x80000;i++) memory[i]=0;
	say("cd ",fat0+fatsize*2);
	read(indexbuffer,fat0+fatsize*2,diskaddr,32);
	dir2raw();
	say("",0);
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
say("name:",name);

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
			say("directory not found",0);
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
		say("file not found,bye!",0);
		return;
	}

	QWORD file=(QWORD)(*(WORD*)(p+0x1a));	//fat16,only 16bit
	fat16_data(programhome,file);
}


void fat16(QWORD fatsector)
{
	//QWORD fatsector=(QWORD)( *(DWORD*)(pbrbuffer+0x1c) );
	fatsize=(QWORD)( *(WORD*)(pbrbuffer+0x16) );
	say("fatsize:",fatsize);
	fat0=fatsector + (QWORD)( *(WORD*)(pbrbuffer+0xe) );
	say("fat0:",fat0);
	clustersize=(QWORD)( *(BYTE*)(pbrbuffer+0xd) );
	say("clustersize:",clustersize);
	cluster0=fat0+fatsize*2+32-clustersize*2;
	say("cluster0:",cluster0);
	say("",0);

	checkfatcache();
	fat16_cd("/");

	//保存函数地址
        remember(0x6463,(QWORD)fat16_cd);
        remember(0x64616f6c,(QWORD)fat16_load);

}


void fat32_data(QWORD dest,QWORD source)		//destine,clusternum
{
	say("cluster:",source);

	QWORD rdi=dest;
	while(rdi<dest+0x80000)
	{
		read(rdi,cluster0+clustersize*source,diskaddr,clustersize);
		rdi+=clustersize*0x200;

		cacheblock=source/0x10000;
		checkfatcache();

		source=(QWORD)(*(DWORD*)(fatbuffer+4*(source%0x10000)));

		if(source<2){say("impossible cluster,bye!",source);return;}
		if(source==0x0ffffff7){say("bad cluster,bye!",source);return;}
		if(source>=0x0ffffff8) break;
	}
	say("count:",rdi-dest);
	say("",0);
}


void fat32_root()
{
	BYTE* memory=(BYTE*)(indexbuffer);
	int i;
	for(i=0;i<0x80000;i++) memory[i]=0;

	say("cd root:",cluster0+clustersize*2);
	read(indexbuffer,cluster0+clustersize*2,diskaddr,32);
	dir2raw();
	say("",0);
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
			say("directory not found!",p);
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
		say("file not found,bye!",0);
		return;
	}

	QWORD file=((QWORD)(*(WORD*)(p+0x14)))<<16; //high 16bit
	file+=(QWORD)(*(WORD*)(p+0x1a));  //low 16bit

	fat32_data(programhome,file);
}


void fat32(QWORD fatsector)
{
	//QWORD fatsector=(QWORD)( *(DWORD*)(pbrbuffer+0x1c) );
	fatsize=(QWORD)( *(DWORD*)(pbrbuffer+0x24) );
	say("fatsize:",fatsize);
	fat0=fatsector + (QWORD)( *(WORD*)(pbrbuffer+0xe) );
	say("fat0:",fat0);
	clustersize=(QWORD)( *(BYTE*)(pbrbuffer+0xd) );
	say("clustersize:",clustersize);
	cluster0=fat0+fatsize*2-clustersize*2;
	say("cluster0:",cluster0);
	say("",0);

	checkfatcache();
	fat32_cd("/");

	//保存函数地址
        remember(0x6463,(QWORD)fat32_cd);
        remember(0x64616f6c,(QWORD)fat32_load);

}


int mountfat(QWORD sector)
{
	diskaddr=*(QWORD*)(0x200000+8);
	say("partition sector:",sector);

        read(pbrbuffer,sector,diskaddr,1); //pbr

        if( *(WORD*)(pbrbuffer+0xb) !=0x200) {
                say("not 512B per sector,bye!",0);
                return -1;
        }
        if( *(BYTE*)(pbrbuffer+0x10) != 2) {
                say("not 2 fat,bye!",0);
                return -2;
        }

        int similarity=50;

        if( *(WORD*)(pbrbuffer+0x11) == 0) similarity++;         //fat32为0
        else similarity--;
        if( *(WORD*)(pbrbuffer+0x16) ==0) similarity++;         //fat32为0
        else similarity--;

	cachecurrent=0xffffffff;
	cacheblock=0;

        if(similarity==48)
        {
                say("fat16",0);
                fat16(sector);
        }
        else if(similarity==52)
        {
                say("fat32",0);
                fat32(sector);
        }
        else
        {
                say("seem not fatxx,bye!",0);
                return -3;
        }

	return 0;
}

