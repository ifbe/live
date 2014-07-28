#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long
#define xhcihome 0x300000



unsigned int findaddr()
{
        QWORD addr=0x5004;
        unsigned int temp;
        for(;addr<0x6000;addr+=0x10)
        {
                temp=*(unsigned int*)addr;
                temp&=0xffffff00;
                if(temp==0x0c033000)
                {
                        addr+=4;
                        temp=*(unsigned int*)addr;
                        say("xhci(port)@",(QWORD)temp);
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
unsigned int probepci(QWORD addr)
{
        out32(0xcf8,addr+0x4);
        unsigned int temp=in32(0xcfc)|(1<<10);
        out32(0xcf8,addr+0x4);
        out32(0xcfc,temp);

        out32(0xcf8,addr+0x4);
        //say("pci status and command:",(QWORD)in32(0xcfc));

        out32(0xcf8,addr+0x10);
        addr=in32(0xcfc)&0xfffffff0;
        say("xhci@",addr);

        int i=0;
        unsigned long long* table=(unsigned long long*)0x5000;
        for(i=0;i<0x200;i+=2)
        {
                if(table[i]==0){
                        table[i]=0x69636878;
                        table[i+1]=addr;
                        break;
                }
        }
        return addr;
}




int ownership(QWORD addr)
{
	QWORD temp=*(DWORD*)addr;

	//set hc os owned semaphore
	*(DWORD*)addr=temp|0x1000000;

	//等一会
	QWORD waiter=0xffffff;
	for(;waiter>0;waiter--) asm("nop");

	//时间到了，看看到手没
	temp=*(DWORD*)addr;
	temp&=0x1000000;
	if(temp == 0x1000000)
	{
		say("    grabing ownership");

		temp=*(DWORD*)addr;
		temp&=0x10000;
		if(temp == 0)
		{
			say("    bios gone");
			return 0;
		}
	}

	say("    failed",0);
	return -1;
}




void explainxecp(QWORD addr)
{
	DWORD temp;
	BYTE kind;
	QWORD next;

	say("{",0);
	while(1)
	{
		temp=*(DWORD*)addr;

		kind=temp&0xff;
		if(kind == 0) break;
		next=(temp>>6)&0x3fc;
		if(next == 0) break;

		say("    @",addr);
		say("    cap:",kind);

		if(kind == 1)
		{
			int result=ownership(addr);
			if(result < 0) return;
		}

		addr+=next;
	}
	say("}",0);
}




void probexhci(QWORD addr)
{
	say("version:",(*(DWORD*)addr) >> 16);

	QWORD caplength=(*(DWORD*)addr) & 0xffff;
	say("caplength:",caplength&0xffff);

	QWORD hcsparams1=*(DWORD*)(addr+4);
	say("hcsparams1:",hcsparams1);
	QWORD hcsparams2=*(DWORD*)(addr+8);
	say("hcsparams2:",hcsparams2);
	QWORD hcsparams3=*(DWORD*)(addr+0xc);
	say("hcsparams3:",hcsparams3);
	QWORD capparams=*(DWORD*)(addr+0x10);
	say("capparams:",capparams);

	say("",0);

	QWORD dboff=addr+(*(DWORD*)(addr+0x14));
	say("dboff:",dboff);
	QWORD rtsoff=addr+(*(DWORD*)(addr+0x18));
	say("rtsoff:",rtsoff);
	QWORD operational=addr+caplength;
	say("operational:",operational);
	QWORD xecp=addr+( (capparams >> 16) << 2 );
	say("xecp:",xecp);

	explainxecp(xecp);	//mostly,grab ownership

	//-------------------reset-------------------
	//init system io memory map,if supported
	//xhci reset,wait until CNR flag is 0

	//-------------------xhci结构---------------------
	//program the max device slot enabled field in config register
	//program the dcbaap
	//program crcr,point to addr of first trb in command ring

	//-----------------------中断----------------------
	//misx table,message addr,message data,enable vector
	//allocate&init msix pending bit array
	//point table&pba offset to message control table and pending bit array
	//init message control register of msix capability structure

	//--------------------每个设备---------------------------

		//-------------define event ring-----------
		//allocate and initialize the event ring segments
		//erst,point to and define size(in trbs)of event ring
		//erstsz,number of segments described by erst
		//erdp,addr of first segment described by erst
		//erstba,point to where event ring segment table is located

		//enable msix,enable flag in msix cap struct meessage control r
		//init interval field of interrupt moderation register
		//enable INTE in USBCMD
		//enable IE in interrupt management r

	//-----------------xhci启动-----------------
	//write usbcmd,turn host controller on
}




void initxhci()
{
        QWORD addr;

        addr=findaddr();                //pci addr of port
        if(addr==0) return;

        addr=probepci(addr);            //memory addr of ahci
        if(addr==0) return;

	probexhci(addr);

	say("",0);
}
