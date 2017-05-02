#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long
#define journalhome 0x1200000
#define journalsize 0x100000
void printmemory(u8* buf, int len)
{
	u64 temp=*(u64*)(journalhome+journalsize-8);
	if(temp >= journalsize-0x80)
	{
		temp = 0;
		*(u64*)(journalhome+journalsize-8) = 0;
	}
	u8* dst = (void*)(journalhome+temp);

	u8 c;
	int j,k = 0;
	for(j=0;j<len;j++)
	{
		c = buf[j]>>4;
		c += 0x30;
		if(c > 0x39)c += 0x27;
		dst[k+0] = c;

		c = buf[j]&0xf;
		c += 0x30;
		if(c > 0x39)c += 0x27;
		dst[k+1] = c;

		c = buf[j];
		if(c<0x20|c>0x7e)c=0x20;
		dst[k+0x40] = 0x20;
		dst[k+0x41] = c;
		k += 2;

		if((j>0)&&((j&0x1f) == 0x1f))k += 0x40;
	}

	*(u64*)(journalhome+journalsize-8) += k&0xffffff80;
}




void debugcreate()
{
}
