#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long

#define xhcihome 0x600000
#define ersthome xhcihome+0x10000

#define dcbahome xhcihome+0x20000
	//#define scratchpadhome xhcihome+0x30000

#define cmdringhome xhcihome+0x40000
#define eventringhome xhcihome+0x80000

#define usbhome 0x700000
	//0:			//+0:		incontext,+800:outcontext
				//+1000:	device infomation
	//1:ep0			//+2000:	ep0 ring
				//+3000:	ep0 buffer
	//2:ep1.0		//+4000:
				//+5000:
	//3:ep1.1		//+6000:	ep1.1 ring
				//+7000:	ep1.1 buffer




void say(char* , ...);




volatile static u64 contextsize;

volatile static u64 eventringdequeue;
volatile static u64 eventringcycle;
volatile static u64 runtime;

volatile static u64 cmdringenqueue;
volatile static u64 cmdringcycle;
volatile static u64 doorbell;




struct setup{
	u8 bmrequesttype;	//	0x80
	u8 brequest;		//	0x6
	u16 wvalue;		//	(descriptor type>>8)+descriptor index
	u16 windex;		//	0
	u16 wlength;		//	length

	u64 buffer;
};
static struct setup packet;
void sendpacket(u64 ringaddr)
{
	//得到这次往哪儿写
	u64 enqueue=*(u64*)(ringaddr+0xff0);
	u32* p=(u32*)(ringaddr+enqueue);

	//say("(enqueue before:%x)\n",enqueue);
	if(packet.bmrequesttype>=0x80)
	{
		//setup stage
		p[0]=(packet.wvalue<<16)+(packet.brequest<<8)+packet.bmrequesttype;
		p[1]=(packet.wlength<<16)+(packet.windex);
		p[2]=8;
		p[3]=(3<<16)+(2<<10)+(1<<6)+1;

		//data stage
		p[4]=packet.buffer;
		p[5]=0;
		p[6]=packet.wlength;
		p[7]=(1<<16)+(3<<10)+1;

		//status stage
		p[8]=p[9]=p[0xa]=0;
		p[0xb]=(4<<10)+(1<<5)+1;

		//下次往哪儿写
		enqueue+=0x30;
	}
	else
	{
		//setup stage
		p[0]=(packet.wvalue<<16)+(packet.brequest<<8)+packet.bmrequesttype;
		p[1]=(packet.wlength<<16)+(packet.windex);
		p[2]=8;
		p[3]=(2<<10)+(1<<6)+1;

		//status stage
		p[4]=p[5]=p[6]=0;
		p[7]=(1<<16)+(4<<10)+(1<<5)+1;	//in

		//下次往哪儿写
		enqueue+=0x20;
	}




	//next
	if(enqueue>0xf00)
	{
		enqueue=0;
		//cycle取反
	}
	*(u64*)(ringaddr+0xff0)=enqueue;
	//say("(enqueue after:%x)\n",enqueue);
}
void usbcommand(u8 bmrequesttype,u8 brequest,u16 wvalue,u16 windex,u16 wlength,u64 buffer)
{
	packet.bmrequesttype=bmrequesttype;
	packet.brequest=brequest;
	packet.wvalue=wvalue;
	packet.windex=windex;
	packet.wlength=wlength;
	packet.buffer=buffer;
}




//直接在command ring上写TRB（1个TRB=4个u32）,然后处理enqueue指针，最后ring
void hostcommand(u32 d0,u32 d1,u32 d2,u32 d3)
{
	u32* pointer=(u32*)(cmdringenqueue);

	//写ring
	pointer[0]=d0;
	pointer[1]=d1;
	pointer[2]=d2;
	pointer[3]=d3 | cmdringcycle;

	//enqueue指针怎么变
	cmdringenqueue+=0x10;
	if(cmdringenqueue >= cmdringhome+0xfff0)
	{
		//reload enqueue , change cycle bit
		cmdringenqueue=cmdringhome;

		if(cmdringcycle==0)cmdringcycle=1;
		else cmdringcycle=0;
	}

	//处理完了，敲门铃
	//ring(0,0);
	*(u32*)(doorbell)=0;
}







