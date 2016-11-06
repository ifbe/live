#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long
#define idehome 0x400000




//用了别人的函数
u32 in32(u32 addr);
void out32(u32 port, u32 addr);
void diary(char* , ...);




static unsigned int searchpci()
{
	int bus,slot,func;
	u32 temp,data;
	for(bus=0;bus<256;bus++)
	{
		for(slot=0;slot<32;slot++)
		{
			for(func=0;func<8;func++)
			{
				temp = 0x80000000 | (bus<<16) | (slot<<11) + (func<<8) + 8;
				out32(0xcf8, temp);

				data = in32(0xcfc);
				if((data>>16) == 0x0101)
				{
					return temp & 0xffffff00;
				}
			}
		}
	}
	return 0;
}




static void rememberharddisk(u64 name,u64 command,u64 control)
{
	u64* p=(u64*)idehome;
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
static unsigned int probepci(u64 addr)
{
//?：pci地址
//出：?存地址
	unsigned int temp;
	unsigned int bar0,bar1;
	diary("(ide)pciaddr:%x{\n",addr);

	out32(0xcf8,addr+0x4);
	temp=in32(0xcfc)|(1<<10)|(1<<2);		//bus master=1

	out32(0xcf8,addr+0x4);
	out32(0xcfc,temp);

	out32(0xcf8,addr+0x4);
	temp=(u64)in32(0xcfc);
	diary("    pci sts&cmd:%x",temp);

	//ide port
	out32(0xcf8,addr+0x10);
	bar0=in32(0xcfc)&0xfffffffe;
	diary("    (command)bar0:%x\n",bar0);

	out32(0xcf8,addr+0x14);
	bar1=in32(0xcfc)&0xfffffffe;
	diary("    (control)bar1:%x\n",bar1);

	out32(0xcf8,addr+0x18);
	temp=in32(0xcfc)&0xfffffffe;
	diary("    bar2:%x\n",temp);

	out32(0xcf8,addr+0x1c);
	temp=in32(0xcfc)&0xfffffffe;
	diary("    bar3:%x\n",temp);

	//找到了，放到自己的表格里
	rememberharddisk(0x656469,bar0,bar1);
	diary("}\n");
}








void initide()
{
	u64 addr;
	diary("@initide");

	//find device
	addr = searchpci();		//get port addr of this storage device
	if(addr == 0) return;

	//probe pci
	addr = probepci(addr);		//memory addr of ahci
	if(addr == 0) return;
}
