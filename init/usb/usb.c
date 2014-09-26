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

//rootport:根port是哪一个
//routestring:物理位置
//比如:rootport=2,routestring=0x34
//表示设备物理位置为:根hub的port2,第一层hub的port4,第二层hub的port3
void device(QWORD rootport,QWORD routestring,DWORD speed);

static QWORD runtime;
static QWORD portbase;
static QWORD portcount;





void clear(QWORD addr,QWORD size)
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
	ring(0,0);
}




struct trb{
	DWORD d0;
	DWORD d1;
	DWORD d2;
	DWORD d3;
};
static struct trb trb;
void writetrb(QWORD ringaddr)
{
	//得到这次往哪儿写
	QWORD temp=*(QWORD*)(ringaddr+0xff0);
	DWORD* pointer=(DWORD*)(ringaddr+temp);
	//say("transfer ring@",&pointer[0]);

	//下次往哪儿写
	temp+=0x10;
	if(temp>0xf00)	temp=0;
	*(QWORD*)(ringaddr+0xff0)=temp;

	pointer[0]=trb.d0;
	pointer[1]=trb.d1;
	pointer[2]=trb.d2;
	pointer[3]=trb.d3;
}




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
if(packet.bmrequesttype>=0x80)
{
	//setup stage
	trb.d0=(packet.wvalue<<16)+(packet.brequest<<8)+packet.bmrequesttype;
	trb.d1=(packet.wlength<<16)+packet.windex;
	trb.d2=8;
	trb.d3=(3<<16)+(2<<10)+(1<<6)+1;
	writetrb(ringaddr);

	//data stage
	trb.d0=packet.buffer;
	trb.d1=0;
	trb.d2=packet.wlength;
	trb.d3=(1<<16)+(3<<10)+1;
	writetrb(ringaddr);

	//status stage
	trb.d0=trb.d1=trb.d2=0;
	trb.d3=(4<<10)+(1<<5)+1;
	writetrb(ringaddr);
}
else
{
	//setup stage
	trb.d0=(packet.wvalue<<16)+(packet.brequest<<8)+packet.bmrequesttype;
	trb.d1=(packet.wlength<<16)+packet.windex;
	trb.d2=8;
	trb.d3=(2<<10)+(1<<6)+1;
	writetrb(ringaddr);

	//status stage
	trb.d0=trb.d1=trb.d2=0;
	trb.d3=(1<<16)+(4<<10)+(1<<5)+1;	//in
	writetrb(ringaddr);
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
static struct context context;
static QWORD contextsize;
void writecontext(QWORD baseaddr,int dci)
{
	DWORD* pointer=(DWORD*)(baseaddr+dci*contextsize);

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
static QWORD deviceprotocol;
static QWORD vendor;
static QWORD product;
static QWORD interfaceclass;
static QWORD interval;
static QWORD reportsize;
static BYTE stringtoread[8];
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
	interfaceclass=0;
	interval=0;
	for(i=0;i<8;i++) stringtoread[i]=0;

say("device@",addr);
	say("blength:",*(BYTE*)addr);
	say("type:",*(BYTE*)(addr+1));
	say("bcdusb:",*(WORD*)(addr+2));

	classcode=*(BYTE*)(addr+4);			//!
	say("bclass:",classcode);

	say("bsubclass:",*(BYTE*)(addr+5));

	deviceprotocol=*(BYTE*)(addr+6);		//!
	say("bprotocol:",deviceprotocol);

	say("bpacketsize:",*(BYTE*)(addr+7));

	vendor=*(WORD*)(addr+8);			//!
	say("vendor:",vendor);

	product=*(WORD*)(addr+0xa);			//!
	say("product:",product);

	say("bcddevice:",*(WORD*)(addr+0xc));

	i=*(BYTE*)(addr+0xe);
	stringtoread[i]=1;
	say("imanufacturer:",i);

	i=*(BYTE*)(addr+0xf);
	stringtoread[i]=1;
	say("iproduct:",i);

	i=*(BYTE*)(addr+0x10);
	stringtoread[i]=1;
	say("iserial:",i);

	say("bnumconf:",*(BYTE*)(addr+0x11));
	say("",0);

break;
}
case 2:	//配置描述符
{
say("conf@",addr);
	say("blength:",*(BYTE*)addr);
	say("type:",*(BYTE*)(addr+1));
	say("wlength:",*(WORD*)(addr+2));
	say("bnuminterface:",*(BYTE*)(addr+4));
	say("bvalue:",*(BYTE*)(addr+5));
	i=*(BYTE*)(addr+0x6);
	stringtoread[i]=1;
	say("i:",i);
	BYTE bmattribute=*(BYTE*)(addr+7);
	say("bmattribute:",bmattribute);
	if( (bmattribute&0x40) ==0x40 ) say("    self powered",0);
	if( (bmattribute&0x20) ==0x20 ) say("    remote wake",0);
	say("bmaxpower:",*(BYTE*)(addr+8));
	say("",0);

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
	say("length:",*(BYTE*)addr);
	say("type:",*(BYTE*)(addr+1));
	say("binterfacenum:",*(BYTE*)(addr+2));
	say("balternatesetting:",*(BYTE*)(addr+3));
	say("bnumendpoint:",*(BYTE*)(addr+4));
	interfaceclass=*(BYTE*)(addr+5);
	say("bclass:",interfaceclass);
	say("bsubclass:",*(BYTE*)(addr+6));
	say("bprotol:",*(BYTE*)(addr+7));
	i=*(BYTE*)(addr+0x8);
	stringtoread[i]=1;
	say("i:",i);
	say("",0);

break;
}
case 5:	//端点描述符
{
say("endpoint@",addr);
	say("blength:",*(BYTE*)addr);
	say("type:",*(BYTE*)(addr+1));

	BYTE endpoint=*(BYTE*)(addr+2);
	say("    endpoint:",endpoint&0xf);
	if(endpoint>0x80){say("    in",0);}
	else{say("    out",0);}

	BYTE eptype=*(BYTE*)(addr+3);
	if(eptype==0) say("    control",0);
	else if(eptype==1) say("    isochronous",0);
	else if(eptype==2) say("    bulk",0);
	else say("    interrupt",0);

	say("wmaxpacketsize:",*(WORD*)(addr+4));
	interval=*(BYTE*)(addr+6);
	say("binterval:",interval);
	say("",0);

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
}
*/
case 0x21:	//hid设备描述符
{
say("hid@",addr);
	say("blength:",*(BYTE*)addr);
	say("type:",*(BYTE*)(addr+1));
	say("bcdhid:",*(WORD*)(addr+2));
	say("bcountrycode:",*(BYTE*)(addr+4));
	say("bnumdescriptor:",*(BYTE*)(addr+5));

	say("btype:",*(BYTE*)(addr+6));
	say("wlength:",*(WORD*)(addr+7));
	say("btype:",*(BYTE*)(addr+9));
	say("wlength:",*(WORD*)(addr+10));

	reportsize=*(WORD*)(addr+7);
	say("",0);

break;
}
case 0x29:
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

break;
}

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
	QWORD slotdata=slothome+slot*0x8000;
	say("0.data@",slotdata);
	QWORD inputcontext=slotdata;
	QWORD outputcontext=slotdata+0x1000;
	QWORD controladdr=slotdata+0x2000;
	QWORD intaddr=slotdata+0x3000;
	QWORD buffer=slotdata+0x4000;
	QWORD hidbuffer=slotdata+0x5000;
	DWORD slotstate;
	DWORD epstate;


	//change input context&ep1.1 context
	say("1.slot...",0);
        context.d0=0;
	context.d1=9;
	context.d2=0;
	context.d3=0;
	context.d4=0;
	writecontext(inputcontext,0);

	context.d0=interval<<16;
	context.d1=(8<<16)+(7<<3)+(3<<1);
	context.d2=intaddr+1;
	context.d3=0;
	context.d4=0x80008;
	writecontext(inputcontext,4);		//ep1.1

	command(inputcontext,0,0, (slot<<24)+(12<<10)+commandcycle );
	if(waitevent(0x21)<0) goto failed;

	slotstate=(*(DWORD*)(outputcontext+0xc))>>27;
	epstate=(*(DWORD*)(outputcontext+0x60))&0x3;
	if(slotstate==3) say("slot configured",0);
	else say("slot state:",slotstate);
	if(epstate==0){
		say("ep3 wrong",0);
		goto failed;
	}
	//-----------------------------------


	//--------------------------------------
	say("2.device...",0);
	say("set configuration...",0);
	packet.bmrequesttype=0;
	packet.brequest=9;
	packet.wvalue=1;
	packet.windex=0;
	packet.wlength=0;
	sendpacket(controladdr);
	ring(slot,1);
	if(waitevent(0x20)<0) goto failed;

	say("get configuration...",0);
	packet.bmrequesttype=0x80;
	packet.brequest=8;
	packet.wvalue=0;
	packet.windex=0;
	packet.wlength=1;
	packet.buffer=buffer+0x500;
	sendpacket(controladdr);
	ring(slot,1);
	if(waitevent(0x20)<0) goto failed;

	say("configure:",*(BYTE*)(buffer+0x500));
	//------------------------------------