void ring(int slot,int endpoint)
{
	*(u32*)(doorbell+4*slot)=endpoint;
}





int waitxhci(u64 wantedslot,u64 wantedtype)
{
	volatile u32* thisevent;
	u64 timeout=0;
	while(1)
	{
		thisevent=(u32*)eventringdequeue;

		//等太久就不等了
		timeout++;
		if(timeout>0xffffff)
		{
			say("(timeout@%x)\n",thisevent);
			return -1;
		}

	


		//拿出一个,cycle不对就回到while(1)
		if( (thisevent[3] & 0x1) != eventringcycle ) continue;

	


		//cycle对了，检查completion code
		//正常就往下走，否则返回失败
		if( (thisevent[2] >> 24) != 0x1 )
		{
			say("(error event@%x)%x,%x,%x,%x\n",thisevent,
			thisevent[0],thisevent[1],thisevent[2],thisevent[3]);
			return -1;
		}




		//没出错,给xhci报告当前erdp
		eventringdequeue+=0x10;
		//*(u32*)(runtime+0x38)=eventringdequeue | 8;
		//*(u32*)(runtime+0x3c)=0;

	


		//如果想等host事件
		u32 slot=thisevent[3]>>24;
		u32 type=(thisevent[3]>>10)&0x3f;
		if(wantedslot == 0)
		{
			if(type == wantedtype)		//正是想要的
			{
				if(type==0x21)
				{
					//如果是command complete
					//就直接返回slot号
					//enable slot需要得到这个slot号
					//这个值肯定大于0
					return thisevent[3] >> 24;
				}
				else return 1;
			}
		}

	


		//如果想等device事件
		else
		{
			if(slot == wantedslot) return 1;
		}
	}//while(1)
}








//以下是提取自descriptor的最重要信息，每次只被这个函数更新,就像人记笔记......
//static u64 vendor;
//static u64 product;
//static u64 classcode;
//static u64 interfaceclass;
//static u64 deviceprotocol;

