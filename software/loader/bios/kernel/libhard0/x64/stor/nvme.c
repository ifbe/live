#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long
u32 in32(u32 addr);
void out32(u32 port, u32 addr);
void say(char* , ...);




int nvme_portinit(u64 addr)
{
	unsigned int temp;
	say("(nvme)pciaddr:%x{\n",addr);
/*
	out32(0xcf8,addr+0x4);
	temp=in32(0xcfc)|(1<<10)|(1<<2);		//bus master=1

	out32(0xcf8,addr+0x4);
	out32(0xcfc,temp);

	out32(0xcf8,addr+0x4);
	say("    pci sts&cmd:%x\n",(u64)in32(0xcfc));
*/
	//ide port
	out32(0xcf8,addr+0x10);
	say("bar0:%x\n" , in32(0xcfc)&0xfffffffe );
	out32(0xcf8,addr+0x14);
	say("bar1:%x\n" , in32(0xcfc)&0xfffffffe );
	out32(0xcf8,addr+0x18);
	say("bar2:%x\n" , in32(0xcfc)&0xfffffffe );
	out32(0xcf8,addr+0x1c);
	say("bar3:%x\n" , in32(0xcfc)&0xfffffffe );

	say("}\n");
}
