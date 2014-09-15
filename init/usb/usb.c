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

//设备分两种，一种是hub，一种是设备
//routestring:上层hub在哪
//port:本设备连在上层hub的哪个port
void hub(QWORD routestring,QWORD port,QWORD slot);
void device(QWORD routestring,QWORD port);

volatile static QWORD doorbell;
static QWORD runtime;
static QWORD contextsize;

static QWORD portbase;
static QWORD portcount;
static QWORD usb3start;
static QWORD usb3count;
static QWORD usb2start;
static QWORD usb2count;





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
	if(	(type==0)|((type>7)&(type<0x21))|(type>0x29)	) return;

	say("	@",addr);

	if(type==1)	//设备描述符
	{
	//这是第一个被读取的描述符，所以几个变量在这里清零最好
	//classcode=0;	//这个就不必了，马上就变掉
	interfaceclass=0;
	interval=0;

	say("    blength:",*(BYTE*)addr);
	say("    device:",*(BYTE*)(addr+1));
	say("    bcdusb:",*(WORD*)(addr+2));
	classcode=*(BYTE*)(addr+4);
	say("    bclass:",classcode);
	say("    bsubclass:",*(BYTE*)(addr+5));
	say("    bprotocol:",*(BYTE*)(addr+6));
	say("    bpacketsize:",*(BYTE*)(addr+7));
	say("    vendor:",*(WORD*)(addr+8));
	say("    product:",*(WORD*)(addr+0xa));
	say("    bcddevice:",*(WORD*)(addr+0xc));
	say("    imanufacturer:",*(BYTE*)(addr+0xe));
	say("    iproduct:",*(BYTE*)(addr+0xf));
	say("    iserial:",*(BYTE*)(addr+0x10));
	say("    bnumconf:",*(BYTE*)(addr+0x11));
	}

	if(type==2)	//配置描述符
	{
	say("    blength:",*(BYTE*)addr);
	say("    conf:",*(BYTE*)(addr+1));
	say("    wlength:",*(WORD*)(addr+2));
	say("    bnuminterface:",*(BYTE*)(addr+4));
	say("    bvalue:",*(BYTE*)(addr+5));
	say("    i:",*(BYTE*)(addr+6));
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
	say("    length:",*(BYTE*)addr);
	say("    interface:",*(BYTE*)(addr+1));
	say("    binterfacenum:",*(BYTE*)(addr+2));
	say("    balternatesetting:",*(BYTE*)(addr+3));
	say("    bnumendpoint:",*(BYTE*)(addr+4));
	interfaceclass=*(BYTE*)(addr+5);
	say("    bclass:",interfaceclass);
	say("    bsubclass:",*(BYTE*)(addr+6));
	say("    bprotol:",*(BYTE*)(addr+7));
	say("    i:",*(BYTE*)(addr+8));
	}

	if(type==5)	//端点描述符
	{
	say("    blength:",*(BYTE*)addr);
	say("    endpoint:",*(BYTE*)(addr+1));

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
	say("    hid:",*(BYTE*)(addr+1));
	say("    bcdhid:",*(WORD*)(addr+2));
	say("    bcountrycode:",*(BYTE*)(addr+4));
	say("    bnumdescriptor:",*(BYTE*)(addr+5));

	thistype=*(BYTE*)(addr+6);
	if(thistype==0x22) reportsize=*(WORD*)(addr+7);
	say("    btype:",thistype);
	say("    wlength:",*(WORD*)(addr+7));

	thistype=*(BYTE*)(addr+9);
	if(thistype==0x22) reportsize=*(WORD*)(addr+10);
	say("    btype:",*(BYTE*)(addr+9));
	say("    wlength:",*(WORD*)(addr+10));
	}
        if(type==0x29)
        {
        say("    blength:",*(BYTE*)addr);
        say("    hub:",*(BYTE*)(addr+1));
        say("    bnumberofport:",*(BYTE*)(addr+2));
        say("    whubcharacteristics:",*(BYTE*)(addr+3));
        say("    bpowerontopowergood:",*(BYTE*)(addr+5));
        say("    bhubcontrolcurrent:",*(BYTE*)(addr+6));
        say("    bremoveandpowermask:",*(BYTE*)(addr+7));
        }
}




