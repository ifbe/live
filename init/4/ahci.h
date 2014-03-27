#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long

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

void anscii(int x,int y,char ch);
void say(char* p,QWORD z);
unsigned int information();
unsigned int probepci(unsigned int addr);
unsigned int probeahci(unsigned int addr);
void probeport(unsigned int addr);
