//fat文件系统大概是下面这样：[头][fat表][数据]
//[首领][官员][一群兵组成团]，[图书馆][各个资料室][一堆书组成一个书架]


#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long

//memory
static QWORD fatbuffer;				//fat表
static QWORD readbuffer;			//一般使用
static QWORD directorybuffer;		//目录专用

//disk
static QWORD diskaddr;
static QWORD fat0;			//fat表所在扇区
static QWORD fatsize;		//fat表总共的扇区数量
static QWORD cluster0;		//0号簇所在扇区
static QWORD clustersize;	//每个簇的扇区数量


//[0,0xf]:name
//[0x10,0x1f]:time?
//[0x20,0x2f]:firstcluster
//[0x30,0x3f]:size
void explaindirectory()
{
	BYTE* rsi=(BYTE*)(readbuffer);
	BYTE* rdi=(BYTE*)(directorybuffer);
	int i;

	for(i=0;i<0x4000;i++) rdi[i]=0;

	while(rsi<(BYTE*)(readbuffer+0x2000) )
	{
		if( rsi[0xb] !=0xf ){		//fat ignore
		if( rsi[0] !=0xe5 ){		//not deleted
		if( *(QWORD*)rsi !=0 ){		//have name
			//name
			*(QWORD*)rdi=*(QWORD*)rsi;
			//cluster
			QWORD temp;
			temp=((QWORD)(*(WORD*)(rsi+0x14)))<<16; //high
			temp+=(QWORD)(*(WORD*)(rsi+0x1a));  //low
			*(QWORD*)(rdi+0x10)=temp;
			//type
			rdi[0x20]=rsi[0xb];
			//size
			*(QWORD*)(rdi+0x30)=*(DWORD*)(rsi+0x1c);
			//
			rdi+=0x40;
		}
		}
		}
		rsi+=0x20;
	}
}







//从收到的簇号开始一直读最多1MB，接收参数为目的内存地址，第一个簇号
int fat16_data(QWORD dest,QWORD cluster)
{
	say("cluster:%x\n",cluster);

	QWORD rdi=dest;
	while(rdi<dest+0x100000)		//大于1M的不管
	{
		//判断退出
		if(cluster<2)break;
		if(cluster==0xfff7){say("bad cluster:%x\n",cluster);break;}
		if(cluster>=0xfff8)break;

		//读一个簇
		read(rdi,cluster0+clustersize*cluster,diskaddr,clustersize);

		//准备下一个地址，找下一个簇，全部fat表在内存里不用担心
		rdi+=clustersize*0x200;
		cluster=(QWORD)(*(WORD*)(fatbuffer+2*cluster));
	}

	say("count:%x\n",rdi-dest);
	return rdi-dest;
}
//接收参数：文件名字符串，调用者要的文件内部偏移（以1M为单元）
static void fat16_load(BYTE* addr,QWORD offset)
{
//1:文件名转换成首簇号
	//1.1:把字符串转换成fat16喜欢的格式
	QWORD name=0;
	str2data(addr,&name);
	zero2blank(&name);
	small2capital(&name);

	//1.2:在directorybuffer里面搜索
	QWORD p=readbuffer;
	for(;p<(readbuffer+0x40000);p+=0x40)
	{
		if( *(QWORD*)p==name ) break;
	}
	if(p==readbuffer+0x40000)
	{
		say("file not found,bye!\n");
		return;
	}

	//1.3:搜到了就取出首簇号
	QWORD cluster=*(QWORD*)(p+0x10);	//fat16,only 16bit


//2.从首簇开始，沿着fat的链表，慢慢挪，直到得到调用者要求的位置对应的簇号
	QWORD temp=0;
	while(1)
	{
		//就是这里，就从这个簇开始
		if(cluster==temp)break;
		
		//准备下一个地址，找下一个簇，全部fat表在内存里不用担心
		temp+=clustersize*0x200;
		cluster=(QWORD)(*(WORD*)(fatbuffer+2*cluster));
	}


//3.开始读
	fat16_data(readbuffer,cluster);
}
void fat16_root()
{
	//清理内存
	BYTE* memory=(BYTE*)(readbuffer);
	int i;
	for(i=0;i<0x40000;i++) memory[i]=0;

	//fat16的
	//文件分配表区最多0xffff个簇记录*每个记录占2个字节<=0x20000=0x100个扇区
	//而数据区最大0xffff个簇记录*每簇0x8000字节(?)<=0x80000000=2G=0x400000个扇区
	say("reading whole fat table\n");
	read(fatbuffer,fat0,diskaddr,0x100);

	say("cd %x\n",fat0+fatsize*2);
	read(readbuffer,fat0+fatsize*2,diskaddr,32);	//0x40000=0x20*0x200
	explaindirectory();

	say("\n");
}
static int fat16_cd(BYTE* addr)
{
	QWORD name=0;
	QWORD directory=0;
	QWORD p=directorybuffer;
	int i;

	//处理名字
	str2data(addr,&name);
	zero2blank(&name);
	small2capital(&name);
	say("name:%x\n",name);

	//是根目录或者首簇号为0就直接调用fat16_root()，否则得到首簇号完事
	if( (name&0xff) != 0x2f)
	{
		for(;p<directorybuffer+0x800;p+=0x40)
		{
			if( *(QWORD*)p == name )
			{
				if( *(BYTE*)(p+0x20)&0x10) break;
			}
		}
		if(p==directorybuffer+0x800)		//太大的目录会出问题
		{
			say("directory not found\n");
			return -1;
		}
		directory=*(QWORD*)(p+0x10); //fat16,only 16bit
	}
	else
	{
		fat16_root();
		return 0;
	}
	if(directory==0)
	{
		fat16_root();
		return 0;
	}

	//清理
	BYTE* memory=(BYTE*)(directorybuffer);
	for(i=0;i<0x40000;i++) memory[i]=0;

	//读取,转换
	fat16_data(readbuffer,directory);
	explaindirectory();

	return 1;
}