/*
	//-------------------------------
	say("set idle...",0);
	packet.bmrequesttype=0x21;
	packet.brequest=0xa;
	packet.wvalue=0;
	packet.windex=0;
	packet.wlength=0;
	sendpacket(controladdr);
	ring(slot,1);
	if(waitevent(0x20)<0) goto failed;

	say("get idle...",0);
	packet.bmrequesttype=0xa1;
	packet.brequest=2;
	packet.wvalue=0;
	packet.windex=0;
	packet.wlength=1;
	packet.buffer=buffer+0x600;
	sendpacket(controladdr);
	ring(slot,1);
	if(waitevent(0x20)<0) goto failed;

	say("idle:",*(BYTE*)(buffer+0x600));
	//--------------------------------------
*/



/*
	//-------------------------------------
	say("set interface...",0);
	packet.bmrequesttype=0x1;
	packet.brequest=0xb;
	packet.wvalue=0;		//alternate setting
	packet.windex=0;		//interface
	packet.wlength=0;
	sendpacket(controladdr);
	ring(slot,1);
	if(waitevent(0x20)<0) goto failed;


	say("get interface...",0);
	packet.bmrequesttype=0x81;
	packet.brequest=0xa;
	packet.wvalue=0;
	packet.windex=0;
	packet.wlength=1;
	packet.buffer=buffer+0x600;
	sendpacket(controladdr);
	ring(slot,1);
	if(waitevent(0x20)<0) goto failed;

	say("interface:",*(BYTE*)(buffer+0x600));
	//-------------------------------------
*/



	say("3.report desc@",buffer+0x400);
	packet.bmrequesttype=0x81;
	packet.brequest=6;
	packet.wvalue=0x2200;
	packet.windex=0;
	packet.wlength=reportsize;
	packet.buffer=buffer+0x400;
	sendpacket(controladdr);
	ring(slot,1);
	if(waitevent(0x20)<0) goto failed;
	explainhid(buffer+0x400);






	//----------prepare ep1.1 ring-------------
	//[0,3f0)第一段正常trb
	QWORD temp;
	say("4.ep1.1",0);
	trb.d1=0;
	trb.d2=4;
	trb.d3=0x21+(1<<10);
	for(temp=0;temp<0x3f;temp++)
	{
		trb.d0=hidbuffer+temp*0x10;
		writetrb(intaddr);
	}
	//[3f0,400)第一段结尾link trb指向[0,3f0)
	trb.d0=intaddr;
	trb.d1=0;
	trb.d2=6;
	trb.d3=1+(6<<10);
	writetrb(intaddr);

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




