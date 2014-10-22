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


QWORD finddevice(QWORD vendor,QWORD product)
{
	//从/usb里面找到设备，得到两个值：speed和slot
        QWORD value=vendor+(product<<16);
        QWORD* p=(QWORD*)0x150000;
        int i;
        for(i=0;i<0x200;i+=8)
        {
                if(p[i]== value)
                {
			return (QWORD)&p[i];
                        break;
                }
        }
	return 0;
}


struct setup{
	BYTE bmrequesttype;	//	0x80
	BYTE brequest;		//	0x6
	WORD wvalue;		//	(descriptor type>>8)+descriptor index
	WORD windex;		//	0
	WORD wlength;		//	length

	QWORD buffer;
};
struct setup packet;
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
struct context context;




void explaindescriptor(QWORD addr)
{
	DWORD type=*(BYTE*)(addr+1);
	if(	(type==0)|((type>7)&(type<0x21))|(type>0x29)	) return;

	volatile DWORD i;
	switch(type)
	{
	case 1:	//设备描述符
	{
		say("device@",addr);
		say("blength:",*(BYTE*)addr);
		say("type:",*(BYTE*)(addr+1));
		say("bcdusb:",*(WORD*)(addr+2));
		say("bclass:",*(BYTE*)(addr+4));
		say("bsubclass:",*(BYTE*)(addr+5));
		say("bprotocol:",*(BYTE*)(addr+6));
		say("bpacketsize:",*(BYTE*)(addr+7));
		say("vendor:",*(WORD*)(addr+8));
		say("product:",*(WORD*)(addr+0xa));
		say("bcddevice:",*(WORD*)(addr+0xc));
		say("imanufacturer:",*(BYTE*)(addr+0xe));
		say("iproduct:",*(BYTE*)(addr+0xf));
		say("iserial:",*(BYTE*)(addr+0x10));
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
		say("i:",*(BYTE*)(addr+0x6));

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
		say("bclass:",*(BYTE*)(addr+5));
		say("bsubclass:",*(BYTE*)(addr+6));
		say("bprotol:",*(BYTE*)(addr+7));
		say("i:",*(BYTE*)(addr+0x8));

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

		say("wmaxpacket:",*(WORD*)(addr+4));
		say("binterval:",*(BYTE*)(addr+6));

		say("",0);
		break;
	}
	}
}




static QWORD wmaxpacket;
static QWORD interval;
void explainconfig(QWORD addr)
{
        DWORD totallength=*(WORD*)(addr+2);
        DWORD offset=0;

        while(1)
        {
                if(offset>totallength) break;
                if( *(BYTE*)(addr+offset) <8) break;
                offset+=*(BYTE*)(addr+offset);
                BYTE type=*(BYTE*)(addr+offset+1);
                switch(type)
                {
                case 5:
                {
                        wmaxpacket=*(WORD*)(addr+offset+4);
                        interval=*(BYTE*)(addr+offset+6);
                        break;
                }
                }
        }
}




int vendor_read(QWORD slot,WORD value)
{
	QWORD ep0=usbhome+slot*0x8000+0x2000;
	QWORD buffer=ep0+0x1000;

        packet.bmrequesttype=0xc0;
        packet.brequest=1;
        packet.wvalue=value;
        packet.windex=0;
        packet.wlength=1;
        packet.buffer=buffer;
        sendpacket(ep0,&packet);
        ring(slot,1);
        if(waitdevice(slot,1)<0) return -1;
	return 1;
}
int vendor_write(QWORD slot,WORD value,WORD index)
{
	QWORD ep0=usbhome+slot*0x8000+0x2000;

        packet.bmrequesttype=0x40;
        packet.brequest=1;
        packet.wvalue=value;
        packet.windex=index;
        packet.wlength=0;
        sendpacket(ep0,&packet);
        ring(slot,1);
        if(waitdevice(slot,1)<0) return -1;
	return 1;
}




void pl2303(QWORD speed,QWORD slot)
{
	//只要传递slot号，就能得到我们手动分配的内存
	QWORD slotcontext=usbhome+slot*0x8000;
	QWORD devinfo=slotcontext+0x1000;
	QWORD ep0=slotcontext+0x2000;
	QWORD data0=slotcontext+0x3000;
	QWORD ep11=slotcontext+0x6000;
	QWORD data11=slotcontext+0x7000;
	DWORD slotstate;
	DWORD epstate;

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
        explainconfig(data0+0x100);
        fixinterval(&interval,speed);



	say("4.set configuration...",0);
	packet.bmrequesttype=0;
	packet.brequest=9;
	packet.wvalue=1;
	packet.windex=0;
	packet.wlength=0;
	sendpacket(ep0,&packet);
	ring(slot,1);
	if(waitdevice(slot,1)<0) goto failed;


	/*
	//change input context&ep1.1 context
	say("2.configure ep1.1...",0);
        context.d0=0;
	context.d1=9;
	context.d2=0;
	context.d3=0;
	context.d4=0;
	writecontext(slotcontext,0,&context);

	context.d0=interval<<16;
	//context.d1=(64<<16)+(7<<3)+(3<<1);
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
	*/




        vendor_read(slot, 0x8484);		//slot,value
        vendor_write(slot, 0x0404, 0);		//slot,value,index
        vendor_read(slot, 0x8484);
        vendor_read(slot, 0x8383);
        vendor_read(slot, 0x8484);
        vendor_write(slot, 0x0404, 1);
        vendor_read(slot, 0x8484);
        vendor_read(slot, 0x8383);
        vendor_write(slot, 0, 1);
        vendor_write(slot, 1, 0);
	//if(legacy) vendor_write(slot, 2, 0x24);
	//else vendor_write(slot, 2, 0x44);




failed:
	return;
}




void main()
{
	say("pl2303",0);

	//lib/usb.c可以看成一个小型机器，需要得到xhci的doorbell等变量来正常运行
	if(preparevariety()<0) return;

        QWORD addr=finddevice(0x67b,0x2303);
        if(addr<=0) return;

        pl2303(*(QWORD*)(addr+0x18),*(QWORD*)(addr+0x20));

}