//static u64 wmaxpacket;
//static u64 interval;
//static u8 stringtoread[8];
void explaindescriptor(u64 addr)
{
	u32 type=*(u8*)(addr+1);
	if(type==0)return;
	//say("type:%x\n",type);
	//if(	(type==0)|((type>7)&(type<0x21))|(type>0x29)	) return;

	volatile u32 i;
	switch(type)
	{
	case 1:	//设备描述符
	{
		//这是第一个被读取的描述符，所以几个变量在这里清零最好
		//classcode=0;	//这个就不必了，马上就变掉
		//for(i=0;i<8;i++) stringtoread[i]=0;

		say("(desc 1)device@%x{\n",addr);
		//say("blength:%x\n",*(u8*)addr);
		//say("type:%x\n",*(u8*)(addr+1));
		//say("bcdusb:%x\n",*(u16*)(addr+2));

		//classcode=*(u8*)(addr+4);			//!
		//say("bclass:%x\n",classcode);

		//say("bsubclass:%x\n",*(u8*)(addr+5));

		//deviceprotocol=*(u8*)(addr+6);		//!
		//say("bprotocol:%x\n",deviceprotocol);

		say("	bpacketsize:%x\n",*(u8*)(addr+7));
		say("	vendor:%x\n",*(u16*)(addr+8));
		say("	product:%x\n",*(u16*)(addr+0xa));

		//say("	bcddevice:%x\n",*(u16*)(addr+0xc));

		//i=*(u8*)(addr+0xe);
		//stringtoread[i]=1;
		//say("	imanufacturer:%x\n",i);

		//i=*(u8*)(addr+0xf);
		//stringtoread[i]=1;
		//say("	iproduct:%x\n",i);

		//i=*(u8*)(addr+0x10);
		//stringtoread[i]=1;
		//say("	iserial:%x\n",i);

		say("	bnumconf:%x\n",*(u8*)(addr+0x11));
		say("}\n");
		break;
	}
	case 2:	//配置描述符
	{
		say("(desc 2)conf@%x{\n",addr);
		//say("	blength:%x\n",*(u8*)addr);
		//say("	type:%x\n",*(u8*)(addr+1));
		//say("	wlength:%x\n",*(u16*)(addr+2));
		say("	bnuminterface:%x\n",*(u8*)(addr+4));
		//say("	bvalue:%x\n",*(u8*)(addr+5));
		//i=*(u8*)(addr+0x6);
		//stringtoread[i]=1;
		//say("	i:",i);
		//u8 bmattribute=*(u8*)(addr+7);
		//say("	bmattribute:%x\n",bmattribute);
		//if( (bmattribute&0x40) ==0x40 ) say("	self powered");
		//if( (bmattribute&0x20) ==0x20 ) say("	remote wake");
		//say("	bmaxpower:%x\n",*(u8*)(addr+8));

		u32 totallength=*(u16*)(addr+2);
		say("	wlength:%x\n",totallength);
		u32 offset=9;
		u32 nextlen,nexttype;
		while(1)
		{
			//偏移超过总长度肯定错了
			if(offset>=totallength) break;

			//拿出下一个的长度和种类
			//用的递归,所以要是里面再出现type2的错误desc,可能会死着...
			nextlen=*(u8*)(addr+offset);
			nexttype=*(u8*)(addr+offset);
			//say("	@%x:(%x,%x)\n",offset,nextlen,nexttype);
			if(nexttype==2)break;

			//一切正常,解释下一个表
			explaindescriptor(addr+offset);

			//下一轮循环
			offset+=nextlen;
		}

		say("}\n");
		break;
	}
	case 3:	//接口描述符
	{
		say("(desc 3)string@%x{\n",addr);

		say("	blength:%x\n",*(u8*)(addr));
		u16 language=*(u16*)(addr+2);
		say("	language:%x\n",language);

		say("}\n");
		break;
	}
	case 4:	//接口描述符
	{
		say("	(desc 4)interface@%x{\n",addr);
		//say("	length:%x\n",*(u8*)addr);
		//say("	type:%x\n",*(u8*)(addr+1));
		say("		binterfacenum:%x\n",*(u8*)(addr+2));
		say("		balternatesetting:%x\n",*(u8*)(addr+3));
		say("		bnumendpoint:%x\n",*(u8*)(addr+4));

		//interfaceclass=*(u8*)(addr+5);
		//say("		bclass:%x\n",interfaceclass);
		//say("		bsubclass:%x\n",*(u8*)(addr+6));
		//say("		bprotol:%x\n",*(u8*)(addr+7));

		//i=*(u8*)(addr+0x8);
		//stringtoread[i]=1;
		//say("		i:%x\n",i);

		say("	}\n");
		break;
	}
	case 5:	//端点描述符
	{
		say("	(desc 5)endpoint@%x{\n",addr);
		//say("		blength:%x\n",*(u8*)addr);
		//say("		type:%x\n",*(u8*)(addr+1));

		u8 endpoint=*(u8*)(addr+2);
		u8 eptype=*(u8*)(addr+3);
		say("		ep:%x\n",endpoint&0xf);
		if(endpoint>0x80)
		{
			if(eptype==0) say("		in,control\n");
			else if(eptype==1) say("		in,isochronous\n");
			else if(eptype==2) say("		in,bulk\n");
			else say("		in,interrupt\n");
		}
		else
		{
			if(eptype==0) say("		out,control\n");
			else if(eptype==1) say("		out,isochronous\n");
			else if(eptype==2) say("		out,bulk\n");
			else say("		out,interrupt\n");
		}


		//wmaxpacket=*(u16*)(addr+4);
		//say("	wmaxpacket:%x\n",wmaxpacket);
		//interval=*(u8*)(addr+6);
		//say("	binterval:%x\n",interval);

		say("	}\n");
		break;
	}
	case 6:	//设备限定描述符
	{
		say("	(desc 6)@%x{\n",addr);
		//say("	blength:%x\n",*(u8*)addr);
		//say("	bdescriptortype:%x\n",*(u8*)(addr+1));
		//say("	bcdusb:%x\n",*(u16*)(addr+2));
		//say("	bdeviceclass:%x\n",*(u8*)(addr+4));
		//say("	bdevicesubclass:%x\n",*(u8*)(addr+5));
		//say("	bdeviceprotocol:%x\n",*(u8*)(addr+6));
		//say("	bmaxpacketsize0:%x\n",*(u8*)(addr+7));
		//say("	bnumconfigurations:%x\n",*(u8*)(addr+8));
		break;
	}
	case 7:	//其他速率配置描述符
	{
		say("	(desc 7)@%x{\n",addr);
		//say("	blength:%x\n",*(u8*)addr);
		//say("	bdescriptortype:%x\n",*(u8*)(addr+1));
		//say("	wtotallength:%x\n",*(u16*)(addr+2));
		//say("	bnuminterfaces:%x\n",*(u8*)(addr+4));
		//say("	bconfigurationvalue:%x\n",*(u8*)(addr+5));
		//say("	iconfiguration:%x\n",*(u8*)(addr+6));
		//say("	bmattributes:%x\n",*(u8*)(addr+7));
		//say("	bmaxpower:%x\n",*(u8*)(addr+8));
		break;
	}
	case 0x21:	//hid设备描述符
	{
		say("	(desc 21)hid@%x{\n",addr);
		//say("		blength:",*(u8*)addr);
		//say("		type:",*(u8*)(addr+1));
		//say("		bcdhid:",*(u16*)(addr+2));
		//say("		bcountrycode:",*(u8*)(addr+4));
		//say("		bnumdescriptor:",*(u8*)(addr+5));

		//say("		btype:",*(u8*)(addr+6));
		//say("		wlength:",*(u16*)(addr+7));
		//say("		btype:",*(u8*)(addr+9));
		//say("		wlength:",*(u16*)(addr+10));

		//reportsize=*(u16*)(addr+7);

		say("	}\n");
		break;
	}
	default:
	{
		say("	(desc ?)unknown@%x{\n",type,addr);
	}
	}
}