//有routestring和port就能得到设备物理位置，有slot就能得到hc眼中的虚拟位置
void hub(QWORD rootport,QWORD routestring,QWORD slot)
{
QWORD childport;




//如果这个是roothub,slot没有被分配好，为0
//用xhci spec里面的root port reset等办法处理port,得到port信息
if(slot==0)
{
for(childport=1;childport<=portcount;childport++)
{
say("root port:",childport);
say("{",0);
	DWORD portsc=*(DWORD*)(portbase+childport*0x10-0x10);
	say("portsc:",portsc);
	if( (portsc&0x1) == 0x1)
	{
		QWORD* memory=(QWORD*)(slothome);
		say("usb version:",memory[childport]);

		*(DWORD*)(portbase+childport*0x10-0x10)=portsc|0x10;
		waitevent(0x22);
		portsc=*(DWORD*)(portbase+childport*0x10-0x10);
		say("status(reset1):",portsc);

		*(DWORD*)(portbase+childport*0x10-0x10)=portsc|0x10;
		waitevent(0x22);
		portsc=*(DWORD*)(portbase+childport*0x10-0x10);
		say("status(reset2):",portsc);

		say("linkstate:",(portsc>>5)&0xf);
	}
say("}",0);

	if( (portsc&0x1) == 0x1)
	{
		device(childport,0,(portsc>>10)&0xf);
	}
}
return;
}



//如果非roothub，那么它本身是个设备，基本初始化后已经被分配了slot
//只要传递slot号，就能得到我们手动分配的内存

	if(deviceprotocol==0) say("hub(no TT)",0);
	if(deviceprotocol==1) say("hub(single TT)",0);
	if(deviceprotocol==2) say("hub(multi TT)",0);
	say("{",0);
	say("rootport:",rootport);
	say("routestring:",routestring);

	QWORD slotdata=slothome+slot*0x8000;
	say("0.data@",slotdata);
	QWORD inputcontext=slotdata;
	QWORD outputcontext=slotdata+0x1000;
	QWORD controladdr=slotdata+0x2000;
	QWORD intaddr=slotdata+0x3000;
	QWORD buffer=slotdata+0x4000;
	QWORD hubbuffer=slotdata+0x5000;
	DWORD slotstate;
	DWORD epstate;




	//----------------得到hub描述符-----------------
	say("1.hub descriptor...",0);
	packet.bmrequesttype=0xa0;
	packet.brequest=6;
	packet.wvalue=0x2900;
	packet.windex=0;
	packet.wlength=8;
	packet.buffer=buffer+0x400;
	sendpacket(controladdr);
	ring(slot,1);
	if(waitevent(0x20)<0) goto failed;
	explaindescriptor(buffer+0x400);

	QWORD count=*(BYTE*)(buffer+0x402);
	if(count==0) goto failed;

	//to do:support mtt??




	//-------------这是一个hub,修改部分context---------------
	say("2.slot...",0);
	say("evaluate...",0);

	//slot context
	DWORD* tempaddr=(DWORD*)(inputcontext+contextsize);
	tempaddr[0]|=(1<<26);		//is hub
	tempaddr[1]|=(count<<24);	//howmany hub this hub has
	tempaddr[2]|=0;		//(ttt<<16)+(ttportnum<<8)+(tthubslot);

	command(inputcontext,0,0, (slot<<24)+(13<<10)+commandcycle );
	if(waitevent(0x21)<0) goto failed;

	slotstate=(*(DWORD*)(outputcontext+0xc))>>27; //if2,addressed
	if(slotstate==2) say("slot evaluated",0);
	else say("slot state:",slotstate);





	//change input context&ep1.1 context
	say("configure...",0);
        context.d0=0;
	context.d1=9;
	context.d2=0;
	context.d3=0;
	context.d4=0;
	writecontext(inputcontext,0);

	context.d0=interval<<16;
	context.d1=(8<<16)+(7<<3)+(3<<1);
	context.d2=intaddr+1;
	context.d3=0;
	context.d4=0x80008;
	writecontext(inputcontext,4);		//ep1.1

	command(inputcontext,0,0, (slot<<24)+(12<<10)+commandcycle );
	if(waitevent(0x21)<0) goto failed;

	slotstate=(*(DWORD*)(outputcontext+0xc))>>27;
	epstate=(*(DWORD*)(outputcontext+0x60))&0x3;
	if(slotstate==3) say("slot configured",0);
	else say("slot state:",slotstate);
	if(epstate==0){
		say("ep3 wrong",0);
		goto failed;
	}




        //set configuration:0x0000,0000,0001,09,00
	say("device...",0);
	say("set configure...",0);
        packet.bmrequesttype=0;
        packet.brequest=9;
        packet.wvalue=1;
        packet.windex=0;
        packet.wlength=0;
        sendpacket(controladdr);
        ring(slot,1);
        if(waitevent(0x20)<0) goto failed;

	say("get configure...",0);
	packet.bmrequesttype=0x80;
	packet.brequest=8;
	packet.wvalue=0;
	packet.windex=0;
	packet.wlength=1;
	packet.buffer=buffer+0x500;
	sendpacket(controladdr);
	ring(slot,1);
	if(waitevent(0x20)<0) goto failed;

	say("device configured:",*(BYTE*)(buffer+0x500));
        //------------------------------------




	//----------prepare ep1.1 ring-------------
	//[0,3f0)第一段正常trb
	say("4.ep1.1 ring:",0);
	QWORD temp;
	trb.d1=0;
	trb.d2=1;
	trb.d3=0x21+(1<<10);
	for(temp=0;temp<0x3f;temp++)
	{
		trb.d0=hubbuffer+temp*0x10;
		writetrb(intaddr);
	}
	//[3f0,400)第一段结尾link trb指向[0,3f0)
	trb.d0=intaddr;
	trb.d1=0;
	trb.d2=6;
	trb.d3=1+(6<<10);
	writetrb(intaddr);

	//敲门铃，开始执行
	ring(slot,3);
	//------------------------------------------




	//------------------set port feathre-------------
	for(childport=1;childport<=count;childport++)
	{
	say("port:",childport);
	say("{",0);
		say("resetting...",0);
		packet.bmrequesttype=0x23;	//host2dev|class|rt_other
		packet.brequest=3;		//set feathre
		packet.wvalue=0x4;		//f_port_reset
		packet.wlength=0;		//0
		packet.buffer=0;
		packet.windex=childport;	//(??<<8)|(childport+1)
		sendpacket(controladdr);
		ring(slot,1);
		if(waitevent(0x20)<0) goto failed;

		say("getting status...",0);
		packet.bmrequesttype=0xa3;	//devhost|class|rt_other
		packet.brequest=0;		//req_get_status
		packet.wvalue=0;		//0
		packet.wlength=4;		//0
		packet.windex=childport;	//childport+1
		packet.buffer=buffer+0x600;	//某个地址
		sendpacket(controladdr);
		ring(slot,1);
		if(waitevent(0x20)<0) goto failed;

		DWORD status=*(DWORD*)(buffer+0x600);
		say("status:",status);

	say("}",0);

		if( (status&1) == 1 )
		{
			device(rootport,childport,(status>>9)&3);
		}
	}




	say("}",0);
	return;




failed:
say("}",0);
}




