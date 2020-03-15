#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long
//
void initahci(u64);
void initide(u64);
void initehci(u64);
void initxhci(u64);
//
void out32(u32, u32);
u32 in32(u32);
//
void say(void*, ...);




void initpci()
{
	u32 bus,dev,fun,addr;
	u32 id,type;
	for(bus=0;bus<256;bus++)
	{
		for(dev=0;dev<32;dev++)
		{
			for(fun=0;fun<8;fun++)
			{
				addr = 0x80000000|(bus<<16)|(dev<<11)|(fun<<8);
				out32(0xcf8, addr);
				id = in32(0xcfc);
				if(id == 0xffffffff)continue;

				out32(0xcf8, addr+8);
				type = in32(0xcfc);
				if( (type&0xffff0000) == 0x01010000)
				{
					initide(addr);
				}
				else if( (type&0xffffff00) == 0x01060100)
				{
					initahci(addr);
				}
				else if( (type&0xffffff00) == 0x0c033000)
				{
					//initxhci(addr);
				}
				else say("%x:%x\n", id, type);
			}
		}
	}
}
