#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long

#define xhcihome 0x600000
#define ersthome xhcihome+0x10000

#define dcbahome xhcihome+0x20000
#define scratchpadhome xhcihome+0x30000

#define cmdringhome xhcihome+0x40000
#define eventringhome xhcihome+0x80000




static u64 xhciport=0;
static u64 xhciaddr=0;




//用了别人的函数
u32 in32(u32 addr);
void out32(u32 port, u32 addr);
void say(char* , ...);
void initusb();





/*
void explaincapability()
{
say("pci cap{");

	u32 offset;
	u32 temp;
	u32 type;
	u32 next;

	out32(0xcf8,xhciport+0x34);
	offset=in32(0xcfc)&0xff;

	while(1)
	{
	out32(0xcf8,xhciport+offset);
	temp=in32(0xcfc);
	type=temp&0xff;
	next=(temp>>8)&0xff;

	switch(type)
	{
		case 0x5:
		{
			out32(0xcf8,xhciport+offset);
			temp=in32(0xcfc);
			if( (temp&0x800000) ==0 )
			{
			say("32bit msi@%x",offset);

			//before
			out32(0xcf8,xhciport+offset);
			say("	control:%x\n",in32(0xcfc));
			out32(0xcf8,xhciport+offset+4);
			say("	addr:%x\n",in32(0xcfc));
			out32(0xcf8,xhciport+offset+8);
			say("	data:%x\n",in32(0xcfc));

			//do something
			out32(0xcf8,xhciport+offset+8);
			out32(0xcfc,0x20);
			out32(0xcf8,xhciport+offset+4);
			out32(0xcfc,0xfee00000);
			out32(0xcf8,xhciport+offset);
			out32(0xcfc,temp|0x10000);

			//after
			out32(0xcf8,xhciport+offset);
			say("	control:%x\n",in32(0xcfc));
			out32(0xcf8,xhciport+offset+4);
			say("	addr:%x\n",in32(0xcfc));
			out32(0xcf8,xhciport+offset+8);
			say("	data:%x\n",in32(0xcfc));
			}
			else
			{
			say("64bit msi@",offset);

			out32(0xcf8,xhciport+offset);
			say("	control:%x\n",in32(0xcfc));
			out32(0xcf8,xhciport+offset+4);
			say("	addr:%x\n",in32(0xcfc));
			out32(0xcf8,xhciport+offset+0xc);
			say("	data:%x\n",in32(0xcfc));

			out32(0xcf8,xhciport+offset+0xc);
			out32(0xcfc,0x20);
			out32(0xcf8,xhciport+offset+4);
			out32(0xcfc,0xfee00000);
			out32(0xcf8,xhciport+offset);
			out32(0xcfc,temp|0x10000);

			out32(0xcf8,xhciport+offset);
			say("	control:%x\n",in32(0xcfc));
			out32(0xcf8,xhciport+offset+4);
			say("	addr:%x\n",in32(0xcfc));
			out32(0xcf8,xhciport+offset+0xc);
			say("	data:%x\n",in32(0xcfc));
			}
			break;
		}

		case 0x11:
		{
			say("msix@%x\n",offset);
break;
			u32 msixcontrol;
			u32* msixtable;
			u32* pendingtable;

			//get mmio addr,msixtable addr,pendingtable addr
			out32(0xcf8,xhciport+0x10);
			xhciaddr=in32(0xcfc)&0xfffffff0;

			out32(0xcf8,xhciport+offset+8);
			pendingtable=(u32*)( xhciaddr+in32(0xcfc) );
			out32(0xcf8,xhciport+offset+4);
			msixtable=(u32*)( xhciaddr+in32(0xcfc) );
			out32(0xcf8,xhciport+offset);
			msixcontrol=in32(0xcfc);
			out32(0xcf8,xhciport+offset);
			out32(0xcfc,msixcontrol|0x80000000);
			out32(0xcf8,xhciport+offset);
			msixcontrol=in32(0xcfc);

			say("	msix control:%x\n",msixcontrol);
			say("	msix table@%x\n",msixtable);
			say("	pending table@%x\n",pendingtable);

			//msixtable[0],addrlow,addrhigh,vector,control
			msixtable[0]=0xfee00000;
			msixtable[1]=0;
			msixtable[2]=0x20;
			msixtable[3]=msixtable[3]&0xfffffffe;

			say("	@0:%x\n",msixtable[0]);
			say("	@4:%x\n",msixtable[1]);
			say("	@8:%x\n",msixtable[2]);
			say("	@c:%x\n",msixtable[3]);

			break;
		}

		default:
		{
			say("unknown type:%x\n",type);
		}
	}
	
	if(type == 0) break;	//当前capability类型为0，结束
	if(next < 0x40) break;	//下一capability位置错误，结束
	
	offset=next;
	}
	
say("}\n",0);

}
*/




