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


void fat16_cd(QWORD name)
{
	QWORD p=0x200000;
	for(;p>0x180000;p-=0x20)
	{
		if( *(QWORD*)p==name )
		{
			if( *(BYTE*)(p+0xb)&0x10) break;
		}
	}
	if(p==0x180000){say("directory not found!",p);return;}

	QWORD directory=(QWORD)(*(WORD*)(p+0x1a)); //fat16,only 16bit

	if(directory==0){			//root
		read(0x180000,fat0+fatsize*2,getdisk(),32);
		say("cd root:",fat0+fatsize*2);
		return;
	}
	read(0x180000,cluster0+directory*clustersize,getdisk(),clustersize);
	say("cd cluster:",directory);
}

void fat16_load(QWORD name)
{
	QWORD p=0x200000;
	for(;p>0x180000;p-=0x20){ if( *(QWORD*)p==name ) break;	}
	if(p==0x180000){say("file not found,bye!",0);return;}

	QWORD file=(QWORD)(*(WORD*)(p+0x1a));	//fat16,only 16bit
	say("first cluster of file:",file);

	p=0x200000;
	while(p<0x400000)
	{
		read(p,cluster0+clustersize*file,getdisk(),clustersize);
		//say("read:",cluster0+clustersize*file);
		p+=clustersize*0x200;

		file=(QWORD)(*(WORD*)(0x140000+2*file));

		if(file<2){say("impossible cluster,bye!",file);return;}
		if(file==0xfff7){say("bad cluster,bye!",file);return;}
		if(file>=0xfff8){say("last cluster:",file);break;}
	}
	say("total bytes:",p-0x200000);
}


fat16()
{
	QWORD sector=(QWORD)( *(DWORD*)0x13001c );
	say("fat sector:",sector);
	fatsize=(QWORD)( *(WORD*)0x130016 );
	say("fatsize:",fatsize);
	fat0=sector + (QWORD)( *(WORD*)0x13000e );
	say("fat0:",fat0);
	clustersize=(QWORD)( *(BYTE*)0x13000d );
	say("clustersize:",clustersize);
	cluster0=fat0+fatsize*2+32-clustersize*2;
	say("cluster0:",cluster0);
	read(0x180000,fat0+fatsize*2,getdisk(),32);
	say("cd root:",fat0+fatsize*2);

	fatcache();
	finishfat16();
}


void fat32_cd(QWORD name)
{
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

	if(directory==0){
		read(0x180000,cluster0+clustersize*2,getdisk(),32);
		say("cd root:",cluster0+clustersize*2);
		return;
	}
	read(0x180000,cluster0+directory*clustersize,getdisk(),clustersize);
	say("cd cluster:",directory);
}

void fat32_load(QWORD name)
{
	QWORD p=0x180000+clustersize*0x200;
	for(;p>0x180000;p-=0x20)
	{
		if( *(QWORD*)p== name) break;
	}
	if(p==0x180000){say("file not found,bye!",0);return;}

	QWORD file=((QWORD)(*(WORD*)(p+0x14)))<<16; //high 16bit
	file+=(QWORD)(*(WORD*)(p+0x1a));  //low 16bit
	say("first cluster of file:",file);

	p=0x200000;
	while(p<0x400000)
	{
		read(p,cluster0+clustersize*file,getdisk(),clustersize);//1簇
		p+=clustersize*0x200;
		//say("read:",cluster0+clustersize*file);

		cacheblock=file/0x10000;
		fatcache();

		file=(QWORD)(*(DWORD*)(0x140000+4*(file%0x10000)));

		if(file<2){say("impossible cluster,bye!",file);return;}
		if(file==0x0ffffff7){say("bad cluster,bye!",file);return;}
		if(file>=0x0ffffff8){say("last cluster:",file);break;}
	}
	say("total bytes:",p-0x200000);
}


fat32()
{
	QWORD sector=(QWORD)( *(DWORD*)0x13001c );
	say("fat sector:",sector);
	fatsize=(QWORD)( *(DWORD*)0x130024 );
	say("fatsize:",fatsize);
	fat0=sector + (QWORD)( *(WORD*)0x13000e );
	say("fat0:",fat0);
	clustersize=(QWORD)( *(BYTE*)0x13000d );
	say("clustersize:",clustersize);
	cluster0=fat0+fatsize*2-clustersize*2;
	say("cluster0:",cluster0);
	read(0x180000,cluster0+clustersize*2,getdisk(),32);
	say("cd root:",cluster0+clustersize*2);

	fatcache();
	finishfat32();
}
