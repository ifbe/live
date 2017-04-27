#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long
u8 in8(u16 port);
void diary(char*,...);




u8 ps2kbd()
{
	u32 key;
	while(1)
	{
		key = in8(0x64);
		if( (key&1) == 0)continue;

		key = in8(0x60);
		return key;
	}
	return 0;
}
