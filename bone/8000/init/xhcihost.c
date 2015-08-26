#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long

#define pcihome 0x40000

#define xhcihome 0x200000
#define ersthome xhcihome+0x10000

#define dcbahome xhcihome+0x20000
#define scratchpadhome xhcihome+0x30000

#define cmdringhome xhcihome+0x40000
#define eventringhome xhcihome+0x80000




static QWORD xhciport=0;
static QWORD xhciaddr=0;




//用了别人的函数
void say(char* , ...);





//全部设备信息我放在0x110000了，格式如下:(非本环境自己处理这一步)
//[0,7]:(vendorid<<16)+deviceid
//[8,0xf]:(class<<24)+(subclass<<16)+(progif<<8)+revisionid
//[0x10,0x17]:portaddress of the device
//[0x18,0x1f]:ansciiname of the device
//本函数作用是：
//1.返回要驱动的设备的portaddress
//2.填上[0x18,0x1f],(为了工整好看)
void findxhci()
{
	QWORD* addr=(QWORD*)(pcihome);
	DWORD temp;
	int i;
	for(i=0;i<0x80;i++)
	{
		temp=addr[8*i+1];
		temp&=0xffffff00;

		if(temp==0x0c033000)
		{
			xhciport=addr[8*i+2];
			addr[8*i+3]=0x69636878;

			return;
		}
	}
}




