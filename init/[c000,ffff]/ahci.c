#include "ahci.h"

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

	unsigned char* p=(unsigned char*)0xd800;
	for(;p<(unsigned char*)0x10000;p++)	*p=0;

	//32*32=0x400
	port->clb =0xd800;
	port->clbu = 0;
 
	//0x100
	port->fb = 0xdc00;
	port->fbu = 0;
 
	//0x100*32=0x2000=8k
	HBA_CMD_HEADER *cmdheader=(HBA_CMD_HEADER*)(QWORD)(port->clb);
	int i;
	for (i=0; i<32; i++)
	{
		cmdheader[i].prdtl = 8;	// 8 prdt entries per command table
		cmdheader[i].ctba=0xe000+(i<<8);
		cmdheader[i].ctbau = 0;
	}
	say("port->clb:",(QWORD)port->clb);
	say("port->fb:",(QWORD)port->fb);
	enable(port);	// Start command engine
/*
	say("port->is:",(QWORD)port->is);
	say("port->ie:",(QWORD)port->ie);
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
