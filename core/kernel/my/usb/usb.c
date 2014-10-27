#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long
#define usbinfo 0x150000		//	/usb

#define xhcihome 0x300000
#define dcbahome xhcihome+0x10000
#define cmdringhome xhcihome+0x20000
#define cmdpointer xhcihome+0x30000
#define hostevent xhcihome+0x80000
#define deviceevent xhcihome+0xc0000

#define usbhome 0x500000
	//0:			//+0:		incontext,+800:outcontext
				//+1000:	device infomation
	//1:ep0			//+2000:	ep0 ring
				//+3000:	ep0 buffer
	//2:ep1.0		//+4000:
				//+5000:
	//3:ep1.1		//+6000:	ep1.1 ring
				//+7000:	ep1.1 buffer


void clean(QWORD addr,QWORD size)
{
	BYTE* pointer=(BYTE*)addr;
	int i;
	for(i=0;i<size;i++) pointer[i]=0;
}


volatile static QWORD doorbell;
void ring(int slot,int endpoint)
{
	*(DWORD*)(doorbell+4*slot)=endpoint;
}


//直接在command ring上写TRB（1个TRB=4个DWORD）,然后处理enqueue指针，最后ring
void command(DWORD d0,DWORD d1,DWORD d2,DWORD d3)
{
	QWORD enqueue=*(QWORD*)(cmdpointer);
	DWORD* pointer=(DWORD*)(cmdringhome+enqueue);

	QWORD cycle=*(QWORD*)(cmdpointer+8);
	cycle=(cycle+1)%2;
	//say("command ring@",commandenqueue);

	//写ring
	pointer[0]=d0;
	pointer[1]=d1;
	pointer[2]=d2;
	pointer[3]=d3|cycle;

	//enqueue指针怎么变
	*(QWORD*)(cmdpointer)=enqueue+0x10;

	//处理完了，敲门铃
	ring(0,0);
}


struct setup{
	BYTE bmrequesttype;	//	0x80
	BYTE brequest;		//	0x6
	WORD wvalue;		//	(descriptor type>>8)+descriptor index
	WORD windex;		//	0
	WORD wlength;		//	length

	QWORD buffer;
};
void sendpacket(QWORD ringaddr,struct setup* packet)
{
	//得到这次往哪儿写
	QWORD temp=*(QWORD*)(ringaddr+0xff0);
	DWORD* p=(DWORD*)(ringaddr+temp);

	if(packet->bmrequesttype>=0x80)
	{
	//setup stage
	p[0]=(packet->wvalue<<16)+(packet->brequest<<8)+packet->bmrequesttype;
	p[1]=(packet->wlength<<16)+packet->windex;
	p[2]=8;
	p[3]=(3<<16)+(2<<10)+(1<<6)+1;

	//data stage
	p[4]=packet->buffer;
	p[5]=0;
	p[6]=packet->wlength;
	p[7]=(1<<16)+(3<<10)+1;

	//status stage
	p[8]=p[9]=p[0xa]=0;
	p[0xb]=(4<<10)+(1<<5)+1;

	//下次往哪儿写
	temp+=0x30;
	if(temp>0xf00)	temp=0;
	*(QWORD*)(ringaddr+0xff0)=temp;
	}
	else
	{
	//setup stage
	p[0]=(packet->wvalue<<16)+(packet->brequest<<8)+packet->bmrequesttype;
	p[1]=(packet->wlength<<16)+packet->windex;
	p[2]=8;
	p[3]=(2<<10)+(1<<6)+1;

	//status stage
	p[4]=p[5]=p[6]=0;
	p[7]=(1<<16)+(4<<10)+(1<<5)+1;	//in

	//下次往哪儿写
	temp+=0x20;
	if(temp>0xf00)	temp=0;
	*(QWORD*)(ringaddr+0xff0)=temp;
	}
}