void recorddevice(u64 vidpid,u64 class,u64 position,u64 speed,u64 slot)
{
	u64* p=(u64*)(usbhome);
	int i=0;
	for(i=0;i<0x200;i+=8)
	{
		if(p[i] == 0)
		{
			p[i]=vidpid;
			p[i+1]=class;
			p[i+2]=position;
			p[i+3]=speed;
			p[i+4]=slot;
			break;
		}
	}
}




/*
void explainhub(u64 addr)
{
	say("hub@%x\n",addr);
        say("blength:%x\n",*(u8*)addr);
        say("type:%x\n",*(u8*)(addr+1));
        say("bnumberofport:%x\n",*(u8*)(addr+2));
	u16 hubcharacteristics=*(u8*)(addr+3);
        say("    power mode:%x\n",hubcharacteristics&0x3);
        say("    compound device:%x\n",(hubcharacteristics>>2)&0x1);
        say("    overcurrent protect:%x\n",(hubcharacteristics>>3)&0x3);
        say("    TT thinktime:%x\n",(hubcharacteristics>>5)&0x3);
        say("    port indicator:%x\n",(hubcharacteristics>>7)&0x1);
        say("bpowerontopowergood:%x\n",*(u8*)(addr+5));
        say("bhubcontrolcurrent:%x\n",*(u8*)(addr+6));
        say("bremoveandpowermask:%x\n",*(u8*)(addr+7));
	say("",0);
}
*/



