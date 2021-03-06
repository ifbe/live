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
static DWORD ipaddr;




//全部设备信息我放在0x140000了，格式如下:(非本环境自己处理这一步)
//[0,7]:(vendorid<<16)+deviceid
//[8,0xf]:(class<<24)+(subclass<<16)+(progif<<8)+revisionid
//[0x10,0x17]:portaddress of the device
//[0x18,0x1f]:ansciiname of the device
//本函数作用是：
//1.返回要驱动的设备的portaddress
//2.填上[0x18,0x1f],(为了工整好看)
void findport()
{
	QWORD* addr=(QWORD*)0x140000;
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
void probepci()
{
	say("ethernet(port)@",portaddr);

	out32(0xcf8,portaddr+0x4);
	DWORD temp=in32(0xcfc);
	temp|=1<<2;
	temp&=~(DWORD)(1<<10);
	out32(0xcf8,portaddr+0x4);
	out32(0xcfc,temp);

	out32(0xcf8,portaddr+0x4);
	say("pci status and command:",(QWORD)in32(0xcfc));

	out32(0xcf8,portaddr+0x10);
	mmio=in32(0xcfc)&0xfffffff0;

	out32(0xcf8,portaddr+0x3c);
	DWORD intwhat=in32(0xcfc);
	say("interrupt line:",intwhat&0xff);
	say("interrupt pin:",(intwhat>>8)&0xff);
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
void e1000()
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




	//-----------------------------------
	interrupt();
	//--------------------------------------




	//-----------------control-----------------
	//set link up
	*(DWORD*)(mmio+0)=(*(DWORD*)(mmio+0))|(1<<6);

	//clear multicast table array
	QWORD i;
	for(i=0;i<128;i++) *(DWORD*)(mmio+0x5200+4*i)=0;

	//enable interrupt
	*(DWORD*)(mmio+0xd0)=0x1f6dc;

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
static void sendpacket(QWORD addr,int size)
{
	//包错了就return

	//txdesc
	DWORD head=*(DWORD*)(mmio+0x3810);
	QWORD desc=txdesc+0x10*head;

	BYTE* to=(BYTE*)( *(QWORD*)desc );
	BYTE* from=(BYTE*)(addr);
	int i;
	for(i=0;i<size;i++) to[i]=from[i];


	//填满desc
	*(WORD*)(desc+8)=size;			//length
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
			say("sent:",desc);
			return;
		}
	}
}




DWORD str2ip(BYTE* arg)
{
	DWORD first=0;
	DWORD second=0;
	DWORD third=0;
	DWORD fourth=0;
	DWORD retip=0;
	DWORD i=0;

	//第一个
	while(arg[i]>'.')
	{
		if( (arg[i]>='0')&&(arg[i]<='9') )
		{
			first=first*10+arg[i]-0x30;
		}
		i++;
	}
	if(first==0)
	{
		print("error1",0);
		return 0;
	}
	i++;

	//第二个
	while(arg[i]>'.')
	{
		if( (arg[i]>='0')&&(arg[i]<='9') )
		{
			second=second*10+arg[i]-0x30;
		}
		i++;
	}
	if(second==0)
	{
		print("error2",0);
		return 0;
	}
	i++;

	//第三个
	while(arg[i]>'.')
	{
		if( (arg[i]>='0')&&(arg[i]<='9') )
		{
			third=third*10+arg[i]-0x30;
		}
		i++;
	}
	if(third==0)
	{
		print("error3",0);
		return 0;
	}
	i++;

	//第四个
	while(arg[i]>'.')
	{
		if( (arg[i]>='0')&&(arg[i]<='9') )
		{
			fourth=fourth*10+arg[i]-0x30;
		}
		i++;
	}
	if(fourth==0)
	{
		print("error4",0);
		return 0;
	}
	i++;

	retip=first+(second<<8)+(third<<16)+(fourth<<24);
	print("ip:",retip);
	return retip;
}




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




struct arp
{
	BYTE macdst[6];		//[0,5]
	BYTE macsrc[6];		//[6,0xb]
	WORD type;		//[0xc,0xd]

