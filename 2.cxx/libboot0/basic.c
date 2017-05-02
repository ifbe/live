#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long
int waitkbd(void*);
int waituart(void*);
//
void initprint(void*);
void diary(void*, ...);
void say(void*, ...);
//
static char* inputqueue;	//stdin
static char* outputqueue;	//stdout
static char* journalqueue;	//stderr
static char* eventqueue;	//stdevent




void basiccreate(void* type, void* addr)
{
	int j;
	u64* p;
	inputqueue = addr;
	outputqueue = addr+0x100000;
	journalqueue = addr+0x200000;
	eventqueue = addr+0x300000;

	initprint(addr);
	diary("spaceship landing...3...2...1...");
}
void* waitevent()
{
	int ret;
	ret = waitkbd(eventqueue);
	if(ret > 0)return eventqueue;

	ret = waituart(eventqueue);
	if(ret > 0)return eventqueue;

	return 0;
}
