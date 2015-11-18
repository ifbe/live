#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long

#define idehome 0x400000
#define ahcihome 0x400000
#define xhcihome 0x600000
#define usbhome 0x700000

#define diskhome 0x800000
#define fshome 0x900000
#define dirhome 0xa00000
#define datahome 0xb00000




typedef enum
{
	FIS_TYPE_REG_H2D	= 0x27,	// Register FIS - host to device
	FIS_TYPE_REG_D2H	= 0x34,	// Register FIS - device to host
	FIS_TYPE_DMA_ACT	= 0x39,	// DMA activate FIS - device to host
	FIS_TYPE_DMA_SETUP	= 0x41,	// DMA setup FIS - bidirectional
	FIS_TYPE_DATA		= 0x46,	// Data FIS - bidirectional
	FIS_TYPE_BIST		= 0x58,	// BIST activate FIS - bidirectional
	FIS_TYPE_PIO_SETUP	= 0x5F,	// PIO setup FIS - device to host
	FIS_TYPE_DEV_BITS	= 0xA1,	// Set device bits FIS - device to host
} FIS_TYPE;
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
	volatile DWORD prdbc;	// Physical region descriptor byte count transferred
 
	// DW2, 3
	DWORD	ctba;	// Command table descriptor base address
	DWORD	ctbau;	// Command table descriptor base address upper 32 bits
 
	// DW4 - 7
	DWORD	rsv1[4];	// Reserved
} HBA_CMD_HEADER;
typedef struct tagFIS_REG_H2D
{
	// DWORD 0
	BYTE	fis_type;	//0:		FIS_TYPE_REG_H2D
 
	BYTE	pmport:4;	//1.[7,4]:		Port multiplier
	BYTE	rsv0:3;		//1.[3,1]:		Reserved
	BYTE	c:1;		//1.[0,0]:		1: Command, 0: Control
 
	BYTE	command;	//2:		Command register
	BYTE	featurel;	//3:		Feature register, 7:0
 
	// DWORD 1
	BYTE	lba0;		//4:		LBA low register, 7:0
	BYTE	lba1;		//5:		LBA mid register, 15:8
	BYTE	lba2;		//6:		LBA high register, 23:16
	BYTE	device;		//7:		Device register
 
	// DWORD 2
	BYTE	lba3;		//8:		LBA register, 31:24
	BYTE	lba4;		//9:		LBA register, 39:32
	BYTE	lba5;		//a:		LBA register, 47:40
	BYTE	featureh;	//b:		Feature register, 15:8
 
	// DWORD 3
	BYTE	countl;		//c:		Count register, 7:0
	BYTE	counth;		//d:		Count register, 15:8
	BYTE	icc;		//e:		Isochronous command completion
	BYTE	control;	//f:		Control register
 
	// DWORD 4
	BYTE	rsv1[4];	//[10,13]:		Reserved
} FIS_REG_H2D;
typedef struct tagHBA_PRDT_ENTRY
{
	DWORD	dba;		// Data base address
	DWORD	dbau;		// Data base address upper 32 bits
	DWORD	rsv0;		// Reserved
 
	// DW3
	DWORD	dbc:22;		// Byte count, 4M max
	DWORD	rsv1:9;		// Reserved
	DWORD	i:1;		// Interrupt on completion
}HBA_PRDT_ENTRY;
typedef struct tagHBA_CMD_TBL
{
	// 0x00
	BYTE	cfis[64];	// Command FIS
 
	// 0x40
	BYTE	acmd[16];	// ATAPI command, 12 or 16 bytes
 
	// 0x50
	BYTE	rsv[48];	// Reserved
 
	// 0x80
	HBA_PRDT_ENTRY	prdt_entry[1];
	// Physical region descriptor table entries, 0 ~ 65535
	// 0xffff个，每个0x18,总共0x180000=1.5M
}CMD_TABLE;




void diary(char*,...);




