#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long

int where=0;

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

typedef struct tagFIS_REG_H2D
{
	// DWORD 0
	BYTE	fis_type;	// FIS_TYPE_REG_H2D
 
	BYTE	pmport:4;	// Port multiplier
	BYTE	rsv0:3;		// Reserved
	BYTE	c:1;		// 1: Command, 0: Control
 
	BYTE	command;	// Command register
	BYTE	featurel;	// Feature register, 7:0
 
	// DWORD 1
	BYTE	lba0;		// LBA low register, 7:0
	BYTE	lba1;		// LBA mid register, 15:8
	BYTE	lba2;		// LBA high register, 23:16
	BYTE	device;		// Device register
 
	// DWORD 2
	BYTE	lba3;		// LBA register, 31:24
	BYTE	lba4;		// LBA register, 39:32
	BYTE	lba5;		// LBA register, 47:40
	BYTE	featureh;	// Feature register, 15:8
 
	// DWORD 3
	BYTE	countl;		// Count register, 7:0
	BYTE	counth;		// Count register, 15:8
	BYTE	icc;		// Isochronous command completion
	BYTE	control;	// Control register
 
	// DWORD 4
	BYTE	rsv1[4];	// Reserved
} FIS_REG_H2D;

typedef struct tagFIS_REG_sdb
{
	// DWORD 0
	BYTE	fis_type;	// FIS_TYPE_REG_H2D
 
	BYTE	rcv:5;
	BYTE	r1:1;
	BYTE	i:1;
	BYTE	r2:1;

	BYTE	status1:3;
	BYTE	r3:1;
	BYTE	status2:3;
	BYTE	r4:1;

	BYTE	error;

	// DWORD 1
	DWORD	sactive;

} FIS_DEV_BITS;

typedef struct tagFIS_REG_D2H
{
	// DWORD 0
	BYTE	fis_type;    // FIS_TYPE_REG_D2H
 
	BYTE	pmport:4;    // Port multiplier
	BYTE	rsv0:2;      // Reserved
	BYTE	i:1;         // Interrupt bit
	BYTE	rsv1:1;      // Reserved
 
	BYTE	status;      // Status register
	BYTE	error;       // Error register
 
	// DWORD 1
	BYTE	lba0;        // LBA low register, 7:0
	BYTE	lba1;        // LBA mid register, 15:8
	BYTE	lba2;        // LBA high register, 23:16
	BYTE	device;      // Device register
 
	// DWORD 2
	BYTE	lba3;        // LBA register, 31:24
	BYTE	lba4;        // LBA register, 39:32
	BYTE	lba5;        // LBA register, 47:40
	BYTE	rsv2;        // Reserved
 
	// DWORD 3
	BYTE	countl;      // Count register, 7:0
	BYTE	counth;      // Count register, 15:8
	BYTE	rsv3[2];     // Reserved
 
	// DWORD 4
	BYTE	rsv4[4];     // Reserved
} FIS_REG_D2H;

typedef struct tagFIS_DATA
{
	// DWORD 0
	BYTE	fis_type;	// FIS_TYPE_DATA
 
	BYTE	pmport:4;	// Port multiplier
	BYTE	rsv0:4;		// Reserved
 
	BYTE	rsv1[2];	// Reserved
 
	// DWORD 1 ~ N
	DWORD	data[1];	// Payload
} FIS_DATA;

typedef struct tagFIS_PIO_SETUP
{
	// DWORD 0
	BYTE	fis_type;	// FIS_TYPE_PIO_SETUP
 
	BYTE	pmport:4;	// Port multiplier
	BYTE	rsv0:1;		// Reserved
	BYTE	d:1;		// Data transfer direction, 1 - device to host
	BYTE	i:1;		// Interrupt bit
	BYTE	rsv1:1;
 
	BYTE	status;		// Status register
	BYTE	error;		// Error register
 
	// DWORD 1
	BYTE	lba0;		// LBA low register, 7:0
	BYTE	lba1;		// LBA mid register, 15:8
	BYTE	lba2;		// LBA high register, 23:16
	BYTE	device;		// Device register
 
	// DWORD 2
	BYTE	lba3;		// LBA register, 31:24
	BYTE	lba4;		// LBA register, 39:32
	BYTE	lba5;		// LBA register, 47:40
	BYTE	rsv2;		// Reserved
 
	// DWORD 3
	BYTE	countl;		// Count register, 7:0
	BYTE	counth;		// Count register, 15:8
	BYTE	rsv3;		// Reserved
	BYTE	e_status;	// New value of status register
 
	// DWORD 4
	WORD	tc;		// Transfer count
	BYTE	rsv4[2];	// Reserved
} FIS_PIO_SETUP;

