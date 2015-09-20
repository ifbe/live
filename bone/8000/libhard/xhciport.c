#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long

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




void diary(char* , ...);




volatile static QWORD contextsize;

volatile static QWORD eventringdequeue;
volatile static QWORD eventringcycle;
volatile static QWORD runtime;

volatile static QWORD cmdringenqueue;
volatile static QWORD cmdringcycle;
volatile static QWORD doorbell;




struct setup{
	BYTE bmrequesttype;	//	0x80
	BYTE brequest;		//	0x6
	WORD wvalue;		//	(descriptor type>>8)+descriptor index
	WORD windex;		//	0
	WORD wlength;		//	length

	QWORD buffer;
};
static struct setup packet;
void sendpacket(QWORD ringaddr)
{
	//得到这次往哪儿写
	QWORD enqueue=*(QWORD*)(ringaddr+0xff0);
	DWORD* p=(DWORD*)(ringaddr+enqueue);

	//diary("(enqueue before:%x)\n",enqueue);
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
	*(QWORD*)(ringaddr+0xff0)=enqueue;
	//diary("(enqueue after:%x)\n",enqueue);
}
void usbcommand(BYTE bmrequesttype,BYTE brequest,WORD wvalue,WORD windex,WORD wlength,QWORD buffer)
{
	packet.bmrequesttype=bmrequesttype;
	packet.brequest=brequest;
	packet.wvalue=wvalue;
	packet.windex=windex;
	packet.wlength=wlength;
	packet.buffer=buffer;
}




//直接在command ring上写TRB（1个TRB=4个DWORD）,然后处理enqueue指针，最后ring
void hostcommand(DWORD d0,DWORD d1,DWORD d2,DWORD d3)
{
	DWORD* pointer=(DWORD*)(cmdringenqueue);

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
	*(DWORD*)(doorbell)=0;
}







void ring(int slot,int endpoint)
{
	*(DWORD*)(doorbell+4*slot)=endpoint;
}





