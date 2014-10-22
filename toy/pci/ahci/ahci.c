#include "ahci.h"
#define ahcihome 0x200000
#define receivefisbuffer ahcihome+0x10000
#define cmdlistbuffer ahcihome+0x20000
#define cmdtablebuffer ahcihome+0x30000


//全部设备信息我放在0x140000了，格式如下:(非本环境自己处理这一步)
//[0,7]:(vendorid<<16)+deviceid
//[8,0xf]:(class<<24)+(subclass<<16)+(progif<<8)+revisionid
//[0x10,0x17]:portaddress of the device
//[0x18,0x1f]:ansciiname of the device
//本函数作用是：
//1.返回要驱动的设备的portaddress
//2.填上[0x18,0x1f],(为了工整好看)
unsigned int findaddr()
{
	QWORD* addr=(QWORD*)0x140000;
	int i;
	unsigned int temp;
	for(i=0;i<0x80;i++)		//每个0x40
	{
		temp=addr[8*i+1];
		temp&=0xffffff00;
		if(temp==0x01060100)
		{
			temp=addr[8*i+2];
			addr[8*i+3]=0x69636861;

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


//进：pci地址
//出：内存地址
unsigned int probepci(QWORD addr)
{
	say("ahci(port)@",addr);

	out32(0xcf8,addr+0x4);
	unsigned int temp=in32(0xcfc)|(1<<10)|(1<<2);
	out32(0xcf8,addr+0x4);
	out32(0xcfc,temp);

	out32(0xcf8,addr+0x4);
	say("pci status and command:",(QWORD)in32(0xcfc));

	out32(0xcf8,addr+0x24);
	addr=in32(0xcfc)&0xfffffff0;

	return addr;
}




void probeahci(QWORD addr)
{
	say("ahci@",addr);

	HBA_MEM* abar=(HBA_MEM*)addr;
	abar->ghc|=0x80000000;
	abar->ghc&=0xfffffffd;
}


//进：ahci内存地址
//出：找到就返回第一个sata地址，否则0
QWORD checkandsave(QWORD addr)
{
	HBA_MEM* abar=(HBA_MEM*)addr;

	DWORD pi = abar->pi;
	int count=0;
	while(pi & 1)
	{
		pi >>= 1;
		count++;
	}

	QWORD memory=ahcihome;
	QWORD i = 0;
	for(i=0;i<count;i++)
	{
	switch(abar->ports[i].sig)
	{
		case 0x00000101:{		//sata
			*(QWORD*)memory=0x61746173;
			*(QWORD*)(memory+8)=addr+0x100+i*0x80;
			memory+=0x10;
			break;			//继续for循环
		}
		case 0xeb140101:{		//atapi
			*(QWORD*)memory=0x6970617461;
			*(QWORD*)(memory+8)=addr+0x100+i*0x80;
			memory+=0x10;
			break;
		}
		case 0xc33c0101:{		//enclosure....
			*(QWORD*)memory=0x2e2e2e6f6c636e65;
			*(QWORD*)(memory+8)=addr+0x100+i*0x80;
			memory+=0x10;
			break;
		}
		case 0x96690101:{		//port multiplier
			*(QWORD*)memory=0x2e2e2e69746c756d;
			*(QWORD*)(memory+8)=addr+0x100+i*0x80;
			memory+=0x10;
			break;
		}
	}
	}

	QWORD temp=*(QWORD*)(ahcihome+8);
	if(temp!=0) return temp;
	else return 0;
}


//进：ahci内存地址
//出：找到就返回第一个sata地址，否则0
//有些bios没初始化
unsigned int findport(QWORD addr)
{
	unsigned int temp;
	HBA_MEM* abar=(HBA_MEM*)addr;

	//try to get port
	temp=checkandsave(addr);
	if(temp!=0){
		DWORD ssts = ( (HBA_PORT*)(QWORD)temp ) -> ssts;
		BYTE ipm = (ssts >> 8) & 0x0F;		//[8,11]
		BYTE det = ssts & 0x0F;			//[0,3]

		if( (ipm != 0) && (det != 0) )
		{return temp;}
	}
	say("failed to find port(try1)",0);

	//something wrong,reset ports
	int i;
	char* p=(char*)(receivefisbuffer);
	for(i=0;i<0x10000;i++) p[i]=0;

	DWORD pi = abar->pi;
	int count=0;
	while(pi & 1)
	{
		pi >>= 1;
		count++;
	}

	for(i=0;i<count;i++)
	{
		//32个，每个拥有256B的buffer
		abar->ports[i].fb = receivefisbuffer+i*0x100;
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
	say("failed to find port(try2)",0);

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
void probeport(QWORD addr)
{
	char* p;
	int i;
	p=(char*)(cmdlistbuffer);
	for(i=0;i<0x10000;i++) p[i]=0;
	p=(char*)(cmdtablebuffer);
	for(i=0;i<0x10000;i++) p[i]=0;

	HBA_PORT* port=(HBA_PORT*)addr;
	disable(port);	// Stop command engine

	//32*32=0x400
	port->clb =cmdlistbuffer;
	port->clbu = 0;
 
	//0x100*32=0x2000=8k
	HBA_CMD_HEADER *cmdheader=(HBA_CMD_HEADER*)(QWORD)(port->clb);
	for (i=0; i<32; i++)
	{
		cmdheader[i].prdtl = 8;	// 8 prdt entries per command table
		cmdheader[i].ctba=cmdtablebuffer+(i<<8);
		cmdheader[i].ctbau = 0;
	}
	enable(port);	// Start command engine
}


void initahci()
{
	QWORD addr;

	//clear home
	addr=ahcihome;
	for(;addr<ahcihome+0x100000;addr++) *(BYTE*)addr=0;

	addr=findaddr();		//port addr of port
	if(addr==0) return;

	addr=probepci(addr);		//memory addr of ahci
	if(addr==0) return;

	probeahci(addr);

	addr=findport(addr);		//memory addr of port
	if(addr==0) return;

	probeport(addr);

	say("",0);
}