//rootport:设备自己所在roothub port号,routestring:一层层地址
void device(QWORD rootport,QWORD routestring,DWORD speed)
{
	say("device",0);
	say("{",0);

	say("0.information:",0);
	say("rootport:",rootport);
	say("routestring:",routestring);
	if(speed==0) say("full speed:",speed);
	if(speed==1) say("low speed:",speed);
	if(speed==2) say("high speed:",speed);
	if(speed>=3) say("unknown speed:",speed);


	//---------------obtain slot------------------
	say("1.requesting slot...",0);

	command(0,0,0, (9<<10)+commandcycle );
	if(waitevent(0x21)<0) goto failed;

	QWORD slot=(*(DWORD*)(eventaddr+0xc)) >> 24;
	if(slot>=0x18){
		say("bad slotnum",slot);
		goto failed;
	}
	say("obtained slot:",slot);
	//-------------------------------------------




	//-------------slot initialization----------------
	QWORD slotdata=slothome+slot*0x8000;
	QWORD inputcontext=slotdata;
	QWORD outputcontext=slotdata+0x1000;
	QWORD controladdr=slotdata+0x2000;
	QWORD intaddr=slotdata+0x3000;
	QWORD buffer=slotdata+0x4000;
	QWORD hidbuffer=slotdata+0x5000;
	DWORD slotstate;
	DWORD epstate;

	say("2.initing structure...",0);
	say("    inputcontext@",inputcontext);
	say("    outputcontext@",outputcontext);
	say("    ep0@",controladdr);
	say("    ep1.1@",intaddr);

	//1)clear context
	clear(slotdata,0x8000);
	//construct input
	context.d0=0;
	context.d1=3;
	context.d2=0;
	context.d3=0;
	context.d4=0;
	writecontext(inputcontext,0);

        context.d0=(3<<27)+(speed<<20)+routestring;
	context.d1=rootport<<16;
	writecontext(inputcontext,1);	//slot context

	context.d0=0;
	context.d1=(8<<16)+(4<<3)+6;
	context.d2=controladdr+1;
	context.d3=0;
	context.d4=0x8;
	writecontext(inputcontext,2);	//ep0

	//output context地址填入对应dcbaa
	*(DWORD*)(dcbahome+slot*8)=outputcontext;
	*(DWORD*)(dcbahome+slot*8+4)=0;
	//------------------------------------




	//-----------address device-----------------
	say("3.initing slot...",0);


	say("address command...",0);
	command(inputcontext,0,0, (slot<<24)+(11<<10)+commandcycle );
	if(waitevent(0x21)<0) goto failed;

	slotstate=(*(DWORD*)(outputcontext+0xc))>>27; //if2,addressed
	epstate=(*(DWORD*)(outputcontext+0x20))&0x3;
	if(slotstate==2) say("slot addressed",0);
	else say("slot state:",slotstate);
	if(epstate==0){
		say("ep0 wrong",0);
		goto failed;
	}

	//first read(to obtain maxsize)
	say("getting packetsize....",0);
	packet.bmrequesttype=0x80;
	packet.brequest=6;
	packet.wvalue=0x100;
	packet.windex=0;
	packet.wlength=8;
	packet.buffer=buffer;
	sendpacket(controladdr);
	ring(slot,1);
	if(waitevent(0x20)<0) goto failed;

	DWORD packetsize=*(BYTE*)(buffer+7);
	say("got packetsize:",packetsize);

	//evaluate
        say("evaluating...",0);
	context.d0=0;
	context.d1=(packetsize<<16)+(4<<3)+6;
	context.d2=controladdr+1;
	context.d3=0;
	context.d4=0x40;
	writecontext(inputcontext,2);	//ep0

	command(inputcontext,0,0, (slot<<24)+(13<<10)+commandcycle );
	if(waitevent(0x21)<0) goto failed;

	slotstate=(*(DWORD*)(outputcontext+0xc))>>27; //if2,addressed
	epstate=(*(DWORD*)(outputcontext+0x20))&0x3;
	if(slotstate==2) say("slot evaluated",0);
	else say("slot state:",slotstate);
	if(epstate==0){
		say("ep0 wrong",0);
		goto failed;
	}
	//--------------------------------------------




	//------------------get descriptor-----------------
	say("4.descriptors:",0);

	//[buffer]:device descriptor
	say("device descriptor...",0);
	packet.bmrequesttype=0x80;
	packet.brequest=6;
	packet.wvalue=0x100;
	packet.windex=0;
	packet.wlength=0x12;
	packet.buffer=buffer;
	sendpacket(controladdr);
	ring(slot,1);
	if(waitevent(0x20)<0) goto failed;
	explaindescriptor(buffer);


	//[buffer+0x100]:configure descriptor
	say("configure descriptor...",0);
	packet.bmrequesttype=0x80;
	packet.brequest=6;
	packet.wvalue=0x200;
	packet.windex=0;
	packet.wlength=0x9;
	packet.buffer=buffer+0x100;
	sendpacket(controladdr);
	ring(slot,1);
	if(waitevent(0x20)<0) goto failed;

	packet.wlength=*(WORD*)(buffer+0x102);
	sendpacket(controladdr);
	ring(slot,1);
	if(waitevent(0x20)<0) goto failed;
	explaindescriptor(buffer+0x100);


	//[buffer+0x200]:string descriptors
	say("string descriptor...",0);
	packet.bmrequesttype=0x80;
	packet.brequest=6;
	packet.wvalue=0x300;
	packet.windex=0;
	packet.wlength=0xff;
	packet.buffer=buffer+0x200;
	sendpacket(controladdr);
	ring(slot,1);
	if(waitevent(0x20)<0) goto failed;

	packet.windex=*(WORD*)(buffer+0x202);
	DWORD temp=1;
	for(;temp<8;temp++)		//所有字符串描述符
	{
	if(stringtoread[temp]!=0)
	{
		packet.wvalue=0x300+temp;
		packet.buffer=buffer+0x200+temp*0x40;
		sendpacket(controladdr);
		ring(slot,1);
		waitevent(0x20);	//不用管成功失败
	}
	}
	//--------------------------------------------




	//上一步得到基础信息
	say("important infomation:",0);
	say("classcode:",classcode);
	say("interfaceclass:",interfaceclass);
	say("vendor:",vendor);
	say("product:",product);

	say("interval:",interval);
	if( (speed==0)|(speed==1) )
	{
		if(interval == 1) interval=3;
		else if(interval == 2) interval=4;
		else if( (interval>=3) & (interval<=4) ) interval=5;
		else if( (interval>=5) & (interval<=8) ) interval=6;
		else if( (interval>=9) & (interval<=16) ) interval=7;
		else if( (interval>=17) & (interval<=32) ) interval=8;
		else if( (interval>=33) & (interval<=64) ) interval=9;
		else if( (interval>=65) & (interval<=128) ) interval=0xa;
		else if( (interval>=129) & (interval<256) ) interval=0xb;
	}
	say("fixed interval:",interval);		//[3,11]


	say("}",0);




	//已经知道设备是什么
	if(classcode==0&&interfaceclass==3)
	{
		hid(slot);		//基础信息处理过了，交给设备驱动
	}
	else if(classcode==9&&interfaceclass==9)
	{
		hub(rootport,0,slot);	//交给设备驱动
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




int preparevariety()
{
	volatile DWORD temp;
	QWORD xhciaddr;
        QWORD* memory;
	int i;

	//找xhci在哪里
	memory=(QWORD*)0x4000;
	for(i=0;i<0x200;i++)
	{
		if(memory[i]==0x69636878){
			xhciaddr=memory[i+1];
			break;
		}
	}
	if(i>=0x1ff) return -1;

	say("xhci@",xhciaddr);
	say("{",0);

	//读几个变量
	temp=(*(DWORD*)(xhciaddr+0x14));
	doorbell=xhciaddr+temp;
        say("doorbell@",doorbell);

	temp=(*(DWORD*)(xhciaddr+0x18));
        runtime=xhciaddr+temp;
	say("runtime@",runtime);

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




void initusb()
{
	//
	if(preparevariety()<0) return;

	//干掉原来的isr，用下面的新的
	int20();

	//从根开始
	hub(0,0,0);
}




explainevent(QWORD addr)
{
	DWORD* p=(DWORD*)addr;

        DWORD trbtype=(p[3]>>10)&0x3f;
	DWORD completecode=p[2]>>24;
        switch(trbtype)
        {
                case 0x20:{
			shout("event@",addr);
			QWORD transfertrb=p[0];
                        shout("    transfer complete:",transfertrb);
			QWORD slot=p[3]>>24;
			shout("    slot:",slot);
			QWORD endpoint=( p[3]>> 16 )&0x3f;
			shout("    endpoint:",endpoint);
			if(completecode!=1)
			{
				shout("    error:",completecode);
				break;
			}
			if(endpoint!=1)
			{
				QWORD buffer=*(QWORD*)transfertrb;
				shout("    buffer:",buffer);
				QWORD report=*(QWORD*)buffer;
				shout("    report:",report);
			}
                        break;
                }
                case 0x21:{
			shout("event@",addr);
                        shout("    command complete:",p[0]);
                        break;
                }
                case 0x22:{             //设备插入拔出
			shout("event@",addr);

                        //哪个端口改变了
                        QWORD portid=(*(DWORD*)addr) >> 24;
                        shout("    port change:",portid);

                        QWORD portaddr=portbase+portid*0x10-0x10;
                        shout("    port addr:",portaddr);

                        //到改变的地方看看
                        QWORD portsc=*(DWORD*)portaddr;
                        shout("    portsc:",portsc);

                        //告诉主控，收到变化,bit17写1(bit1不能写)
                        *(DWORD*)portaddr=portsc&0xfffffffd;

                        break;
                }
		default:
		{
        		shout("unknown type:",trbtype);
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
	QWORD count=0;
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

		count++;
		if(count>0x20) break;

                if( ((*(DWORD*)(temp+0xc))&0x1) != pcs) break;
        }

        //下一次主控把event写到哪儿
        *(DWORD*)(runtime+0x38)=temp|8;
        *(DWORD*)(runtime+0x3c)=0;

theend:
shout("}",0);
return;
}