u64 probepci()
{
	u64 temp;
	say("(xhci)portaddr:%x{\n",xhciport);

	//disable pin interrupt+enable bus mastering
	//very important,in qemu-kvm 1.6.2,bus master bit is 0,must set 1
	out32(0xcf8,xhciport+0x4);
	temp=in32(0xcfc)|(1<<10)|(1<<2);

	out32(0xcf8,xhciport+0x4);
	out32(0xcfc,temp);

	out32(0xcf8,xhciport+0x4);
	say("	sts&cmd:%x\n",(u64)in32(0xcfc));

	//deal with capability list
	//explaincapability();

	//get xhciaddr from bar0
	out32(0xcf8,xhciport+0x10);
	xhciaddr=in32(0xcfc)&0xfffffff0;
	say("}\n");
}




int ownership(u64 addr)
{
	//set hc os owned semaphore
	int timeout;
	volatile u32* temp=(u32*)addr;
	temp[0]=temp[0] | 0x1000000;




	//看看控制权到手没
	timeout=0;
	while(1)
	{
		timeout++;
		if(timeout > 0xffffff)
		{
			say("	failed to get ownership");
			return -1;
		}


		//
		if( (temp[0]&0x1000000) == 0x1000000 )
		{
			if( (temp[0]&0x10000) == 0 )
			{
				say("	bios gone");
				return 1;
			}
		}
	}
}
void supportedprotocol(u64 addr)
{
	u64* memory=(u64*)(xhcihome+0x40);
	int i;
	u64 first=(*(u32*)addr) >> 16;
	u64 third=*(u32*)(addr+8);
	u64 start=third & 0xff;
	u64 count=(third>>8) & 0xff;

	//say("first:",first);
	if(first<0x100)		//[0,0xff]
	{
		say("	usb?\n");
	}
	else if(first<=0x200)	//[0x100,0x1ff]
	{
		say("	usb1:%x\n",start);
		for(i=start;i<start+count;i++) memory[i]=1;
	}
	else if(first<=0x300)	//[0x200,0x2ff]
	{
		say("	usb2:%x\n",start);
		for(i=start;i<start+count;i++) memory[i]=2;
	}
	else			//[0x300,0x3ff]
	{
		say("	usb3:%x\n",start);
		for(i=start;i<start+count;i++) memory[i]=3;
	}
}




