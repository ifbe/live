#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long
#define xhcihome 0x300000
#define ersthome xhcihome+0
#define eventhome xhcihome+0x10000
#define crcrhome xhcihome+0x40000
#define dcbaahome xhcihome+0x80000
#define scratchpadhome xhcihome+0x90000
#define inputhome xhcihome+0xa0000
#define outputhome xhcihome+0xb0000
#define transferhome xhcihome+0xc0000


static QWORD portaddr=0;

static QWORD memaddr=0;
static QWORD msixtable=0;
static QWORD pendingtable=0;

static QWORD operational;
static QWORD portbase;
static QWORD doorbell;
static QWORD runtime;
static QWORD contextsize;

volatile static QWORD eventsignal=0;
volatile static QWORD eventaddr;

static QWORD commandenqueue=crcrhome;
static DWORD commandcycle=1;




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
say("pci cap:",0);
say("{",0);

	DWORD offset;
	DWORD temp;
	DWORD kind;
	DWORD next;

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

			out32(0xcf8,portaddr+offset);
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

	say("explain xecp@",addr);
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
//基本信息
say("base information",0);
say("{",0);

	QWORD version=(*(DWORD*)memaddr) >> 16;
	QWORD caplength=(*(DWORD*)memaddr) & 0xffff;

	QWORD hcsparams1=*(DWORD*)(memaddr+4);
	QWORD hcsparams2=*(DWORD*)(memaddr+8);
	QWORD hcsparams3=*(DWORD*)(memaddr+0xc);
	QWORD capparams=*(DWORD*)(memaddr+0x10);

	operational=memaddr+caplength;
	portbase=operational+0x400;
	doorbell=memaddr+(*(DWORD*)(memaddr+0x14));
	runtime=memaddr+(*(DWORD*)(memaddr+0x18));

	if( (capparams&0x4) == 0x4){contextsize=0x40;}
	else{contextsize=0x20;}

	say("    version:",version);
	say("    caplength:",caplength);

	say("    hcsparams1:",hcsparams1);
	say("    hcsparams2:",hcsparams2);
	say("    hcsparams3:",hcsparams3);
	say("    capparams:",capparams);
	say("    contextsize:",contextsize);

	say("    operational@",operational);
	say("    portbase@",portbase);
	say("    doorbell@",doorbell);
	say("    runtime@",runtime);

say("}",0);




//mostly,grab ownership
QWORD xecp=memaddr+( (capparams >> 16) << 2 );
explainxecp(xecp);




//打印bios初始化完后的状态
say("before we destory everything",0);
say("{",0);

	QWORD usbcommand=*(DWORD*)operational;
	QWORD usbstatus=*(DWORD*)(operational+4);
	QWORD pagesize=0x1000;
	QWORD crcr=*(DWORD*)(operational+0x18);
	QWORD dcbaa=*(DWORD*)(operational+0x30);
	QWORD config=*(DWORD*)(operational+0x38);
	QWORD msixcontrol=*(DWORD*)(msixtable+0xc);
	QWORD msixpending=*(DWORD*)pendingtable;

	//蛋疼的计算pagesize
	QWORD psz=*(DWORD*)(operational+8);
	while(1)
	{
		if(psz == 0){
			say("psz:",psz);
			return;
		}
		if(psz == 1) break;

		psz>>1;
		pagesize<<1;
	}

	say("    usbcommand:",usbcommand);
	say("    usbstatus:",usbstatus);
	say("    pagesize:",pagesize);
	say("    crcr:",crcr);
	say("    dcbaa:",dcbaa);
	say("    config:",config);
	say("    msixcontrol:",msixcontrol);
	say("    msixpending:",msixpending);


say("}",0);



say("init xhci",0);
say("{",0);
//----------------------------1.xhci复位------------------------------
//init system io memory map,if supported
//xhci reset,wait until CNR flag is 0
//--------------------------------------------------------------------
say("1.stop&reset:",0);

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

	//controller not ready=1就是没准备好
	usbstatus=*(DWORD*)(operational+4);
	if( (usbstatus&0x800) == 0x800 )
	{
		say("    error:controller not ready:",usbstatus);
		return;
	}

	//好像第一步成功了
	say("    usbcommand:",usbcommand);
	say("    usbstatus:",usbstatus);
	say("    ok!",0);




//----------------------2.xhci结构---------------------------
//program the max device slot enabled field in config register
//program the dcbaap
//program crcr,point to addr of first trb in command ring
//----------------------------------------------------------
say("2.maxslot&dcbaa&crcr:",0);

	//maxslot=deviceslots
	*(DWORD*)(operational+0x38)=(*(DWORD*)(memaddr+4)) &0xff;
	say("    maxslot:",*(DWORD*)(operational+0x38) );

	//dcbaa
	*(DWORD*)(operational+0x30)=dcbaahome;
	*(DWORD*)(operational+0x34)=0;
	say("    dcbaa:",*(DWORD*)(operational+0x30) );

	//scratchpad
	//*(DWORD*)dcbaahome=scratchpadhome;
	//say("    scratchpad:",scratchpadhome);

	//command linktrb:lastone point to firstone
	*(QWORD*)(crcrhome+255*0x10)=crcrhome;
	*(QWORD*)(crcrhome+255*0x10+8)=0;
	*(QWORD*)(crcrhome+255*0x10+0xc)=(6<<10)+2;

	//crcr
	*(DWORD*)(operational+0x18)=crcrhome+1;
	*(DWORD*)(operational+0x1c)=0;
	say("    crcr:",*(DWORD*)(operational+0x18));




