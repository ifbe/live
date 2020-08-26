#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long
//stor
void ide_portinit(u64);
void ahci_portinit(u64);
void nvme_portinit(u64);
//eth
void e1000_portinit(u64);
//usb
void ehci_portinit(u64);
void xhci_portinit(u64);
//
void out32(u32, u32);
u32 in32(u32);
//
void say(void*, ...);




void initpci()
{
	u32 bus,dev,fun,addr;
	u32 idid,type;
	for(bus=0;bus<256;bus++){
	for(dev=0;dev<32;dev++){
	for(fun=0;fun<8;fun++){
		addr = 0x80000000 | (bus<<16) | (dev<<11) | (fun<<8);
		out32(0xcf8, addr);
		idid = in32(0xcfc);
		if(idid == 0xffffffff)continue;

		out32(0xcf8, addr+8);
		type = in32(0xcfc);
		say("@%08x: idid=%08x, type=%08x\n", addr, idid, type);

		switch(type >> 16){
		case 0x0101:
			ide_portinit(addr);
			break;

		case 0x0106:
			if(0x01 == ((type>>8)&0xff))ahci_portinit(addr);
			break;

		case 0x0108:
			if(0x02 == ((type>>8)&0xff))nvme_portinit(addr);
			break;

		case 0x0200:
			if(0x100e8086 == idid)e1000_portinit(addr);
			break;

/*		case 0x0c03:
			switch((type>>8)&0xff){
			case 0x00:uhci_portinit(addr);break;
			case 0x10:ohci_portinit(addr);break;
			case 0x20:ehci_portinit(addr);break;
			case 0x30:xhci_portinit(addr);break;
			}//usbver
			break;
*/
		}//class,subclass
	}//fun
	}//dev
	}//bus
}