void explainhid(QWORD addr)
{
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
	if(btype==0)	//main
	{
		if(btag==8) say("m.input:",p[i+1]);
		else if(btag==9) say("m.output:",p[i+1]);
		else if(btag==0xa) say("m.collection:",p[i+1]);
		else if(btag==0xb) say("m.feature",0);
		else if(btag==0xc) say("m.endcollection",0);
		else say("unknown tag:",btag);
	}
	if(btype==1)		//global
	{
		if(btag==0) say("g.usagepage:",p[i+1]);
		else if(btag==1) say("g.logicalmin:",p[i+1]);
		else if(btag==2) say("g.logicalmax:",p[i+1]);
		else if(btag==3) say("g.physicalmin:",p[i+1]);
		else if(btag==4) say("g.physicalmax:",p[i+1]);
		else if(btag==5) say("g.unitexponent:",p[i+1]);
		else if(btag==6) say("g.unit:",p[i+1]);
		else if(btag==7) say("g.reportsize:",p[i+1]);
		else if(btag==8) say("g.reportid:",p[i+1]);
		else if(btag==9) say("g.reportcount:",p[i+1]);
		else if(btag==0xa) say("g.push:",p[i+1]);
		else if(btag==0xb) say("g.pop:",p[i+1]);
		else say("unknown tag:",btag);
	}
	if(btype==2)		//local
	{
		if(btag==0) say("    l.usage:",p[i+1]);
		else if(btag==1) say("    l.usagemin:",p[i+1]);
		else if(btag==2) say("    l.usagemax:",p[i+1]);
		else if(btag==3) say("    l.desinatorindex:",p[i+1]);
		else if(btag==4) say("    l.desinatormin:",p[i+1]);
		else if(btag==5) say("    l.desinatormax:",p[i+1]);
		else if(btag==7) say("    l.stringindex:",p[i+1]);
		else if(btag==8) say("    l.stringmin:",p[i+1]);
		else if(btag==9) say("    l.stringmax:",p[i+1]);
		else if(btag==0xa) say("    l.delimiter:",p[i+1]);
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
		say("    usage min:",p[i+1]);
		break;
		}
		case 0x29:{
		say("    usage max:",p[i+1]);
		break;
		}
		case 0x15:{
		say("    logical max:",p[i+1]);
		break;
		}
		case 0x25:{
		say("    logical max:",p[i+1]);
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




//键盘鼠标，该干嘛干嘛
void hid(QWORD slot)
{
	say("it is hid device:",0);
	say("{",0);

	//只要传递slot号，就能得到我们手动分配的内存
	QWORD inputcontext=slothome+slot*0x8000;
	QWORD outputcontext=slothome+slot*0x8000+0x1000;
	QWORD controladdr=slothome+slot*0x8000+0x2000;
	QWORD intaddr=slothome+slot*0x8000+0x3000;
	QWORD buffer=slothome+slot*0x8000+0x4000;
	QWORD hidbuffer=slothome+slot*0x8000+0x5000;

	say("hid descriptor@",buffer+0x400);
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
	explainhid(buffer+0x400);
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
	context.addr=inputcontext;
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
	command(inputcontext,0,0, (slot<<24)+(12<<10)+commandcycle );
	if(waitevent(0x21)<0) goto failed;

	DWORD slotstate=(*(DWORD*)(outputcontext+0xc))>>27;
	DWORD epstate=(*(DWORD*)(outputcontext+0x60))&0x3;
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
	QWORD temp;
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


	//正常结束-------------------
	say("}",0);
	return;
	//--------------------------------




failed:
say("}",0);
}




//routestring:设备的上级hub位置，port:设备自己所在hub内部port号
void device(QWORD routestring,QWORD port)
{
	DWORD speed;		//第一步得到这个，给context用
	if(routestring==0)	//this device is under roothub port
	{
		DWORD portsc=*(DWORD*)(portbase+port*0x10-0x10);
		if( (portsc&0x1) != 0x1){
			say("nothing in port:",port);
			return;
		}

		say("port:",port);
		say("{",0);

		say("    portsc(before):",portsc);
	

		if( (port>=usb3start) && (port<usb3start+usb3count) )
		{
			say("    3.0",0);
		}
		if( (port>=usb2start) && (port<usb2start+usb2count) )
		{
			say("    2.0(must reset twice,why?)",0);

			*(DWORD*)(portbase+port*0x10-0x10)=portsc|0x10;
			waitevent(0x22);
			portsc=*(DWORD*)(portbase+port*0x10-0x10);
			say("    status(reset1):",portsc);

			*(DWORD*)(portbase+port*0x10-0x10)=portsc|0x10;
			waitevent(0x22);
			portsc=*(DWORD*)(portbase+port*0x10-0x10);
			say("    status(reset2):",portsc);
		}

		portsc=*(DWORD*)(portbase+port*0x10-0x10);
		say("    status(after):",portsc);
		DWORD pls=(portsc>>5)&0xf;
		speed=(portsc>>10)&0xf;
		say("    pls:",pls);
		say("    speed:",speed);
	}
	else		//this device is under normal hub
	{

	}
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
	QWORD slotdata=slothome+slot*0x8000;
	QWORD inputcontext=slotdata;
	QWORD outputcontext=slotdata+0x1000;
	QWORD controladdr=slotdata+0x2000;
	QWORD intaddr=slotdata+0x3000;
	QWORD buffer=slotdata+0x4000;
	QWORD hidbuffer=slotdata+0x5000;

	say("2.initing structure",0);
	say("    inputcontext@",inputcontext);
	say("    outputcontext@",outputcontext);
	say("    ep0@",controladdr);
	say("    ep1.1@",intaddr);

	//1)clear context
	clear(slothome+slot*0x8000,0x8000);
	//construct input
	context.addr=inputcontext;
	context.dci=0;			//input context
	context.d0=0;
	context.d1=3;
	context.d2=0;
	context.d3=0;
	context.d4=0;
	writecontext();

	context.dci=1;			//slot context
        context.d0=(3<<27)+(speed<<20);
	context.d1=port<<16;
	writecontext();

	context.dci=2;			//ep0 context
	context.d0=0;
	context.d1=(64<<16)+(4<<3)+6;
	context.d2=controladdr+1;
	context.d3=0;
	context.d4=0x40;
	writecontext();

	//output context地址填入对应dcbaa
	*(DWORD*)(dcbahome+slot*8)=outputcontext;
	*(DWORD*)(dcbahome+slot*8+4)=0;
	//------------------------------------




	//-----------address device-----------------
	say("3.initing slot",0);


	say("    address device",0);
	command(inputcontext,0,0, (slot<<24)+(11<<10)+commandcycle );
	if(waitevent(0x21)<0) goto failed;

	DWORD slotstate=(*(DWORD*)(outputcontext+0xc))>>27; //if2,addressed
	DWORD epstate=(*(DWORD*)(outputcontext+0x20))&0x3;
	if(slotstate==2) say("    slot addressed!",0);
	else say("    slot state:",slotstate);
	if(epstate==0){
		say("    ep0 wrong",0);
		goto failed;
	}

	//first read(to obtain maxsize)
	say("    getting packetsize",0);
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
        say("    evaluating:",0);
	context.addr=inputcontext;
	context.dci=2;			//ep0 context
	context.d0=0;
	context.d1=(packetsize<<16)+(4<<3)+6;
	context.d2=controladdr+1;
	context.d3=0;
	context.d4=0x40;
	writecontext();

	command(inputcontext,0,0, (slot<<24)+(13<<10)+commandcycle );
	if(waitevent(0x21)<0) goto failed;

	slotstate=(*(DWORD*)(outputcontext+0xc))>>27; //if2,addressed
	epstate=(*(DWORD*)(outputcontext+0x20))&0x3;
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
	say("device descriptors@",buffer);
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
	say("configure descriptor@",buffer+0x100);
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
	say("configure descriptor(all)@",buffer+0x100);
	packet.wlength=*(WORD*)(buffer+0x102);
	controltransfer();
	ring(slot,1);
	if(waitevent(0x20)<0) goto failed;
	explaindescriptor(buffer+0x100);


	//[buffer+0x200]:string descriptors
	say("string descriptors@",buffer+0x200);
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
	for(;temp<2;temp++)		//所有字符串描述符
	{
		packet.wvalue=0x300+temp;
		packet.data=buffer+0x200+temp*0x40;
		controltransfer();
		ring(slot,1);
		waitevent(0x20);	//不用管成功失败
	}
	//--------------------------------------------




	//上一步得到基础信息
	say("important infomation:",0);
	say("    classcode:",classcode);
	say("    interfaceclass:",interfaceclass);
	say("    interval:",interval);
	say("    reportsize:",reportsize);

	say("}",0);

	//已经知道设备是什么
	if(classcode==0&&interfaceclass==3)
	{
		hid(slot);		//基础信息处理过了，交给设备驱动
	}
	else if(classcode==9&&interfaceclass==9)
	{
		hub(routestring,port,slot);	//这是个hub，交给设备驱动
	}
	else
	{
		say("unknown device",0);
	}

	//到这里正常结束
	return;



	failed:
	say("}",0);
}




//有routestring和port就能得到设备物理位置，有slot就能得到hc眼中的虚拟位置
void hub(QWORD routestring,QWORD port,QWORD slot)
{
//如果这个是roothub,slot没有被分配好，为0
	if(slot==0)
	{
		say("root hub",0);

		for(port=1;port<=portcount;port++)
		{
			device(0,port);
		}
		return;
	}



//如果非roothub，那么它本身是个设备，基本初始化后本函数被使用，已经有了slot
//只要传递slot号，就能得到我们手动分配的内存
	say("normal hub",0);
	say("{",0);
	QWORD slotdata=slothome+slot*0x8000;
	QWORD inputcontext=slotdata;
	QWORD outputcontext=slotdata+0x1000;
	QWORD controladdr=slotdata+0x2000;
	QWORD buffer=slotdata+0x4000;
	QWORD hubbuffer=slotdata+0x5000;




	//----------------得到hub描述符-----------------
	say("1.hub desc@",buffer+0x400);
	packet.bmrequesttype=0xa0;
	packet.brequest=6;
	packet.wvalue=0x2900;
	packet.windex=0;
	packet.wlength=8;
	packet.addr=controladdr;
	packet.data=buffer+0x400;
	controltransfer();
	ring(slot,1);
	if(waitevent(0x20)<0) goto failed;
	explaindescriptor(buffer+0x400);

	QWORD count=*(BYTE*)(buffer+0x402);
	if(count==0) goto failed;

	//to do:support mtt??




	//-------------这是一个hub,修改部分context---------------
        say("2.change&evaluate:",0);

	//slot context
	DWORD* tempaddr=(DWORD*)(inputcontext+contextsize);
        tempaddr[0]|=(1<<26);		//is hub
	tempaddr[1]|=(count<<24);	//howmany hub this hub has
/*
	context.d2=(ttt<<16)+(ttportnum<<8)+(tthubslot);

	//ep0 context
	context.d0=0;
	context.d1=(packetsize<<16)+(4<<3)+6;
	context.d2=controladdr+1;
	context.d3=0;
	context.d4=0x40;
*/

	command(inputcontext,0,0, (slot<<24)+(13<<10)+commandcycle );
	if(waitevent(0x21)<0) goto failed;

	DWORD slotstate=(*(DWORD*)(outputcontext+0xc))>>27; //if2,addressed
	if(slotstate==2) say("    slot evaluated!",0);
	else say("    slot state:",slotstate);





	//------------------set port feathre-------------
	QWORD childport;
	say("3.childports",0);

	say("    resetting",0);
	packet.bmrequesttype=0x23;	//host2dev|class|rt_other
	packet.brequest=3;		//set feathre
	packet.wvalue=0x4;		//f_port_reset
	packet.wlength=0;		//0
	packet.addr=controladdr;
	packet.data=0;
	for(childport=1;childport<=count;childport++)
	{
		packet.windex=childport;	//(??<<8)|(childport+1)
		controltransfer();
		ring(slot,1);
		if(waitevent(0x20)<0) goto failed;
	}

	//wait 100ms

	//get current status
	say("    getting statu:",0);
	packet.bmrequesttype=0xa3;	//devhost|class|rt_other
	packet.brequest=0;		//req_get_status
	packet.wvalue=0;		//0
	packet.wlength=4;		//0
	packet.addr=controladdr;
	for(childport=1;childport<=count;childport++)
	{
		packet.windex=childport;	//childport+1
		packet.data=hubbuffer+childport*4;	//某个地址
		controltransfer();
		ring(slot,1);
		if(waitevent(0x20)<0) goto failed;
	}
	for(childport=1;childport<=count;childport++)
	{
		DWORD status=*(DWORD*)(hubbuffer+childport*4);
		if( (status&1) == 0 ) say("nothing in port:",childport);
		else
		{
			say("port:",0);
			say("    status:",status);

			QWORD speed=(status>>9)&1;
			if(speed==1) say("    low speed",0);
			if(speed==3) say("    wrong speed",0);
			if(speed==0) say("    full speed",0);
			if(speed==2) say("    high speed",0);

		}
	}




	say("4.device under one hub:",0);




	say("}",0);
	return;




failed:
say("}",0);
}




void initusb()
{
	//得到变量
        QWORD* memory=(QWORD*)slothome;

        doorbell=memory[0];
	runtime=memory[1];
        contextsize=memory[2];
        portbase=memory[4];
        portcount=memory[5];
        usb3start=memory[6];
        usb3count=memory[7];
        usb2start=memory[8];
        usb2count=memory[9];

	//
	int20();

	//从根开始
	hub(0,0,0);
}
explainevent(QWORD addr)
{
shout("event@",addr);
        //trb种类
        QWORD trbtype=*(DWORD*)(addr+0xc);
        trbtype=(trbtype>>10)&0x3f;
        shout("    trbtype:",trbtype);

        switch(trbtype)
        {
                case 0x20:{
                        shout("    transfer complete:",*(DWORD*)addr);
                        break;
                }
                case 0x21:{
                        shout("    command complete:",*(DWORD*)addr);
                        break;
                }
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
