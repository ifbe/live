#define u64 unsigned long long
#define u32 unsigned int
#define u16 unsigned short
#define u8 unsigned char
void termread_wnd();




struct window
{
	u8* buf;
	u32 fmt;
	u32 w;
	u32 h;
};
static struct window win;
static struct window img;
static int enable = 0;




#define screeninfo 0x2000
void initscreen()
{
	u64 tmp;
	if(0 == enable)return;

	tmp = *(u32*)screeninfo;
	win.buf = (void*)tmp;
	win.fmt = *(u32*)(screeninfo+8);

	img.buf = (void*)0x2000000;
	//img.fmt = rgba;
}
void enablescreen()
{
	enable = 1;
}




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
		for(j=0;j<320*200;j++)aa[j] = img.buf[j];
		return;
	}

	for(j=0;j<1024*768;j++)
	{
		p = (void*)( (win.buf) + (j*win.fmt) );
		*p = *(u32*)(img.buf + (j<<2));
	}
}
void readwnd()
{
	if(0 == enable)return;
	if(0 == win.buf)return;
	if(0 == img.buf)return;

	termread_wnd();
	writescreen();
}
