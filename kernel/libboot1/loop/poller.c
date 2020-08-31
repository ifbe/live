#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long




int network_read();
//
int stdio_read();
int window_read();




int poller()
{
	while(1)
	{
		network_read();

		window_read();
		stdio_read();
	}
	return 0;
}
