#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long
#define xhcihome 0x300000

#define ersthome xhcihome+0
#define dcbahome xhcihome+0x10000
#define eventringhome xhcihome+0x20000
#define cmdringhome xhcihome+0x30000

#define slothome xhcihome+0x40000	//+0:		in context
					//+1000:	out context
					//+2000:	ep0 ring
					//+3000:	e1.1 ring
					//+4000:	ep0 buffer
					//+5000:	ep1.1 buffer


static QWORD portaddr=0;
static QWORD memaddr=0;

volatile static QWORD operational;
volatile static QWORD doorbell;
static QWORD runtime;
static QWORD contextsize;

static QWORD portbase;
static QWORD portcount;
static QWORD usb3start;
static QWORD usb3count;
static QWORD usb2start;
static QWORD usb2count;





void findaddr()
{
        QWORD addr=0x4004;
        DWORD temp;
        for(;addr<0x5000;addr+=0x10)
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
	DWORD type;
	DWORD next;

	out32(0xcf8,portaddr+0x34);
	offset=in32(0xcfc)&0xff;

	while(1)
	{
	out32(0xcf8,portaddr+offset);
	temp=in32(0xcfc);
	type=temp&0xff;
	next=(temp>>8)&0xff;

	switch(type)
	{
		case 0x5:
		{
			out32(0xcf8,portaddr+offset);
			temp=in32(0xcfc);
			if( (temp&0x800000) ==0 )
			{
			say("32bit msi@",offset);

			//before
			out32(0xcf8,portaddr+offset);
			say("    control:",in32(0xcfc));
			out32(0xcf8,portaddr+offset+4);
			say("    addr:",in32(0xcfc));
			out32(0xcf8,portaddr+offset+8);
			say("    data:",in32(0xcfc));

			//do something
			out32(0xcf8,portaddr+offset+8);
			out32(0xcfc,0x20);
			out32(0xcf8,portaddr+offset+4);
			out32(0xcfc,0xfee00000);
			out32(0xcf8,portaddr+offset);
			out32(0xcfc,temp|0x10000);

			//after
			out32(0xcf8,portaddr+offset);
			say("    control:",in32(0xcfc));
			out32(0xcf8,portaddr+offset+4);
			say("    addr:",in32(0xcfc));
			out32(0xcf8,portaddr+offset+8);
			say("    data:",in32(0xcfc));
			}
			else
			{
			say("64bit msi@",offset);

			out32(0xcf8,portaddr+offset);
			say("    control:",in32(0xcfc));
			out32(0xcf8,portaddr+offset+4);
			say("    addr:",in32(0xcfc));
			out32(0xcf8,portaddr+offset+0xc);
			say("    data:",in32(0xcfc));

			out32(0xcf8,portaddr+offset+0xc);
			out32(0xcfc,0x20);
			out32(0xcf8,portaddr+offset+4);
			out32(0xcfc,0xfee00000);
			out32(0xcf8,portaddr+offset);
			out32(0xcfc,temp|0x10000);

			out32(0xcf8,portaddr+offset);
			say("    control:",in32(0xcfc));
			out32(0xcf8,portaddr+offset+4);
			say("    addr:",in32(0xcfc));
			out32(0xcf8,portaddr+offset+0xc);
			say("    data:",in32(0xcfc));
			}
			break;
		}

		case 0x11:
		{
			say("msix@",offset);
break;
			DWORD msixcontrol;
			DWORD* msixtable;
			DWORD* pendingtable;

			//get mmio addr,msixtable addr,pendingtable addr
			out32(0xcf8,portaddr+0x10);
			memaddr=in32(0xcfc)&0xfffffff0;

			out32(0xcf8,portaddr+offset+8);
			pendingtable=(DWORD*)( memaddr+in32(0xcfc) );
			out32(0xcf8,portaddr+offset+4);
			msixtable=(DWORD*)( memaddr+in32(0xcfc) );
			out32(0xcf8,portaddr+offset);
			msixcontrol=in32(0xcfc);
			out32(0xcf8,portaddr+offset);
			out32(0xcfc,msixcontrol|0x80000000);
			out32(0xcf8,portaddr+offset);
			msixcontrol=in32(0xcfc);

			say("    msix control:",msixcontrol);
			say("    msix table@",msixtable);
			say("    pending table@",pendingtable);

			//msixtable[0],addrlow,addrhigh,vector,control
			msixtable[0]=0xfee00000;
			msixtable[1]=0;
			msixtable[2]=0x20;
			msixtable[3]=msixtable[3]&0xfffffffe;

			say("    @0:",msixtable[0]);
			say("    @4:",msixtable[1]);
			say("    @8:",msixtable[2]);
			say("    @c:",msixtable[3]);

			break;
		}

		default:
		{
			say("unknown type:",type);
		}
	}
	
	if(type == 0) break;	//当前capability类型为0，结束
	if(next < 0x40) break;	//下一capability位置错误，结束
	
	offset=next;
	}
	
say("}",0);

}


