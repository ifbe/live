#define u64 unsigned long long
#define u32 unsigned int
#define u16 unsigned short
#define u8 unsigned char
void term_readbywnd();
void say(void*, ...);




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
void windowcreate()
{
	u64 tmp = *(u32*)screeninfo;
	if(0 == tmp)return;

	win.buf = (void*)tmp;
	win.fmt = *(u32*)(screeninfo+8);
	say("win: %llx,%x,%d,%d\n", win.buf, win.fmt, win.w, win.h);

	img.buf = (void*)0x2000000;
	//img.fmt = rgba;
	say("img: %llx,%x,%d,%d\n", img.buf, img.fmt, img.w, img.h);

	enable = 1;
}
void initscreen()
{
}




void writescreen()
{
	int j,k;
	u32* p;
	u8* aa;

	if(0xb8000 == win.fmt)
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

	if(0xa0000 == win.fmt)
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
void window_read()
{
	if(0 == enable)return;
	if(0 == win.buf)return;
	if(0 == img.buf)return;

	term_readbywnd();
	writescreen();
}
void window_write()
{
}
