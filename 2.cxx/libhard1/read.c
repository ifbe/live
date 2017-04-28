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
void diary(char*,...);




int read(u64 fd, u64 buf, u64 from, u64 count)
{
	//暂时管不了多硬盘,所以指定用寻找到的第一个
	u64 type=*(u64*)diskhome;
	if(type == 0x61746173)
	{
		u64 sata=*(u64*)(diskhome+8);
		u64 j=0;
		while(count>0x80)
		{
			//0x80sectors=0x10000=64KB
			ahciread(sata, buf+j*0x10000, from+j*0x80, 0x80);

			j++;
			count-=0x80;
		}
		ahciread(sata, buf+j*0x10000, from+j*0x80, count);
	}
}