//fat32
//文件分配表区总共可以有0xffffffff个*每个簇记录占4个字节<=0x400000000=16G=0x2000000个扇区
//数据区总共0xffffffff个簇*每簇512k<=0x20000000000=2T=0x100000000个扇区
static QWORD cachecurrent;		//单位是扇区
void checkcacheforcluster(QWORD cluster)
{
	//0x40000=0x200个扇区=0x10000个簇记录
	//所以要读的扇区为：[cacheblock,cacheblock+0x1ff](cacheblock=fat0扇区+(cluster/0x10000)*0x200)
	//举例子：777在第0大块，13578在第1大块，每大块0x200个扇区
	//当前返回
	QWORD cacheblock=(cluster/0x10000);
	if(cachecurrent == cacheblock) return;


	//读，之后保存这块的编号
	QWORD i;
	for(i=0;i<0x40;i++)
	{
		read(fatbuffer+i*0x1000,fat0+cacheblock*0x200+8*i,diskaddr,8);
	}
	say("cacheblock:%x\n",cacheblock);
	cachecurrent=cacheblock;
}
//从收到的簇号开始一直读最多1MB，接收参数为目的内存地址，第一个簇号
void fat32_data(QWORD dest,QWORD cluster)		//destine,clusternum
{
	say("cluster:%x\n",cluster);

	QWORD rdi=dest;
	while(rdi<dest+0x100000)
	{
		read(rdi,cluster0+clustersize*cluster,diskaddr,clustersize);
		rdi+=clustersize*0x200;

		//检查缓冲，从检查完的缓冲区里面读一个cluster号
		checkcacheforcluster(cluster);
		cluster=(QWORD)(*(DWORD*)(fatbuffer+4*(cluster%0x10000)));

		if(cluster<2){say("impossible cluster,bye!%x\n",cluster);return;}
		if(cluster==0x0ffffff7){say("bad cluster,bye!%x\n",cluster);return;}
		if(cluster>=0x0ffffff8) break;
	}
	say("count:%x\n",rdi-dest);
	say("\n");
}
//接收参数：文件名字符串，调用者要的文件内部偏移（以1M为单元）
static void fat32_load(BYTE* addr,QWORD offset)
{
//1:文件名转换成首簇号
	//1.1:把字符串转换成fat16喜欢的格式
	QWORD name=0;
	str2data(addr,&name);
	zero2blank(&name);
	small2capital(&name);

	//1.2:在directorybuffer里面搜索
	QWORD p=readbuffer;
	for(;p<readbuffer+clustersize*0x200;p+=0x40)
	{
		if( *(QWORD*)p== name) break;
	}
	if(p==readbuffer+clustersize*0x200)
	{
		say("file not found,bye!\n");
		return;
	}

	//1.3:搜到了就取出首簇号
	QWORD cluster=*(QWORD*)(p+0x10);


//2.从首簇开始，沿着fat的链表，慢慢挪，直到得到调用者要求的位置对应的簇号
	//cluster=???


//3.开始读
	fat32_data(readbuffer,cluster);
}
void fat32_root()
{
	BYTE* memory=(BYTE*)(readbuffer);
	int i;
	for(i=0;i<0x40000;i++) memory[i]=0;

	cachecurrent=0xffffffff;
	checkcacheforcluster(0);

	say("cd root:%x\n",cluster0+clustersize*2);
	read(readbuffer,cluster0+clustersize*2,diskaddr,32);
	explaindirectory();
}
static int fat32_cd(BYTE* addr)
{
//1:文件名转换成首簇号
	//1.1:把字符串转换成fat16喜欢的格式
	QWORD name=0;
	QWORD directory=0;
	QWORD p=directorybuffer;
	int i;
	str2data(addr,&name);
	zero2blank(&name);
	small2capital(&name);

	//搜索
	if((name&0xff) != 0x2f)
	{
		for(;p<directorybuffer+clustersize*0x200;p+=0x40)
		{
		    if( *(QWORD*)p== name)
		    {
				if( *(BYTE*)(p+0x20)&0x10) break;
		    }
		}
		if(p==directorybuffer+clustersize*0x200)
		{
			say("directory not found!%x\n",p);
			return -1;
		}
		directory=*(QWORD*)(p+0x10); //high 16bit
	}
	else
	{
		fat32_root();
		return 0;
	}
	if(directory==0)
	{
		fat32_root();
		return 0;
	}

	//清理
	BYTE* memory=(BYTE*)(directorybuffer);
	for(i=0;i<0x40000;i++) memory[i]=0;

	//读取，转换
	fat32_data(readbuffer,directory);
	explaindirectory();

	return 1;
}








