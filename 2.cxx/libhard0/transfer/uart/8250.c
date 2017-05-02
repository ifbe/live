#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long
#define PORT 0x3f8
void out8(u32 port, u8 data);
u8 in8(u32 port);




char readuart()
{
	while((in8(PORT + 5) & 1) == 0);
	return in8(PORT);
}
void writeuart_one(u8 data)
{
	while((in8(PORT + 5) & 0x20) == 0);
	out8(PORT, data);
}
void writeuart(u8* buf, int len)
{
	int j;
	for(j=0;j<len;j++)writeuart_one(buf[j]);
}
void inituart()
{
	out8(PORT + 1, 0x00);//Disable all interrupts
	out8(PORT + 3, 0x80);//Enable DLAB (set baud rate divisor)
	out8(PORT + 0, 0x01);//1=115200, 3=38400
	out8(PORT + 1, 0x00);//(high byte)
	out8(PORT + 3, 0x03);//8 bits, no parity, one stop bit
	out8(PORT + 2, 0xC7);//Enable FIFO, clear them, with 14-byte threshold
	out8(PORT + 4, 0x0B);//IRQs enabled, RTS/DSR set

	writeuart("fuckyou\n", 8);
}
