#include "ahci.h"

unsigned int findahci()
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
	//say("pci status and command:",(QWORD)in32(0xcfc));

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
	//say("ahci cap and ghc:",((QWORD)(abar->cap)<<32)+(QWORD)(abar->ghc));

	QWORD i = 0;
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
			*(QWORD*)(0x2800+i*0x10)=addr+0x100+i*0x80;
			*(QWORD*)(0x2808+i*0x10)=*(QWORD*)"sata\0\0\0\0";
			return addr+0x100+0x80*i;

			case 0xeb140101:
			*(QWORD*)(0x2800+i*0x10)=addr+0x100+i*0x80;
			*(QWORD*)(0x2808+i*0x10)=*(QWORD*)"atapi\0\0\0";

			case 0xc33c0101:
			*(QWORD*)(0x2800+i*0x10)=addr+0x100+i*0x80;
			*(QWORD*)(0x2808+i*0x10)=*(QWORD*)("enclo...");

			case 0x96690101:
			*(QWORD*)(0x2800+i*0x10)=addr+0x100+i*0x80;
			*(QWORD*)(0x2808+i*0x10)=*(QWORD*)("multi...");
			}
			}
		}
		pi >>= 1;
		i ++;
	}
}




void enable(HBA_PORT *port)
{
	//say("port->cmd before enable:",(QWORD)(port->cmd));
	while (port->cmd & (1<<15));	//bit15
 
	port->cmd |= 1<<4;	//bit4,receive enable
	port->cmd |= 1; 	//bit0,start
	//say("port->cmd after enable:",(QWORD)(port->cmd));
}
void disable(HBA_PORT *port)
{
	//say("port->cmd before disable:",(QWORD)(port->cmd));
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

	//say("port->cmd after disable:",(QWORD)(port->cmd));
}
void probeport(unsigned int addr)
{
	HBA_PORT* port=(HBA_PORT*)(QWORD)addr;
	disable(port);	// Stop command engine

	char* p=(char*)0x98000;
	for(;p<(char*)0xa0000;p++)	*p=0;

	//32*32=0x400
	port->clb =0x98000;
	//*(DWORD*)0x7000=port->clb;
	port->clbu = 0;
 
	//0x100
	port->fb = 0x98400;
	//*(DWORD*)0x7008=port->fb;
	port->fbu = 0;
 
	//0x100*32=0x2000=8k
	HBA_CMD_HEADER *cmdheader=(HBA_CMD_HEADER*)(QWORD)(port->clb);
	int i;
	for (i=0; i<32; i++)
	{
		cmdheader[i].prdtl = 8;	// 8 prdt entries per command table
		cmdheader[i].ctba=0x98800+(i<<8);
		cmdheader[i].ctbau = 0;
	}
	//*(DWORD*)0x7010=cmdheader[0].ctba;
	//say("port->clb:",(QWORD)port->clb);
	//say("port->fb:",(QWORD)port->fb);
	enable(port);	// Start command engine
}




void initahci()
{
	unsigned int addr;
	addr=findahci();
	addr=probepci(addr);
	addr=probeahci(addr);
	probeport(addr);
}