struct context{		//传参太麻烦...
	DWORD d0;
	DWORD d1;
	DWORD d2;
	DWORD d3;
	DWORD d4;
	DWORD d5;
	DWORD d6;
	DWORD d7;
};
static QWORD contextsize;
void readcontext(QWORD baseaddr,int dci,struct context* context)
{
	DWORD* pointer=(DWORD*)(baseaddr+dci*contextsize);

	context->d0=pointer[0];
	context->d1=pointer[1];
	context->d2=pointer[2];
	context->d3=pointer[3];
	context->d4=pointer[4];
}
void writecontext(QWORD baseaddr,int dci,struct context* context)
{
	DWORD* pointer=(DWORD*)(baseaddr+dci*contextsize);

	pointer[0]=context->d0;
	pointer[1]=context->d1;
	pointer[2]=context->d2;
	pointer[3]=context->d3;
	pointer[4]=context->d4;
}


int waithost(QWORD wantedtype)
{
        volatile QWORD timeout=0;
        while(1)
        {
                timeout++;					//超时不管了
                if(timeout>0xfffffff){
                        say("    timeout!",0);
                        return -1;
                }

		QWORD* enqueue=(QWORD*)(hostevent+0xfff0);
		QWORD* dequeue=(QWORD*)(hostevent+0xfff8);

		if(enqueue[0] == dequeue[0]) continue;		//空队，继续等

		DWORD* p=(DWORD*)(hostevent+dequeue[0]);	//取得现在的
		dequeue[0]=(0x10+dequeue[0])%0xfff0;		//这个取完了

		QWORD type=(p[3]>>10)&0x3f;
		if(type == wantedtype)				//正是想要的
		{
			if(type==0x21)
			{
				//如果是command complete
				//就直接返回slot号
				//enable slot需要得到这个slot号
				//这个值肯定大于0
				return p[3] >> 24;
			}
			else
			{
				return 1;
			}
		}
	}
}


int waitdevice(QWORD slot,QWORD endpoint)
{
        volatile QWORD timeout=0;
	QWORD queueaddr=deviceevent+slot*0x4000+endpoint*0x1000;
	while(1)
        {
                timeout++;					//超时不管了
                if(timeout>0xfffffff){
                        say("    timeout!",0);
                        return -1;
                }

		QWORD* enqueue=(QWORD*)(queueaddr+0xff0);
		QWORD* dequeue=(QWORD*)(queueaddr+0xff8);

		if(enqueue[0] == dequeue[0]) continue;		//空队，继续等

		DWORD* p=(DWORD*)(queueaddr+dequeue[0]);	//取得现在的
		dequeue[0]=(0x10+dequeue[0])%0xff0;		//这个取完了

		return 1;
	}
}


//以下是提取自descriptor的最重要信息，每次只被这个函数更新,就像人记笔记......
static QWORD vendor;
static QWORD product;
static QWORD classcode;
static QWORD interfaceclass;
static QWORD deviceprotocol;

