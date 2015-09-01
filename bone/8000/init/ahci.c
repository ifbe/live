#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long

#define pcihome 0x60000
#define ahcihome 0x400000
#define receivefisbuffer ahcihome+0x10000
#define cmdlistbuffer ahcihome+0x20000
#define cmdtablebuffer ahcihome+0x30000




//用了别人的函数
void diary(char* , ...);



//主控就是这么做的，程序员没法改
typedef struct tagHBA_CMD_HEADER
{
	// DW0
	BYTE	cfl:5;	// Command FIS length in DWORDS, 2 ~ 16
	BYTE	a:1;	// ATAPI
	BYTE	w:1;	// Write, 1: H2D, 0: D2H
	BYTE	p:1;	// Prefetchable

	BYTE	r:1;	// Reset
	BYTE	b:1;	// BIST
	BYTE	c:1;	// Clear busy upon R_OK
	BYTE	rsv0:1;	// Reserved
	BYTE	pmp:4;	// Port multiplier port
 
	WORD	prdtl;	// Physical region descriptor table length in entries
 
	// DW1
	volatile
	DWORD	prdbc;	// Physical region descriptor byte count transferred
 
	// DW2, 3
	DWORD	ctba;	// Command table descriptor base address
	DWORD	ctbau;	// Command table descriptor base address upper 32 bits
 
	// DW4 - 7
	DWORD	rsv1[4];	// Reserved
} HBA_CMD_HEADER;

typedef volatile struct tagHBA_PORT
{
	DWORD	clb;	// 0x00, command list base address, 1K-byte aligned
	DWORD	clbu;	// 0x04, command list base address upper 32 bits
	DWORD	fb;	// 0x08, FIS base address, 256-byte aligned
	DWORD	fbu;	// 0x0C, FIS base address upper 32 bits
	DWORD	is;	// 0x10, interrupt status
	DWORD	ie;	// 0x14, interrupt enable
	DWORD	cmd;	// 0x18, command and status
	DWORD	rsv0;	// 0x1C, Reserved
	DWORD	tfd;	// 0x20, task file data
	DWORD	sig;	// 0x24, signature
	DWORD	ssts;	// 0x28, SATA status (SCR0:SStatus)
	DWORD	sctl;	// 0x2C, SATA control (SCR2:SControl)
	DWORD	serr;	// 0x30, SATA error (SCR1:SError)
	DWORD	sact;	// 0x34, SATA active (SCR3:SActive)
	DWORD	ci;	// 0x38, command issue
	DWORD	sntf;	// 0x3C, SATA notification (SCR4:SNotification)
	DWORD	fbs;	// 0x40, FIS-based switch control
	DWORD	rsv1[11];	// 0x44 ~ 0x6F, Reserved
	DWORD	vendor[4];	// 0x70 ~ 0x7F, vendor specific
} HBA_PORT;

typedef volatile struct tagHBA_MEM
{
	// 0x00 - 0x2B, Generic Host Control
	DWORD	cap;		// 0x00, Host capability
	DWORD	ghc;		// 0x04, Global host control
	DWORD	is;		// 0x08, Interrupt status
	DWORD	pi;		// 0x0C, Port implemented
	DWORD	vs;		// 0x10, Version
	DWORD	ccc_ctl;	// 0x14, Command completion coalescing control
	DWORD	ccc_pts;	// 0x18, Command completion coalescing ports
	DWORD	em_loc;		// 0x1C, Enclosure management location
	DWORD	em_ctl;		// 0x20, Enclosure management control
	DWORD	cap2;		// 0x24, Host capabilities extended
	DWORD	bohc;		// 0x28, BIOS/OS handoff control and status
 
	// 0x2C - 0x9F, Reserved
	BYTE	rsv[0xA0-0x2C];
 
	// 0xA0 - 0xFF, Vendor specific registers
	BYTE	vendor[0x100-0xA0];
 
	// 0x100 - 0x10FF, Port control registers
	HBA_PORT	ports[1];	// 1 ~ 32
} HBA_MEM;








//[0,7]:(vendorid<<16)+deviceid
//[8,0xf]:(class<<24)+(subclass<<16)+(progif<<8)+revisionid
//[0x10,0x17]:portaddress of the device
//[0x18,0x1f]:ansciiname of the device
static unsigned int finddevice()
{
	QWORD* addr=(QWORD*)(pcihome);
	int i;
	unsigned int temp;
	for(i=0;i<0x80;i++)				//每个0x40
	{
		temp=addr[8*i+1];
		temp&=0xffffff00;
		if(temp==0x01060100)
		{
			addr[8*i+3]=0x69636861;			//ahci

			return addr[8*i+2];		//1.返回要驱动的设备的portaddress
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
static unsigned int probepci(QWORD addr)
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
	diary("    pci sts&cmd:%x",(QWORD)in32(0xcfc));

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
			//diary("port->cmd before disable:%x",(QWORD)(port->cmd));
	port->cmd &= 0xfffffffe;	//0x18,bit0
	port->cmd &= 0xffffffef;	//0x18,bit4,FRE
 
	int timeout=100000;
	while(timeout)
	{
		timeout--;
		if(timeout==0)
		{
			diary("(timeout)still running:%x",(QWORD)(port->cmd));
			return;
		}

		if (port->cmd & (1<<14))continue;		//0x18,bit14,receive running
		if (port->cmd & (1<<15))continue;		//0x18,bit15,commandlist running

		break;
	}
			//diary("port->cmd after disable:%x",(QWORD)(port->cmd));




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
	//diary("port->cmd before enable:%x",(QWORD)(port->cmd));
	while (port->cmd & (1<<15));	//bit15
 
	port->cmd |= 1<<4;	//bit4,receive enable
	port->cmd |= 1; 	//bit0,start
	//diary("port->cmd after enable:%x",(QWORD)(port->cmd));
}
static void probeahci(QWORD addr)
{
	HBA_MEM* abar=(HBA_MEM*)addr;
	HBA_PORT* port;
	HBA_CMD_HEADER* cmdheader;

	QWORD temp;
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

		cmdheader=(HBA_CMD_HEADER*)(QWORD)(port->clb);
		for (j=0; j<32; j++)	//0x100*32=0x2000=8k
		{
			cmdheader[j].prdtl = 8;			//8 prdt entries per command table
			cmdheader[j].ctba=cmdtablebuffer + (i*0x10000) + (j<<8);		//0x10000/0x20=0x800,(0x800-0x80)/0x10=0x78个
			cmdheader[j].ctbau = 0;
		}




		enableport(port);	// Start command engine
	}




//第三步:list all known sata devices
	QWORD* mytable=(QWORD*)ahcihome;
	QWORD from = 0;
	QWORD to = 0;
	for(from=0;from<32*2+8;from++)		//清空目的地内存
	{
		mytable[from]=0;
	}

	to=0;		//翻译到自己的容易理解的表格里面(+0:名字,+8:地址)
	for(from=0;from<count;from++)
	{
		port=(HBA_PORT*)(addr+0x100+from*0x80);
		DWORD ssts = port -> ssts;			//0x28
		BYTE ipm = (ssts >> 8) & 0x0F;		//0x28.[8,11]
		BYTE det = ssts & 0x0F;			//0x28.[0,3]
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
	QWORD addr;

	//clear home
	addr=ahcihome;
	for(;addr<ahcihome+0x100000;addr++) *(BYTE*)addr=0;

	addr=finddevice();		//get port addr of this storage device
	if(addr==0) return;

	addr=probepci(addr);		//memory addr of ahci
	if(addr==0) return;

	probeahci(addr);
}
