#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long
#define usbhome 0x500000
	//0:			//+0:		incontext,+800:outcontext
				//+1000:	device infomation
	//1:ep0			//+2000:	ep0 ring
				//+3000:	ep0 buffer
	//2:ep1.0		//+4000:
				//+5000:
	//3:ep1.1		//+6000:	ep1.1 ring
				//+7000:	ep1.1 buffer



struct setup{
        BYTE bmrequesttype;     //      0x80
        BYTE brequest;          //      0x6
        WORD wvalue;            //      (descriptor type>>8)+descriptor index
        WORD windex;            //      0
        WORD wlength;           //      length

        QWORD buffer;
};
struct context{         //传参太麻烦...
        DWORD d0;
        DWORD d1;
        DWORD d2;
        DWORD d3;
        DWORD d4;
        DWORD d5;
        DWORD d6;
        DWORD d7;
};




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
		say("long item!",0);
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




//本函数就为了得到这三个变量
static QWORD wmaxpacket;
static QWORD interval;
static QWORD reportsize;
void explainconfig(QWORD addr)
{
	say("conf@",addr);

	DWORD totallength=*(WORD*)(addr+2);
	DWORD offset=0;

	while(1)
	{
		if(offset>totallength) break;
		if( *(BYTE*)(addr+offset) <8) break;
		offset+=*(BYTE*)(addr+offset);
		//explaindescriptor(addr+offset);
		BYTE type=*(BYTE*)(addr+offset+1);
		switch(type)
		{
		case 4:
		{
			say("interface@",addr+offset);
			break;
		}
		case 5:
		{
			say("endpoint@",addr+offset);
			wmaxpacket=*(WORD*)(addr+offset+4);
			interval=*(BYTE*)(addr+offset+6);
			break;
		}
		case 0x21:
		{
			say("hid@",addr+offset);
			reportsize=*(WORD*)(addr+offset+7);
			break;
		}
		}
	}
}




void hid(QWORD speed,QWORD slot)
{
	say("hid!",0);
	//----------------变量--------------------
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
	//--------------------------------------




	//------必须得到wmaxpacket，interval，reportsize---------
        say("config desc@",data0+0x100);
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

        explainconfig(data0+0x100);
	fixinterval(&interval,speed);
	//---------------------------------------------




	//----------------------------------
	say("4.set configuration...",0);
	packet.bmrequesttype=0;
	packet.brequest=9;
	packet.wvalue=1;
	packet.windex=0;
	packet.wlength=0;
	sendpacket(ep0,&packet);
	ring(slot,1);
	if(waitdevice(slot,1)<0) goto failed;
	//--------------------------------------




	//-----------------------------------
	say("1.report desc@",data0+0x400);
	packet.bmrequesttype=0x81;
	packet.brequest=6;
	packet.wvalue=0x2200;
	packet.windex=0;
	packet.wlength=reportsize;
	packet.buffer=data0+0x400;
	sendpacket(ep0,&packet);
	ring(slot,1);
	if(waitdevice(slot,1)<0) goto failed;
	explainhid(data0+0x400);
	//-------------------------------------




	//------------------------------------
	say("2.configure ep1.1...",0);
        context.d0=0;
	context.d1=9;
	context.d2=0;
	context.d3=0;
	context.d4=0;
	writecontext(slotcontext,0,&context);

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
	//-----------------------------------




	//----------prepare ep1.1 ring-------------
	//[0,3f0)第一段正常trb
	say("3.ep1.1",0);
	DWORD* temp=(DWORD*)ep11;
	int i=0;
	for(i=0;i<0x3fc;i+=4)		//0xff0/4=0x3fc
	{
		temp[i]=data11+i*4;
		temp[i+1]=0;
		temp[i+2]=wmaxpacket;
		temp[i+3]=0x25+(1<<10);
	}
	temp[i]=ep11;
	temp[i+1]=0;
	temp[i+2]=0;
	temp[i+3]=1+(6<<10);

	//敲门铃，开始执行
	ring(slot,3);
	//------------------------------------------




	//-----注册(放到todo队列中?)函数专门解释数据
	//像下面这样?
	//todo(explainer);
	//不管todo怎么实现
	//反正一有空就被调用
	//无数据就自己yield
	//有数据就一直解释到没有数据?
	//------------------------------------------------------




failed:
	return;
}




//[0,3]:pidvid					+0
//[8,f]:deviceclass+(interfaceclass<<32)	+1
//[10,17]:rootport+(routestring<<32)		+2
//[18,1f]:speed					+3
//[20,27]:slot					+4
void hiddevice()
{
	QWORD* p=(QWORD*)0x150000;		//在/usb里面找
	int i;
	for(i=0;i<0x200;i+=8)
	{
		if(p[i+1]== 0x300000000)	//classcode=0,interfaceclass=3
		{
			hid(p[i+3],p[i+4]);
			break;
		}
	}
}
