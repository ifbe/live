#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long
int ahciread(u64 dev, u64 off, u64 buf, u64 len);
int nvmeread(u64 dev, u64 off, u64 buf, u64 len);
void say(char*,...);




//read the chosen device, byte to sector
int diskread(u64 fd, u64 off, u64 buf, u64 len)
{
/*
	u64 j=0;
	off = off>>9;
	len = (len+0x1ff)>>9;
	while(len>0x80)
	{
		//0x80sectors=0x10000=64KB
		ahciread(0, off+j*0x80, buf+j*0x10000, 0x80);

		j++;
		len -= 0x80;
	}
	ahciread(0, off+j*0x80, buf+j*0x10000, len);
*/
	u64 j = 0;
	u64 sec = off >> 9;
	u64 cnt = (len+0x1ff)>>9;
	while(j+0x80 < cnt){
		ahciread(0, sec+j, buf+j*0x200, 0x80);
		j += 0x80;
	}
	ahciread(0, sec+j, buf+j*0x200, cnt-j);
	return len;
}
