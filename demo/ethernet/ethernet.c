#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long

#define ethhome 0x500000
#define rxdesc ethhome+0x40000
#define txdesc ethhome+0x60000
#define rxbuffer ethhome+0x80000
#define txbuffer ethhome+0xc0000

static QWORD portaddr;
static QWORD mmio;
static QWORD macaddr;




//全部设备信息我放在0x110000了，格式如下:(非本环境自己处理这一步)
//[0,7]:(vendorid<<16)+deviceid
//[8,0xf]:(class<<24)+(subclass<<16)+(progif<<8)+revisionid
//[0x10,0x17]:portaddress of the device
//[0x18,0x1f]:ansciiname of the device
//本函数作用是：
//1.返回要驱动的设备的portaddress
//2.填上[0x18,0x1f],(为了工整好看)
void findport()
{
	QWORD* addr=(QWORD*)0x110000;
	int i;
	unsigned int temp;
	for(i=0;i<0x80;i++)		//每个0x40
	{
		temp=addr[8*i+1];
		temp&=0xffffff00;
		if(temp==0x02000000)
		{
			addr[8*i+3]=0x74656e7265687465;
			portaddr=addr[8*i+2];
			return;
		}
	}
}




static inline void out32( unsigned short port, unsigned int val )
{
    asm volatile( "outl %0, %1"
                  : : "a"(val), "Nd"(port) );
}
static inline unsigned int in32( unsigned short port )
{
    unsigned int ret;
    asm volatile( "inl %1, %0"
                  : "=a"(ret) : "Nd"(port) );
    return ret;
}
void explaincapability()
{
say("pci cap:",0);
say("{",0);

        DWORD offset;
        DWORD temp;
        DWORD type;
        DWORD next;

        out32(0xcf8,portaddr+0x34);
        offset=in32(0xcfc)&0xff;

        while(1)
        {
        out32(0xcf8,portaddr+offset);
        temp=in32(0xcfc);
        type=temp&0xff;
        next=(temp>>8)&0xff;

        switch(type)
        {
                case 0x5:
                {
                        out32(0xcf8,portaddr+offset);
                        temp=in32(0xcfc);
                        if( (temp&0x800000) ==0 )
                        {
                        say("32bit msi@",offset);

                        //before
                        out32(0xcf8,portaddr+offset);
                        say("    control:",in32(0xcfc));
                        out32(0xcf8,portaddr+offset+4);
                        say("    addr:",in32(0xcfc));
                        out32(0xcf8,portaddr+offset+8);
                        say("    data:",in32(0xcfc));

                        out32(0xcf8,portaddr+offset+8);
                        out32(0xcfc,0x21);
                        out32(0xcf8,portaddr+offset+4);
                        out32(0xcfc,0xfee00000);
                        out32(0xcf8,portaddr+offset);
                        out32(0xcfc,temp|0x10000);

                        out32(0xcf8,portaddr+offset);
                        say("    control:",in32(0xcfc));
                        out32(0xcf8,portaddr+offset+4);
                        say("    addr:",in32(0xcfc));
                        out32(0xcf8,portaddr+offset+8);
                        say("    data:",in32(0xcfc));
                        }
                        else
                        {
                        say("64bit msi@",offset);

                        out32(0xcf8,portaddr+offset);
                        say("    control:",in32(0xcfc));
                        out32(0xcf8,portaddr+offset+4);
                        say("    addr:",in32(0xcfc));
                        out32(0xcf8,portaddr+offset+0xc);
                        say("    data:",in32(0xcfc));

                        out32(0xcf8,portaddr+offset+0xc);
                        out32(0xcfc,0x21);
                        out32(0xcf8,portaddr+offset+4);
                        out32(0xcfc,0xfee00000);
                        out32(0xcf8,portaddr+offset);
                        out32(0xcfc,temp|0x10000);

                        out32(0xcf8,portaddr+offset);
                        say("    control:",in32(0xcfc));
                        out32(0xcf8,portaddr+offset+4);
                        say("    addr:",in32(0xcfc));
                        out32(0xcf8,portaddr+offset+0xc);
                        say("    data:",in32(0xcfc));
                        }

	                break;
		}
        }
        if(type == 0){
		say("unknown",0);
		break;    //当前capability类型为0，结束
	}
        if(next < 0x40){
		say("no next",0);		//下一capability位置错误，结束
		break;
	}

        offset=next;

	}

say("}",0);
}
void probepci()
{
	say("ethernet(port)@",portaddr);

	out32(0xcf8,portaddr+0x4);
	unsigned int temp=in32(0xcfc)|(1<<10)|(1<<2);
	out32(0xcf8,portaddr+0x4);
	out32(0xcfc,temp);

	out32(0xcf8,portaddr+0x4);
	say("pci status and command:",(QWORD)in32(0xcfc));

	out32(0xcf8,portaddr+0x10);
	mmio=in32(0xcfc)&0xfffffff0;

	explaincapability();
}




