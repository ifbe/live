#define u64 unsigned long long
#define u32 unsigned int
#define u16 unsigned short
#define u8 unsigned char
void printstring(int x, int y, int size, u8* ch, u32 fgcolor, u32 bgcolor);




//
static u8* logaddr;
static u32* window;




void showprint()
{
	int x,y;
	for(y=0;y<768/16;y++)
	{
		printstring(0, y*16, 1, logaddr+y*0x40+x, 0xffffff, 0);
	}
}




void initcharacter(void* addr)
{
	logaddr = (void*)0xe00000;
	window = addr;
}