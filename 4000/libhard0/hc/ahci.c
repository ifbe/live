#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long
//
#define ahcihome 0x400000
#define receivefisbuffer ahcihome+0x10000
#define cmdlistbuffer ahcihome+0x20000
#define cmdtablebuffer ahcihome+0x30000




//用了别人的函数
u32 in32(u32 addr);
void out32(u32 port, u32 addr);
void diary(char* , ...);



//主控就是这么做的，程序员没法改
typedef struct tagHBA_CMD_HEADER
{
	// DW0
	u8	cfl:5;	// Command FIS length in DWORDS, 2 ~ 16
	u8	a:1;	// ATAPI
	u8	w:1;	// Write, 1: H2D, 0: D2H
	u8	p:1;	// Prefetchable

	u8	r:1;	// Reset
	u8	b:1;	// BIST
	u8	c:1;	// Clear busy upon R_OK
	u8	rsv0:1;	// Reserved
	u8	pmp:4;	// Port multiplier port
 
	u16 prdtl;	// Physical region descriptor table length in entries
 
	// DW1
	volatile u32 prdbc;	// Physical region descriptor byte count transferred
 
	// DW2, 3
	u32 ctba;	// Command table descriptor base address
	u32 ctbau;	// Command table descriptor base address upper 32 bits
 
	// DW4 - 7
	u32 rsv1[4];	// Reserved
} HBA_CMD_HEADER;

typedef volatile struct tagHBA_PORT
{
	u32	clb;	// 0x00, command list base address, 1K-byte aligned
	u32	clbu;	// 0x04, command list base address upper 32 bits
	u32	fb;	// 0x08, FIS base address, 256-byte aligned
	u32	fbu;	// 0x0C, FIS base address upper 32 bits
	u32	is;	// 0x10, interrupt status
	u32	ie;	// 0x14, interrupt enable
	u32	cmd;	// 0x18, command and status
	u32	rsv0;	// 0x1C, Reserved
	u32	tfd;	// 0x20, task file data
	u32	sig;	// 0x24, signature
	u32	ssts;	// 0x28, SATA status (SCR0:SStatus)
	u32	sctl;	// 0x2C, SATA control (SCR2:SControl)
	u32	serr;	// 0x30, SATA error (SCR1:SError)
	u32	sact;	// 0x34, SATA active (SCR3:SActive)
	u32	ci;	// 0x38, command issue
	u32	sntf;	// 0x3C, SATA notification (SCR4:SNotification)
	u32	fbs;	// 0x40, FIS-based switch control
	u32	rsv1[11];	// 0x44 ~ 0x6F, Reserved
	u32	vendor[4];	// 0x70 ~ 0x7F, vendor specific
} HBA_PORT;

typedef volatile struct tagHBA_MEM
{
	// 0x00 - 0x2B, Generic Host Control
	u32	cap;		// 0x00, Host capability
	u32	ghc;		// 0x04, Global host control
	u32	is;		// 0x08, Interrupt status
	u32	pi;		// 0x0C, Port implemented
	u32	vs;		// 0x10, Version
	u32	ccc_ctl;	// 0x14, Command completion coalescing control
	u32	ccc_pts;	// 0x18, Command completion coalescing ports
	u32	em_loc;		// 0x1C, Enclosure management location
	u32	em_ctl;		// 0x20, Enclosure management control
	u32	cap2;		// 0x24, Host capabilities extended
	u32	bohc;		// 0x28, BIOS/OS handoff control and status
 
	// 0x2C - 0x9F, Reserved
	u8	rsv[0xA0-0x2C];
 
	// 0xA0 - 0xFF, Vendor specific registers
	u8	vendor[0x100-0xA0];
 
	// 0x100 - 0x10FF, Port control registers
	HBA_PORT	ports[1];	// 1 ~ 32
} HBA_MEM;




//[0,7]:(vendorid<<16)+deviceid
//[8,0xf]:(class<<24)+(subclass<<16)+(progif<<8)+revisionid
//[0x10,0x17]:portaddress of the device
//[0x18,0x1f]:ansciiname of the device
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
				if((data>>8) == 0x010601)
				{
					return temp & 0xffffff00;
				}
			}
		}
	}
	return 0;
}
static unsigned int probepci(u64 addr)
{
//进：pci地址
//出：内存地址
	unsigned int temp;
	diary("(ahci)pciaddr:%x{\n",addr);

	out32(0xcf8,addr+0x4);
	temp=in32(0xcfc)|(1<<10)|(1<<2);		//bus master=1

	out32(0xcf8,addr+0x4);
	out32(0xcfc,temp);

	out32(0xcf8,addr+0x4);
	diary("    pci sts&cmd:%x",(u64)in32(0xcfc));

	//ide port
	out32(0xcf8,addr+0x10);
	diary("    bar0:%x\n" , in32(0xcfc)&0xfffffffe );
	out32(0xcf8,addr+0x14);
	diary("    bar1:%x\n" , in32(0xcfc)&0xfffffffe );
	out32(0xcf8,addr+0x18);
	diary("    bar2:%x\n" , in32(0xcfc)&0xfffffffe );
	out32(0xcf8,addr+0x1c);
	diary("    bar3:%x\n" , in32(0xcfc)&0xfffffffe );

	diary("}\n");

	//hba addr
	out32(0xcf8,addr+0x24);
	return in32(0xcfc)&0xfffffff0;
}








