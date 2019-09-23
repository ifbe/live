#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long




void start()
{
	__asm__("int $0x3");
}
