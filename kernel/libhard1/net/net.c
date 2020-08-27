typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
#define self_ip  0x4002000a
#define self_mac 0x563412005452
#define peer_ip  0x0202000a
#define peer_mac 0x0202000a5552
//
void e1000_write(u8* buf, int len);
//
void printmemory(void*, ...);
void say(char*, ...);




static u16 swap16(u16 tmp)
{
	return (tmp>>8) | ((tmp&0xff)<<8);
}
u16 checksum(u16 *buf, int len)  
{  
	u32 sum = 0;  
	while(len>1)
	{  
		sum += *buf;
		buf += 1;  
		len -= 2;  
	}  
	if(len)	//偶数跳过这一步
	{  
		sum += *(u8*)buf;  
	}      

	//32位转换成16位
	while(sum>>16)  
	{
		sum = (sum>>16) + (sum & 0xffff);  
	}
	return (u16)(~sum);
}  




struct arp
{
	u8 macdst[6];		//[0,5]
	u8 macsrc[6];		//[6,0xb]
	u16     type;		//[0xc,0xd]

	u16       hwtype;	//[0xe,0xf]
	u16 protocoltype;	//[0x10,0x11]
	u8        hwsize;	//[0x12]
	u8  protocolsize;	//[0x13]
	u16       opcode;	//[0x14,0x15]
	u8  sendermac[6];	//[0x16,0x1b]
	u8   senderip[4];	//[0x1c,0x1f]
	u8  targetmac[6];	//[0x20,0x25]
	u8   targetip[4];	//[0x26,0x29]
};
void arprequest(u8* url)
{
	struct arp arp;
	*(u64*)(arp.macdst) = 0xffffffffffff;
	*(u64*)(arp.macsrc) = 0;	//unknown
	arp.type = 0x0608;

	arp.hwtype = 0x0100;
	arp.protocoltype = 8;
	arp.hwsize = 6;
	arp.protocolsize = 4;
	arp.opcode = 0x0200;	//reply
	*(u64*)(arp.sendermac) = self_mac;
	*(u32*)(arp.senderip) =  self_ip;
	*(u64*)(arp.targetmac) = peer_mac;
	*(u32*)(arp.targetip) =  peer_ip;

	e1000_write((u8*)&arp, sizeof(struct arp));
}




struct icmp
{
	u8 macdst[6];		//[0,5]
	u8 macsrc[6];		//[6,0xb]
	u16     type;		//[0xc,0xd]

	u8      iphead;		//[0xe]
	u8         tos;		//[0xf]
	u16     length;		//[0x10,0x11]
	u16         id;		//[0x12,0x13]
	u16 fragoffset;		//[0x14,0x15]
	u8         ttl;		//[0x16]
	u8    protocol;		//[0x17]
	u16   checksum;		//[0x18,0x19]
	u8    ipsrc[4];		//[0x1a,0x1d]
	u8    ipdst[4];		//[0x1e,0x21]

	u8      icmptype;	//[0x22]
	u8          code;	//[0x23]
	u16 icmpchecksum;	//[0x24,0x25]
	u16    identifer;	//[0x26,0x27]
	u16           sn;	//[0x28,0x29]

	u8  timestamp[8];	//[0x2a,0x31]
	u8    data[0x30];	//[0x32,0x61]
};
void icmprequest(u8* url)
{
	struct icmp icmp;
	*(u64*)(icmp.macdst) = 0xffffffffffff;
	*(u64*)(icmp.macsrc) = 0;	//unknown
	icmp.type = 0x0008;

	icmp.iphead = 0x45;
	icmp.tos = 0;
	icmp.length = (20+64)<<8;
	icmp.id = 0x233;
	icmp.fragoffset = 0;
	icmp.ttl = 0x40;
	icmp.protocol = 1;
	icmp.checksum = 0;
	*(u32*)(icmp.ipsrc) = self_ip;
	*(u32*)(icmp.ipdst) = 0x0100a8c0;	//192.168.0.1
	icmp.checksum = checksum((u16*)&(icmp.iphead), 20);

	icmp.icmptype = 8;	//echo request
	icmp.code = 0;
	icmp.icmpchecksum = 0;
	icmp.identifer = 233;
	icmp.sn = 233;

	int j;
	for(j=0;j<0x30;j++)icmp.data[j] = j;
	icmp.icmpchecksum = checksum((u16*)&icmp.icmptype, 0x40);

	e1000_write((u8*)&icmp, sizeof(struct icmp));
}




struct dhcp{
	//eth
	u8 macdst[6];		//[0,5]
	u8 macsrc[6];		//[6,0xb]
	u16     type;		//[0xc,0xd]

	//ip
	u8      iphead;		//[0xe]
	u8         tos;		//[0xf]
	u16     length;		//[0x10,0x11]
	u16         id;		//[0x12,0x13]
	u16 fragoffset;		//[0x14,0x15]
	u8         ttl;		//[0x16]
	u8    protocol;		//[0x17]
	u16   checksum;		//[0x18,0x19]
	u8    ipsrc[4];		//[0x1a,0x1d]
	u8    ipdst[4];		//[0x1e,0x21]

	//udp
	u16 srcport;
	u16 dstport;
	u16 udplen;
	u16 udpsum;

	//dhcp
	u8 op;
	u8 htype;
	u8 hlen;
	u8 hops;
	u32 xid;
	u16 secs;
	u16 flags;
	u32 ciaddr;
	u32 yiaddr;
	u32 siaddr;
	u32 giaddr;
	u8 chaddr[16];
	u8 sname[64];
	u8 file[128];
};
void dhcprequest(u8* url)
{
	int len;
	struct dhcp dhcp;

	dhcp.op = 1;
	dhcp.htype = 1;
	dhcp.hlen = 6;
	dhcp.hops = 0;
	dhcp.xid = 0x89abcdef;
	dhcp.secs = 0;
	dhcp.flags = 0;
	dhcp.ciaddr = 0;
	dhcp.yiaddr = 0;
	dhcp.siaddr = 0;
	dhcp.giaddr = 0;
	dhcp.chaddr[0] = 0;
	dhcp.sname[0] = 0;
	dhcp.file[0] = 0;
	len = 12+16+16+64+128;

	len += 8;
	dhcp.srcport = 68;
	dhcp.dstport = swap16(1234);
	dhcp.udplen = swap16(len);
	dhcp.udpsum = 0;

	len += 20;
	dhcp.iphead = 0x45;
	dhcp.tos = 0;
	dhcp.length = swap16(len);
	dhcp.id = 0x233;
	dhcp.fragoffset = 0;
	dhcp.ttl = 0x40;
	dhcp.protocol = 0x11;
	dhcp.checksum = 0;
	*(u32*)(dhcp.ipsrc) = self_ip;  //0x00000000;  //0.0.0.0
	*(u32*)(dhcp.ipdst) = 0xd900a8c0;  //0xffffffff;  //255.255.255.255
	dhcp.checksum = checksum((u16*)&dhcp.iphead, 20);

	*(u64*)(dhcp.macdst) = 0xffffffffffff;
	*(u64*)(dhcp.macsrc) = 0;	//unknown
	dhcp.type = 0x0008;
	e1000_write((u8*)&dhcp, sizeof(struct dhcp));
}




/*
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
}*/
