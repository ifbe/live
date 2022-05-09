#define u64 unsigned long long
#define u32 unsigned int
#define u16 unsigned short
#define u8 unsigned char
#define powerport (*(u16*)0xffc)
#define powerdata ((*(u16*)0xffe)|0x2000)
void out8(u32, u8);
void out16(u32, u16);
void say(void*, ...);




void reboot()
{
	out8(0x64,0xfe);
}
void poweroff()
{
	u16 port = powerport;
	u16 data = powerdata;
	say("port=%x,data=%x\n",port,data);
	out16(port, data);
}