int mountfat(QWORD firstsector,QWORD* cdfunc,QWORD* loadfunc)
{
	//准备好可用的内存地址
	getaddrofbuffer(&readbuffer);
	getaddrofdir(&directorybuffer);
	getaddroffs(&fatbuffer);

	//读取pbr
	//say("partition sector:%x\n",firstsector);
	//diskaddr=*(QWORD*)(0x200000+8);
	read(readbuffer,firstsector,diskaddr,1); //pbr

	//检查分区问题，有就滚
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

	//检查fat版本
	int similarity=50;
	if( *(WORD*)(readbuffer+0x11) == 0) similarity++;         //fat32为0
	else similarity--;
	if( *(WORD*)(readbuffer+0x16) ==0) similarity++;         //fat32为0
	else similarity--;
	if(similarity==48)		//这是fat16
	{
		//返回cd和load函数的地址给老大master用
		say("fat16\n");
		*cdfunc=(QWORD)fat16_cd;
		*loadfunc=(QWORD)fat16_load;

		//准备本程序需要的变量
		//QWORD firstsector=(QWORD)( *(DWORD*)(readbuffer+0x1c) );
		fatsize=(QWORD)( *(WORD*)(readbuffer+0x16) );
		say("fatsize:%x\n",fatsize);
		fat0=firstsector + (QWORD)( *(WORD*)(readbuffer+0xe) );
		say("fat0:%x\n",fat0);
		clustersize=(QWORD)( *(BYTE*)(readbuffer+0xd) );
		say("clustersize:%x\n",clustersize);
		cluster0=fat0+fatsize*2+32-clustersize*2;
		say("cluster0:%x\n",cluster0);
		say("\n");

		//change directory /
		fat16_cd("/");
	}
	else if(similarity==52)		//这是fat16
	{
		//返回cd和load函数的地址给老大master用
		say("fat32\n");
		*cdfunc=(QWORD)fat32_cd;
		*loadfunc=(QWORD)fat32_load;

		//准备本程序需要的变量
		//QWORD firstsector=(QWORD)( *(DWORD*)(readbuffer+0x1c) );
		fatsize=(QWORD)( *(DWORD*)(readbuffer+0x24) );
		say("fatsize:%x\n",fatsize);
		fat0=firstsector + (QWORD)( *(WORD*)(readbuffer+0xe) );
		say("fat0:%x\n",fat0);
		clustersize=(QWORD)( *(BYTE*)(readbuffer+0xd) );
		say("clustersize:%x\n",clustersize);
		cluster0=fat0+fatsize*2-clustersize*2;
		say("cluster0:%x\n",cluster0);
		say("\n");

		//change directory /
		fat32_cd("/");
	}
	else
	{
		say("seem not fatxx,bye!\n");
		return -3;
	}

	return 0;
}

