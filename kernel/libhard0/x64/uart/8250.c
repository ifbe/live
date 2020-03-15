#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long
#define PORT 0x3f8
void out8(u32 port, u8 data);
u8 in8(u32 port);




static u8 state = 0;
void inituart()
{
	out8(PORT + 1, 0x00);//Disable all interrupts
	out8(PORT + 3, 0x80);//Enable DLAB (set baud rate divisor)
	out8(PORT + 0, 0x01);//1=115200, 3=38400
	out8(PORT + 1, 0x00);//(high byte)
	out8(PORT + 3, 0x03);//8 bits, no parity, one stop bit
	out8(PORT + 2, 0xC7);//Enable FIFO, clear them, with 14-byte threshold
	out8(PORT + 4, 0x0B);//IRQs enabled, RTS/DSR set

	state = 1;
	writeuart((u8*)"fuckyou\n", 8);
}




int readuart_one(u8* buf)
{
	if(0 == (in8(PORT+5) & 1))return 0xffff;

	return in8(PORT);
}
int readuart(u8* buf, int len)
{
	int j,ret;
	for(j=0;j<len;j++)
	{
		ret = readuart_one(buf+j);
		if(ret > 0xff)break;

		buf[j] = ret;
	}
	return j;
}




int writeuart_one(u8 data)
{
	int j=0;
	while((in8(PORT + 5) & 0x20) == 0)
	{
		j++;
		if(j>0xffffff)return 0;
	}
	out8(PORT, data);
	return 1;
}
int writeuart(u8* buf, int len)
{
	int j;
	if(state == 0)return 0;
	for(j=0;j<len;j++)writeuart_one(buf[j]);
	return j;
}