//-----------------------3.中断----------------------
//msix table,message addr,message data,enable vector
//allocate&init msix pending bit array
//point table&pba offset to message control table and pending bit array
//init message control register of msix capability structure
//-------------------------------------------------
say("3.interrupt:",0);

	int20();

	//msixtable[0].addr
	*(DWORD*)msixtable=0xfee00000;
	*(DWORD*)(msixtable+4)=0;
	//msixtable[0].data
	*(DWORD*)(msixtable+8)=0x20;

	//msixtable[0].control,enable
	msixcontrol=*(DWORD*)(msixtable+0xc);
	*(DWORD*)(msixtable+0xc)=msixcontrol&0xfffffffe;
	say("    msixcontrol:",*(DWORD*)(msixtable+0xc));




//--------------------4.eventring---------------------------
//------------------------------------------------------
say("4.eventring:",0);

	//-------------define event ring-----------
	//build the "event ring segment table"
	*(DWORD*)(ersthome)=eventhome;
	*(DWORD*)(ersthome+0x8)=0x100;

	//ERSTSZ
	*(DWORD*)(runtime+0x28)=1;

	//ERSTBA
	*(DWORD*)(runtime+0x30)=ersthome;
	*(DWORD*)(runtime+0x34)=0;	//这一行必须有

	//ERDP
	*(DWORD*)(runtime+0x38)=eventhome;
	*(DWORD*)(runtime+0x3c)=0;	//这一行也必须有

	//enable EWE|HSEIE|INTE(0x400|0x8|0x4) in USBCMD
	*(DWORD*)operational |= 0xc;
	//*(DWORD*)operational |= 0x40c;

	//IMOD
	*(DWORD*)(runtime+0x24)=4000;

	//IMAN
	*(DWORD*)(runtime+0x20) |= 0x2;

	say("    event ring done",0);




//---------------------xhci启动---------------------
//write usbcmd,turn host controller on
//-------------------------------------------------
say("5.turnon",0);
	//turn on
	*(DWORD*)operational |= 0x1;

	//wait for 100ms?
	QWORD wait3=0xffffff;
	while(wait3--)asm("nop");

	//
	say("    command:",*(DWORD*)operational);
	say("    status:",*(DWORD*)(operational+4));

say("}",0);
}




int waitevent(QWORD trbtype)
{
	QWORD timeout=0;
	QWORD data;
	while(1)
	{
		if(timeout>0xfffff){
			say("timeout!",0);
			return -1;
		}
		if(eventsignal==0){
			timeout++;
			continue;
		}

		data=*(DWORD*)(eventaddr+0xc);
		if( ( (data>>10)&0x3f) == trbtype)
		{
			eventsignal=0;
			return 1;
		}
	}
}