void explainxecp(u64 addr)
{
	u32 temp;

	say("	explain xecp@%x{\n",addr);
	while(1)
	{
		say("	@%x\n",addr);
		temp=*(u32*)addr;
		u8 type=temp&0xff;
		u64 next=(temp>>6)&0x3fc;

		say("	cap:%x\n",type);
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
	say("	}\n");
}




int probexhci()
{
say("(xhci)memaddr:%x{\n",xhciaddr);

//基本信息
//say("base information{");
	u64 version=(*(u32*)xhciaddr) >> 16;
	u64 caplength=(*(u32*)xhciaddr) & 0xffff;

	u64 hcsparams1=*(u32*)(xhciaddr+4);
	u64 hcsparams2=*(u32*)(xhciaddr+8);
	u64 hcsparams3=*(u32*)(xhciaddr+0xc);
	u64 capparams=*(u32*)(xhciaddr+0x10);

	volatile u64 operational=xhciaddr+caplength;
	u64 runtime=xhciaddr+(*(u32*)(xhciaddr+0x18));

	//say("	version:%x\n",version);
	//say("	caplength:%x\n",caplength);

	//say("	hcsparams1:%x\n",hcsparams1);
	//say("	hcsparams2:%x\n",hcsparams2);
	//say("	hcsparams3:%x\n",hcsparams3);
	//say("	capparams:%x\n",capparams);

	//say("	operational@%x\n",operational);
	//say("	runtime@%x\n",runtime);
//say("}\n",0);




//mostly,grab ownership
	u64 xecp=xhciaddr+( (capparams >> 16) << 2 );
	explainxecp(xecp);




//打印bios初始化完后的状态
//say("before we destory everything{\n");
	u64 usbcommand=*(u32*)operational;
	u64 usbstatus=*(u32*)(operational+4);
	u64 pagesize=0x1000;
	u64 crcr=*(u32*)(operational+0x18);
	u64 dcbaa=*(u32*)(operational+0x30);
	u64 config=*(u32*)(operational+0x38);

	//蛋疼的计算pagesize
	u64 psz=*(u32*)(operational+8);
	while(1)
	{
		if(psz == 0){
			say("psz:\n",psz);
			return -1;
		}
		if(psz == 1) break;

		psz>>1;
		pagesize<<1;
	}

	//say("	usbcommand:%x\n",usbcommand);
	//say("	usbstatus:%x\n",usbstatus);
	//say("	pagesize:%x\n",pagesize);
	//say("	crcr:%x\n",crcr);
	//say("	dcbaa:%x\n",dcbaa);
	//say("	config:%x\n",config);
	say("}\n",0);




//----------------------------1.xhci复位------------------------------
//init system io memory map,if supported
//xhci reset,wait until CNR flag is 0
//--------------------------------------------------------------------
	say("init xhci{\n",0);
	say("	1.stop&reset\n");

	//xhci正在运行吗
	if( (usbstatus&0x1) == 0)		//HCH位为0，即正在运行
	{
		say("		running,stopping\n");

		//按下停止按钮
		*(u32*)operational=usbcommand&0xfffffffe;

		//等一会
		u64 wait1=0xfffffff;
		for(;wait1>0;wait1--) asm("nop");

		//xhci停了吗
		usbstatus=*(u32*)(operational+4);
		if( (usbstatus&0x1) == 0)	//HCH位为0，即正在运行
		{
			say("		not stop\n");
			return -2;
		}
	}

	//按下复位按钮
	usbcommand=*(u32*)operational;
	*(u32*)operational=usbcommand|2;

	//等一会
	u64 wait2=0xfffffff;
	for(;wait2>0;wait2--) asm("nop");

	usbcommand=*(u32*)(operational);
	if((usbcommand&0x2) == 2)
	{
		say("	reset failed:%x\n",usbcommand);
		return -3;
	}

	//controller not ready=1就是没准备好
	usbstatus=*(u32*)(operational+4);
	if( (usbstatus&0x800) == 0x800 )
	{
		say("	controller not ready:%x\n",usbstatus);
		return -4;
	}

	//好像第一步成功了
	//say("	command:%x\n",usbcommand);
	//say("	status:%x\n",usbstatus);




//----------------------2.xhci结构---------------------------
//program the max device slot enabled field in config register
//program the dcbaap
//program crcr,point to addr of first trb in command ring
//----------------------------------------------------------
	say("	2.maxslot&dcbaa&crcr:\n");
	//maxslot=deviceslots
	*(u32*)(operational+0x38)=(*(u32*)(xhciaddr+4)) &0xff;
	say("		maxslot:%x\n",*(u32*)(operational+0x38) );

	//dcbaa(scratchpad)
	*(u32*)(operational+0x30)=dcbahome;
	*(u32*)(operational+0x34)=0;
	say("		dcbaa:%x\n",dcbahome);

	*(u32*)(dcbahome) = scratchpadhome;		//用了宏，加括号
	*(u32*)(dcbahome+4)=0;					//高位
	say("		scratchpad:%x\n",scratchpadhome);

	//command linktrb:lastone point to firstone
	*(u64*)(cmdringhome+0xfff*0x10)=cmdringhome;
	*(u32*)(cmdringhome+0xfff*0x10+8)=0;
	*(u32*)(cmdringhome+0xfff*0x10+0xc)=(6<<10)+2;

	//crcr
	*(u32*)(operational+0x18)=cmdringhome+1;
	*(u32*)(operational+0x1c)=0;
	say("		crcr:%x\n",cmdringhome+1);




//-----------------------3.中断,eventring----------------------
//msix table,message addr,message data,enable vector
//allocate&init msix pending bit array
//point table&pba offset to message control table and pending bit array
//init message control register of msix capability structure
//-------------------------------------------------
	say("	3.interrupt&eventring\n");

	//setupisr(xhciaddr);
	//----------------------if(msix)--------------------------
	//----------------------if(msi)--------------------------
	//----------------------if(intx)--------------------------

	//-------------define event ring-----------
	//build the "event ring segment table"
	*(u64*)(ersthome)=eventringhome;
	*(u64*)(ersthome+0x8)=0x100;		//0x1000*0x10=0x10000
						//but!!!my asus n55 must set it below 0x100

	//ERSTSZ
	*(u32*)(runtime+0x28)=1;

	//ERSTBA
	*(u32*)(runtime+0x30)=ersthome;
	*(u32*)(runtime+0x34)=0;	//这一行必须有

	//ERDP
	*(u32*)(runtime+0x38)=eventringhome;
	*(u32*)(runtime+0x3c)=0;	//这一行也必须有

	//enable EWE|HSEIE|INTE(0x400|0x8|0x4) in USBCMD
	//*(u32*)operational |= 0x40c;
	//*(u32*)operational = (*(u32*)operational) | 0xc;	//no interrupt

	//IMOD
	*(u32*)(runtime+0x24)=4000;

	//IMAN
	*(u32*)(runtime+0x20) = (*(u32*)(runtime+0x20)) | 0x2;

	say("		event ring@%x\n",eventringhome);




//---------------------xhci启动---------------------
//write usbcmd,turn host controller on
//-------------------------------------------------
	say("	4.turnon\n");
	//turn on
	*(u32*)operational = (*(u32*)operational) | 0x1;

	//不用等?
	//u64 wait3=0xffffff;
	//while(wait3--)asm("nop");

	//
	//say("	command:%x\n",*(u32*)operational);
	//say("	status:%x\n",*(u32*)(operational+4));

	say("}\n");




return 0;
}








void initxhci(u64 pciaddr)
{
	say("xhci@%x\n",pciaddr);

	xhciport = pciaddr;
	if(xhciport == 0) goto end;

	//pci部分
	probepci();
	if(xhciaddr == 0) goto end;

	//stop xhci
	//probexhci();

	//clean home(1m)
	u8* p=(u8*)(xhcihome);
	int i;
	for(i=0;i<0x100000;i++) p[i]=0;

	//start xhci
	probexhci();

	//initusb
	initusb(xhciaddr);




end:		//就一行空白
	say("\n");
}
