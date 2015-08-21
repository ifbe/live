#include "sata.h"

#define idehome 0x100000




void say(char*,...);




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
void maketable(QWORD buf,QWORD from,HBA_CMD_HEADER* cmdheader,DWORD count)
{
	cmdheader->cfl=sizeof(FIS_REG_H2D)/sizeof(DWORD);//Command FIS size
	cmdheader->w = 0;		// Read from device
	cmdheader->prdtl = (WORD)((count-1)>>4) + 1;	// PRDT entries count
	//say("ptdtl",(QWORD)cmdheader->prdtl);




	CMD_TABLE* cmdtable = (CMD_TABLE*)(QWORD)cmdheader->ctba;
								//say("cmdtable@",(QWORD)cmdtable);
	char* p=(char*)cmdtable;
	int i=sizeof(CMD_TABLE)+(cmdheader->prdtl-1)*sizeof(HBA_PRDT_ENTRY);
	for(;i>0;i--){p[i]=0;}
 



	// 8K bytes (16 sectors) per PRDT
	for (i=0; i<cmdheader->prdtl-1; i++)
	{
		cmdtable->prdt_entry[i].dba = (DWORD)buf;
		cmdtable->prdt_entry[i].dbc = 16*512+1; // 0x2000
		//cmdtable->prdt_entry[i].i = 1;
		buf += 0x2000;  // 8K words
		count -= 16;    // 16 sectors
	}
	cmdtable->prdt_entry[i].dba = (DWORD)buf;		// Last entry
	cmdtable->prdt_entry[i].dbc = count*512+1;		// 512 bytes per sector
	//cmdtable->prdt_entry[i].i = 1;




	// Setup command
	FIS_REG_H2D* fis = (FIS_REG_H2D*)(&cmdtable->cfis);

	fis->fis_type = FIS_TYPE_REG_H2D;		//		0
	fis->c = 1;	// Command					//		[1].[0,0]
	//fis->command = 0x20;
	//fis->command = ATA_CMD_READ_DMA_EX;
	fis->command = 0x25;					//		2

	fis->lba0 = (BYTE)from;					//		4
	fis->lba1 = (BYTE)(from>>8);			//		5
	fis->lba2 = (BYTE)(from>>16);			//		6
	fis->device = 1<<6;						//		7		1<<6=LBA mode

	fis->lba3 = (BYTE)(from>>24);			//		8
	fis->lba4 = (BYTE)(from>>32);			//		9
	fis->lba5 = (BYTE)(from>>40);			//		a

	fis->countl = count&0xff;				//		c
	fis->counth = (count>>8)&0xff;			//		d
}
int readpart(QWORD buf,QWORD from,QWORD addr,DWORD count)
{
	HBA_PORT* port =(HBA_PORT*)addr;
	HBA_CMD_HEADER* cmdheader = (HBA_CMD_HEADER*)(QWORD)(port->clb);
	port->is = (DWORD)-1;		// Clear pending interrupt bits




	//find cmdslot
	int cmdslot = find_cmdslot(port);
	if (cmdslot == -1)
	{
		say("error:no cmdslot",0);
		return -1;
	}
	cmdheader += cmdslot;
	//say("cmdslot:",(QWORD)cmdslot);
	//say("cmdheader:",(QWORD)cmdheader);




	//make the table
	maketable(buf,from,cmdheader,count);
	DWORD temp=0;
	volatile int timeout = 0;
	while(1)
	{
		timeout++;
		if(timeout>0xfffff)
		{
			say("(timeout1)port->tfd:%x",(QWORD)port->tfd);
			return -11;
		}

		temp=port->tfd;							//0x20
		if( (temp&0x80) == 0x80 )continue;		//bit7,busy
		if( (temp&0x8) == 0x8)continue;			//bit3,DRQ
					//0x88=interface busy|data transfer requested

		break;
	}
	//say("is:",(QWORD)port->is);
	//unsigned int* pointer=(unsigned int*)(QWORD)(port->fb);




	//issue
	port->ci = 1<<cmdslot;	// Issue command
	timeout=0;
	while (1)
	{
		timeout++;
		if(timeout>0xfffff)
		{
			say("(timeout2)port->ci=%x",temp);
			return -22;
		}

		//
		temp=port->is;
		if (temp & 0x40000000)	// Task file error
		{
			say("port error 1",0);
			return -33;
		}

		// in the PxIS port field as well (1 << 5)
		temp=port->ci;
		if ((temp & (1<<cmdslot)) != 0)continue;

		break;
	}
	return 0;
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
void readide(QWORD buf,QWORD from,QWORD notcare,DWORD count)
{
	QWORD command=*(QWORD*)(idehome+0x10);		//command
	QWORD control=*(QWORD*)(idehome+0x20);		//control
	char* dest=(char*)buf;
	volatile int i;
	volatile DWORD temp;

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
			return;
		}

		temp=in8(command+7);
		if( (temp&0x80) == 0x80 )continue;			//busy
		if( (temp&0x40) == 0x40 )break;			//Device ReaDY?
	}

	//count
	out8(command+2,count&0xff);
	say("total:%x\n",count);

	//lba
	out8(command+3,from&0xff);
	out8(command+4,(from>>8)&0xff);
	out8(command+5,(from>>16)&0xff);
	say("lba:%x\n",from);

	//mode
	out8(command+6,0xe0);
	say("out %x %x\n",command+6,0xe0);

	//start reading
	out8(command+7,0x20);
	say("out %x %x\n",command+7,0x20);

	//check hd data ready?
	i=0;
	while(1)
	{
		i++;
		if(i>0xfffff)
		{
			say("data not ready!\n");
			return;
		}

		temp=in8(command+7);
		if( (temp&0x80) == 0x80 )continue;			//busy
		if( (temp&0x8) == 0 )continue;			//DRQ

		break;
		//if( (temp&0x1) == 0x1 )
		//{
		//	say("read error:%x\n",temp);
		//	return;
		//}
		//else break;
	}

	//read data
	say("reading data\n");
	for(i=0;i<0x200;i+=2)
	{
		temp=in8(command+7);
		//if( (temp&1) == 1)break;
		if( (temp&0x8) != 8)break;

		temp=in16(command+0);
		dest[i]=temp&0xff;
		dest[i+1]=(temp>>8)&0xff;
	}

	if(i < 0x200) say("not finished:%x\n",i);
}








int read(QWORD buf,QWORD from,QWORD notcare,DWORD count)
{
	//暂时管不了多硬盘,所以指定用寻找到的第一个
	QWORD type=*(QWORD*)idehome;
	if(type == 0x61746173)
	{
		QWORD addr=*(QWORD*)(idehome+8);
		QWORD i=0;
		while(count>16)
		{
			readpart(buf+i*0x2000,from+i*16,addr,16);
			i++;
			count-=16;
		}
		readpart(buf+i*0x2000,from+i*16,addr,count);
	}
	else if(type == 0x656469)
	{
		readide(buf,from,0,count);
	}
	//todo:		U盘
}