WORD eepromread(whichreg)
{
	DWORD* p=(DWORD*)(mmio+0x14);
	*p=(whichreg<<8)|0x1;		//start
	while(1)
	{
		if( (p[0]&0x10) == 0x10 ) return (WORD)(p[0]>>8);
	}
}
void ethernet()
{
	say("ethernet@",mmio);

	//---------------------macaddr-------------------
	DWORD ral=*(DWORD*)(mmio+0x5400);	//receive address low
	if(ral)
	{
		DWORD rah=*(DWORD*)(mmio+0x5404);
		macaddr=((QWORD)(rah&0xffff)<<32)+ral;
	}
	else
	{
		say("read eeprom",0);
		WORD mac01=eepromread(0);
		WORD mac23=eepromread(1);
		WORD mac45=eepromread(2);
		macaddr=((QWORD)(mac45)<<32)+((QWORD)(mac23)<<16)+mac01;
	}
	say("macaddr:",macaddr);
	//------------------------------------------




	//-----------------control-----------------
	//set link up
	*(DWORD*)(mmio+0)=(*(DWORD*)(mmio+0))|(1<<6);

	//clear multicast table array
	QWORD i;
	for(i=0;i<128;i++) *(DWORD*)(mmio+0x5200+4*i)=0;

	//read to clear "interrupt cause read"
	say("icr:",*(DWORD*)(mmio+0xc0));
	//---------------------------------------




	//--------------------receive descriptor---------------------
	for(i=0;i<32;i++)
	{
		*(QWORD*)(rxdesc+0x10*i)=rxbuffer+0x800*i;	//每个2048B
	}
	*(DWORD*)(mmio+0x2800)=rxdesc;	//addrlow
	*(DWORD*)(mmio+0x2804)=0;	//addrhigh
	*(DWORD*)(mmio+0x2808)=32*16;	//32个*每个16B
	*(DWORD*)(mmio+0x2810)=0;	//head
	*(DWORD*)(mmio+0x2818)=31;	//tail
	*(DWORD*)(mmio+0x100)=0x400801e;//control
	//   4    0    0    8    0    1    e
	//0100 0000 0000 1000 0000 0001 1110
        //RCTL_EN		1<<1
        //RCTL_SBP		1<<2
        //RCTL_UPE		1<<3
        //RCTL_MPE		1<<4
        //RCTL_LBM_NONE		0<<6
       	//RCTL_RDMTS_HALF	0<<8
        //RCTL_BAM		1<<15
        //RCTL_SECRC		1<<26
        //RCTL_BSIZE_2048	0<<16
	//------------------------------------------




	//----------------transmit descriptor-----------------
	for(i=0;i<8;i++)
	{
		*(QWORD*)(txdesc+0x10*i)=txbuffer+0x800*i;	//每个2048B
	}
	*(DWORD*)(mmio+0x3800)=txdesc;	//addrlow
	*(DWORD*)(mmio+0x3804)=0;	//addrhigh
	*(DWORD*)(mmio+0x3808)=8*16;	//8个*每个16B
	*(DWORD*)(mmio+0x3810)=0;	//head
	*(DWORD*)(mmio+0x3818)=0;	//tail
	*(DWORD*)(mmio+0x400)=0x10400fa;//control
	//   1    0    4    0    0    f    a
	//0001 0000 0100 0000 0000 1111 1010
	//TCTL_EN			1<<1
	//TCTL_PSP			1<<3
	//(15 << TCTL_CT_SHIFT)		4
	//(64 << TCTL_COLD_SHIFT)	12
	//TCTL_RTLC			1<<24
	//----------------------------------------------
}


