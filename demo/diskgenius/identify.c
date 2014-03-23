#include "define.h"

int identify(DWORD edi,QWORD addr)
{
	HBA_PORT* port=(HBA_PORT*)addr;
	port->is = (DWORD)-1;		// Clear pending interrupt bits
	
	int cmdslot;
	int temp=(port->sact | port->ci);
        for (cmdslot=0;cmdslot<32;cmdslot++)    //cmdslots=32
        {
                if ((temp&1) == 0)break;
                temp>>= 1;
        }

	if (cmdslot == 32){
		say("error:no cmdslot",(QWORD)cmdslot);
		return -1;
	}
	say("cmdslot:",(QWORD)cmdslot);

	HBA_CMD_HEADER* cmdheader = (HBA_CMD_HEADER*)(QWORD)(port->clb);
	cmdheader += cmdslot;
	say("cmdheader:",(QWORD)cmdheader);
	cmdheader->cfl=sizeof(FIS_REG_H2D)/sizeof(DWORD);//Command FIS size
	cmdheader->w = 0;		// Read from device
	cmdheader->prdtl=1;

	CMD_TABLE* cmdtable = (CMD_TABLE*)(QWORD)cmdheader->ctba;
	say("cmdtable(comheader->ctba):",(QWORD)cmdtable);

	char* p=(char*)cmdtable;
	int i=sizeof(CMD_TABLE);
	for(;i>0;i--){p[i]=0;}

	cmdtable->prdt_entry[0].dba = edi;
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
