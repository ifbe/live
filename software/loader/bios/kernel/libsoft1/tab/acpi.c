#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long
#define hex32(a,b,c,d) (a | (b<<8) | (c<<16) | (d<<24))
#define _FACP_ hex32('F','A','C','P')
#define _DSDT_ hex32('D','S','D','T')
#define _S5_ hex32('_','S','5','_')
void say(void*, ...);




void parse_S5_(void* p)
{
	u16 data;
	say("%.4s@%x\n", p, p);

	data = (*(u8*)(p+8))<<10;
	say("data=%x\n", data);
	*(u16*)0xffe = data;
}
void parsedsdt(void* p)
{
	int j,cnt;
	say("%.4s@%x\n", p, p);

	cnt = *(u32*)(p+4);
	for(j=4;j<cnt;j++){
		if(_S5_ == *(u32*)(p+j)){
			parse_S5_(p+j);
			break;
		}
	}
}
void parsefacp(void* p)
{
	u16 port;
	u64 addr;
	say("%.4s@%x\n", p, p);

	port = *(u16*)(p+0x40);
	say("port=%x\n", port);
	*(u16*)0xffc = port;

	addr = *(u32*)(p+0x28);
	parsedsdt((void*)addr);
}




void parsetable(void* p)
{
	u64 addr = *(u32*)p;
	switch(*(u32*)addr){
		case _FACP_:parsefacp((void*)addr);break;
		default:say("%.4s@%x\n", (u8*)addr, addr);
	}
}
void parsexsdt(void* p)
{
	int c,j;
	say("xsdt@%x\n",p);

	p = (void*)(u64)(*(u32*)(p+0x18));
	c = *(u8*)(p+4);
	say("cnt:%x\n", c);

	p += 0x24;
	c -= 0x24;
	for(j=0;j<c;j+=8)parsetable(p+j);
}
void parsersdt(void* p)
{
	int c,j;
	say("rsdt@%x\n",p);

	p = (void*)(u64)(*(u32*)(p+0x10));
	c = *(u8*)(p+4);
	say("cnt:%x\n", c);

	p += 0x24;
	c -= 0x24;
	for(j=0;j<c;j+=4)parsetable(p+j);
}




void parseacpi(void* p)
{
	say("RSD PTR @%x\n",p);
	if(*(u8*)(p+0xf) == 0)parsersdt(p);
	else parsexsdt(p);
}
void initacpi()
{
	int j;
	u64* p = (void*)0xe0000;
	for(j=0;j<0x4000;j++)
	{
		if(p[j] == 0x2052545020445352)parseacpi(&p[j]);
	}
}
