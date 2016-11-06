#define u64 unsigned long long
#define u32 unsigned int
#define u16 unsigned short
#define u8 unsigned char
static u8* screeninfo=(void*)0x2000;
static u8* addr;
static int bpp;
//
static u8* img;




void writescreen()
{
	int j;
	for(j=0;j<1024*768;j++)
	{
		*(u32*)(addr + (j*bpp)) = *(u32*)(img + (j<<2));
	}
}




void initscreen(void* p)
{
	u64 haha = *(u32*)(screeninfo+0x28);
	addr = (void*)haha;

	bpp = *(u8*)(screeninfo+0x19);
	bpp >>= 3;

	img = p;
}