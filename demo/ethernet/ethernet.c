#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long

#define ethhome 0x500000
#define rxdesc ethhome+0x40000
#define txdesc ethhome+0x60000
#define rxbuffer ethhome+0x80000
#define txbuffer ethhome+0xc0000

static QWORD portaddr;
static QWORD mmio;




//全部设备信息我放在0x110000了，格式如下:(非本环境自己处理这一步)
//[0,7]:(vendorid<<16)+deviceid
//[8,0xf]:(class<<24)+(subclass<<16)+(progif<<8)+revisionid
//[0x10,0x17]:portaddress of the device
//[0x18,0x1f]:ansciiname of the device
//本函数作用是：
//1.返回要驱动的设备的portaddress
//2.填上[0x18,0x1f],(为了工整好看)
void findport()
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
			portaddr=addr[8*i+2];
			return;
		}
	}
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
void probepci()
{
	say("ethernet(port)@",portaddr);

	out32(0xcf8,portaddr+0x4);
	unsigned int temp=in32(0xcfc)|(1<<10)|(1<<2);
	out32(0xcf8,portaddr+0x4);
	out32(0xcfc,temp);

	out32(0xcf8,portaddr+0x4);
	say("pci status and command:",(QWORD)in32(0xcfc));

	out32(0xcf8,portaddr+0x10);
	mmio=in32(0xcfc)&0xfffffff0;
}




WORD eepromread(whichreg)
{
	DWORD* p=(DWORD*)(mmio+0x14);
	*p=(whichreg<<8)|0x1;		//start
	while(1)
	{
		if( (p[0]&0x10) == 0x10 ) return (WORD)(p[0]>>8);
	}
}
void ethernet()
{
	say("ethernet@",mmio);

	//---------------------macaddr-------------------
	QWORD macaddr;
	DWORD ral=*(DWORD*)(mmio+0x5400);	//receive address low
	if(ral)
	{
		DWORD rah=*(DWORD*)(mmio+0x5404);
		macaddr=((QWORD)(rah&0xffff)<<32)+ral;
	}
	else
	{
		say("read eeprom",0);
		WORD mac01=eepromread(0);
		WORD mac23=eepromread(1);
		WORD mac45=eepromread(2);
		macaddr=((QWORD)(mac45)<<32)+((QWORD)(mac23)<<16)+mac01;
	}
	say("macaddr:",macaddr);
	//------------------------------------------




	//-----------------control-----------------
	//set link up
	*(DWORD*)(mmio+0)=(*(DWORD*)(mmio+0))|(1<<6);

	//clear multicast table array
	QWORD i;
	for(i=0;i<128;i++) *(DWORD*)(mmio+0x5200+4*i)=0;

	//read to clear "interrupt cause read"
	say("icr:",*(DWORD*)(mmio+0xc0));
	//---------------------------------------




	//--------------------receive descriptor---------------------
	for(i=0;i<32;i++)
	{
		*(QWORD*)(rxdesc+0x10*i)=rxbuffer+0x800*i;	//每个2048B
	}
	*(DWORD*)(mmio+0x2800)=rxdesc;	//addrlow
	*(DWORD*)(mmio+0x2804)=0;	//addrhigh
	*(DWORD*)(mmio+0x2808)=32*16;	//32个*每个16B
	*(DWORD*)(mmio+0x2810)=0;	//head
	*(DWORD*)(mmio+0x2818)=31;	//tail
	*(DWORD*)(mmio+0x100)=0x400801e;//control
	//   4    0    0    8    0    1    e
	//0100 0000 0000 1000 0000 0001 1110
        //RCTL_EN		1<<1
        //RCTL_SBP		1<<2
        //RCTL_UPE		1<<3
        //RCTL_MPE		1<<4
        //RCTL_LBM_NONE		0<<6
       	//RCTL_RDMTS_HALF	0<<8
        //RCTL_BAM		1<<15
        //RCTL_SECRC		1<<26
        //RCTL_BSIZE_2048	0<<16
	//------------------------------------------




	//----------------transmit descriptor-----------------
	for(i=0;i<8;i++)
	{
		*(QWORD*)(txdesc+0x10*i)=txbuffer+0x800*i;	//每个2048B
	}
	*(DWORD*)(mmio+0x3800)=txdesc;	//addrlow
	*(DWORD*)(mmio+0x3804)=0;	//addrhigh
	*(DWORD*)(mmio+0x3808)=8*16;	//8个*每个16B
	*(DWORD*)(mmio+0x3810)=0;	//head
	*(DWORD*)(mmio+0x3818)=0;	//tail
	*(DWORD*)(mmio+0x400)=0x10400fa;//control
	//   1    0    4    0    0    f    a
	//0001 0000 0100 0000 0000 1111 1010
	//TCTL_EN			1<<1
	//TCTL_PSP			1<<3
	//(15 << TCTL_CT_SHIFT)		4
	//(64 << TCTL_COLD_SHIFT)	12
	//TCTL_RTLC			1<<24
	//----------------------------------------------
}


/*
struct RecvDesc
{   
    volatile u64 addr;
    volatile u16 len;
    volatile u16 checksum;
    volatile u8 status;
    volatile u8 errors;
    volatile u16 special;
};
struct TransDesc
{
    volatile u64 addr;
    volatile u16 len;
    volatile u8 cso;
    volatile u8 cmd;
    volatile u8 status;
    volatile u8 css;
    volatile u16 special;
};
struct EthHeader
{   
    EthAddr dst;
    EthAddr src;
    u16 etherType;
};
struct EthPacket
{
	EthHeader *hdr;
	u16 etherType;
	u16 hdrLen;
};
static BYTE packet[6+6+2+2+2];
static void sendpacket()
{
	//包错了就return

	//发送
	*(DWORD*)(mmio+0x3818)=0;
}
*/




void initethernet()
{
	//clear home
	QWORD addr=ethhome;
	for(;addr<ethhome+0x80000;addr++) *(BYTE*)addr=0;
	portaddr=0;
	mmio=0;

	findport();		//port addr of port
	if(portaddr==0) return;

	probepci();
	if(mmio==0) return;

	ethernet();

	say("",0);
}
