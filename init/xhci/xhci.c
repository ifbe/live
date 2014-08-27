#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long

#define xhcihome 0x300000

#define ersthome xhcihome+0
#define eventhome xhcihome+0x10000

#define crcrhome xhcihome+0x40000

#define contexthome xhcihome+0x80000
#define scratchpadhome xhcihome+0x90000
#define bufferhome xhcihome+0xa0000
#define hidbufferhome xhcihome+0xb0000

#define inputhome xhcihome+0xc0000
#define outputhome xhcihome+0xd0000
#define controlhome xhcihome+0xe0000
#define inthome xhcihome+0xf0000


static QWORD portaddr=0;

static QWORD memaddr=0;
static QWORD msixtable=0;
static QWORD pendingtable=0;

static QWORD operational;
static QWORD doorbell;
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
		BYTE kind=temp&0xff;
		QWORD next=(temp>>6)&0x3fc;

		say("    cap:",kind);
		switch(kind)
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
	*(DWORD*)(operational+0x30)=contexthome;
	*(DWORD*)(operational+0x34)=0;
	say("    dcbaa:",*(DWORD*)(operational+0x30) );

	//scratchpad
	//*(DWORD*)contexthome=scratchpadhome;
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
	QWORD wait3=0xffffff;
	while(wait3--)asm("nop");

	//
	say("    command:",*(DWORD*)operational);
	say("    status:",*(DWORD*)(operational+4));

say("}",0);
}




void clear(QWORD addr,QWORD size)
{
	BYTE* pointer=(BYTE*)addr;
	int i;
	for(i=0;i<size;i++) pointer[i]=0;
}




