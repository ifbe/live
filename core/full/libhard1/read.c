#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long

#define idehome 0x400000
#define ahcihome 0x400000
#define xhcihome 0x600000
#define usbhome 0x700000

#define diskhome 0x800000
#define fshome 0x900000
#define dirhome 0xa00000
#define datahome 0xb00000
int ahciread(u64, u64, u64, u64);
void say(char*,...);




int read(u64 fd, u64 buf, u64 off, u64 len)
{
	u64 j=0;
	off = off>>9;
	len = (len+0x1ff)>>9;
	while(len>0x80)
	{
		//0x80sectors=0x10000=64KB
		ahciread(0, buf+j*0x10000, off+j*0x80, 0x80);

		j++;
		len -= 0x80;
	}
	ahciread(0, buf+j*0x10000, off+j*0x80, len);
	return len;
}
