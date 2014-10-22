#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long

#define eventringhome 0x340000
#define hostevent 0x380000
#define deviceevent 0x3c0000
	//[+0,+0x3fff]:slot
		//[+0,+0xfff]:
		//[+0x1000,+0x1fff]:ep0
		//[+2000,+0x2fff]:ep1.0
		//[+3000,+0x3fff]:ep1.1......

static QWORD runtime;
static QWORD portbase;




//这种trb最多，分类摆放
void classifystore(QWORD queueaddr,QWORD source)
{
        QWORD* enqueue=(QWORD*)(queueaddr+0xff0);
        QWORD* dequeue=(QWORD*)(queueaddr+0xff8);

	if( ( (0x10+enqueue[0])%0xff0 ) == dequeue[0] )	//满了就抛弃
	{
		return;
	}

	QWORD dest=queueaddr+enqueue[0];	//复制到enqueue指向的地方
	*(QWORD*)(dest)=*(QWORD*)(source);
	*(QWORD*)(dest+8)=*(QWORD*)(source+8);

	enqueue[0]=(0x10+enqueue[0])%0xff0;
}
void transfercomplete(QWORD addr)		//0x20
{
	DWORD* p=(DWORD*)addr;
	QWORD slot=p[3]>>24;
	QWORD endpoint=( p[3]>> 16 )&0x3f;
	shout("    slot:",slot);
	shout("    endpoint:",endpoint);

	QWORD queueaddr=deviceevent+slot*0x4000+endpoint*0x1000;
	classifystore(queueaddr,addr);
}




//这些trb很少很少
//把这些trb扔进一个接待队列里等着就行,格式如下：
//baseaddr=hostevent
//[+0,+0xffef]:每0x10一个trb
//[+0xfff0]:enqueue pointer
//[+0xfff8]:dequeue pointer
void store(QWORD source)
{
        QWORD* enqueue=(QWORD*)(hostevent+0xfff0);
        QWORD* dequeue=(QWORD*)(hostevent+0xfff8);

	if( ( (0x10+enqueue[0])%0xfff0 ) == dequeue[0] )	//满了就抛弃
	{
		return;
	}

	QWORD dest=hostevent+enqueue[0];	//复制到enqueue指向的地方
	*(QWORD*)(dest)=*(QWORD*)(source);
	*(QWORD*)(dest+8)=*(QWORD*)(source+8);

	enqueue[0]=(0x10+enqueue[0])%0xfff0;
}
void commandcomplete(QWORD addr)
{
		//shout("event@",addr);
		shout("command complete:",*(DWORD*)addr);

		store(addr);
}
void portchange(QWORD addr)
{
		shout("event@",addr);

		//哪个端口改变了
		QWORD portid=(*(DWORD*)addr) >> 24;
		QWORD portaddr=portbase+portid*0x10-0x10;
		shout("    port change:",portid);
		shout("    port addr:",portaddr);

		QWORD portsc=*(DWORD*)portaddr;
		shout("    portsc:",portsc);

		//告诉主控，收到变化,bit17写1(bit1不能写)
		*(DWORD*)portaddr=portsc&0xfffffffd;
		portsc=*(DWORD*)portaddr;
		shout("    portsc:",portsc);

		store(addr);
}
void unknown(addr)
{
	//shout("unknown:",trbtype);

	store(addr);
}


explainevent(QWORD addr)
{
        DWORD* p=(DWORD*)addr;
        DWORD trbtype=(p[3]>>10)&0x3f;

	switch(trbtype)
	{
		case 0x20:{
			transfercomplete(addr);
			break;
		}
		case 0x21:{
			commandcomplete(addr);
			break;
		}
		case 0x22:{
			portchange(addr);
			break;
		}
		default:{
			unknown(addr);
		}
	}
}
void realisr()
{
	QWORD erdp=( *(DWORD*)(runtime+0x38) )&0xfffffff0;
	int count=0;
	QWORD pcs=(*(DWORD*)(erdp+0xc))&0x1;
	while(1)
	{
		explainevent(erdp);

                erdp+=0x10;
                if(erdp>=eventringhome+0x1000)
                {
                        erdp=eventringhome;
			break;
                }

		count++;
		if(count>0x20) break;

                if( ((*(DWORD*)(erdp+0xc))&0x1) != pcs) break;
        }

        *(DWORD*)(runtime+0x38)=erdp|8;
        *(DWORD*)(runtime+0x3c)=0;
}


void setupisr(QWORD xhciaddr)
{
        runtime=xhciaddr+(*(DWORD*)(xhciaddr+0x18));
        portbase=xhciaddr+0x400+((*(DWORD*)xhciaddr) & 0xffff);


	installisr(0x1200);
}
