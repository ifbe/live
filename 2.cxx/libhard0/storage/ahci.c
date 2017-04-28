#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long
//
#define ahcihome 0x400000
#define receivefisbuffer ahcihome+0x10000
#define cmdlistbuffer ahcihome+0x20000
#define cmdtablebuffer ahcihome+0x30000




u32 in32(u32 addr);
void out32(u32 port, u32 addr);
void diary(char* , ...);



//
typedef enum
{
	FIS_TYPE_REG_H2D	= 0x27, // Register FIS - host to device
	FIS_TYPE_REG_D2H	= 0x34, // Register FIS - device to host
	FIS_TYPE_DMA_ACT	= 0x39, // DMA activate FIS - device to host
	FIS_TYPE_DMA_SETUP      = 0x41, // DMA setup FIS - bidirectional
	FIS_TYPE_DATA	   = 0x46, // Data FIS - bidirectional
	FIS_TYPE_BIST	   = 0x58, // BIST activate FIS - bidirectional
	FIS_TYPE_PIO_SETUP      = 0x5F, // PIO setup FIS - device to host
	FIS_TYPE_DEV_BITS       = 0xA1, // Set device bits FIS - device to host
}FIS_TYPE;
typedef struct tagFIS_REG_H2D
{
	// u32 0
	u8    fis_type;       //0:	    FIS_TYPE_REG_H2D

	u8    pmport:4;       //1.[7,4]:	      Port multiplier
	u8    rsv0:3;	 //1.[3,1]:	      Reserved
	u8    c:1;	    //1.[0,0]:	      1: Command, 0: Control

	u8    command;	//2:	    Command register
	u8    featurel;       //3:	    Feature register, 7:0

	// u32 1
	u8    lba0;	   //4:	    LBA low register, 7:0
	u8    lba1;	   //5:	    LBA mid register, 15:8
	u8    lba2;	   //6:	    LBA high register, 23:16
	u8    device;	 //7:	    Device register

	// u32 2
	u8    lba3;	   //8:	    LBA register, 31:24
	u8    lba4;	   //9:	    LBA register, 39:32
	u8    lba5;	   //a:	    LBA register, 47:40
	u8    featureh;       //b:	    Feature register, 15:8

	// u32 3
	u8    countl;	 //c:	    Count register, 7:0
	u8    counth;	 //d:	    Count register, 15:8
	u8    icc;	    //e:	    Isochronous command completion
	u8    control;	//f:	    Control register

	// u32 4
	u8    rsv1[4];	//[10,13]:	      Reserved
}FIS_REG_H2D;
typedef struct tagHBA_PRDT_ENTRY
{
	u32   dba;	    // Data base address
	u32   dbau;	   // Data base address upper 32 bits
	u32   rsv0;	   // Reserved

	// DW3
	u32   dbc:22;	 // Byte count, 4M max
	u32   rsv1:9;	 // Reserved
	u32   i:1;	    // Interrupt on completion
}HBA_PRDT_ENTRY;
typedef struct tagHBA_CMD_TBL
{
	// 0x00
	u8    cfis[64];       // Command FIS

	// 0x40
	u8    acmd[16];       // ATAPI command, 12 or 16 bytes

	// 0x50
	u8    rsv[48];	// Reserved

	// 0x80
	HBA_PRDT_ENTRY  prdt_entry[1];
	// Physical region descriptor table entries, 0 ~ 65535
	// 0xffff个，每个0x18,总共0x180000=1.5M
}CMD_TABLE;
typedef struct tagHBA_CMD_HEADER
{
	// DW0
	u8 cfl:5;	// Command FIS length in u32S, 2 ~ 16
	u8 a:1;		// ATAPI
	u8 w:1;		// Write, 1: H2D, 0: D2H
	u8 p:1;		// Prefetchable

	u8 r:1;		// Reset
	u8 b:1;		// BIST
	u8 c:1;		// Clear busy upon R_OK
	u8 rsv0:1;	// Reserved
	u8 pmp:4;	// Port multiplier port
 
	u16 prdtl;	// Physical region descriptor table length in entries
 
	// DW1
	volatile u32 prdbc;	// Physical region descriptor byte count transferred
 
	// DW2, 3
	u32 ctba;	// Command table descriptor base address
	u32 ctbau;	// Command table descriptor base address upper 32 bits
 
	// DW4 - 7
	u32 rsv1[4];	// Reserved
}HBA_CMD_HEADER;
typedef volatile struct tagHBA_PORT
{
	u32 clb;	// 0x00, command list base address, 1K-byte aligned
	u32 clbu;	// 0x04, command list base address upper 32 bits
	u32 fb;		// 0x08, FIS base address, 256-byte aligned
	u32 fbu;	// 0x0C, FIS base address upper 32 bits
	u32 is;		// 0x10, interrupt status
	u32 ie;		// 0x14, interrupt enable
	u32 cmd;	// 0x18, command and status
	u32 rsv0;	// 0x1C, Reserved
	u32 tfd;	// 0x20, task file data
	u32 sig;	// 0x24, signature
	u32 ssts;	// 0x28, SATA status (SCR0:SStatus)
	u32 sctl;	// 0x2C, SATA control (SCR2:SControl)
	u32 serr;	// 0x30, SATA error (SCR1:SError)
	u32 sact;	// 0x34, SATA active (SCR3:SActive)
	u32 ci;		// 0x38, command issue
	u32 sntf;	// 0x3C, SATA notification (SCR4:SNotification)
	u32 fbs;	// 0x40, FIS-based switch control
	u32 rsv1[11];	// 0x44 ~ 0x6F, Reserved
	u32 vendor[4];	// 0x70 ~ 0x7F, vendor specific
}HBA_PORT;
typedef volatile struct tagHBA_MEM
{
	// 0x00 - 0x2B, Generic Host Control
	u32 cap;	// 0x00, Host capability
	u32 ghc;	// 0x04, Global host control
	u32 is;		// 0x08, Interrupt status
	u32 pi;		// 0x0C, Port implemented
	u32 vs;		// 0x10, Version
	u32 ccc_ctl;	// 0x14, Command completion coalescing control
	u32 ccc_pts;	// 0x18, Command completion coalescing ports
	u32 em_loc;	// 0x1C, Enclosure management location
	u32 em_ctl;	// 0x20, Enclosure management control
	u32 cap2;	// 0x24, Host capabilities extended
	u32 bohc;	// 0x28, BIOS/OS handoff control and status
 
	// 0x2C - 0x9F, Reserved
	u8 rsv[0xA0-0x2C];
 
	// 0xA0 - 0xFF, Vendor specific registers
	u8 vendor[0x100-0xA0];
 
	// 0x100 - 0x10FF, Port control registers
	HBA_PORT ports[1];	//1 ~ 32
}HBA_MEM;