int find_cmdslot(HBA_PORT *port)
{
	// If not set in SACT and CI, the slot is free
	int i;
	DWORD cmdslot = (port->sact | port->ci);
	for (i=0; i<32; i++)	//cmdslots=32
	{
		if ((cmdslot&1) == 0)
			return i;
		cmdslot >>= 1;
	}
	return -1;
}
void maketable(QWORD buf,QWORD from,HBA_CMD_HEADER* cmdheader,DWORD count)
{
	cmdheader->cfl=sizeof(FIS_REG_H2D)/sizeof(DWORD);//Command FIS size
	cmdheader->w = 0;		// Read from device
	cmdheader->prdtl = (WORD)((count-1)>>7) + 1;	//PRDT entries count,example:
													//13sectors    >>>>    prdt=1个
													//0x80sectors    >>>>    prdt=1个
													//0x93sectors    >>>>    prdt=2个
													//0x100sectors    >>>>    prdt=2个
													//0x173sectors    >>>>    prdt=3个
													//0x181sectors    >>>>    prdt=4个
	//diary("ptdtl",(QWORD)cmdheader->prdtl);




	CMD_TABLE* cmdtable = (CMD_TABLE*)(QWORD)cmdheader->ctba;
								//diary("cmdtable@",(QWORD)cmdtable);
	char* p=(char*)cmdtable;
	int i=sizeof(CMD_TABLE)+(cmdheader->prdtl-1)*sizeof(HBA_PRDT_ENTRY);
	for(;i>0;i--){p[i]=0;}
 



	// 8K bytes (16 sectors) per PRDT
	for (i=0; i<cmdheader->prdtl-1; i++)
	{
		cmdtable->prdt_entry[i].dba = (DWORD)buf;
		cmdtable->prdt_entry[i].dbc = 0x80*0x200-1;		 //bit0必须等于1 , ????0x1ffff
		//cmdtable->prdt_entry[i].i = 1;
		buf += 0x10000;  // 64KB words?
		count -= 0x80;    // 16 sectors
	}
	cmdtable->prdt_entry[i].dba = (DWORD)buf;		// Last entry
	cmdtable->prdt_entry[i].dbc = count*0x200-1;		// 512 bytes per sector
	//cmdtable->prdt_entry[i].i = 1;




	// Setup command
	FIS_REG_H2D* fis = (FIS_REG_H2D*)(&cmdtable->cfis);

	fis->fis_type = FIS_TYPE_REG_H2D;		//		0
	fis->c = 1;	// Command					//		[1].[0,0]
	//fis->command = 0x20;
	//fis->command = ATA_CMD_READ_DMA_EX;
	fis->command = 0x25;					//		2

	fis->lba0 = (BYTE)from;					//		4
	fis->lba1 = (BYTE)(from>>8);			//		5
	fis->lba2 = (BYTE)(from>>16);			//		6
	fis->device = 1<<6;						//		7		1<<6=LBA mode

	fis->lba3 = (BYTE)(from>>24);			//		8
	fis->lba4 = (BYTE)(from>>32);			//		9
	fis->lba5 = (BYTE)(from>>40);			//		a

	fis->countl = count&0xff;				//		c
	fis->counth = (count>>8)&0xff;			//		d
}
int readpart(QWORD buf,QWORD from,QWORD addr,DWORD count)
{
	HBA_PORT* port =(HBA_PORT*)addr;
	HBA_CMD_HEADER* cmdheader = (HBA_CMD_HEADER*)(QWORD)(port->clb);
	port->is = (DWORD)-1;		// Clear pending interrupt bits




	//find cmdslot
	int cmdslot = find_cmdslot(port);
	if (cmdslot == -1)
	{
		diary("error:no cmdslot",0);
		return -1;
	}
	cmdheader += cmdslot;
	//diary("cmdslot:",(QWORD)cmdslot);
	//diary("cmdheader:",(QWORD)cmdheader);




	//make the table
	maketable(buf,from,cmdheader,count);
	DWORD temp=0;
	volatile int timeout = 0;
	while(1)
	{
		timeout++;
		if(timeout>0xfffff)
		{
			diary("(timeout1)port->tfd:%x",(QWORD)port->tfd);
			return -11;
		}

		temp=port->tfd;							//0x20
		if( (temp&0x80) == 0x80 )continue;		//bit7,busy
		if( (temp&0x8) == 0x8)continue;			//bit3,DRQ
					//0x88=interface busy|data transfer requested

		break;
	}
	//diary("is:",(QWORD)port->is);
	//unsigned int* pointer=(unsigned int*)(QWORD)(port->fb);




	//issue
	port->ci = 1<<cmdslot;	// Issue command
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
		if (temp & 0x40000000)	// Task file error
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








static inline void out8(WORD port, BYTE val)
{
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}
static inline BYTE in8(WORD port)
{
    BYTE ret;
    asm volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );
    return ret;
}
static inline void out16(WORD port, WORD val)
{
    asm volatile ( "outw %0, %1" : : "a"(val), "Nd"(port) );
}
static inline WORD in16(WORD port)
{
    WORD ret;
    asm volatile ( "inw %1, %0" : "=a"(ret) : "Nd"(port) );
    return ret;
}
void readide(QWORD buf,QWORD from,QWORD notcare,DWORD count)
{
	QWORD command=*(QWORD*)(idehome+0x10);		//command
	QWORD control=*(QWORD*)(idehome+0x20);		//control
	char* dest=(char*)buf;
	volatile int i;
	volatile DWORD temp;

	//channel reset
	out8(control+2,4);
	out16(0x80,0x1111);		//out8(0xeb,0)
	diary("out %x %x\n",control+2,4);

	out8(control+2,0);
	diary("out %x %x\n",control+2,0);

	out8(command+6,0xa0);
	diary("out %x %x\n",command+6,0xa0);

	//50:hd exist
	i=0;
	while(1)
	{
		i++;
		if(i>0xfffff)
		{
			diary("device not ready:%x\n",temp);
			return;
		}

		temp=in8(command+7);
		if( (temp&0x80) == 0x80 )continue;			//busy
		if( (temp&0x40) == 0x40 )break;			//Device ReaDY?
	}

	//count
	out8(command+2,count&0xff);
	diary("total:%x\n",count);

	//lba
	out8(command+3,from&0xff);
	out8(command+4,(from>>8)&0xff);
	out8(command+5,(from>>16)&0xff);
	diary("lba:%x\n",from);

	//mode
	out8(command+6,0xe0);
	diary("out %x %x\n",command+6,0xe0);

	//start reading
	out8(command+7,0x20);
	diary("out %x %x\n",command+7,0x20);

	//check hd data ready?
	i=0;
	while(1)
	{
		i++;
		if(i>0xfffff)
		{
			diary("data not ready!\n");
			return;
		}

		temp=in8(command+7);
		if( (temp&0x80) == 0x80 )continue;			//busy
		if( (temp&0x8) == 0 )continue;			//DRQ

		break;
		//if( (temp&0x1) == 0x1 )
		//{
		//	diary("read error:%x\n",temp);
		//	return;
		//}
		//else break;
	}

	//read data
	diary("reading data\n");
	for(i=0;i<0x200;i+=2)
	{
		temp=in8(command+7);
		//if( (temp&1) == 1)break;
		if( (temp&0x8) != 8)break;

		temp=in16(command+0);
		dest[i]=temp&0xff;
		dest[i+1]=(temp>>8)&0xff;
	}

	if(i < 0x200) diary("not finished:%x\n",i);
}








int read(QWORD buf,QWORD from,QWORD notcare,DWORD count)
{
	//暂时管不了多硬盘,所以指定用寻找到的第一个
	QWORD type=*(QWORD*)diskhome;
	if(type == 0x61746173)
	{
		QWORD addr=*(QWORD*)(diskhome+8);
		QWORD i=0;
		while(count>0x80)
		{
			readpart(buf+i*0x10000,from+i*0x80,addr,0x80);		//0x80sectors=0x10000=64KB
			i++;
			count-=0x80;
		}
		readpart(buf+i*0x10000,from+i*0x80,addr,count);
	}
	else if(type == 0x656469)
	{
		readide(buf,from,0,count);
	}
	//todo:		U盘
}