int waitevent(QWORD wantedslot,QWORD wantedtype)
{
	volatile DWORD* thisevent;
	QWORD timeout=0;
	while(1)
	{
		thisevent=(DWORD*)eventringdequeue;

		//等太久就不等了
		timeout++;
		if(timeout>0xffffff)
		{
			diary("(timeout@%x)\n",thisevent);
			return -1;
		}

	


		//拿出一个,cycle不对就回到while(1)
		if( (thisevent[3] & 0x1) != eventringcycle ) continue;

	


		//cycle对了，检查completion code
		//正常就往下走，否则返回失败
		if( (thisevent[2] >> 24) != 0x1 )
		{
			diary("(error event@%x)%x,%x,%x,%x\n",thisevent,
			thisevent[0],thisevent[1],thisevent[2],thisevent[3]);
			return -1;
		}




		//没出错,给xhci报告当前erdp
		eventringdequeue+=0x10;
		//*(DWORD*)(runtime+0x38)=eventringdequeue | 8;
		//*(DWORD*)(runtime+0x3c)=0;

	


		//如果想等host事件
		DWORD slot=thisevent[3]>>24;
		DWORD type=(thisevent[3]>>10)&0x3f;
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
//static QWORD vendor;
//static QWORD product;
//static QWORD classcode;
//static QWORD interfaceclass;
//static QWORD deviceprotocol;

//static QWORD wmaxpacket;
//static QWORD interval;
//static BYTE stringtoread[8];
void explaindescriptor(QWORD addr)
{
	DWORD type=*(BYTE*)(addr+1);
	if(type==0)return;
	//diary("type:%x\n",type);
	//if(	(type==0)|((type>7)&(type<0x21))|(type>0x29)	) return;

	volatile DWORD i;
	switch(type)
	{
	case 1:	//设备描述符
	{
		//这是第一个被读取的描述符，所以几个变量在这里清零最好
		//classcode=0;	//这个就不必了，马上就变掉
		//for(i=0;i<8;i++) stringtoread[i]=0;

		diary("(desc 1)device@%x{\n",addr);
		//diary("blength:%x\n",*(BYTE*)addr);
		//diary("type:%x\n",*(BYTE*)(addr+1));
		//diary("bcdusb:%x\n",*(WORD*)(addr+2));

		//classcode=*(BYTE*)(addr+4);			//!
		//diary("bclass:%x\n",classcode);

		//diary("bsubclass:%x\n",*(BYTE*)(addr+5));

		//deviceprotocol=*(BYTE*)(addr+6);		//!
		//diary("bprotocol:%x\n",deviceprotocol);

		diary("	bpacketsize:%x\n",*(BYTE*)(addr+7));
		diary("	vendor:%x\n",*(WORD*)(addr+8));
		diary("	product:%x\n",*(WORD*)(addr+0xa));

		//diary("	bcddevice:%x\n",*(WORD*)(addr+0xc));

		//i=*(BYTE*)(addr+0xe);
		//stringtoread[i]=1;
		//diary("	imanufacturer:%x\n",i);

		//i=*(BYTE*)(addr+0xf);
		//stringtoread[i]=1;
		//diary("	iproduct:%x\n",i);

		//i=*(BYTE*)(addr+0x10);
		//stringtoread[i]=1;
		//diary("	iserial:%x\n",i);

		diary("	bnumconf:%x\n",*(BYTE*)(addr+0x11));
		diary("}\n");
		break;
	}
	case 2:	//配置描述符
	{
		diary("(desc 2)conf@%x{\n",addr);
		//diary("	blength:%x\n",*(BYTE*)addr);
		//diary("	type:%x\n",*(BYTE*)(addr+1));
		//diary("	wlength:%x\n",*(WORD*)(addr+2));
		diary("	bnuminterface:%x\n",*(BYTE*)(addr+4));
		//diary("	bvalue:%x\n",*(BYTE*)(addr+5));
		//i=*(BYTE*)(addr+0x6);
		//stringtoread[i]=1;
		//diary("	i:",i);
		//BYTE bmattribute=*(BYTE*)(addr+7);
		//diary("	bmattribute:%x\n",bmattribute);
		//if( (bmattribute&0x40) ==0x40 ) diary("	self powered");
		//if( (bmattribute&0x20) ==0x20 ) diary("	remote wake");
		//diary("	bmaxpower:%x\n",*(BYTE*)(addr+8));

		DWORD totallength=*(WORD*)(addr+2);
		diary("	wlength:%x\n",totallength);
		DWORD offset=9;
		DWORD nextlen,nexttype;
		while(1)
		{
			//偏移超过总长度肯定错了
			if(offset>=totallength) break;

			//拿出下一个的长度和种类
			//用的递归,所以要是里面再出现type2的错误desc,可能会死着...
			nextlen=*(BYTE*)(addr+offset);
			nexttype=*(BYTE*)(addr+offset);
			//diary("	@%x:(%x,%x)\n",offset,nextlen,nexttype);
			if(nexttype==2)break;

			//一切正常,解释下一个表
			explaindescriptor(addr+offset);

			//下一轮循环
			offset+=nextlen;
		}

		diary("}\n");
		break;
	}
	case 3:	//接口描述符
	{
		diary("(desc 3)string@%x{\n",addr);

		diary("	blength:%x\n",*(BYTE*)(addr));
		WORD language=*(WORD*)(addr+2);
		diary("	language:%x\n",language);

		diary("}\n");
		break;
	}
	case 4:	//接口描述符
	{
		diary("	(desc 4)interface@%x{\n",addr);
		//diary("	length:%x\n",*(BYTE*)addr);
		//diary("	type:%x\n",*(BYTE*)(addr+1));
		diary("		binterfacenum:%x\n",*(BYTE*)(addr+2));
		diary("		balternatesetting:%x\n",*(BYTE*)(addr+3));
		diary("		bnumendpoint:%x\n",*(BYTE*)(addr+4));

		//interfaceclass=*(BYTE*)(addr+5);
		//diary("		bclass:%x\n",interfaceclass);
		//diary("		bsubclass:%x\n",*(BYTE*)(addr+6));
		//diary("		bprotol:%x\n",*(BYTE*)(addr+7));

		//i=*(BYTE*)(addr+0x8);
		//stringtoread[i]=1;
		//diary("		i:%x\n",i);

		diary("	}\n");
		break;
	}
	case 5:	//端点描述符
	{
		diary("	(desc 5)endpoint@%x{\n",addr);
		//diary("		blength:%x\n",*(BYTE*)addr);
		//diary("		type:%x\n",*(BYTE*)(addr+1));

		BYTE endpoint=*(BYTE*)(addr+2);
		BYTE eptype=*(BYTE*)(addr+3);
		diary("		ep:%x\n",endpoint&0xf);
		if(endpoint>0x80)
		{
			if(eptype==0) diary("		in,control");
			else if(eptype==1) diary("		in,isochronous");
			else if(eptype==2) diary("		in,bulk");
			else diary("		in,interrupt");
		}
		else
		{
			if(eptype==0) diary("		out,control");
			else if(eptype==1) diary("		out,isochronous");
			else if(eptype==2) diary("		out,bulk");
			else diary("		out,interrupt");
		}


		//wmaxpacket=*(WORD*)(addr+4);
		//diary("	wmaxpacket:%x\n",wmaxpacket);
		//interval=*(BYTE*)(addr+6);
		//diary("	binterval:%x\n",interval);

		diary("	}\n");
		break;
	}
	case 6:	//设备限定描述符
	{
		diary("	(desc 6)@%x{\n",addr);
		//diary("	blength:%x\n",*(BYTE*)addr);
		//diary("	bdescriptortype:%x\n",*(BYTE*)(addr+1));
		//diary("	bcdusb:%x\n",*(WORD*)(addr+2));
		//diary("	bdeviceclass:%x\n",*(BYTE*)(addr+4));
		//diary("	bdevicesubclass:%x\n",*(BYTE*)(addr+5));
		//diary("	bdeviceprotocol:%x\n",*(BYTE*)(addr+6));
		//diary("	bmaxpacketsize0:%x\n",*(BYTE*)(addr+7));
		//diary("	bnumconfigurations:%x\n",*(BYTE*)(addr+8));
		break;
	}
	case 7:	//其他速率配置描述符
	{
		diary("	(desc 7)@%x{\n",addr);
		//diary("	blength:%x\n",*(BYTE*)addr);
		//diary("	bdescriptortype:%x\n",*(BYTE*)(addr+1));
		//diary("	wtotallength:%x\n",*(WORD*)(addr+2));
		//diary("	bnuminterfaces:%x\n",*(BYTE*)(addr+4));
		//diary("	bconfigurationvalue:%x\n",*(BYTE*)(addr+5));
		//diary("	iconfiguration:%x\n",*(BYTE*)(addr+6));
		//diary("	bmattributes:%x\n",*(BYTE*)(addr+7));
		//diary("	bmaxpower:%x\n",*(BYTE*)(addr+8));
		break;
	}
	case 0x21:	//hid设备描述符
	{
		diary("	(desc 21)hid@%x{\n",addr);
		//diary("		blength:",*(BYTE*)addr);
		//diary("		type:",*(BYTE*)(addr+1));
		//diary("		bcdhid:",*(WORD*)(addr+2));
		//diary("		bcountrycode:",*(BYTE*)(addr+4));
		//diary("		bnumdescriptor:",*(BYTE*)(addr+5));

		//diary("		btype:",*(BYTE*)(addr+6));
		//diary("		wlength:",*(WORD*)(addr+7));
		//diary("		btype:",*(BYTE*)(addr+9));
		//diary("		wlength:",*(WORD*)(addr+10));

		//reportsize=*(WORD*)(addr+7);

		diary("	}\n");
		break;
	}
	default:
	{
		diary("	(desc ?)unknown@%x{\n",type,addr);
	}
	}
}


void recorddevice(QWORD vidpid,QWORD class,QWORD position,QWORD speed,QWORD slot)
{
	QWORD* p=(QWORD*)(usbhome);
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
void explainhub(QWORD addr)
{
	diary("hub@%x\n",addr);
        diary("blength:%x\n",*(BYTE*)addr);
        diary("type:%x\n",*(BYTE*)(addr+1));
        diary("bnumberofport:%x\n",*(BYTE*)(addr+2));
	WORD hubcharacteristics=*(BYTE*)(addr+3);
        diary("    power mode:%x\n",hubcharacteristics&0x3);
        diary("    compound device:%x\n",(hubcharacteristics>>2)&0x1);
        diary("    overcurrent protect:%x\n",(hubcharacteristics>>3)&0x3);
        diary("    TT thinktime:%x\n",(hubcharacteristics>>5)&0x3);
        diary("    port indicator:%x\n",(hubcharacteristics>>7)&0x1);
        diary("bpowerontopowergood:%x\n",*(BYTE*)(addr+5));
        diary("bhubcontrolcurrent:%x\n",*(BYTE*)(addr+6));
        diary("bremoveandpowermask:%x\n",*(BYTE*)(addr+7));
	diary("",0);
}
*/



/*
void fixinterval(QWORD* interval,QWORD speed)
{
	//if(speed==0) diary("undefined speed:",speed);
	//if(speed==1) diary("full speed:",speed);
	//if(speed==2) diary("low speed:",speed);
	//if(speed==3) diary("high speed:",speed);
	//if(speed==4) diary("super speed:",speed);


	QWORD val=*interval;
	diary("val:%x\n",val);
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
	diary("fixed interval:%x\n",val);		//[3,11]
	*interval=val;
}
*/








//rootport:设备自己所在roothub port号,routestring:一层层地址
//static QWORD slot;
void hello(QWORD rootport,QWORD routestring,DWORD speed)
{
	//diary("device{",0);


	//---------------obtain slot------------------
	//diary("1.enable slot...",0);
	hostcommand(0,0,0, (9<<10) );
	QWORD slot=waitevent(0,0x21);
	if(slot<=0) goto failed;
	if(slot>=0x10)
	{
		diary("bad slotnum:%x\n",slot);
		goto failed;
	}
	diary("slot obtained:%x\n",slot);
	//-------------------------------------------




	//-----------address device-----------------
	QWORD slotcontext=usbhome+slot*0x10000;
	QWORD ep0ring=slotcontext+0x1000;
	QWORD ep1out=slotcontext+0x2000;
	QWORD ep1in=slotcontext+0x3000;

	QWORD data0=slotcontext+0x8000;
	QWORD data11=slotcontext+0x9000;
	QWORD inputcontext=slotcontext+0xf000;
	DWORD maxpacketsize;
	diary("	slot context@%x\n",slotcontext);

	//clear context
	int i=0;
	for(i=0;i<0x10000;i++){ *(BYTE*)(slotcontext+i) == 0; }

	//packetsize
	if(speed==4)maxpacketsize=0x200;
	else if(speed==3)maxpacketsize=0x40;
	else maxpacketsize=8;

	//construct input
	DWORD* pcontext=(DWORD*)inputcontext;
	pcontext[1]=3;

	pcontext=(DWORD*)(inputcontext+contextsize);
	pcontext[0]=(3<<27)+(speed<<20)+routestring;
	pcontext[1]=rootport<<16;

	pcontext=(DWORD*)(inputcontext+contextsize*2);
	pcontext[0]=0;
	pcontext[1]=(maxpacketsize<<16)+(4<<3)+6;
	pcontext[2]=ep0ring+1;
	pcontext[3]=0;
	pcontext[4]=0x8;

	//output context地址填入对应dcbaa
	*(DWORD*)(dcbahome+slot*8)=slotcontext;
	*(DWORD*)(dcbahome+slot*8+4)=0;

	//发送command
	hostcommand(inputcontext,0,0, (slot<<24)+(11<<10) );
	if(waitevent(0,0x21)<0) goto failed;

	//if2,addressed
	DWORD slotstate=(*(DWORD*)(slotcontext+0xc))>>27;
	DWORD epstate=(*(DWORD*)(slotcontext+contextsize))&0x3;
	if(slotstate==2) diary("slot addressed");
	else diary("	slot state:%x\n",slotstate);
	if(epstate == 0)
	{
		diary("	ep0 wrong");
		goto failed;
	}
	diary("	ep0ring@%x\n",ep0ring);
	//------------------------------------------------




	//_______________device descriptor__________________
	//diary("getting device desc(0x12 bytes)......\n");
	packet.bmrequesttype=0x80;
	packet.brequest=6;
	packet.wvalue=0x100;
	packet.windex=0;
	packet.wlength=0x12;
	packet.buffer=data0+0x100;
	sendpacket(ep0ring);
	ring(slot,1);
	if(waitevent(slot,1)<0) goto failed;
	explaindescriptor(data0+0x100);
	QWORD vendorproduct=*(DWORD*)(data0+0x108);
	//___________________________________________________




	//----------------configuration descriptor-----------
	//diary("3.descriptors:",0);
	//[data0+0x100]:configure descriptor
	//diary("getting conf desc......\n");
	packet.bmrequesttype=0x80;
	packet.brequest=6;
	packet.wvalue=0x200;
	packet.windex=0;
	packet.wlength=0x9;
	packet.buffer=data0+0x200;
	sendpacket(ep0ring);
	ring(slot,1);
	if(waitevent(slot,1)<0) goto failed;

	packet.wlength=*(WORD*)(data0+0x102);
	sendpacket(ep0ring);
	ring(slot,1);
	if(waitevent(slot,1)<0) goto failed;
	explaindescriptor(data0+0x200);




	//----------------string descriptors----------------
	//diary("string descriptor...",0);
	packet.bmrequesttype=0x80;
	packet.brequest=6;
	packet.wvalue=0x300;
	packet.windex=0;				//语言id
	packet.wlength=4;
	packet.buffer=data0+0x300;
	sendpacket(ep0ring);
	ring(slot,1);
	if(waitevent(slot,1)<0) goto failed;

	/*
	int wantedindex=1;
	for()
	{
		packet.windex=*(WORD*)(data0+0x302);		//从上一步的结果里取出第一个语言
		packet.wvalue=0x300+wantedindex;			//0x300+这种语言下的索引号
		sendpacket(ep0ring);						//为了拿到实际长度
		ring(slot,1);
		if(waitevent(slot,1)<0) goto failed;

		packet.wlength=*(BYTE*)(data0+0x300);		//从上一步的结果里取出正确的长度
		sendpacket(ep0ring);
		ring(slot,1);
		if(waitevent(slot,1)<0) goto failed;
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
	//diary("}",0);
}







/*
void normalhub(QWORD rootport,QWORD routestring,QWORD speed,QWORD slot)
{
	if(deviceprotocol==0) diary("no TT hub!");
	if(deviceprotocol==1) diary("single TT hub!");
	if(deviceprotocol==2) diary("multi TT hub!");

	QWORD slotcontext=usbhome+slot*0x8000;
	QWORD ep0ring=slotcontext+0x1000;
	QWORD ep1out=slotcontext+0x2000;
	QWORD ep1in=slotcontext+0x3000;

	QWORD data0=slotcontext+0x8000;
	QWORD data11=slotcontext+0x9000;
	QWORD inputcontext=slotcontext+0xf000;

	DWORD slotstate;
	DWORD epstate;
	struct setup packet;
	struct context context;


	diary("4.set configuration...\n");
	packet.bmrequesttype=0;
	packet.brequest=9;
	packet.wvalue=1;
	packet.windex=0;
	packet.wlength=0;
	sendpacket(ep0ring,&packet);
	ring(slot,1);
	if(waitevent(slot,1)<0) goto failed;


	//----------------得到hub描述符-----------------
	diary("1.hub desc@%x\n",data0+0x400);
	packet.bmrequesttype=0xa0;
	packet.brequest=6;
	packet.wvalue=0x2900;
	packet.windex=0;
	packet.wlength=8;
	packet.buffer=data0+0x400;
	sendpacket(ep0ring,&packet);
	ring(slot,1);
	if(waitevent(slot,1)<0) goto failed;
	explainhub(data0+0x400);

	QWORD count=*(BYTE*)(data0+0x402);
	if(count==0) goto failed;

	//to do:support mtt??




	//-------------这是一个hub,修改部分context---------------
	//diary("2.evaluate...",0);

	//slot context
	readcontext(slotcontext,1,&context);
	context.d0|=(1<<26);		//is hub
	context.d1|=(count<<24);	//howmany hub this hub has
	context.d2|=0;		//(ttt<<16)+(ttportnum<<8)+(tthubslot);
	writecontext(slotcontext,1,&context);

	hostcommand(slotcontext,0,0, (slot<<24)+(13<<10) );
	if(waitevent(0,0x21)<0) goto failed;

	slotstate=(*(DWORD*)(slotcontext+0x80c))>>27; //if2,addressed
	if(slotstate==2) diary("slot evaluated");
	else diary("slot state:%x\n",slotstate);





	//change input context&ep1.1 context
	//diary("3.configure ep1.1..",0);
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
	if(waitevent(0,0x21)<0) goto failed;

	slotstate=(*(DWORD*)(slotcontext+0x80c))>>27;
	epstate=(*(DWORD*)(slotcontext+0x860))&0x3;
	if(slotstate==3) diary("slot configured");
	else diary("slot state:%x\n",slotstate);
	if(epstate==0){
		diary("ep3 wrong");
		goto failed;
	}




	//----------prepare ep1.1 ring-------------
	diary("4.ep1.1 ring");
	DWORD* temp=(DWORD*)ep1in;
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
	QWORD childport;
	for(childport=1;childport<=count;childport++)
	{
		diary("child port:%x\n",childport);

		//diary("resetting...",0);
		packet.bmrequesttype=0x23;	//host2dev|class|rt_other
		packet.brequest=3;		//set feathre
		packet.wvalue=0x4;		//f_port_reset
		packet.wlength=0;		//0
		packet.buffer=0;
		packet.windex=childport;	//(??<<8)|(childport+1)
		sendpacket(ep0ring,&packet);
		ring(slot,1);
		if(waitevent(slot,1)<0) goto failed;

		//diary("getting status...",0);
		packet.bmrequesttype=0xa3;	//devhost|class|rt_other
		packet.brequest=0;		//req_get_status
		packet.wvalue=0;		//0
		packet.wlength=4;		//0
		packet.windex=childport;	//childport+1
		packet.buffer=data0+0x600;	//某个地址
		sendpacket(ep0ring,&packet);
		ring(slot,1);
		if(waitevent(slot,1)<0) goto failed;

		DWORD status=*(DWORD*)(data0+0x600);
		diary("status:%x\n",status);

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




static QWORD portbase;
static QWORD portcount;
int resetport(volatile DWORD* childaddr)
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

		//portsc=*(DWORD*)(childaddr);
		if( (childaddr[0] & 0x10) == 0 )break;
	}
	diary("pr done\n");

	//wait for enable=1
	i=0;
	while(1)
	{
		i++;
		if(i>0xffffffff)return -3;

		//portsc=*(DWORD*)(childaddr);
		if( (childaddr[0] & 0x2) == 0x2 )break;
	}
	diary("enable done\n");

	//wait for portchange event
	if(waitevent(0,0x22) <= 0)return -2;
	diary("event done\n");

	//nothing wrong haha
	return 1;
}
void roothub()
{
	//用xhci spec里面的root port reset等办法处理port,得到port信息
	QWORD childport;
	volatile DWORD* childaddr;
	for(childport=1;childport<=portcount;childport++)
	{
		//gcc please i am begging you......do not try to optimize my code......
		//check
		childaddr=(DWORD*)(portbase+childport*0x10-0x10);
		diary("root port:%x@%x{\n",childport,childaddr);
		diary("portsc(before):%x\n",childaddr[0]);
		if( ( childaddr[0] & 0x1 ) == 0)goto thisfinish;




		//进来再判断enable位(bit2)
		//=1说明大概是3.0设备,否则是<=2.0的设备需要手动reset
		//reset之后，从默认的disable状态变成enable状态
		//收到change event的时候仍然没有reset好,要一直等到bit4=0
		if( ( childaddr[0] & 0x2 ) == 0)
		{
			if( resetport(childaddr) <= 0 )
			{
				diary("reset failed");
				goto thisfinish;
			}
			diary("portsc(reset1):%x\n",childaddr[0]);
		}
		//todo:检查错误
		//---------------------------------------------




		//---------第一步:初始化，读取并记录基本信息--------
		diary("	linkstate:%x\n",( childaddr[0] >> 5 ) & 0xf);

		DWORD speed=( childaddr[0] >> 10 ) & 0xf;
		if(speed == 4)diary("	superspeed:4");
		else if(speed ==3)diary("	highspeed:3");
		else if(speed == 2)diary("	fullspeed:2");
		else if(speed == 1)diary("	lowspeed:1");
		else diary("	speed:%x",speed);
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
		diary("}");
	}
	return;
}




void initusb(QWORD xhciaddr)
{
	//拿到xhci的mmio地址
	if(xhciaddr==0) return;
	diary("xhci@%x{\n",xhciaddr);




	//清空usbhome,eventringdequeue
	char* p=(char*)usbhome;
	int i;
	for(i=0;i<0x100000;i++) p[i]=0;

	eventringdequeue=eventringhome;
	eventringcycle=1;
	cmdringenqueue=cmdringhome;
	cmdringcycle=1;




	//runtime
	volatile QWORD temp=*(DWORD*)(xhciaddr+0x18);
	runtime=xhciaddr+temp;
	diary("	runtime@%x\n",runtime);




	//doorbell位置
	temp=(*(DWORD*)(xhciaddr+0x14));
	doorbell=xhciaddr+temp;
	diary("	doorbell@%x\n",doorbell);




	//port们集中在哪儿,总共多少个port
	temp=(*(DWORD*)xhciaddr) & 0xffff;	//caplength
	portbase=xhciaddr+temp+0x400;
	diary("	portbase@%x\n",portbase);

	temp=*(DWORD*)(xhciaddr+4);		//hcsparams1
	portcount=temp>>24;
	diary("	portcount:%x\n",portcount);




	//contextsize
	temp=*(DWORD*)(xhciaddr+0x10);		//capparams
	contextsize=0x20;
	if((temp&0x4) == 0x4) contextsize*=2;
	diary("	contextsize:%x\n",contextsize);
	diary("}");




	//从根开始
	roothub();
}