/*
void fixinterval(u64* interval,u64 speed)
{
	//if(speed==0) say("undefined speed:",speed);
	//if(speed==1) say("full speed:",speed);
	//if(speed==2) say("low speed:",speed);
	//if(speed==3) say("high speed:",speed);
	//if(speed==4) say("super speed:",speed);


	u64 val=*interval;
	say("val:%x\n",val);
	if( (speed==1)|(speed==2) )
	{
		//if(val == 1) val=3;
		//else if(val == 2) val=4;
		//else if( (val>=3) & (val<=4) ) val=5;
		//else if( (val>=5) & (val<=8) ) val=6;
		//else if( (val>=9) & (val<=16) ) val=7;
		//else if( (val>=17) & (val<=32) ) val=8;
		//else if( (val>=33) & (val<=64) ) val=9;
		//else if( (val>=65) & (val<=128) ) val=0xa;
		//else if( (val>=129) & (val<256) ) val=0xb;

		if(val>128)val=0xb;
		else if(val>64)val=0xa;
		else if(val>32)val=0x9;
		else if(val>16)val=0x8;
		else if(val>8)val=7;
		else if(val>4)val=6;
		else if(val>2)val=5;
		else if(val==2)val=4;
		else if(val==1)val=3;
	}
	say("fixed interval:%x\n",val);		//[3,11]
	*interval=val;
}
*/








//rootport:设备自己所在roothub port号,routestring:一层层地址
//static u64 slot;
void hello(u64 rootport,u64 routestring,u32 speed)
{
	//say("device{\n",0);


	//---------------obtain slot------------------
	//say("1.enable slot...",0);
	hostcommand(0,0,0, (9<<10) );
	u64 slot = waitxhci(0,0x21);
	if(slot<=0) goto failed;
	if(slot>=0x10)
	{
		say("bad slotnum:%x\n",slot);
		goto failed;
	}
	say("slot obtained:%x\n",slot);
	//-------------------------------------------




	//-----------address device-----------------
	u64 slotcontext=usbhome+slot*0x10000;
	u64 ep0ring=slotcontext+0x1000;
	u64 ep1out=slotcontext+0x2000;
	u64 ep1in=slotcontext+0x3000;

	u64 data0=slotcontext+0x8000;
	u64 data11=slotcontext+0x9000;
	u64 inputcontext=slotcontext+0xf000;
	u32 maxpacketsize;
	say("	slot context@%x\n",slotcontext);

	//clear context
	int i=0;
	for(i=0;i<0x10000;i++){ *(u8*)(slotcontext+i) = 0; }

	//packetsize
	if(speed==4)maxpacketsize=0x200;
	else if(speed==3)maxpacketsize=0x40;
	else maxpacketsize=8;

	//construct input
	u32* pcontext=(u32*)inputcontext;
	pcontext[1]=3;

	pcontext=(u32*)(inputcontext+contextsize);
	pcontext[0]=(3<<27)+(speed<<20)+routestring;
	pcontext[1]=rootport<<16;

	pcontext=(u32*)(inputcontext+contextsize*2);
	pcontext[0]=0;
	pcontext[1]=(maxpacketsize<<16)+(4<<3)+6;
	pcontext[2]=ep0ring+1;
	pcontext[3]=0;
	pcontext[4]=0x8;

	//output context地址填入对应dcbaa
	*(u32*)(dcbahome+slot*8)=slotcontext;
	*(u32*)(dcbahome+slot*8+4)=0;

	//发送command
	hostcommand(inputcontext,0,0, (slot<<24)+(11<<10) );
	if(waitxhci(0,0x21)<0) goto failed;

	//if2,addressed
	u32 slotstate=(*(u32*)(slotcontext+0xc))>>27;
	u32 epstate=(*(u32*)(slotcontext+contextsize))&0x3;
	if(slotstate==2) say("slot addressed\n");
	else say("	slot state:%x\n",slotstate);
	if(epstate == 0)
	{
		say("	ep0 wrong\n");
		goto failed;
	}
	say("	ep0ring@%x\n",ep0ring);
	//------------------------------------------------




	//_______________device descriptor__________________
	//say("getting device desc(0x12 bytes)......\n");
	packet.bmrequesttype=0x80;
	packet.brequest=6;
	packet.wvalue=0x100;
	packet.windex=0;
	packet.wlength=0x12;
	packet.buffer=data0+0x100;
	sendpacket(ep0ring);
	ring(slot,1);
	if(waitxhci(slot,1)<0) goto failed;
	explaindescriptor(data0+0x100);
	u64 vendorproduct=*(u32*)(data0+0x108);
	//___________________________________________________




	//----------------configuration descriptor-----------
	//say("3.descriptors:",0);
	//[data0+0x100]:configure descriptor
	//say("getting conf desc......\n");
	packet.bmrequesttype=0x80;
	packet.brequest=6;
	packet.wvalue=0x200;
	packet.windex=0;
	packet.wlength=0x9;
	packet.buffer=data0+0x200;
	sendpacket(ep0ring);
	ring(slot,1);
	if(waitxhci(slot,1)<0) goto failed;

	packet.wlength=*(u16*)(data0+0x102);
	sendpacket(ep0ring);
	ring(slot,1);
	if(waitxhci(slot,1)<0) goto failed;
	explaindescriptor(data0+0x200);




	//----------------string descriptors----------------
	//say("string descriptor...",0);
	packet.bmrequesttype=0x80;
	packet.brequest=6;
	packet.wvalue=0x300;
	packet.windex=0;				//语言id
	packet.wlength=4;
	packet.buffer=data0+0x300;
	sendpacket(ep0ring);
	ring(slot,1);
	if(waitxhci(slot,1)<0) goto failed;

	/*
	int wantedindex=1;
	for()
	{
		packet.windex=*(u16*)(data0+0x302);		//从上一步的结果里取出第一个语言
		packet.wvalue=0x300+wantedindex;			//0x300+这种语言下的索引号
		sendpacket(ep0ring);						//为了拿到实际长度
		ring(slot,1);
		if(waitxhci(slot,1)<0) goto failed;

		packet.wlength=*(u8*)(data0+0x300);		//从上一步的结果里取出正确的长度
		sendpacket(ep0ring);
		ring(slot,1);
		if(waitxhci(slot,1)<0) goto failed;
		explaindescriptor(data0+0x300);
	}
	*/
	//--------------------------------------------




	recorddevice(
	vendorproduct,						//+0		//vendor+(product<<16)
	0,									//+8
	rootport+(routestring<<32),			//+10
	speed,								//+18
	slot);								//+20


	failed:
	return;
	//say("}",0);
}







