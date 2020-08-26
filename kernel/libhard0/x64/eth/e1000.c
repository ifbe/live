typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
#define i825xx_REG_CTRL   0x0000
#define i825xx_REG_STATUS 0x0008
#define i825xx_REG_EECD   0x0010
#define i825xx_REG_EERD   0x0014

#define i825xx_REG_MDIC   0x0020

#define i825xx_REG_IMS    0x00D0
#define i825xx_REG_RCTL   0x0100
#define i825xx_REG_TCTL   0x0400

#define i825xx_REG_RDBAL  0x2800
#define i825xx_REG_RDBAH  0x2804
#define i825xx_REG_RDLEN  0x2808
#define i825xx_REG_RDH    0x2810
#define i825xx_REG_RDT    0x2818

#define i825xx_REG_TDBAL  0x3800
#define i825xx_REG_TDBAH  0x3804
#define i825xx_REG_TDLEN  0x3808
#define i825xx_REG_TDH    0x3810
#define i825xx_REG_TDT    0x3818

#define i825xx_REG_MTA    0x5200

#define i825xx_REG_RAL    0x5400
#define i825xx_REG_RAH    0x5404

// PHY REGISTERS (for use with the MDI/O Interface)
#define i825xx_PHYREG_PCTRL   0
#define i825xx_PHYREG_PSTATUS 1
#define i825xx_PHYREG_PSSTAT  17
//
u32 in32(u16);
void out32(u16, u32);
//
void printmemory(void*, ...);
void say(char*, ...);




static u8* mmioaddr = 0;
static u8 macaddr[8] = {};
//
static u8 rxdesc[16*32];
static u8 txdesc[16*32];
static u8 rxbuffer[0x800*32];
static u8 txbuffer[0x800*32];




struct RecvDesc
{   
    volatile u64 addr;
    volatile u16 len;
    volatile u16 checksum;
    volatile u8 status;
    volatile u8 errors;
    volatile u16 special;
};
void e1000_read()
{
	if(0 == mmioaddr)return;

	//changed = tail+1
	u32 tail = *(u32*)(mmioaddr+0x2818);
	tail = (tail+1)%32;

	//desc = changed
	u64 desc = (u64)rxdesc + 0x10*tail;
	u8 status = *(u8*)(desc+12);
	if(0 == (status&0x1))return;
	*(u8*)(desc+12) = 0;			//status写0
//say("@e1000_recv\n");

	//recv
	u8* buf = *(u8**)(desc);
	int len = *(u16*)(desc+8);
	printmemory(buf, len);

	//next
	volatile u32 dummy = *(u32*)(mmioaddr+0xc0);	//必须读，防止被优化
	*(u32*)(mmioaddr+0x2818) = tail;
}




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
void e1000_write(u8* buf, int len)
{
	//error return
	if(0 == mmioaddr)return;

	//txdesc
	u32 head = *(u32*)(mmioaddr+0x3810);
	u64 desc = (u64)txdesc+0x10*head;

	//put data
	int j;
	u8* dst = (u8*)( *(u64*)desc );
	for(j=0;j<len;j++)dst[j] = buf[j];
	for(j=0;j<6;j++)dst[j+6] = macaddr[j];

	//fill desc: reportstatus, insert fcs/crc, end of packet
	*(u16*)(desc+8) = len;
	*(u8*)(desc+11) = (1<<3) | (1<<1) | 1;

	//new tail
	u32 tail = *(u32*)(mmioaddr+0x3818);
	*(u32*)(mmioaddr+0x3818) = (tail+1)%8;

	//check send
	u64 timeout=0;
	while(1)
	{
		timeout++;
		if(timeout>0xfffffff)
		{
			say("fail",0);
			return;
		}

		u8 status=*(u8*)(desc+12);
		if( (status&0x1) == 0x1 )
		{
			say("sent@%x\n",desc);
			return;
		}
	}
}




