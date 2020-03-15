#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long




int readcli();
int readwnd();




int poller()
{
	while(1)
	{
		readwnd();
		readcli();
	}
	return 0;
}