	WORD hwtype;		//[0xe,0xf]
	WORD protocoltype;	//[0x10,0x11]
	BYTE hwsize;		//[0x12]
	BYTE protocolsize;	//[0x13]
	WORD opcode;		//[0x14,0x15]
	BYTE sendermac[6];	//[0x16,0x1b]
	BYTE senderip[4];	//[0x1c,0x1f]
	BYTE targetmac[6];	//[0x20,0x25]
	BYTE targetip[4];	//[0x26,0x29]
};
static struct arp arp;
static void arprequest(BYTE* arg1)
{
	DWORD dstip=str2ip(arg1);

	*(QWORD*)(arp.macdst)=0xffffffffffff;
	*(QWORD*)(arp.macsrc)=macaddr;
	arp.type=0x0608;

	arp.hwtype=0x0100;
	arp.protocoltype=8;
	arp.hwsize=6;
	arp.protocolsize=4;
	arp.opcode=0x0100;		//这是请求，回复是0x200
	*(QWORD*)(arp.sendermac)=macaddr;
	*(DWORD*)(arp.senderip)=ipaddr;		//200.200.200.23
	*(QWORD*)(arp.targetmac)=0;
	*(DWORD*)(arp.targetip)=dstip;


	sendpacket((QWORD)(&arp),14+0x1c);
}




struct icmp
{
	BYTE macdst[6];		//[0,5]
	BYTE macsrc[6];		//[6,0xb]
	WORD type;		//[0xc,0xd]

	BYTE iphead;		//[0xe]
	BYTE tos;		//[0xf]
	WORD length;		//[0x10,0x11]
	WORD id;		//[0x12,0x13]
	WORD fragoffset;	//[0x14,0x15]
	BYTE ttl;		//[0x16]
	BYTE protocol;		//[0x17]
	WORD checksum;		//[0x18,0x19]
	BYTE ipsrc[4];		//[0x1a,0x1d]
	BYTE ipdst[4];		//[0x1e,0x21]

	BYTE icmptype;		//[0x22]
	BYTE code;		//[0x23]
	WORD icmpchecksum;	//[0x24,0x25]
	WORD identifer;		//[0x26,0x27]
	WORD sn;		//[0x28,0x29]

	BYTE timestamp[8];	//[0x2a,0x31]
	BYTE data[0x30];	//[0x32,0x61]
};
static struct icmp icmp;
static void icmprequest(BYTE* arg1)
{
	DWORD dstip=str2ip(arg1);
	*(QWORD*)(icmp.macdst)=0xffffffffffff;
	*(QWORD*)(icmp.macsrc)=macaddr;
	icmp.type=0x0008;

	icmp.iphead=0x45;
	icmp.tos=0;
	icmp.length=(20+64)<<8;
	icmp.id=0x233;
	icmp.fragoffset=0;
	icmp.ttl=0x40;
	icmp.protocol=1;
	icmp.checksum=0;
	*(DWORD*)(icmp.ipsrc)=ipaddr;		//200.200.200.23
	*(DWORD*)(icmp.ipdst)=dstip;		//200.200.200.7
	icmp.checksum=checksum((WORD*)(&(icmp.iphead)),20);

	icmp.icmptype=8;		//echo request
	icmp.code=0;
	icmp.icmpchecksum=0;
	icmp.identifer=233;
	icmp.sn=233;

	int i;
	for(i=0;i<0x30;i++) icmp.data[i]=i;
	icmp.icmpchecksum=checksum((WORD*)(&(icmp.icmptype)),0x40);


	sendpacket((QWORD)(&icmp),14+20+0x40);
}




/*
void explainpacket()
{
}
*/