//第一个DWORD，第二个DWORD，第三个DWORD。。。。
static QWORD commandenqueue=crcrhome;
static DWORD commandcycle=1;
void command(DWORD d0,DWORD d1,DWORD d2,DWORD d3)
{
	DWORD* pointer=(DWORD*)commandenqueue;
	say("command ring@",commandenqueue);

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




//setuppacket
//[0,7]:bmrequesttype	example:	0x80
//[8,15]:brequest			0x6
//[16,31]:wvalue			(descriptor type>>8)+descriptor index
//[32,47]:windex			0
//[48,63]:wlength			length
void controltransfer(int slot,QWORD setuppacket,QWORD buffer)
{
	QWORD temp;

	//从(output)context得到ring地址
	temp=*(QWORD*)(outputhome+slot*0x1000+contextsize+8);
	QWORD ring=temp&0xfffffffffffffff0;
	temp=*(QWORD*)(ring+0xff0);
	DWORD* pointer=(DWORD*)(ring+temp*0x10);

	//
	say("transfer ring@",&pointer[0]);

	//we should analyse the setuppacket
	if((setuppacket&0xff)==0)
	{
		//setup stage
		pointer[0]=setuppacket&0xffffffff;
		pointer[1]=setuppacket>>32;
		pointer[2]=8;
		pointer[3]=0x41+(2<<10);
		//status stage
		pointer[4]=0;
		pointer[5]=0;
		pointer[6]=0;
		pointer[7]=0x21+(4<<10)+(1<<16);	//in

		*(QWORD*)(ring+0xff0)+=2;
	}
	else
	{
		//setup stage
		pointer[0]=setuppacket&0xffffffff;
		pointer[1]=setuppacket>>32;
		pointer[2]=8;
		pointer[3]=0x30041+(2<<10);
		//data stage
		pointer[4]=buffer;
		pointer[5]=0;
		pointer[6]=setuppacket>>48;
		pointer[7]=0x10001+(3<<10);
		//status stage
		pointer[8]=0;
		pointer[9]=0;
		pointer[10]=0;
		pointer[11]=0x21+(4<<10);

		*(QWORD*)(ring+0xff0)+=3;
	}

	//doorbell
	*(DWORD*)(doorbell+4*slot)=1;
}
void transfer(int slot,int endpoint,DWORD d0,DWORD d1,DWORD d2,DWORD d3)
{
	QWORD temp;

	//从(output)context得到ring地址
	temp=*(QWORD*)(outputhome+slot*0x1000+endpoint*contextsize+8);
	QWORD ring=temp&0xfffffffffffffff0;
	temp=*(QWORD*)(ring+0xff0);
	DWORD* pointer=(DWORD*)(ring+temp*0x10);

	say("transfer ring@",&pointer[0]);

	pointer[0]=d0;
	pointer[1]=d1;
	pointer[2]=d2;
	pointer[3]=d3;

	*(DWORD*)(doorbell+4*slot)=endpoint;
}




void context(QWORD addr,int dci,DWORD d0,DWORD d1,DWORD d2,DWORD d3,DWORD d4)
{
	DWORD* pointer=(DWORD*)(addr+dci*contextsize);

	pointer[0]=d0;
	pointer[1]=d1;
	pointer[2]=d2;
	pointer[3]=d3;
	pointer[4]=d4;
}




volatile static QWORD eventsignal=0;
volatile static QWORD eventaddr;
int waitevent(QWORD trbtype)
{
	QWORD timeout=0;
	DWORD* p;
	while(1)
	{
		if(timeout>0xffffffff){
			say("    timeout!",0);
			return -1;
		}
		if(eventsignal==0){
			timeout++;
			continue;
		}

		p=(DWORD*)eventaddr;
		if( ( (p[3]>>10)&0x3f) == trbtype)
		{
			if( (p[2]>>24) != 1)
			{
				say("    error code:",p[2]>>24);
				return -2;
			}
			else
			{
				eventsignal=0;
				return 1;
			}
		}
	}
}




//以下是提取自descriptor的最重要信息，每次只被这个函数更新,就像人记笔记......
//
void explaindescriptor(QWORD addr)
{
	DWORD type=*(BYTE*)(addr+1);
	if(	(type==0)|((type>7)&(type<0x21))|(type>0x23)	) return;

	say("	@",addr);

	if(type==1)	//设备描述符
	{
	say("    blength:",*(BYTE*)addr);
	say("    bdescriptortype:",*(BYTE*)(addr+1));
	say("    bcdusb:",*(WORD*)(addr+2));
	say("    bdeviceclass:",*(BYTE*)(addr+4));
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

	if(type==3)	//字符串描述符
	{
	say("    blength:",*(BYTE*)addr);
	say("    bdescriptortype:",*(BYTE*)(addr+1));
	}

	if(type==4)	//接口描述符
	{
	say("    blength:",*(BYTE*)addr);
	say("    bdescriptortype:",*(BYTE*)(addr+1));
	say("    binterfacenumber:",*(BYTE*)(addr+2));
	say("    balternatesetting:",*(BYTE*)(addr+3));
	say("    bnumendpoints:",*(BYTE*)(addr+4));
	say("    binterfaceclass:",*(BYTE*)(addr+5));
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
	say("    binterval:",*(BYTE*)(addr+6));
	}

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

	if(type==0x21)	//hid设备描述符
	{
	say("    blength:",*(BYTE*)addr);
	say("    bdescriptortype:",*(BYTE*)(addr+1));
	say("    bcdhid:",*(WORD*)(addr+2));
	say("    bcountrycode:",*(BYTE*)(addr+4));
	say("    bnumdescriptor:",*(BYTE*)(addr+5));
	say("    bdescriptortype:",*(BYTE*)(addr+6));
	say("    wdescriptorlength:",*(WORD*)(addr+7));
	say("    bdescriptortype:",*(BYTE*)(addr+9));
	say("    wdescriptorlength:",*(WORD*)(addr+10));

	}
	//if(type==0x21)	//hid报告描述符
	//if(type==0x21)	//hid物理描述符
}




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
	if( (portsc>>16) >0 )
	{
		*(DWORD*)(portbase+portnum*0x10-0x10)=portsc;
	}
	portsc=*(DWORD*)(portbase+portnum*0x10-0x10);
	

	if( (portnum>=usb3start) && (portnum<usb3start+usb3count) )
	{
		say("    3.0",0);
	}
	if( (portnum>=usb2start) && (portnum<usb2start+usb2count) )
	{
		say("    2.0,resetting",0);
		*(DWORD*)(portbase+portnum*0x10-0x10)=portsc|0x10;
		waitevent(0x22);
		portsc=*(DWORD*)(portbase+portnum*0x10-0x10);
	}

	DWORD pls=(portsc>>5)&0xf;
	DWORD speed=(portsc>>10)&0xf;
	say("    status:",portsc);
	say("    pls:",pls);
	say("    speed:",speed);
	//----------------------------------------




	//---------------obtain slot------------------
	say("1.requesting slot",0);

	command(0,0,0, (9<<10)+commandcycle );
	if(waitevent(0x21)<0) goto failed;

	QWORD slot=(*(DWORD*)(eventaddr+0xc)) >> 24;
	if(slot>=0x10){
		say("    bad slotnum",slot);
		goto failed;
	}
	say("    obtained slot:",slot);
	//-------------------------------------------




	//-------------slot initialization----------------
	QWORD buffer=bufferhome+slot*0x1000;
	QWORD hidbuffer=hidbufferhome+slot*0x1000;
	QWORD inputaddr=inputhome+slot*0x1000;
	QWORD outputaddr=outputhome+slot*0x1000;
	QWORD controladdr=controlhome+slot*0x1000;
	QWORD intaddr=inthome+slot*0x1000;

	say("2.initing slot",0);
	say("    buffer:",buffer);
	say("    hidbuffer:",hidbuffer);
	say("    inputaddr:",inputaddr);
	say("    outputaddr:",outputaddr);
	say("    controladdr:",controladdr);
	say("    intaddr:",intaddr);

	//1)clear context
	clear(buffer,0x1000);
	clear(hidbuffer,0x1000);
	clear(inputaddr,0x1000);
	clear(outputaddr,0x1000);
	clear(controladdr,0x1000);
	clear(intaddr,0x1000);

	//construct input
	context(inputaddr,0,        0,	3,	0,	0,	0);
	context(inputaddr,1,        (1<<27)+(speed<<20),portnum<<16,0,0,0);
	context(inputaddr,2,        0,(64<<16)|(4<<3)|6,controladdr+1,0,0x40);

	//output context地址填入对应dcbaa
	*(DWORD*)(contexthome+slot*8)=outputaddr;
	*(DWORD*)(contexthome+slot*8+4)=0;

	//address device(bsr=0)
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
	//---------------------------------------------




	//-----------------change packetsize------------------
	say("3.adjust input",0);

	controltransfer(slot,0x8000001000680,buffer);
	if(waitevent(0x20)<0) goto failed;

	//change input
	DWORD packetsize=*(BYTE*)(buffer+7);
	context(inputaddr,2,0,(packetsize<<16)|(4<<3)|6,controladdr+1,0,0x40);
	say("    got packetsize:",packetsize);

	//evaluate
	command(inputaddr,0,0, (slot<<24)+(13<<10)+commandcycle );
	if(waitevent(0x21)<0) goto failed;


	slotstate=(*(DWORD*)(outputaddr+0xc))>>27; //if2,addressed
	epstate=(*(DWORD*)(outputaddr+0x20))&0x3;

	if(slotstate==2) say("    slot addressed!",0);
	else say("    slot state:",slotstate);
	if(epstate==0){
		say("    ep0 wrong",0);
		goto failed;
	}
	//--------------------------------------------




	//------------------get descriptor-----------------
	say("4.now we know everything:",0);

	controltransfer(slot,0x12000001000680,buffer);
	if(waitevent(0x20)<0) goto failed;
	explaindescriptor(buffer);

	controltransfer(slot,0x9000002000680,buffer+0x20);
	if(waitevent(0x20)<0) goto failed;

	QWORD temp=*(WORD*)(buffer+0x22);
	temp=(temp<<48)+0x2000680;
	controltransfer(slot,temp,buffer+0x20);
	if(waitevent(0x20)<0) goto failed;
	explaindescriptor(buffer+0x20);
	//--------------------------------------------




	//---------------configure------------------
	say("5.configure:",0);

	context(inputaddr,0,        0,	9,	0,	0,	0);
	context(inputaddr,4,        0x30000,0x8003e,intaddr+1,0,0x80008);

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
	controltransfer(slot,0x10900,0);
	if(waitevent(0x20)<0) goto failed;
	say("    device configured",0);

	transfer(slot,3,	hidbuffer,0,6,	0x25+(1<<10));
	//------------------------------------------




	failed:
	say("}",0);
}




void probeport()
{
	QWORD portnum;
	DWORD portsc;

	command(0,0,0, (1<<24)+(10<<10)+commandcycle );
	if(waitevent(0x21)<0) return;

	for(portnum=1;portnum<=portcount;portnum++)
	{
		checkport(portnum);
	}
}




void initxhci()
{
	//clear home
	clear(xhcihome,0x100000);

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
shout("interrupt20",0);
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

	//if( (erdp&0x8) == 0x8)
	//{
	//取一条event,看看是啥情况
	QWORD pointer=erdp&0xfffffffffffffff0;
	eventsignal=0xffff;
	eventaddr=pointer;

	QWORD trbtype=*(DWORD*)(pointer+0xc);
	trbtype=(trbtype>>10)&0x3f;
	shout("    trbtype:",trbtype);

	switch(trbtype)
	{
		case 0x20:{
			QWORD endpoint=((*(DWORD*)(eventaddr+0xc))>>16)&0x1f;
			shout("    endpoint:",endpoint);
			if(endpoint==3)
			{
				QWORD hidbuffer=hidbufferhome+0x1000;
				QWORD offset=*(DWORD*)(hidbuffer+0xff0) +8;

				if(offset>0xf00) offset=0;
				*(DWORD*)(hidbuffer+0xff0)=offset;

				transfer(1,3,	hidbuffer+offset,0,6,	0x25+(1<<10));
			}
			break;
		}
		case 0x22:{		//设备插入拔出

			//哪个端口改变了
			QWORD portid=(*(DWORD*)pointer) >> 24;
			shout("    port id:",portid);

			QWORD portaddr=portbase+portid*0x10-0x10;
			shout("    port addr:",portaddr);

			//到改变的地方看看
			QWORD portsc=*(DWORD*)portaddr;
			shout("    port status:",portsc);

			//告诉主控，收到变化,bit17写1
			*(DWORD*)portaddr=portsc&0xfffffffd;

			break;
		}
	}

	*(DWORD*)(runtime+0x38)=erdp+0x10;
	*(DWORD*)(runtime+0x3c)=0;
	//}
}

shout("}",0);
}
