#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long
//
static char* inputqueue;	//stdin
static char* outputqueue;	//stdout
static char* journalqueue;	//stderr
static char* eventqueue;	//stdevent




void basiccreate(void* addr)
{
	int j;
	inputqueue = addr;
	outputqueue = addr+0x100000;
	journalqueue = addr+0x200000;
	eventqueue = addr+0x300000;
	for(j=0;j<0x400000;j++)inputqueue[j] = 0;
}
void basicdelete()
{
}
