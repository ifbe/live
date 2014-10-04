#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long

#define ethhome 0x500000


//全部设备信息我放在0x110000了，格式如下:(非本环境自己处理这一步)
//[0,7]:(vendorid<<16)+deviceid
//[8,0xf]:(class<<24)+(subclass<<16)+(progif<<8)+revisionid
//[0x10,0x17]:portaddress of the device
//[0x18,0x1f]:ansciiname of the device
//本函数作用是：
//1.返回要驱动的设备的portaddress
//2.填上[0x18,0x1f],(为了工整好看)
unsigned int findaddr()
{
	QWORD* addr=(QWORD*)0x110000;
	int i;
	unsigned int temp;
	for(i=0;i<0x80;i++)		//每个0x40
	{
		temp=addr[8*i+1];
		temp&=0xffffff00;
		if(temp==0x02000000)
		{
			addr[8*i+3]=0x74656e7265687465;
			return addr[8*i+2];
		}
	}
return 0;
}




static inline void out32( unsigned short port, unsigned int val )
{
    asm volatile( "outl %0, %1"
                  : : "a"(val), "Nd"(port) );
}
static inline unsigned int in32( unsigned short port )
{
    unsigned int ret;
    asm volatile( "inl %1, %0"
                  : "=a"(ret) : "Nd"(port) );
    return ret;
}


//进：pci地址
//出：内存地址
unsigned int probepci(QWORD addr)
{
	say("ethernet(port)@",addr);

	out32(0xcf8,addr+0x4);
	unsigned int temp=in32(0xcfc)|(1<<10)|(1<<2);
	out32(0xcf8,addr+0x4);
	out32(0xcfc,temp);

	out32(0xcf8,addr+0x4);
	say("pci status and command:",(QWORD)in32(0xcfc));

	out32(0xcf8,addr+0x10);
	addr=in32(0xcfc)&0xfffffff0;

	return addr;
}


void ethernet(QWORD addr)
{
	say("ethernet@",addr);
}


void initahci()
{
	QWORD addr;

	//clear home
	addr=ethhome;
	for(;addr<ethhome+0x80000;addr++) *(BYTE*)addr=0;

	addr=findaddr();		//port addr of port
	if(addr==0) return;

	addr=probepci(addr);
	if(addr==0) return;

	ethernet(addr);

	say("",0);
}
