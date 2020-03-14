#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long
#define hex16(a,b) (a | (b<<8))
#define hex32(a,b,c,d) (a | (b<<8) | (c<<16) | (d<<24))
#define hex64(a,b,c,d,e,f,g,h) (hex32(a,b,c,d) | (((u64)hex32(e,f,g,h))<<32))
int pt_read(u64 fd, u64 off, u8* buf, int len);
int fat_read(u64 fd, u64 off, u8* buf, int len);
void say(char* , ...);




int fs_check(u8* addr)
{
	pt_read(0, 0, (void*)0x100000, 0x1000);
	if(fat_check(addr)){
		fat_parse(addr);
		return 0;
	}
	say("?\n");
	return 0;
}
int fs_read(u64 fd, u64 off, u8* buf, int len)
{
	return fat_read(fd, off, buf, len);
}