static void disableport(HBA_PORT* port)
{
	//step2:	clear status : 0x30,0x10,0x8
	port->serr = 0x07ff0f03;		//0x30
	port->is = 0xfd8000af;			//0x10




	//step3:	put port in idle mode
			//diary("port->cmd before disable:%x",(u64)(port->cmd));
	port->cmd &= 0xfffffffe;	//0x18,bit0
	port->cmd &= 0xffffffef;	//0x18,bit4,FRE
 
	int timeout=100000;
	while(timeout)
	{
		timeout--;
		if(timeout==0)
		{
			diary("(timeout)still running:%x",(u64)(port->cmd));
			return;
		}

		if (port->cmd & (1<<14))continue;		//0x18,bit14,receive running
		if (port->cmd & (1<<15))continue;		//0x18,bit15,commandlist running

		break;
	}
			//diary("port->cmd after disable:%x",(u64)(port->cmd));




	//step4:	reset port
	port->sctl |= 0x2;
	for(timeout=0;timeout<0xffffff;timeout++)asm("nop");				//wait 1ms(5ms)
	port->sctl &= 0xfffffffd;



	//step5:	wait for device detect and communication established
	timeout=100000;
	while(timeout)
	{
		timeout--;
		if(timeout==0)
		{
			//diary("no device:%x",11111);
			return;
		}

		if( (port->ssts & 0x3) == 0x3)break;
	}
	




	//step6:	clear port serial ata error register
	port->serr = 0x07ff0f03;		//0x30
}
static void enableport(HBA_PORT* port)
{
	//diary("port->cmd before enable:%x",(u64)(port->cmd));
	while (port->cmd & (1<<15));	//bit15
 
	port->cmd |= 1<<4;	//bit4,receive enable
	port->cmd |= 1; 	//bit0,start
	//diary("port->cmd after enable:%x",(u64)(port->cmd));
}
static void probeahci(u64 addr)
{
	HBA_MEM* abar=(HBA_MEM*)addr;
	HBA_PORT* port;
	HBA_CMD_HEADER* cmdheader;

	u64 temp;
	int i,j;
	int count=0;




//第一步:host controller settings
	diary("(ahci)memaddr:%x{\n",addr);
	abar->ghc|=0x80000000;
	abar->ghc&=0xfffffffd;
	abar->is = 0xffffffff;		//clear all

	temp=abar->pi;		//实际多少个port
	while(temp & 1)
	{
		temp >>= 1;
		count++;
	}
	diary("    total implemented=%x\n",count);




//第二步:32 ports settings
	for(i=0;i<count;i++)
	{
		port=(HBA_PORT*)(addr+0x100+i*0x80);
		disableport(port);	// Stop command engine




		port->fb = receivefisbuffer+i*0x100;		//每个rcvdfis=0x100
		port->fbu = 0;

		port->clb =cmdlistbuffer+i*0x400;		//每个header=(32*32)*32=0x400*32=0x8000
		port->clbu = 0;

		cmdheader=(HBA_CMD_HEADER*)(u64)(port->clb);
		for (j=0; j<32; j++)	//0x100*32=0x2000=8k
		{
			cmdheader[j].prdtl = 8;			//8 prdt entries per command table
			cmdheader[j].ctba=cmdtablebuffer + (i*0x10000) + (j<<8);		//0x10000/0x20=0x800,(0x800-0x80)/0x10=0x78个
			cmdheader[j].ctbau = 0;
		}




		enableport(port);	// Start command engine
	}




//第三步:list all known sata devices
	u64* mytable=(u64*)ahcihome;
	u64 from = 0;
	u64 to = 0;
	for(from=0;from<32*2+8;from++)		//清空目的地内存
	{
		mytable[from]=0;
	}

	to=0;		//翻译到自己的容易理解的表格里面(+0:名字,+8:地址)
	for(from=0;from<count;from++)
	{
		port=(HBA_PORT*)(addr+0x100+from*0x80);
		u32 ssts = port -> ssts;			//0x28
		u8 ipm = (ssts >> 8) & 0x0F;		//0x28.[8,11]
		u8 det = ssts & 0x0F;			//0x28.[0,3]
		if( (ipm != 0) && (det != 0) )
		{
		switch(port->sig)
		{
			case 0x00000101:{		//sata
				mytable[to*8]=0x61746173;
				mytable[to*8+1]=addr+0x100+from*0x80;
				to+=8;
				break;			//继续for循环
			}
			case 0xeb140101:{		//atapi
				mytable[to*8]=0x6970617461;
				mytable[to*8+1]=addr+0x100+from*0x80;
				to+=8;
				break;
			}
			case 0xc33c0101:{		//enclosure....
				mytable[to*8]=0x2e2e2e6f6c636e65;
				mytable[to*8+1]=addr+0x100+from*0x80;
				to+=8;
				break;
			}
			case 0x96690101:{		//port multiplier
				mytable[to*8]=0x2e2e2e69746c756d;
				mytable[to*8+1]=addr+0x100+from*0x80;
				to+=8;
				break;
			}
		}//switch
		}//if this port usable
	}//for

	diary("}\n");
}








void initahci()
{
	u64 addr;
	diary("@initahci");

	//clear home
	addr=ahcihome;
	for(;addr<ahcihome+0x100000;addr++) *(u8*)addr=0;

	addr=searchpci();		//get port addr of this storage device
	if(addr==0) return;

	addr=probepci(addr);		//memory addr of ahci
	if(addr==0) return;

	probeahci(addr);
}
