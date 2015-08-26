#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long

#define pcihome 0x40000

#define idehome 0x100000




//用了别人的函数
void say(char* , ...);




static unsigned int finddevice()
{
	QWORD* addr=(QWORD*)(pcihome);
	int i;
	unsigned int temp;
	for(i=0;i<0x80;i++)				//每?0x40
	{
		temp=addr[8*i+1];
		temp&=0xffff0000;
		if(temp==0x01010000)
		{
			addr[8*i+3]=0x656469;			//ide

			return addr[8*i+2];		//1.返回要??的??的portaddress
		}
	}
return 0;
}
static void rememberharddisk(QWORD name,QWORD command,QWORD control)
{
	QWORD* p=(QWORD*)idehome;
	int i;

	//最多0x1000字节,每0x40字节一个记录
	//所以最多挂满64个sata设备,正常人不会挂那么多sata设备吧?
	//多了直接覆盖......
	//(如果实在超过这个数量,其实有0x10000字节可以用)
	for( i=0; i<0x1000/8; i+=8 )
	{
		if( p[i] == 0 )break;
	}

	//已经找到了空地，放东西
	p[i]=name;
	p[i+2]=command;
	p[i+4]=control;
}








static inline void out32( unsigned short port, unsigned int val )
{
    asm volatile( "outl %0, %1" : : "a"(val), "Nd"(port) );
}
static inline unsigned int in32( unsigned short port )
{
    unsigned int ret;
    asm volatile( "inl %1, %0"  : "=a"(ret) : "Nd"(port) );
    return ret;
}
static unsigned int probepci(QWORD addr)
{
//?：pci地址
//出：?存地址
	unsigned int temp;
	unsigned int bar0,bar1;
	say("(ide)pciaddr:%x{\n",addr);

	out32(0xcf8,addr+0x4);
	temp=in32(0xcfc)|(1<<10)|(1<<2);		//bus master=1

	out32(0xcf8,addr+0x4);
	out32(0xcfc,temp);

	out32(0xcf8,addr+0x4);
	temp=(QWORD)in32(0xcfc);
	say("    pci sts&cmd:%x",temp);

	//ide port
	out32(0xcf8,addr+0x10);
	bar0=in32(0xcfc)&0xfffffffe;
	say("    (command)bar0:%x\n",bar0);

	out32(0xcf8,addr+0x14);
	bar1=in32(0xcfc)&0xfffffffe;
	say("    (control)bar1:%x\n",bar1);

	out32(0xcf8,addr+0x18);
	temp=in32(0xcfc)&0xfffffffe;
	say("    bar2:%x\n",temp);

	out32(0xcf8,addr+0x1c);
	temp=in32(0xcfc)&0xfffffffe;
	say("    bar3:%x\n",temp);

	//找到了，放到自己的表格里
	rememberharddisk(0x656469,bar0,bar1);
	say("}\n");
}








void initide()
{
	QWORD addr;

	//find device
	addr=finddevice();		//get port addr of this storage device
	if(addr==0) return;

	//probe pci
	addr=probepci(addr);		//memory addr of ahci
	if(addr==0) return;
}
