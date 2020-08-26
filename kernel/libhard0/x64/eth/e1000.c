typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
u32 in32(u16);
void out32(u16, u32);
void say(char*, ...);




static u8* mmio;
static u64 macaddr;
static u32 ipaddr;




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
static void sendpacket(u64 addr,int size)
{
	//包错了就return

	//txdesc
	u32 head=*(u32*)(mmio+0x3810);
	u64 desc=txdesc+0x10*head;

	u8* to=(u8*)( *(u64*)desc );
	u8* from=(u8*)(addr);
	int i;
	for(i=0;i<size;i++) to[i]=from[i];


	//填满desc
	*(u16*)(desc+8)=size;			//length
	*(u8*)(desc+11)=(1<<3)|(1<<1)|1;	//reportstatus
						//insert fcs/crc
						//end of packet


	//这玩意是头追尾巴，改了tail，head自动往下追不管软件的事
	u32 tail=*(u32*)(mmio+0x3818);
	*(u32*)(mmio+0x3818)=(tail+1)%8;


	//检查是否发送失败
	u64 timeout=0;
	while(1)
	{
		timeout++;
		if(timeout>0xfffffff)
		{
			say("fail",0);
			return;
		}

		u8 status=*(u8*)(desc+12);
		if( (status&0x1) == 0x1 )
		{
			say("sent:",desc);
			return;
		}
	}
}




