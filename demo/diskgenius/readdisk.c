#include "readdisk.h"

void start()
{
	QWORD addr;

	addr=finddisk();
	if(addr!=0)diskgenius(addr);
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

    if(ch<0)ch=0x20;
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





 

QWORD finddisk()
{
        QWORD addr=0x6008;
        unsigned int temp;
        for(;addr<0x6200;addr+=0x10)
        {
                temp=*(unsigned int*)addr;
                if(temp==0x61746173)
                {
                        addr-=0x8;
                        temp=*(unsigned int*)addr;
                        say("sata address:",(QWORD)temp);
                        return (QWORD)temp;
                }
        }
	return 0;
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
void maketable(QWORD buf,QWORD start,HBA_CMD_HEADER* cmdheader,DWORD count)
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
int read(QWORD buf,QWORD start,QWORD addr,DWORD count)
{
	HBA_PORT* port =(HBA_PORT*)addr;
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
	maketable(buf,start,cmdheader,count);

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
void diskgenius(QWORD addr)
{
	read(0x100000,0,addr,8);

        char* p=(char*)0x100000;
        int count=0;

        for(;count<0x200;count++){
        anscii(2*(count%0x20),where+count/0x20,(p[count]>>4)&0xf);
        anscii(2*(count%0x20)+1,where+count/0x20,p[count]&0xf);

        anscii(0x40+2*count%0x40+1,where+count/0x20,p[count]);
        }

}
