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
	int j,k;
	u32* p;
	u8* aa;

	if(win.fmt == 0xb8000)
	{
		aa = (void*)0xb8000;
		for(j=0;j<80;j++)
		{
			k = (j%16)+0x30;
			if(k > 0x39)k+=7;
			aa[j*2+0] = k;
			aa[j*2+1] = (j%16);
		}
		return;
	}

	if(win.fmt == 0xa0000)
	{
		aa = (void*)0xa0000;
		for(j=0;j<320*200;j++)aa[j] = img[j];
		return;
	}

	for(j=0;j<1024*768;j++)
	{
		p = (void*)( (win.buf) + (j*win.fmt) );
		*p = *(u32*)(img + (j<<2));
	}
}




void initscreen()
{
	img = (void*)0x2000000;

	win.buf = *(u32*)screeninfo;
	win.fmt = *(u32*)(screeninfo+8);
}
