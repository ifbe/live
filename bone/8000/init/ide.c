#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define QWORD unsigned long long

#define pcihome 0x40000

#define idehome 0x100000




//用了别人的函数
void say(char* , ...);




static unsigned int finddevice()
{
	QWORD* addr=(QWORD*)(pcihome);
	int i;
	unsigned int temp;
	for(i=0;i<0x80;i++)				//每?0x40
	{
		temp=addr[8*i+1];
		temp&=0xffff0000;
		if(temp==0x01010000)
		{
			addr[8*i+3]=0x656469;			//ide

			return addr[8*i+2];		//1.返回要??的??的portaddress
		}
	}
return 0;
}








static inline void out32( unsigned short port, unsigned int val )
{
    asm volatile( "outl %0, %1" : : "a"(val), "Nd"(port) );
}
static inline unsigned int in32( unsigned short port )
{
    unsigned int ret;
    asm volatile( "inl %1, %0"  : "=a"(ret) : "Nd"(port) );
    return ret;
}
static unsigned int probepci(QWORD addr)
{
//?：pci地址
//出：?存地址
	unsigned int temp;
	say("(ide)pciaddr:%x{\n",addr);

	out32(0xcf8,addr+0x4);
	temp=in32(0xcfc)|(1<<10)|(1<<2);		//bus master=1

	out32(0xcf8,addr+0x4);
	out32(0xcfc,temp);

	out32(0xcf8,addr+0x4);
	say("    pci sts&cmd:%x",(QWORD)in32(0xcfc));

	//ide port
	out32(0xcf8,addr+0x10);
	temp=in32(0xcfc)&0xfffffffe;
	*(QWORD*)(idehome+0x10)=temp;		//command
	say("    (command)bar0:%x\n",temp);

	out32(0xcf8,addr+0x14);
	temp=in32(0xcfc)&0xfffffffe;
	*(QWORD*)(idehome+0x20)=temp;		//control
	say("    (control)bar1:%x\n",temp);

	out32(0xcf8,addr+0x18);
	temp=in32(0xcfc)&0xfffffffe;
	say("    bar2:%x\n",temp);

	out32(0xcf8,addr+0x1c);
	temp=in32(0xcfc)&0xfffffffe;
	say("    bar3:%x\n",temp);

	//asdfasdfasdfasdf
	*(QWORD*)(idehome)=0x656469;

	say("}\n");
}








void initide()
{
	QWORD addr;

	//clear home
	addr=idehome;
	for(;addr<idehome+0x100000;addr++) *(BYTE*)addr=0;

	//find device
	addr=finddevice();		//get port addr of this storage device
	if(addr==0) return;

	//probe pci
	addr=probepci(addr);		//memory addr of ahci
	if(addr==0) return;
}
