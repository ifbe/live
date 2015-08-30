#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long

#define idehome 0x100000
#define ahcihome 0x100000
#define xhcihome 0x200000
#define usbhome 0x300000

#define diskhome 0x400000
#define fshome 0x500000
#define dirhome 0x600000
#define datahome 0x700000




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





//告诉编译器这俩函数长什么样
void say(char*,...);
void diary(char*,...);




int identifysata(QWORD rdi,QWORD sataport)
{
	HBA_PORT* port=(HBA_PORT*)sataport;
	HBA_CMD_HEADER* cmdheader = (HBA_CMD_HEADER*)(QWORD)(port->clb);
	port->is = (DWORD)-1;		// Clear pending interrupt bits
	DWORD cmdslot;
	DWORD temp;




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
					//diary("cmdslot:",(QWORD)cmdslot);
					//diary("cmdheader:",(QWORD)cmdheader);
	cmdheader->cfl=sizeof(FIS_REG_H2D)/sizeof(DWORD);//Command FIS size
	cmdheader->w = 0;		// Read from device
	cmdheader->prdtl=1;
	cmdheader->prdbc=0;




	//make the table
	CMD_TABLE* cmdtable = (CMD_TABLE*)(QWORD)cmdheader->ctba;
					//diary("cmdtable(comheader->ctba):",(QWORD)cmdtable);
	char* p=(char*)cmdtable;
	int i=sizeof(CMD_TABLE);
	for(;i>0;i--){p[i]=0;}
	cmdtable->prdt_entry[0].dba = rdi;
	cmdtable->prdt_entry[0].dbc = 0x200-1;		//bit0必须等于1 , 到底要-1还是+1?
	//cmdtable->prdt_entry[0].i = 1;




	//
	FIS_REG_H2D *fis = (FIS_REG_H2D*)(&cmdtable->cfis);

	fis->fis_type = FIS_TYPE_REG_H2D;		//		0
	fis->c = 1;								//		1		Command
	fis->command = 0xec;					//		2
	fis->device = 0;						//		7		LBA mode




	// The below loop waits until the port is no longer busy before issuing a new command
	volatile int timeout = 0;
	while(1)
	{
		timeout++;
		if(timeout>0xfffff)
		{
			diary("(timeout1)port->tfd:%x",(QWORD)port->tfd);
			return -1;
		}

									//0x88=interface busy|data transfer requested
		temp=port->tfd;								//0x20
		if( (temp&0x80) == 0x80 )continue;			//bit7,busy
		if( (temp&0x8) == 0x8)continue;		//bit3,DRQ

		break;
	}
	//diary("is:",(QWORD)port->is);
	//unsigned int* pointer=(unsigned int*)(QWORD)(port->fb);




	//set issue,wait for completion
	port->ci = 1<<cmdslot;	// Issue command,start reading
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
		if (temp & 0x40000000)	// Task file error
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
int identifyide(QWORD rdi)
{
	QWORD command=*(QWORD*)(idehome+0x10);		//command
	QWORD control=*(QWORD*)(idehome+0x20);		//control
	WORD* dest=(WORD*)rdi;
	volatile int i;
	volatile DWORD temp=0;

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
			return -1;
		}

		temp=in8(command+7);
		if( (temp&0x80) == 0x80 )continue;			//busy
		if( (temp&0x40) == 0x40 )break;			//Device ReaDY?
	}

	//count
	out8(command+2,0);
	out8(command+3,0);
	out8(command+4,0);
	out8(command+5,0);

	//mode
	out8(command+6,0xe0);
	diary("out %x %x\n",command+6,0xe0);

	//start reading
	out8(command+7,0xec);
	diary("out %x %x\n",command+7,0xec);

	//check hd data ready?
	i=0;
	while(1)
	{
		i++;
		if(i>0xfffff)
		{
			diary("data not ready!\n");
			return -2;
		}

		temp=in8(command+7);
		if( (temp&0x80) == 0x80 )continue;			//busy
		if( (temp&0x8) == 0 )continue;			//DRQ

		break;
	}

	//read data
	diary("reading data\n");
	for(i=0;i<0x100;i++)
	{
		temp=in8(command+7);
		//if( (temp&1) == 1)break;
		if( (temp&0x8) != 8)break;

		temp=in16(command+0);		//只管放，后面再调字节序
		dest[i]=temp;
	}

	if(i < 0x100) diary("not finished:%x\n",i);
	return 1;
}








int identify(QWORD rdi)
{
	QWORD type=*(QWORD*)diskhome;
	QWORD temp=0;
	if(type == 0x61746173)					//sata
	{
		temp=*(QWORD*)(diskhome+8);
		identifysata(rdi,temp);
	}
	else if(type == 0x656469)				//ide
	{
		identifyide(rdi);
	}
	//todo:		U盘

	//SN，调转字节序
	char* p=(char*)datahome;
	int i;
	for(i=0x14;i<0x60;i+=2)
	{
		temp=p[i];
		p[i]=p[i+1];
		p[i+1]=temp;
	}
	say((char*)(datahome+20));

	//size
	temp=*(QWORD*)(datahome+200);
	say("%dGB(%xsectors)",temp>>21,temp);
	return temp;
}