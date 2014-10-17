#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long

#define xhcihome 0x300000

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




void fixinterval(QWORD* interval,QWORD speed)
{
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

	temp=*(DWORD*)(xhciaddr+0x10);		//capparams
	contextsize=0x20;
	if((temp&0x4) == 0x4) contextsize*=2;
        say("contextsize:",contextsize);

	say("}",0);
	return 1;	//success
}