u32 str2ip(u8* arg)
{
	u32 first=0;
	u32 second=0;
	u32 third=0;
	u32 fourth=0;
	u32 retip=0;
	u32 i=0;

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




u16 checksum(u16 *buffer,int size)  
{  
	u32 cksum=0;  
	while(size>1)  
	{  
		cksum+=*buffer++;  
		size-=2;  
	}  
	if(size)	//偶数跳过这一步
	{  
		cksum+=*(u8*)buffer;  
	}      

	//32位转换成16位
	while (cksum>>16)  
	{
		cksum=(cksum>>16)+(cksum & 0xffff);  
	}
	return (u16) (~cksum);  
}  




struct arp
{
	u8 macdst[6];		//[0,5]
	u8 macsrc[6];		//[6,0xb]
	u16 type;		//[0xc,0xd]

	u16 hwtype;		//[0xe,0xf]
	u16 protocoltype;	//[0x10,0x11]
	u8 hwsize;		//[0x12]
	u8 protocolsize;	//[0x13]
	u16 opcode;		//[0x14,0x15]
	u8 sendermac[6];	//[0x16,0x1b]
	u8 senderip[4];	//[0x1c,0x1f]
	u8 targetmac[6];	//[0x20,0x25]
	u8 targetip[4];	//[0x26,0x29]
};
static struct arp arp;
static void arprequest(u8* arg1)
{
	u32 dstip=str2ip(arg1);

	*(u64*)(arp.macdst)=0xffffffffffff;
	*(u64*)(arp.macsrc)=macaddr;
	arp.type=0x0608;

	arp.hwtype=0x0100;
	arp.protocoltype=8;
	arp.hwsize=6;
	arp.protocolsize=4;
	arp.opcode=0x0100;		//这是请求，回复是0x200
	*(u64*)(arp.sendermac)=macaddr;
	*(u32*)(arp.senderip)=ipaddr;		//200.200.200.23
	*(u64*)(arp.targetmac)=0;
	*(u32*)(arp.targetip)=dstip;


	sendpacket((u64)(&arp),14+0x1c);
}




struct icmp
{
	u8 macdst[6];		//[0,5]
	u8 macsrc[6];		//[6,0xb]
	u16 type;		//[0xc,0xd]

	u8 iphead;		//[0xe]
	u8 tos;		//[0xf]
	u16 length;		//[0x10,0x11]
	u16 id;		//[0x12,0x13]
	u16 fragoffset;	//[0x14,0x15]
	u8 ttl;		//[0x16]
	u8 protocol;		//[0x17]
	u16 checksum;		//[0x18,0x19]
	u8 ipsrc[4];		//[0x1a,0x1d]
	u8 ipdst[4];		//[0x1e,0x21]

	u8 icmptype;		//[0x22]
	u8 code;		//[0x23]
	u16 icmpchecksum;	//[0x24,0x25]
	u16 identifer;		//[0x26,0x27]
	u16 sn;		//[0x28,0x29]

	u8 timestamp[8];	//[0x2a,0x31]
	u8 data[0x30];	//[0x32,0x61]
};
static struct icmp icmp;
static void icmprequest(u8* arg1)
{
	u32 dstip=str2ip(arg1);
	*(u64*)(icmp.macdst)=0xffffffffffff;
	*(u64*)(icmp.macsrc)=macaddr;
	icmp.type=0x0008;

	icmp.iphead=0x45;
	icmp.tos=0;
	icmp.length=(20+64)<<8;
	icmp.id=0x233;
	icmp.fragoffset=0;
	icmp.ttl=0x40;
	icmp.protocol=1;
	icmp.checksum=0;
	*(u32*)(icmp.ipsrc)=ipaddr;		//200.200.200.23
	*(u32*)(icmp.ipdst)=dstip;		//200.200.200.7
	icmp.checksum=checksum((u16*)(&(icmp.iphead)),20);

	icmp.icmptype=8;		//echo request
	icmp.code=0;
	icmp.icmpchecksum=0;
	icmp.identifer=233;
	icmp.sn=233;

	int i;
	for(i=0;i<0x30;i++) icmp.data[i]=i;
	icmp.icmpchecksum=checksum((u16*)(&(icmp.icmptype)),0x40);


	sendpacket((u64)(&icmp),14+20+0x40);
}




struct RecvDesc
{   
    volatile u64 addr;
    volatile u16 len;
    volatile u16 checksum;
    volatile u8 status;
    volatile u8 errors;
    volatile u16 special;
};
void explainicmp(u64 bufferaddr,u64 length)
{
	u64 targetmac=( *(u64*)(bufferaddr) ) &0xffffffffffff;
	if(targetmac!=macaddr) return;
	u32 targetip=*(u32*)(bufferaddr+0x1e);
	if(targetip!=ipaddr) return;

	u64 sendermac=( *(u64*)(bufferaddr+6) ) &0xffffffffffff;
	u16 id=*(u32*)(bufferaddr+0x12);
	u32 senderip=*(u32*)(bufferaddr+0x1a);

	*(u64*)(icmp.macdst)=sendermac;
	*(u64*)(icmp.macsrc)=macaddr;
	icmp.type=0x0008;

	icmp.iphead=0x45;
	icmp.tos=0;
	icmp.length=(20+64)<<8;
	icmp.id=id;
	icmp.fragoffset=0;
	icmp.ttl=0x40;
	icmp.protocol=1;
	icmp.checksum=0;
	*(u32*)(icmp.ipsrc)=ipaddr;		//200.200.200.23
	*(u32*)(icmp.ipdst)=senderip;		//200.200.200.7
	icmp.checksum=checksum((u16*)(&(icmp.iphead)),20);

	icmp.icmptype=0;		//echo request
	icmp.code=0;
	icmp.icmpchecksum=0;
	u16 identifer=*(u32*)(bufferaddr+0x26);
	icmp.identifer=identifer;
	u16 sn=*(u32*)(bufferaddr+0x28);
	icmp.sn=sn;
	u64 timestamp=*(u64*)(bufferaddr+0x2a);
	*(u64*)(icmp.timestamp)=timestamp;
	int i=0;
	for(i=0;i<0x30;i++) icmp.data[i]=*(u8*)(bufferaddr+0x32+i);

	icmp.icmpchecksum=checksum((u16*)(&(icmp.icmptype)),64);

	sendpacket((u64)(&icmp),14+20+64);
}
void explainipv4(u64 bufferaddr,u64 length)
{
	u8 whatthis=*(u8*)(bufferaddr+0x22);
	if(whatthis==8) explainicmp(bufferaddr,length);
}
void explainarp(u64 bufferaddr,u64 length)
{
	u32 targetip=*(u32*)(bufferaddr+0x26);
	shout("buf@",bufferaddr);
	shout("targetip:",targetip);
	if(targetip!=ipaddr) return;

	u64 sendermac=( *(u64*)(bufferaddr+6) ) &0xffffffffffff;
	u32 senderip=*(u32*)(bufferaddr+0x1c);
	shout("senderip:",senderip);

	*(u64*)(arp.macdst)=sendermac;
	*(u64*)(arp.macsrc)=macaddr;
	arp.type=0x0608;

	arp.hwtype=0x0100;
	arp.protocoltype=8;
	arp.hwsize=6;
	arp.protocolsize=4;
	arp.opcode=0x0200;		//这是回复
	*(u64*)(arp.sendermac)=macaddr;
	*(u32*)(arp.senderip)=ipaddr;		//200.200.200.23
	*(u64*)(arp.targetmac)=sendermac;
	*(u32*)(arp.targetip)=senderip;


	sendpacket((u64)(&arp),14+0x1c);
}
void explainpacket(u64 bufferaddr,u64 length)
{
	u16 type=*(u16*)(bufferaddr+12);
	type=( (type&0xff)<<8 )+(type>>8);	//高低位互换

	switch(type)
	{
		case 0x0806:explainarp(bufferaddr,length);
		case 0x0800:explainipv4(bufferaddr,length);
	}
}
void realisr22()
{
	u32 tail=*(u32*)(mmio+0x2818);

	while(1)
	{
		tail=(tail+1)%32;
		u64 desc=rxdesc+0x10*tail;

		u8 status=*(u8*)(desc+12);
		if( (status&0x1) == 0 ) break;

		u64 bufferaddr=*(u64*)(desc);
		u64 length=*(u16*)(desc+8);
		explainpacket(bufferaddr,length);

		volatile u32 dummy=*(u32*)(mmio+0xc0);//必须读，防止被优化
		*(u8*)(desc+12)=0;			//status写0
		*(u32*)(mmio+0x2818)=tail;		//更新tail
	}
}*/




u16 eepromread(u32 whichreg)
{
	u32* p=(u32*)(mmio+0x14);
	*p=(whichreg<<8)|0x1;		//start
	while(1)
	{
		if( (p[0]&0x10) == 0x10 ) return (u16)(p[0]>>8);
	}
}
void e1000_mmioinit(u8* mmio)
{
	say("e1000@mmio:%llx{\n", mmio);

	//---------------------macaddr-------------------
	u32 ral=*(u32*)(mmio+0x5400);	//receive address low
	if(ral)
	{
		u32 rah=*(u32*)(mmio+0x5404);
		macaddr=((u64)(rah&0xffff)<<32)+ral;
	}
	else
	{
		say("reading eeprom\n");
		u64 mac01 = eepromread(0);
		u64 mac23 = eepromread(1);
		u64 mac45 = eepromread(2);
		macaddr=(mac45<<32) + (mac23<<16) + mac01;
	}
	say("macaddr=%012x\n",macaddr);
	//------------------------------------------



/*
	//-----------------control-----------------
	//set link up
	*(u32*)(mmio+0)=(*(u32*)(mmio+0))|(1<<6);

	//clear multicast table array
	u64 i;
	for(i=0;i<128;i++) *(u32*)(mmio+0x5200+4*i)=0;

	//enable interrupt
	*(u32*)(mmio+0xd0)=0x1f6dc;

	//read to clear "interrupt cause read"
	say("icr=%x\n",*(u32*)(mmio+0xc0));
	//---------------------------------------




	//--------------------receive descriptor---------------------
	for(i=0;i<32;i++)
	{
		*(u64*)(rxdesc+0x10*i)=rxbuffer+0x800*i;	//每个2048B
	}
	*(u32*)(mmio+0x2800)=rxdesc;	//addrlow
	*(u32*)(mmio+0x2804)=0;	//addrhigh
	*(u32*)(mmio+0x2808)=32*16;	//32个*每个16B
	*(u32*)(mmio+0x2810)=0;	//head
	*(u32*)(mmio+0x2818)=31;	//tail
	*(u32*)(mmio+0x100)=0x400801e;//control
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
		*(u64*)(txdesc+0x10*i)=txbuffer+0x800*i;	//每个2048B
	}
	*(u32*)(mmio+0x3800)=txdesc;	//addrlow
	*(u32*)(mmio+0x3804)=0;	//addrhigh
	*(u32*)(mmio+0x3808)=8*16;	//8个*每个16B
	*(u32*)(mmio+0x3810)=0;	//head
	*(u32*)(mmio+0x3818)=0;	//tail
	*(u32*)(mmio+0x400)=0x10400fa;//control
	//   1    0    4    0    0    f    a
	//0001 0000 0100 0000 0000 1111 1010
	//TCTL_EN			1<<1
	//TCTL_PSP			1<<3
	//(15 << TCTL_CT_SHIFT)		4
	//(64 << TCTL_COLD_SHIFT)	12
	//TCTL_RTLC			1<<24
	//----------------------------------------------
*/
	say("}\n");
}
void e1000_portinit(u64 addr)
{
	u32 temp;
	say("e1000@port:%x{\n",addr);

	out32(0xcf8, addr+0x4);
	temp = in32(0xcfc);
	temp |= 1<<2;
	temp &= ~(u32)(1<<10);
	out32(0xcf8, addr+0x4);
	out32(0xcfc, temp);

	out32(0xcf8, addr+0x4);
	say("sts,cmd=%x\n", in32(0xcfc));

	out32(0xcf8, addr+0x3c);
	temp = in32(0xcfc);
	say("int line=%x,pin=%x\n", temp&0xff, (temp>>8)&0xff);

	out32(0xcf8, addr+0x10);
	temp = in32(0xcfc)&0xfffffff0;

	say("}\n");

	e1000_mmioinit((void*)(u64)temp);
}


