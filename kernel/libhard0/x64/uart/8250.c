#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long
#define BIOSDATA 0x400
#define UARTPORT 0x3f8
void out8(u32 port, u8 data);
u8 in8(u32 port);




static int enable = 0;




int readuart_one(u8* buf)
{
	if(UARTPORT != enable)return 0xffff;

	if(0 == (in8(UARTPORT+5) & 1))return 0xffff;
	return in8(UARTPORT);
}
int readuart(u8* buf, int len)
{
	int j,ret;
	if(UARTPORT != enable)return 0;

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
	if(UARTPORT != enable)return 0xffff;

	while((in8(UARTPORT + 5) & 0x20) == 0)
	{
		j++;
		if(j>0xffffff)return 0;
	}
	out8(UARTPORT, data);
	return 1;
}
int writeuart(u8* buf, int len)
{
	int j;
	if(UARTPORT != enable)return 0;

	for(j=0;j<len;j++)writeuart_one(buf[j]);
	return j;
}




void inituart()
{
	if(UARTPORT != *(u16*)(BIOSDATA+0))return;

	out8(UARTPORT + 1, 0x00);//Disable all interrupts
	out8(UARTPORT + 3, 0x80);//Enable DLAB (set baud rate divisor)
	out8(UARTPORT + 0, 0x01);//1=115200, 3=38400
	out8(UARTPORT + 1, 0x00);//(high byte)
	out8(UARTPORT + 3, 0x03);//8 bits, no parity, one stop bit
	out8(UARTPORT + 2, 0xC7);//Enable FIFO, clear them, with 14-byte threshold
	out8(UARTPORT + 4, 0x0B);//IRQs enabled, RTS/DSR set

	writeuart((u8*)"fuckyou\n", 8);
	enable = UARTPORT;
}
