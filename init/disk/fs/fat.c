#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long
#define fatbuffer 0x140000
#define indexbuffer 0x180000
#define rawbuffer 0x1c0000

static QWORD fat0;
static QWORD fatsize;
static QWORD clustersize;
static QWORD cluster0;
static QWORD cachecurrent;
static QWORD cacheblock;

void checkfatcache()	//put cache in [0x140000]
{
	if(cachecurrent == cacheblock) return;


	QWORD i;
	for(i=0;i<0x40;i++){
		read(fatbuffer+i*0x1000,fat0+cacheblock*0x200+8*i,getdisk(),8);
	}
	say("[cache]reload:",fat0+cacheblock*0x200);
	cachecurrent=cacheblock;
}


void dir2raw()
{
	BYTE* rsi=(BYTE*)indexbuffer;
	BYTE* rdi=(BYTE*)rawbuffer;
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
	say("first cluster:",source);

	QWORD rdi=dest;
	while(rdi<dest+0x80000)
	{
		read(rdi,cluster0+clustersize*source,getdisk(),clustersize);
		rdi+=clustersize*0x200;

		source=(QWORD)(*(WORD*)(fatbuffer+2*source));

		if(source<2){say("impossible cluster,bye!",source);return;}
		if(source==0xfff7){say("bad cluster,bye!",source);return;}
		if(source>=0xfff8) break;
	}

	say("total bytes:",rdi-dest);
}


void fat16_root()
{
	BYTE* memory=(BYTE*)indexbuffer;
	int i;
	for(i=0;i<0x80000;i++) memory[i]=0;
	say("cd root:",fat0+fatsize*2);
	read(indexbuffer,fat0+fatsize*2,getdisk(),32);
	dir2raw();
}


void fat16_cd(QWORD name)
{
	QWORD directory=0;
	QWORD p=indexbuffer;
	int i;

	small2capital(&name);

	if( (name&0xff) != 0x2f)
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
			return;
		}
		directory=(QWORD)(*(WORD*)(p+0x1a)); //fat16,only 16bit
	}

	if(directory==0)
	{
		fat16_root();
		return;
	}

	//清理
	BYTE* memory=(BYTE*)indexbuffer;
	for(i=0;i<0x80000;i++) memory[i]=0;

	//读取
	fat16_data(indexbuffer,directory);

	//转换
	dir2raw();
}


void fat16_load(QWORD name)
{
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
	fat16_data(0x200000,file);
}


void fat16()
{
	QWORD fatsector=(QWORD)( *(DWORD*)0x13001c );
	say("fatsector:",fatsector);
	fatsize=(QWORD)( *(WORD*)0x130016 );
	say("fatsize:",fatsize);
	fat0=fatsector + (QWORD)( *(WORD*)0x13000e );
	say("fat0:",fat0);
	clustersize=(QWORD)( *(BYTE*)0x13000d );
	say("clustersize:",clustersize);
	cluster0=fat0+fatsize*2+32-clustersize*2;
	say("cluster0:",cluster0);
	say("",0);

	checkfatcache();
	fat16_cd('/');

	//保存函数地址
	finishfat16();
}


void fat32_data(QWORD dest,QWORD source)		//destine,clusternum
{
	say("first cluster:",source);

	QWORD rdi=dest;
	while(rdi<dest+0x80000)
	{
		read(rdi,cluster0+clustersize*source,getdisk(),clustersize);
		rdi+=clustersize*0x200;

		cacheblock=source/0x10000;
		checkfatcache();

		source=(QWORD)(*(DWORD*)(fatbuffer+4*(source%0x10000)));

		if(source<2){say("impossible cluster,bye!",source);return;}
		if(source==0x0ffffff7){say("bad cluster,bye!",source);return;}
		if(source>=0x0ffffff8) break;
	}
	say("total bytes:",rdi-dest);
}


void fat32_root()
{
	BYTE* memory=(BYTE*)indexbuffer;
	int i;
	for(i=0;i<0x80000;i++) memory[i]=0;

	say("cd root:",cluster0+clustersize*2);
	read(indexbuffer,cluster0+clustersize*2,getdisk(),32);
	dir2raw();
}


void fat32_cd(QWORD name)
{
	QWORD directory=0;
	QWORD p=indexbuffer;
	int i;

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
			return;
		}
		directory=((QWORD)(*(WORD*)(p+0x14)))<<16; //high 16bit
		directory+=(QWORD)(*(WORD*)(p+0x1a));  //low 16bit
	}

	if(directory==0)
	{
		fat32_root();
		return;
	}

	//清理
	BYTE* memory=(BYTE*)indexbuffer;
	for(i=0;i<0x80000;i++) memory[i]=0;

	//读取
	fat32_data(indexbuffer,directory);

	//转换
	dir2raw();
}


void fat32_load(QWORD name)
{
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

	fat32_data(0x200000,file);
}


void fat32()
{
	QWORD fatsector=(QWORD)( *(DWORD*)0x13001c );
	say("fatsector:",fatsector);
	fatsize=(QWORD)( *(DWORD*)0x130024 );
	say("fatsize:",fatsize);
	fat0=fatsector + (QWORD)( *(WORD*)0x13000e );
	say("fat0:",fat0);
	clustersize=(QWORD)( *(BYTE*)0x13000d );
	say("clustersize:",clustersize);
	cluster0=fat0+fatsize*2-clustersize*2;
	say("cluster0:",cluster0);
	say("",0);

	checkfatcache();
	fat32_cd('/');

	//保存函数地址
	finishfat32();
}


int mountfat(QWORD sector)
{
        read(0x130000,sector,getdisk(),1); //pbr

        if( *(WORD*)0x13000b !=0x200) {
                say("not 512B per sector,bye!",0);
                return -1;
        }
        if( *(BYTE*)0x130010 !=2) {
                say("not 2 fat,bye!",0);
                return -2;
        }

        int similarity=50;

        if( *(WORD*)0x130011 ==0) similarity++;         //fat32为0
        else similarity--;
        if( *(WORD*)0x130016 ==0) similarity++;         //fat32为0
        else similarity--;

	cachecurrent=0xffffffff;
	cacheblock=0;

        if(similarity==48)
        {
                say("fat16",0);
                fat16();
        }
        else if(similarity==52)
        {
                say("fat32",0);
                fat32();
        }
        else
        {
                say("seem not fatxx,bye!",0);
                return -3;
        }

        DWORD* pointer=(DWORD*)rawbuffer;
        int i;
        for(i=0;i<0x1000;i+=4)
        {
                if( pointer[i] == 0x4556494c )
                {
                        say("find our directory!!!!",0);
                        return 1;
                }
        }

        say("not in this partition",0);
        return -4;
}