static QWORD wmaxpacket;
static QWORD interval;
//static BYTE stringtoread[8];
void explaindescriptor(QWORD addr)
{
	DWORD type=*(BYTE*)(addr+1);
	if(	(type==0)|((type>7)&(type<0x21))|(type>0x29)	) return;

	volatile DWORD i;
	switch(type)
	{
	case 1:	//设备描述符
	{
		//这是第一个被读取的描述符，所以几个变量在这里清零最好
		//classcode=0;	//这个就不必了，马上就变掉
		//for(i=0;i<8;i++) stringtoread[i]=0;
		interfaceclass=0;
		wmaxpacket=0;
		interval=0;

		say("device@",addr);
		//say("blength:",*(BYTE*)addr);
		//say("type:",*(BYTE*)(addr+1));
		//say("bcdusb:",*(WORD*)(addr+2));

		classcode=*(BYTE*)(addr+4);			//!
		//say("bclass:",classcode);

		//say("bsubclass:",*(BYTE*)(addr+5));

		deviceprotocol=*(BYTE*)(addr+6);		//!
		//say("bprotocol:",deviceprotocol);

		//say("bpacketsize:",*(BYTE*)(addr+7));

		vendor=*(WORD*)(addr+8);			//!
		//say("vendor:",vendor);

		product=*(WORD*)(addr+0xa);			//!
		//say("product:",product);

		//say("bcddevice:",*(WORD*)(addr+0xc));

		//i=*(BYTE*)(addr+0xe);
		//stringtoread[i]=1;
		//say("imanufacturer:",i);

		//i=*(BYTE*)(addr+0xf);
		//stringtoread[i]=1;
		//say("iproduct:",i);

		//i=*(BYTE*)(addr+0x10);
		//stringtoread[i]=1;
		//say("iserial:",i);

		//say("bnumconf:",*(BYTE*)(addr+0x11));
		//say("",0);

		break;
	}
	case 2:	//配置描述符
	{
		say("conf@",addr);
		//say("blength:",*(BYTE*)addr);
		//say("type:",*(BYTE*)(addr+1));
		//say("wlength:",*(WORD*)(addr+2));
		//say("bnuminterface:",*(BYTE*)(addr+4));
		//say("bvalue:",*(BYTE*)(addr+5));
		//i=*(BYTE*)(addr+0x6);
		//stringtoread[i]=1;
		//say("i:",i);
		//BYTE bmattribute=*(BYTE*)(addr+7);
		//say("bmattribute:",bmattribute);
		//if( (bmattribute&0x40) ==0x40 ) say("    self powered",0);
		//if( (bmattribute&0x20) ==0x20 ) say("    remote wake",0);
		//say("bmaxpower:",*(BYTE*)(addr+8));
		//say("",0);

		DWORD totallength=*(WORD*)(addr+2);
		DWORD offset=0;
		while(1)
		{
			if(offset>totallength) break;
			if( *(BYTE*)(addr+offset) <8) break;
			offset+=*(BYTE*)(addr+offset);
			explaindescriptor(addr+offset);
		}

		break;
	}
	case 4:	//接口描述符
	{
		say("interface@",addr);
		//say("length:",*(BYTE*)addr);
		//say("type:",*(BYTE*)(addr+1));
		//say("binterfacenum:",*(BYTE*)(addr+2));
		//say("balternatesetting:",*(BYTE*)(addr+3));
		//say("bnumendpoint:",*(BYTE*)(addr+4));

		interfaceclass=*(BYTE*)(addr+5);
		//say("bclass:",interfaceclass);
		//say("bsubclass:",*(BYTE*)(addr+6));
		//say("bprotol:",*(BYTE*)(addr+7));

		//i=*(BYTE*)(addr+0x8);
		//stringtoread[i]=1;
		//say("i:",i);
		//say("",0);

		break;
	}
	case 5:	//端点描述符
	{
		say("endpoint@",addr);
		//say("blength:",*(BYTE*)addr);
		//say("type:",*(BYTE*)(addr+1));

		//BYTE endpoint=*(BYTE*)(addr+2);
		//say("    endpoint:",endpoint&0xf);
		//if(endpoint>0x80){say("    in",0);}
		//else{say("    out",0);}

		//BYTE eptype=*(BYTE*)(addr+3);
		//if(eptype==0) say("    control",0);
		//else if(eptype==1) say("    isochronous",0);
		//else if(eptype==2) say("    bulk",0);
		//else say("    interrupt",0);

		wmaxpacket=*(WORD*)(addr+4);
		//say("wmaxpacket:",wmaxpacket);
		interval=*(BYTE*)(addr+6);
		//say("binterval:",interval);
		//say("",0);

		break;
	}
/*
	case 6:	//设备限定描述符
	{
		say("    blength:",*(BYTE*)addr);
		say("    bdescriptortype:",*(BYTE*)(addr+1));
		say("    bcdusb:",*(WORD*)(addr+2));
		say("    bdeviceclass:",*(BYTE*)(addr+4));
		say("    bdevicesubclass:",*(BYTE*)(addr+5));
		say("    bdeviceprotocol:",*(BYTE*)(addr+6));
		say("    bmaxpacketsize0:",*(BYTE*)(addr+7));
		say("    bnumconfigurations:",*(BYTE*)(addr+8));
		say("",0);
		break;
	}
	case 7:	//其他速率配置描述符
	{
		say("    blength:",*(BYTE*)addr);
		say("    bdescriptortype:",*(BYTE*)(addr+1));
		say("    wtotallength:",*(WORD*)(addr+2));
		say("    bnuminterfaces:",*(BYTE*)(addr+4));
		say("    bconfigurationvalue:",*(BYTE*)(addr+5));
		say("    iconfiguration:",*(BYTE*)(addr+6));
		say("    bmattributes:",*(BYTE*)(addr+7));
		say("    bmaxpower:",*(BYTE*)(addr+8));
		say("",0);
		break;
	}
*/
	case 0x21:	//hid设备描述符
	{
		say("hid@",addr);
		//say("blength:",*(BYTE*)addr);
		//say("type:",*(BYTE*)(addr+1));
		//say("bcdhid:",*(WORD*)(addr+2));
		//say("bcountrycode:",*(BYTE*)(addr+4));
		//say("bnumdescriptor:",*(BYTE*)(addr+5));

		//say("btype:",*(BYTE*)(addr+6));
		//say("wlength:",*(WORD*)(addr+7));
		//say("btype:",*(BYTE*)(addr+9));
		//say("wlength:",*(WORD*)(addr+10));

		//reportsize=*(WORD*)(addr+7);
		//say("",0);

		break;
	}
	}
}


