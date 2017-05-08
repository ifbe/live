#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long
void say(void*, ...);




void parsetable(void* p)
{
	u32* q = (void*)(u64)(*(u32*)p);
	say("%x,%x,%x", p, q, *q);
}
void parsexsdt(void* p)
{
	int c,j;
	say("xsdt");

	p = (void*)(u64)(*(u32*)(p+0x18));
	c = *(u8*)(p+4);
	say("count:%x", c);

	p += 0x24;
	c -= 0x24;
	for(j=0;j<c;j+=8)parsetable(p+j);
}
void parsersdt(void* p)
{
	int c,j;
	say("rsdt");

	p = (void*)(u64)(*(u32*)(p+0x10));
	c = *(u8*)(p+4);
	say("count:%x", c);

	p += 0x24;
	c -= 0x24;
	for(j=0;j<c;j+=4)parsetable(p+j);
}
void parseacpi(void* p)
{
	say("RSD PTR @%x",p);
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