/*
struct RecvDesc
{   
    volatile u64 addr;
    volatile u16 len;
    volatile u16 checksum;
    volatile u8 status;
    volatile u8 errors;
    volatile u16 special;
};
struct TransDesc
{
    volatile u64 addr;
    volatile u16 len;
    volatile u8 cso;
    volatile u8 cmd;
    volatile u8 status;
    volatile u8 css;
    volatile u16 special;
};
*/
WORD checksum(WORD *buffer,int size)  
{  
	DWORD cksum=0;  
	while(size>1)  
	{  
		cksum+=*buffer++;  
		size-=2;  
	}  
	if(size)	//偶数跳过这一步
	{  
		cksum+=*(BYTE*)buffer;  
	}      

	//32位转换成16位
	while (cksum>>16)  
	{
		cksum=(cksum>>16)+(cksum & 0xffff);  
	}
	return (WORD) (~cksum);  
}  
static void sendpacket()
{
	//包错了就return

	//txdesc
	DWORD head=*(DWORD*)(mmio+0x3810);
	QWORD desc=txdesc+0x10*head;
	QWORD buffer=*(QWORD*)desc;



	//[0,0xd]:mac头，总共0xe=14B
	*(QWORD*)buffer=0xffffffffffff;		//[0,5]:dst
	*(QWORD*)(buffer+6)=macaddr;		//[6,0xb]src
	*(WORD*)(buffer+12)=0x0008;		//[0xc,0xd]:type
	//[0xe,0x0x21]ip头,总共0x14=20B
	*(BYTE*)(buffer+0xe)=0x45;		//[0xe]:(版本<<4)+报头长度
	*(BYTE*)(buffer+0xf)=0;			//[0xf]:typeofsevice
	*(WORD*)(buffer+0x10)=0x4000;		//[0x10,0x11]:length
	*(WORD*)(buffer+0x12)=0x233;		//[0x12,0x13]:identification
	*(WORD*)(buffer+0x14)=0;		//[0x14,0x15]:fragment offset
	*(BYTE*)(buffer+0x16)=0x40;		//[0x16]:ttl
	*(BYTE*)(buffer+0x17)=1;		//[0x17]:protocol
	*(WORD*)(buffer+0x18)=0;		//[0x18,0x19]:checksum先置0
	*(DWORD*)(buffer+0x1a)=0x07cca8c0;	//[0x1a,0x1d]:src:192.168.204.1
	*(DWORD*)(buffer+0x1e)=0x07c8c8c8;	//[0x1e,0x21]:dst
	*(WORD*)(buffer+0x18)=checksum((WORD*)(buffer+0xe),20);
	//[0x22,0x31]:icmp整体，0x10=16B
	*(BYTE*)(buffer+0x22)=8;		//[0x22]:echo request
	*(BYTE*)(buffer+0x23)=0;		//[0x23]:code
	*(WORD*)(buffer+0x24)=0;		//[0x24,0x25]:checksum
	*(WORD*)(buffer+0x26)=(WORD)head;	//[0x26,0x27]:idetifer
	*(WORD*)(buffer+0x28)=(WORD)head;	//[0x28,0x29]:sn
	*(QWORD*)(buffer+0x2a)=0x123456;	//[0x2a,0x31]:timestamp
	//64-20-16=28B				//没有data部分应该可以把
	*(WORD*)(buffer+0x24)=checksum((WORD*)(buffer+0x22),44);


	//填满desc
	*(WORD*)(desc+8)=14+64;		//length
	*(BYTE*)(desc+11)=(1<<3)|(1<<1)|1;	//reportstatus
						//insert fcs/crc
						//end of packet


	//这玩意是头追尾巴，改了tail，head自动往下追不管软件的事
	DWORD tail=*(DWORD*)(mmio+0x3818);
	*(DWORD*)(mmio+0x3818)=(tail+1)%8;


	//检查是否发送失败
	QWORD timeout=0;
	while(1)
	{
		timeout++;
		if(timeout>0xfffffff)
		{
			say("fail",0);
			return;
		}

		BYTE status=*(BYTE*)(desc+12);
		if( (status&0x1) == 0x1 )
		{
			say("sent packet@",buffer);
			return;
		}
	}
}




/*
void explainpacket()
{
}
*/



void initethernet()
{
	//clear home
	QWORD addr=ethhome;
	for(;addr<ethhome+0x100000;addr++) *(BYTE*)addr=0;
	portaddr=0;
	mmio=0;

	findport();		//port addr of port
	if(portaddr==0) return;

	probepci();
	if(mmio==0) return;

	ethernet();

	remember(0x646e6573,sendpacket);

	say("",0);
}
