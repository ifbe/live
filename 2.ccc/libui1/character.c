#define u64 unsigned long long
#define u32 unsigned int
#define u16 unsigned short
#define u8 unsigned char
void printstring(int x, int y, int size, u8* ch, u32 fgcolor, u32 bgcolor);




//
static u8* logaddr;
static u32* window;
//
static u8 buffer[128];
static int count=0;




void characterread()
{
	int x,y;
	u64 temp = *(u64*)(logaddr + 0xffff8);
	if(temp < 0x40*0x30)temp = 0;
	else temp = temp-0x40*0x30;

	//
	for(y=0;y<768;y++)
	{
		for(x=0;x<256;x++)
		{
			window[(y<<10)+x] = 0;
		}
	}

	//
	for(y=0;y<752/16;y++)
	{
		printstring(0, y*16, 1, logaddr+temp+y*0x40, 0xffffff, 0xff000000);
	}

	//
	printstring(0, 752, 1, buffer, 0xffffff, 0xff000000);
}
void characterwrite(u64 key, u64 type)
{
	if(key == 0x7f)
	{
		buffer[count] = 0x20;
		if(count>0)count--;
	}
	else
	{
		if(count > 127)return;

		buffer[count] = (u8)key;
		count ++;
	}
}




void initcharacter(void* addr)
{
	logaddr = (void*)0x100000;
	window = addr;
}