typedef struct tagFIS_DMA_SETUP
{
	// DWORD 0
	BYTE	fis_type;	// FIS_TYPE_DMA_SETUP
 
	BYTE	pmport:4;	// Port multiplier
	BYTE	rsv0:1;		// Reserved
	BYTE	d:1;		// Data transfer direction, 1 - device to host
	BYTE	i:1;		// Interrupt bit
	BYTE	a:1;            // Auto-activate. Specifies if DMA Activate FIS is needed
 
        BYTE    rsved[2];       // Reserved
 
	//DWORD 1&2
 
        QWORD   DMAbufferID;    // DMA Buffer Identifier
 
        //DWORD 3
        DWORD   rsvd;           //More reserved
 
        //DWORD 4
        DWORD   DMAbufOffset;   //Byte offset into buffer. First 2 bits must be 0
 
        //DWORD 5
        DWORD   TransferCount;  //Number of bytes to transfer. Bit 0 must be 0
 
        //DWORD 6
        DWORD   resvd;          //Reserved
 
} FIS_DMA_SETUP;

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
 

typedef volatile struct tagHBA_FIS
{
	// 0x00
	FIS_DMA_SETUP	dsfis;		// DMA Setup FIS
	BYTE		pad0[4];
 
	// 0x20
	FIS_PIO_SETUP	psfis;		// PIO Setup FIS
	BYTE		pad1[12];
 
	// 0x40
	FIS_REG_D2H	rfis;		// Register – Device to Host FIS
	BYTE		pad2[4];
 
	// 0x58
	FIS_DEV_BITS	sdbfis;		// Set Device Bit FIS
 
	// 0x60
	BYTE		ufis[64];
 
	// 0xA0
	BYTE		rsv[0x100-0xA0];
} HBA_FIS;

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

typedef struct tagHBA_PRDT_ENTRY
{
	DWORD	dba;		// Data base address
	DWORD	dbau;		// Data base address upper 32 bits
	DWORD	rsv0;		// Reserved
 
	// DW3
	DWORD	dbc:22;		// Byte count, 4M max
	DWORD	rsv1:9;		// Reserved
	DWORD	i:1;		// Interrupt on completion
} HBA_PRDT_ENTRY;

typedef struct tagHBA_CMD_TBL
{
	// 0x00
	BYTE	cfis[64];	// Command FIS
 
	// 0x40
	BYTE	acmd[16];	// ATAPI command, 12 or 16 bytes
 
	// 0x50
	BYTE	rsv[48];	// Reserved
 
	// 0x80
	HBA_PRDT_ENTRY	prdt_entry[1];	// Physical region descriptor table entries, 0 ~ 65535
}CMD_TABLE;








void point(int x,int y,int color)
{
    QWORD* video=(QWORD*)0x3028;
    QWORD base=*video;
    char* p=(char*)0x3019;
    char bpp=*p/8;

    int* address;

    address=(int*)(base+(y*1024+x)*bpp);
    *address=color;
}
void anscii(int x,int y,char ch)
{
    int i,j;
    QWORD source=0xb000;
    char temp;
    char* p;

    ch&=0x7f;
    source+=ch<<4;
    x=8*x;
    y=16*y;

    for(i=0;i<16;i++)
    {
        p=(char*)source;
        for(j=0;j<8;j++)
        {
            temp=*p;
            temp=temp<<j;
            temp&=0x80;
            if(temp!=0){point(j+x,i+y,0xffffffff);}
            else{point(j+x,i+y,0);}

        }
    source++;
    }
}
void say(char* p,QWORD z)
{
	int x=0;
	while(*p!='\0')
	{
		anscii(x,where,*p);
		p++;
		x++;
	}

        int i=0;
	char ch;
        for(i=15;i>=0;i--)
        {
        ch=(char)(z&0x0000000f);
        z=z>>4;
        anscii(32+i,where,ch);
        }

	where++;
	if(where==50)where=0;
}





 