/*
void normalhub(u64 rootport,u64 routestring,u64 speed,u64 slot)
{
	if(deviceprotocol==0) say("no TT hub!");
	if(deviceprotocol==1) say("single TT hub!");
	if(deviceprotocol==2) say("multi TT hub!");

	u64 slotcontext=usbhome+slot*0x8000;
	u64 ep0ring=slotcontext+0x1000;
	u64 ep1out=slotcontext+0x2000;
	u64 ep1in=slotcontext+0x3000;

	u64 data0=slotcontext+0x8000;
	u64 data11=slotcontext+0x9000;
	u64 inputcontext=slotcontext+0xf000;

	u32 slotstate;
	u32 epstate;
	struct setup packet;
	struct context context;


	say("4.set configuration...\n");
	packet.bmrequesttype=0;
	packet.brequest=9;
	packet.wvalue=1;
	packet.windex=0;
	packet.wlength=0;
	sendpacket(ep0ring,&packet);
	ring(slot,1);
	if(waitxhci(slot,1)<0) goto failed;


	//----------------得到hub描述符-----------------
	say("1.hub desc@%x\n",data0+0x400);
	packet.bmrequesttype=0xa0;
	packet.brequest=6;
	packet.wvalue=0x2900;
	packet.windex=0;
	packet.wlength=8;
	packet.buffer=data0+0x400;
	sendpacket(ep0ring,&packet);
	ring(slot,1);
	if(waitxhci(slot,1)<0) goto failed;
	explainhub(data0+0x400);

	u64 count=*(u8*)(data0+0x402);
	if(count==0) goto failed;

	//to do:support mtt??




	//-------------这是一个hub,修改部分context---------------
	//say("2.evaluate...",0);

	//slot context
	readcontext(slotcontext,1,&context);
	context.d0|=(1<<26);		//is hub
	context.d1|=(count<<24);	//howmany hub this hub has
	context.d2|=0;		//(ttt<<16)+(ttportnum<<8)+(tthubslot);
	writecontext(slotcontext,1,&context);

	hostcommand(slotcontext,0,0, (slot<<24)+(13<<10) );
	if(waitxhci(0,0x21)<0) goto failed;

	slotstate=(*(u32*)(slotcontext+0x80c))>>27; //if2,addressed
	if(slotstate==2) say("slot evaluated");
	else say("slot state:%x\n",slotstate);





	//change input context&ep1.1 context
	//say("3.configure ep1.1..",0);
	context.d0=0;
	context.d1=9;
	context.d2=0;
	context.d3=0;
	context.d4=0;
	writecontext(slotcontext,0,&context);

	fixinterval(&interval,speed);
	context.d0=interval<<16;
	context.d1=(wmaxpacket<<16)+(7<<3)+(3<<1);
	context.d2=ep1in+1;
	context.d3=0;
	context.d4=0x80008;
	writecontext(slotcontext,4,&context);		//ep1.1

	hostcommand(slotcontext,0,0, (slot<<24)+(12<<10) );
	if(waitxhci(0,0x21)<0) goto failed;

	slotstate=(*(u32*)(slotcontext+0x80c))>>27;
	epstate=(*(u32*)(slotcontext+0x860))&0x3;
	if(slotstate==3) say("slot configured");
	else say("slot state:%x\n",slotstate);
	if(epstate==0){
		say("ep3 wrong");
		goto failed;
	}




	//----------prepare ep1.1 ring-------------
	say("4.ep1.1 ring");
	u32* temp=(u32*)ep1in;
	int i=0;
	for(i=0;i<0x3fc;i+=4)		//0xff0/4=0x3fc
	{
		temp[i]=data11+i*4;
		temp[i+1]=0;
		temp[i+2]=1;
		temp[i+3]=0x21+(1<<10);
	}
	temp[i]=ep1in;
	temp[i+1]=0;
	temp[i+2]=0;
	temp[i+3]=1+(6<<10);

	//敲门铃，开始执行
	ring(slot,3);
	//------------------------------------------




	//------------------set port feathre-------------
	u64 childport;
	for(childport=1;childport<=count;childport++)
	{
		say("child port:%x\n",childport);

		//say("resetting...",0);
		packet.bmrequesttype=0x23;	//host2dev|class|rt_other
		packet.brequest=3;		//set feathre
		packet.wvalue=0x4;		//f_port_reset
		packet.wlength=0;		//0
		packet.buffer=0;
		packet.windex=childport;	//(??<<8)|(childport+1)
		sendpacket(ep0ring,&packet);
		ring(slot,1);
		if(waitxhci(slot,1)<0) goto failed;

		//say("getting status...",0);
		packet.bmrequesttype=0xa3;	//devhost|class|rt_other
		packet.brequest=0;		//req_get_status
		packet.wvalue=0;		//0
		packet.wlength=4;		//0
		packet.windex=childport;	//childport+1
		packet.buffer=data0+0x600;	//某个地址
		sendpacket(ep0ring,&packet);
		ring(slot,1);
		if(waitxhci(slot,1)<0) goto failed;

		u32 status=*(u32*)(data0+0x600);
		say("status:%x\n",status);

		if( (status&1) == 1 )
		{
			//---------第0步:enable？reset？------------
			//----------------------------------------




			//---------第一步:初始化，读取并记录设备信息--------
			hello(rootport,childport,1+(status>>9)&3);
			//------------------------------------




			//---------第二步:是hub就交给hub()----------
			//---------不是hub就把这个初始化完的东西放着不管----
			if(classcode==9&&interfaceclass==9)
			{
				normalhub(rootport,childport,speed,slot);
			}
			//------------------------------------
		}
	}




failed:
	return;
}
*/




