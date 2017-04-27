#define u64 unsigned long long
#define u32 unsigned int
#define u16 unsigned short
#define u8 unsigned char
#define screeninfo 0x2000
struct window
{
	u64 buf;
	u64 fmt;
	u64 w;
	u64 h;
};
static struct window win;
static u8* img;




void writescreen()
{
	int j;
	u32* p;
	u8* aa;
	u8* bb;
	if(win.fmt == 0)
	{
		aa = (void*)0xb8000;
		bb = (void*)win.buf;
		for(j=0;j<80*25*2;j++)aa[j] = bb[j];
		return;
	}

	for(j=0;j<1024*768;j++)
	{
		p = (void*)( (win.buf) + (j*win.fmt) );
		*p = *(u32*)(img + (j<<2));
	}
}




void initscreen(void* p)
{
	win.buf = *(u32*)screeninfo;
	win.fmt = *(u8*)(screeninfo+8);

	img = p;
}
