#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long
#define journalhome 0x100000	//1m
//
void diary(void*, ...);
void say(void*, ...);




void basiccreate()
{
	int j;
	u64* p=(u64*)(journalhome);
	for(j=0;j<0x20000;j++)p[j]=0;
	diary("spaceship landing...3...2...1...");
}
