#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long
u8 in8(u16 port);
void diary(char*,...);




int readkbd_one(u8* buf)
{
	u8 flag = in8(0x64);
	if( (flag&1) == 0)return 0;

	buf[0] = in8(0x60);
	return 1;
}
int readkbd(u8* buf, int len)
{
	int j,ret;
	for(j=0;j<len;j++)
	{
		ret = readkbd_one(buf+j);
		if(ret == 0)break;
	}
	return j;
}
