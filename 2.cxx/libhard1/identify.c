#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long
int ahciidentify(void* rdi);
void printmemory(void*, int);
void say(void*, ...);




void identify()
{
	int j;
	u8 c;
	u8* rdi = (void*)0x100000;
	ahciidentify(rdi);

	for(j=0;j<0x200;j+=2)
	{
		c = rdi[j];
		rdi[j] = rdi[j+1];
		rdi[j+1] = c;
	}
	printmemory(rdi,0x200);
}
