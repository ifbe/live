#define u64 unsigned long long
#define u32 unsigned int
#define u16 unsigned short
#define u8 unsigned char
//
void printstring(int x, int y, int size, u8* ch, u32 fgcolor, u32 bgcolor);
//
int ncmp(void*, void*, int);
int cmp(void*, void*);
//
void identify();
void ahcilist();
//
void diary(char*,...);




//
static u32* window;
//
static u8* logaddr;
static int offset=0;
//
static u8 buffer[128];
static int count=0;




void characterread_image()
{
	int x,y;
	int temp = *(int*)(logaddr + 0xffff8);
	temp -= offset;
	if(temp < 0)temp = 0;

	if(temp < 0x80*0x2f)temp = 0;
	else temp = temp-0x80*0x2f;

	//
	for(y=0;y<768;y++)
	{
		for(x=0;x<1024;x++)
		{
			window[(y<<10)+x] = 0;
		}
	}

	//
	for(y=0;y<752/16;y++)
	{
		printstring(0, y*16, 1, logaddr+temp+y*0x80, 0xffffff, 0xff000000);
	}

	//
	printstring(0, 752, 1, buffer, 0xffffff, 0xff000000);
}
void characterread_b8000()
{
	int x,y;
	int c,t;
	u8* p;
	u8* q;
	int temp = *(int*)(logaddr + 0xffff8);
	temp -= offset;
	if(temp < 0)temp = 0;

	if(temp < 0x80*24)temp = 0;
	else temp = temp-0x80*24;

	q = logaddr+temp;
	p = (u8*)0xb8000;
	for(y=0;y<25;y++)
	{
		for(x=0;x<80;x++)
		{
			c = q[y*0x80 + x];
			if(c < 0x20)c = 0x20;

			t = (y*80+x)*2;
			p[t+0] = c;
			p[t+1] = 0x70;
		}
	}
}
void characterread()
{
	if(*(u32*)0x2000 == 0xb8000)characterread_b8000();
	else characterread_image();
}
void characterwrite(u64 key, u64 type)
{
	if(type == 0x64626b)
	{
		if(key == 0x1b)		//esc
		{
			offset = 0;
		}
		else if(key == 0x47)	//home
		{
			offset = 0x100000;
		}
		else if(key == 0x4f)	//end
		{
			offset = 0;
		}
		else if(key == 0x49)	//page up
		{
			offset += 0x800;
		}
		else if(key == 0x51)	//page down
		{
			if(offset>0x800)offset -= 0x800;
		}

		return;
	}
	if(type == 0x72616863)
	{
		if(key == 0x7f)
		{
			if(count>0)count--;
			buffer[count] = 0;
		}
		else if(key == 0xd)
		{
			diary(buffer);
			if(ncmp(buffer,"ahcilist",8) == 0)ahcilist();
			else if(ncmp(buffer,"identify",8) == 0)identify();

			for(count=127;count>=0;count--)buffer[count] = 0;
			count = 0;
		}
		else
		{
			if(count > 127)return;

			buffer[count] = (u8)key;
			count ++;
		}
	}
}




void initcharacter(void* addr)
{
	logaddr = (void*)0x100000;
	window = addr;
}