static inline void out32( unsigned short port, unsigned int val )
{
	asm volatile( "outl %0, %1"  : : "a"(val), "Nd"(port) );
}
static inline unsigned int in32( unsigned short port )
{
	unsigned int ret;
	asm volatile( "inl %1, %0"  : "=a"(ret) : "Nd"(port) );
	return ret;
}/*
void explaincapability()
{
say("pci cap{");

	DWORD offset;
	DWORD temp;
	DWORD type;
	DWORD next;

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
			DWORD msixcontrol;
			DWORD* msixtable;
			DWORD* pendingtable;

			//get mmio addr,msixtable addr,pendingtable addr
			out32(0xcf8,xhciport+0x10);
			xhciaddr=in32(0xcfc)&0xfffffff0;

			out32(0xcf8,xhciport+offset+8);
			pendingtable=(DWORD*)( xhciaddr+in32(0xcfc) );
			out32(0xcf8,xhciport+offset+4);
			msixtable=(DWORD*)( xhciaddr+in32(0xcfc) );
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




QWORD probepci()
{
	say("(xhci)portaddr:%x{\n",xhciport);

	//disable pin interrupt+enable bus mastering
	//very important,in qemu-kvm 1.6.2,bus master bit is 0,must set 1
	out32(0xcf8,xhciport+0x4);
	DWORD temp=in32(0xcfc)|(1<<10)|(1<<2);
	out32(0xcf8,xhciport+0x4);
	out32(0xcfc,temp);

	out32(0xcf8,xhciport+0x4);
	say("	sts&cmd:%x\n",(QWORD)in32(0xcfc));

	//deal with capability list
	//explaincapability();

	//get xhciaddr from bar0
	out32(0xcf8,xhciport+0x10);
	xhciaddr=in32(0xcfc)&0xfffffff0;
	say("}\n");
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
		say("	grabing ownership");

		temp=*(DWORD*)addr;
		temp&=0x10000;
		if(temp == 0)
		{
			say("	bios gone");
			return 0;
		}
	}

	say("	failed");
	return -1;
}
void supportedprotocol(QWORD addr)
{
	QWORD* memory=(QWORD*)(xhcihome+0x40);
	int i;
	QWORD first=(*(DWORD*)addr) >> 16;
	QWORD third=*(DWORD*)(addr+8);
	QWORD start=third & 0xff;
	QWORD count=(third>>8) & 0xff;

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




void explainxecp(QWORD addr)
{
	DWORD temp;

	say("	explain xecp@%x{\n",addr);
	while(1)
	{
		say("	@%x\n",addr);
		temp=*(DWORD*)addr;
		BYTE type=temp&0xff;
		QWORD next=(temp>>6)&0x3fc;

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
	QWORD version=(*(DWORD*)xhciaddr) >> 16;
	QWORD caplength=(*(DWORD*)xhciaddr) & 0xffff;

	QWORD hcsparams1=*(DWORD*)(xhciaddr+4);
	QWORD hcsparams2=*(DWORD*)(xhciaddr+8);
	QWORD hcsparams3=*(DWORD*)(xhciaddr+0xc);
	QWORD capparams=*(DWORD*)(xhciaddr+0x10);

	volatile QWORD operational=xhciaddr+caplength;
	QWORD runtime=xhciaddr+(*(DWORD*)(xhciaddr+0x18));

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
	QWORD xecp=xhciaddr+( (capparams >> 16) << 2 );
	explainxecp(xecp);




//打印bios初始化完后的状态
//say("before we destory everything{\n");
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

	//say("	usbcommand:%x\n",usbcommand);
	//say("	usbstatus:%x\n",usbstatus);
	//say("	pagesize:%x\n",pagesize);
	//say("	crcr:%x\n",crcr);
	//say("	dcbaa:%x\n",dcbaa);
	//say("	config:%x\n",config);
//say("}\n",0);




//----------------------------1.xhci复位------------------------------
//init system io memory map,if supported
//xhci reset,wait until CNR flag is 0
//--------------------------------------------------------------------
	say("	init xhci{",0);
	say("	1.stop&reset");

	//xhci正在运行吗
	if( (usbstatus&0x1) == 0)		//HCH位为0，即正在运行
	{
		say("		running,stopping\n");

		//按下停止按钮
		*(DWORD*)operational=usbcommand&0xfffffffe;

		//等一会
		QWORD wait1=0xffffff;
		for(;wait1>0;wait1--) asm("nop");

		//xhci停了吗
		usbstatus=*(DWORD*)(operational+4);
		if( (usbstatus&0x1) == 0)	//HCH位为0，即正在运行
		{
			say("		not stop\n");
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
		say("	reset failed:%x\n",usbcommand);
		return -3;
	}

	//controller not ready=1就是没准备好
	usbstatus=*(DWORD*)(operational+4);
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
	say("	2.maxslot&dcbaa&crcr:");
	//maxslot=deviceslots
	*(DWORD*)(operational+0x38)=(*(DWORD*)(xhciaddr+4)) &0xff;
	say("		maxslot:%x\n",*(DWORD*)(operational+0x38) );

	//dcbaa(scratchpad)
	*(DWORD*)(operational+0x30)=dcbahome;
	*(DWORD*)(operational+0x34)=0;
	say("		dcbaa:%x\n",dcbahome);
	*(DWORD*)(dcbahome) = scratchpadhome;		//用了宏，加括号
	*(DWORD*)(dcbahome+4)=0;					//高位
	say("		scratchpad:%x\n",scratchpadhome);

	//command linktrb:lastone point to firstone
	*(QWORD*)(cmdringhome+0xfff*0x10)=cmdringhome;
	*(DWORD*)(cmdringhome+0xfff*0x10+8)=0;
	*(DWORD*)(cmdringhome+0xfff*0x10+0xc)=(6<<10)+2;

	//crcr
	*(DWORD*)(operational+0x18)=cmdringhome+1;
	*(DWORD*)(operational+0x1c)=0;
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
	*(QWORD*)(ersthome)=eventringhome;
	*(QWORD*)(ersthome+0x8)=0x100;		//0x1000*0x10=0x10000
						//but!!!my asus n55 must set it below 0x100

	//ERSTSZ
	*(DWORD*)(runtime+0x28)=1;

	//ERSTBA
	*(DWORD*)(runtime+0x30)=ersthome;
	*(DWORD*)(runtime+0x34)=0;	//这一行必须有

	//ERDP
	*(DWORD*)(runtime+0x38)=eventringhome;
	*(DWORD*)(runtime+0x3c)=0;	//这一行也必须有

	//enable EWE|HSEIE|INTE(0x400|0x8|0x4) in USBCMD
	//*(DWORD*)operational |= 0x40c;
	//*(DWORD*)operational = (*(DWORD*)operational) | 0xc;	//no interrupt

	//IMOD
	*(DWORD*)(runtime+0x24)=4000;

	//IMAN
	*(DWORD*)(runtime+0x20) = (*(DWORD*)(runtime+0x20)) | 0x2;

	say("		event ring@%x\n",eventringhome);




//---------------------xhci启动---------------------
//write usbcmd,turn host controller on
//-------------------------------------------------
	say("	4.turnon\n");
	//turn on
	*(DWORD*)operational = (*(DWORD*)operational) | 0x1;

	//wait for 100ms?
	QWORD wait3=0xffffff;
	while(wait3--)asm("nop");

	//
	//say("	command:%x\n",*(DWORD*)operational);
	//say("	status:%x\n",*(DWORD*)(operational+4));

	say("	}\n");




say("}\n",0);
return 0;
}








void initxhci()
{
	//clean home(1m)
	BYTE* p=(BYTE*)(xhcihome);
	int i;
	for(i=0;i<0x100000;i++) p[i]=0;

	//find pci address
	findxhci();
	if(xhciport==0) goto end;

	//pci部分
	probepci();
	if(xhciaddr==0) goto end;
	*(QWORD*)(xhcihome)=0x69636878;			//xhci
	*(QWORD*)(xhcihome+8)=xhciaddr;			//0x????????

	//xhci
	probexhci();




end:		//就一行空白
	say("\n");
}