u16 eepromread(u8* mmio, u32 reg)
{
	u32* p = (u32*)(mmio+0x14);
	*p = (reg<<8) | 0x1;		//start
	while(1)
	{
		if( (p[0]&0x10) == 0x10 ) return (u16)(p[0]>>8);
	}
}
void e1000_mmioinit(u8* mmio)
{
	mmioaddr = mmio;
	say("e1000@mmio:%llx{\n", mmio);

	//---------------------macaddr-------------------
	u64 lo, hi;
	lo = *(u32*)(mmio+0x5400);	//receive address low
	if(lo){
		hi = *(u32*)(mmio+0x5404);
		say("lo=%x,hi=%x\n", lo, hi);
		*(u64*)macaddr = ((hi&0xffff)<<32) + lo;
	}
	else{
		say("reading eeprom\n");
		u64 mac01 = eepromread(mmio, 0);
		u64 mac23 = eepromread(mmio, 1);
		u64 mac45 = eepromread(mmio, 2);
		*(u64*)macaddr = (mac45<<32) + (mac23<<16) + mac01;
	}
	say("macaddr=%llx\n", *(u64*)macaddr);
	//------------------------------------------




	//-----------------control-----------------
	//set link up
	u32 tmp = *(u32*)(mmio+0);
	*(u32*)(mmio+0) = tmp | (1<<6);

	//clear multicast table array
	int j;
	for(j=0;j<128;j++) *(u32*)(mmio+0x5200+4*j)=0;

	//enable interrupt
	*(u32*)(mmio+0xd0)=0x1f6dc;

	//read to clear "interrupt cause read"
	say("icr=%x\n",*(u32*)(mmio+0xc0));
	//---------------------------------------




	//--------------------receive descriptor---------------------
	for(j=0;j<32*16;j++){
		rxdesc[j] = 0;
	}
	for(j=0;j<32;j++){
		*(u64*)(rxdesc+0x10*j) = (u64)rxbuffer + 0x800*j;	//每个2048B
	}
	*(u32*)(mmio+0x2800) = ((u64)rxdesc) & 0xffffffff;	//addrlow
	*(u32*)(mmio+0x2804) = ((u64)rxdesc) >>        32;	//addrhigh
	*(u32*)(mmio+0x2808) =      32*16;	//32个*每个16B
	*(u32*)(mmio+0x2810) =          0;	//head
	*(u32*)(mmio+0x2818) =         31;	//tail
	*(u32*)(mmio+0x0100) = 0x0400801e;	//control
	//   0    4    0    0    8    0    1    e
	//0000 0100 0000 0000 1000 0000 0001 1110
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
	for(j=0;j<32*16;j++){
		txdesc[j] = 0;
	}
	for(j=0;j<8;j++)
	{
		*(u64*)(txdesc+0x10*j) = (u64)txbuffer+0x800*j;	//每个2048B
	}
	*(u32*)(mmio+0x3800) = ((u64)txdesc) & 0xffffffff;	//addrlow
	*(u32*)(mmio+0x3804) = ((u64)txdesc) >>        32;	//addrhigh
	*(u32*)(mmio+0x3808) =       8*16;	//8个*每个16B
	*(u32*)(mmio+0x3810) =          0;	//head
	*(u32*)(mmio+0x3818) =          0;	//tail
	*(u32*)(mmio+0x0400) = 0x010400fa;	//control
	//   0    1    0    4    0    0    f    a
	//0000 0001 0000 0100 0000 0000 1111 1010
	//TCTL_EN			1<<1
	//TCTL_PSP			1<<3
	//(15 << TCTL_CT_SHIFT)		4
	//(64 << TCTL_COLD_SHIFT)	12
	//TCTL_RTLC			1<<24
	//----------------------------------------------

	say("}\n");
}
void e1000_portinit(u64 addr)
{
	u32 temp;
	say("e1000@port:%x{\n",addr);

	out32(0xcf8, addr+0x4);
	temp = in32(0xcfc);
	temp |= 1<<2;
	temp &= ~(u32)(1<<10);
	out32(0xcf8, addr+0x4);
	out32(0xcfc, temp);

	out32(0xcf8, addr+0x4);
	say("sts,cmd=%x\n", in32(0xcfc));

	out32(0xcf8, addr+0x3c);
	temp = in32(0xcfc);
	say("int line=%x,pin=%x\n", temp&0xff, (temp>>8)&0xff);

	out32(0xcf8, addr+0x10);
	temp = in32(0xcfc)&0xfffffff0;

	say("}\n");

	e1000_mmioinit((void*)(u64)temp);
}
