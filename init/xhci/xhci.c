#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long
#define xhcihome 0x300000


static QWORD portaddr=0;
static QWORD memaddr=0;
static QWORD msixtable=0;
static QWORD pendingtable=0;



void findaddr()
{
        QWORD addr=0x5004;
        DWORD temp;
        for(;addr<0x6000;addr+=0x10)
        {
                temp=*(DWORD*)addr;
                temp&=0xffffff00;
                if(temp==0x0c033000)
                {
                        addr+=4;
                        portaddr=*(DWORD*)addr;
                        say("xhci(port)@",portaddr);
                        return;
                }
        }
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
void explaincapability()
{
	DWORD offset;
	DWORD temp;
	DWORD kind;
	DWORD next;

	say("pci cap:",0);
	say("{",0);

	out32(0xcf8,portaddr+0x34);
	offset=in32(0xcfc)&0xff;

	while(1)
	{
		out32(0xcf8,portaddr+offset);
		temp=in32(0xcfc);
		kind=temp&0xff;
		next=(temp>>8)&0xff;

		say("    @",offset);
		switch(kind)
		{
			case 0x5:
			{
				out32(0xcf8,portaddr+offset);
				temp=in32(0xcfc);
				if( (temp&0x800000) !=0 )
				{
					say("    32bit msi:",kind);
					//out32(0xcf8,portaddr+offset+4);
					//out32(0xcfc,0xfee00000);
					//out32(0xcf8,portaddr+offset+8);
					//out32(0xcfc,0x20);
					//out32(0xcf8,portaddr+offset);
					//out32(0xcfc,temp|0x10000);

					//out32(0xcf8,portaddr+offset+4);
					//say("    addr:",in32(0xcfc));
					//out32(0xcf8,portaddr+offset+8);
					//say("    data:",in32(0xcfc)&0xffff);
					//out32(0xcf8,portaddr+offset);
					//say("    control:",in32(0xcfc)>>16);
				}
				else
				{
					say("    64bit msi:",kind);
					//out32(0xcf8,portaddr+offset+8);
					//out32(0xcfc,0xfee00000);
					//out32(0xcf8,portaddr+offset+0x10);
					//out32(0xcfc,0x20);
					//out32(0xcf8,portaddr+offset);
					//out32(0xcfc,temp|0x10000);

					//out32(0xcf8,portaddr+offset+8);
					//say("    addr:",in32(0xcfc));
					//out32(0xcf8,portaddr+offset+0x10);
					//say("    data:",in32(0xcfc)&0xffff);
					//out32(0xcf8,portaddr+offset);
					//say("    control:",in32(0xcfc)>>16);
				}

				break;
			}
			case 0x11:
			{
				say("    msix:",kind);

				//read bar0
				out32(0xcf8,portaddr+0x10);
				memaddr=in32(0xcfc)&0xfffffff0;

				//table offset
				out32(0xcf8,portaddr+offset+4);
				msixtable=memaddr+in32(0xcfc);
				say("    msix table:",msixtable);

				//pba offset
				out32(0xcf8,portaddr+offset+8);
				pendingtable=memaddr+in32(0xcfc);
				say("    pending table:",pendingtable);

				//messagecontrol
				out32(0xcf8,portaddr+offset);
				temp=in32(0xcfc)|0x80000000;
				out32(0xcf8,portaddr+offset);
				out32(0xcfc,temp);
				say("    control:",in32(0xcfc)>>16);

				break;
			}
			default:
			{
				say("    ?:",kind);
			}
		}

		if(kind == 0) break;	//当前capability类型为0，结束
		if(next < 0x40) break;	//下一capability位置错误，结束

		offset=next;
	}

	say("}",0);
}
QWORD probepci()
{
	//disable pin interrupt
        out32(0xcf8,portaddr+0x4);
        DWORD temp=in32(0xcfc)|(1<<10);
        out32(0xcf8,portaddr+0x4);
        out32(0xcfc,temp);

        //out32(0xcf8,portaddr+0x4);
        //say("pci status and command:",(QWORD)in32(0xcfc));

	//deal with capability list
	explaincapability();

	//get memaddr from bar0
        out32(0xcf8,portaddr+0x10);
        memaddr=in32(0xcfc)&0xfffffff0;
        say("xhci@",memaddr);

        int i=0;
        QWORD* table=(QWORD*)0x5000;
        for(i=0;i<0x200;i+=2)
        {
                if(table[i]==0){
                        table[i]=0x69636878;
                        table[i+1]=memaddr;
                        break;
                }
        }
}




int ownership(QWORD addr)
{
	QWORD temp=*(DWORD*)addr;

	//set hc os owned semaphore
	*(DWORD*)addr=temp|0x1000000;

	//等一会
	QWORD waiter=0xffffff;
	for(;waiter>0;waiter--) asm("nop");

	//时间到了，看看控制权到手没
	temp=*(DWORD*)addr;
	temp&=0x1000000;
	if(temp == 0x1000000)
	{
		say("    grabing ownership",0);

		temp=*(DWORD*)addr;
		temp&=0x10000;
		if(temp == 0)
		{
			say("    bios gone",0);
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




void probexhci()
{
	say("version:",(*(DWORD*)memaddr) >> 16);

	QWORD caplength=(*(DWORD*)memaddr) & 0xffff;
	say("caplength:",caplength&0xffff);

	QWORD hcsparams1=*(DWORD*)(memaddr+4);
	say("hcsparams1:",hcsparams1);
	QWORD hcsparams2=*(DWORD*)(memaddr+8);
	say("hcsparams2:",hcsparams2);
	QWORD hcsparams3=*(DWORD*)(memaddr+0xc);
	say("hcsparams3:",hcsparams3);
	QWORD capparams=*(DWORD*)(memaddr+0x10);
	say("capparams:",capparams);

	say("",0);

	QWORD dboff=memaddr+(*(DWORD*)(memaddr+0x14));
	say("dboff:",dboff);
	QWORD rtsoff=memaddr+(*(DWORD*)(memaddr+0x18));
	say("rtsoff:",rtsoff);
	QWORD operational=memaddr+caplength;
	say("operational:",operational);
	QWORD xecp=memaddr+( (capparams >> 16) << 2 );
	say("xecp:",xecp);

	explainxecp(xecp);	//mostly,grab ownership

	say("initxhci",0);
	say("{",0);




//----------------------------1.xhci复位------------------------------
//init system io memory map,if supported
//xhci reset,wait until CNR flag is 0
//--------------------------------------------------------------------
	say("    1.stop&reset:",0);

	QWORD usbcommand=*(DWORD*)operational;
	QWORD usbstatus=*(DWORD*)(operational+4);
	say("    usbcommand(origin):",usbcommand);
	say("    usbstatus(origin):",usbstatus);

	//xhci正在运行吗
	if( (usbstatus&0x1) == 0)		//HCH位为0，即正在运行
	{
		say("    it's running,it'll stop",0);

		//按下停止按钮
		*(DWORD*)operational=usbcommand&0xfffffffe;

		//等一会
		QWORD wait1=0xffffff;
		for(;wait1>0;wait1--) asm("nop");

		//xhci停了吗
		usbstatus=*(DWORD*)(operational+4);
		if( (usbstatus&0x1) == 0)	//HCH位为0，即正在运行
		{
			say("    error:not stop",0);
			return;
		}
	}

	//关掉中断
	usbcommand=*(DWORD*)operational;
	*(DWORD*)operational=usbcommand&0xfffffffb;

	//按下复位按钮
	usbcommand=*(DWORD*)operational;
	*(DWORD*)operational=usbcommand&0xfffffffd;

	//等一会
	QWORD wait2=0xffffff;
	for(;wait2>0;wait2--) asm("nop");

	//复位成功了吗
	usbcommand=*(DWORD*)operational;
	if( (usbcommand&0x2) ==2)
	{
		say("    error:reset failed:",usbcommand);
		return;
	}

	//好像第一步成功了
	usbcommand=*(DWORD*)operational;
	usbstatus=*(DWORD*)(operational+4);
	say("    usbcommand(now):",usbcommand);
	say("    usbstatus(now):",usbstatus);
	say("    ok!",0);




//----------------------2.xhci结构---------------------------
//program the max device slot enabled field in config register
//program the dcbaap
//program crcr,point to addr of first trb in command ring
//----------------------------------------------------------
	say("    2.maxslot&context&ring",0);




//-----------------------3.中断----------------------
//misx table,message addr,message data,enable vector
//allocate&init msix pending bit array
//point table&pba offset to message control table and pending bit array
//init message control register of msix capability structure
//-------------------------------------------------
	say("    3.interrupt",0);




//--------------------4.设备---------------------------
//------------------------------------------------------
	say("    4.device",0);

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




//---------------------xhci启动---------------------
//write usbcmd,turn host controller on
//-------------------------------------------------
	say("    5.turnon",0);

	say("}",0);
}




void initxhci()
{
	//clear home
        QWORD addr;
	addr=xhcihome;
	for(;addr<xhcihome+0x100000;addr++) *(BYTE*)addr=0;

        findaddr();		//pci addr of port
        if(portaddr==0) return;

        probepci();		//memory addr of xhci
        if(memaddr==0) return;

	probexhci();

	say("",0);
}
