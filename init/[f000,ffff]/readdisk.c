#include "readdisk.h"

void start()
{
	unsigned int addr;

	addr=information();
	addr=probepci(addr);
	addr=probeahci(addr);
	probeport(addr);

	read((HBA_PORT*)(QWORD)addr,0,8,0x400000);
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
    QWORD points=0xb000;
    char temp;
    char* p;

    points+=ch<<4;
    x=8*x;
    y=16*y;

    for(i=0;i<16;i++)
    {
        p=(char*)points;
        for(j=0;j<8;j++)
        {
            temp=*p;
            temp=temp<<j;
            temp&=0x80;
            if(temp!=0){point(j+x,i+y,0xffffffff);}
            else{point(j+x,i+y,0);}

        }
    points++;
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
void maketable(HBA_CMD_HEADER* cmdheader,QWORD start,DWORD count,QWORD buf)
{
	CMD_TABLE* cmdtable = (CMD_TABLE*)(QWORD)cmdheader->ctba;
	say("cmdtable(comheader->ctba):",(QWORD)cmdtable);

	char* p=(char*)cmdtable;
	int i=sizeof(CMD_TABLE)+(cmdheader->prdtl-1)*sizeof(HBA_PRDT_ENTRY);
	say("i:",(QWORD)i);
	for(;i>0;i--){*p=0;p++;}
 
	// 8K bytes (16 sectors) per PRDT
	for (i=0; i<cmdheader->prdtl-1; i++)
	{
		cmdtable->prdt_entry[i].dba = (DWORD)buf;
		cmdtable->prdt_entry[i].dbc = 8*1024;	// 8K bytes
		cmdtable->prdt_entry[i].i = 1;
		buf += 8192;	// 4K words
		count -= 16;	// 16 sectors
	}
	// Last entry
	cmdtable->prdt_entry[i].dba = (DWORD)buf;
	cmdtable->prdt_entry[i].dbc = count<<9;	// 512 bytes per sector
	//cmdtable->prdt_entry[i].i = 1;
 
	// Setup command
	FIS_REG_H2D *fis = (FIS_REG_H2D*)(&cmdtable->cfis);
 
	fis->fis_type = FIS_TYPE_REG_H2D;
	fis->c = 1;	// Command
	fis->command = 0x25;
	//fis->command = ATA_CMD_READ_DMA_EX;
 
	fis->lba0 = (BYTE)start;
	fis->lba1 = (BYTE)(start>>8);
	fis->lba2 = (BYTE)(start>>16);
	fis->device = 0x40;	// LBA mode
 
	fis->lba3 = (BYTE)(start>>24);
	fis->lba4 = (BYTE)(start>>32);
	fis->lba5 = (BYTE)(start>>40);
 
	fis->countl = count&0xff;
	fis->counth = (count>>8)&0xff;
}
int read(HBA_PORT *port,QWORD start,DWORD count,QWORD buf)
{
	port->is = (DWORD)-1;		// Clear pending interrupt bits
	
	int cmdslot = find_cmdslot(port);
	if (cmdslot == -1){
		say("error:no cmdslot",(QWORD)cmdslot);
		return 0;
	}
	say("cmdslot:",(QWORD)cmdslot);

	HBA_CMD_HEADER* cmdheader = (HBA_CMD_HEADER*)(QWORD)(port->clb);
	cmdheader += cmdslot;
	say("cmdheader:",(QWORD)cmdheader);

	cmdheader->cfl=sizeof(FIS_REG_H2D)/sizeof(DWORD);//Command FIS size
	cmdheader->w = 0;		// Read from device
	cmdheader->prdtl = (WORD)((count-1)>>4) + 1;	// PRDT entries count
	maketable(cmdheader,start,count,buf);

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
/*
	say("fb(dma setup fis):",(QWORD)*(pointer));
	say("fb+4:",(QWORD)*(pointer+1));
	say("fb+8:",(QWORD)*(pointer+2));
	say("fb+c:",(QWORD)*(pointer+3));
	say("fb+20(pio setup fis):",(QWORD)*(pointer+8));
	say("fb+24:",(QWORD)*(pointer+9));
	say("fb+28:",(QWORD)*(pointer+10));
	say("fb+2c:",(QWORD)*(pointer+11));
	say("fb+40(d2h register fis):",(QWORD)*(pointer+0x10));
	say("fb+44:",(QWORD)*(pointer+0x11));
	say("fb+48:",(QWORD)*(pointer+0x12));
	say("fb+4c:",(QWORD)*(pointer+0x13));
	say("fb+58(set device bit fis):",(QWORD)*(pointer+0x16));
	say("fb+5c:",(QWORD)*(pointer+0x17));
	say("fb+60(unknown fis):",(QWORD)*(pointer+0x18));
	say("fb+64:",(QWORD)*(pointer+0x19));
	say("fb+68:",(QWORD)*(pointer+0x1a));
	say("fb+6c:",(QWORD)*(pointer+0x1b));
*/

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