QWORD probepci()
{
	//disable pin interrupt+enable bus mastering
	//very important,in qemu-kvm 1.6.2,bus master bit is 0,must set 1
        out32(0xcf8,portaddr+0x4);
        DWORD temp=in32(0xcfc)|(1<<10)|(1<<2);
        out32(0xcf8,portaddr+0x4);
        out32(0xcfc,temp);

        out32(0xcf8,portaddr+0x4);
        say("pci status and command:",(QWORD)in32(0xcfc));

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
void supportedprotocol(QWORD addr)
{
	QWORD first=(*(DWORD*)addr) >> 16;
	QWORD third=*(DWORD*)(addr+8);

	if(first>=0x300)	//usb3
	{
		usb3start=third & 0xff;
		usb3count=(third>>8) & 0xff;

		say("    usb3:",first);
		say("    firstport:",usb3start);
		say("    count:",usb3count);
	}
	else if(first>=0x200)	//usb2
	{
		usb2start=third & 0xff;
		usb2count=(third>>8) & 0xff;

		say("    usb2:",first);
		say("    firstport:",usb2start);
		say("    count:",usb2count);
	}
	else
	{
		say("usb1?",0);
	}
}




void explainxecp(QWORD addr)
{
	DWORD temp;

	say("explain xecp@",addr);
	say("{",0);
	while(1)
	{
		say("    @",addr);
		temp=*(DWORD*)addr;
		BYTE type=temp&0xff;
		QWORD next=(temp>>6)&0x3fc;

		say("    cap:",type);
		switch(type)
		{
			case 1:{
				if( ownership(addr) < 0 ) goto failed;
				else break;
			}
			case 2:{
				supportedprotocol(addr);
				break;
			}
		}

		if(next == 0) break;
		addr+=next;
	}


failed:
	say("}",0);
}



int probexhci()
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
	doorbell=memaddr+(*(DWORD*)(memaddr+0x14));
	runtime=memaddr+(*(DWORD*)(memaddr+0x18));
	portbase=operational+0x400;
	portcount=hcsparams1>>24;

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
	say("    doorbell@",doorbell);
	say("    runtime@",runtime);
	say("    portbase@",portbase);
	say("    portcount:",portcount);

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

	//蛋疼的计算pagesize
	QWORD psz=*(DWORD*)(operational+8);
	while(1)
	{
		if(psz == 0){
			say("psz:",psz);
			return -1;
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
			say("    not stop",0);
			return -2;
		}
	}

	//按下复位按钮
	usbcommand=*(DWORD*)operational;
	*(DWORD*)operational=usbcommand|2;

	//等一会
	QWORD wait2=0xfffffff;
	for(;wait2>0;wait2--) asm("nop");

	usbcommand=*(DWORD*)(operational);
	if((usbcommand&0x2) == 2)
	{
		say("    reset failed:",usbcommand);
		return -3;
	}

	//controller not ready=1就是没准备好
	usbstatus=*(DWORD*)(operational+4);
	if( (usbstatus&0x800) == 0x800 )
	{
		say("    controller not ready:",usbstatus);
		return -4;
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
	*(DWORD*)(operational+0x30)=dcbahome;
	*(DWORD*)(operational+0x34)=0;
	say("    dcbaa:",*(DWORD*)(operational+0x30) );

	//scratchpad
	//*(DWORD*)dcbahome=dcbahome+0x8000;
	//say("    scratchpad:",dcbahome+0x8000);

	//command linktrb:lastone point to firstone
	*(QWORD*)(cmdringhome+255*0x10)=cmdringhome;
	*(QWORD*)(cmdringhome+255*0x10+8)=0;
	*(QWORD*)(cmdringhome+255*0x10+0xc)=(6<<10)+2;

	//crcr
	*(DWORD*)(operational+0x18)=cmdringhome+1;
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

	//----------------------if(msix)--------------------------
	//----------------------if(msi)--------------------------
	//----------------------if(intx)--------------------------




//--------------------4.eventring---------------------------
//------------------------------------------------------
say("4.eventring:",0);

	//-------------define event ring-----------
	//build the "event ring segment table"
	*(DWORD*)(ersthome)=eventringhome;
	*(DWORD*)(ersthome+0x8)=0x100;

	//ERSTSZ
	*(DWORD*)(runtime+0x28)=1;

	//ERSTBA
	*(DWORD*)(runtime+0x30)=ersthome;
	*(DWORD*)(runtime+0x34)=0;	//这一行必须有

	//ERDP
	*(DWORD*)(runtime+0x38)=eventringhome;
	*(DWORD*)(runtime+0x3c)=0;	//这一行也必须有

	//enable EWE|HSEIE|INTE(0x400|0x8|0x4) in USBCMD
	*(DWORD*)operational = (*(DWORD*)operational) | 0xc;
	//*(DWORD*)operational |= 0x40c;

	//IMOD
	*(DWORD*)(runtime+0x24)=4000;

	//IMAN
	*(DWORD*)(runtime+0x20) = (*(DWORD*)(runtime+0x20)) | 0x2;

	say("    event ring done",0);




//---------------------xhci启动---------------------
//write usbcmd,turn host controller on
//-------------------------------------------------
say("5.turnon",0);
	//turn on
	*(DWORD*)operational = (*(DWORD*)operational) | 0x1;

	//wait for 100ms?
	QWORD wait3=0xfffffff;
	while(wait3--)asm("nop");

	//
	say("    command:",*(DWORD*)operational);
	say("    status:",*(DWORD*)(operational+4));

say("}",0);
return 0;
}




void clear(QWORD addr,QWORD size)
{
	BYTE* pointer=(BYTE*)addr;
	int i;
	for(i=0;i<size;i++) pointer[i]=0;
}




//直接在command ring上写TRB（1个TRB=4个DWORD）,然后处理enqueue指针，最后ring
static QWORD commandenqueue=cmdringhome;
static DWORD commandcycle=1;
void command(DWORD d0,DWORD d1,DWORD d2,DWORD d3)
{
	DWORD* pointer=(DWORD*)commandenqueue;
	//say("command ring@",commandenqueue);

	//写ring
	pointer[0]=d0;
	pointer[1]=d1;
	pointer[2]=d2;
	pointer[3]=d3;

	//enqueue指针怎么变
	commandenqueue+=0x10;

	//处理完了，敲门铃
	*(DWORD*)doorbell=0;
}




struct trb{
	QWORD addr;
	DWORD d0;
	DWORD d1;
	DWORD d2;
	DWORD d3;
};
static struct trb trb;
void writetrb()
{
	//得到这次往哪儿写
	QWORD temp=*(QWORD*)(trb.addr+0xff0);
	DWORD* pointer=(DWORD*)(trb.addr+temp);
	//say("transfer ring@",&pointer[0]);

	//下次往哪儿写
	temp+=0x10;
	if(temp>0xf00)	temp=0;
	*(QWORD*)(trb.addr+0xff0)=temp;

	pointer[0]=trb.d0;
	pointer[1]=trb.d1;
	pointer[2]=trb.d2;
	pointer[3]=trb.d3;
}




void ring(int slot,int endpoint)
{
	*(DWORD*)(doorbell+4*slot)=endpoint;
}



struct setup{
	BYTE bmrequesttype;	//	0x80
	BYTE brequest;		//	0x6
	WORD wvalue;		//	(descriptor type>>8)+descriptor index
	WORD windex;		//	0
	WORD wlength;		//	length

	QWORD addr;
	QWORD data;
};
static struct setup packet;
void controltransfer()
{
if(packet.bmrequesttype>=0x80)
{
	//setup stage
	trb.addr=packet.addr;
	trb.d0=(packet.wvalue<<16)+(packet.brequest<<8)+packet.bmrequesttype;
	trb.d1=(packet.wlength<<16)+packet.windex;
	trb.d2=8;
	trb.d3=(3<<16)+(2<<10)+(1<<6)+1;
	writetrb();

	//data stage
	trb.d0=packet.data;
	trb.d1=0;
	trb.d2=packet.wlength;
	trb.d3=(1<<16)+(3<<10)+1;
	writetrb();

	//status stage
	trb.d0=trb.d1=trb.d2=0;
	trb.d3=(4<<10)+(1<<5)+1;
	writetrb();
}
else
{
	//setup stage
	trb.addr=packet.addr;
	trb.d0=(packet.wvalue<<16)+(packet.brequest<<8)+packet.bmrequesttype;
	trb.d1=(packet.wlength<<16)+packet.windex;
	trb.d2=8;
	trb.d3=(2<<10)+(1<<6)+1;
	writetrb();

	//status stage
	trb.d0=trb.d1=trb.d2=0;
	trb.d3=(1<<16)+(4<<10)+(1<<5)+1;	//in
	writetrb();
}
}




struct context{		//传参太麻烦...
	QWORD addr;	//目的context地址头
	int dci;	//偏移多少个context(每个context0x20或者0x40字节)
	DWORD d0;
	DWORD d1;
	DWORD d2;
	DWORD d3;
	DWORD d4;
	DWORD d5;
	DWORD d6;
	DWORD d7;
};
static struct context context;
void writecontext()
{
	DWORD* pointer=(DWORD*)(context.addr+context.dci*contextsize);

	pointer[0]=context.d0;
	pointer[1]=context.d1;
	pointer[2]=context.d2;
	pointer[3]=context.d3;
	pointer[4]=context.d4;
}




volatile static QWORD eventsignal;
volatile static QWORD eventaddr;
int waitevent(QWORD trbtype)
{
	QWORD timeout=0;
	DWORD* p;
	while(1)
	{
		timeout++;
		if(timeout>0xfffffff){
			say("    timeout!",0);
			return -1;
		}

		if(eventsignal!=0)
		{
			eventsignal=0;

			p=(DWORD*)eventaddr;
			if( ( (p[3]>>10)&0x3f) == trbtype)
			{
				if( (p[2]>>24) != 1)
				{
					say("    error:",p[2]>>24);
					return -2;
				}
				else
				{
					return 1;
				}
			}
		}
	}
}




//以下是提取自descriptor的最重要信息，每次只被这个函数更新,就像人记笔记......
static QWORD classcode;
static QWORD interfaceclass;
static QWORD interval;
static QWORD reportsize;
void explaindescriptor(QWORD addr)
{
	DWORD type=*(BYTE*)(addr+1);
	if(	(type==0)|((type>7)&(type<0x21))|(type>0x23)	) return;

	say("	@",addr);

	if(type==1)	//设备描述符
	{
	//这是第一个被读取的描述符，所以几个变量在这里清零最好
	//classcode=0;	//这个就不必了，马上就变掉
	interfaceclass=0;
	interval=0;

	say("    blength:",*(BYTE*)addr);
	say("    bdescriptortype:",*(BYTE*)(addr+1));
	say("    bcdusb:",*(WORD*)(addr+2));
	classcode=*(BYTE*)(addr+4);
	say("    bdeviceclass:",classcode);
	say("    bdevicesubclass:",*(BYTE*)(addr+5));
	say("    bdeviceprotocol:",*(BYTE*)(addr+6));
	say("    bmaxpacketsize0:",*(BYTE*)(addr+7));
	say("    vendor:",*(WORD*)(addr+8));
	say("    product:",*(WORD*)(addr+0xa));
	say("    bcddevice:",*(WORD*)(addr+0xc));
	say("    imanufacturer:",*(BYTE*)(addr+0xe));
	say("    iproduct:",*(BYTE*)(addr+0xf));
	say("    iserialnumber:",*(BYTE*)(addr+0x10));
	say("    bnumconfigurations:",*(BYTE*)(addr+0x11));
	}

	if(type==2)	//配置描述符
	{
	say("    blength:",*(BYTE*)addr);
	say("    bdescriptortype:",*(BYTE*)(addr+1));
	say("    wtotallength:",*(WORD*)(addr+2));
	say("    bnuminterface:",*(BYTE*)(addr+4));
	say("    bconfigurationvalue:",*(BYTE*)(addr+5));
	say("    iconfiguration:",*(BYTE*)(addr+6));
	say("    bmattributes:",*(BYTE*)(addr+7));
	say("    bmaxpower:",*(BYTE*)(addr+8));

	DWORD totallength=*(WORD*)(addr+2);
	DWORD offset=0;
	while(1)
	{
		if(offset>totallength) break;
		if( *(BYTE*)(addr+offset) <8) break;
		offset+=*(BYTE*)(addr+offset);
		explaindescriptor(addr+offset);
	}
	}
	if(type==4)	//接口描述符
	{
	say("    blength:",*(BYTE*)addr);
	say("    bdescriptortype:",*(BYTE*)(addr+1));
	say("    binterfacenumber:",*(BYTE*)(addr+2));
	say("    balternatesetting:",*(BYTE*)(addr+3));
	say("    bnumendpoints:",*(BYTE*)(addr+4));
	interfaceclass=*(BYTE*)(addr+5);
	say("    binterfaceclass:",interfaceclass);
	say("    binterfacesubclass:",*(BYTE*)(addr+6));
	say("    binterfaceprotol:",*(BYTE*)(addr+7));
	say("    iinterface:",*(BYTE*)(addr+8));
	}

	if(type==5)	//端点描述符
	{
	say("    blength:",*(BYTE*)addr);
	say("    bdescriptortype:",*(BYTE*)(addr+1));

	BYTE endpoint=*(BYTE*)(addr+2);
	say("    endpoint:",endpoint&0xf);
	if(endpoint>0x80){say("    in",0);}
	else{say("    out",0);}

	BYTE eptype=*(BYTE*)(addr+3);
	if(eptype==0) say("    control",0);
	else if(eptype==1) say("    isochronous",0);
	else if(eptype==2) say("    bulk",0);
	else say("    interrupt",0);

	say("    wmaxpacketsize:",*(WORD*)(addr+4));
	interval=*(BYTE*)(addr+6);
	say("    binterval:",interval);
	}

/*
	if(type==6)	//设备限定描述符
	{
	say("    blength:",*(BYTE*)addr);
	say("    bdescriptortype:",*(BYTE*)(addr+1));
	say("    bcdusb:",*(WORD*)(addr+2));
	say("    bdeviceclass:",*(BYTE*)(addr+4));
	say("    bdevicesubclass:",*(BYTE*)(addr+5));
	say("    bdeviceprotocol:",*(BYTE*)(addr+6));
	say("    bmaxpacketsize0:",*(BYTE*)(addr+7));
	say("    bnumconfigurations:",*(BYTE*)(addr+8));
	}
	if(type==7)	//其他速率配置描述符
	{
	say("    blength:",*(BYTE*)addr);
	say("    bdescriptortype:",*(BYTE*)(addr+1));
	say("    wtotallength:",*(WORD*)(addr+2));
	say("    bnuminterfaces:",*(BYTE*)(addr+4));
	say("    bconfigurationvalue:",*(BYTE*)(addr+5));
	say("    iconfiguration:",*(BYTE*)(addr+6));
	say("    bmattributes:",*(BYTE*)(addr+7));
	say("    bmaxpower:",*(BYTE*)(addr+8));
	}
*/
	if(type==0x21)	//hid设备描述符
	{
	QWORD thistype;

	say("    blength:",*(BYTE*)addr);
	say("    bdescriptortype:",*(BYTE*)(addr+1));
	say("    bcdhid:",*(WORD*)(addr+2));
	say("    bcountrycode:",*(BYTE*)(addr+4));
	say("    bnumdescriptor:",*(BYTE*)(addr+5));

	thistype=*(BYTE*)(addr+6);
	if(thistype==0x22) reportsize=*(WORD*)(addr+7);
	say("    bdescriptortype:",thistype);
	say("    wdescriptorlength:",*(WORD*)(addr+7));

	thistype=*(BYTE*)(addr+9);
	if(thistype==0x22) reportsize=*(WORD*)(addr+10);
	say("    bdescriptortype:",*(BYTE*)(addr+9));
	say("    wdescriptorlength:",*(WORD*)(addr+10));
	}
}




void explainhidreport(QWORD addr)
{
say("	@",addr);

BYTE* p=(BYTE*)addr;	
int i=0;
BYTE bsize;
BYTE btype;
BYTE btag;

while(1)
{
	//哪些不正常情况跳出循环
	if(p[i]==0) break;
	if(p[i]==0xfe)
	{
		say("    cannot understand long item",0);
		break;
	}

	//变量
	bsize=p[i]&0x3;
	btype=(p[i]>>2)&0x3;
	btag=(p[i]>>4)&0xf;

	//看看这一块数据什么意思
	if(btype==0)
	{
		if(btag==8) say("main.input:",p[i+1]);
		else if(btag==9) say("main.output:",p[i+1]);
		else if(btag==0xa) say("main.collection:",p[i+1]);
		else if(btag==0xb) say("main.feature",0);
		else if(btag==0xc) say("main.endcollection",0);
		else say("unknown tag:",btag);
	}
	if(btype==1)
	{
		if(btag==0) say("global.usagepage:",p[i+1]);
		else if(btag==1) say("global.logicalminimum:",p[i+1]);
		else if(btag==2) say("global.logicalmaximum:",p[i+1]);
		else if(btag==3) say("global.physicalminimum:",p[i+1]);
		else if(btag==4) say("global.physicalmaximum:",p[i+1]);
		else if(btag==5) say("global.unitexponent:",p[i+1]);
		else if(btag==6) say("global.unit:",p[i+1]);
		else if(btag==7) say("global.reportsize:",p[i+1]);
		else if(btag==8) say("global.reportid:",p[i+1]);
		else if(btag==9) say("global.reportcount:",p[i+1]);
		else if(btag==0xa) say("global.push:",p[i+1]);
		else if(btag==0xb) say("global.pop:",p[i+1]);
		else say("unknown tag:",btag);
	}
	if(btype==2)
	{
		if(btag==0) say("    local.usage:",p[i+1]);
		else if(btag==1) say("    local.usageminimum:",p[i+1]);
		else if(btag==2) say("    local.usagemaximum:",p[i+1]);
		else if(btag==3) say("    local.desinatorindex:",p[i+1]);
		else if(btag==4) say("    local.desinatorminimum:",p[i+1]);
		else if(btag==5) say("    local.desinatormaximum:",p[i+1]);
		else if(btag==7) say("    local.stringindex:",p[i+1]);
		else if(btag==8) say("    local.stringminimum:",p[i+1]);
		else if(btag==9) say("    local.stringmaximum:",p[i+1]);
		else if(btag==0xa) say("    local.delimiter:",p[i+1]);
		else say("unknown tag:",btag);
	}

	//准备下一个循环
	i+=1;
	if(bsize==3) i+=4;
	else i+=bsize;
}
}


/*
	switch(p[i])
	{
		case 5:{
		say("    usage page:",p[i+1]);
		break;
		}
		case 9:{
		say("    usage:",p[i+1]);
		break;
		}
		case 0xa1:{
		say("    collection:",p[i+1]);
		break;
		}
		case 0x19:{
		say("    usage minimum:",p[i+1]);
		break;
		}
		case 0x29:{
		say("    usage maximum:",p[i+1]);
		break;
		}
		case 0x15:{
		say("    logical maximum:",p[i+1]);
		break;
		}
		case 0x25:{
		say("    logical maximum:",p[i+1]);
		break;
		}
		case 0x95:{
		say("    report count:",p[i+1]);
		break;
		}
		case 0x75:{
		say("    report size:",p[i+1]);
		break;
		}
		case 0x81:{
		say("    input:",p[i+1]);
		break;
		}
		case 0xc0:{
		say("    finish",0);
		return;
		}
		default:{
		say("    unknown@",&p[i]);
		break;
		}
	}
*/


void checkport(portnum)
{
	DWORD portsc=*(DWORD*)(portbase+portnum*0x10-0x10);
	if( (portsc&0x1) != 0x1){
		say("nothing in port:",portnum);
		return;
	}




	say("port:",portnum);
	say("{",0);




	//-----------we only know these for now-----------
	say("0.information:",0);
	say("    portsc(before):",portsc);
	

	if( (portnum>=usb3start) && (portnum<usb3start+usb3count) )
	{
		say("    3.0",0);
	}
	if( (portnum>=usb2start) && (portnum<usb2start+usb2count) )
	{
		say("    2.0(must reset twice,why?)",0);

		*(DWORD*)(portbase+portnum*0x10-0x10)=portsc|0x10;
		waitevent(0x22);
		portsc=*(DWORD*)(portbase+portnum*0x10-0x10);
		say("    status(reset1):",portsc);

		*(DWORD*)(portbase+portnum*0x10-0x10)=portsc|0x10;
		waitevent(0x22);
		portsc=*(DWORD*)(portbase+portnum*0x10-0x10);
		say("    status(reset2):",portsc);
	}

	portsc=*(DWORD*)(portbase+portnum*0x10-0x10);
	say("    status(after):",portsc);
	DWORD pls=(portsc>>5)&0xf;
	DWORD speed=(portsc>>10)&0xf;
	say("    pls:",pls);
	say("    speed:",speed);
	//----------------------------------------




	//---------------obtain slot------------------
	say("1.requesting slot",0);

	command(0,0,0, (9<<10)+commandcycle );
	if(waitevent(0x21)<0) goto failed;

	QWORD slot=(*(DWORD*)(eventaddr+0xc)) >> 24;
	if(slot>=0x18){
		say("    bad slotnum",slot);
		goto failed;
	}
	say("    obtained slot:",slot);
	//-------------------------------------------




	//-------------slot initialization----------------
	QWORD inputaddr=slothome+slot*0x8000;
	QWORD outputaddr=slothome+slot*0x8000+0x1000;
	QWORD controladdr=slothome+slot*0x8000+0x2000;
	QWORD intaddr=slothome+slot*0x8000+0x3000;
	QWORD buffer=slothome+slot*0x8000+0x4000;
	QWORD hidbuffer=slothome+slot*0x8000+0x5000;

	say("2.initing structure",0);
	say("    buffer:",buffer);
	say("    hidbuffer:",hidbuffer);
	say("    inputaddr:",inputaddr);
	say("    outputaddr:",outputaddr);
	say("    controladdr:",controladdr);
	say("    intaddr:",intaddr);

	//1)clear context
	clear(slothome+slot*0x8000,0x8000);
	//construct input
	context.addr=inputaddr;
	context.dci=0;			//input context
	context.d0=0;
	context.d1=3;
	context.d2=0;
	context.d3=0;
	context.d4=0;
	writecontext();

	context.dci=1;			//slot context
        context.d0=(3<<27)+(speed<<20);
	context.d1=portnum<<16;
	writecontext();

	context.dci=2;			//ep0 context
	context.d0=0;
	context.d1=(64<<16)+(4<<3)+6;
	context.d2=controladdr+1;
	context.d3=0;
	context.d4=0x40;
	writecontext();

	//output context地址填入对应dcbaa
	*(DWORD*)(dcbahome+slot*8)=outputaddr;
	*(DWORD*)(dcbahome+slot*8+4)=0;
	//------------------------------------




	//-----------address device-----------------
	say("3.initing slot",0);


	say("    address device",0);
	command(inputaddr,0,0, (slot<<24)+(11<<10)+commandcycle );
	if(waitevent(0x21)<0) goto failed;

	DWORD slotstate=(*(DWORD*)(outputaddr+0xc))>>27; //if2,addressed
	DWORD epstate=(*(DWORD*)(outputaddr+0x20))&0x3;
	if(slotstate==2) say("    slot addressed!",0);
	else say("    slot state:",slotstate);
	if(epstate==0){
		say("    ep0 wrong",0);
		goto failed;
	}

	//first read(to obtain maxsize)
	packet.bmrequesttype=0x80;
	packet.brequest=6;
	packet.wvalue=0x100;
	packet.windex=0;
	packet.wlength=8;
	packet.addr=controladdr;
	packet.data=buffer;
	controltransfer();
	ring(slot,1);
	if(waitevent(0x20)<0) goto failed;

	DWORD packetsize=*(BYTE*)(buffer+7);
	say("    got packetsize:",packetsize);

	//evaluate
	context.addr=inputaddr;
	context.dci=2;			//ep0 context
	context.d0=0;
	context.d1=(packetsize<<16)+(4<<3)+6;
	context.d2=controladdr+1;
	context.d3=0;
	context.d4=0x40;
	writecontext();

	command(inputaddr,0,0, (slot<<24)+(13<<10)+commandcycle );
	if(waitevent(0x21)<0) goto failed;

	slotstate=(*(DWORD*)(outputaddr+0xc))>>27; //if2,addressed
	epstate=(*(DWORD*)(outputaddr+0x20))&0x3;
	if(slotstate==2) say("    slot evaluated!",0);
	else say("    slot state:",slotstate);
	if(epstate==0){
		say("    ep0 wrong",0);
		goto failed;
	}
	//--------------------------------------------




	//------------------get descriptor-----------------
	say("4.now we know everything:",0);


	//[buffer]:device descriptor
	packet.bmrequesttype=0x80;
	packet.brequest=6;
	packet.wvalue=0x100;
	packet.windex=0;
	packet.wlength=0x12;
	packet.addr=controladdr;
	packet.data=buffer;
	controltransfer();
	ring(slot,1);
	if(waitevent(0x20)<0) goto failed;
	explaindescriptor(buffer);


	//[buffer+0x100]:configure descriptor
	packet.bmrequesttype=0x80;
	packet.brequest=6;
	packet.wvalue=0x200;
	packet.windex=0;
	packet.wlength=0x9;
	packet.addr=controladdr;
	packet.data=buffer+0x100;
	controltransfer();
	ring(slot,1);
	if(waitevent(0x20)<0) goto failed;
	packet.wlength=*(WORD*)(buffer+0x102);
	controltransfer();
	ring(slot,1);
	if(waitevent(0x20)<0) goto failed;
	explaindescriptor(buffer+0x100);


	//[buffer+0x200]:string descriptors
	packet.bmrequesttype=0x80;
	packet.brequest=6;
	packet.wvalue=0x300;
	packet.windex=0;
	packet.wlength=0xff;
	packet.addr=controladdr;
	packet.data=buffer+0x200;
	controltransfer();
	ring(slot,1);
	if(waitevent(0x20)<0) goto failed;

	packet.windex=*(WORD*)(buffer+0x202);
	DWORD temp=1;
	for(;temp<8;temp++)		//所有字符串描述符
	{
		packet.wvalue=0x300+temp;
		packet.data=buffer+0x200+temp*0x40;
		controltransfer();
		ring(slot,1);
		waitevent(0x20);	//不用管成功失败
	}
	say("string descriptors@",buffer+0x200);
	//--------------------------------------------




	//---------------configure------------------
	//上一步得到的最重要信息
	say("important infomation:",0);
	say("    classcode:",classcode);
	say("    interfaceclass:",interfaceclass);
	say("    interval:",interval);
	say("    reportsize:",reportsize);

	//现在只管hid设备
	if(classcode != 0) goto failed;
	if(interfaceclass != 3) goto failed;

	say("5.hid!:",0);

	packet.bmrequesttype=0x81;
	packet.brequest=6;
	packet.wvalue=0x2200;
	packet.windex=0;
	packet.wlength=reportsize;
	packet.addr=controladdr;
	packet.data=buffer+0x400;
	controltransfer();
	ring(slot,1);
	if(waitevent(0x20)<0) goto failed;
	explainhidreport(buffer+0x400);
	//get report
	//packet.bmrequesttype=0xa1;
	//packet.brequest=1;
	//packet.windex=0;
	//packet.wlength=6;
	//packet.addr=controladdr;
	//for(temp=0;temp<4;temp++)		//所有字符串描述符
	//{
	//	packet.wvalue=(temp+1)*0x100;
	//	packet.data=buffer+0x400+temp*0x40;
	//	controltransfer();
	//	ring(slot,1);
	//	if(waitevent(0x20)<0) goto failed;
	//}
	//explaindescriptor(buffer+0x400);




	//change input context&ep1.1 context
	context.addr=inputaddr;
	context.dci=0;			//input context
        context.d0=0;
	context.d1=9;
	context.d2=0;
	context.d3=0;
	context.d4=0;
	writecontext();

	if(interval<=2) interval=3;
	context.dci=4;			//ep1.1 context
	context.d0=interval<<16;
	context.d1=(8<<16)+(7<<3)+(3<<1);
	context.d2=intaddr+1;
	context.d3=0;
	context.d4=0x80008;
	writecontext();


	//ok,sent command
	command(inputaddr,0,0, (slot<<24)+(12<<10)+commandcycle );
	if(waitevent(0x21)<0) goto failed;

	slotstate=(*(DWORD*)(outputaddr+0xc))>>27;
	epstate=(*(DWORD*)(outputaddr+0x60))&0x3;
	if(slotstate==3) say("    slot configured!",0);
	else say("    slot state:",slotstate);
	if(epstate==0){
		say("    ep3 wrong",0);
		goto failed;
	}


	//set configuration:0x0000,0000,0001,09,00
	packet.bmrequesttype=0;
	packet.brequest=9;
	packet.wvalue=1;
	packet.windex=0;
	packet.wlength=0;
	packet.addr=controladdr;
	controltransfer();
	ring(slot,1);
	if(waitevent(0x20)<0) goto failed;
	say("    device configured",0);
	//------------------------------------


	//----------prepare ep1.1 ring-------------
	//[0,3f0)第一段正常trb
	trb.addr=intaddr;
	trb.d1=0;
	trb.d2=6;
	trb.d3=0x25+(1<<10);
	for(temp=0;temp<0x3f;temp++)
	{
		trb.d0=hidbuffer+temp*0x10;
		writetrb();
	}
	//[3f0,400)第一段结尾link trb指向[0,3f0)
	trb.addr=intaddr;
	trb.d0=intaddr;
	trb.d1=0;
	trb.d2=6;
	trb.d3=1+(6<<10);
	writetrb();

	//敲门铃，开始执行
	ring(slot,3);
	//------------------------------------------




	failed:
	say("}",0);
}




void probeport()
{
	QWORD portnum;
	DWORD portsc;

	for(portnum=1;portnum<=portcount;portnum++)
	{
		checkport(portnum);
	}
}




void initxhci()
{
	//clear home
	clear(xhcihome,0x100000);

	//find pci address
        findaddr();
        if(portaddr==0) goto end;

	//pci部分
        probepci();
        if(memaddr==0) goto end;

	//xhci
	if(probexhci()<0) goto end;

	//port
	probeport();

end:		//就一行空白
	say("",0);
}




explainevent(QWORD addr)
{
shout("event@",eventaddr);
        //trb种类
        QWORD trbtype=*(DWORD*)(addr+0xc);
        trbtype=(trbtype>>10)&0x3f;
        shout("    trbtype:",trbtype);

        switch(trbtype)
        {
                case 0x22:{             //设备插入拔出

                        //哪个端口改变了
                        QWORD portid=(*(DWORD*)addr) >> 24;
                        shout("    port id:",portid);

                        QWORD portaddr=portbase+portid*0x10-0x10;
                        shout("    port addr:",portaddr);

                        //到改变的地方看看
                        QWORD portsc=*(DWORD*)portaddr;
                        shout("    status:",portsc);

                        //告诉主控，收到变化,bit17写1(bit1不能写)
                        *(DWORD*)portaddr=portsc&0xfffffffd;

                        break;
                }
        }
}
void realisr20()
{
shout("int20",0);
shout("{",0);
        //检查ERDP.EHB
        DWORD erdp=*(DWORD*)(runtime+0x38);
        //告诉waitevent函数,中断来了
        eventsignal=1;
        eventaddr=erdp&0xfffffffffffffff0;

        QWORD temp=eventaddr;
        QWORD pcs=(*(DWORD*)(temp+0xc))&0x1;
        while(1)
        {
                explainevent(temp);

                //dequeue pointer update
                temp+=0x10;
                if(temp>=eventringhome+0x1000)
                {
                        say("reload......",erdp);
                        temp=eventringhome;
                }
                if( ((*(DWORD*)(temp+0xc))&0x1) != pcs) break;
        }

        //下一次主控把event写到哪儿
        *(DWORD*)(runtime+0x38)=temp|8;
        *(DWORD*)(runtime+0x3c)=0;

theend:
shout("}",0);
}
