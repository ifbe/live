#include "ahci.h"

unsigned int findaddr()
{
	QWORD addr=0x5004;
	unsigned int temp;
	for(;addr<0x6000;addr+=0x10)
	{
		temp=*(unsigned int*)addr;
		temp&=0xffffff00;
		if(temp==0x01060100)
		{
			addr+=4;
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

	int i=0;
	unsigned long long* table=(unsigned long long*)0x5000;
	for(i=0;i<0x200;i+=2)
	{
		if(table[i]==0){
			table[i]=0x69636861;
			table[i+1]=addr;
			break;
		}
	}
	return addr;
}




void probeahci(unsigned int addr)
{
	HBA_MEM* abar=(HBA_MEM*)(QWORD)addr;
	abar->ghc|=0x80000000;
	abar->ghc&=0xfffffffd;
}




void savedisk(QWORD first,QWORD second)
{
	int i;
	QWORD* pointer=(QWORD*)0x4000;
	for(i=0;i<0x200;i+=2)
	{
		if( pointer[i] == 0 )
		{
			pointer[i]=first;
			pointer[i+1]=second;
			break;
		}
	}
}




QWORD getdisk()
{
	int i;
	QWORD* pointer=(QWORD*)0x4000;
	for(i=0;i<0x200;i+=2)
	{
		if( pointer[i] == 0x2020202061746173 )
		{
			return pointer[i+1];
		}
	}

	return 0;		//only when there is no sata
}




QWORD checkandsave(QWORD addr)
{
	HBA_MEM* abar=(HBA_MEM*)(QWORD)addr;

	DWORD pi = abar->pi;
	int count=0;
	while(pi & 1)
	{
		pi >>= 1;
		count++;
	}

	QWORD i = 0;
	for(i=0;i<count;i++)
	{
	switch(abar->ports[i].sig)
	{
		case 0x00000101:{		//sata
			savedisk(0x2020202061746173,addr+0x100+i*0x80);
			break;
		}
		case 0xeb140101:{		//atapi
			savedisk(0x2020206970617461,addr+0x100+i*0x80);
			break;
		}
		case 0xc33c0101:{		//enclosure....
			savedisk(0x2e2e2e6f6c636e65,addr+0x100+i*0x80);
			break;
		}
		case 0x96690101:{		//port multiplier
			savedisk(0x2e2e2e69746c756d,addr+0x100+i*0x80);
			break;
		}
	}
	}

	return getdisk();
}


unsigned int findport(unsigned int addr)
{
	unsigned int temp;
	HBA_MEM* abar=(HBA_MEM*)(QWORD)addr;

	//try to get port
	temp=checkandsave(addr);
	if(temp!=0){
		DWORD ssts = ( (HBA_PORT*)(QWORD)temp ) -> ssts;
		BYTE ipm = (ssts >> 8) & 0x0F;		//[8,11]
		BYTE det = ssts & 0x0F;			//[0,3]

		if( (ipm != 0) && (det != 0) )
		{return temp;}
	}
	say("failed to find port 1:",0);

	//something wrong,reset ports
	int i;
	char* p=(char*)0x100000;
	for(i=0;i<0x2000;i++) p[i]=0;

	DWORD pi = abar->pi;
	int count=0;
	while(pi & 1)
	{
		pi >>= 1;
		count++;
	}

	for(i=0;i<count;i++)
	{
		abar->ports[i].fb = 0x100000+i*0x100;
		abar->ports[i].fbu = 0;
		abar->ports[i].cmd |=0x10;
		abar->ports[i].cmd |=0x2;
	}

	//try again
	temp=checkandsave(addr);
	if(temp!=0){
		DWORD ssts = ( (HBA_PORT*)(QWORD)temp ) -> ssts;
		BYTE ipm = (ssts >> 8) & 0x0F;		//[8,11]
		BYTE det = ssts & 0x0F;			//[0,3]

		if( (ipm != 0) && (det != 0) )
		{return temp;}
	}
	say("failed to find port 2:",0);

	return 0;
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
	int i;
	char* p=(char*)0x108000;
	for(i=0;i<0x6000;i++) p[i]=0;

	HBA_PORT* port=(HBA_PORT*)(QWORD)addr;
	disable(port);	// Stop command engine

	//32*32=0x400
	port->clb =0x108000;
	port->clbu = 0;
 
	//0x100*32=0x2000=8k
	HBA_CMD_HEADER *cmdheader=(HBA_CMD_HEADER*)(QWORD)(port->clb);
	for (i=0; i<32; i++)
	{
		cmdheader[i].prdtl = 8;	// 8 prdt entries per command table
		cmdheader[i].ctba=0x110000+(i<<8);
		cmdheader[i].ctbau = 0;
	}
	enable(port);	// Start command engine
}


void initahci()
{
	unsigned int addr;

	addr=findaddr();		//port addr of port
	if(addr==0) return;

	addr=probepci(addr);		//memory addr of ahci
	if(addr==0) return;

	probeahci(addr);

	addr=findport(addr);		//memory addr of port
	if(addr==0) return;

	probeport(addr);
}