unsigned int information()
{
	QWORD addr=0x5008;
	unsigned int temp;
	for(;addr<0x6000;addr+=0x10)
	{
		temp=*(unsigned int*)addr;
		temp&=0xffffff00;
		if(temp==0x01060100)
		{
			addr-=0x8;
			temp=*(unsigned int*)addr;
			say("pci address:",(QWORD)temp);
			return temp;
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
unsigned int probepci(unsigned int addr)
{
	out32(0xcf8,addr+0x4);
	unsigned int temp=in32(0xcfc)|(1<<10);
	out32(0xcf8,addr+0x4);
	out32(0xcfc,temp);

	out32(0xcf8,addr+0x4);
	say("pci status and command:",(QWORD)in32(0xcfc));

	out32(0xcf8,addr+0x24);
	addr=in32(0xcfc)&0xfffffff0;
	say("ahci address:",(QWORD)addr);
	return addr;
}








QWORD check_type(HBA_PORT *port)
{
	DWORD ssts = port->ssts;
 
	BYTE ipm = (ssts >> 8) & 0x0F;		//[8,11]
	if (ipm == 0)	return 0;	//not exist
	BYTE det = ssts & 0x0F;			//[0,3]
	if (det == 0)	return 0;	//not exist
	return (QWORD)(port->sig);
}
unsigned int probeahci(unsigned int addr)
{
	HBA_MEM* abar=(HBA_MEM*)(QWORD)addr;
	abar->ghc|=0x2;
	say("ahci cap and ghc:",((QWORD)(abar->cap)<<32)+(QWORD)(abar->ghc));

	int i = 0;
	DWORD pi = abar->pi;
	while (i<32)
	{
		if (pi & 1)
		{
			QWORD what=check_type(&abar->ports[i]);
			if(what!=0)
			{
			switch(what)
			{
			case 0x00000101:
			say("sata address:",(QWORD)(addr+0x100+i*0x80));
			return addr+0x100+0x80*i;

			case 0xeb140101:
			say("atapi address:",(QWORD)(addr+0x100+i*0x80));

			case 0xc33c0101:
			say("enclosure????:",(QWORD)i);

			case 0x96690101:
			say("portmultiplier:",(QWORD)i);
			}
			}
		}
		pi >>= 1;
		i ++;
	}
}








void enable(HBA_PORT *port)
{
	say("port->cmd before enable:",(QWORD)(port->cmd));
	while (port->cmd & (1<<15));	//bit15
 
	port->cmd |= 1<<4;	//bit4,receive enable
	port->cmd |= 1; 	//bit0,start
	say("port->cmd after enable:",(QWORD)(port->cmd));
}
void disable(HBA_PORT *port)
{
	say("port->cmd before disable:",(QWORD)(port->cmd));
	port->cmd &= 0xfffffffe;	//bit0
	port->cmd &= 0xffffffef;	//bit4,FRE
 
	int i=100000;
	while(i)
	{
		i--;
		if (port->cmd & (1<<14))	//bit14,receive running
			continue;
		if (port->cmd & (1<<15))	//bit15,commandlist running
			continue;
		break;
	}
 
	if(i==0){
	say("error:can't disable:",(QWORD)(port->cmd));
	return;
	}

	say("port->cmd after disable:",(QWORD)(port->cmd));
}
void probeport(unsigned int addr)
{
	HBA_PORT* port=(HBA_PORT*)(QWORD)addr;
	disable(port);	// Stop command engine

	unsigned char* p=(unsigned char*)0x200000;
	for(;p<(unsigned char*)0x300000;p++)	*p=0;

	//32*32 = 1K per port
	port->clb =0x200000;
	port->clbu = 0;
 
	//256 bytes per port
	port->fb = 0x200000 + (32<<10);
	port->fbu = 0;
 
	// Command table offset: 40K + 8K*portno
	// Command table size = 256*32 = 8K per port
	HBA_CMD_HEADER *cmdheader=(HBA_CMD_HEADER*)(QWORD)(port->clb);
	int i;
	for (i=0; i<32; i++)
	{
		cmdheader[i].prdtl = 8;	// 8 prdt entries per command table

		// Command table offset: 40K + 8K*portno + cmdheader_index*256
		cmdheader[i].ctba=0x200000+(40<<10)+(1<<13)+(i<<8);
		cmdheader[i].ctbau = 0;
	}
	enable(port);	// Start command engine
	say("port->clb:",(QWORD)port->clb);
	say("port->fb:",(QWORD)port->fb);
	say("port->is:",(QWORD)port->is);
	say("port->ie:",(QWORD)port->ie);
/*
	say("port->clb:",(QWORD)port->clb);
	say("port->fb:",(QWORD)port->fb);
	say("port->is:",(QWORD)port->is);
	say("port->ie:",(QWORD)port->ie);
	say("port->cmd:",(QWORD)port->cmd);
	say("port->tfd:",(QWORD)port->tfd);
	say("port->sig:",(QWORD)port->sig);
	say("port->ssts:",(QWORD)port->ssts);
	say("port->sctl:",(QWORD)port->sctl);
	say("port->serr:",(QWORD)port->serr);
	say("port->sact:",(QWORD)port->sact);
	say("port->ci:",(QWORD)port->ci);
	say("port->sntf:",(QWORD)port->sntf);
	say("port->fbs:",(QWORD)port->fbs);
*/
}








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
int identify(HBA_PORT *port)
{
	port->is = (DWORD)-1;		// Clear pending interrupt bits
	
	int cmdslot = find_cmdslot(port);
	if (cmdslot == -1){
		say("error:no cmdslot",(QWORD)cmdslot);
		return -1;
	}
	say("cmdslot:",(QWORD)cmdslot);

	HBA_CMD_HEADER* cmdheader = (HBA_CMD_HEADER*)(QWORD)(port->clb);
	cmdheader += cmdslot;
	say("cmdheader:",(QWORD)cmdheader);
	cmdheader->cfl=sizeof(FIS_REG_H2D)/sizeof(DWORD);//Command FIS size
	cmdheader->w = 0;		// Read from device

	CMD_TABLE* cmdtable = (CMD_TABLE*)(QWORD)cmdheader->ctba;
	say("cmdtable(comheader->ctba):",(QWORD)cmdtable);
	char* p=(char*)cmdtable;
	int i=sizeof(CMD_TABLE)+(cmdheader->prdtl-1)*sizeof(HBA_PRDT_ENTRY);
	say("i:",(QWORD)i);
	for(;i>0;i--){*p=0;p++;}
	cmdtable->prdt_entry[0].dba =0x400000;
	cmdtable->prdt_entry[0].dbc = 512;	// 512 bytes per sector
	//cmdtable->prdt_entry[0].i = 1;
 
	FIS_REG_H2D *fis = (FIS_REG_H2D*)(&cmdtable->cfis);
	fis->fis_type = FIS_TYPE_REG_H2D;
	fis->command = 0xec;
	fis->device=0;
	fis->c = 1;	// Command

	int spin = 0;
	//0x88=interface busy|data transfer requested
	while ((port->tfd & (0x88)) && spin < 1000000){
		spin++;
	}
	if (spin==1000000){
		say("error:port->tfd:",(QWORD)port->tfd);
	}
 
	port->ci = 1<<cmdslot;	// Issue command

	say("is:",(QWORD)port->is);
	unsigned int* pointer=(unsigned int*)(QWORD)(port->fb);
	say("fb+20(pio setup fis):",(QWORD)*(pointer+8));
	say("fb+24:",(QWORD)*(pointer+9));
	say("fb+28:",(QWORD)*(pointer+10));
	say("fb+2c:",(QWORD)*(pointer+11));
	say("fb+40(d2h register fis):",(QWORD)*(pointer+0x10));
	say("fb+44:",(QWORD)*(pointer+0x11));
	say("fb+48:",(QWORD)*(pointer+0x12));
	say("fb+4c:",(QWORD)*(pointer+0x13));

	while (1){
		// in the PxIS port field as well (1 << 5)
		if ((port->ci & (1<<cmdslot)) == 0) 
			break;
		if (port->is & 0x40000000)	// Task file error
		{
			say("port error 1",0);
			return 0;
		}
	}
	return 1;
}







void start()
{
	unsigned int addr;

	addr=information();
	addr=probepci(addr);
	addr=probeahci(addr);
	probeport(addr);

	identify((HBA_PORT*)(QWORD)addr);

	char* p=(char*)0x400000;
	int count=0;

	for(;count<0x80;count+=2){
	anscii(count,where,p[count+1]);
	anscii(count+1,where,p[count]);
	}
}
