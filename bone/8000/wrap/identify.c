#include "sata.h"

#define idehome 0x100000
#define fshome 0x200000
#define dirhome 0x300000
#define datahome 0x400000




void say(char*,...);




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
		say("error:no cmdslot",0);
		return -1;
	}
	cmdheader += cmdslot;
					//say("cmdslot:",(QWORD)cmdslot);
					//say("cmdheader:",(QWORD)cmdheader);
	cmdheader->cfl=sizeof(FIS_REG_H2D)/sizeof(DWORD);//Command FIS size
	cmdheader->w = 0;		// Read from device
	cmdheader->prdtl=1;
	cmdheader->prdbc=0;




	//make the table
	CMD_TABLE* cmdtable = (CMD_TABLE*)(QWORD)cmdheader->ctba;
					//say("cmdtable(comheader->ctba):",(QWORD)cmdtable);
	char* p=(char*)cmdtable;
	int i=sizeof(CMD_TABLE);
	for(;i>0;i--){p[i]=0;}
	cmdtable->prdt_entry[0].dba = rdi;
	cmdtable->prdt_entry[0].dbc = 0x200+1;	// ? bytes per sector(last 1 is)
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
			say("(timeout1)port->tfd:%x",(QWORD)port->tfd);
			return -1;
		}

									//0x88=interface busy|data transfer requested
		temp=port->tfd;								//0x20
		if( (temp&0x80) == 0x80 )continue;			//bit7,busy
		if( (temp&0x8) == 0x8)continue;		//bit3,DRQ

		break;
	}
	//say("is:",(QWORD)port->is);
	//unsigned int* pointer=(unsigned int*)(QWORD)(port->fb);




	//set issue,wait for completion
	port->ci = 1<<cmdslot;	// Issue command,start reading
	timeout=0;
	while(1)
	{
		timeout++;
		if(timeout>0xfffff)
		{
			say("(timeout2)ci=%x,prdbc=%x",temp,cmdheader->prdbc);
			return -2;
		}

		temp=port->is;
		if (temp & 0x40000000)	// Task file error
		{
			say("port error 1");
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
	say("out %x %x\n",control+2,4);

	out8(control+2,0);
	say("out %x %x\n",control+2,0);

	out8(command+6,0xa0);
	say("out %x %x\n",command+6,0xa0);

	//50:hd exist
	i=0;
	while(1)
	{
		i++;
		if(i>0xfffff)
		{
			say("device not ready:%x\n",temp);
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
	say("out %x %x\n",command+6,0xe0);

	//start reading
	out8(command+7,0xec);
	say("out %x %x\n",command+7,0xec);

	//check hd data ready?
	i=0;
	while(1)
	{
		i++;
		if(i>0xfffff)
		{
			say("data not ready!\n");
			return -2;
		}

		temp=in8(command+7);
		if( (temp&0x80) == 0x80 )continue;			//busy
		if( (temp&0x8) == 0 )continue;			//DRQ

		break;
	}

	//read data
	say("reading data\n");
	for(i=0;i<0x100;i++)
	{
		temp=in8(command+7);
		//if( (temp&1) == 1)break;
		if( (temp&0x8) != 8)break;

		temp=in16(command+0);		//只管放，后面再调字节序
		dest[i]=temp;
	}

	if(i < 0x100) say("not finished:%x\n",i);
	return 1;
}








int identify(QWORD rdi)
{
	QWORD type=*(QWORD*)idehome;
	QWORD temp=0;
	if(type == 0x61746173)					//sata
	{
		temp=*(QWORD*)(idehome+8);
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