void recorddevice(QWORD vidpid,QWORD class,QWORD position,QWORD speed,QWORD slot)
{
	QWORD* p=(QWORD*)usbinfo;
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


void explainhub(QWORD addr)
{
	say("hub@",addr);
        say("blength:",*(BYTE*)addr);
        say("type:",*(BYTE*)(addr+1));
        say("bnumberofport:",*(BYTE*)(addr+2));
	WORD hubcharacteristics=*(BYTE*)(addr+3);
        say("    power mode:",hubcharacteristics&0x3);
        say("    compound device:",(hubcharacteristics>>2)&0x1);
        say("    overcurrent protect:",(hubcharacteristics>>3)&0x3);
        say("    TT thinktime:",(hubcharacteristics>>5)&0x3);
        say("    port indicator:",(hubcharacteristics>>7)&0x1);
        say("bpowerontopowergood:",*(BYTE*)(addr+5));
        say("bhubcontrolcurrent:",*(BYTE*)(addr+6));
        say("bremoveandpowermask:",*(BYTE*)(addr+7));
	say("",0);
}


void fixinterval(QWORD* interval,QWORD speed)
{
	//if(speed==0) say("undefined speed:",speed);
	//if(speed==1) say("full speed:",speed);
	//if(speed==2) say("low speed:",speed);
	//if(speed==3) say("high speed:",speed);
	//if(speed==4) say("super speed:",speed);


	QWORD val=*interval;
	say("val:",val);
	if( (speed==1)|(speed==2) )
	{
	if(val == 1) val=3;
	else if(val == 2) val=4;
	else if( (val>=3) & (val<=4) ) val=5;
	else if( (val>=5) & (val<=8) ) val=6;
	else if( (val>=9) & (val<=16) ) val=7;
	else if( (val>=17) & (val<=32) ) val=8;
	else if( (val>=33) & (val<=64) ) val=9;
	else if( (val>=65) & (val<=128) ) val=0xa;
	else if( (val>=129) & (val<256) ) val=0xb;
	}
	say("fixed interval:",val);		//[3,11]
	*interval=val;
}








//rootport:设备自己所在roothub port号,routestring:一层层地址
static QWORD slot;
void hello(QWORD rootport,QWORD routestring,DWORD speed)
{
	//say("device",0);
	//say("{",0);


	//---------------obtain slot------------------
	//say("1.enable slot...",0);
	slot=0;

	command(0,0,0, (9<<10) );

	int result=waithost(0x21);
	if(result<0) goto failed;

	slot=result;
	if(slot>=0x10){
		say("bad slotnum",slot);
		goto failed;
	}
	say("obtained slot:",slot);
	//-------------------------------------------




	//-------------slot initialization----------------
	QWORD slotcontext=usbhome+slot*0x8000;
	QWORD devinfo=slotcontext+0x1000;
	QWORD ep0=slotcontext+0x2000;
	QWORD data0=slotcontext+0x3000;
	QWORD ep11=slotcontext+0x6000;
	QWORD data11=slotcontext+0x7000;
	DWORD slotstate;
	DWORD epstate;
	struct setup packet;
	struct context context;

	//say("2.initing structure...",0);
	say("context@",slotcontext);
	//say("    ep0@",ep0);
	//say("    ep1.1@",ep11);

	//1)clear context
	clean(slotcontext,0x8000);
	//construct input
	context.d0=0;
	context.d1=3;
	context.d2=0;
	context.d3=0;
	context.d4=0;
	writecontext(slotcontext,0,&context);

        context.d0=(3<<27)+(speed<<20)+routestring;
	context.d1=rootport<<16;
	writecontext(slotcontext,1,&context);	//slot context

	context.d0=0;
	context.d1=(8<<16)+(4<<3)+6;
	context.d2=ep0+1;
	context.d3=0;
	context.d4=0x8;
	writecontext(slotcontext,2,&context);	//ep0

	//output context地址填入对应dcbaa
	*(DWORD*)(dcbahome+slot*8)=slotcontext+0x800;
	*(DWORD*)(dcbahome+slot*8+4)=0;
	//------------------------------------




	//-----------address device-----------------
	//say("2.address device...",0);
	command(slotcontext,0,0, (slot<<24)+(11<<10) );
	if(waithost(0x21)<0) goto failed;

	slotstate=(*(DWORD*)(slotcontext+0x80c))>>27; //if2,addressed
	epstate=(*(DWORD*)(slotcontext+0x820))&0x3;
	if(slotstate==2) say("slot addressed",0);
	else say("slot state:",slotstate);
	if(epstate==0){
		say("ep0 wrong",0);
		goto failed;
	}

	//first read(to obtain maxsize)
	//say("getting packetsize....",0);
	packet.bmrequesttype=0x80;
	packet.brequest=6;
	packet.wvalue=0x100;
	packet.windex=0;
	packet.wlength=8;
	packet.buffer=data0;
	sendpacket(ep0,&packet);
	ring(slot,1);
	if(waitdevice(slot,1)<0) goto failed;

	DWORD packetsize=*(BYTE*)(data0+7);
	say("got packetsize:",packetsize);

	//evaluate
        //say("evaluating...",0);
	context.d0=0;
	context.d1=(packetsize<<16)+(4<<3)+6;
	context.d2=ep0+1;
	context.d3=0;
	context.d4=0x40;
	writecontext(slotcontext,2,&context);	//ep0

	command(slotcontext,0,0, (slot<<24)+(13<<10) );
	if(waithost(0x21)<0) goto failed;

	slotstate=(*(DWORD*)(slotcontext+0x80c))>>27; //if2,addressed
	epstate=(*(DWORD*)(slotcontext+0x820))&0x3;
	if(slotstate==2) say("slot evaluated",0);
	else say("slot state:",slotstate);
	if(epstate==0){
		say("ep0 wrong",0);
		goto failed;
	}
	//--------------------------------------------




	//------------------get descriptor-----------------
	//say("3.descriptors:",0);

	//[data0]:device descriptor
	say("device desc@",data0);
	packet.bmrequesttype=0x80;
	packet.brequest=6;
	packet.wvalue=0x100;
	packet.windex=0;
	packet.wlength=0x12;
	packet.buffer=data0;
	sendpacket(ep0,&packet);
	ring(slot,1);
	if(waitdevice(slot,1)<0) goto failed;

	explaindescriptor(data0);


	say("config desc@",data0+0x100);
	//[data0+0x100]:configure descriptor
	packet.bmrequesttype=0x80;
	packet.brequest=6;
	packet.wvalue=0x200;
	packet.windex=0;
	packet.wlength=0x9;
	packet.buffer=data0+0x100;
	sendpacket(ep0,&packet);
	ring(slot,1);
	if(waitdevice(slot,1)<0) goto failed;

	packet.wlength=*(WORD*)(data0+0x102);
	sendpacket(ep0,&packet);
	ring(slot,1);
	if(waitdevice(slot,1)<0) goto failed;

	explaindescriptor(data0+0x100);


	//[buffer+0x200]:string descriptors
	//say("string descriptor...",0);
	//packet.bmrequesttype=0x80;
	//packet.brequest=6;
	//packet.wvalue=0x300;
	//packet.windex=0;
	//packet.wlength=0xff;
	//packet.buffer=data0+0x200;
	//sendpacket(ep0,&packet);
	//ring(slot,1);
	//if(waitdevice(slot,1)<0) goto failed;

	//packet.windex=*(WORD*)(data0+0x202);
	//DWORD temp=1;
	//for(;temp<8;temp++)		//所有字符串描述符
	//{
	//if(stringtoread[temp]!=0)
	//{
	//	packet.wvalue=0x300+temp;
	//	packet.buffer=data0+0x200+temp*0x40;
	//	sendpacket(ep0,&packet);
	//	ring(slot,1);
	//	waitdevice(slot,1);	//不用管成功失败
	//}
	//}
	//--------------------------------------------




	//--------------------------------------
	//say("get configuration...",0);
	//packet.bmrequesttype=0x80;
	//packet.brequest=8;
	//packet.wvalue=0;
	//packet.windex=0;
	//packet.wlength=1;
	//packet.buffer=data0+0x500;
	//sendpacket(ep0,&packet);
	//ring(slot,1);
	//if(waitdevice(slot,1)<0) goto failed;

	//say("configure:",*(BYTE*)(data0+0x500));
	//------------------------------------




	recorddevice(
	vendor+(product<<16),			//+0
	classcode+(interfaceclass<<32),		//+8
	rootport+(routestring<<32),		//+10
	speed,					//+18
	slot);					//+20


	failed:
	return;
	//say("}",0);
}
void hub(QWORD rootport,QWORD routestring,QWORD speed,QWORD slot)
{
	if(deviceprotocol==0) say("no TT hub!",0);
	if(deviceprotocol==1) say("single TT hub!",0);
	if(deviceprotocol==2) say("multi TT hub!",0);

	QWORD slotcontext=usbhome+slot*0x8000;
	QWORD devinfo=slotcontext+0x1000;
	QWORD ep0=slotcontext+0x2000;
	QWORD data0=slotcontext+0x3000;
	QWORD ep11=slotcontext+0x6000;
	QWORD data11=slotcontext+0x7000;
	DWORD slotstate;
	DWORD epstate;
	struct setup packet;
	struct context context;


	say("4.set configuration...",0);
	packet.bmrequesttype=0;
	packet.brequest=9;
	packet.wvalue=1;
	packet.windex=0;
	packet.wlength=0;
	sendpacket(ep0,&packet);
	ring(slot,1);
	if(waitdevice(slot,1)<0) goto failed;


	//----------------得到hub描述符-----------------
	say("1.hub desc@",data0+0x400);
	packet.bmrequesttype=0xa0;
	packet.brequest=6;
	packet.wvalue=0x2900;
	packet.windex=0;
	packet.wlength=8;
	packet.buffer=data0+0x400;
	sendpacket(ep0,&packet);
	ring(slot,1);
	if(waitdevice(slot,1)<0) goto failed;
	explainhub(data0+0x400);

	QWORD count=*(BYTE*)(data0+0x402);
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

	command(slotcontext,0,0, (slot<<24)+(13<<10) );
	if(waithost(0x21)<0) goto failed;

	slotstate=(*(DWORD*)(slotcontext+0x80c))>>27; //if2,addressed
	if(slotstate==2) say("slot evaluated",0);
	else say("slot state:",slotstate);





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
	context.d2=ep11+1;
	context.d3=0;
	context.d4=0x80008;
	writecontext(slotcontext,4,&context);		//ep1.1

	command(slotcontext,0,0, (slot<<24)+(12<<10) );
	if(waithost(0x21)<0) goto failed;

	slotstate=(*(DWORD*)(slotcontext+0x80c))>>27;
	epstate=(*(DWORD*)(slotcontext+0x860))&0x3;
	if(slotstate==3) say("slot configured",0);
	else say("slot state:",slotstate);
	if(epstate==0){
		say("ep3 wrong",0);
		goto failed;
	}




	//----------prepare ep1.1 ring-------------
	say("4.ep1.1 ring:",0);
	DWORD* temp=(DWORD*)ep11;
	int i=0;
	for(i=0;i<0x3fc;i+=4)		//0xff0/4=0x3fc
	{
		temp[i]=data11+i*4;
		temp[i+1]=0;
		temp[i+2]=1;
		temp[i+3]=0x21+(1<<10);
	}
	temp[i]=ep11;
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
		say("child port:",childport);

		//say("resetting...",0);
		packet.bmrequesttype=0x23;	//host2dev|class|rt_other
		packet.brequest=3;		//set feathre
		packet.wvalue=0x4;		//f_port_reset
		packet.wlength=0;		//0
		packet.buffer=0;
		packet.windex=childport;	//(??<<8)|(childport+1)
		sendpacket(ep0,&packet);
		ring(slot,1);
		if(waitdevice(slot,1)<0) goto failed;

		//say("getting status...",0);
		packet.bmrequesttype=0xa3;	//devhost|class|rt_other
		packet.brequest=0;		//req_get_status
		packet.wvalue=0;		//0
		packet.wlength=4;		//0
		packet.windex=childport;	//childport+1
		packet.buffer=data0+0x600;	//某个地址
		sendpacket(ep0,&packet);
		ring(slot,1);
		if(waitdevice(slot,1)<0) goto failed;

		DWORD status=*(DWORD*)(data0+0x600);
		say("status:",status);

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
				hub(rootport,childport,speed,slot);
			}
			//------------------------------------
		}
	}