static u64 portbase;
static u64 portcount;
int resetport(volatile u32* childaddr)
{
	int i;

	//gcc please i am begging you......do not try to optimize my code......
	childaddr[0]=childaddr[0]|0x10;

	//wait for PR=0
	i=0;
	while(1)
	{
		i++;
		if(i>0xffffffff)return -1;

		//portsc=*(u32*)(childaddr);
		if( (childaddr[0] & 0x10) == 0 )break;
	}
	say("pr done\n");

	//wait for enable=1
	i=0;
	while(1)
	{
		i++;
		if(i>0xffffffff)return -3;

		//portsc=*(u32*)(childaddr);
		if( (childaddr[0] & 0x2) == 0x2 )break;
	}
	say("enable done\n");

	//wait for portchange event
	if(waitxhci(0,0x22) <= 0)return -2;
	say("event done\n");

	//nothing wrong haha
	return 1;
}
void roothub()
{
	//用xhci spec里面的root port reset等办法处理port,得到port信息
	u64 childport;
	volatile u32* childaddr;
	for(childport=1;childport<=portcount;childport++)
	{
		//gcc please i am begging you......do not try to optimize my code......
		//check
		childaddr=(u32*)(portbase+childport*0x10-0x10);
		say("root port:%x@%x{\n",childport,childaddr);
		say("portsc(before):%x\n",childaddr[0]);
		if( ( childaddr[0] & 0x1 ) == 0)goto thisfinish;




		//进来再判断enable位(bit2)
		//=1说明大概是3.0设备,否则是<=2.0的设备需要手动reset
		//reset之后，从默认的disable状态变成enable状态
		//收到change event的时候仍然没有reset好,要一直等到bit4=0
		if( ( childaddr[0] & 0x2 ) == 0)
		{
			if( resetport(childaddr) <= 0 )
			{
				say("reset failed\n");
				goto thisfinish;
			}
			say("portsc(reset1):%x\n",childaddr[0]);
		}
		//todo:检查错误
		//---------------------------------------------




		//---------第一步:初始化，读取并记录基本信息--------
		say("	linkstate:%x\n",( childaddr[0] >> 5 ) & 0xf);

		u32 speed=( childaddr[0] >> 10 ) & 0xf;
		if(speed == 4)say("	superspeed:4\n");
		else if(speed ==3)say("	highspeed:3\n");
		else if(speed == 2)say("	fullspeed:2\n");
		else if(speed == 1)say("	lowspeed:1\n");
		else say("	speed:%x\n",speed);
		hello(childport,0,speed);
		//---------------------------------------




		//---------第二步:是hub就交给hub(),键鼠就交给inputer()----------
		//---------否则就放在那不管,直接跑去处理下一个port----
		//if(classcode==9&&interfaceclass==9)
		//{
		//	normalhub(childport,0,(portsc>>10)&0xf,slot);
		//}
		//-------------------------------------




thisfinish:
		say("}\n");
	}
	return;
}




