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
void read(u64,u64,u64,u64);
void identify();
//
void ahci_list();
void ahci_choose();
void out8(u32, u8);
//
void printmemory(void*, int);
void say(char*,...);




//
static u32* window;
//
static u8* input;
static u8* output;
static u8* journal;
//
static int count=0;
static int offset=0;




void characterread_image()
{
	int x,y;
	int temp = *(int*)(journal + 0xffff8);
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
		printstring(0, y*16, 1, journal+temp+y*0x80, 0xffffff, 0xff000000);
	}

	//
	printstring(0, 752, 1, input, 0xffffff, 0xff000000);
}
void characterread_a0000()
{
	int x,y;
	u8 color;
	u8* p = (void*)window;

	for(y=0;y<200;y++)
	{
		for(x=0;x<320;x++)
		{
			color = ((y*16/200)<<4) + (x*16/320);
			p[320*y + x] = color;
		}
	}
}
void characterread_b8000()
{
	u8* p;
	u8* q;
	int x,y;
	int c,t;
	u32 temp = *(u32*)(journal + 0xffff8);
	if(temp < offset)temp = 0;
	else
	{
		temp -= offset;
		if(temp < 0x80*24)temp = 0;
		else temp = temp-0x80*24;
	}

	p = (void*)window;
	q = journal+temp;
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

	for(x=0;x<80;x++)
	{
		p[2*(80*24+x) + 0] = input[x];
		p[2*(80*24+x) + 1] = 0x70;
	}
}
void characterread()
{
	u32 type = *(u32*)0x2008;
	if(type == 0xb8000)characterread_b8000();
	else if(type == 0xa0000)characterread_a0000();
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
			input[count] = 0;
		}
		else if(key == 0xd)
		{
			say(input);
			if(ncmp(input,"reboot",6) == 0)out8(0x64,0xfe);
			else if(ncmp(input,"print",5) == 0)printmemory((void*)0x100000,0x200);
			else if(ncmp(input,"ahci.ls",7) == 0)ahci_list();
			else if(ncmp(input,"ahci.cd",7) == 0)ahci_choose();
			else if(ncmp(input,"disk.id",7) == 0)identify();
			else if(ncmp(input,"disk.read",9) == 0)read(0, 0x100000, 0, 1);

			for(count=127;count>=0;count--)input[count] = 0;
			count = 0;
		}
		else
		{
			if(count > 127)return;

			input[count] = (u8)key;
			count ++;
		}
	}
}




void charactercreate()
{
	count = offset = 0;
	input = (void*)0x1000000;
	output = (void*)0x1100000;
	journal = (void*)0x1200000;

	window = (void*)0x2000000;
}
