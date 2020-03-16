#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long
void initscreen();
void say(void*, ...);




void supplycreate()
{
	say("supply?\n");
	initscreen();
	say("supply!\n");
}
