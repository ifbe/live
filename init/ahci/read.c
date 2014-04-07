#include "sata.h"

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

	CMD_TABLE* cmdtable = (CMD_TABLE*)(QWORD)cmdheader->ctba;//e000左右
	//say("cmdtable(comheader->ctba):",(QWORD)cmdtable);

	char* p=(char*)cmdtable;
	int i=sizeof(CMD_TABLE)+(cmdheader->prdtl-1)*sizeof(HBA_PRDT_ENTRY);
	for(;i>0;i--){p[i]=0;}
 
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
 
	fis->lba0 = (BYTE)from;
	fis->lba1 = (BYTE)(from>>8);
	fis->lba2 = (BYTE)(from>>16);
	fis->device = 0x40;	// LBA mode
 
	fis->lba3 = (BYTE)(from>>24);
	fis->lba4 = (BYTE)(from>>32);
	fis->lba5 = (BYTE)(from>>40);
 
	fis->countl = count&0xff;
	fis->counth = (count>>8)&0xff;
}
int read(QWORD buf,QWORD from,QWORD addr,DWORD count)
{
	HBA_PORT* port =(HBA_PORT*)addr;
	port->is = (DWORD)-1;		// Clear pending interrupt bits
	
	int cmdslot = find_cmdslot(port);
	if (cmdslot == -1){
		//say("error:no cmdslot",0);
		return 0;
	}
	//say("cmdslot:",(QWORD)cmdslot);

	HBA_CMD_HEADER* cmdheader = (HBA_CMD_HEADER*)(QWORD)(port->clb);
	cmdheader += cmdslot;
	//say("cmdheader:",(QWORD)cmdheader);

	maketable(buf,from,cmdheader,count);

	int spin = 0;
	//0x88=interface busy|data transfer requested
	while ((port->tfd & (0x88)) && spin < 1000000){
		spin++;
	}
	if (spin==1000000){
		//say("error:port->tfd:",(QWORD)port->tfd);
	}
 
	port->ci = 1<<cmdslot;	// Issue command

	//say("is:",(QWORD)port->is);
	unsigned int* pointer=(unsigned int*)(QWORD)(port->fb);

	while (1){
		// in the PxIS port field as well (1 << 5)
		if ((port->ci & (1<<cmdslot)) == 0) 
			break;
		if (port->is & 0x40000000)	// Task file error
		{
			//say("port error 1",0);
			return 0;
		}
	}
	return 1;
}