void initport(portnum)
{
say("init port:",0);
say("{",0);

//------------who is calling me?--------------------
say("1.i am port manager",0);
//----------------------------------------------------------

//---------------------which naughty port-------------------
say("2.portnum:",portnum);
//---------------------------------------------------------

DWORD portsc=*(DWORD*)(portbase+portnum*0x10-0x10);
DWORD slot;

if( (portsc&0x1) == 0x1)	//是attach
{
	say("3.device attach!",0);




	//---------------obtain slot------------------
	say("4.enable slot",0);

	*(QWORD*)commandenqueue=0;
	*(QWORD*)(commandenqueue+8)=0;
	*(DWORD*)(commandenqueue+0xc)=(9<<10)+commandcycle;
	commandenqueue+=0x10;
	*(DWORD*)doorbell=0;

	if(waitevent(0x21)<0) return;

	slot=(*(DWORD*)(eventaddr+0xc)) >> 24;
	say("    slot:",slot);
	//-------------------------------------------




	//-------------slot initialization----------------
	say("5.init slot",0);
	if(slot>=0x10){
		say("    bad slotnum",0);
		return;
	}

	//1)clear to zero
	QWORD inputaddr=inputhome+slot*0x1000;
	for(;inputaddr<inputhome+slot*0x1000+0x1000;inputaddr+=4)
	{
		*(DWORD*)inputaddr=0;
	}
	inputaddr=inputhome+slot*0x1000;
	//2)A0,A1 flag=1
	*(DWORD*)(inputaddr+4)=3;
	//3)slot context
	*(DWORD*)(inputaddr+contextsize)=1<<27;
	*(DWORD*)(inputaddr+contextsize+4)=portnum<<16;
	//4)transfer ring initialization
	QWORD transferaddr=transferhome+slot*0x1000;
	//5)endpoint context0
	*(DWORD*)(inputaddr+contextsize*2+4)=(((portsc>>10)&0xf)<<16)|(4<<3)|6;
	*(DWORD*)(inputaddr+contextsize*2+8)=transferaddr+1;

	//6)output context
	QWORD outputaddr=outputhome+slot*0x1000;
	//7)output context填入对应dcbaa
	//8)address device command for device slot
	//---------------------------------------------




	//--------------address device----------------
	say("6.address device",0);
	//--------------------------------------------



	//------------------get descriptor-----------------
	say("7.get descriptor",0);
	//i.allocate an 8B buffer to receive the device descriptor
	//ii.init setup stage td
	//	trbtype=setup stage trb
	//	trb transfer length=8
	//	ioc=0
	//	idt=1
	//	bmrequesttype=0x80(dir=device2host,type=standard,recepient=dev)
	//	brequest=6(get descriptor)
	//	wvalue=0x100,low byte=0(index),high byte=1(type)
	//	windex=0
	//	wlength=8
	//	cycle bit=current producer cycle state
	//iii.advance endpoint0 transfer ring enqueue pointer
	//iv.init data stage td
	//	trbtype=data stage trb
	//	dir=1
	//	trb transfer length=8
	//	chain bit=0
	//	ioc=0
	//	idt=0
	//	data buffer pointer=&device descriptor receive buffer
	//	cycle bit=current producer cycle state
	//v.advance endpoint0 transfer ring enqueue pointer
	//vi.init status stage td
	//	trbtype=status stage trb
	//	dir=0
	//	length=0
	//	ioc=1
	//	idt=0
	//	data buffer pointer=0
	//	cycle bit=current producer cycle state
	//vii.advance endpoint0 transfer ring enqueue pointer
	//viii.ring device slot doorbell(control ep0 enqueue pointer update)
	//vx.update ep0 context max packet size with wmaxpacketsize
	//x.issue evaluate context command,use updated max packet size

	//--------------------------------------------




	//read complete usb device descriptor
	say("8.get descriptor2",0);
	//------------------------------------




	//evaluate context command,inform xhc of the value of hub and max....
	say("9.evaluate context",0);




	//class driver 4.3.5
	say("9.set configuration",0);



	//if required configure alternate interface,4.3.6
	say("10.alternate interface",0);




	//pipe fully operational
	say("done!",0);
}
else			//是detach
{
	say("3.device detach!",0);

	say("sending command:disable slot",0);
}

say("}",0);
}




void probeport()
{
	QWORD portnum;
	DWORD portsc;

	for(portnum=1;portnum<8;portnum++)
	{
		portsc=*(DWORD*)(portbase+portnum*0x10-0x10);
		if( (portsc&0x1) == 0x1)	//里面有东西
		{
			initport(portnum);
		}
	}
}




void initxhci()
{
	//clear home
        QWORD addr;
	addr=xhcihome;
	for(;addr<xhcihome+0x100000;addr++) *(BYTE*)addr=0;

	//pci部分
        findaddr();
        if(portaddr==0) return;
        probepci();
        if(memaddr==0) return;

	//xhci
	probexhci();

	//port
	probeport();

	say("",0);
}




void realisr20()
{
shout("oh!interrupt!",0);
shout("{",0);

	QWORD status=*(DWORD*)(operational+4);
	shout("    status:",0);
	if( (status&0x8) == 0x8)
	{
		//清理EINT
		*(DWORD*)(operational+4)=status|0x8;

		//检查IMAN.IP
		DWORD iman=*(DWORD*)(runtime+0x20);
		shout("    IMAN:",iman);

		//检查ERDP.EHB
		DWORD erdp=*(DWORD*)(runtime+0x38);
		shout("    ERDP:",erdp);
		if( (erdp&0x8) == 0x8)
		{
			//取一条event,看看是啥情况
			QWORD pointer=erdp&0xfffffffffffffff0;
			QWORD trbtype=*(DWORD*)(pointer+0xc);
			trbtype=(trbtype>>10)&0x3f;
			shout("    trbtype:",trbtype);

			//设备插入拔出
			if(trbtype == 0x22)
			{
				//哪个端口改变了
				QWORD portid=(*(DWORD*)pointer) >> 24;
				shout("    port id:",portid);

				QWORD portaddr=portbase+portid*0x10-0x10;
				shout("    port addr:",portaddr);

				//到改变的地方看看
				QWORD portsc=*(DWORD*)portaddr;
				shout("    port status:",portsc);

				//告诉主控，收到变化,bit17写1
				*(DWORD*)portaddr=portsc;
			}
			if(trbtype == 0x21)
			{
				eventsignal=0xffff;
				eventaddr=pointer;
			}

			*(DWORD*)(runtime+0x38)=erdp+0x10;
			*(DWORD*)(runtime+0x3c)=0;
		}
	}

shout("}",0);
}