failed:
	return;
}
static QWORD portbase;
static QWORD portcount;
void roothub()
{
	//用xhci spec里面的root port reset等办法处理port,得到port信息
	QWORD childport;
	for(childport=1;childport<=portcount;childport++)
	{
		say("root port:",childport);
		DWORD portsc=*(DWORD*)(portbase+childport*0x10-0x10);
		say("portsc:",portsc);

		if( (portsc&0x1) == 0x1)
		{
			//------------第0步:reset(不知道为什么要两次)---------
			*(DWORD*)(portbase+childport*0x10-0x10)=portsc|0x10;
			waithost(0x22);
			portsc=*(DWORD*)(portbase+childport*0x10-0x10);
			say("status(reset1):",portsc);
			*(DWORD*)(portbase+childport*0x10-0x10)=portsc|0x10;
			waithost(0x22);
			portsc=*(DWORD*)(portbase+childport*0x10-0x10);
			say("status(reset2):",portsc);
			//say("linkstate:",(portsc>>5)&0xf);
			//---------------------------------------------




			//---------第一步:初始化，读取并记录设备信息--------
			hello(childport,0,(portsc>>10)&0xf);
			//---------------------------------------




			//---------第二步:是hub就交给hub()----------
			//---------不是hub就把这个初始化完的东西放着不管----
			if(classcode==9&&interfaceclass==9)
			{
			hub(childport,0,(portsc>>10)&0xf,slot);
			}
			//-------------------------------------
		}
	}
	return;
}



int preparevariety()
{
	volatile DWORD temp;
	QWORD xhciaddr;
        QWORD* memory;
	int i;

	xhciaddr=*(QWORD*)(xhcihome+8);		//xhci地址我放在这儿了
	if(xhciaddr==0) return -1;

	say("xhci@",xhciaddr);
	say("{",0);

	//读几个变量
	temp=(*(DWORD*)(xhciaddr+0x14));
	doorbell=xhciaddr+temp;
        say("doorbell@",doorbell);

	temp=(*(DWORD*)xhciaddr) & 0xffff;	//caplength
	portbase=xhciaddr+temp+0x400;
	say("portbase@",portbase);

	temp=*(DWORD*)(xhciaddr+4);		//hcsparams1
	portcount=temp>>24;
	say("portcount:",portcount);

	temp=*(DWORD*)(xhciaddr+0x10);		//capparams
	contextsize=0x20;
	if((temp&0x4) == 0x4) contextsize*=2;
        say("contextsize:",contextsize);

	say("}",0);
	return 1;	//success
}
void usb()
{
	if(preparevariety()<0) return;

	//从根开始
	roothub();

	//自带hid的初始化和解释到屏幕程序
	hiddevice();
}
