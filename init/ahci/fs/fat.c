#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long

static QWORD fat0;
static QWORD fatsize;
static QWORD clustersize;
static QWORD cluster0;
static QWORD cachecurrent=0xffffffff;
static QWORD cacheblock=0;

void fatcache()	//put cache in [0x180000]
{
	if(cachecurrent!=cacheblock)
	{
		QWORD i;
		for(i=0;i<0x40;i++){
			read(0x140000+i*0x1000,fat0+cacheblock*0x200+8*i,getdisk(),8);
		}
		say("[cache]reload:",fat0+cacheblock*0x200);
		cachecurrent=cacheblock;
	}
}


QWORD small2capital(QWORD name)
{
	QWORD temp;
	int i;
	for(i=0;i<8;i++)
	{
		temp=( name>>(i*8) )&0xff;
		if(temp>='a' && temp<='z')
		{
			name-= ( (QWORD)0x20 )<<(i*8);
		}
	}

	return name;
}


void explain()
{
	BYTE* rsi=(BYTE*)0x180000;
	BYTE* rdi=(BYTE*)0x1c0000;
	int i;

	for(i=0;i<0x4000;i++) rdi[i]=0;

	while(rsi<(BYTE*)0x1c0000)
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


void fat16_root()
{
	say("cd root:",fat0+fatsize*2);
	read(0x180000,fat0+fatsize*2,getdisk(),32);
	explain();
}


void fat16_data(QWORD dest,QWORD source)	//destine,clusternum
{
	say("first cluster:",source);

	QWORD rdi=dest;
	while(rdi<dest+0x80000)
	{
		read(rdi,cluster0+clustersize*source,getdisk(),clustersize);
		rdi+=clustersize*0x200;

		source=(QWORD)(*(WORD*)(0x140000+2*source));

		if(source<2){say("impossible cluster,bye!",source);return;}
		if(source==0xfff7){say("bad cluster,bye!",source);return;}
		if(source>=0xfff8) break;
	}

	say("total bytes:",rdi-dest);
}


void fat16_cd(QWORD name)
{
	name=small2capital(name);
	QWORD p=0x1c0000;
	for(;p>0x180000;p-=0x20)
	{
		if( *(QWORD*)p==name )
		{
			if( *(BYTE*)(p+0xb)&0x10) break;
		}
	}
	if(p==0x180000){say("directory not found!",p);return;}

	QWORD directory=(QWORD)(*(WORD*)(p+0x1a)); //fat16,only 16bit

	if(directory==0) fat16_root();
	else fat16_data(0x180000,directory);

	explain();
}


void fat16_load(QWORD name)
{
	name=small2capital(name);
	QWORD p=0x1c0000;
	for(;p>0x180000;p-=0x20){ if( *(QWORD*)p==name ) break;	}
	if(p==0x180000){say("file not found,bye!",0);return;}

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

	fatcache();
	fat16_root();
	finishfat16();
}


void fat32_root()
{
	read(0x180000,cluster0+clustersize*2,getdisk(),32);
	say("cd root:",cluster0+clustersize*2);
	explain();
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
		fatcache();

		source=(QWORD)(*(DWORD*)(0x140000+4*(source%0x10000)));

		if(source<2){say("impossible cluster,bye!",source);return;}
		if(source==0x0ffffff7){say("bad cluster,bye!",source);return;}
		if(source>=0x0ffffff8) break;
	}
	say("total bytes:",rdi-dest);
}


void fat32_cd(QWORD name)
{
	name=small2capital(name);
	QWORD p=0x180000+clustersize*0x200;
	for(;p>0x180000;p-=0x20)
	{
	    if( *(QWORD*)p== name)
	    {
		if( *(BYTE*)(p+0xb)&0x10) break;
	    }
	}
	if(p==0x180000){say("directory not found!",p);return;}

	QWORD directory=((QWORD)(*(WORD*)(p+0x14)))<<16; //high 16bit
	directory+=(QWORD)(*(WORD*)(p+0x1a));  //low 16bit

	if(directory==0) fat32_root();
	else fat32_data(0x180000,directory);

	explain();
}


void fat32_load(QWORD name)
{
	name=small2capital(name);
	QWORD p=0x180000+clustersize*0x200;
	for(;p>0x180000;p-=0x20)
	{
		if( *(QWORD*)p== name) break;
	}
	if(p==0x180000){say("file not found,bye!",0);return;}

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

	fatcache();

	//read(0x180000,cluster0+clustersize*2,getdisk(),32);
	//say("cd root:",cluster0+clustersize*2);
	fat32_cd(0x202020202020202f);

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

        unsigned int* pointer=(unsigned int*)0x180000;
        int i=0;
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