int find_cmdslot(HBA_PORT *port)
{
	// If not set in SACT and CI, the slot is free
	int i;
	u32 cmdslot = (port->sact | port->ci);
	for (i=0; i<32; i++)    //cmdslots=32
	{
		if ((cmdslot&1) == 0)
			return i;
		cmdslot >>= 1;
	}
	return -1;
}
void maketable(u64 buf,u64 from,HBA_CMD_HEADER* cmdheader,u32 count)
{
	cmdheader->cfl=sizeof(FIS_REG_H2D)/sizeof(u32);//Command FIS size
	cmdheader->w = 0;	       // Read from device
	cmdheader->prdtl = (u16)((count-1)>>7) + 1;    //PRDT entries count,example:
	//13sectors    >>>>    prdt=1个
	//0x80sectors    >>>>    prdt=1个
	//0x93sectors    >>>>    prdt=2个
	//0x100sectors    >>>>    prdt=2个
	//0x173sectors    >>>>    prdt=3个
	//0x181sectors    >>>>    prdt=4个
	//diary("ptdtl",(u64)cmdheader->prdtl);




	CMD_TABLE* cmdtable = (CMD_TABLE*)(u64)cmdheader->ctba;
	//diary("cmdtable@",(u64)cmdtable);
	char* p=(char*)cmdtable;
	int i=sizeof(CMD_TABLE)+(cmdheader->prdtl-1)*sizeof(HBA_PRDT_ENTRY);
	for(;i>0;i--){p[i]=0;}




	// 8K bytes (16 sectors) per PRDT
	for (i=0; i<cmdheader->prdtl-1; i++)
	{
		cmdtable->prdt_entry[i].dba = (u32)buf;
		cmdtable->prdt_entry[i].dbc = 0x80*0x200-1;      //bit0=1, 0x1ffff
		//cmdtable->prdt_entry[i].i = 1;
		buf += 0x10000;  // 64KB words?
		count -= 0x80;    // 16 sectors
	}
	cmdtable->prdt_entry[i].dba = (u32)buf;	       // Last entry
	cmdtable->prdt_entry[i].dbc = count*0x200-1;	    // 512 bytes per sector
	//cmdtable->prdt_entry[i].i = 1;

	// Setup command
	FIS_REG_H2D* fis = (FIS_REG_H2D*)(&cmdtable->cfis);
	fis->fis_type = FIS_TYPE_REG_H2D;	       //0
	fis->c = 1;     // Command		      //[1].[0,0]
	//fis->command = 0x20;
	//fis->command = ATA_CMD_READ_DMA_EX;
	fis->command = 0x25;			    //2

	fis->lba0 = (u8)from;			 //4
	fis->lba1 = (u8)(from>>8);		    //5
	fis->lba2 = (u8)(from>>16);		   //6
	fis->device = 1<<6;			     //7     1<<6=LBA mode

	fis->lba3 = (u8)(from>>24);		   //8
	fis->lba4 = (u8)(from>>32);		   //9
	fis->lba5 = (u8)(from>>40);		   //a

	fis->countl = count&0xff;		       //c
	fis->counth = (count>>8)&0xff;		  //d
}
int ahciread(u64 sata, u64 buf, u64 from, u64 count)
{
	HBA_PORT* port =(HBA_PORT*)sata;
	HBA_CMD_HEADER* cmdheader = (HBA_CMD_HEADER*)(u64)(port->clb);
	port->is = 0xffffffff;	   // Clear pending interrupt




	//find cmdslot
	int cmdslot = find_cmdslot(port);
	if (cmdslot == -1)
	{
		diary("error:no cmdslot",0);
		return -1;
	}
	cmdheader += cmdslot;
	//diary("cmdslot:",(u64)cmdslot);
	//diary("cmdheader:",(u64)cmdheader);




	//make the table
	maketable(buf,from,cmdheader,count);
	u32 temp=0;
	volatile int timeout = 0;
	while(1)
	{
		timeout++;
		if(timeout>0xfffff)
		{
			diary("(timeout1)port->tfd:%x",(u64)port->tfd);
			return -11;
		}

		temp=port->tfd;			 //0x20
		if( (temp&0x80) == 0x80 )continue;      //bit7,busy
		if( (temp&0x8) == 0x8)continue;	 //bit3,DRQ
		//0x88=interface busy|data transfer requested

		break;
	}
	//diary("is:",(u64)port->is);
	//unsigned int* pointer=(unsigned int*)(u64)(port->fb);

	//issue
	port->ci = 1<<cmdslot;	  //Issue command
	timeout=0;
	while (1)
	{
		timeout++;
		if(timeout>0xffffff)
		{
			diary("(timeout2)port->ci=%x",temp);
			return -22;
		}

		//
		temp=port->is;
		if (temp & 0x40000000)  //Task file error
		{
			diary("port error 1",0);
			return -33;
		}

		// in the PxIS port field as well (1 << 5)
		temp=port->ci;
		if ((temp & (1<<cmdslot)) != 0)continue;

		break;
	}
	return 0;
}
int ahciidentify(u64 dev, u64 rdi)
{
        HBA_PORT* port=(HBA_PORT*)dev;
        HBA_CMD_HEADER* cmdheader = (HBA_CMD_HEADER*)(u64)(port->clb);
        port->is = (u32)-1;             // Clear pending interrupt bits
        u32 cmdslot;
        u32 temp;




        //find a cmd slot
        temp=(port->sact | port->ci);
        for (cmdslot=0;cmdslot<32;cmdslot++)    //cmdslots=32
        {
                if ((temp&1) == 0)break;
                temp>>= 1;
        }
        if (cmdslot == 32)
        {
                diary("error:no cmdslot",0);
                return -1;
        }
        cmdheader += cmdslot;
        //diary("cmdslot:",(u64)cmdslot);
        //diary("cmdheader:",(u64)cmdheader);

        cmdheader->cfl=sizeof(FIS_REG_H2D)/sizeof(u32);//Command FIS size
        cmdheader->w = 0;               // Read from device
        cmdheader->prdtl=1;
        cmdheader->prdbc=0;




        //make the table
        CMD_TABLE* cmdtable = (CMD_TABLE*)(u64)cmdheader->ctba;
        //diary("cmdtable(comheader->ctba):",(u64)cmdtable);
        char* p=(char*)cmdtable;
        int i=sizeof(CMD_TABLE);
        for(;i>0;i--){p[i]=0;}
        cmdtable->prdt_entry[0].dba = rdi;
        cmdtable->prdt_entry[0].dbc = 0x200-1;          //bit0必须等于1 , 到底要-1还是+1?
        //cmdtable->prdt_entry[0].i = 1;
        FIS_REG_H2D *fis = (FIS_REG_H2D*)(&cmdtable->cfis);

        fis->fis_type = FIS_TYPE_REG_H2D;               //0
        fis->c = 1;                                     //1     Command
        fis->command = 0xec;                            //2
        fis->device = 0;                                //7     LBA mode




        //wait until the port is no longer busy
        volatile int timeout = 0;
        while(1)
        {
                timeout++;
                if(timeout>0xfffff)
                {
                        diary("(timeout1)port->tfd:%x",(u64)port->tfd);
                        return -1;
                }

                //0x88=interface busy|data transfer requested
                temp=port->tfd;         //0x20
                if( (temp&0x80) == 0x80 )continue;      //bit7,busy
                if( (temp&0x8) == 0x8)continue;         //bit3,DRQ

                break;
        }
        //diary("is:",(u64)port->is);
        //unsigned int* pointer=(unsigned int*)(u64)(port->fb);
        //set issue,wait for completion
        port->ci = 1<<cmdslot;  // Issue command,start reading
        timeout=0;
        while(1)
        {
                timeout++;
                if(timeout>0xfffff)
                {
                        diary("(timeout2)ci=%x,prdbc=%x",temp,cmdheader->prdbc);
                        return -2;
                }

                temp=port->is;
                if (temp & 0x40000000)  // Task file error
                {
                        diary("port error 1");
                        return -9;
                }

                // in the PxIS port field as well (1 << 5)
                temp=port->ci;
                if( (temp & (1<<cmdslot)) != 0) continue;

                break;
        }

        return 1;
}