void main()
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

	e1000();
	ipaddr=0x17c8c8c8;

	remember(0x676e6970,icmprequest);
	remember(0x707261,arprequest);
	//remember(0x646e6573,sendpacket);

	say("",0);
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
*/
void explainicmp(QWORD bufferaddr,QWORD length)
{
	QWORD targetmac=( *(QWORD*)(bufferaddr) ) &0xffffffffffff;
	if(targetmac!=macaddr) return;
	DWORD targetip=*(DWORD*)(bufferaddr+0x1e);
	if(targetip!=ipaddr) return;

	QWORD sendermac=( *(QWORD*)(bufferaddr+6) ) &0xffffffffffff;
	WORD id=*(DWORD*)(bufferaddr+0x12);
	DWORD senderip=*(DWORD*)(bufferaddr+0x1a);

	*(QWORD*)(icmp.macdst)=sendermac;
	*(QWORD*)(icmp.macsrc)=macaddr;
	icmp.type=0x0008;

	icmp.iphead=0x45;
	icmp.tos=0;
	icmp.length=(20+64)<<8;
	icmp.id=id;
	icmp.fragoffset=0;
	icmp.ttl=0x40;
	icmp.protocol=1;
	icmp.checksum=0;
	*(DWORD*)(icmp.ipsrc)=ipaddr;		//200.200.200.23
	*(DWORD*)(icmp.ipdst)=senderip;		//200.200.200.7
	icmp.checksum=checksum((WORD*)(&(icmp.iphead)),20);

	icmp.icmptype=0;		//echo request
	icmp.code=0;
	icmp.icmpchecksum=0;
	WORD identifer=*(DWORD*)(bufferaddr+0x26);
	icmp.identifer=identifer;
	WORD sn=*(DWORD*)(bufferaddr+0x28);
	icmp.sn=sn;
	QWORD timestamp=*(QWORD*)(bufferaddr+0x2a);
	*(QWORD*)(icmp.timestamp)=timestamp;
	int i=0;
	for(i=0;i<0x30;i++) icmp.data[i]=*(BYTE*)(bufferaddr+0x32+i);

	icmp.icmpchecksum=checksum((WORD*)(&(icmp.icmptype)),64);

	sendpacket((QWORD)(&icmp),14+20+64);
}
void explainipv4(QWORD bufferaddr,QWORD length)
{
	BYTE whatthis=*(BYTE*)(bufferaddr+0x22);
	if(whatthis==8) explainicmp(bufferaddr,length);
}
void explainarp(QWORD bufferaddr,QWORD length)
{
	DWORD targetip=*(DWORD*)(bufferaddr+0x26);
	shout("buf@",bufferaddr);
	shout("targetip:",targetip);
	if(targetip!=ipaddr) return;

	QWORD sendermac=( *(QWORD*)(bufferaddr+6) ) &0xffffffffffff;
	DWORD senderip=*(DWORD*)(bufferaddr+0x1c);
	shout("senderip:",senderip);

	*(QWORD*)(arp.macdst)=sendermac;
	*(QWORD*)(arp.macsrc)=macaddr;
	arp.type=0x0608;

	arp.hwtype=0x0100;
	arp.protocoltype=8;
	arp.hwsize=6;
	arp.protocolsize=4;
	arp.opcode=0x0200;		//这是回复
	*(QWORD*)(arp.sendermac)=macaddr;
	*(DWORD*)(arp.senderip)=ipaddr;		//200.200.200.23
	*(QWORD*)(arp.targetmac)=sendermac;
	*(DWORD*)(arp.targetip)=senderip;


	sendpacket((QWORD)(&arp),14+0x1c);
}
void explainpacket(QWORD bufferaddr,QWORD length)
{
	WORD type=*(WORD*)(bufferaddr+12);
	type=( (type&0xff)<<8 )+(type>>8);	//高低位互换

	switch(type)
	{
		case 0x0806:explainarp(bufferaddr,length);
		case 0x0800:explainipv4(bufferaddr,length);
	}
}
void realisr22()
{
	DWORD tail=*(DWORD*)(mmio+0x2818);

	while(1)
	{
		tail=(tail+1)%32;
		QWORD desc=rxdesc+0x10*tail;

		BYTE status=*(BYTE*)(desc+12);
		if( (status&0x1) == 0 ) break;

		QWORD bufferaddr=*(QWORD*)(desc);
		QWORD length=*(WORD*)(desc+8);
		explainpacket(bufferaddr,length);

		volatile DWORD dummy=*(DWORD*)(mmio+0xc0);//必须读，防止被优化
		*(BYTE*)(desc+12)=0;			//status写0
		*(DWORD*)(mmio+0x2818)=tail;		//更新tail
	}
}