void initusb(u64 xhciaddr)
{
	//拿到xhci的mmio地址
	if(xhciaddr==0) return;
	say("xhci@%x{\n",xhciaddr);




	//清空usbhome,eventringdequeue
	char* p=(char*)usbhome;
	int i;
	for(i=0;i<0x100000;i++) p[i]=0;

	eventringdequeue=eventringhome;
	eventringcycle=1;
	cmdringenqueue=cmdringhome;
	cmdringcycle=1;




	//runtime
	volatile u64 temp=*(u32*)(xhciaddr+0x18);
	runtime=xhciaddr+temp;
	say("	runtime@%x\n",runtime);




	//doorbell位置
	temp=(*(u32*)(xhciaddr+0x14));
	doorbell=xhciaddr+temp;
	say("	doorbell@%x\n",doorbell);




	//port们集中在哪儿,总共多少个port
	temp=(*(u32*)xhciaddr) & 0xffff;	//caplength
	portbase=xhciaddr+temp+0x400;
	say("	portbase@%x\n",portbase);

	temp=*(u32*)(xhciaddr+4);		//hcsparams1
	portcount=temp>>24;
	say("	portcount:%x\n",portcount);




	//contextsize
	temp=*(u32*)(xhciaddr+0x10);		//capparams
	contextsize=0x20;
	if((temp&0x4) == 0x4) contextsize*=2;
	say("	contextsize:%x\n",contextsize);
	say("}\n");




	//从根开始
	roothub();
}
