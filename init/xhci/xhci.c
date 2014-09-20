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

static QWORD runtime;




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
        QWORD* table=(QWORD*)0x4000;
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
		say("grabing ownership",0);

		temp=*(DWORD*)addr;
		temp&=0x10000;
		if(temp == 0)
		{
			say("bios gone",0);
			return 0;
		}
	}

	say("    failed",0);
	return -1;
}
void supportedprotocol(QWORD addr)
{
	QWORD* memory=(QWORD*)(slothome);
	int i;
	QWORD first=(*(DWORD*)addr) >> 16;
	QWORD third=*(DWORD*)(addr+8);
	QWORD start=third & 0xff;
	QWORD count=(third>>8) & 0xff;

	//say("first:",first);
	if(first<0x100)		//[0,0xff]
	{
		say("    usb?",0);
	}
	else if(first<=0x200)	//[0x100,0x1ff]
	{
		say("    usb1:port",start);
		for(i=start;i<start+count;i++) memory[i]=1;
	}
	else if(first<=0x300)	//[0x200,0x2ff]
	{
		say("    usb2:port",start);
		for(i=start;i<start+count;i++) memory[i]=2;
	}
	else			//[0x300,0x3ff]
	{
		say("    usb3:port",start);
		for(i=start;i<start+count;i++) memory[i]=3;
	}
}




void explainxecp(QWORD addr)
{
	DWORD temp;

	say("explain xecp@",addr);
	say("{",0);
	while(1)
	{
		say("@",addr);
		temp=*(DWORD*)addr;
		BYTE type=temp&0xff;
		QWORD next=(temp>>6)&0x3fc;

		say("cap:",type);
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

	volatile QWORD operational=memaddr+caplength;
	runtime=memaddr+(*(DWORD*)(memaddr+0x18));

	say("    version:",version);
	say("    caplength:",caplength);

	say("    hcsparams1:",hcsparams1);
	say("    hcsparams2:",hcsparams2);
	say("    hcsparams3:",hcsparams3);
	say("    capparams:",capparams);

	say("    operational@",operational);
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

end:		//就一行空白
	say("",0);
}




void realisr20()
{
	shout("int20",0);
        DWORD erdp=*(DWORD*)(runtime+0x38);

        QWORD temp=erdp&0xfffffff0;
        QWORD pcs=(*(DWORD*)(temp+0xc))&0x1;
        while(1)
        {
                temp+=0x10;
                if(temp>=eventringhome+0x1000)
                {
                        temp=eventringhome;
                }
                if( ((*(DWORD*)(temp+0xc))&0x1) != pcs) break;
        }

        *(DWORD*)(runtime+0x38)=temp|8;
        *(DWORD*)(runtime+0x3c)=0;

}