static void disableport(HBA_PORT* port)
{
	//clear status : 0x30,0x10,0x8
	port->serr = 0x07ff0f03;		//0x30
	port->is = 0xfd8000af;			//0x10

	//put port in idle mode
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

		//0x18,bit14,receive running
		if (port->cmd & (1<<14))continue;

		//0x18,bit15,commandlist running
		if (port->cmd & (1<<15))continue;

		//
		break;
	}
	//diary("port->cmd after disable:%x",(u64)(port->cmd));

	//reset port
	port->sctl |= 0x2;
	for(timeout=0;timeout<0xffffff;timeout++)asm("nop");	//wait 1ms(5ms)
	port->sctl &= 0xfffffffd;

	//wait for device detect and communication established
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

	//clear port serial ata error register
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
		//disable
		port=(HBA_PORT*)(addr+0x100+i*0x80);
		disableport(port);	// Stop command engine

		//每个rcvdfis=0x100
		port->fb = receivefisbuffer+i*0x100;
		port->fbu = 0;

		//每个header=(32*32)*32=0x400*32=0x8000
		port->clb =cmdlistbuffer+i*0x400;
		port->clbu = 0;

		//
		cmdheader=(HBA_CMD_HEADER*)(u64)(port->clb);
		for(j=0;j<32;j++)	//0x100*32=0x2000=8k
		{
			//8 prdt entries per command table
			cmdheader[j].prdtl = 8;

			//0x10000/0x20=0x800,(0x800-0x80)/0x10=0x78个
			cmdheader[j].ctba=cmdtablebuffer + (i*0x10000) + (j<<8);

			//0
			cmdheader[j].ctbau = 0;
		}

		//enable
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
		u32 ssts = port -> ssts;		//0x28
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

				diary("sata@%x",addr+0x100+from*0x80);
				break;
			}
			case 0xeb140101:{		//atapi
				mytable[to*8]=0x6970617461;
				mytable[to*8+1]=addr+0x100+from*0x80;
				to+=8;

				diary("atapi@%x",addr+0x100+from*0x80);
				break;
			}
			case 0xc33c0101:{		//enclosure....
				mytable[to*8]=0x2e2e2e6f6c636e65;
				mytable[to*8+1]=addr+0x100+from*0x80;
				to+=8;

				diary("enclosure@%x",addr+0x100+from*0x80);
				break;
			}
			case 0x96690101:{		//port multiplier
				mytable[to*8]=0x2e2e2e69746c756d;
				mytable[to*8+1]=addr+0x100+from*0x80;
				to+=8;

				diary("multiplier@%x",addr+0x100+from*0x80);
				break;
			}
		}//switch
		}//if this port usable
	}//for

	diary("}\n");
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
void initahci(u64 pciaddr)
{
	u64 addr;
	diary("ahci@%x",pciaddr);

	//clear home
	addr=ahcihome;
	for(;addr<ahcihome+0x100000;addr++) *(u8*)addr=0;

	//
	addr=probepci(pciaddr);
	if(addr==0) return;

	//
	probeahci(addr);
}
